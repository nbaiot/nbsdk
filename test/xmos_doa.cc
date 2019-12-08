//
// Created by genglei-cuan on 2019/8/21.
//

#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fftw3.h>
#include <math.h>

#include "xmos_doa.h"

namespace xmos {

#define PI 3.1416

typedef struct PCM PCM;
typedef struct Matrix Matrix;
typedef struct X X;
typedef struct conj_X conj_X;
typedef struct max_index max_index;
typedef struct doa_res doa_res;


int spline(int n, int end1, int end2,
           double slope1, double slope2,
           double x[], double y[],
           double b[], double c[], double d[],
           int *iflag) {
  int nm1, ib, i, ascend;
  double t;
  nm1 = n - 1;
  *iflag = 0;
  if (n < 2) {  /* no possible interpolation */
    *iflag = 1;
    goto LeaveSpline;
  }
  ascend = 1;
  for (i = 1; i < n; ++i) if (x[i] <= x[i - 1]) ascend = 0;
  if (!ascend) {
    *iflag = 2;
    goto LeaveSpline;
  }
  if (n >= 3) {
    d[0] = x[1] - x[0];
    c[1] = (y[1] - y[0]) / d[0];
    for (i = 1; i < nm1; ++i) {
      d[i] = x[i + 1] - x[i];
      b[i] = 2.0 * (d[i - 1] + d[i]);
      c[i + 1] = (y[i + 1] - y[i]) / d[i];
      c[i] = c[i + 1] - c[i];
    }
    /* ---- Default End conditions */
    b[0] = -d[0];
    b[nm1] = -d[n - 2];
    c[0] = 0.0;
    c[nm1] = 0.0;
    if (n != 3) {
      c[0] = c[2] / (x[3] - x[1]) - c[1] / (x[2] - x[0]);
      c[nm1] = c[n - 2] / (x[nm1] - x[n - 3]) - c[n - 3] / (x[n - 2] - x[n - 4]);
      c[0] = c[0] * d[0] * d[0] / (x[3] - x[0]);
      c[nm1] = -c[nm1] * d[n - 2] * d[n - 2] / (x[nm1] - x[n - 4]);
    }
    /* Alternative end conditions -- known slopes */
    if (end1 == 1) {
      b[0] = 2.0 * (x[1] - x[0]);
      c[0] = (y[1] - y[0]) / (x[1] - x[0]) - slope1;
    }
    if (end2 == 1) {
      b[nm1] = 2.0 * (x[nm1] - x[n - 2]);
      c[nm1] = slope2 - (y[nm1] - y[n - 2]) / (x[nm1] - x[n - 2]);
    }
    /* Forward elimination */
    for (i = 1; i < n; ++i) {
      t = d[i - 1] / b[i - 1];
      b[i] = b[i] - t * d[i - 1];
      c[i] = c[i] - t * c[i - 1];
    }
    /* Back substitution */
    c[nm1] = c[nm1] / b[nm1];
    for (ib = 0; ib < nm1; ++ib) {
      i = n - ib - 2;
      c[i] = (c[i] - d[i] * c[i + 1]) / b[i];
    }
    b[nm1] = (y[nm1] - y[n - 2]) / d[n - 2] + d[n - 2] * (c[n - 2] + 2.0 * c[nm1]);
    for (i = 0; i < nm1; ++i) {
      b[i] = (y[i + 1] - y[i]) / d[i] - d[i] * (c[i + 1] + 2.0 * c[i]);
      d[i] = (c[i + 1] - c[i]) / d[i];
      c[i] = 3.0 * c[i];
    }
    c[nm1] = 3.0 * c[nm1];
    d[nm1] = d[n - 2];
  } else {
    b[0] = (y[1] - y[0]) / (x[1] - x[0]);
    c[0] = 0.0;
    d[0] = 0.0;
    b[1] = b[0];
    c[1] = 0.0;
    d[1] = 0.0;
  }
  LeaveSpline:
  return 0;
}

double seval(int n, double u,
             double x[], double y[],
             double b[], double c[], double d[],
             int *last) {
  int i, j, k;
  double w;
  i = *last;
  if (i >= n - 1) i = 0;
  if (i < 0) i = 0;
  if ((x[i] > u) || (x[i + 1] < u)) {
    i = 0;
    j = n;
    do {
      k = (i + j) / 2;
      if (u < x[k]) j = k;
      if (u >= x[k]) i = k;
    } while (j > i + 1);
  }
  *last = i;
  w = u - x[i];
  w = y[i] + w * (b[i] + w * (c[i] + w * d[i]));
  return (w);
}

void SPL(int n, double *x, double *y, int ni, double *xi, double *yi) {
  double *b, *c, *d;
  int iflag, last, i;
  b = (double *) malloc(sizeof(double) * n);
  c = (double *) malloc(sizeof(double) * n);
  d = (double *) malloc(sizeof(double) * n);
  if (!d) {
    printf("no enough memory for b,c,d\n");
  } else {
    spline(n, 0, 0, 0, 0, x, y, b, c, d, &iflag);
    // if (iflag == 0)  printf("I got coef b,c,d now\n"); else printf("x not in order or other error\n");
    for (i = 0; i < ni; i++) {
      yi[i] = seval(ni, xi[i], x, y, b, c, d, &last);
    }

    free(b);
    free(c);
    free(d);
  }
}


// 读取文件后，文件内容和文件长度
struct PCM {
  int *data;
  int length;
};

// 把两个麦克的数据组成矩阵
struct Matrix {
  int *m[2];
  int length;
};
// fft变换后，二维的频率数据
struct X {
  fftw_complex *x[2];
};
// X数据的复数共轭
struct conj_X {
  fftw_complex *x[2];
};
// 求取最大值和最大值的索引
struct max_index {
  double data;
  int index;
};
// 最终角度的数据和长度
struct doa_res {
  double *data;
  int n_blocks;
};

PCM readPCMFromArray(const char *src, int len) {

  const int MAX_END = 2;
  const int MAX_START = 0;


  int file_len = len;
  unsigned char *origin_data = (unsigned char *) malloc(file_len + 1);
  int *data = (int *) calloc(file_len / 2, sizeof(int));


  memcpy(origin_data, src, len);

  origin_data[file_len] = '\0'; // 空字符，表示字符串的结束
  int count = 0;
  for (int i = 0; i < file_len - 1; i += 2) {
    data[count++] = (int) ((origin_data[i + 1] << 8) | origin_data[i]);
  }


  free(origin_data);
  origin_data = NULL;
  struct PCM cur_pcm = {data, file_len / 2};
  return cur_pcm;
}

// 合成二维矩阵
Matrix getMatrixFromArray(const char *d1, const char *d2, int len) {
  PCM data1 = readPCMFromArray(d1, len);
  PCM data2 = readPCMFromArray(d2, len);


  // 如果两个pcm文件长度不同，我们以短的作为标准
  if (data1.length > data2.length) {
    data1.length = data2.length;
  } else if (data1.length < data2.length) {
    data2.length = data1.length;
  }

  Matrix cur_m;
  for (int i = 0; i < 2; i++) {
    cur_m.m[i] = (int *) calloc(data1.length, sizeof(int));
  }

  memcpy(cur_m.m[0], data1.data, data1.length * sizeof(int));
  memcpy(cur_m.m[1], data2.data, data1.length * sizeof(int));
  cur_m.length = data1.length;

  free(data1.data);
  free(data2.data);
  data1.data = NULL;
  data2.data = NULL;
  return cur_m;

}

// 把src中的元素拷贝到des中，同时把数据从int转化为double
void cpyInt2Double(double *des, int *src, int start, int end) {
  int i = 0;
  int count = 0;
  for (i = start; i < end; i++) {
    des[count++] = (double) src[i];
  }
  return;
}

// 快速傅里叶变换
X fft(int start_index, int end_index, Matrix matrix, int N_L) {

  fftw_plan p_left, p_right, p1_left, p1_right;

  /**
     这里本来准备使用二维方法，不过二维方法反变换一直存在问题，所以暂且使用一维
     left和right其实表示矩阵的两列，即两个麦克风接收到的数据，之所以这么表示因为
     我们使用一维方式进行转换，所以需要挨个转换然后拼接，说白了就是把矩阵分开计算
  */
  fftw_complex *out_left;
  fftw_complex *out_right;
  struct X fft_X;

  double *in_left;
  double *in_right;

  double *iout_left;
  double *iout_right;


  in_left = (double *) fftw_malloc(N_L * sizeof(double));
  in_right = (double *) fftw_malloc(N_L * sizeof(double));

  out_left = (fftw_complex *) fftw_malloc((N_L / 2 + 1) * sizeof(fftw_complex));
  out_right = (fftw_complex *) fftw_malloc((N_L / 2 + 1) * sizeof(fftw_complex));

  cpyInt2Double(in_left, matrix.m[0], start_index, end_index);
  cpyInt2Double(in_right, matrix.m[1], start_index, end_index);

  p_left = fftw_plan_dft_r2c_1d(N_L, in_left, out_left, FFTW_ESTIMATE);
  p_right = fftw_plan_dft_r2c_1d(N_L, in_right, out_right, FFTW_ESTIMATE);
  fftw_execute(p_left);
  fftw_execute(p_right);


  fft_X.x[0] = out_left;
  fft_X.x[1] = out_right;


  fftw_destroy_plan(p_left);
  fftw_destroy_plan(p_right);


  fftw_free(in_left);
  fftw_free(in_right);
  return fft_X;
}

// 求一个复数的共轭
void conj(fftw_complex old) {
  old[1] = 0 - old[1];
}

// 获取最大值和最大值索引
max_index getMax_index(double *yy, int num) {
  max_index m;
  m.data = yy[0];
  m.index = 0;
  for (int j = 0; j < num - 1; j++) {
    for (int jj = j + 1; jj < num; jj++) {
      if (yy[jj] >= m.data) {
        m.data = yy[jj];
        m.index = jj;
      }
    }
    break;
  }
  return m;
}


// 波达方向估计
doa_res DOA(int fs, Matrix matrix) {
  int Mic_Num = 2;
  double Marray[2][2] = {0.0457, 0.00, -0.0457, 0.00};
  int N_L = 1024;
  int N = 256;
  int Snap = N_L / N;
  int n_blocks = floor(matrix.length / N_L);
  int c = 340;

  // 最终角度集合
  double *angle_gcc = (double *) calloc(n_blocks, sizeof(double));
  fftw_plan p;

  for (int i = 0; i < n_blocks; i++) {
    int start_index = i * N_L; // C语言中 索引从0开始
    int end_index = (i + 1) * N_L;

    X x = fft(start_index, end_index, matrix, N_L);

    //TODO优化
    double(*delay)[2] = (double (*)[2]) calloc((Mic_Num - 1) * Mic_Num, sizeof(double));

    for (int chm = 0; chm < Mic_Num - 1; chm++)
      for (int chn = chm + 1; chn < Mic_Num; chn++) {

        /// fft -> spectrum
        fftw_plan p;
        fftw_complex *Rmn;
        double *Rt;
        Rmn = (fftw_complex *) fftw_malloc((N_L / 2 + 1) * sizeof(fftw_complex));
        Rt = (double *) fftw_malloc(N_L * sizeof(fftw_complex));
        for (int col = 0; col < (N_L / 2 + 1); col++) {
          double a = (x.x[chm][col][0] * x.x[chn][col][0] + x.x[chm][col][1] * x.x[chn][col][1]);
          double b = (x.x[chm][col][1] * x.x[chn][col][0] - x.x[chm][col][0] * x.x[chn][col][1]);
          Rmn[col][0] = a;
          Rmn[col][1] = b;
        }

        p = fftw_plan_dft_c2r_1d(N_L, Rmn, Rt, FFTW_ESTIMATE);
        fftw_execute(p);

        /// normalization
        for (int j = 0; j < N_L; j++) {
          Rt[j] /= N_L;
        }

        int step = 1;
        double xx[1024] = {0.0};
        double x_old[1024] = {0.0};

        for (int j = 0; j < N_L; j += step) {
          xx[j] = (double) j;
        }
        for (int j = 0; j < N_L; j++) {
          x_old[j] = (double) j;
        }

        double yy[1024];
        SPL(N_L, xx, Rt, N_L, x_old, yy);
        max_index m = getMax_index(yy, N_L);

        if (m.index < N_L / 2) {
          delay[chm][chn] = (double) m.index;
        }
        if (m.index > N_L / 2) {
          delay[chm][chn] = (double) -(N_L - m.index);
        }

        fftw_free(Rmn);
        fftw_free(Rt);
        fftw_destroy_plan(p);
      }

    fftw_free(x.x[0]);
    fftw_free(x.x[1]);

    for (int cm = 0; cm < Mic_Num - 1; cm++)
      for (int cn = 0; cn < Mic_Num; cn++) {
        delay[cm][cn] = delay[cm][cn] / fs * c;
      }

    double(*dMArray)[2] = (double (*)[2]) calloc(Mic_Num * Mic_Num, sizeof(double));
    double *dDelay = (double *) calloc(Mic_Num * (Mic_Num / 2), sizeof(double));

    int n = 0;
    for (int chm = 0; chm < Mic_Num - 1; chm++)
      for (int chn = chm + 1; chn < Mic_Num; chn++) {
        for (int j = 0; j < 2; j++) {
          dMArray[n][j] = Marray[chm][j] - Marray[chn][j];
          dDelay[n] = -delay[chm][chn];
        }
        n += 1;
      }

    int step = 1;
    double *J1 = (double *) calloc(180 / step, sizeof(double));

    double Jmin = 1000.0;
    int k = 0;
    double phi1_f = 0.0;
    for (int phi = 1; phi <= 180; phi += step) {

      double phi1 = phi * PI / 180;
      double sc[2] = {cos(phi1), sin(phi1)};
      double new_dMArray = 0;

      for (int row = 0; row < n; row++)
        for (int col = 0; col < 2; col++) {
          new_dMArray = new_dMArray + dMArray[row][col] * sc[col];
        }

      double tmp = fabs(dDelay[0] - new_dMArray);
      J1[k] = pow(fabs(dDelay[0] - new_dMArray), 2);

      double J = J1[k];
      if (J < Jmin) {
        Jmin = J;
        phi1_f = phi1;
      }
      k = k + 1;
    }

    angle_gcc[i] = phi1_f * 180 / PI;


    free(dMArray);
    free(delay);
    free(dDelay);
    free(J1);

  }

  doa_res doa = {
      angle_gcc, n_blocks
  };

  return doa;
}

// 求取角度平均值，就是最终结果
int angle_mean(doa_res doa) {

  double sums = 0.0;
  for (int i = 0; i < doa.n_blocks; i++) {
    sums = sums + doa.data[i];
  }
  int angle = (int) (sums / doa.n_blocks + 0.5);
  return angle;
}


int XmosDoa::currentAngle(void *da1, void *da2, void *da3, void *da4,
                          int len, uint32_t fs) {
  if (len < 1024)
    return -1;

  int needLen = (len / 1024) * 1024;
  Matrix matrix02 = getMatrixFromArray((const char *) da4, (const char *) da2, needLen);
  Matrix matrix13 = getMatrixFromArray((const char *) da1, (const char *) da3, needLen);
  doa_res doa02 = DOA(fs, matrix02);
  doa_res doa13 = DOA(fs, matrix13);
  int angle02 = angle_mean(doa02);
  int angle13 = angle_mean(doa13);

  int real = 0;
  if (angle13 <= 90) {
    real = angle02;
  } else if (angle13 > 90) {
    real = 360 - angle02;
  }

  free(matrix02.m[0]);
  free(matrix02.m[1]);
  free(matrix13.m[0]);
  free(matrix13.m[1]);
  free(doa02.data);
  free(doa13.data);

  return (real + 225) % 360;
}
}

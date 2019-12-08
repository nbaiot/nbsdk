//
// Created by nbaiot@126.com on 2019/12/5.
//
#pragma once

#ifndef NBSDK_FIFO_BUFFER_H
#define NBSDK_FIFO_BUFFER_H

#include <memory>
#include <mutex>
#include <cstdio>

namespace nbsdk::memory {

class FifoBuffer {

public:
	explicit FifoBuffer(size_t length);

	size_t GetDataLen();

	bool SetCapacity(size_t size);

	size_t Read(void *buffer, size_t bytes);

	size_t Write(const void *buffer, size_t bytes);

private:
	int ReadOffsetLocked(void *buffer,
											 size_t bytes,
											 size_t offset,
											 size_t *bytes_read);

	int WriteOffsetLocked(const void *buffer,
												size_t bytes,
												size_t offset,
												size_t *bytes_written);

private:
	std::mutex mutex_;
	std::unique_ptr<char[]> buffer_;
	size_t buffer_length_;
	size_t data_length_;
	size_t read_position_;
};

}

#endif // NBSDK_FIFO_BUFFER_H

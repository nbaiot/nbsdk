
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${FFTW3_INCLUDES})

aux_source_directory(. SRCS)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(nb_math SHARED ${SRCS})

target_link_libraries(nb_math ${FFTW3_LIBRARIES} nb_base)
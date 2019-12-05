
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

aux_source_directory(. SRCS)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(nb_memory SHARED ${SRCS})
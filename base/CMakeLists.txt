
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

aux_source_directory(. SRCS)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${Boost_INCLUDE_DIRS})

add_library(nb_base SHARED ${SRCS})
# for build static library
#add_library(nb_base ${BASE})

target_link_libraries(nb_base dl ${Boost_LIBRARIES})

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

aux_source_directory(. SRCS)

aux_source_directory(./messenger MESSENGER_SRCS)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(nb_net SHARED ${SRCS} ${MESSENGER_SRCS})

target_link_libraries(nb_net nb_base ${Boost_LIBRARIES})
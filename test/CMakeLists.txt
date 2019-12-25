include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

aux_source_directory(. SRCS)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(test ${SRCS})

target_link_libraries(test nb_base nb_net nb_memory libwebrtc_audio_processing.a)
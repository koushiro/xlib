include_directories(${PROJECT_SOURCE_DIR}/target/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/target/bin)
set(DEPENDENT_LIBRARIES xlib)

set(TEST_FILES

        )

foreach(test_file ${TEST_FILES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} ${DEPENDENT_LIBRARIES})
endforeach()

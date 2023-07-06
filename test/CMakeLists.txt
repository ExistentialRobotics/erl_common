cmake_minimum_required(VERSION 3.20)

link_libraries(${PROJECT_NAME}::${PROJECT_NAME} GTest::gtest_main)

add_executable(test_yaml test_yaml.cpp)
gtest_discover_tests(test_yaml)

add_executable(test_csv test_csv.cpp)
gtest_discover_tests(test_csv)

add_executable(test_binary test_binary.cpp)
gtest_discover_tests(test_binary)

add_executable(test_eigen test_eigen.cpp)
gtest_discover_tests(test_eigen)

add_executable(test_grid_map_info test_grid_map_info.cpp)
gtest_discover_tests(test_grid_map_info)

add_executable(test_storage_order test_storage_order.cpp)
gtest_discover_tests(test_storage_order)

add_executable(test_incremental_grid_map_2d test_incremental_grid_map_2d.cpp)
gtest_discover_tests(test_incremental_grid_map_2d)

add_test(NAME test_tensor_py
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/test_tensor.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../python)
set_tests_properties(test_tensor_py PROPERTIES
        ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/../python"
        DEPENDS ${PROJECT_NAME}_py_develop)

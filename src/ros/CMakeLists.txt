if (ROS1_ACTIVATED)
    # node: transform_to_tf_node
    add_executable(transform_to_tf_node ros1/transform_to_tf_node.cpp)
    target_include_directories(transform_to_tf_node PRIVATE ${catkin_INCLUDE_DIRS})
    target_link_libraries(transform_to_tf_node PRIVATE ${PROJECT_NAME} ${catkin_LIBRARIES})

    # node: clock_node
    add_executable(clock_node ros1/clock_node.cpp)
    target_include_directories(clock_node PRIVATE ${catkin_INCLUDE_DIRS})
    target_link_libraries(clock_node PRIVATE ${PROJECT_NAME} ${catkin_LIBRARIES})
endif ()

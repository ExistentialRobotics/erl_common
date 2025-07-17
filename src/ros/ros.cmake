if (ROS1_ACTIVATED)
    message(STATUS "ROS1 activated, building ROS1 stuff")

    # node: transform_to_tf_node
    add_executable(transform_to_tf_node src/ros/ros1/transform_to_tf_node.cpp)
    erl_target_dependencies(transform_to_tf_node)
    erl_collect_targets(EXECUTABLES transform_to_tf_node)

    # node: clock_node
    add_executable(clock_node src/ros/ros1/clock_node.cpp)
    erl_target_dependencies(clock_node)
    erl_collect_targets(EXECUTABLES clock_node)
endif ()

if (ROS2_ACTIVATED)
    message(STATUS "ROS2 activated, building ROS2 stuff")

    # node: clock_node
    add_executable(clock_node src/ros/ros2/clock_node.cpp)
    erl_target_dependencies(clock_node)
    erl_collect_targets(EXECUTABLES clock_node)
endif ()

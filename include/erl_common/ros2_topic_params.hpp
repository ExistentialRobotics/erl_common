#pragma once

#ifdef ERL_ROS_VERSION_2
    #include "yaml.hpp"

    #include <rclcpp/rclcpp.hpp>

namespace erl::common::ros_params {
    struct Ros2TopicParams : Yamlable<Ros2TopicParams> {
        std::string path;
        std::string qos_preset;  // defined in /opt/ros/<distro>/include/rclcpp/rclcpp/qos.hpp
        std::string qos_history;
        int qos_depth = 10;
        std::string qos_reliability;
        std::string qos_durability;
        int32_t qos_deadline_sec = 0;
        uint32_t qos_deadline_nanosec = 0;
        int32_t qos_lifespan_sec = 0;
        uint32_t qos_lifespan_nanosec = 0;
        std::string qos_liveliness;
        int32_t qos_liveliness_lease_duration_sec = 0;
        uint32_t qos_liveliness_lease_duration_nanosec = 0;
        bool avoid_ros_namespace_conventions = false;

        ERL_REFLECT_SCHEMA(
            Ros2TopicParams,
            ERL_REFLECT_MEMBER(Ros2TopicParams, path),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_preset),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_history),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_depth),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_reliability),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_durability),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_deadline_sec),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_deadline_nanosec),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_lifespan_sec),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_lifespan_nanosec),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_liveliness),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_liveliness_lease_duration_sec),
            ERL_REFLECT_MEMBER(Ros2TopicParams, qos_liveliness_lease_duration_nanosec),
            ERL_REFLECT_MEMBER(Ros2TopicParams, avoid_ros_namespace_conventions));

        explicit Ros2TopicParams(std::string path)
            : path(std::move(path)) {}

        bool
        PostDeserialization() override;

        [[nodiscard]] const rclcpp::QoS &
        GetQoS() const {
            return m_qos_;
        }

    private:
        /*
         * Default:
         *  History: KEEP_LAST with depth 10
         *  Reliability: RELIABLE
         *  Durability: VOLATILE
         *  Liveliness: System default
         */
        rclcpp::QoS m_qos_ = rclcpp::SystemDefaultsQoS();
    };
}  // namespace erl::common::ros_params

#endif

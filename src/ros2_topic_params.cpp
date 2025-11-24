#include "erl_common/ros2_topic_params.hpp"

#ifdef ERL_ROS_VERSION_2

namespace erl::common::ros_params {
    bool
    Ros2TopicParams::PostDeserialization() {
        // setup QoS based on the parameters
        if (!qos_preset.empty()) {
            if (qos_preset == "clock") {
                m_qos_ = rclcpp::ClockQoS();
            } else if (qos_preset == "sensor_data") {
                m_qos_ = rclcpp::SensorDataQoS();
            } else if (qos_preset == "parameters") {
                m_qos_ = rclcpp::ParametersQoS();
            } else if (qos_preset == "services") {
                m_qos_ = rclcpp::ServicesQoS();
            } else if (qos_preset == "parameter_events") {
                m_qos_ = rclcpp::ParameterEventsQoS();
            } else if (qos_preset == "rosout") {
                m_qos_ = rclcpp::RosoutQoS();
            } else if (
                qos_preset == "system_defaults" || qos_preset == "default" ||
                qos_preset == "system_default") {
                m_qos_ = rclcpp::SystemDefaultsQoS();
            } else if (qos_preset == "best_available") {
    #ifdef ROS_HUMBLE
                ERL_WARN(
                    "rclcpp::BestAvailableQoS is not available in ROS Humble, using "
                    "SystemDefaultsQoS instead.");
                m_qos_ = rclcpp::SystemDefaultsQoS();
    #else
                m_qos_ = rclcpp::BestAvailableQoS();
    #endif
            } else {
                ERL_ERROR("Unknown qos_preset: {}", qos_preset);
                return false;
            }
        }

        if (!qos_history.empty()) {
            if (qos_history == "keep_last") {
                m_qos_.keep_last(static_cast<size_t>(qos_depth));
            } else if (qos_history == "keep_all") {
                m_qos_.keep_all();
            } else if (qos_history == "system_default") {
                m_qos_.history(rclcpp::HistoryPolicy::SystemDefault);
            } else {
                ERL_ERROR("Unknown qos_history: {}", qos_history);
                return false;
            }
        }

        if (!qos_reliability.empty()) {
            if (qos_reliability == "reliable") {
                m_qos_.reliable();
            } else if (qos_reliability == "best_effort") {
                m_qos_.best_effort();
            } else if (qos_reliability == "best_available") {
    #ifdef ROS_HUMBLE
                ERL_WARN(
                    "best_available is not available in ROS Humble, using reliable instead.");
                m_qos_.reliable();
    #else
                m_qos_.reliability_best_available();
    #endif
            } else if (qos_reliability == "system_default") {
                m_qos_.reliability(rclcpp::ReliabilityPolicy::SystemDefault);
            } else {
                ERL_ERROR("Unknown qos_reliability: {}", qos_reliability);
                return false;
            }
        }

        if (!qos_durability.empty()) {
            if (qos_durability == "volatile") {
                m_qos_.durability_volatile();
            } else if (qos_durability == "transient_local") {
                m_qos_.transient_local();
            } else if (qos_durability == "best_available") {
    #ifdef ROS_HUMBLE
                ERL_WARN(
                    "durability best_available is not available in ROS Humble, using volatile instead.");
                m_qos_.durability_volatile();
    #else
                m_qos_.durability_best_available();
    #endif
            } else if (qos_durability == "system_default") {
                m_qos_.durability(rclcpp::DurabilityPolicy::SystemDefault);
            } else {
                ERL_ERROR("Unknown qos_durability: {}", qos_durability);
                return false;
            }
        }

        if (qos_deadline_sec > 0 || qos_deadline_nanosec > 0) {
            rclcpp::Duration deadline(qos_deadline_sec, qos_deadline_nanosec);
            m_qos_.deadline(deadline);
        }

        if (qos_lifespan_sec > 0 || qos_lifespan_nanosec > 0) {
            rclcpp::Duration lifespan(qos_lifespan_sec, qos_lifespan_nanosec);
            m_qos_.lifespan(lifespan);
        }

        if (!qos_liveliness.empty()) {
            if (qos_liveliness == "automatic") {
                m_qos_.liveliness(rclcpp::LivelinessPolicy::Automatic);
            } else if (qos_liveliness == "manual_by_topic") {
                m_qos_.liveliness(rclcpp::LivelinessPolicy::ManualByTopic);
            } else if (qos_liveliness == "best_available") {
    #ifdef ROS_HUMBLE
                ERL_WARN(
                    "liveliness best_available is not available in ROS Humble, using automatic instead.");
                m_qos_.liveliness(rclcpp::LivelinessPolicy::Automatic);
    #else
                m_qos_.liveliness(rclcpp::LivelinessPolicy::BestAvailable);
    #endif
            } else if (qos_liveliness == "system_default") {
                m_qos_.liveliness(rclcpp::LivelinessPolicy::SystemDefault);
            } else {
                ERL_ERROR("Unknown qos_liveliness: {}", qos_liveliness);
                return false;
            }
        }

        if (qos_liveliness_lease_duration_sec > 0 || qos_liveliness_lease_duration_nanosec > 0) {
            rclcpp::Duration lease_duration(
                qos_liveliness_lease_duration_sec,
                qos_liveliness_lease_duration_nanosec);
            m_qos_.liveliness_lease_duration(lease_duration);
        }

        m_qos_.avoid_ros_namespace_conventions(avoid_ros_namespace_conventions);

        return true;
    }
}  // namespace erl::common::ros_params
#endif

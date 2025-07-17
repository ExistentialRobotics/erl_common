#include <rclcpp/rclcpp.hpp>
#include <rosgraph_msgs/msg/clock.hpp>

#include <chrono>
#include <memory>

class ClockNode : public rclcpp::Node {
public:
    ClockNode()
        : Node("clock_node") {
        // Declare parameters with default values
        this->declare_parameter("clock_rate", 100.0);
        this->declare_parameter("start_time", this->now().seconds());

        // Get parameters
        double clock_rate = this->get_parameter("clock_rate").as_double();
        double start_time = this->get_parameter("start_time").as_double();

        // Validate parameters
        if (clock_rate <= 0.0) {
            RCLCPP_ERROR(
                this->get_logger(),
                "Invalid clock_rate parameter: %f. Must be > 0.0",
                clock_rate);
            rclcpp::shutdown();
            return;
        }

        if (start_time < 0.0) {
            RCLCPP_ERROR(
                this->get_logger(),
                "Invalid start_time parameter: %f. Must be >= 0.0",
                start_time);
            rclcpp::shutdown();
            return;
        }

        // Create publisher for clock messages
        clock_pub_ = this->create_publisher<rosgraph_msgs::msg::Clock>("/clock", 10);

        // Calculate timer period
        auto timer_period = std::chrono::duration<double>(1.0 / clock_rate);
        dt_ = rclcpp::Duration::from_seconds(1.0 / clock_rate);

        RCLCPP_INFO(
            this->get_logger(),
            "Clock node started with rate: %.2f Hz, start_time: %.2f",
            clock_rate,
            start_time);

        // Initialize current time
        current_time_ = rclcpp::Time(static_cast<int64_t>(start_time * 1e9));

        // Create timer
        timer_ = this->create_wall_timer(timer_period, std::bind(&ClockNode::CallbackTimer, this));
    }

private:
    void
    CallbackTimer() {
        auto clock_msg = rosgraph_msgs::msg::Clock();
        clock_msg.clock = current_time_;
        clock_pub_->publish(clock_msg);
        current_time_ = current_time_ + dt_;
    }

    rclcpp::Publisher<rosgraph_msgs::msg::Clock>::SharedPtr clock_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Time current_time_;
    rclcpp::Duration dt_ = rclcpp::Duration::from_seconds(0.01);  // default 100Hz
};

int
main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ClockNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

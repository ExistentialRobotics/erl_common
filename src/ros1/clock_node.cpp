#include <ros/ros.h>
#include <rosgraph_msgs/Clock.h>

int
main(int argc, char** argv) {
    ros::init(argc, argv, "clock_node");
    ros::NodeHandle nh("~");

    double clock_rate = 100.0;
    nh.param("clock_rate", clock_rate, clock_rate);
    if (clock_rate <= 0.0) {
        ROS_ERROR("Invalid clock_rate parameter: %f. Must be > 0.0", clock_rate);
        return -1;
    }

    double start_time = ros::Time::now().toSec();
    nh.param("start_time", start_time, start_time);
    if (start_time < 0.0) {
        ROS_ERROR("Invalid start_time parameter: %f. Must be >= 0.0", start_time);
        return -1;
    }

    // Publisher for the clock message
    ros::Publisher clock_pub = nh.advertise<rosgraph_msgs::Clock>("/clock", 10);

    ros::Rate rate(clock_rate);
    ros::Duration dt(1.0 / clock_rate);
    ROS_INFO("Clock node started with rate: %.2f Hz, start_time: %.2f", clock_rate, start_time);

    ros::Time current_time(start_time);
    ros::Time::setNow(current_time);
    while (ros::ok()) {
        rosgraph_msgs::Clock clock_msg;
        clock_msg.clock = current_time;
        clock_pub.publish(clock_msg);
        current_time += dt;
        rate.sleep();
    }

    return 0;
}

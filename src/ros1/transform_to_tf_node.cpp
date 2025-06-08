#include <geometry_msgs/TransformStamped.h>
#include <ros/ros.h>
#include <tf2_ros/transform_broadcaster.h>

class TransformToTfNode {

    ros::NodeHandle m_nh_;
    ros::Subscriber m_sub_transform_;
    std::unique_ptr<tf2_ros::TransformBroadcaster> m_broadcaster_;
    bool m_verbose_ = true;

public:
    TransformToTfNode(ros::NodeHandle& nh)
        : m_nh_(nh) {
        // Initialize the transform broadcaster
        m_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>();
        // Subscribe to the transform topic
        std::string transform_topic = "transform";
        nh.param("transform_topic", transform_topic, transform_topic);
        m_sub_transform_ = m_nh_.subscribe(transform_topic, 1, &TransformToTfNode::Callback, this);
        ROS_INFO("TransformToTfNode initialized, listening to topic: %s", transform_topic.c_str());
    }

    void
    Callback(const geometry_msgs::TransformStamped::ConstPtr& msg) {
        // Broadcast the transform
        m_broadcaster_->sendTransform(*msg);
        if (m_verbose_) {
            ROS_INFO(
                "Broadcasting transform from %s to %s",
                msg->header.frame_id.c_str(),
                msg->child_frame_id.c_str());
            m_verbose_ = false;  // only log once
        }
    }
};

int
main(int argc, char** argv) {
    ros::init(argc, argv, "transform_to_tf_node");
    ros::NodeHandle nh("~");
    TransformToTfNode node(nh);
    ros::spin();
    return 0;
}

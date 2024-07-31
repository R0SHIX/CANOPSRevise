#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <iostream>
#include <mutex>
#include "rclcpp/rclcpp.hpp"
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include <ostream>
#include <thread>

void captureFrames(cv::VideoCapture& cap, cv:: Mat& frame, bool& running, std::mutex& frame_mutex);

class CameraPublisher: public rclcpp::Node{
    public:
        CameraPublisher():
            Node("cameraNode") {
                //Initialize the publisher and how often we send camera data.
                publisher_ = this->create_publisher<sensor_msgs::msg::Image>("camera_data", 5);
                timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&CameraPublisher::publish_image, this));
                running_ = true;
                captureThread_ = std::thread(captureFrames, std::ref(cap_), std::ref(frame_), std::ref(running_), std::ref(frame_mutex_));
            }

        ~CameraPublisher() {
            running_ = false;
            captureThread_.join();
        }

    private:
        void publish_image() {
            //Presents the camera footage
            //cv::imshow("Camera Feed", frame_);
            //cv::waitKey(1);

            auto msg = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", frame_).toImageMsg();
            publisher_->publish(*msg);
            //RCLCPP_INFO(this->get_logger(), "Publishing...");
        }

        rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
        rclcpp::TimerBase::SharedPtr timer_;
        cv::VideoCapture cap_ {0, cv::CAP_GSTREAMER};
        cv::Mat frame_;
        std::mutex frame_mutex_;
        bool running_;
        std::thread captureThread_;
};

int main(int argc, char **argv){
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<CameraPublisher>());

    rclcpp::shutdown();

    return 0;
}

void captureFrames(cv::VideoCapture& cap, cv::Mat& frame, bool& running, std::mutex& frame_mutex){
    while(running){
        std::lock_guard<std::mutex> lock(frame_mutex);
        cap >> frame;
    }
}

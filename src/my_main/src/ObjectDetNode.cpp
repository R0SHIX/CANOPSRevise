#include <functional>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include <rclcpp/executors.hpp>
#include <rclcpp/subscription.hpp>
#include <rclcpp/utilities.hpp>
#include <sensor_msgs/msg/detail/image__struct.hpp>
#include "sensor_msgs/msg/image.hpp"
#include <std_msgs/msg/bool.hpp>

class HumanDetector: public rclcpp::Node{
    public:
        HumanDetector(): Node("image_processor"){
            subscription_ = this->create_subscription<sensor_msgs::msg::Image>("camera_data", 
            5, 
            std::bind(&HumanDetector::image_callback, this, std::placeholders::_1));
            publisher_ = this->create_publisher<std_msgs::msg::Bool>("detect", 5);
            //timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&HumanDetector::image_callback, this ));

            net_ = cv::dnn::readNetFromCaffe("/home/pi/CANOPS/src/my_main/models/MobileNet-SSD/deploy.prototxt","/home/pi/CANOPS/src/my_main/models/MobileNet-SSD/mobilenet_iter_73000.caffemodel");
        }


        private:
            void image_callback(const sensor_msgs::msg::Image::SharedPtr msg){
                auto frame = cv_bridge::toCvShare(msg, "bgr8")->image;

                cv::resize(frame, frame, cv::Size(300, 300));

                auto inputBlob = cv::dnn::blobFromImage(frame, 0.007843, cv::Size(300, 300), 127.5);

                net_.setInput(inputBlob);
                auto detection = net_.forward();

                cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

                for (int i = 0; i < detectionMat.rows; i++){

                    //Initialize a true or false ros2 datatype
                    auto detectionChecker = std_msgs::msg::Bool();

                    //float confidence = detectionMat.at<float>(i,2);
		            float confidence = detectionMat.at<float>(i,2);
		            auto class_id = static_cast<int>(detectionMat.at<float>(i,1));

                    if ((class_id == 9) && (confidence > 0.20)){
                    	//RCLCPP_INFO(this->get_logger(), "Confidence Level: %.2f",  confidence);
                        detectionChecker.data = true;
                        publisher_->publish(detectionChecker);
                    }
                    else{
                        detectionChecker.data = false;
                        publisher_->publish(detectionChecker);
                    }
                   
                }
                //cv::imshow("Human Detection", frame);
                //cv::waitKey();
		

            }
        rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
        cv::dnn::Net net_;
        rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr publisher_;
        rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<HumanDetector>());

    rclcpp::shutdown();
    return 0;
}

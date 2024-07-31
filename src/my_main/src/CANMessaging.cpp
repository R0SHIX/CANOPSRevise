#include <functional>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include <rclcpp/subscription.hpp>
#include <rclcpp/utilities.hpp>
#include <std_msgs/msg/bool.hpp>
#include <PCANBasic.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include <cstring>
#include <vector>

class CANMessage: public rclcpp::Node{
    public:
        CANMessage(): Node("can_node"){
            subscription_ = this ->create_subscription<std_msgs::msg::Bool>("detect", 
            5,
            std::bind(&CANMessage::detect_callback, this, std::placeholders::_1));

            //Initialize the PCAN Connection
            TPCANStatus status = CAN_Initialize(PCAN_USBBUS1, PCAN_BAUD_250K);
            if (status != PCAN_ERROR_OK){
                RCLCPP_ERROR(this ->get_logger(), "Failed to initialize PCAN: %d", status);
            }
        }

        private:
            void detect_callback(const std_msgs::msg::Bool & msg){
                static const uint8_t LEN = 8;
	            static const uint16_t ID = 0x608;
                std::vector<uint8_t> sendData;
                //if true
                if(msg.data){
                    /*sendData = {0x22,0x13,0x40,0x00,0x52,0x00,0x00,0x00};
                    SendMessage(ID, sendData, LEN);
                    std::this_thread::sleep_for(std::chrono::seconds(0.5));

                    sendData = {0x22,0x13,0x40,0x00,0x29,0x00,0x00,0x00};
                    SendMessage(ID, sendData, LEN);
                    std::this_thread::sleep_for(std::chrono::seconds(0.5));
                    */
                    sendData = {0x22,0x13,0x40,0x00,0x00,0x00,0x00,0x00};
                    SendMessage(ID, sendData, LEN);
                }else{
                    //A5 = 165 = 16.5
                    sendData = {0x42,0x13,0x40,0x00,0xB8,0x00,0x00,0x00};
                    SendMessage(ID, sendData, LEN);

                }
            }

            void SendMessage(uint32_t id, std::vector<uint8_t>& data, uint8_t length ){
                TPCANMsg message;
                message.ID = id;
                message.MSGTYPE = PCAN_MESSAGE_STANDARD;
                message.LEN = length;

                memcpy(message.DATA, data.data(), length);

                CAN_Write(PCAN_USBBUS1, &message);
            }


        rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr subscription_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<CANMessage>());

    rclcpp::shutdown();
    
    return 0;
}

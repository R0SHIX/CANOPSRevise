cmake_minimum_required(VERSION 3.8)
project(my_main)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_library(PCAN_LIBRARY NAMES pcanbasic)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

add_compile_options(-O3 -march=armv8-a+crc -mcpu=cortex-a72)

add_executable(camera src/cameraNode.cpp)
#target_link_libraries(camera ${OpenCV_LIBS})
ament_target_dependencies(camera rclcpp cv_bridge OpenCV std_msgs)

add_executable(object src/ObjectDetNode.cpp)
ament_target_dependencies(object rclcpp sensor_msgs cv_bridge OpenCV)

add_executable(can src/CANMessaging.cpp)
target_link_libraries(can ${PCAN_LIBRARY})
target_link_directories(can PRIVATE /usr/include)
ament_target_dependencies(can rclcpp sensor_msgs std_msgs)

install(TARGETS camera object can DESTINATION lib/${PROJECT_NAME})


install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()

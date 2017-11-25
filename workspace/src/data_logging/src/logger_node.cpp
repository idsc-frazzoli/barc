/*
 * logger_node.cpp
 *
 *  Created on: Nov 21, 2017
 *      Author: jelavice
 */

/*
 * logging_node.cpp
 *
 *  Created on: Nov 15, 2017
 *      Author: jelavice
 */

// inlcude iostream and string libraries
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <ros/ros.h>
#include <boost/filesystem.hpp>

#include "encoder_data_logger.h"
#include "imu_data_logger.h"
#include "vicon_data_logger.h"

volatile sig_atomic_t flag = 0;

//not the greatest solution
void signalCallback(int sig) {
    flag = 1;
}

//don't need this unless I want to do some dynamic loading
//class LoggerFactory {
//
//public:
//
//    enum LoggerType {
//        VICON,
//        IMU,
//        ENCODER
//    };
//
//};

int main(int argc, char** argv) {

    int buffSize = 10000;
    ros::init(argc, argv, "data_logger");
    ros::NodeHandle n;
    double sleepTime = 0.01;

    vicon_log::ViconDataLogger viconDataLogger(buffSize, "viconOutput", "vicon/CAR/CAR", n, 1, "t, x, y, z, angX, angY, angZ, qx, qy, qz, qw");
    imu_log::ImuDataLogger imuDataLogger(buffSize, "imuOutput", "imu/data", n, 1000, "t, a_x, a_y, a_z, w_x, w_y, w_z, qx, qy, qz, qw");
    enc_log::EncDataLogger encDataLogger(buffSize, "encOutput", "forward_vel", n, 1000, "time, velFL, velFR, velBL, velBR");

    //current working directory
    boost::filesystem::path full_path(boost::filesystem::current_path());

    signal(SIGINT, signalCallback);

    while (true) {
        ros::spinOnce();
        if (flag) {
            std::cout << " \n Data logger node terminated. Saving measurements into: " << full_path << std::endl;
            break;
        }
        ros::Duration(sleepTime).sleep();
    }
    ros::shutdown();
    return 0;
}

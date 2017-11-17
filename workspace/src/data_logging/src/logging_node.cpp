/*
 * logging_node.cpp
 *
 *  Created on: Nov 15, 2017
 *      Author: jelavice
 */

// inlcude iostream and string libraries
#include <iostream>
#include <fstream>
#include <string>
#include <boost/circular_buffer.hpp>
#include <geometry_msgs/TransformStamped.h>
#include <sensor_msgs/Imu.h>
#include <Eigen/Dense>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <ros/ros.h>
#include "barc/Velocity.h"

volatile sig_atomic_t flag = 0;

//not the greatest solution
void signalCallback(int sig) {
    flag = 1;
}

// create a name for the file output
std::string filename = "exampleOutput.csv";

//TODO clean up this shit

struct VelData {
    double time;
    double vel;
};

//TODO add a time header here
class VelDataLogger {

public:
    VelDataLogger(int buffSize, std::string filename) :
            m_filename("Vel" + filename) {
        m_buffer.set_capacity(buffSize);

    }

    void msgCallback(barc::Velocity::ConstPtr msg) {

//        static double timeOffset = -1;
//
//        if (timeOffset < 0)
//            timeOffset = msg->header.stamp.sec + msg->header.stamp.nsec * 1e-9;

        VelData velData;

        velData.time = msg->time;
        velData.vel = msg->velocity;

        m_buffer.push_back(velData);
    }

    void run() {

    }

    void dumpToFile() {

        // create and open the .csv file
        m_file.open("filename");

        // write the file headers
        m_file << "time, vel_enc" << std::endl;

        for (boost::circular_buffer<VelData>::const_iterator it = m_buffer.begin(); it != m_buffer.end(); it++)
            m_file << it->time << ", " << it->vel << std::endl;

        // close the output file
        m_file.close();
    }

private:
    boost::circular_buffer<VelData> m_buffer;
    std::string m_filename;
    std::ofstream m_file;

};



















struct ImuData {

    double time;
    Eigen::Matrix<double, 3, 1> linAcc;
    Eigen::Matrix<double, 3, 1> angVel;
    Eigen::Quaternion<double> q;

};

//TODO add a time header here
class ImuDataLogger {

public:
    ImuDataLogger(int buffSize, std::string filename) :
            m_filename("Imu" + filename) {
        m_buffer.set_capacity(buffSize);

    }

    void msgCallback(sensor_msgs::Imu::ConstPtr msg) {

//        static double timeOffset = -1;
//
//        if (timeOffset < 0)
//            timeOffset = msg->header.stamp.sec + msg->header.stamp.nsec * 1e-9;

        ImuData imuData;

        imuData.angVel(0) = msg->angular_velocity.x;
        imuData.angVel(1) = msg->angular_velocity.y;
        imuData.angVel(2) = msg->angular_velocity.z;

        imuData.linAcc(0) = msg->linear_acceleration.x;
        imuData.linAcc(1) = msg->linear_acceleration.y;
        imuData.linAcc(2) = msg->linear_acceleration.z;

        imuData.q = Eigen::Quaternion<double>(msg->orientation.w, msg->orientation.x, msg->orientation.y, msg->orientation.z);

        m_buffer.push_back(imuData);
    }

    void run() {

    }

    void dumpToFile() {

        // create and open the .csv file
        m_file.open("filename");

        // write the file headers
        m_file << "t, a_x, a_y, a_z, w_x, w_y, w_z, qx, qy, qz, qw " << std::endl;

        for (boost::circular_buffer<ImuData>::const_iterator it = m_buffer.begin(); it != m_buffer.end(); it++)
            m_file << it->time << ", " << it->linAcc(0) << ", " << it->linAcc(1) << ", " << it->linAcc(2) << ", "
            << it->angVel(0) << ", " << it->angVel(1) << ", " << it->angVel(2) << ", "
            << it->q.x() << ", " << it->q.y() << ", " << it->q.z() << ", " << it->q.w() << std::endl;

        // close the output file
        m_file.close();
    }

private:
    boost::circular_buffer<ImuData> m_buffer;
    std::string m_filename;
    std::ofstream m_file;

};


























struct DataVicon {

    double time;
    Eigen::Matrix<double, 3, 1> pos;
    Eigen::Matrix<double, 3, 1> rot;
    Eigen::Quaternion<double> q;
};

class ViconDataLogger {

public:
    ViconDataLogger(int buffSize, std::string filename) :
            m_filename("Vicon_" + filename) {
        m_buffer.set_capacity(buffSize);

    }

    void msgCallback(geometry_msgs::TransformStamped::ConstPtr msg) {

        static double timeOffset = -1;

        if (timeOffset < 0)
            timeOffset = msg->header.stamp.sec + msg->header.stamp.nsec * 1e-9;

        DataVicon dataVicon;
        dataVicon.time = msg->header.stamp.sec + msg->header.stamp.nsec * 1e-9 - timeOffset;
        dataVicon.pos = Eigen::Matrix<double, 3, 1>(msg->transform.translation.x, msg->transform.translation.y, msg->transform.translation.z);
        dataVicon.q = Eigen::Quaternion<double>(msg->transform.rotation.w, msg->transform.rotation.x, msg->transform.rotation.y, msg->transform.rotation.z);
        dataVicon.rot = dataVicon.q.toRotationMatrix().eulerAngles(0, 1, 2);

        m_buffer.push_back(dataVicon);
    }

    void run() {

    }

    void dumpToFile() {

        // create and open the .csv file
        m_file.open(filename);

        // write the file headers
        m_file << "t, x, y, z, angX, angY, angZ, qx, qy, qz, qw" << std::endl;

        for (boost::circular_buffer<DataVicon>::const_iterator it = m_buffer.begin(); it != m_buffer.end(); it++)
            m_file << it->time << ", " << it->pos(0) << ", " << it->pos(1) << ", " << it->pos(2) << ", " << it->rot(0) << ", " << it->rot(1) << ", "
                    << it->rot(2) << ", " << it->q.x() << ", " << it->q.y() << ", " << it->q.z() << ", " << it->q.w() << std::endl;

        // close the output file
        m_file.close();
    }

private:
    boost::circular_buffer<DataVicon> m_buffer;
    std::string m_filename;
    std::ofstream m_file;

};

int main(int argc, char** argv) {

    int buffSize = 10000;
    ViconDataLogger viconDataLogger(buffSize, filename);
    ImuDataLogger imuDataLogger(buffSize, filename);
    VelDataLogger velDataLogger(buffSize, filename);
    ros::init(argc, argv, "data_logger");
    ros::NodeHandle n;
    ros::Subscriber sub, sub2, sub3;
    sub = n.subscribe("vicon/CAR/CAR", 1, &ViconDataLogger::msgCallback, &viconDataLogger);
    sub2 = n.subscribe("imu/Data", 1, &ImuDataLogger::msgCallback, &imuDataLogger);
    sub3 = n.subscribe("forward_vel", 1, &VelDataLogger::msgCallback, &velDataLogger);

    signal(SIGINT, signalCallback);

    while (true) {
        ros::spinOnce();
        if (flag) {
            std::cout << " \n Data logger node terminated. Saving measurements into: " << filename << std::endl;
            viconDataLogger.dumpToFile();
            imuDataLogger.dumpToFile();
            velDataLogger.dumpToFile();
            break;
        }
        ros::Duration(0.01).sleep();
    }
    ros::shutdown();
    return 0;
}

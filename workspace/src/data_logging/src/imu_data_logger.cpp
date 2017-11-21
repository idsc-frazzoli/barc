/*
 * imu_data_logger.cpp
 *
 *  Created on: Nov 21, 2017
 *      Author: jelavice
 */

#include "imu_data_logger.h"

ImuDataLogger::ImuDataLogger(int buffSize, std::string filename, std::string topic, ros::NodeHandle& n, int queueSize) :
        m_filename("Imu" + filename) {
    m_buffer.set_capacity(buffSize);
    m_sub = n.subscribe(topic, queueSize, &ImuDataLogger::msgCallback, this);

}

void ImuDataLogger::msgCallback(sensor_msgs::Imu::ConstPtr msg) {

    static double timeOffset = -1;

    if (timeOffset < 0)
        timeOffset = msg->header.stamp.sec + msg->header.stamp.nsec * 1e-9;

    ImuData imuData;
    imuData.time = msg->header.stamp.sec + msg->header.stamp.nsec * 1e-9 - timeOffset;
    imuData.angVel(0) = msg->angular_velocity.x;
    imuData.angVel(1) = msg->angular_velocity.y;
    imuData.angVel(2) = msg->angular_velocity.z;

    imuData.linAcc(0) = msg->linear_acceleration.x;
    imuData.linAcc(1) = msg->linear_acceleration.y;
    imuData.linAcc(2) = msg->linear_acceleration.z;

    imuData.q = Eigen::Quaternion<double>(msg->orientation.w, msg->orientation.x, msg->orientation.y, msg->orientation.z);

    m_buffer.push_back(imuData);
}

void ImuDataLogger::dumpToFile() {

    // create and open the .csv file
    m_file.open(m_filename);

    // write the file headers
    m_file << "t, a_x, a_y, a_z, w_x, w_y, w_z, qx, qy, qz, qw " << std::endl;

    for (boost::circular_buffer<ImuData>::const_iterator it = m_buffer.begin(); it != m_buffer.end(); it++)
        m_file << it->time << ", " << it->linAcc(0) << ", " << it->linAcc(1) << ", " << it->linAcc(2) << ", " << it->angVel(0) << ", " << it->angVel(1) << ", "
                << it->angVel(2) << ", " << it->q.x() << ", " << it->q.y() << ", " << it->q.z() << ", " << it->q.w() << std::endl;

    // close the output file
    m_file.close();
}

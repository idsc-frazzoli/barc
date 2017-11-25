/*
 * vicon_data_logger.cpp
 *
 *  Created on: Nov 21, 2017
 *      Author: jelavice
 */

#include "vicon_data_logger.h"

//TODO do this in the base class
ViconDataLogger::ViconDataLogger(int buffSize, std::string filename, std::string topic, ros::NodeHandle& n, int queueSize, std::string header) :
        BASE(buffSize, filename, queueSize, header) {
    m_sub = n.subscribe(topic, queueSize, &ViconDataLogger::msgCallback, this);
}

void ViconDataLogger::msgCallback(msgPtr_t msg) {

    static double timeOffset = -1;

    if (timeOffset < 0)
        timeOffset = msg->header.stamp.sec + msg->header.stamp.nsec * 1e-9;

    //magic number, number of elements that we want to insert
    int numElem = 11;
    std::vector<double> data(numElem);

    //the order has to match to the one in the header
    data.push_back(msg->header.stamp.sec + msg->header.stamp.nsec * 1e-9 - timeOffset);
    data.push_back(msg->transform.translation.x);
    data.push_back(msg->transform.translation.y);
    data.push_back(msg->transform.translation.z);
    data.push_back(msg->transform.rotation.w);
    data.push_back(msg->transform.rotation.x);
    data.push_back(msg->transform.rotation.y);
    data.push_back(msg->transform.rotation.z);

    Eigen::Quaternion<double> q(msg->transform.rotation.w, msg->transform.rotation.x, msg->transform.rotation.y, msg->transform.rotation.z);
    Eigen::Matrix<double, 3, 1> rot = q.toRotationMatrix().eulerAngles(0, 1, 2);
    data.push_back(rot(0));
    data.push_back(rot(1));
    data.push_back(rot(2));

    addToBuff(data);

}


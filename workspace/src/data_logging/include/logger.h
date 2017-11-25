/*
 * logger.h
 *
 *  Created on: Nov 21, 2017
 *      Author: jelavice
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <boost/circular_buffer.hpp>

template<typename MSGTYPEPTR>
class DataLogger {

public:

    typedef boost::circular_buffer<std::vector<double> > buffer_t;

    DataLogger(int buffSize, std::string filename, int queueSize, std::string header) :
            m_filename(filename), m_header(header), m_dumpedToFile(false) {
        m_buffer.set_capacity(buffSize);
    }

    virtual ~DataLogger() {
        dumpToFile();
    }

    virtual void msgCallback(MSGTYPEPTR msg) = 0;

    void dumpToFile() {

        if (m_dumpedToFile)
            return;

        std::string fileName = m_filename + "_" + getCurrTime() + ".csv";
        std::ofstream file;
        file.open(fileName);

        // write the file headers
        file << m_header << std::endl;

        for (buffer_t::const_iterator it = m_buffer.begin(); it != m_buffer.end(); it++) {
            for (int i = 0; i < it->size(); i++)
                file << it->at(i) << ", ";
            file << std::endl;
        }

        // close the output file
        file.close();

        m_dumpedToFile = true;
    }
protected:

    std::string getCurrTime() {
        time_t rawtime;
        struct tm * timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%d-%m-%Y_%I:%M:%S", timeinfo);
        return std::string(buffer);

    }

    void addToBuff(const std::vector<double>& vec) {
        m_buffer.push_back(vec);
    }

private:
    buffer_t m_buffer;
    std::string m_filename;
    std::string m_header;
    bool m_dumpedToFile;

};

#endif /* LOGGER_H_ */

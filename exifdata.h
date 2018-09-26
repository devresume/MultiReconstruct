#ifndef EXIFDATA_H
#define EXIFDATA_H

#include "ExifTool.h"

#include <time.h>
#include <string>
#include <fstream>

#include "boost/date_time/posix_time/posix_time.hpp"

#include <opencv2/core/core.hpp>

//2018-03-12T11:19:55.365

using namespace boost::posix_time;

class ExifData
{
public:
    ExifData();
    ExifData(long atTime, int width, int height, double focal_ratio,  double altitude, double latitude, double longitude, int relAlt, int vx, int vy, int vz);
    void exportToFile(std::string filename);
    void writeExifToImage(const std::string& filename);
    static void exportOverridesFile(std::string path, int width, int height, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, double focal_ratio);
    long atTime;
    int width, height, relAlt, vx, vy, vz;
    double focal_ratio, latitude, altitude, longitude;
};

#endif // EXIFDATA_H

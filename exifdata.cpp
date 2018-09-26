#include "exifdata.h"
#include "dms.h"
#include <vector>

using std::string;      using std::vector;

ExifData::ExifData()
{

}

ExifData::ExifData(long atTime, int width, int height, double focal_ratio,  double altitude, double latitude, double longitude, int relAlt, int vx, int vy, int vz)
{
    this->atTime = atTime;
    this->width = width;
    this->height = height;
    this->focal_ratio = focal_ratio;
    this->altitude = altitude;
    this->latitude = latitude;
    this->longitude = longitude;
    this->relAlt = relAlt;
    this->vx = vx;
    this->vy = vy;
    this->vz = vz;
}

void ExifData::exportToFile(std::string path)
{
    std::ofstream out(path + ".exif");
    out << "{" << std::endl;
        out << "    \"width\": " << width << "," << std::endl;
        out << "    \"camera\": \"v2 unknown unknown 2704 1520 perspective 0\"," << std::endl;
        out << "    \"projection_type\": \"perspective\"," << std::endl;
        out << "    \"orientation\": 1," << std::endl;
        out << "    \"focal_ratio\": " << std::fixed << std::setprecision(14) << focal_ratio << "," << std::endl;
        out << "    \"make\": \"unknown\"," << std::endl;
        out << "    \"gps\": {" << std::endl;
                   out << "        \"latitude\": " << std::fixed << std::setprecision(14) << latitude / 1000.0 << "," << std::endl;
                   out << "        \"altitude\": " << std::fixed << std::setprecision(1) << altitude / 10009.0 << "," << std::endl;
                   out << "        \"dop\": 5.0," << std::endl;
                   out << "        \"longitude\": " << std::fixed << std::setprecision(14) << longitude / 1000.0 << std::endl;
               out << "    }," << std::endl;
        out << "    \"model\": \"unknown\"," << std::endl;
        out << "    \"capture_time\": 0.0," << std::endl;
        out << "    \"height\": " << height << std::endl;
    out << "}" << std::endl;
    out.close();
}

void ExifData::exportOverridesFile(std::string path, int width, int height, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, double focal_ratio)
{
    std::ofstream out(path + "/" + "camera_models_overrides.json");
    out << "{" << std::endl;
    out << "    \"v2 unknown unknown 2704 1520 perspective 0\": { " << std::endl;
    out << "        \"focal_prior\": " << std::fixed << std::setprecision(14) << focal_ratio << "," << std::endl;
    out << "        \"width\": " << width << ", " << std::endl;
    out << "        \"k1\": " << distCoeffs.at<double>(0) << ", " << std::endl;
    out << "        \"k2\": " << distCoeffs.at<double>(1) << ", " << std::endl;
    out << "        \"k1_prior\": " << distCoeffs.at<double>(0) << ", " << std::endl;
    out << "        \"k2_prior\": " << distCoeffs.at<double>(1) << ", " << std::endl;
    out << "        \"projection_type\": \"perspective\", " << std::endl;
    out << "        \"focal\": " << std::fixed << std::setprecision(14) << focal_ratio << "," << std::endl;
    out << "        \"height\": " << height << " " << std::endl;
    out << "    }" << std::endl;
    out << "}" << std::endl;
    out.close();
}

void ExifData::writeExifToImage(const string& filename)
{
    ExifTool *et = new ExifTool();

//    // set new values of tags to write
    et->SetNewValue("EXIF:XResolution", "72");
    et->SetNewValue("EXIF:YResolution", "72");
    et->SetNewValue("EXIF:FocalLength", "22.7041");
    et->SetNewValue("EXIF:FocalLengthIn35mmFormat", "16");

    vector<string> latvec = DegreesMinutesSeconds(latitude);
    vector<string> lonvec = DegreesMinutesSeconds(longitude);

    string latstr = latvec.at(0) + " " + latvec.at(1) + " " + latvec.at(2);
    string lonstr = lonvec.at(0) + " " + lonvec.at(1) + " " + lonvec.at(2);


    et->SetNewValue("GPS:GPSLatitudeRef", "N");
    et->SetNewValue("GPS:GPSLatitude", latstr.c_str());
    et->SetNewValue("GPS:GPSLongitudeRef", "W");
    et->SetNewValue("GPS:GPSLongitude", lonstr.c_str());
    et->SetNewValue("GPS:GPSAltitude", std::to_string(((int)altitude / 10000)).c_str());
    et->SetNewValue("GPS:GPSDOP", "5");

    // write the information
    et->WriteInfo(filename.c_str(), "-overwrite_original");
    delete et;
}

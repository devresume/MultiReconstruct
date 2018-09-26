#ifndef UTILITIES_H
#define UTILITIES_H
#include <ostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>

#include "boost/date_time/posix_time/posix_time.hpp"

#include <QString>
#include <QFileInfo>

#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

using std::vector;  using std::string;
using cv::Mat;      using cv::Size;
using cv::Point2f;  using cv::Point3f;
using cv::FileStorage;  using cv::Range;
using cv::FileNode; using cv::FileNodeIterator;

using std::ostringstream;

using namespace boost::posix_time;

enum PatternType{
    CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID, CHESSBOARD
};


enum CaptureType{ OPENSFM, OPENDRONEMAP, ORBSLAM2 };

class utilities
{
public:
    utilities();


    static double computeReprojectionErrors(
            const vector<vector<Point3f> >& objectPoints,
            const vector<vector<Point2f> >& imagePoints,
            const vector<Mat>& rvecs, const vector<Mat>& tvecs,
            const Mat& cameraMatrix, const Mat& distCoeffs,
            vector<float>& perViewErrors )
    {
        vector<Point2f> imagePoints2;
        int i, totalPoints = 0;
        double totalErr = 0, err;
        perViewErrors.resize(objectPoints.size());

        for( i = 0; i < (int)objectPoints.size(); i++ )
        {
            projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
                          cameraMatrix, distCoeffs, imagePoints2);
            err = norm(Mat(imagePoints[i]), Mat(imagePoints2), cv::NORM_L2);
            int n = (int)objectPoints[i].size();
            perViewErrors[i] = (float)std::sqrt(err*err/n);
            totalErr += err*err;
            totalPoints += n;
        }

        return std::sqrt(totalErr/totalPoints);
    }


    static void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, PatternType patternType = CHESSBOARD)
    {
        corners.resize(0);

        switch(patternType)
        {
          case CHESSBOARD:
          case CIRCLES_GRID:
            for( int i = 0; i < boardSize.height; i++ )
                for( int j = 0; j < boardSize.width; j++ )
                    corners.push_back(Point3f(float(j*squareSize),
                                              float(i*squareSize), 0));
            break;

          case ASYMMETRIC_CIRCLES_GRID:
            for( int i = 0; i < boardSize.height; i++ )
                for( int j = 0; j < boardSize.width; j++ )
                    corners.push_back(Point3f(float((2*j + i % 2)*squareSize),
                                              float(i*squareSize), 0));
            break;

          default:
            CV_Error(cv::Error::StsBadArg, "Unknown pattern type\n");
        }
    }

    static bool runCalibration( vector<vector<Point2f> > imagePoints,
                        Size imageSize, Size boardSize, PatternType patternType,
                        float squareSize, float aspectRatio,
                        int flags, Mat& cameraMatrix, Mat& distCoeffs,
                        vector<Mat>& rvecs, vector<Mat>& tvecs,
                        vector<float>& reprojErrs,
                        double& totalAvgErr)
    {
        cameraMatrix = Mat::eye(3, 3, CV_64F);
        if( flags & cv::CALIB_FIX_ASPECT_RATIO )
            cameraMatrix.at<double>(0,0) = aspectRatio;

        distCoeffs = Mat::zeros(8, 1, CV_64F);

        vector<vector<Point3f> > objectPoints(1);
        calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

        objectPoints.resize(imagePoints.size(),objectPoints[0]);

        double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                        distCoeffs, rvecs, tvecs, flags|cv::CALIB_FIX_K4|cv::CALIB_FIX_K5);
                        ///*|CALIB_FIX_K3*/|CALIB_FIX_K4|CALIB_FIX_K5);
        printf("RMS error reported by calibrateCamera: %g\n", rms);

        bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

        totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                    rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

        return ok;
    }
//    static void saveCameraParams( const string& filename,
//                           Size imageSize, Size boardSize,
//                           float squareSize, float aspectRatio, int flags,
//                           const Mat& cameraMatrix, const Mat& distCoeffs,
//                           const vector<Mat>& rvecs, const vector<Mat>& tvecs,
//                           const vector<float>& reprojErrs,
//                           const vector<vector<Point2f> >& imagePoints,
//                           double totalAvgErr )
//    {
//        FileStorage fs( filename, FileStorage::WRITE );

//        time_t tt;
//        time( &tt );
//        struct tm *t2 = localtime( &tt );
//        char buf[1024];
//        strftime( buf, sizeof(buf)-1, "%c", t2 );

//        fs << "calibration_time" << buf;

//        if( !rvecs.empty() || !reprojErrs.empty() )
//            fs << "nframes" << (int)std::max(rvecs.size(), reprojErrs.size());
//        fs << "image_width" << imageSize.width;
//        fs << "image_height" << imageSize.height;
//        fs << "board_width" << boardSize.width;
//        fs << "board_height" << boardSize.height;
//        fs << "square_size" << squareSize;

//        if( flags & cv::CALIB_FIX_ASPECT_RATIO )
//            fs << "aspectRatio" << aspectRatio;

//        if( flags != 0 )
//        {
//            sprintf( buf, "flags: %s%s%s%s",
//                flags & cv::CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
//                flags & cv::CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
//                flags & cv::CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
//                flags & cv::CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "" );
//            //cvWriteComment( *fs, buf, 0 );
//        }

//        fs << "flags" << flags;

//        fs << "camera_matrix" << cameraMatrix;
//        fs << "distortion_coefficients" << distCoeffs;

//        fs << "avg_reprojection_error" << totalAvgErr;
//        if( !reprojErrs.empty() )
//            fs << "per_view_reprojection_errors" << Mat(reprojErrs);

//        if( !rvecs.empty() && !tvecs.empty() )
//        {
//            CV_Assert(rvecs[0].type() == tvecs[0].type());
//            Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
//            for( int i = 0; i < (int)rvecs.size(); i++ )
//            {
//                Mat r = bigmat(Range(i, i+1), Range(0,3));
//                Mat t = bigmat(Range(i, i+1), Range(3,6));

//                CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
//                CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
//                //*.t() is MatExpr (not Mat) so we can use assignment operator
//                r = rvecs[i].t();
//                t = tvecs[i].t();
//            }
//            //cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
//            fs << "extrinsic_parameters" << bigmat;
//        }

//        if( !imagePoints.empty() )
//        {
//            Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
//            for( int i = 0; i < (int)imagePoints.size(); i++ )
//            {
//                Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
//                Mat imgpti(imagePoints[i]);
//                imgpti.copyTo(r);
//            }
//            fs << "image_points" << imagePtMat;
//        }
//    }


    static void saveCameraParams( const string& filename,
                           Size imageSize, Size boardSize,
                           float squareSize, float aspectRatio, int flags,
                           const Mat& cameraMatrix, const Mat& distCoeffs,
                           const vector<Mat>& rvecs, const vector<Mat>& tvecs,
                           const vector<float>& reprojErrs,
                           const vector<vector<Point2f> >& imagePoints,
                           double totalAvgErr )
    {
        FileStorage fs( filename, FileStorage::WRITE );

        fs << "camera_matrix" << cameraMatrix;
        fs << "distortion_coefficients" << distCoeffs;

    }

    static void saveORBSlamYaml(const std::string& outputFilename, cv::Mat cameraMatrix, cv::Mat distCoeffs)
    {
        std::ofstream out(outputFilename);
        out <<
               "%YAML:1.0" << std::endl <<
               std::endl <<
               "#--------------------------------------------------------------------------------------------"  << std::endl <<
               "# Camera Parameters. Adjust them!"  << std::endl <<
               "#--------------------------------------------------------------------------------------------"  << std::endl <<
               std::endl <<
               "# Camera calibration and distortion parameters (OpenCV)"  << std::endl <<
               "Camera.fx: " << cameraMatrix.at<double>(0)  << std::endl <<
               "Camera.fy: " << cameraMatrix.at<double>(4)  << std::endl <<
               "Camera.cx: " << cameraMatrix.at<double>(2)  << std::endl <<
               "Camera.cy: " << cameraMatrix.at<double>(5)  << std::endl <<
               std::endl <<
               "Camera.k1: " << distCoeffs.at<double>(0)  << std::endl <<
               "Camera.k2: " << distCoeffs.at<double>(1)    << std::endl <<
               "Camera.p1: " << distCoeffs.at<double>(2)    << std::endl <<
               "Camera.p2: " << distCoeffs.at<double>(3)    << std::endl <<
               "Camera.k3: " << distCoeffs.at<double>(4)    << std::endl <<
                std::endl <<
               "# Camera frames per second"  << std::endl <<
               "Camera.fps: 60.0"  << std::endl <<
               std::endl <<
               "# Color order of the images (0: BGR, 1: RGB. It is ignored if images are grayscale)"  << std::endl <<
               "Camera.RGB: 1"  << std::endl <<
                std::endl <<
               "#--------------------------------------------------------------------------------------------"  << std::endl <<
               "# ORB Parameters"  << std::endl <<
               "#--------------------------------------------------------------------------------------------"  << std::endl <<
               std::endl <<
               "# ORB Extractor: Number of features per image"  << std::endl <<
               "ORBextractor.nFeatures: 1000"  << std::endl <<
               std::endl <<
               "# ORB Extractor: Scale factor between levels in the scale pyramid"  << std::endl <<
               "ORBextractor.scaleFactor: 1.2"  << std::endl <<
               std::endl <<
               "# ORB Extractor: Number of levels in the scale pyramid"  << std::endl <<
               "ORBextractor.nLevels: 8"  << std::endl <<
               std::endl <<
               "# ORB Extractor: Fast threshold"  << std::endl <<
               "# Image is divided in a grid. At each cell FAST are extracted imposing a minimum response."  << std::endl <<
               "# Firstly we impose iniThFAST. If no corners are detected we impose a lower value minThFAST"  << std::endl <<
               "# You can lower these values if your images have low contrast"  << std::endl <<
               "ORBextractor.iniThFAST: 20"  << std::endl <<
               "ORBextractor.minThFAST: 7"  << std::endl <<
               std::endl <<
               "#--------------------------------------------------------------------------------------------"  << std::endl <<
               "# Viewer Parameters"  << std::endl <<
               "#--------------------------------------------------------------------------------------------"  << std::endl <<
               "Viewer.KeyFrameSize: 0.05"  << std::endl <<
               "Viewer.KeyFrameLineWidth: 1"  << std::endl <<
               "Viewer.GraphLineWidth: 0.9"  << std::endl <<
               "Viewer.PointSize:2"  << std::endl <<
               "Viewer.CameraSize: 0.08"  << std::endl <<
               "Viewer.CameraLineWidth: 3"  << std::endl <<
               "Viewer.ViewpointX: 0"  << std::endl <<
               "Viewer.ViewpointY: -0.7"  << std::endl <<
               "Viewer.ViewpointZ: -1.8"  << std::endl <<
               "Viewer.ViewpointF: 500" << std::endl;
    }

    static bool runAndSave(const string& outputFilename,
                    const vector<vector<Point2f> >& imagePoints,
                    Size imageSize, Size boardSize, PatternType patternType, float squareSize,
                    float aspectRatio, int flags, Mat& cameraMatrix,
                    Mat& distCoeffs, bool writeExtrinsics, bool writePoints )
    {
        vector<Mat> rvecs, tvecs;
        vector<float> reprojErrs;
        double totalAvgErr = 0;

        bool ok = runCalibration(imagePoints, imageSize, boardSize, patternType, squareSize,
                       aspectRatio, flags, cameraMatrix, distCoeffs,
                       rvecs, tvecs, reprojErrs, totalAvgErr);
        printf("%s. avg reprojection error = %.2f\n",
               ok ? "Calibration succeeded" : "Calibration failed",
               totalAvgErr);

        if( ok )
            saveCameraParams( outputFilename, imageSize,
                             boardSize, squareSize, aspectRatio,
                             flags, cameraMatrix, distCoeffs,
                             writeExtrinsics ? rvecs : vector<Mat>(),
                             writeExtrinsics ? tvecs : vector<Mat>(),
                             writeExtrinsics ? reprojErrs : vector<float>(),
                             writePoints ? imagePoints : vector<vector<Point2f> >(),
                             totalAvgErr );
        return ok;
    }






    static long pt_to_time_t(const ptime& pt)
    {
        ptime timet_start(boost::gregorian::date(1970,1,1));
        time_duration diff = pt - timet_start;
        return diff.total_milliseconds();

    }

    static long seconds_from_epoch(const std::string& s)
    {
        std::string str = s;
        str.replace(10, 1, " ");
        ptime pt = time_from_string(str);

        return pt_to_time_t(pt);
    }

    static bool dirExists(QString dir)
    {
        QFileInfo checkFile(dir);
        if (checkFile.exists() && checkFile.isDir()) {
            return true;
        } else  {
            return false;
        }
    }

    static QString nextDirName(QString folder)
    {
        int i = 1;
        while(true)
        {
            QString searchFile = QString(folder).arg(i);
            if(!dirExists(searchFile))
            {
                return searchFile;
            }
            ++i;
        }
        return NULL;
    }

    static QString getFileName(QString path, int num)
    {
        ostringstream out2;
        out2 << std::internal << std::setfill('0') << std::setw(10) << num;

        return path + QString::fromStdString("/img" + out2.str() + ".jpg");
    }

    static bool readStringList( const string& filename, vector<string>& l )
    {
        l.resize(0);
        FileStorage fs(filename, FileStorage::READ);
        if( !fs.isOpened() )
            return false;
        FileNode n = fs.getFirstTopLevelNode();
        if( n.type() != FileNode::SEQ )
            return false;
        FileNodeIterator it = n.begin(), it_end = n.end();
        for( ; it != it_end; ++it )
            l.push_back((string)*it);
        return true;
    }
};

#endif

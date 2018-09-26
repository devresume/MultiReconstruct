#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "exifdata.h"
#include "utilities.h"

#include <cctype>
#include <stdio.h>
#include <string>
#include <time.h>

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <vector>
#include <QRadioButton>
#include <QGridLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>
#include <QBoxLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "boost/date_time/posix_time/posix_time.hpp"

#define MILLI 1000
#define MICRO 1000000
#define TEMPPATH = "./tmp"
#define DIR_NAME_FORMAT "/output%1"
#define TEMP_OUTPUT_PATH "./tmp/output%1"

using namespace boost::posix_time;

const std::locale formats = std::locale(std::locale::classic(),new time_input_facet("%Y-%m-%d %H:%M:%S.%f"));
const std::string recPath = "./resources";
static const std::vector<std::string> osfmResources { "/config.yaml"};



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initControls();
    void layoutControls();
    void connectControls();
    void runCalib();
    QString getImageScaleTxt();
    QString getCameraIntrinsicsTxt();
    cv::Mat getCameraMatrix();
    void setCameraMatrix(cv::Mat mat);
    cv::Mat getDistCoeffs();
    void setDistCoeffs(cv::Mat mat);
    void deserializeCameraMats();
    void deserialize();
    void serialize();
    void readCameraParams(const std::string& filename);
    int getIntPType();
    void setPType(int ptype);
    bool setPatternType();
    std::vector<ExifData*>::size_type findExifData(long start, std::vector<ExifData*>::size_type csvIdx, int frameIdx);

    void processCSV();
    bool setCapType();
    void copyResources(std::string outputPath);
    int getIntOutputType();
    void setOutputType(int ptype);
public slots:
    void save();
    void saveAs();
    void load();

    void processVid();

    void selectIntrinsicsFile();

    void selectVidFile();
    void captureAndCalibrate();
    void quitPreview();

    void onOutputPathBtnClicked();
    void onIntrinsicsOutputBtnClicked();
    void onSelectVidBtnClicked();
    void onSelectCSVBtnClicked();
    void onCaptureBtnClicked();
    void onPlaySegmentVidBtnClicked();
    void onExportCameraIntrBtnClicked();
private:
    Ui::MainWindow *ui;

    QGraphicsPixmapItem pixmap;
    QGraphicsView *graphicsView;

    QLabel *originalLbl, *imageScaleLbl, *curFileLbl;

    QLineEdit *curFileTxt, *imageScaleTxt, *camIntrinsicsFileTxt;

    QPushButton *loadBtn, *saveBtn, *saveAsBtn, *camIntrinsicsFileBtn;

    QWidget *calibTab, *captureTab, *displayTab;

    cv::Mat cameraMatrix, distCoeffs;

    // calibrate

    cv::VideoCapture capture;

    PatternType selPattern;
    QPushButton* calibBtn, *videoFileBtn, *exportCameraIntrBtn;

    QRadioButton *pTypeCircles, *pTypeAsymCircles, *pTypeChess;

    QLineEdit *videoFileTxt, *ptWidthTxt, *ptHeightTxt, *skipFramesTxt, *calibNumFramesTxt, *frameRateTxt;

    QLabel *ptWidthLbl, *ptHeightLbl, *skipFramesLbl, *calibNumFramesLbl, *frameRateLbl;

    // Capture

    QRadioButton *outputTypeOSFM, *outputTypeORBSLM2, *outputTypeODM;

    QPushButton* outputPathBtn, *selectVidBtn, *selectCSVBtn, *captureBtn, *playSegmentVidBtn, *selectIntrinsicsOutputBtn;

    QLineEdit* outputPathTxt, *outputTxt, *capCurFileTxt, *segmentVideoTxt, *csvTxt, *startIdxTxt,
    *endIdxTxt, *playStartFrameTxt, *layStartFrameTxt, *stepLenTxt, *playLengthTxt, *captureNumFramesTxt,
    *focalLenTxt, *selectIntrinsicsOutputTxt;

    QLabel* outputLbl, *capCurFileLbl, *startIdxLbl, *endIdxLbl, *startFrameLbl,
    *stepLenLbl, *playLengthLbl, *captureNumFramesLbl, *focalLenLbl;

    CaptureType selCapType;

    QString vidFile, csvFile, outputPath;
    QTimer *tmrTimer;
    cv::VideoCapture capWebcam;
    std::vector<cv::Mat> images;
    std::vector<int> labels;
    std::vector<ExifData*>* csvData;
    cv::Mat original, originalColor;
    int m_csvidx = 0, m_videoIdx = 0, prevAlt = 0, prevLon = 0, prevLat = 0, prevCSVidx = 0;
    long m_time1 = 0;
};

#endif // MAINWINDOW_H

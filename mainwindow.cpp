#include "mainwindow.h"
#include "ui_mainwindow.h"

//ORB_SLAM2 Shell: ./Examples/Monocular/mono_tum Vocabulary/ORBvoc.txt Examples/Monocular/DevonGoPro4.yaml /media/devon/datapartition/ORBOutput2/
//OpenSFM Shell: bin/opensfm_run_all data/flight1
//OpenSFM shell#2: python -m SimpleHTTPServer
//OpenSFM Browser: http://localhost:8000/viewer/reconstruction.html#file=/data/flight1/reconstruction.meshed.json

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initControls();
    layoutControls();
    connectControls();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::deserializeCameraMats()
{
    FileStorage fs( camIntrinsicsFileTxt->text().toStdString(), FileStorage::READ );

    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
}

void MainWindow::deserialize()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video"), "./", tr("All files (*.dat)"));
    if(fileName.isEmpty())
    {
        return;
    }

    string curFileStr, imageScaleStr, camIntrinsicsFileStr;

    cv::FileStorage file(fileName.toStdString(), cv::FileStorage::READ);
    file["curFileTxt"] >> curFileStr;
    file["imageScaleTxt"] >> imageScaleStr;
    file["camIntrinsicsFileTxt"] >> camIntrinsicsFileStr;

    curFileTxt->setText(QString(curFileStr.c_str()));
    imageScaleTxt->setText(QString(imageScaleStr.c_str()));
    camIntrinsicsFileTxt->setText(QString(camIntrinsicsFileStr.c_str()));

    // calibrate

    string videoFileStr, ptWidthStr, ptHeightStr, skipFramesStr, calibNumFramesStr, frameRateStr;
    int pType = 0;
    file["pType"] >> pType;
    file["videoFileTxt"] >> videoFileStr;
    file["ptWidthTxt"] >> ptWidthStr;
    file["ptHeightTxt"] >> ptHeightStr;
    file["skipFramesTxt"] >> skipFramesStr;
    file["calibNumFramesTxt"] >> calibNumFramesStr;
    file["frameRateTxt"] >> frameRateStr;

    setPType(pType);
    videoFileTxt->setText(QString(videoFileStr.c_str()));
    ptWidthTxt->setText(QString(ptWidthStr.c_str()));
    ptHeightTxt->setText(QString(ptHeightStr.c_str()));
    skipFramesTxt->setText(QString(skipFramesStr.c_str()));
    calibNumFramesTxt->setText(QString(calibNumFramesStr.c_str()));
    frameRateTxt->setText(QString(frameRateStr.c_str()));

    // capture

    string segmentVideoStr, csvStr, outputPathStr, outputStr, selectIntrinsicsOutputStr,
            startIdxStr, endIdxStr, playStartFrameStr, stepLenStr,
            playLengthStr, captureNumFramesStr, focalLenStr;

    int outputType = 0;
    file["outputType"] >> outputType;
    file["segmentVideoTxt"] >> segmentVideoStr;
    file["csvTxt"] >> csvStr;
    file["outputPathTxt"] >> outputPathStr;
    file["outputTxt"] >> outputStr;
    file["selectIntrinsicsOutputTxt"] >> selectIntrinsicsOutputStr;
    file["startIdxTxt"] >> startIdxStr;
    file["endIdxTxt"] >> endIdxStr;
    file["playStartFrameTxt"] >> playStartFrameStr;
    file["stepLenTxt"] >> stepLenStr;
    file["playLengthTxt"] >> playLengthStr;
    file["captureNumFramesTxt"] >> captureNumFramesStr;
    file["focalLenTxt"] >> focalLenStr;

    setOutputType(outputType);
    segmentVideoTxt->setText(QString(segmentVideoStr.c_str()));
    csvTxt->setText(QString(csvStr.c_str()));
    outputPathTxt->setText(QString(outputPathStr.c_str()));
    outputTxt->setText(QString(outputStr.c_str()));
    selectIntrinsicsOutputTxt->setText(QString(selectIntrinsicsOutputStr.c_str()));
    startIdxTxt->setText(QString(startIdxStr.c_str()));
    endIdxTxt->setText(QString(endIdxStr.c_str()));
    playStartFrameTxt->setText(QString(playStartFrameStr.c_str()));
    stepLenTxt->setText(QString(stepLenStr.c_str()));
    playLengthTxt->setText(QString(playLengthStr.c_str()));
    captureNumFramesTxt->setText(QString(captureNumFramesStr.c_str()));
    focalLenTxt->setText(QString(focalLenStr.c_str()));
}

void MainWindow::serialize()
{

    cv::FileStorage file(curFileTxt->text().toStdString(), cv::FileStorage::WRITE);

    file << "curFileTxt" << curFileTxt->text().toStdString();
    file << "imageScaleTxt" << imageScaleTxt->text().toStdString();
    file << "camIntrinsicsFileTxt" << camIntrinsicsFileTxt->text().toStdString();

    //calibrate
    file << "pType" << getIntPType();
    file << "videoFileTxt" << videoFileTxt->text().toStdString();
    file << "ptWidthTxt" << ptWidthTxt->text().toStdString();
    file << "ptHeightTxt" << ptHeightTxt->text().toStdString();
    file << "skipFramesTxt" << skipFramesTxt->text().toStdString();
    file << "calibNumFramesTxt" << calibNumFramesTxt->text().toStdString();
    file << "frameRateTxt" << frameRateTxt->text().toStdString();

    //captue;
    file << "outputType" << getIntOutputType();
    file << "segmentVideoTxt" << segmentVideoTxt->text().toStdString();
    file << "csvTxt" << csvTxt->text().toStdString();
    file << "outputPathTxt" << outputPathTxt->text().toStdString();
    file << "outputTxt" << outputTxt->text().toStdString();
    file << "selectIntrinsicsOutputTxt" << selectIntrinsicsOutputTxt->text().toStdString();
    file << "startIdxTxt" << startIdxTxt->text().toStdString();
    file << "endIdxTxt" << endIdxTxt->text().toStdString();
    file << "playStartFrameTxt" << playStartFrameTxt->text().toStdString();
    file << "stepLenTxt" << stepLenTxt->text().toStdString();
    file << "playLengthTxt" << playLengthTxt->text().toStdString();
    file << "captureNumFramesTxt" << captureNumFramesTxt->text().toStdString();
    file << "focalLenTxt" << focalLenTxt->text().toStdString();




}

void MainWindow::saveAs()
{
    curFileTxt->setText(QFileDialog::getSaveFileName(this, tr("Save Config Data"), "./", tr("Config Data (*.dat);")));
    serialize();
}

void MainWindow::save()
{
    if(!curFileTxt->text().isEmpty())
    {
        serialize();
    }

}

void MainWindow::load()
{
    deserialize();
}

void MainWindow::selectIntrinsicsFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Cam Intrinsics"), "./", tr("Camera Intrinsics files (*.intr)"));
    if(fileName.isEmpty())
    {
        return;
    }
    camIntrinsicsFileTxt->setText(fileName);
}

void MainWindow::initControls()
{
    //Main

    //originalLbl = new QLabel();
    //originalLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    loadBtn = new QPushButton(tr("Load"));
    saveBtn = new QPushButton(tr("Save"));
    saveAsBtn = new QPushButton(tr("Save As"));

    curFileLbl = new QLabel(tr("Current File:"));
    curFileTxt = new QLineEdit();

    imageScaleLbl = new QLabel(tr("Scale amount: "));
    imageScaleTxt = new QLineEdit();

    camIntrinsicsFileBtn = new QPushButton(tr("Camera Intrinsics File"));
    camIntrinsicsFileTxt = new QLineEdit();

    calibTab = new QWidget();
    captureTab = new QWidget();
    displayTab = new QWidget();
    // Calibration

    pTypeCircles = new QRadioButton(tr("Circles Grid"));
    pTypeAsymCircles = new QRadioButton(tr("Asymetrical Circles Grid"));
    pTypeChess = new QRadioButton(tr("Chessboard"));

    videoFileBtn = new QPushButton(tr("Calib. Video File:"));
    videoFileTxt = new QLineEdit();

    ptWidthLbl = new QLabel(tr("Pattern Width:"));
    ptWidthTxt = new QLineEdit();

    ptHeightLbl = new QLabel(tr("Pattern Height:"));
    ptHeightTxt = new QLineEdit();

    skipFramesLbl = new QLabel(tr("Skip Frames:"));
    skipFramesTxt = new QLineEdit();

    calibNumFramesLbl = new QLabel(tr("Num Frames:"));
    calibNumFramesTxt = new QLineEdit();

    frameRateLbl = new QLabel(tr("Num Frames:"));
    frameRateTxt = new QLineEdit();

    calibBtn = new QPushButton(tr("Run Calibrate"));

    // Capture

    selectVidBtn = new QPushButton(tr("Select Video"));
    segmentVideoTxt = new QLineEdit();

    selectCSVBtn = new QPushButton(tr("Select Data CSV"));
    csvTxt = new QLineEdit();

    outputPathBtn = new QPushButton(tr("Output Path"));
    outputPathTxt = new QLineEdit();

    outputLbl = new QLabel(tr("Output Name:"));
    outputTxt = new QLineEdit();

    selectIntrinsicsOutputBtn = new QPushButton(tr("Output Camera Intrinsics"));
    selectIntrinsicsOutputTxt = new QLineEdit();

    startIdxLbl = new QLabel(tr("Start Index:"));
    startIdxTxt = new QLineEdit();

    endIdxLbl = new QLabel(tr("End Index:"));
    endIdxTxt = new QLineEdit();

    startFrameLbl = new QLabel(tr("Video Start:"));
    playStartFrameTxt  = new QLineEdit();

    stepLenLbl = new QLabel(tr("Step Size:"));
    stepLenTxt = new QLineEdit();

    playLengthLbl = new QLabel(tr("Play Length(sec):"));
    playLengthTxt = new QLineEdit();

    captureNumFramesLbl = new QLabel(tr("# Frames:"));
    captureNumFramesTxt = new QLineEdit();

    focalLenLbl = new QLabel(tr("Focal Len:"));
    focalLenTxt = new QLineEdit();

    graphicsView = new QGraphicsView;


    outputTypeOSFM = new QRadioButton(tr("Open SFM"));
    outputTypeODM = new QRadioButton(tr("Open Drone Map"));
    outputTypeORBSLM2 = new QRadioButton(tr("ORB SLAM 2"));

    captureBtn = new QPushButton(tr("Run Capture"));
    playSegmentVidBtn = new QPushButton(tr("Play Segment"));
    exportCameraIntrBtn = new QPushButton(tr("Export Camera Intrinsics"));

}
void MainWindow::layoutControls()
{


    //CalibTab

    QFormLayout* formLayout2 = new QFormLayout();
    formLayout2->addRow(videoFileBtn, videoFileTxt);
    formLayout2->addRow(ptWidthLbl, ptWidthTxt);
    formLayout2->addRow(ptHeightLbl, ptHeightTxt);
    formLayout2->addRow(skipFramesLbl, skipFramesTxt);
    formLayout2->addRow(calibNumFramesLbl, calibNumFramesTxt);
    formLayout2->addRow(frameRateLbl, frameRateTxt);

    QVBoxLayout* pTypeLayout = new QVBoxLayout();
    pTypeLayout->addWidget(pTypeCircles);
    pTypeLayout->addWidget(pTypeAsymCircles);
    pTypeLayout->addWidget(pTypeChess);

    QGroupBox* pointTypeGroup = new QGroupBox(tr("Point Type"));
    pointTypeGroup->setLayout(pTypeLayout);

    QHBoxLayout* buttonLayout2 = new QHBoxLayout();
    buttonLayout2->addWidget(calibBtn);

    QGridLayout* mainLayout2 = new QGridLayout;
    mainLayout2->addLayout(formLayout2, 0, 0);
    mainLayout2->addWidget(pointTypeGroup, 1, 0);
    mainLayout2->addLayout(buttonLayout2, 2, 0);

    calibTab->setLayout(mainLayout2);

    //CaptureTab
    QFormLayout* formLayout3 = new QFormLayout();
    formLayout3->addRow(outputPathBtn, outputPathTxt);
    formLayout3->addRow(outputLbl, outputTxt);
    formLayout3->addRow(selectVidBtn, segmentVideoTxt);
    formLayout3->addRow(selectCSVBtn, csvTxt);
    formLayout3->addRow(outputPathBtn, outputPathTxt);
    formLayout3->addRow(outputLbl, outputTxt);
    formLayout3->addRow(selectIntrinsicsOutputBtn, selectIntrinsicsOutputTxt);
    formLayout3->addRow(startIdxLbl, startIdxTxt);
    formLayout3->addRow(endIdxLbl, endIdxTxt);
    formLayout3->addRow(startFrameLbl, playStartFrameTxt);
    formLayout3->addRow(stepLenLbl, stepLenTxt);
    formLayout3->addRow(playLengthLbl, playLengthTxt);
    formLayout3->addRow(captureNumFramesLbl, captureNumFramesTxt);
    formLayout3->addRow(focalLenLbl, focalLenTxt);


    QVBoxLayout* outputTypeLayout = new QVBoxLayout();
    outputTypeLayout->addWidget(outputTypeOSFM);
    outputTypeLayout->addWidget(outputTypeODM);
    outputTypeLayout->addWidget(outputTypeORBSLM2);

    QGroupBox* outputTypeGroup = new QGroupBox(tr("Output Type"));
    outputTypeGroup->setLayout(outputTypeLayout);

    QHBoxLayout* buttonLayout3 = new QHBoxLayout();
    buttonLayout3->addWidget(captureBtn);
    buttonLayout3->addWidget(playSegmentVidBtn);
    buttonLayout3->addWidget(exportCameraIntrBtn);

    QGridLayout* mainLayout3 = new QGridLayout;
    mainLayout3->addLayout(formLayout3, 0, 0);
    mainLayout3->addWidget(outputTypeGroup, 1, 0);
    mainLayout3->addLayout(buttonLayout3, 2, 0);

    captureTab->setLayout(mainLayout3);

    //Display Tab

    QGridLayout* vidLayout = new QGridLayout;


    graphicsView->setScene(new QGraphicsScene(this));
    graphicsView->scene()->addItem(&pixmap);

    vidLayout->addWidget(graphicsView);
    displayTab->setLayout(vidLayout);

    // Main


    QHBoxLayout* toolBar = new QHBoxLayout();
    toolBar->addWidget(loadBtn);
    toolBar->addWidget(saveBtn);
    toolBar->addWidget(saveAsBtn);

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(curFileLbl, curFileTxt);
    formLayout->addRow(imageScaleLbl, imageScaleTxt);
    formLayout->addRow(camIntrinsicsFileBtn, camIntrinsicsFileTxt);

    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(calibTab, tr("Calibration"));
    tabWidget->addTab(captureTab, tr("Capture"));
    tabWidget->addTab(displayTab, tr("Video"));

    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addLayout(toolBar, 1, 0);
    mainLayout->addLayout(formLayout, 2, 0);
    mainLayout->addWidget(tabWidget, 3, 0);


    QWidget *window = new QWidget();
            window->setLayout(mainLayout);

    setCentralWidget(window);
}
void MainWindow::connectControls()
{
    connect(saveBtn, SIGNAL(clicked(bool)), this, SLOT(save()));
    connect(saveAsBtn, SIGNAL(clicked(bool)), this, SLOT(saveAs()));
    connect(loadBtn, SIGNAL(clicked(bool)), this, SLOT(load()));
    connect(camIntrinsicsFileBtn, SIGNAL(clicked(bool)), this, SLOT(selectIntrinsicsFile()));
    //calib
    connect(videoFileBtn, SIGNAL(clicked(bool)), this, SLOT(selectVidFile()));
    connect(calibBtn, SIGNAL(clicked(bool)), this, SLOT(captureAndCalibrate()));
    //capture
    connect(playSegmentVidBtn, SIGNAL(clicked(bool)), this, SLOT(onPlaySegmentVidBtnClicked()));
    connect(captureBtn, SIGNAL(clicked(bool)), this, SLOT(onCaptureBtnClicked()));
    connect(selectCSVBtn, SIGNAL(clicked(bool)), this, SLOT(onSelectCSVBtnClicked()));
    connect(outputPathBtn, SIGNAL(clicked(bool)), this, SLOT(onOutputPathBtnClicked()));
    connect(selectIntrinsicsOutputBtn, SIGNAL(clicked(bool)), this, SLOT(onIntrinsicsOutputBtnClicked()));
    connect(selectVidBtn, SIGNAL(clicked(bool)), this, SLOT(onSelectVidBtnClicked()));
    connect(exportCameraIntrBtn, SIGNAL(clicked(bool)), this, SLOT(onExportCameraIntrBtnClicked()));
}

QString MainWindow::getImageScaleTxt()
{
    return imageScaleTxt->text();
}

QString MainWindow::getCameraIntrinsicsTxt()
{
    return camIntrinsicsFileTxt->text();
}

Mat MainWindow::getCameraMatrix()
{
    return cameraMatrix;
}
Mat MainWindow::getDistCoeffs()
{
    return distCoeffs;
}

void MainWindow::setCameraMatrix(Mat mat)
{
    this->cameraMatrix = mat;
}
void MainWindow::setDistCoeffs(Mat mat)
{
    this->distCoeffs = mat;
}

void MainWindow::readCameraParams(const string& filename)
{
    FileStorage fs( filename, FileStorage::READ );
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
}

void MainWindow::selectVidFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video"), "./", tr("All files (*.*)"));
    if(fileName.isEmpty())
    {
        return;
    }
    videoFileTxt->setText(fileName);
}

//calibrate
void MainWindow::captureAndCalibrate()
{
    if(!setPatternType())
    {
        return;
    }

    Size boardSize, imageSize;
    float squareSize, aspectRatio;
    Mat cameraMatrix, distCoeffs, frame;
    vector<vector<Point2f> > imagePoints;
    vector<string> imageList;
    bool writeExtrinsics, writePoints;

    boardSize.width = ptWidthTxt->text().toInt();
    boardSize.height = ptHeightTxt->text().toInt();

    squareSize = 1;
    aspectRatio = 1;
    writePoints = false;
    writeExtrinsics = false;
    int flags = 2;

    capture.open(videoFileTxt->text().toStdString());
    if(capture.isOpened())
    {
        capture.set(CAP_PROP_FPS, frameRateTxt->text().toDouble());
    }

    double resize = imageScaleTxt->text().toDouble();

    int numFrames = calibNumFramesTxt->text().toInt();
    int skipFrames = skipFramesTxt->text().toInt();
    QString tempPath = "./tmp";

    QString tempOutputFolder = utilities::nextDirName(QString(TEMP_OUTPUT_PATH));
    QDir().mkdir(tempPath);
    QDir().mkdir(tempOutputFolder);
    QDir().mkdir(tempOutputFolder + "/images");


    ofstream out(tempOutputFolder.toStdString() + "/images.xml");
            out << "<?xml version=\"1.0\"?>" << endl
                << "<opencv_storage>" << endl
                << "<images>" << endl;

    for(int i = 0;; i++)
    {
        Mat view, viewGray;

        if( capture.isOpened() )
        {
            Mat view0;
            capture.grab();
            if(i % skipFrames != 0)
                continue;
            capture.retrieve( view0 );
            view0.copyTo(view);
        }

        if(!view.empty())
        {
            cv::resize(view, frame, cv::Size(), resize, resize);
            std::string outputImgeName = tempOutputFolder.toStdString() + "/images/image" + std::to_string(i) + ".jpg";
            cv::imwrite(outputImgeName, frame);
            out << outputImgeName << endl;
        } else
        {
            break;
        }

    }
    out << "</images>" << endl <<
           "</opencv_storage>";
    out.close();

    utilities::readStringList(tempOutputFolder.toStdString() + "/images.xml", imageList);
    numFrames = (int)imageList.size();

    for(int i = 0;;i++)
    {
        Mat view, viewGray;


        if( i < (int)imageList.size() )
            view = imread(imageList[i], 1);

        if(view.empty())
        {
            if( imagePoints.size() > 0 )
                utilities::runAndSave(camIntrinsicsFileTxt->text().toStdString(),
                   imagePoints, imageSize,
                   boardSize, selPattern, squareSize, aspectRatio,
                   flags, cameraMatrix, distCoeffs,
                   writeExtrinsics, writePoints);
            break;
        }

        imageSize = view.size();

//        if( flipVertical )
//            flip( view, view, 0 );

        vector<Point2f> pointbuf;
        cvtColor(view, viewGray, COLOR_BGR2GRAY);

        bool found;
        switch( selPattern )
        {
            case CHESSBOARD:
                found = findChessboardCorners( view, boardSize, pointbuf,
                    CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
                break;
            case CIRCLES_GRID:
                found = findCirclesGrid( view, boardSize, pointbuf );
                break;
            case ASYMMETRIC_CIRCLES_GRID:
                found = findCirclesGrid( view, boardSize, pointbuf, CALIB_CB_ASYMMETRIC_GRID );
                break;
            default:
                return;
        }

       // improve the found corners' coordinate accuracy
        if( selPattern == CHESSBOARD && found)
            cornerSubPix( viewGray, pointbuf, Size(11,11), Size(-1,-1),
                TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));

        if( found )
        {
            imagePoints.push_back(pointbuf);
            drawChessboardCorners( view, boardSize, Mat(pointbuf), found );
        }
        if(imagePoints.size() >= (unsigned)numFrames)
        {
            utilities::runAndSave(camIntrinsicsFileTxt->text().toStdString(),
               imagePoints, imageSize,
               boardSize, selPattern, squareSize, aspectRatio,
               flags, cameraMatrix, distCoeffs,
               writeExtrinsics, writePoints);

            break;
        }
    }
}





bool MainWindow::setPatternType()
{

    if(pTypeCircles->isChecked())
    {
        selPattern = PatternType::CIRCLES_GRID;
        return true;
    }
    else if (pTypeAsymCircles->isChecked())
    {
        selPattern = PatternType::ASYMMETRIC_CIRCLES_GRID;
        return true;
    }
    else if(pTypeChess->isChecked())
    {
        selPattern = PatternType::CHESSBOARD;
        return true;
    }
    return false;
}



vector<ExifData*>::size_type MainWindow::findExifData(long start, vector<ExifData*>::size_type csvIdx, int frameIdx)
{
    long elapsedMillis = (frameIdx * 1000)/60;
    long curTime = start + elapsedMillis;

    if(csvIdx > 0)
    {
        --csvIdx;
    }

    for(vector<ExifData*>::size_type i = csvIdx; i < csvData->size() - 1; ++i)
    {

        long upperTime = csvData->at(i + 1)->atTime;
        long lowerTime = csvData->at(i)->atTime;

        if( curTime < upperTime && curTime >= lowerTime )
        {
            long diffLower = curTime - lowerTime;
            long diffUpper = upperTime - curTime;
            if(diffLower < diffUpper )
            {
                return i;
            }
            else
            {
                return i + 1;
            }
            break;
        }
    }

    return 0;
}

void MainWindow::processVid()
{

    m_csvidx = findExifData(m_time1, m_csvidx, m_videoIdx);

    capWebcam.read(original);

    putText(original,
                "Alt: " + std::to_string( csvData->at(m_csvidx)->altitude),
                Point(5,100), FONT_HERSHEY_DUPLEX, 1, Scalar(0,143,143), 2);

    putText(original,
                "Lat: " + std::to_string( csvData->at(m_csvidx)->latitude),
                Point(5,150), FONT_HERSHEY_DUPLEX, 1, Scalar(0,143,143), 2);

    putText(original,
                "Lon: " + std::to_string(csvData->at(m_csvidx)->longitude),
                Point(5,200), FONT_HERSHEY_DUPLEX, 1, Scalar(0,143,143), 2);

    putText(original,
                "Relative Alt: " + std::to_string(csvData->at(m_csvidx)->relAlt),
                Point(5,250), FONT_HERSHEY_DUPLEX, 1, Scalar(0,143,143), 2);

    putText(original,
                "vx: " + std::to_string(csvData->at(m_csvidx)->vx) + " vy: " +
                std::to_string(csvData->at(m_csvidx)->vy) + "vz: " + std::to_string(csvData->at(m_csvidx)->vz),
                Point(5,300), FONT_HERSHEY_DUPLEX, 1, Scalar(0,143,143), 2);

    if(original.empty() == true) return;
    //cv::resize(original, original, cv::Size(originalLbl->width(),originalLbl->height()));
    cv::cvtColor(original, originalColor, cv::COLOR_BGR2RGB);
    QImage qimgProcessed2((uchar*)originalColor.data, originalColor.cols, originalColor.rows, originalColor.step, QImage::Format_RGB888);
    //originalLbl->setPixmap(QPixmap::fromImage(qimgProcessed2));

    //QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    pixmap.setPixmap( QPixmap::fromImage(qimgProcessed2));
    graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);

    ++m_videoIdx;
    qApp->processEvents();
}

void MainWindow::onIntrinsicsOutputBtnClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Output Intrinsics File"),
                               "./",
                               tr("Intrinsics (*.*)"));
    selectIntrinsicsOutputTxt->setText(fileName);
}

void MainWindow::onOutputPathBtnClicked()
{
    QString tempPath = QFileDialog::getExistingDirectory(this, ("Select Output Folder"), QDir::currentPath());
    if(!tempPath.isEmpty())
    {
        outputPathTxt->setText(tempPath);
    }
}
void MainWindow::onSelectVidBtnClicked()
{
    vidFile = QFileDialog::getOpenFileName(this, tr("Open Video"), "./", tr("All files (*.*)"));
    segmentVideoTxt->setText(vidFile);

}
void MainWindow::onSelectCSVBtnClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open CSV"), "./", tr("CSV files (*.csv)"));
    if(!filename.isEmpty())
    {
        csvTxt->setText(filename);
    }
}

void MainWindow::onExportCameraIntrBtnClicked()
{
    if(!setCapType())
    {
        return;
    }
    deserializeCameraMats();
    if(selCapType == CaptureType::ORBSLAM2)
    {
        utilities::saveORBSlamYaml(selectIntrinsicsOutputTxt->text().toStdString(), cameraMatrix, distCoeffs);
    }
//    else if(selCapType == CaptureType::OPENSFM)
//    {
//        ExifData::exportOverridesFile(newDir.toStdString(), 2704, 1520, cameraMatrix, distCoeffs,  stod(focalLenTxt->text().toStdString()));
//    }
}

void MainWindow::processCSV()
{
    csvData = new vector<ExifData*>();

    std::ifstream myfile(csvTxt->text().toStdString());
    int entryStart = atoi(startIdxTxt->text().toStdString().c_str());
    int entryEnd = atoi(endIdxTxt->text().toStdString().c_str());
    int entryCount = 0;
    int lineCount = 0;
    if (myfile.is_open())
    {
        std::string line;
        while (std::getline(myfile, line))
        {
            ++lineCount;
            typedef boost::escaped_list_separator<char> Separator;
            typedef boost::tokenizer<Separator> Tokenizer;

            bool validLine = false;
            Tokenizer tokenizer(line);
            int count = 0;
            for (Tokenizer::iterator iter = tokenizer.begin(); (iter != tokenizer.end()) && (count <= 9); ++iter)
            {
                if(count == 9 && *iter == "mavlink_global_position_int_t")
                {
                    validLine = true;
                    break;
                }
                ++count;
            }
            if(validLine == false)
            {
                continue;
            }

            long atTime;
            int width = 2704;
            int height = 1520;
            double focal_ratio = stod(focalLenTxt->text().toStdString());
            double altitude;
            double latitude;
            double longitude;
            int relAlt, vx, vy, vz;

            ++entryCount;
            if(entryCount < entryStart)
            {
                continue;
            }
            if( entryCount > entryEnd)
            {
                break;
            }
            count = 0;

            for (Tokenizer::iterator iter = tokenizer.begin(); (iter != tokenizer.end()) && (count <= 25); ++iter)
            {
                if(count == 0 || count == 13 || count == 15 || count == 17 || count == 19 || count == 21 || count == 23 || count == 25)
                {
                    switch(count)
                    {
                        case 0:
                            // time
                            atTime = utilities::seconds_from_epoch(*iter);
                            break;
                        case 13:
                            // lat
                            latitude = stod(*iter) / 10000;
                            break;
                        case 15:
                            // lon
                            longitude = stod(*iter) / 10000;
                            break;
                        case 17:
                            // alt
                            altitude = stod(*iter);
                            break;
                        case 19:
                            relAlt = stoi(*iter);
                            break;
                        case 21:
                            vx = stoi(*iter);
                            break;
                        case 23:
                            vy = stoi(*iter);
                            break;
                        case 25:
                            vz = stoi(*iter);
                            break;
                    }
                }
                ++count;
            }
            csvData->push_back(new ExifData(atTime, width, height, focal_ratio,  altitude, latitude, longitude, relAlt, vx, vy, vz));
        }
    }
}
void MainWindow::onCaptureBtnClicked()
{
    processCSV();
    deserializeCameraMats();

    capWebcam.open(segmentVideoTxt->text().toStdString());
    int step = atoi(stepLenTxt->text().toStdString().c_str());

    long long int now = static_cast<long long int>(std::time(nullptr)) * MICRO;
    double fps = capWebcam.get(cv::CAP_PROP_FPS);
    double microsperstep = (MICRO / fps) * step;

    if(!setCapType())
    {
        return;
    }

    outputPath = outputPathTxt->text();

    QString newDir = utilities::nextDirName(outputPath);
    QDir().mkdir(newDir);
    if(selCapType == CaptureType::OPENSFM)
    {
        copyResources(newDir.toStdString());
        ExifData::exportOverridesFile(newDir.toStdString(), 2704, 1520, cameraMatrix, distCoeffs,  stod(focalLenTxt->text().toStdString()));
    }
    if((selCapType == CaptureType::OPENSFM) || (selCapType == CaptureType::OPENDRONEMAP))
    {
        QDir().mkdir(newDir + "/images");
        if(selCapType == CaptureType::OPENSFM)
        {
            QDir().mkdir(newDir + "/exif");
        }
    }
    if(selCapType == CaptureType::ORBSLAM2)
    {
        QDir().mkdir(newDir + "/rgb");
    }

    double resize = 1;
    std::ofstream out;
    if(selCapType == CaptureType::ORBSLAM2)
    {
        out = std::ofstream(newDir.toStdString() + "/rgb.txt");
        resize = imageScaleTxt->text().toDouble();
    }

    int j = 1, videoIdx = 0, errcount = 0, errmax = 100;
    typedef vector<ExifData*>::size_type  vecExif;
    vecExif csvidx = static_cast<vecExif>(atoi(this->startIdxTxt->text().toStdString().c_str()));
    vecExif csvEnd = static_cast<vecExif>(atoi(this->endIdxTxt->text().toStdString().c_str()));
    int fameLimit = atoi(this->captureNumFramesTxt->text().toStdString().c_str());
    Mat frame, frame2;

    int frameIdx = atoi(playStartFrameTxt->text().toStdString().c_str());

    long time1 = 0;
    if((selCapType == CaptureType::OPENSFM) || (selCapType == CaptureType::OPENDRONEMAP))
    {
        time1 = csvData->at(csvidx)->atTime;
    }

    capWebcam.set(cv::CAP_PROP_POS_FRAMES, frameIdx);

    while( j <= fameLimit)
    {
        capWebcam >> frame;
        if(frame.empty())
        {
            ++errcount;
            if( errcount >= errmax )
            {
                break;
            }
        }
        else
        {
            errcount = 0;
            if((selCapType == CaptureType::OPENSFM) || (selCapType == CaptureType::OPENDRONEMAP))
            {
                csvidx = findExifData(time1, csvidx, videoIdx);

                QString fname = utilities::getFileName(newDir + "/images", j);
                imwrite(cv::String(fname.toStdString()),frame);

                if(selCapType == CaptureType::OPENDRONEMAP)
                {
                    csvData->at(csvidx)->writeExifToImage(fname.toStdString());
                }

                if(selCapType == CaptureType::OPENSFM)
                {
                    std::string fname2(cv::String(utilities::getFileName(newDir + "/exif", j).toStdString()));
                    csvData->at(csvidx)->exportToFile(fname2);
                }
            }




            if(selCapType == CaptureType::ORBSLAM2)
            {
                long long int secNow = now / MICRO;
                int microNow = now % MICRO;
                ostringstream out2;
                out2 << std::internal << std::setfill('0') << std::setw(6) << std::to_string(microNow);

                string strNow = to_string(secNow) + "." + out2.str();

                cv::resize(frame, frame2, cv::Size(), resize, resize);

                imwrite(newDir.toStdString() + "/rgb/" + strNow + ".png",frame2);
                out << strNow << " rgb/" << strNow << ".png" << std::endl;
            }
        }

        if((selCapType == CaptureType::OPENSFM
                  || selCapType == CaptureType::OPENDRONEMAP)
                 && (csvidx >= csvEnd))
        {
            break;
        }


        videoIdx += step;
        frameIdx += step;
        now += microsperstep;
        ++j;
        capWebcam.set(cv::CAP_PROP_POS_FRAMES, frameIdx);
    }
    out.close();
    if(selCapType == CaptureType::ORBSLAM2)
    {

        utilities::saveORBSlamYaml(selectIntrinsicsOutputTxt->text().toStdString(), cameraMatrix, distCoeffs);
    }
    QMessageBox msgBox;
    msgBox.setText("Completed capturing of: " + newDir);
    msgBox.exec();
}


void MainWindow::onPlaySegmentVidBtnClicked()
{
    processCSV();

    m_csvidx = 0;
    m_videoIdx = 0;

    m_time1 = csvData->at(m_csvidx)->atTime;


    capWebcam = cv::VideoCapture(segmentVideoTxt->text().toStdString());

    double fps = capWebcam.get(cv::CAP_PROP_FPS);

    capWebcam.set(cv::CAP_PROP_POS_FRAMES, stod(playStartFrameTxt->text().toStdString()));
    /* iterate through first 10 frames */

    tmrTimer = new QTimer(this);
    connect(tmrTimer, SIGNAL(timeout()), this, SLOT(processVid()));
    tmrTimer->start(1000 / fps);

    QTimer::singleShot( atoi(playLengthTxt->text().toStdString().c_str()) * 1000, this, SLOT(quitPreview()));
}

void MainWindow::quitPreview()
{

    tmrTimer->setSingleShot(true);
    //originalLbl->clear();
}

bool MainWindow::setCapType()
{

    if(outputTypeOSFM->isChecked())
    {
        selCapType = CaptureType::OPENSFM;
        return true;
    }
    else if (outputTypeODM->isChecked())
    {
        selCapType = CaptureType::OPENDRONEMAP;
        return true;
    }
    else if(outputTypeORBSLM2->isChecked())
    {
        selCapType = CaptureType::ORBSLAM2;
        return true;
    }
    return false;
}

void MainWindow::setOutputType(int outputtype)
{
    switch(outputtype)
    {
    case 1:
        outputTypeOSFM->setChecked(true);
        break;
    case 2:
        outputTypeODM->setChecked(true);
        break;
    case 3:
        outputTypeORBSLM2->setChecked(true);
        break;
    }
}

int MainWindow::getIntOutputType()
{
    if(outputTypeOSFM->isChecked())
    {
        return 1;
    } else if(outputTypeODM->isChecked() )
    {
        return 2;
    } else if (outputTypeORBSLM2->isChecked())
    {
        return 3;
    }
    return 0;
}


int MainWindow::getIntPType()
{
    if(pTypeCircles->isChecked())
    {
        return 1;
    } else if(pTypeAsymCircles->isChecked() )
    {
        return 2;
    } else if (pTypeChess->isChecked())
    {
        return 3;
    }
    return 0;
}

void MainWindow::setPType(int ptype)
{
    switch(ptype)
    {
    case 1:
        pTypeCircles->setChecked(true);
        break;
    case 2:
        pTypeAsymCircles->setChecked(true);
        break;
    case 3:
        pTypeChess->setChecked(true);
        break;
    }
}

void MainWindow::copyResources(std::string outputPath)
{
    for(int i = 0; i < osfmResources.size(); ++i)
    {
        std::ifstream  src(recPath + osfmResources.at(i), std::ios::binary);
        std::ofstream  dst(outputPath + osfmResources.at(i) ,   std::ios::binary);
        dst << src.rdbuf();
    }
}

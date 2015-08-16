RedBallTrackerQt

this is not an actual C++ file !!!

this file is 3 files in one:
-the Qt project file (.pro)
-the main form .h file (ex frmmain.h)
-the main form .cpp file (ex frmmain.cpp)

follow the video to create the project, place widgets on your form,
and have Qt Creator write as much of the code for you as possible,
then copy/paste ONLY THE ADDITIONAL PORTIONS from the code below:

for this program the widgets you need to add are:

lblOriginal (QLabel)
lblProcessed (QLabel)
btnPauseOrResume (QPushButton)
txtXYRadius (QPlainTextEdit)

####################################################################################################
## RedBallTrackerQt.pro (1 of 3) ###################################################################
####################################################################################################

#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T13:55:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtCannyStill1
TEMPLATE = app


SOURCES += main.cpp\
        frmmain.cpp

HEADERS  += frmmain.h

FORMS    += frmmain.ui

INCLUDEPATH += C:\\OpenCV-2.4.11\\opencv\\build\\include

LIBS += -LC:\\OpenCV-2.4.11\\mybuild\\lib\\Debug \
    -lopencv_calib3d2411d \
    -lopencv_contrib2411d \
    -lopencv_core2411d \
    -lopencv_features2d2411d \
    -lopencv_flann2411d \
    -lopencv_gpu2411d \
    -lopencv_haartraining_engined \
    -lopencv_highgui2411d \
    -lopencv_imgproc2411d \
    -lopencv_legacy2411d \
    -lopencv_ml2411d \
    -lopencv_nonfree2411d \
    -lopencv_objdetect2411d \
    -lopencv_ocl2411d \
    -lopencv_photo2411d \
    -lopencv_stitching2411d \
    -lopencv_superres2411d \
    -lopencv_ts2411d \
    -lopencv_video2411d \
    -lopencv_videostab2411d

# Note: it is recommended to leave a blank line at the end of your .pro file

///////////////////////////////////////////////////////////////////////////////////////////////////
// frmmain.h (2 of 3) /////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace Ui {
    class frmMain;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
class frmMain : public QMainWindow {
    Q_OBJECT

public slots:
    void processFrameAndUpdateGUI();                // function prototype

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private slots:
    void on_btnPauseOrResume_clicked();

private:
    Ui::frmMain *ui;

    cv::VideoCapture capWebcam;             // Capture object to use with webcam
    cv::Mat matOriginal;                    // original image
    cv::Mat matProcessed;                   // processed image

    QTimer* qtimer;                 // timer for processFrameAndUpdateGUI()

    QImage frmMain::matToQImage(cv::Mat mat);       // function prototype

    void frmMain::exitProgram();                    // function prototype
};

#endif // FRMMAIN_H


///////////////////////////////////////////////////////////////////////////////////////////////////
// frmmain.cpp (3 of 3) ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "frmmain.h"
#include "ui_frmmain.h"

#include<QtCore>
#include<QMessageBox>

// constructor ////////////////////////////////////////////////////////////////////////////////////
frmMain::frmMain(QWidget *parent) : QMainWindow(parent), ui(new Ui::frmMain) {
    ui->setupUi(this);

    capWebcam.open(0);                  // associate the capture object to the default webcam

    if(capWebcam.isOpened() == false) {                 // if unsuccessful
        QMessageBox::information(this, "", "error: capWebcam not accessed successfully \n\n exiting program\n");        // show error message
        exitProgram();                                  // and exit program
        return;                                         //
    }

    qtimer = new QTimer(this);                          // instantiate timer
    connect(qtimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));     // associate timer to processFrameAndUpdateGUI
    qtimer->start(20);                                  // start timer, set to cycle every 20 msec (50x per sec), it will not actually cycle this often
}

// destructor /////////////////////////////////////////////////////////////////////////////////////
frmMain::~frmMain() {
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void frmMain::exitProgram() {
    if(qtimer->isActive()) qtimer->stop();          // if timer is running, stop timer
    QApplication::quit();                           // and exit program
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void frmMain::processFrameAndUpdateGUI() {
    bool blnFrameReadSuccessfully = capWebcam.read(matOriginal);                    // get next frame from the webcam

    if (!blnFrameReadSuccessfully || matOriginal.empty()) {                            // if we did not get a frame
        QMessageBox::information(this, "", "unable to read from webcam \n\n exiting program\n");        // show error via message box
        exitProgram();                                                              // and exit program
        return;                                                                     //
    }

    cv::GaussianBlur(matOriginal, matProcessed, cv::Size(5, 5), 2.0);

    cv::inRange(matProcessed, cv::Scalar(0, 0, 175), cv::Scalar(100, 100, 256), matProcessed);

    cv::GaussianBlur(matProcessed, matProcessed, cv::Size(5, 5), 2.0);

    cv::dilate(matProcessed, matProcessed, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));		// close image (dilate, then erode)
    cv::erode(matProcessed, matProcessed, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));		// closing "closes" (i.e. fills in) foreground gaps

    std::vector<cv::Vec3f> v3fCircles;                                                                              // declare circles variable
    cv::HoughCircles(matProcessed, v3fCircles, CV_HOUGH_GRADIENT, 2, matProcessed.rows / 4, 100, 50, 10, 400);      // fill variable circles with all circles in the processed image

    for(unsigned int i = 0; i < v3fCircles.size(); i++) {                                                                                       // for each circle
        ui->txtXYRadius->appendPlainText(QString("ball position x =") + QString::number(v3fCircles[i][0]).rightJustified(4, ' ') +              // print ball position and radius
                                                                         QString(", y =") + QString::number(v3fCircles[i][1]).rightJustified(4, ' ') +
                                                                         QString(", radius =") + QString::number(v3fCircles[i][2], 'f', 3).rightJustified(7, ' '));

        cv::circle(matOriginal, cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]), 3, cv::Scalar(0, 255, 0), CV_FILLED);                  // draw small green circle at center of detected object
        cv::circle(matOriginal, cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]), (int)v3fCircles[i][2], cv::Scalar(0, 0, 255), 3);      // draw red circle around the detected object
    }

    QImage qimgOriginal = matToQImage(matOriginal);                         // convert from OpenCV Mat to Qt QImage
    QImage qimgProcessed = matToQImage(matProcessed);                       //

    ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));           // show images on form labels
    ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgProcessed));         //
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QImage frmMain::matToQImage(cv::Mat mat) {
    if(mat.channels() == 1) {                                   // if 1 channel (grayscale or black and white) image
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);     // return QImage
    } else if(mat.channels() == 3) {                            // if 3 channel color image
        cv::cvtColor(mat, mat, CV_BGR2RGB);                     // flip colors
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);       // return QImage
    } else {
        qDebug() << "in openCVMatToQImage, image was not 1 channel or 3 channel, should never get here";
    }
    return QImage();        // return a blank QImage if the above did not work
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void frmMain::on_btnPauseOrResume_clicked() {
    if(qtimer->isActive() == true) {                // if timer is running we are currently processing an image, so . . .
        qtimer->stop();                                 // stop timer
        ui->btnPauseOrResume->setText(" resume ");      // and update button text
    } else {                                        // else timer is not running, so we are currently paused, so . . .
        qtimer->start(20);                              // start timer again
        ui->btnPauseOrResume->setText(" pause ");       // and update button text
    }
}

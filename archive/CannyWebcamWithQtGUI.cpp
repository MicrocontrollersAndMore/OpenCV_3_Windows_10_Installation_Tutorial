CannyWebcamQt

this is not an actual C++ file !!!

this file is 2 files in one:
-the main form .h file (ex frmmain.h)
-the main form .cpp file (ex frmmain.cpp)

follow the video to create the project, edit the .pro file, place widgets on your form,
and have Qt Creator write as much of the code for you as possible,
then copy/paste ONLY THE ADDITIONAL PORTIONS from the code below:

for this program the widgets you need to add are:

lblOriginal (QLabel)
lblCanny (QLabel)

///////////////////////////////////////////////////////////////////////////////////////////////////
// frmmain.h (1 of 2) /////////////////////////////////////////////////////////////////////////////
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
    void processFrameAndUpdateGUI();            // function prototype

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private:
    Ui::frmMain *ui;

    cv::VideoCapture capWebcam;         // Capture object to use with webcam

    QTimer* qtimer;             // timer for processFrameAndUpdateGUI()

    QImage frmMain::convertOpenCVMatToQtQImage(cv::Mat mat);       // function prototype

    void frmMain::exitProgram();                    // function prototype
};

#endif // FRMMAIN_H


///////////////////////////////////////////////////////////////////////////////////////////////////
// frmmain.cpp (2 of 2) ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "frmmain.h"
#include "ui_frmmain.h"

#include<QtCore>
#include<QMessageBox>

// constructor ////////////////////////////////////////////////////////////////////////////////////
frmMain::frmMain(QWidget *parent) : QMainWindow(parent), ui(new Ui::frmMain) {
    ui->setupUi(this);

    capWebcam.open(0);              // associate the capture object to the default webcam

    if(capWebcam.isOpened() == false) {                 // if unsuccessful
        QMessageBox::information(this, "", "error: capWebcam not accessed successfully \n\n exiting program\n");        // show error message
        exitProgram();                          // and exit program
        return;                                 //
    }

    qtimer = new QTimer(this);                                                      // instantiate timer
    connect(qtimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));     // associate timer to processFrameAndUpdateGUI
    qtimer->start(20);                  // start timer, set to cycle every 20 msec (50x per sec), it will not actually cycle this often
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
    cv::Mat imgOriginal;                // input image
    cv::Mat imgGrayscale;               // grayscale of input image
    cv::Mat imgBlurred;                 // intermediate blured image
    cv::Mat imgCanny;                   // Canny edge image

    bool blnFrameReadSuccessfully = capWebcam.read(imgOriginal);                    // get next frame from the webcam

    if (!blnFrameReadSuccessfully || imgOriginal.empty()) {                            // if we did not get a frame
        QMessageBox::information(this, "", "unable to read from webcam \n\n exiting program\n");        // show error via message box
        exitProgram();                                                                              // and exit program
        return;                                                                                     //
    }

    cv::cvtColor(imgOriginal, imgGrayscale, CV_BGR2GRAY);		// convert to grayscale

    cv::GaussianBlur(imgGrayscale, imgBlurred, cv::Size(5, 5), 1.8);     // blur

    cv::Canny(imgBlurred, imgCanny, 50, 100);                           // get Canny edges

    QImage qimgOriginal = convertOpenCVMatToQtQImage(imgOriginal);             // convert from OpenCV Mat to Qt QImage
    QImage qimgCanny = convertOpenCVMatToQtQImage(imgCanny);                   //

    ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));       // show images on form labels
    ui->lblCanny->setPixmap(QPixmap::fromImage(qimgCanny));             //
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QImage frmMain::convertOpenCVMatToQtQImage(cv::Mat mat) {
    if(mat.channels() == 1) {                   // if grayscale image
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);     // declare and return a QImage
    } else if(mat.channels() == 3) {            // if 3 channel color image
        cv::cvtColor(mat, mat, CV_BGR2RGB);     // invert BGR to RGB
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);       // declare and return a QImage
    } else {
        qDebug() << "in convertOpenCVMatToQtQImage, image was not 1 channel or 3 channel, should never get here";
    }
    return QImage();        // return a blank QImage if the above did not work
}

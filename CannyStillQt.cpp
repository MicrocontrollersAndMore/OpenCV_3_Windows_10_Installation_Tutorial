CannyStillQt

this is not an actual C++ file !!!

this file is 3 files in one:
-the Qt project file (.pro)
-the main form .h file (ex frmmain.h)
-the main form .cpp file (ex frmmain.cpp)

follow the video to create the project, place widgets on your form,
and have Qt Creator write as much of the code for you as possible,
then copy/paste ONLY THE ADDITIONAL PORTIONS from the code below:

for this program the widgets you need to add are:

btnOpenFile (QPushButton)
lblChosenFile (QLabel)
lblOriginal (QLabel)
lblCanny (QLabel)

####################################################################################################
## CannyStillQt.pro (1 of 3) #######################################################################
####################################################################################################

#-------------------------------------------------
#
# Project created by QtCreator 2015-05-25T01:38:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CannyStillQt1
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

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private slots:
    void on_btnOpenFile_clicked();

private:
    Ui::frmMain *ui;

    cv::Mat matOriginal;            // input image
    cv::Mat matGrayscale;           // grayscale of input image
    cv::Mat matBlurred;             // intermediate blured image
    cv::Mat matCanny;               // Canny edge image

    QImage frmMain::matToQImage(cv::Mat mat);           // function prototype
};

#endif // FRMMAIN_H

///////////////////////////////////////////////////////////////////////////////////////////////////
// frmmain.cpp (3 of 3) ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "frmmain.h"
#include "ui_frmmain.h"

#include<QFileDialog>
#include<QtCore>

// constructor ////////////////////////////////////////////////////////////////////////////////////
frmMain::frmMain(QWidget *parent) : QMainWindow(parent), ui(new Ui::frmMain) {
    ui->setupUi(this);
}

// destructor /////////////////////////////////////////////////////////////////////////////////////
frmMain::~frmMain() {
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void frmMain::on_btnOpenFile_clicked() {
    QString strFileName = QFileDialog::getOpenFileName();       // bring up open file dialog

    if(strFileName == "") {                                     // if file was not chosen
        ui->lblChosenFile->setText("file not chosen");          // update label
        return;                                                 // and exit function
    }

    matOriginal = cv::imread(strFileName.toStdString());        // open image

    if (matOriginal.empty()) {									// if unable to open image
        ui->lblChosenFile->setText("error: image not read from file");      // update lable with error message
        return;                                                             // and exit function
    }
        // if we get to this point image was opened successfully
    ui->lblChosenFile->setText(strFileName);                // update label with file name

    cv::cvtColor(matOriginal, matGrayscale, CV_BGR2GRAY);               // convert to grayscale
    cv::GaussianBlur(matGrayscale, matBlurred, cv::Size(5, 5), 1.5);    // blur
    cv::Canny(matBlurred, matCanny, 100, 200);                          // get Canny edges

    QImage qimgOriginal = matToQImage(matOriginal);         // convert original and Canny images to QImage
    QImage qimgCanny = matToQImage(matCanny);               //

    ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));   // show original and Canny images on labels
    ui->lblCanny->setPixmap(QPixmap::fromImage(qimgCanny));         //
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QImage frmMain::matToQImage(cv::Mat mat) {
    if(mat.channels() == 1) {                   // if grayscale image
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);     // declare and return a QImage
    } else if(mat.channels() == 3) {            // if 3 channel color image
        cv::cvtColor(mat, mat, CV_BGR2RGB);     // invert BGR to RGB
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);       // declare and return a QImage
    } else {
        qDebug() << "in openCVMatToQImage, image was not 1 channel or 3 channel, should never get here";
    }
    return QImage();        // return a blank QImage if the above did not work
}

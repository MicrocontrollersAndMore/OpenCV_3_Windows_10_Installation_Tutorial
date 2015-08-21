CannyStillWithQtGUI

this is not an actual C++ file !!!

this file is 2 files in one:
-the main form .h file (ex frmmain.h)
-the main form .cpp file (ex frmmain.cpp)

follow the video to create the project, edit the .pro file, place widgets on your form,
and have Qt Creator write as much of the code for you as possible (especially any events),
then copy/paste ONLY THE ADDITIONAL PORTIONS from the code below:

for this program the widgets you need to add are:

btnOpenFile (QPushButton)
lblChosenFile (QLabel)
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

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private slots:
    void on_btnOpenFile_clicked();

private:
    Ui::frmMain *ui;

    QImage frmMain::convertOpenCVMatToQtQImage(cv::Mat mat);           // function prototype
};

#endif // FRMMAIN_H

///////////////////////////////////////////////////////////////////////////////////////////////////
// frmmain.cpp (2 of 2) ///////////////////////////////////////////////////////////////////////////
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

    cv::Mat imgOriginal;            // input image
    cv::Mat imgGrayscale;           // grayscale of input image
    cv::Mat imgBlurred;             // intermediate blured image
    cv::Mat imgCanny;               // Canny edge image

    imgOriginal = cv::imread(strFileName.toStdString());        // open image

    if (imgOriginal.empty()) {									// if unable to open image
        ui->lblChosenFile->setText("error: image not read from file");      // update lable with error message
        return;                                                             // and exit function
    }
        // if we get to this point image was opened successfully
    ui->lblChosenFile->setText(strFileName);                // update label with file name

    cv::cvtColor(imgOriginal, imgGrayscale, CV_BGR2GRAY);               // convert to grayscale
    cv::GaussianBlur(imgGrayscale, imgBlurred, cv::Size(5, 5), 1.5);    // blur
    cv::Canny(imgBlurred, imgCanny, 100, 200);                          // get Canny edges

    QImage qimgOriginal = convertOpenCVMatToQtQImage(imgOriginal);         // convert original and Canny images to QImage
    QImage qimgCanny = convertOpenCVMatToQtQImage(imgCanny);               //

    ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));   // show original and Canny images on labels
    ui->lblCanny->setPixmap(QPixmap::fromImage(qimgCanny));         //
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

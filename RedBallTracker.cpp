// RedBallTracker.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	cv::VideoCapture capWebcam(0);		// declare a VideoCapture object and associate to webcam, 0 => use 1st webcam

	if (capWebcam.isOpened() == false) {				// check if VideoCapture object was associated to webcam successfully
		std::cout << "error: capWebcam not accessed successfully\n\n";	// if not, print error message to std out
		return(0);														// and exit program
	}

	cv::Mat imgOriginal;		// input image
	cv::Mat imgHSV;
	cv::Mat imgThreshLow;
	cv::Mat imgThreshHigh;
	cv::Mat imgThresh;

	std::vector<cv::Vec3f> v3fCircles;				// 3 element vector of floats, this will be the pass by reference output of HoughCircles()

	char charCheckForEscKey = 0;

	while (charCheckForEscKey != 27 && capWebcam.isOpened()) {		// until the Esc key is pressed or webcam connection is lost
		bool blnFrameReadSuccessfully = capWebcam.read(imgOriginal);		// get next frame

		if (!blnFrameReadSuccessfully || imgOriginal.empty()) {		// if frame not read successfully
			std::cout << "error: frame not read from webcam\n";		// print error message to std out
			break;													// and jump out of while loop
		}

		cv::cvtColor(imgOriginal, imgHSV, CV_BGR2HSV);

		cv::inRange(imgHSV, cv::Scalar(0, 135, 135), cv::Scalar(30, 255, 255), imgThreshLow);
		cv::inRange(imgHSV, cv::Scalar(150, 135, 135), cv::Scalar(179, 255, 255), imgThreshHigh);

		cv::add(imgThreshLow, imgThreshHigh, imgThresh);

		cv::GaussianBlur(imgThresh, imgThresh, cv::Size(3, 3), 0);

		cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

		cv::dilate(imgThresh, imgThresh, structuringElement);
		cv::erode(imgThresh, imgThresh, structuringElement);

		// fill circles vector with all circles in processed image
		cv::HoughCircles(imgThresh,			// input image
			v3fCircles,							// function output (must be a standard template library vector
			CV_HOUGH_GRADIENT,					// two-pass algorithm for detecting circles, this is the only choice available
			2,									// size of image / this value = "accumulator resolution", i.e. accum res = size of image / 2
			imgThresh.rows / 4,				// min distance in pixels between the centers of the detected circles
			100,								// high threshold of Canny edge detector (called by cvHoughCircles)						
			50,									// low threshold of Canny edge detector (set at 1/2 previous value)
			10,									// min circle radius (any circles with smaller radius will not be returned)
			400);								// max circle radius (any circles with larger radius will not be returned)

		for (int i = 0; i < v3fCircles.size(); i++) {		// for each circle . . .
			// show ball position x, y, and radius to command line
			std::cout << "ball position x = " << v3fCircles[i][0]			// x position of center point of circle
				<< ", y = " << v3fCircles[i][1]								// y position of center point of circle
				<< ", radius = " << v3fCircles[i][2] << "\n";				// radius of circle

			// draw small green circle at center of detected object
			cv::circle(imgOriginal,												// draw on original image
				cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]),		// center point of circle
				3,																// radius of circle in pixels
				cv::Scalar(0, 255, 0),											// draw pure green (remember, its BGR, not RGB)
				CV_FILLED);														// thickness, fill in the circle

			// draw red circle around the detected object
			cv::circle(imgOriginal,												// draw on original image
				cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]),		// center point of circle
				(int)v3fCircles[i][2],											// radius of circle in pixels
				cv::Scalar(0, 0, 255),											// draw pure red (remember, its BGR, not RGB)
				3);																// thickness of circle in pixels
		}	// end for

		// declare windows
		cv::namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);	// note: you can use CV_WINDOW_NORMAL which allows resizing the window
		cv::namedWindow("imgThresh", CV_WINDOW_AUTOSIZE);	// or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
		// CV_WINDOW_AUTOSIZE is the default

		cv::imshow("imgOriginal", imgOriginal);			// show windows
		cv::imshow("imgThresh", imgThresh);

		charCheckForEscKey = cv::waitKey(1);			// delay (in ms) and get key press, if any
	}	// end while

	return(0);
}

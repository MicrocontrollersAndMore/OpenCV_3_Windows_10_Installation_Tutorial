// CannyWebcam.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	cv::VideoCapture capWebcam(0);		// declare a VideoCapture object and associate to webcam, 0 => use 1st webcam

	if (capWebcam.isOpened() == false) {			// check if VideoCapture object was associated to webcam successfully
		std::cout << "error: capWebcam not accessed successfully\n\n";	// if not, print error message to std out
		return(0);														// and exit program
	}

	cv::Mat matOriginal;		// input image
	cv::Mat matGrayscale;		// grayscale of input image
	cv::Mat matBlurred;			// intermediate blured image
	cv::Mat matCanny;			// Canny edge image

	char charCheckForEscKey = 0;

	while (charCheckForEscKey != 27 && capWebcam.isOpened()) {		// until the Esc key is pressed or webcam connection is lost
		bool blnFrameReadSuccessfully = capWebcam.read(matOriginal);		// get next frame

		if (!blnFrameReadSuccessfully || matOriginal.empty()) {		// if frame not read successfully
			std::cout << "error: frame not read from webcam\n";		// print error message to std out
			break;													// and jump out of while loop
		}

		cv::cvtColor(matOriginal, matGrayscale, CV_BGR2GRAY);		// convert to grayscale

		cv::GaussianBlur(matGrayscale,			// input image
			matBlurred,							// output image
			cv::Size(5, 5),						// smoothing window width and height in pixels
			1.8);								// sigma value, determines how much the image will be blurred

		cv::Canny(matBlurred,			// input image
			matCanny,					// output image
			50,							// low threshold
			100);						// high threshold

													// declare windows
		cv::namedWindow("Original", CV_WINDOW_NORMAL);	// note: you can use CV_WINDOW_NORMAL which allows resizing the window
		cv::namedWindow("Canny", CV_WINDOW_NORMAL);		// or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
														// CV_WINDOW_AUTOSIZE is the default
		cv::imshow("Original", matOriginal);		// show windows
		cv::imshow("Canny", matCanny);

		charCheckForEscKey = cv::waitKey(1);		// delay (in ms) and get key press, if any
	}	// end while

	return(0);
}

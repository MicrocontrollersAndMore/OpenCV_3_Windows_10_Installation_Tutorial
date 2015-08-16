// CannyStill.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

	cv::Mat matOriginal;		// input image
	cv::Mat matGrayscale;		// grayscale of input image
	cv::Mat matBlurred;			// intermediate blured image
	cv::Mat matCanny;			// Canny edge image

	matOriginal = cv::imread("image.jpg");			// open image

	if (matOriginal.empty()) {									// if unable to open image
		std::cout << "error: image not read from file\n\n";		// show error message on command line
		return(0);												// and exit program
	}

	cv::cvtColor(matOriginal, matGrayscale, CV_BGR2GRAY);		// convert to grayscale

	cv::GaussianBlur(matGrayscale,			// input image
		matBlurred,							// output image
		cv::Size(5, 5),						// smoothing window width and height in pixels
		1.5);								// sigma value, determines how much the image will be blurred

	cv::Canny(matBlurred,			// input image
		matCanny,					// output image
		100,						// low threshold
		200);						// high threshold

													// declare windows
	cv::namedWindow("Original", CV_WINDOW_AUTOSIZE);	// note: you can use CV_WINDOW_NORMAL which allows resizing the window
	cv::namedWindow("Canny", CV_WINDOW_AUTOSIZE);		// or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
														// CV_WINDOW_AUTOSIZE is the default
	cv::imshow("Original", matOriginal);		// show windows
	cv::imshow("Canny", matCanny);

	cv::waitKey(0);					// hold windows open until user presses a key

	return(0);
}

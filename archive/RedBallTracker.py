# RedBallTracker.py

import cv2
import numpy as np
import os

###################################################################################################
def main():

    capWebcam = cv2.VideoCapture(0)                     # declare a VideoCapture object and associate to webcam, 0 => use 1st webcam

    if capWebcam.isOpened() == False:                           # check if VideoCapture object was associated to webcam successfully
        print "error: capWebcam not accessed successfully\n\n"          # if not, print error message to std out
        os.system("pause")                                              # pause until user presses a key so user can see error message
        return                                                          # and exit function (which exits program)

    while cv2.waitKey(1) != 27 and capWebcam.isOpened():                # until the Esc key is pressed or webcam connection is lost
        blnFrameReadSuccessfully, imgOriginal = capWebcam.read()            # read next frame

        if not blnFrameReadSuccessfully or imgOriginal is None:             # if frame was not read successfully
            print "error: frame not read from webcam\n"                     # print error message to std out
            os.system("pause")                                              # pause until user presses a key so user can see error message
            break                                                           # exit while loop (which exits program)

        imgHSV = cv2.cvtColor(imgOriginal, cv2.COLOR_BGR2HSV)

        imgThreshLow = cv2.inRange(imgHSV, (0, 155, 155), (15, 255, 255))
        imgThreshHigh = cv2.inRange(imgHSV, (160, 155, 155), (179, 255, 255))

        imgThresh = cv2.add(imgThreshLow, imgThreshHigh)

        imgThresh = cv2.GaussianBlur(imgThresh, (3, 3), 2)                 # blur

        imgThresh = cv2.dilate(imgThresh, np.ones((5,5),np.uint8))        # close image (dilate, then erode)
        imgThresh = cv2.erode(imgThresh, np.ones((5,5),np.uint8))         # closing "closes" (i.e. fills in) foreground gaps

        intRows, intColumns = imgThresh.shape        # break out number of rows and columns in the image, rows is used for minimum distance between circles in call to Hough Circles

        circles = cv2.HoughCircles(imgThresh, cv2.HOUGH_GRADIENT, 2, intRows / 4)      # fill variable circles with all circles in the processed image

        if circles is not None:                     # this line is necessary to keep program from crashing on next line if no circles were found
            for circle in circles[0]:                           # for each circle
                x, y, radius = circle                                                                       # break out x, y, and radius
                print "ball position x = " + str(x) + ", y = " + str(y) + ", radius = " + str(radius)       # print ball position and radius
                cv2.circle(imgOriginal, (x, y), 3, (0, 255, 0), cv2.FILLED)           # draw small green circle at center of detected object
                cv2.circle(imgOriginal, (x, y), radius, (0, 0, 255), 3)                     # draw red circle around the detected object
            # end for
        # end if

        cv2.namedWindow("imgOriginal", cv2.WINDOW_AUTOSIZE)            # create windows, use WINDOW_AUTOSIZE for a fixed window size
        cv2.namedWindow("imgThresh", cv2.WINDOW_AUTOSIZE)           # or use WINDOW_NORMAL to allow window resizing

        cv2.imshow("imgOriginal", imgOriginal)                 # show windows
        cv2.imshow("imgThresh", imgThresh)
    # end while

    cv2.destroyAllWindows()                     # remove windows from memory

    return

###################################################################################################
if __name__ == "__main__":
    main()

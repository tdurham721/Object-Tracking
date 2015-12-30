#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

//**********************************************************************
//This program uses the OpenCV 2.4 library to detect and outline motion
//in a live video captured from a webcam. It does this by using an
//algorithm that stores the initial frame of the video as a reference
//image, then it checks following frames for changes in pixel values.
//If the pixel values change by a certain threshold, they are marked as
//no longer being part of the background and therefore are part of the
//foreground. The algorithm can also be used to detect shadows that move
//in the image, but I didn't use this. OpenCV implements a lot of this for you.
//(See the paper located here: http://personal.ee.surrey.ac.uk/Personal/R.Bowden/publications/avbs01/avbs01.pdf)
//The code also contains a section to outline the detected foreground 
//for easier identification.
//
//The documentation for the BackgroundSubtractorMOG2 class can be found here:
//http://docs.opencv.org/modules/video/doc/motion_analysis_and_object_tracking.html
//**********************************************************************
 
int main(int argc, char *argv[])
{
    cv::Mat currentFrame; //Matrix to store the current video frame
    cv::Mat background; //Matrix to store the background of the image
    cv::Mat foreground; //Matrix to store the detected foreground of the image

	std::string input = "";
	int choice = 0; //Variable to store whether user selects video file or webcam
	int validNumber = 0; //Make sure the user enters either 1 or 2

	while (validNumber == 0)
	{
		std::cout << "Enter 1 for a video file, or 2 for using the webcam." << std::endl;
		std::getline(std::cin, input);

			std::stringstream myStream(input);
			if (myStream >> choice) //Check to see if what the user entered was an integer
			{
				if (choice == 1 || choice == 2)
				{
				validNumber = 1; 
				}
			}
			if (validNumber != 1) //User either didn't enter an int, or they entered an int not equal to 1 or 2
			{
				std::cout << "Invalid number, please try again" << std::endl;
			}
	}
	std::cout << "You entered: " << choice << std::endl << std::endl;

	cv::VideoCapture cap; //OpenCV object to store the video frames
	if (choice == 1) //If they chose to use a video file
	{
		std::cout << "Enter the full file path of the video file." << std::endl;
		std::string filename = "";
		std::getline(std::cin, filename);
		cap = cv::VideoCapture(filename); //Set the object equal to the frames of the video at the filename specified
	}
	else
	{
		cap = cv::VideoCapture(0); //Set the object equal to the video captured from the machine's default webcam
	}


    cv::BackgroundSubtractorMOG2 bg; //Object that implements the algorithm to detect foreground objects

	bg.set("nmixtures", 3); //Set number of Gaussian mixtures. According to the paper, a number from 3-5 is good
	bg.set("nShadowDetection", 0); //Turn off shadow detection. There is currently a bug in OpenCV 2.4 and up
								   //that doesn't allow you to set this with the boolean value bShadowDetection.

    std::vector<std::vector<cv::Point> > contours; //The contours that outline the foreground are stored in a vector, but
												   //there may be more than one object, so there needs to be multiple vectors
 
    cv::namedWindow("Detected Objects");
    cv::namedWindow("Background as determined by the subtraction algorithm");
 
    for(;;) //Keep running algorithm until a key is pressed
    {
		cap >> currentFrame; //Grab the current frame from the video
        bg.operator ()(currentFrame,foreground); //Update the background model with the new frame and compute foreground mask
        bg.getBackgroundImage(background); //Grab the new background model
        cv::erode(foreground,foreground,cv::Mat()); //These two lines get rid of any noise in the foreground mask
        cv::dilate(foreground,foreground,cv::Mat());
        cv::findContours(foreground,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE); //This calculates the contours in the
		                                                                             //binary image of the foreground mask
		                                                                             //by performing edge detection on the
		                                                                             //binary image.
        cv::drawContours(currentFrame,contours,-1,cv::Scalar(0,0,255),2);
        cv::imshow("Frame",currentFrame); //Display current frame of video
        cv::imshow("Background",background); //Display current background image as determined by the background subtractor
        if(cv::waitKey(30) >= 0) break; //When user presses a key, the program will exit
    }
    return 0;
}
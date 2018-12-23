#include "../includes/MonoLoop.h"

const int MY_IMAGE_WIDTH = 640;
const int MY_IMAGE_HEIGHT = 480;
const int MY_WAIT_IN_MS = 20;

int MonoLoop() {
	cv::VideoCapture cap(0);

	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	// Set cameras to 15fps (if wanted!!!)
	cap.set(cv::CAP_PROP_FPS, 15);

	double dWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

	// Set image size
	cap.set(cv::CAP_PROP_FRAME_WIDTH, MY_IMAGE_WIDTH);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, MY_IMAGE_HEIGHT);

	// display the frame size that OpenCV has picked in order to check
	cout << "cam Frame size: " << dWidth << " x " << dHeight << endl;
	namedWindow("cam", cv::WINDOW_AUTOSIZE);

	cv::Mat inputFrame;
	cv::Mat outputFrame;

	while (true)
	{
		if (!cap.read(inputFrame))
		{
			std::cout << "Cannot read a frame from video stream" << std::endl;
			break;
		}

		/*******************************todo*****************************/
		outputFrame = inputFrame;
		/***************************end todo*****************************/

		imshow("cam", outputFrame);

		if (cv::waitKey(MY_WAIT_IN_MS) == 27)
		{
			std::cout << "ESC key is pressed by user" << std::endl;
			break;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	CMonoLoop myLoop;
//	myLoop.Run();

	return MonoLoop();
}


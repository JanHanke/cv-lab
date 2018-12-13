#include "../includes/stdafx.h"
#include "../includes/MonoLoop.h"

using namespace cv;

const int MY_IMAGE_WIDTH = 640;
const int MY_IMAGE_HEIGHT = 480;
const int MY_WAIT_IN_MS = 20;

int MonoLoop() {
	VideoCapture cap(0);

	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	// Set cameras to 15fps (if wanted!!!)
	cap.set(CAP_PROP_FPS, 15);

	double dWidth = cap.get(CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT);

	// Set image size
	cap.set(CAP_PROP_FRAME_WIDTH, MY_IMAGE_WIDTH);
	cap.set(CAP_PROP_FRAME_HEIGHT, MY_IMAGE_HEIGHT);

	// display the frame size that OpenCV has picked in order to check
	cout << "cam Frame size: " << dWidth << " x " << dHeight << endl;
	namedWindow("cam", WINDOW_AUTOSIZE);

	Mat inputFrame;
	Mat outputFrame;

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

/*

int main(int argc, char *argv[]) {
	CMonoLoop myLoop;
//	myLoop.Run();

	return MonoLoop();
}
*/


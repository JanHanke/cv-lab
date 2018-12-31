#include <vector>
#include <memory>
#include "../includes/MonoLoop.h"

const int MY_IMAGE_WIDTH = 640;
const int MY_IMAGE_HEIGHT = 480;
const int MY_WAIT_IN_MS = 20;

const int ROI_WIDTH = 100;
const int ROI_HEIGHT = 100;

cv::Rect ROI_RECT(cv::Point2d((MY_IMAGE_WIDTH / 2.0) - (ROI_WIDTH / 2.0), (MY_IMAGE_HEIGHT / 2.0) - (ROI_HEIGHT / 2.0)),
						cv::Point2d((MY_IMAGE_WIDTH / 2.0) + (ROI_WIDTH / 2.0), (MY_IMAGE_HEIGHT / 2.0) + (ROI_HEIGHT / 2.0)));

cv::Mat ROI_HIST;

cv::Mat process(cv::Mat& inputFrame);

int MonoLoop()
{
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
	namedWindow("debug", cv::WINDOW_AUTOSIZE);
	namedWindow("roi", cv::WINDOW_AUTOSIZE);

	cv::Mat inputFrame;
	cv::Mat outputFrame;

	cv::Mat roi;
	cv::Mat hsv_roi;


	// First loop to capture the region of interest
	while(true)
	{

		if (!cap.read(inputFrame))
		{
			std::cout << "Cannot read a frame from video stream" << std::endl;
			break;
		}

		/*******************************todo*****************************/
		outputFrame = inputFrame;

		// Draw target square
		cv::rectangle(outputFrame, ROI_RECT, cv::Scalar(0, 255, 0));
		/***************************end todo*****************************/

		imshow("cam", outputFrame);

		if (cv::waitKey(MY_WAIT_IN_MS) == 27)
		{
			outputFrame(ROI_RECT).copyTo(roi);
			cv::cvtColor(roi, hsv_roi, cv::COLOR_BGR2HSV);
			int hbins = 30;
			int sbins = 32;
			int histSize[] = { hbins, sbins };

			float hranges[] = {0, 180};
			float sranges[] = {0, 256};
			const float* ranges[] = { hranges, sranges };
			int channels[] = { 0, 1 };
			cv::calcHist(&hsv_roi, 1, channels, cv::Mat(), ROI_HIST, 2, histSize, ranges);
			break;
		}
	}

//	cv::Mat roi;
//	cv::Mat hsv_roi = cv::cvtColor(roi, cv::COLOR_BGR2HSV);
//	cv::calcHist();

	while (true)
	{
		if (!cap.read(inputFrame))
		{
			std::cout << "Cannot read a frame from video stream" << std::endl;
			break;
		}

		/*******************************todo*****************************/
		outputFrame = process(inputFrame);
		/***************************end todo*****************************/

		//imshow("roi", ROI_HIST);
		imshow("cam", outputFrame);

		if (cv::waitKey(MY_WAIT_IN_MS) == 27)
		{
			std::cout << "ESC key is pressed 	cv::Mat reference_image;by user" << std::endl;
			break;
		}
	}
	return 0;
}

cv::Mat process(cv::Mat& inputFrame)
{
    auto outputFrame = inputFrame;

    cv::Mat hsv, backproject;
   	cv::cvtColor(inputFrame, hsv, cv::COLOR_BGR2HSV);
   	int channels[] = { 0, 1 };
	float hranges[] = {0, 180};
	float sranges[] = {0, 256};
	const float* ranges[] = { hranges, sranges };
   	cv::calcBackProject(&hsv, 1, channels, ROI_HIST, backproject, ranges);

   	cv::RotatedRect tracking_data = cv::CamShift(backproject, ROI_RECT, cv::TermCriteria());
   	std::vector<cv::Point2f> points(4);

   	//cv::boxPoints(tracking_data, points);
   	tracking_data.points(points.data());

   	std::vector<cv::Point> poly_points;
   	for (auto v : points)
	{
   		poly_points.push_back(v);
	}
   	cv::polylines(outputFrame, poly_points, true, cv::Scalar(0, 255, 0));

   	imshow("roi", backproject);

    return outputFrame;
}

int main(int argc, char *argv[]) {
	return MonoLoop();
}


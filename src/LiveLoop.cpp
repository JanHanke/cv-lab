
#include <opencv2/opencv.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

const int MY_IMAGE_WIDTH = 640;
const int MY_IMAGE_HEIGHT = 480;
const int MY_WAIT_IN_MS = 20;

const int ROI_WIDTH = 100;
const int ROI_HEIGHT = 100;

cv::Rect ROI_RECT(cv::Point2d((MY_IMAGE_WIDTH / 2.0) - (ROI_WIDTH / 2.0), (MY_IMAGE_HEIGHT / 2.0) - (ROI_HEIGHT / 2.0)),
						cv::Point2d((MY_IMAGE_WIDTH / 2.0) + (ROI_WIDTH / 2.0),
										(MY_IMAGE_HEIGHT / 2.0) + (ROI_HEIGHT / 2.0)));

cv::Mat ROI_HIST;

cv::Mat process(cv::Mat &inputFrame, server *s, const websocketpp::connection_hdl &hdl);

void loop(server *s, const websocketpp::connection_hdl &hdl) {
	cv::VideoCapture cap(0);

	if (!cap.isOpened())
	{
		std::cout << "Cannot open the video cam" << std::endl;
		std::terminate();
	}

	// Set cameras to 15fps (if wanted!!!)
	cap.set(cv::CAP_PROP_FPS, 15);

	double dWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

	// Set image size
	cap.set(cv::CAP_PROP_FRAME_WIDTH, MY_IMAGE_WIDTH);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, MY_IMAGE_HEIGHT);

	// display the frame size that OpenCV has picked in order to check
	std::cout << "cam Frame size: " << dWidth << " x " << dHeight << std::endl;
	namedWindow("cam", cv::WINDOW_AUTOSIZE);
	namedWindow("debug", cv::WINDOW_AUTOSIZE);
	namedWindow("roi", cv::WINDOW_AUTOSIZE);

	cv::Mat inputFrame;
	cv::Mat outputFrame;

	cv::Mat roi;
	cv::Mat hsv_roi;


	// First loop to capture the region of interest
	while (true)
	{
		if (!cap.read(inputFrame))
		{
			std::cout << "Cannot read a frame from video stream" << std::endl;
			break;
		}
		outputFrame = inputFrame;
		// Rectangle to indicate Region Of Interest
		cv::rectangle(outputFrame, ROI_RECT, cv::Scalar(0, 255, 0));

		imshow("cam", outputFrame);

		// Use waitkey so user can control when ROI is captured
		if (cv::waitKey(MY_WAIT_IN_MS) == 27)
		{
			outputFrame(ROI_RECT).copyTo(roi);
			cv::cvtColor(roi, hsv_roi, cv::COLOR_BGR2HSV);
			int hbins = 30;
			int sbins = 32;
			int histSize[] = {hbins, sbins};

			float hranges[] = {0, 180};
			float sranges[] = {0, 256};
			const float *ranges[] = {hranges, sranges};
			int channels[] = {0, 1};
			cv::calcHist(&hsv_roi, 1, channels, cv::Mat(), ROI_HIST, 2, histSize, ranges);
			imshow("debug", hsv_roi);
			break;
		}
	}

	while (true)
	{
		if (!cap.read(inputFrame))
		{
			std::cout << "Cannot read a frame from video stream" << std::endl;
			break;
		}

		outputFrame = process(inputFrame, s, hdl);
		imshow("cam", outputFrame);


		if (cv::waitKey(MY_WAIT_IN_MS) == 27)
		{
			std::cout << "ESC key is pressed" << std::endl;
			cv::destroyAllWindows();
			std::terminate();
		}
	}
	std::terminate();
}


void send_message(server *s, const websocketpp::connection_hdl &hdl, std::string const &payload) {
	try
	{
		s->send(hdl, payload, websocketpp::frame::opcode::text);
	} catch (websocketpp::exception const &e)
	{
		std::cout << "Transmission to client failed due to: "
					 << "(" << e.what() << ")" << std::endl;
	}
}

cv::Mat process(cv::Mat &inputFrame, server *s, const websocketpp::connection_hdl &hdl) {
	auto outputFrame = inputFrame;
	cv::Mat dilation_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::Mat erosion_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(8, 8));

	cv::Mat hsv, backproject;
	cv::cvtColor(inputFrame, hsv, cv::COLOR_BGR2HSV);
	int channels[] = {0, 1};
	float hranges[] = {0, 180};
	float sranges[] = {0, 256};
	const float *ranges[] = {hranges, sranges};
	cv::calcBackProject(&hsv, 1, channels, ROI_HIST, backproject, ranges);

	cv::GaussianBlur(backproject, backproject, cv::Size(5, 5), 1);
	cv::erode(backproject, backproject, erosion_kernel);
	cv::dilate(backproject, backproject, dilation_kernel);
	cv::RotatedRect tracking_data = cv::CamShift(backproject, ROI_RECT, cv::TermCriteria());
	std::vector<cv::Point2f> points(4);

	//cv::boxPoints(tracking_data, points);
	tracking_data.points(points.data());

	std::vector<cv::Point> poly_points;
	std::ostringstream json;
	json << "{ \"points\": [";
	int i = 0;
	for (const auto &v : points)
	{
		poly_points.push_back(v);
		if (i == points.size() - 1)
		{
			json << "{ \"x\":" << v.x << ", \"y\":" << v.y << "}";
		} else
		{
			json << "{ \"x\":" << v.x << ", \"y\":" << v.y << "},";
		}
		i++;
	}
	json << "]}";
	send_message(s, hdl, json.str());
	cv::polylines(outputFrame, poly_points, true, cv::Scalar(0, 255, 0));

	imshow("roi", backproject);

	return outputFrame;
}

// Define a callback to handle incoming messages
void on_message(server *s, const websocketpp::connection_hdl &hdl, const message_ptr &msg) {
	std::cout << "on_message called with hdl: " << hdl.lock().get()
				 << " and message: " << msg->get_payload()
				 << std::endl;

	// check for a special command to instruct the server to stop listening so
	// it can be cleanly exited.
	if (msg->get_payload() == "stop-listening")
	{
		s->stop_listening();
		return;
	}

	if (msg->get_payload() == "start")
	{
		std::thread([=] { return loop(s, hdl); }).detach();
		return;
	}

	try
	{
		s->send(hdl, msg->get_payload(), msg->get_opcode());
	} catch (websocketpp::exception const &e)
	{
		std::cout << "Echo failed because: "
					 << "(" << e.what() << ")" << std::endl;
	}
}

int main(int argc, char *argv[]) {
	server echo_server;

	try
	{
		// Set logging settings
		echo_server.set_access_channels(websocketpp::log::alevel::all);
		echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
		// Initialize Asio
		echo_server.init_asio();
		// Register our message handler
		echo_server.set_message_handler(bind(&on_message, &echo_server, ::_1, ::_2));
		// Listen on port 8888
		echo_server.listen(8888);
		// Start the server accept loop
		echo_server.start_accept();
		// Start the ASIO io_service run loop
		echo_server.run();
	} catch (websocketpp::exception const &e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "other exception" << std::endl;
	}
}


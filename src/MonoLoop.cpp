#include "../includes/MonoLoop.h"

CMonoLoop::CMonoLoop() {
	m_width = 640;
	m_height = 480;
	m_isInit = false;
}

CMonoLoop::~CMonoLoop(void) = default;

int CMonoLoop::CreateImages() {
	// create output image, same size and type like input image
	cv::Size frameSize = m_inputFrame.size();
	int type = m_inputFrame.type();
	m_outputFrame.create(frameSize, type);
	// alternative: m_outputFrame = m_inputFrame;

	// create additional images for processing here
	// return -1 in case of failure

	return 0;
}

int CMonoLoop::InitCam() {
	m_cap.open(0);

	if (!m_cap.isOpened())
	{
		cout << "Cannot open the video cam [0]" << endl;
		return -1;
	}
	double dWidth = m_cap.get(CAP_PROP_FRAME_WIDTH);
	double dHeight = m_cap.get(CAP_PROP_FRAME_HEIGHT);

	// Set image size
	m_cap.set(CAP_PROP_FRAME_WIDTH, m_width);
	m_cap.set(CAP_PROP_FRAME_HEIGHT, m_height);

	// display the frame size that OpenCV has picked in order to check
	cout << "Frame size: " << dWidth << " x " << dHeight << endl;
	namedWindow("LiveImage", WINDOW_AUTOSIZE);

	bool bSuccess = m_cap.read(m_inputFrame);

	if (!bSuccess)
	{
		cout << "Cannot read a frame from video stream" << endl;
		return -1;
	}

	if (CreateImages() != 0)
		return -1;

	m_isInit = true;

	return 0;
}

int CMonoLoop::Run() {
	if (!m_isInit)
		InitCam();

	while (1)
	{
		bool bSuccess = m_cap.read(m_inputFrame);

		if (!bSuccess)
		{
			cout << "Cannot read a frame from video stream" << endl;
			return -1;
		}
		if (ProcessImage() != 0)
		{
			cout << "Cannot process image" << endl;
			return -1;
		}

		imshow("LiveImage", m_outputFrame);

		if (cv::waitKey(30) == 27)
		{
			cout << "ESC key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}

void CMonoLoop::MyInvert() {
	Size frameSize = m_inputFrame.size();
	int type = m_inputFrame.type();
	int noChannels = m_inputFrame.channels();
	if (type != 8 * (noChannels - 1)) // unsigned char only: CV_8U for CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4
	{
		cout << "Only unsigned char images will be inverted" << endl;
		return;
	}

	auto *pSource = (unsigned char *) m_inputFrame.data;
	auto *pDest = (unsigned char *) m_outputFrame.data;
	int pixels = noChannels * frameSize.height * frameSize.width;

	for (int i = 0; i < pixels; i++)
	{
		pDest[i] = (unsigned char) (255 - pSource[i]);
	}
}

int CMonoLoop::ProcessImage() {
	/// todo, e.g.
	//m_outputFrame = m_inputFrame;
	// or
	MyInvert();

	return 0;
}



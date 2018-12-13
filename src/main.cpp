#include <opencv4/opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char **argv) {

	Mat image = imread("./img/canny_input.jpg", 1);
	Mat gray, canny;
	cvtColor(image, gray, COLOR_RGB2GRAY);
	Canny(gray, canny, 60, 60 * 3);

	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", image);
	namedWindow("Canny", WINDOW_AUTOSIZE);
	imshow("Canny", canny);
	waitKey(0);
	return 0;
}

int process(int argc, char **argv) {
	if (argc != 2)
	{
		printf("usage: DisplayImage.out <Image_Path>\n");
		return -1;
	}
	Mat image;
	image = imread(argv[1], 1);
	if (!image.data)
	{
		printf("No image data \n");
		return -1;
	}
	Mat blurImage = Mat::zeros(image.rows, image.cols, image.type());
	GaussianBlur(image, blurImage, Size(0, 0), 2.0, 0, BORDER_DEFAULT);

	namedWindow("OpenCV", WINDOW_AUTOSIZE);
	imshow("OpenCV", blurImage);
	waitKey(0);
}
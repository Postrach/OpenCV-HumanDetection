/*
	"spacja" - koniec programu
	"p" - pauza on/off
	"s" - zatrzymanie wykrywania/ponowne uruchomienie wykrywania
*/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <time.h>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define SKIP 800
#define SOURCE "Ludzie.mp4"

using namespace cv;
using namespace std;

string caclculateFPS(int frames, time_t startTime) {
	double fps = 0;
	time_t endTime;
	time(&endTime);
	double elapsedTime = difftime(endTime, startTime);
	fps = frames / elapsedTime;
	return to_string(fps);
}

vector<Rect> detect(HOGDescriptor hog, Mat frame) {
	vector<Rect> found;
	hog.detectMultiScale(frame, found, 0, Size(16, 16), Size(16, 16), 1.07, 2);
	return found;
}

int main(int argc, const char* argv[])
{
	VideoCapture cap(SOURCE);
	if (!cap.isOpened()) {
		cout << "Error while opening a video source" << endl;
		return 0;
	}

	Mat frame, img;

	// creating window and size reduction form better performance
	namedWindow("Window");
	resizeWindow("Window", Size(480, 360));

	// Histogram of Oriented Gradients
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	vector<Point> track;

	// time variables for FPS calculation
	time_t startTime;
	int framesCaptured = 0;

	// skipping first n frames; n = SKIP
	if (SKIP != 0) {
		for (int i = 0; i < SKIP; i++) {
			cap.grab();
		}
	}

	// losowe kolory
	vector<Scalar> colors;
	RNG rng;
	for (int i = 0; i < 500; i++)
	{
		int r = rng.uniform(0, 256);
		int g = rng.uniform(0, 256);
		int b = rng.uniform(0, 256);
		colors.push_back(Scalar(r, g, b));
	}

	// get start time
	time(&startTime);

	while (cap.read(frame)) {
		if (frame.empty()) {
			cerr << "Error while reading next frame" << endl;
			return 0;
		}

		// increase amount of frames read for FPS count
		framesCaptured++;

		// resize read frame for better performance
		resize(frame, frame, Size(480, 360));
		// clone frame for detector
		img = frame.clone();
		// change frame to grayscale for better performance
		cvtColor(img, img, COLOR_BGR2GRAY);

		// vector of found Rectangles 
		vector<Rect> found;

		// search for people in frame using HOG default people detection descriptor
		found = detect(hog, img);
		
		/// draw detections rectangles
		for (size_t i = 0; i < found.size(); i++)
		{
			rectangle(frame, found[i], colors[i], 3);
			stringstream temp;
		}

		// display FPS and count of humans detected
		putText(frame, "FPS: " + caclculateFPS(framesCaptured, startTime), Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255), 2);
		putText(frame, "Humans: " + to_string(found.size()), Point(20, 40), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255), 2);
		
		// show frame
		imshow("Window", frame);

		if (cv::waitKey(30) == 32) {
			frame.release();
			img.release();
			destroyAllWindows();
			break;
		}
	}

	frame.release();
	img.release();
	destroyAllWindows();
	return 0;
}
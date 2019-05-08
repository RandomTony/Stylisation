#include "video.hpp"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include "bilateral.hpp"


using namespace cv;

void doVid(string source) {
	const bool askOutputType = true;  // If false it will use the inputs codec type

	VideoCapture inputVideo(source);              // Open input
	if (!inputVideo.isOpened())
	{
		std::cout << "Could not open the input video: " << source << endl;
		return ;
	}

	string::size_type pAt = source.find_last_of('.');                  // Find extension point
	const string NAME = source.substr(0, pAt) + "quantifgresult" + ".mp4";   // Form the new name with container
	int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form

	// Transform from int to char via Bitwise operators
	char EXT[] = { (char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0 };

	Size S = Size((int)inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
		(int)inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

	VideoWriter outputVideo;                                        // Open the output
	if (askOutputType)
		outputVideo.open(NAME, ex = -1, inputVideo.get(CV_CAP_PROP_FPS), S, true);
	else
		outputVideo.open(NAME, ex, inputVideo.get(CV_CAP_PROP_FPS), S, true);

	if (!outputVideo.isOpened())
	{
		std::cout << "Could not open the output video for write: " << source << endl;
		return ;
	}

	std::cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
		<< " of nr#: " << inputVideo.get(CV_CAP_PROP_FRAME_COUNT) << endl;
	std::cout << "Input codec type: " << EXT << endl;

	int n(inputVideo.get(CV_CAP_PROP_FRAME_COUNT));
	// int i(0);

	Mat src, res;
	vector<Mat> spl;
	int end = n > 00 ? 500 : n;
	int k(0);
	for (int i(0);i < end;i++) //Show the image captured in the window and repeat
	{
		inputVideo >> src;              // read
		if (src.empty()) break;         // check if at end

		if (i>100 && k<10 ) {
			res = LuminanceQuant(bilateral(src),4);
		}
		else {
			res = src.clone();
			k = k > 20 ? k : 0;
		}

		outputVideo << res;
		std::cout << "[" << i << "/" << n << "]" << endl;
		k++;
	}

}

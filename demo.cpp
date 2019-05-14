#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include "tests.hpp"

using namespace cv;

int main(int argc, char const *argv[]) {
    Mat im;
    if(argc == 2)
    im = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    else{
        std::cout << "Provide an picture path as an argument." << '\n';
        std::cout << "Usage: " << argv[0] << " <pathToPicture>" << '\n';
        return 1;
    }
    namedWindow("input picture", WINDOW_NORMAL);
    imshow("input picture",im);
    waitKey();

    testAbstraction(im, argv[1]);
    return 0;
}

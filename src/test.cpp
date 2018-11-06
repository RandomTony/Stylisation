#include <iostream>
#include "opencv2/highgui.hpp"

using namespace cv;

int main(int argc, char const *argv[]) {
  Mat im;
  if(argc == 2)
    im = imread(argv[1],CV_LOAD_IMAGE_COLOR);
  else{
    std::cout << "Provide an picture path as an argument." << '\n';
    std::cout << "Usage: " << argv[0] << " <pathToPicture>" << '\n';
    return 1;
  }
  imshow("test picture",im);
  waitKey();
  return 0;
}

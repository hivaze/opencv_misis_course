#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>

using namespace std;
using namespace cv;

float func(float x) {
     return sin(x/255.0)*255.0;
}

vector <uchar> brightness() {
    vector<uchar> lut(256);
    for (int i(0); i < 256; ++i) {
        lut[i] = func(i);
    }
    return lut;
}


int main() {
    string image_path = ("data/cross_0256x0256.png");
    Mat img = imread(image_path, IMREAD_COLOR);
    if(img.empty())
    {
        cout << "Could not read the image: " << image_path << endl;
        return 1;
    }

    Mat img_gray;
    cvtColor(img, img_gray, COLOR_BGR2GRAY);
    Mat img_res, gray_res;
    LUT(img, brightness(), img_res);
    LUT(img_gray, brightness(), gray_res);

    imwrite("lab03_rgb.png", img);
    imwrite("lab03_gre.png", img_gray);
    imwrite("lab03_rgb_res.png", img_res);
    imwrite("lab03_gre_res.png", gray_res);

    // plot
    Mat plot(512, 512, CV_8UC1, 255);
    for (int i(0);i < 512; ++i) {
        Point p(511.0 - 511.0*func(i/511.0),i);
        plot.at<uchar>(p) = 0;
    }

   imwrite("lab03_viz_func.png", plot);

    waitKey(0);
    return 0;
}
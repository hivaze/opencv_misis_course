#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
using namespace cv;

void cmerge(int cnum, vector <Mat> rgbsplit, Mat &image, Mat nullm) {
    vector<Mat> rgb;
    for (int i(0); i < 3; ++i) {
        if (i != cnum) {
            rgb.push_back(nullm);
        } else {
            rgb.push_back(rgbsplit[cnum]);
        }
    }
    merge(rgb, image);
}

void sp_n_mg(Mat image, vector <Mat> &img_v) {
    vector<Mat> rgbchannels(3);
    split(image, rgbchannels);
    Mat nullm(Mat::zeros(image.rows, image.cols, CV_8UC1));
    for (int i(0); i < 3; ++i) {
        cmerge(i, rgbchannels, img_v[i], nullm);
    }
}

void hist (Mat img, Mat &himg) {
    cvtColor(img, img, COLOR_BGR2GRAY);
    vector<int> v(256);
    for (int i(0);i < v.size(); ++i) {
        for (int j(0);j < v.size(); ++j) {
            ++v[img.at<uchar>(j, i)];
        }
    }
    int max = *std::max_element(v.begin(), v.end());
    for (int i(0); i < v.size(); ++i) {
        int norm = v[i] * 255 / max;
        Point p1(i * 2, 255 - norm);
        Point p2(i * 2 + 1, 255);
        rectangle(himg, p1, p2, 0);
    }
}

int main() {
    string image_path = ("data/cross_0256x0256.png");
    Mat img = imread(image_path, IMREAD_COLOR);
    if(img.empty())
    {
        cout << "Could not read the image: " << image_path << endl;
        return 1;
    }
    std::vector<int> compression_params;
    compression_params.push_back(IMWRITE_JPEG_QUALITY);
    compression_params.push_back(25);
    imwrite( "cross_0256x0256.jpg", img, compression_params);
    Mat img_jp = imread("cross_0256x0256.jpg");

    int rows(256);
    int cols(512);
    Mat himg(rows, cols, CV_8UC1, 255);
    Mat himg_jp = himg.clone();

    // histogram
    hist(img, himg);
    hist(img_jp, himg_jp);

    //color
    vector <Mat> img_v(3), img_vjp(3);
    sp_n_mg(img, img_v);
    sp_n_mg(img_jp, img_vjp);

    //concat (res)
    hconcat(img, img_v[2],img);
    hconcat(img_v[1], img_v[0], img_v[1]);
    vconcat(img, img_v[1], img);

    hconcat(img_jp, img_vjp[2], img_jp);
    hconcat(img_vjp[1], img_vjp[0], img_vjp[1]);
    vconcat(img_jp, img_vjp[1], img_jp);
    vconcat(himg, himg_jp, himg);

    imwrite("cross_0256x0256_png_channels.png", img);
    imwrite("cross_0256x0256_jpg_channels.png", img_jp);
    imwrite("cross_0256x0256_hists.png", himg);

    imshow("Press any key ...", img );
    waitKey(0);
    imshow("Press any key ...", img_jp);
    waitKey(0);
    imshow("Press any key ...", himg );
    waitKey(0);
    return 0;
}
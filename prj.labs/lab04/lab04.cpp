#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

static string fl;

Mat comp(const Mat& res, const Mat& std) {
    Mat pic(std.size(), CV_8UC3, Scalar(0));
    Mat gr;
    cvtColor(std, gr, COLOR_BGR2GRAY);
    int tp(0), tn(0), fp(0), fn(0);
    for (int i(0); i < gr.rows; ++i) {
        for (int j(0); j < gr.cols; ++j) {
            uchar pR(res.at<uchar>(i, j));
            uchar pS(gr.at<uchar>(i, j));
            if (pR == pS) {
                if (pR == 255) {
                    tp += 1;
                    pic.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
                } else
                    tn += 1;
            }
            else {
                if (pR == 255) {
                    fp += 1;
                    pic.at<Vec3b>(i, j) = Vec3b(0, 60, 255);
                }
                else {
                    fn += 1;
                    pic.at<Vec3b>(i, j) = Vec3b(255, 255, 0);
                }
            }
        }
    }
    double accuracy((tp + tn) * 1.0 / (tp + tn + fp + fn));
    double precision(tp * 1.0 / (tp + fp));
    double rec(tp * 1.0 / (tp + fn));
    cout << fl << endl;
    cout << "Accuracy = " << accuracy << endl;
    cout << "Precision = " << precision << endl;
    cout << "Recall = " << rec << endl;
    return pic;
}

Mat fltr(const Mat& img) {
    Mat invImg, stats, centroids;
    Mat labelImage(img.size(), CV_32S);
    Mat res(img.size(), CV_8UC1);
    bitwise_not(img, invImg);
    int nLabels(connectedComponentsWithStats(invImg, labelImage, stats, centroids));
    vector<uchar> colors(nLabels, 255);
    for (int label(1); label < nLabels; ++label) {
        int l(stats.at<int>(label, CC_STAT_LEFT));
        int t(stats.at<int>(label, CC_STAT_TOP));
        int la(stats.at<int>(label, CC_STAT_AREA));
        if ((230 <= l) && (l <= 2300) && (170 <= t) && (t <= 2500) && (la > 25)) {
            colors[label] = 0;
        }
    }
    for (int i(0); i < res.rows; ++i) {
        for (int j(0); j < res.cols; ++j) {
            int label = labelImage.at<int>(i, j);
            uchar& pixel = res.at<uchar>(i, j);
            pixel = colors[label];
        }
    }
    return res;
}

Mat bin(const Mat& img) {
    Mat I;
    Mat res(Mat::zeros(img.size(), CV_8UC1));
    Mat G;
    img.convertTo(I, CV_32F, 1 / 255.0);
    Mat gk(getGaussianKernel(20, -1));
    gk *= gk.t();
    filter2D(img, G, CV_32F, gk);
    G /= 255.0;
    Mat M(abs(I - G));
    Mat D(img.size(), CV_32F);
    filter2D(M, D, CV_32F, gk);
    res.convertTo(res, CV_32F, 1 / 255.0);
    double d0(1);
    double thres(0.02);
    for (int i(0); i < res.rows; ++i) {
        for (int j(0); j < res.cols; ++j) {
            float g(G.at<float>(i, j));
            float p(I.at<float>(i, j));
            float m(M.at<float>(i, j));
            if ((g - p) / (m + d0) < thres) {
                res.at<float>(i, j) = 1;
            }
        }
    }
    res.convertTo(res, CV_8UC1, 255.0);
    return res;
}


int main() {
    Mat img(imread("../data/data.jpg"));
    Mat stdImg(imread("../data/std.png"));
    Mat G_1, B_1,  F_1, V_1, E_1, B_2, F_2, V_2, E_2;
    cvtColor(img, G_1, COLOR_BGR2GRAY);
    imwrite("lab04.src.jpg", img);
    adaptiveThreshold(G_1, B_1, 255,
            ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 9, 9);
    fl = "B_1";
    comp(B_1, stdImg);
    medianBlur(B_1, F_1, 3);
    fl = "F_1";
    comp(F_1, stdImg);
    V_1 = fltr(F_1);
    fl = "V_1";
    E_1 = comp(V_1, stdImg);
    B_2 = bin(G_1);
    fl = "B_2";
    comp(B_2, stdImg);
    medianBlur(B_2, F_2, 3);
    fl = "F_2";
    comp(F_2, stdImg);
    V_2 = fltr(F_2);
    fl = "V_2";
    E_2 = comp(V_2, stdImg);
    imwrite("lab04.std.png", stdImg);
    imwrite("lab04.g1.png", G_1);
    imwrite("lab04.b1.png", B_1);
    imwrite("lab04.f1.png", F_1);
    imwrite("lab04.v1.png", V_1);
    imwrite("lab04.e1.png", E_1);
    imwrite("lab04.b2.png", B_2);
    imwrite("lab04.f2.png", F_2);
    imwrite("lab04.v2.png", V_2);
    imwrite("lab04.e2.png", E_2);

    waitKey(0);
    return 0;
}
## Работа 1. Исследование гамма-коррекции
автор: Братчиков С.С.
дата: 2022-02-24T00:35:03

<!-- url: https://gitlab.com/2021-misis-spring/polevoy_d_v/-/tree/master/prj.labs/lab01 -->

### Задание
1. Сгенерировать серое тестовое изображение $I_1$ в виде прямоугольника размером 768х60 пикселя с плавным изменение пикселей от черного к белому, одна градация серого занимает 3 пикселя по горизонтали.
2. Применить  к изображению $I_1$ гамма-коррекцию с коэффициентом из интервала 2.2-2.4 и получить изображение $G_1$ при помощи функци pow.
3. Применить  к изображению $I_1$ гамма-коррекцию с коэффициентом из интервала 2.2-2.4 и получить изображение $G_2$ при помощи прямого обращения к пикселям.
4. Показать визуализацию результатов в виде одного изображения (сверху вниз $I_1$, $G_1$, $G_2$).
5. Сделать замер времени обработки изображений в п.2 и п.3, результаты отфиксировать в отчете.

### Результаты

![](../../lab01.png)
Рис. 1. Результаты работы программы (сверху вниз $I_1$, $G_1$, $G_2$)

Gamma pow time: 0.49 ms
Gamma pixel: 1.9 ms

### Текст программы

```cpp
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const double gamma_ = 2.3;

Mat gamma_pow(Mat image) {
    image.convertTo(image, CV_64F, 1/255.0);

    pow(image, gamma_, image);

    image.convertTo(image, CV_8UC1, 255.0);
    return image;
}

Mat gamma_pixel(Mat image) {
    Mat image_float;
    image.convertTo(image_float, CV_64F);

    for (int i = 0; i < image_float.rows; ++i) {

        for (int j = 0; j < image_float.cols; ++j) {
            image_float.at<double>(i, j) =
                    (pow(image_float.at<double>(i, j) / 255.0, gamma_) * 255.0);
        }

    }

    image_float.convertTo(image_float, CV_8UC1);
    return image_float;
}

int main() {

    int h = 60;
    int l = 768;

    Mat image(Mat::zeros(h, l, CV_8UC1));

    cout << image.rows << "\n";
    cout << image.cols << "\n";

    for (int i(0); i < image.cols; ++i) {

        cv::line(image,
                 Point(i, 255),
                 Point(i, 0),
                 Scalar(i / 3, i / 3, i / 3),
                 3,
                 LINE_8);
    }

    double t1 = (double)cv::getTickCount();
    Mat image_pow = gamma_pow(image);

    double t2 = (double)cv::getTickCount();
    Mat image_pix = gamma_pixel(image);

    double t3 = (double)cv::getTickCount();

    double r2 = 1000 * ((t2 - t1) / cv::getTickFrequency());
    double r3 = 1000 * ((t3 - t2) / cv::getTickFrequency());

    Mat result = Mat::zeros(h * 3, l, CV_8UC1);

    image.copyTo(result(Rect(0, h * 0, l, h)));
    image_pow.copyTo(result(Rect(0, h * 1, l, h)));
    image_pix.copyTo(result(Rect(0, h * 2, l, h)));

    cout << "gamma_pow: " << setprecision(2) << r2 << " ms \n"
         << "gamma_pixel: " << setprecision(2) << r3 << " ms " << endl;

    imwrite("lab01.png", result);
//    Mat img = imread("lab01.png");
    imshow("Name", result);
    waitKey(0);
}

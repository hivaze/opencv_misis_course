## Работа 3. Яркостные преобразования изображений
автор: Братчиков С.С.
дата: 2022-04-05T20:17:47

url: https://github.com/hivaze/opencv_misis_course/tree/main/prj.labs/lab03

### Задание
1. В качестве тестового использовать изображение data/cross_0256x0256.png
2. Сгенерировать нетривиальную новую функцию преобразования яркости (не стоит использовать линейную функцию, гамму, случайная).
3. Сгенерировать визуализацию функцию преобразования яркости в виде изображения размером 512x512, черные точки а белом фоне.
4. Преобразовать пиксели grayscale версии тестового изображения при помощи LUT для сгенерированной функции преобразования.
4. Преобразовать пиксели каждого канала тестового изображения при помощи LUT для сгенерированной функции преобразования.
5. Результы сохранить для вставки в отчет.

### Результаты

![](../../lab03_rgb.png)
Рис. 1. Исходное тестовое изображение

![](../../lab03_gre.png)
Рис. 2. Тестовое изображение greyscale

![](../../lab03_gre_res.png)
Рис. 3. Результат применения функции преобразования яркости для greyscale

![](../../lab03_rgb_res.png)
Рис. 4. Результат применения функции преобразования яркости для каналов

![](../../lab03_viz_func.png)
Рис. 5. Визуализация функции яркостного преобразования

### Текст программы

```cpp
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
```

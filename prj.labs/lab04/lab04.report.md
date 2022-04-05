## Работа 4. Использование бинаризации для анализа изображений (выделение символов) 
автор: Бртатчиков С.С.

url: https://github.com/hivaze/opencv_misis_course/tree/main/prj.labs/lab04

### Задание
1. Самостоятельно изготовить эталонный вариант бинаризованного изображения.
2. Цветоредуцировать исходное цветное изображение до изображения в градациях серого $G_1$.
3. Бинаризовать изображение $G_1$ и получить бинаризованное изображение $B_1$.
4. Применить фильтрацию для улучшения качества бинаризованного изображения $F_1$.
5. Выделить компоненты связности.
6. Произвести фильтрацию компонент связности (классификация на "шумовые" и "объекты", $V_1$).
7. Оценить числовые характеристики качества бинаризации на разных этапах и визуализировать отклонение $E_2$ результата анализа от эталона. 
8. Реализовать и настроить локальную бинаризацию по статье "Быстрый алгоритм локальной бинаризации с гауссовым окном" (https://mipt.ru/upload/e35/09-FIVT-arpg5tlxag0.pdf).
9. Сделать пп.3-7 для метода из п.8 (с теми же настройками, получить $B_2$, $F_2$, $V_2$, $E_2$ и численные оценки).

### Результаты

![](../../lab04.src.jpg)
Рис. 1. Исходное тестовое изображение

![](../../lab04.g1.png)
Рис. 2. Визуализация результата $G_1$ цветоредукции

![](../../lab04.b1.png)
Рис. 3. Визуализация результата бинаризаии $B_1$

	Метод adaptiveThreshold():
	*maxValue = 255;
	*adaptiveMethod = ADAPTIVE_THRESH_MEAN_C;
	*thresholdType = THRESH_BINARY;
	*blockSize = 9;
	*C = 9.

	Оценки качества:
	* Accuracy = 0.97;
	* Precision = 1;
	* Recall = 0.97.

![](../../lab04.f1.png)
Рис. 4. Визуализация результата $F_1$ фильтрации бинаризованного изображения $B_1$

	Метод medianBlur():
	*ksize = 3.

	Оценки качества:
	* Accuracy = 0.98;
	* Precision = 1;
	* Recall = 0.98.

![](../../lab04.v1.png)
Рис. 5. Визуализация результатов фильтрации компонент связности $V_1$
	
	Метод connectedComponentsWithStats()
	Оценки качества:
	* Accuracy = 0.999;
	* Precision = 1;
	* Recall = 0.999.


![](../../lab04.e1.png)
Рис. 6. Визуализация отклонений от эталона $E_1$

![](../../lab04.b2.png)
Рис. 7. Визуализация результата бинаризаии $B_2$ (метод с гауссовым окном)

	Метод getGaussianKernel()
	*ksize = 19;
	*sigma = -1;
	*depth = CV_32F;
	*thres = 0.02.

	Оценки качества:
	* Accuracy = 0.95;
	* Precision = 1;
	* Recall = 0.95.

![](../../lab04.f2.png)
Рис. 8. Визуализация результата $F_2$ фильтрации бинаризованного изображения $B_2$
	
	Метод medianBlur():
	*ksize = 3.

	Оценки качества:
	* Accuracy = 0.95;
	* Precision = 1;
	* Recall = 0.95.

![](../../lab04.v2.png)
Рис. 9. Визуализация результатов фильтрации компонент связности $V_2$ (метод с гауссовым окном)

	Метод connectedComponentsWithStats()
	Оценки качества:
	* Accuracy = 0.996;
	* Precision = 1;
	* Recall = 0.997.

![](../../lab04.e2.png)
Рис. 10. Визуализация отклонений от эталона $E_2$ (метод с гауссовым окном)

### Текст программы

```cpp
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
    Mat img(imread("data/data.jpg"));
    Mat stdImg(imread("data/std.png"));
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
```

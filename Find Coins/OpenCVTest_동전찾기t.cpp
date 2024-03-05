// 엄채윤 202127252 미디어학과
// 2023.11.01
// 동전 개수 세기 프로그램
#include <string>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#ifdef _DEBUG
#pragma comment (lib, "opencv_world480d.lib")
#else
#pragma comment (lib, "opencv_world480.lib")
#endif

using namespace cv;
using namespace std;
using pyramid = std::vector<Mat>; 

Mat findCoins(const Mat& src) {
    Mat dst;
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, gray, Size(0, 0), 0.4665); //sigma best: 0.466
    vector<Vec3f> circles;
    HoughCircles(gray, circles, HOUGH_GRADIENT, 1,
        gray.rows/12.5, // 거리 조절 best: 12.5
        90, 77, // canny threshold 값 high, low 90, 77
        27, 94  // 27, 94
        // 찾는 동전 크기 조절
    );
    int cnt = 0;
    for (size_t i = 0; i < circles.size(); i++)
    {
        Vec3i c = circles[i];
        Point center = Point(c[0], c[1]);
        // 동전 중심 색깔
        circle(src, center, 1, Scalar(255, 0, 255), 3, LINE_AA);
        // 동전 테두리 색깔
        int radius = c[2];
        circle(src, center, radius, Scalar(255, 0, 0), 3, LINE_AA);
        cnt++;
    }
    String cnt_;
    cnt_ = "Coins: " + to_string(cnt);
    putText(src, cnt_, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 0), 2);

    return src;
}

int main()
{
    Mat src0 = imread("D:/대학교/img/CVimg/coins0.jpg");
    Mat dst0 = findCoins(src0);
    imshow("Circles0", dst0);

    Mat src1 = imread("D:/대학교/img/CVimg/coins1.jpg");
    Mat dst1 = findCoins(src1);
    imshow("Circles1", dst1);

    Mat src2 = imread("D:/대학교/img/CVimg/coins2.jpg");
    Mat dst2 = findCoins(src2);
    imshow("Circles2", dst2);

    Mat src3 = imread("D:/대학교/img/CVimg/coins3.jpg");
    Mat dst3 = findCoins(src3);
    imshow("Circles3", dst3);

    Mat src4 = imread("D:/대학교/img/CVimg/coins4.jpg");
    Mat dst4 = findCoins(src4);
    imshow("Circles4", dst4);

    Mat src5 = imread("D:/대학교/img/CVimg/coins5.jpg");
    Mat dst5 = findCoins(src5);
    imshow("Circles5", dst5);


    waitKey();
    
}

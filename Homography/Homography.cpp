// 엄채윤 202127252 미디어학과
// 2023.12
// Homography
#include <string>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
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

int main()
{
    Mat src = imread("card.png");
    //Mat src = imread("D:/대학교/img/card1.png");
    Mat dst, gray, edge, cedge, cedgeP, sortedResult;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    GaussianBlur(src, gray, Size(11, 11), 3.5); // 3은 됨 4는 안됨
    Canny(gray, edge, 230, 150);
    cvtColor(edge, cedge, COLOR_GRAY2BGR);
    vector<Vec2f> lines;
    HoughLines(edge, lines, 1, CV_PI / 120, 70); //theta 수정
   
    cout << "찾은 라인 개수: " << lines.size() << endl;
    
    imshow("blurred", gray);
    imshow("edge", edge);
    imshow("detected lines", cedge);
    
    for (size_t i = 0; i < lines.size(); i++)
    {
        cout << lines[i][0] << " " << lines[i][1] << endl;
        //if (lines[i][0] < 0) lines[i][0] -= lines[i][0];
        float rho = lines[i][0], theta = lines[i][1];
        
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        //xcos(theta)+ysin(theta) = d인데
        //이 식은 왜이러즤...?
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 1000 * (a));
        line(cedge, pt1, pt2, Scalar(0, 0, 255), 2, LINE_AA);

    }
    
    vector<Point3f> points; //점
    for (int i = 0; i < lines.size(); i++) {
        //교점
        Point3f pt;
        float rho = lines[i][0], theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        pt.x = a;
        pt.y = b;
        pt.z = -lines[i][0]; //d가 마이너스면?
        points.push_back(pt); // pt: (cos, sin, d)
    }

    vector<Point2f> crossPoints, sortedCrossPoints_left, sortedCrossPoints_right, sortedCrossPoints;
    for (int i = 0; i < lines.size()-1; i++) {
        for (int j = i+1; j < lines.size(); j++) {
            float theta1 = lines[i][1], theta2 = lines[j][1];
            //string theta1_ = to_string(theta1);
            //string theta2_ = to_string(theta2);
            //if (theta1_.compare(theta2_)) continue;
            if (abs(theta1 - theta2) < 0.01) continue;
            //cross product가 문제인 듯 함
            // l = (cos, sin, d)로 표현 가능
            // (d, 세타) 표현도 가능...
            Point3f tmp1, tmp2, crosstmp_;
            Point2f tmp;
            tmp1 = points[i];
            tmp2 = points[j];
            crosstmp_ = tmp1.cross(tmp2);
            
            cout << "cross 결과: " << crosstmp_ << endl;
            
            tmp.x = crosstmp_.x / crosstmp_.z;
            tmp.y = crosstmp_.y / crosstmp_.z; // points[i].x---
            if (tmp.x < 0 || tmp.y < 0) continue;
            else crossPoints.push_back(tmp);
            cout << "교점: " << tmp << endl;
        }
    }

    //일반적인 점 찾기
    //기울기가 PI/2에 가까운거랑 아닌것 들의 교점을 각각 찾으면 교점 나옴요~
    vector<Point2f> tmpCrossPoints;
    for (int i = 0; i < lines.size(); i++) {
        for (int j = i + 1; j < lines.size(); j++) {
            
        }
    }
    cout << "점 개수: " << crossPoints.size() << endl;
    //y sorting 후 x sorting 하면
    // 1 2 
    // 3 4 로 sort될 것
    //sort(crossPoints)
    //sort(crossPoints, sortedCrossPoints, SORT_ASCENDING); // sort

    cout << "------------------crossPoints 점 순서--------------------" << endl;
    int min_y = src.rows + 1;
    for (int i = 0; i < crossPoints.size(); i++) {
        circle(edge, crossPoints[i], 2, Scalar(255, 0, 0), 3);
        cout << crossPoints[i] << endl;
        if (cvRound(crossPoints[i].y) <= min_y) min_y = cvRound(crossPoints[i].y);
    }
    cout << "minY값: " << min_y << endl;
    vector<Point2f> tmp_up, tmp_down;
    Point2f tmp1, tmp2;
    for (int i = 0; i < crossPoints.size(); i++) {
        if (min_y == cvRound(crossPoints[i].y)) {
            tmp_up.push_back(crossPoints[i]);
        }
        else tmp_down.push_back(crossPoints[i]);
    }
    cout << "left 개수: " << tmp_up.size() << endl;
    cout << "right 개수: " << tmp_down.size() << endl;

    if (tmp_up[0].x < tmp_up[1].x) {
        sortedCrossPoints.push_back(tmp_up[0]);
        sortedCrossPoints.push_back(tmp_up[1]);
        //tmp1 = tmp_up[1];
    }
    else {
        sortedCrossPoints.push_back(tmp_up[1]);
        sortedCrossPoints.push_back(tmp_up[0]);
        //tmp1 = tmp_up[0];
    }
    if (tmp_down[0].x < tmp_down[1].x) {
        sortedCrossPoints.push_back(tmp_down[0]);
        sortedCrossPoints.push_back(tmp_down[1]);
    }
    else {
        sortedCrossPoints.push_back(tmp_down[1]);
        sortedCrossPoints.push_back(tmp_down[0]);
        //tmp2 = tmp_down[0];
    }
    //sortedCrossPoints.push_back(tmp1);
    //sortedCrossPoints.push_back(tmp2);
    for (int i = 0; i < sortedCrossPoints.size(); i++)
        cout << "sortedCrossPoints: " << sortedCrossPoints[i] << endl;
    Size warpSize(src.cols, src.rows);
    Mat warpImg(warpSize, COLOR_GRAY2BGR);
    
    vector<Point2f> getPerspective(4);
    getPerspective[3] = Point2f(0, 0);
    getPerspective[1] = Point2f(0, warpImg.rows);
    getPerspective[0] = Point2f(warpImg.cols, warpImg.rows);
    getPerspective[2] = Point2f(warpImg.cols, 0);
    Mat perspective = getPerspectiveTransform(crossPoints, getPerspective);
    
    //Sort Test
    Mat warpImg2(warpSize, COLOR_GRAY2BGR);
    vector<Point2f> getSortedPerspective(4);
    getSortedPerspective[0] = Point2f(0, 0);
    getSortedPerspective[1] = Point2f(warpImg2.cols, 0);
    getSortedPerspective[2] = Point2f(0, warpImg2.rows);
    getSortedPerspective[3] = Point2f(warpImg2.cols, warpImg2.rows); 
    Mat perspectiveSorted = getPerspectiveTransform(sortedCrossPoints, getSortedPerspective);
    
    
    cout << perspective << endl;
    cout << perspectiveSorted << endl;
    
    warpPerspective(src, dst, perspective, warpSize);
    warpPerspective(src, sortedResult, perspectiveSorted, warpSize);

    
    imshow("src", src);
    imshow("blurred", gray);
    imshow("edge", edge);
    imshow("detected lines", cedge);
    imshow("Homography", dst);
    imshow("sortedResult", sortedResult);

    waitKey();
    
}

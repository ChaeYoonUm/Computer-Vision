// 엄채윤 202127252 미디어학과
// 2023.12.11
// Image Stitching 프로그램
#include <string>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#ifdef _DEBUG
#pragma comment (lib, "opencv_world480d.lib")
#else
#pragma comment (lib, "opencv_world480.lib")
#endif

using namespace cv;
using namespace std;

int main()
{
    Mat src1 = imread("D:/대학교/img/img1.jpg");
    Mat src2 = imread("D:/대학교/img/img2.jpg");
   
    resize(src1, src1, Size(600, 800));
    resize(src2, src2, Size(600, 800));
    Mat gray_src1, dst_src1;
    Mat gray_src2, dst_src2;
    
    //Image1 특징점 찾기
    Mat copy_src1 = src1.clone();
    cvtColor(copy_src1, gray_src1, COLOR_BGR2GRAY);
    vector< Point2f> featurePoints_scr1;
    goodFeaturesToTrack(gray_src1, featurePoints_scr1, 300, 0.01, 15);
    for (int i = 0; i < featurePoints_scr1.size(); i++) {
        circle(copy_src1, featurePoints_scr1[i],3, Scalar(0,255,255), 2);
    }
    
    //Image2 특징점 찾기
    Mat copy_src2 = src2.clone();
    cvtColor(copy_src2, gray_src2, COLOR_BGR2GRAY);
    vector< Point2f> featurePoints_src2;
    goodFeaturesToTrack(gray_src2, featurePoints_src2, 300, 0.01, 15);
    for (int i = 0; i < featurePoints_src2.size(); i++) {
        circle(copy_src2, featurePoints_src2[i], 3, Scalar(0, 255, 255), 2);
    }
    
    //Descriptor 생성 및 Matching 
    Mat descriptor1, descriptor2;
    vector<KeyPoint> keyPt1, keyPt2;
    Ptr<SIFT> feature_src1 = SIFT::create();
    Ptr<SIFT> feature2_src2 = SIFT::create();
    
    feature_src1-> detectAndCompute(gray_src1, Mat(), keyPt1, descriptor1);
    feature2_src2-> detectAndCompute(gray_src2, Mat(), keyPt2, descriptor2);
    
    //Brute Force Match 하기
    Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_L2);
    vector<DMatch> match;
    vector<DMatch> topGoodMatch;
    matcher->match(descriptor1, descriptor2, match);    
    
    //match 거리 짧은 순으로 정렬 및 top 25 match 선별 저장 (보다 정확한 match찾기)
    sort(match.begin(), match.end());
    for (int i = 0; i < 25; i++) {
        topGoodMatch.push_back(match[i]);
    }

    Mat outImg;
    drawMatches(src1, keyPt1, src2, keyPt2, topGoodMatch, outImg,
        cv::Scalar(0, 255, 255), cv::Scalar(-1), vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    
    //top 25 Match에서 point 추출
    vector<Point2f> pt1, pt2;
    for (int i = 0; i < topGoodMatch.size(); i++) {
        pt1.push_back(keyPt1[topGoodMatch[i].queryIdx].pt);
        pt2.push_back(keyPt2[topGoodMatch[i].trainIdx].pt);
    }
    
    //homography 찾기
    Mat homo = findHomography(pt2, pt1, RANSAC);
    cout << homo << endl;
    
    //그림 합치기
    Size stitchedImgSize(src1.cols * 2, src1.rows*2);
    
    Mat warpImg2;
    warpPerspective(src2, warpImg2, homo, Size(src2.cols*2, src2.rows*2));
    
    Mat stitchedImg(stitchedImgSize, CV_8UC3);
    Mat roiImg1(stitchedImg, Rect(0, 0, src1.cols, src1.rows));
    Mat roiImg2(stitchedImg, Rect(0,0, warpImg2.cols, warpImg2.rows));
    warpImg2.copyTo(roiImg2);
    src1.copyTo(roiImg1);
    
    /*imshow("featureDetected1", copy_src1);
    imshow("featureDetected2", copy_src2);
    imshow("warpedImg", warpImg2);*/
    imshow("drawMatch", outImg);
    imshow("stitchedImg", stitchedImg);
    waitKey();
    
}

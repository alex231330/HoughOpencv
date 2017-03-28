//
//  main.cpp
//  HoughOpencv
//
//  Created by Алексадр Тюльпанов on 23.03.17.
//  Copyright © 2017 Алексадр Тюльпанов. All rights reserved.
//
#include <iostream>
#include <vector>
#include <algorithm>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <math.h>

using namespace std;
using namespace cv;

#define PointVal(img,x,y,c) (*((unsigned char*)(img->imageData+y*img->widthStep+x*img->nChannels+c)))

const int rad = 180 / abs(acos(-1));
double theta = rad * -90;
int xpoint = 0;

Mat rotateImage(const Mat& source, double angle)
{
    Point2f src_center(source.cols/2.0F, source.rows/2.0F);
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
    Mat dst;
    warpAffine(source, dst, rot_mat, source.size());
    return dst;
}


double* fillCosArray(double* ar1, int size) {
    for (int i = 0; i < size; i++) {
        ar1[i] = cos(theta);
        theta += rad;
    }
    return ar1;
}

double* fillSinArray(double* ar2, int size) {
    for (int i = 0; i < size; i++) {
        ar2[i] = sin(theta);
        theta += rad;
    }
    return ar2;
}

int main(int argc, const char * argv[]) {
    double coss[20];
    double sins[20];
    fillCosArray(coss, 20);
    fillSinArray(sins, 20);
    //CvCapture* capture = cvCreateCameraCapture(0);
    IplImage* image;
    image = cvLoadImage("/Users/alex231330/Downloads/test.jpg");
    IplImage* grayscale = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
    IplImage* sobel = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
    IplImage* canny = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
    double maxIn = 0;
    cvSmooth(image, image, CV_GAUSSIAN);
    cvCvtColor(image, grayscale, CV_BGR2GRAY);
    cvSobel(grayscale, sobel, 0, 1, 3);
    cvCanny(sobel, canny, 70, 130);
    int RMax = cvRound(sqrt((double)(image->width * image->width + image->height * image->height)));
    while(true) {
        //image = cvQueryFrame(capture);
     //   int diagonal = (int)(sqrt(canny->width * canny->width + canny->height * canny->height));
        IplImage* gray = cvCreateImage(cvSize(RMax, 180), IPL_DEPTH_16U, 1);
        
        for(int y = 0; y < canny->height; y++){
            uchar* ptr = (uchar*) (canny->imageData + y * canny->widthStep);
            for(int x = 0; x < canny->width; x++){
                if(ptr[x]>0){ // это пиксель контура?
                    // рассмотрим все возможные прямые, которые могут
                    // проходить через эту точку
                    for(int f = 0; f < 180; f++){ //перебираем все возможные углы наклона
                        short* ptrP = (short*)(gray->imageData + f * gray->widthStep);
                        for(int r = 0; r < RMax; r++){ // перебираем все возможные расстояния от начала координат
                            int theta = f * CV_PI / 180.0; // переводим градусы в радианы
                            // Если решение уравнения достаточно хорошее (точность больше заданой)
                            if ( abs(((y) * sin(theta) + (x) * cos(theta)) - r) < 0.1 ){
                                ptrP[r]++; // увеличиваем счетчик для этой точки фазового пространства.
                            }
                        }
                    }
                }
            }
        }
        
        // find the hot point
        for (int y  = 0; y <= gray->height; y++) {
            for (int x = 0; x <= gray->width; x++) {
                if (PointVal(gray, x, y, 0) == 0) continue;
                for (int i = 0; i <= 180; i++) {
                    if (PointVal(gray, RMax, i, 1) > maxIn) {
                        maxIn = PointVal(gray, RMax, i, 1);
                        xpoint = i;
                    }
                    PointVal(gray, RMax, i, 1) = 0;
                }
            }
        }
        double thp =  (abs(acos(-1) / 180) * -90) + (abs(acos(-1) / 180)) * xpoint;
        double angle = (90 - abs(thp) * (180 / abs(acos(-1))) * (thp < 0 ? -1 : 1));
        cout << angle << "\n";
        cvShowImage("Gray scale", grayscale);
        cvShowImage("Sobel", sobel);
        cvShowImage("Canny", canny);
        cvShowImage("Out", gray);
        if (waitKey(100) == 27)
        {
            cout << "esc key is pressed" << endl;
        }
    }
    return 0;
}

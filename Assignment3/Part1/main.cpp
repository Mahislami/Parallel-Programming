#include <iostream>
#include <x86intrin.h>
#include "opencv2/opencv.hpp"
#include <ipp.h>
#include <math.h>
using namespace std;
using namespace cv;

int main() {
    
    cout << "SIDs : 810195548 , 810195346" << endl << "----------------------------" <<endl;
    Mat firstImg;
    Mat secondImg;
    Mat result;
    Ipp64u start;
    Ipp64u end;

    uint8_t* firstImgPixel;
    uint8_t* secondImgPixel;
    uint8_t* resultImgPixel;
    int serialTime, parallelTime;
    int rowN, colN;
    int index;
    __m128i* firstImgParallelPixel;
    __m128i* secondImgParallelPixel;
    __m128i* resultImgParallelPixel;
    __m128i firstImgVec, secondImgVec;
    __m128i sub_0, sub_1;

    firstImg = imread("CA03__Q1__Image__01.png", IMREAD_GRAYSCALE);
    secondImg = imread("CA03__Q1__Image__02.png", IMREAD_GRAYSCALE);
    
    rowN = firstImg.rows;
    colN = firstImg.cols;
    firstImgPixel = (uint8_t*) firstImg.data;
    secondImgPixel = (uint8_t*) secondImg.data;
    firstImgParallelPixel = (__m128i*) firstImg.data;
    secondImgParallelPixel = (__m128i*) secondImg.data;
    result.create(firstImg.rows, firstImg.cols, CV_8U);
    resultImgPixel = (uint8_t*) result.data;
    resultImgParallelPixel = (__m128i*) result.data;

    if(firstImg.empty()) {
        cout << "File 1 Not Found!" << endl;
        return 1;
    }
    if(secondImg.empty()) {
        cout << "File 2 Not Found!" << endl;
        return 1;
    }
    
    /*------------** Serial **------------*/
    start = ippGetCpuClocks();
    for(int i = 0; i < rowN; i++) {

        for(int j = 0; j < colN; j++) {
            index = i * colN + j;
            resultImgPixel[index] = abs(secondImgPixel[index] - firstImgPixel[index]);
        }
    }
    end = ippGetCpuClocks();
    serialTime = (Ipp32u)(end - start);
    cout << "Serial Runtime: " << serialTime << endl;
    imwrite("serialResult.png", result);
    //imshow("Serial", result);

    /*-----------** Parallel **-----------*/
    start = ippGetCpuClocks();
    for(int i = 0; i < rowN; i++) {

        for(int j = 0; j < colN/16; j++) {

            index = (i * colN/16 + j);
            firstImgVec = _mm_loadu_si128(&firstImgParallelPixel[index]);
            secondImgVec = _mm_loadu_si128(&secondImgParallelPixel[index]);
            sub_0 = _mm_subs_epu8(firstImgVec, secondImgVec);
            sub_1 = _mm_subs_epu8(secondImgVec, firstImgVec);
            sub_0 = _mm_or_si128(sub_0, sub_1);
            _mm_store_si128(&resultImgParallelPixel[index], sub_0);
        }
    }
    end = ippGetCpuClocks();
    parallelTime = (Ipp32u)(end - start);
    imwrite("parallelResult.png", result);
    //imshow("Parallel", result);

    cout << "Parallel Runtime: " << parallelTime << endl;
    cout << "Speed-up : " << 100*(serialTime - parallelTime)/((float)serialTime) << endl;
    return 0;
}

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include "x86intrin.h"
#include <sys/time.h>

using namespace cv;
using std::endl;
using std::cout;

#define ALPHA 0.5

int main( void )
{
	struct timeval start1, end1;
	struct timeval start2, end2;

	Mat in_img1 = imread( "CA03__Q2__Image__01B.png" ,  IMREAD_GRAYSCALE);
	Mat in_img2 = imread( "CA03__Q2__Image__02B.png" ,  IMREAD_GRAYSCALE);

	unsigned int NCOLSA = in_img1.cols;
	unsigned int NROWSA = in_img1.rows;

	unsigned int NCOLSB = in_img2.cols;
	unsigned int NROWSB = in_img2.rows;


	unsigned char *in_image1;
	unsigned char *in_image2;

	in_image1  = (unsigned char *) in_img1.data;
	in_image2  = (unsigned char *) in_img2.data;

    gettimeofday(&start1, NULL);
	for (int row = 0; row < NROWSB; row++)
		for (int col = 0; col < NCOLSB; col++){
		  *(in_image1 + row * NCOLSA + col) += (*(in_image2 + row * NCOLSB + col) * ALPHA);
          if( *(in_image1 + row * NCOLSA + col) < (*(in_image2 + row * NCOLSB + col) * ALPHA))
             *(in_image1 + row * NCOLSA + col) =255;
    }

    gettimeofday(&end1, NULL);
	long seconds1 = (end1.tv_sec - start1.tv_sec);
	long micros1 = ((seconds1 * 1000000) + end1.tv_usec) - (start1.tv_usec);

	imshow( "Seriall output", in_img1 ); 
    waitKey(0);

 	Mat in_img3 = imread( "CA03__Q2__Image__01B.png" ,  IMREAD_GRAYSCALE);
  	Mat in_img4 = imread( "CA03__Q2__Image__02B.png" ,  IMREAD_GRAYSCALE);

    __m128i *pSrc1;
	__m128i *pSrc2;
	__m128i *pRes;
	__m128i m1, m2, m3, m4;
	Mat out_img2 (in_img1.rows, in_img1.cols, CV_8U) ;

	pSrc1 = (__m128i *) in_img3.data;
	pSrc2 = (__m128i *) in_img4.data;
	pRes = (__m128i *) out_img2.data;

    gettimeofday(&start2, NULL);

	for (int i = 0; i < NROWSB; i++)
		for (int j = 0; j < NCOLSB / 16; j++)
		{
			m1 = _mm_loadu_si128(pSrc1 + i * NCOLSA/16 + j) ;
			m2 = _mm_loadu_si128(pSrc2 + i * NCOLSB/16 + j) ;
            m2 = _mm_srli_epi16(m2, 1);
            m2 = _mm_and_si128(m2, _mm_set1_epi8(0x7F));
            m2 = _mm_adds_epu8(m1,m2);
			_mm_storeu_si128 (pSrc1 + i * NCOLSA/16 + j, m2);
		}
    gettimeofday(&end2, NULL);
	long seconds2 = (end2.tv_sec - start2.tv_sec);
	long micros2 = ((seconds2 * 1000000) + end2.tv_usec) - (start2.tv_usec);

	imshow( "Parallel output", in_img3 );
	waitKey(0);

	cout << "Serial Run time is " << seconds1  << " seconds and " << micros1 << " micro seconds\n\n";
    cout << "Parallel Run time is " << seconds2  << " seconds and " << micros2 << " micro seconds\n\n";
	printf ("Speedup = %f\n\n", (float) (micros1)/(float) micros2);

	return 0;
}

#pragma once
#include "../stdafx.h"

Mat YaeDetector(Mat &blackMask,Mat &bigKernel,Mat &smallKernel) {
	Mat invertedMask,cavityMask,cavityMaskWithNoise,cavityMaskNoNoise,cleary,noisy,floodedMask,flooededCanvasMask,floodedClearyMask,floodedNoisyMask,largeObject,subtitle;
	Scalar zeroHolder = Scalar(0);
	int width = blackMask.cols;
	int height = blackMask.rows;
	int count = height * width;
	int i,x, y;
	uchar *largeObjectPtr, *cavityMaskWithNoisePtr, *clearyPtr;
	//��� 2: ��Ѻ��
	bitwise_not(blackMask, invertedMask);

	//��� 3: ����
	cavityMask = invertedMask;
	floodedMask = Mat::zeros(height + 2, width + 2, CV_8U);
	
	for (y = 0; y < height; y++) {
		floodFill(cavityMask, floodedMask, Point(0, y), zeroHolder);
		floodFill(cavityMask, floodedMask, Point(width - 1, y), zeroHolder);
	}
	for (x = 0; x < width; x++) {
		floodFill(cavityMask, floodedMask, Point(x, 0), zeroHolder);
		floodFill(cavityMask, floodedMask, Point(x, height - 1), zeroHolder);
	}

	//��鹷�� 4: �Ҿ�鹷���˭��Թ���Ҿ
	erode(cavityMask, largeObject, bigKernel);
	//��鹷�� 5: ���ѵ�ط���˭��Թ�
	flooededCanvasMask = Mat::zeros(height + 2, width + 2, CV_8U);
	cavityMaskWithNoise = cavityMask.clone();
	largeObjectPtr = largeObject.data;
	cavityMaskWithNoisePtr = cavityMaskWithNoise.data;
	for (i = 0; i < count; i++) {
		if (largeObjectPtr[i] != 0 && cavityMaskWithNoisePtr[i] != 0) {
			floodFill(cavityMaskWithNoise, floodedMask, Point(i % width, i / width), zeroHolder);
		}
	}
	//��� 6: ź��鹷������Թ��͡�ҡ�Ҿ
	morphologyEx(cavityMaskWithNoise, cavityMaskNoNoise, MORPH_OPEN, smallKernel);
	//��鹷�� 7:�� connected component labeling ���������ǹ����ͧ��������������ѡ����������ҡ�ѡ	
	cleary = cavityMaskNoNoise;
	noisy = cavityMaskWithNoise.clone();
	clearyPtr = cleary.data;
	for (i = 0; i < count; i++) {
		if (clearyPtr[i] != 0) {
			floodFill(cleary, floodedClearyMask, Point(i % width, i / width), zeroHolder);
			floodFill(noisy, floodedNoisyMask, Point(i % width, i / width), zeroHolder);
		}
	}
	bitwise_xor(cavityMaskWithNoise, noisy, subtitle);
	return subtitle;
}
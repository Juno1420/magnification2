#include "VideoMagnification.h"
#include "VolumeMagnification.h"
#include "GraphMagnification.h"


int main()
{
	float spatialFilter[5]={0.0625, 0.2500, 0.3750, 0.2500, 0.0625};
//	VideoMagnification<cv::Vec3b> gridmagnify(5,spaceFilter,LAPLACIAN,3);
//	gridmagnify.ReadData(std::string("penguin.jpg"));

//	VolumeMagnification<cv::Vec3b> volumemagnify(5,spaceFilter,LAPLACIAN,3);
//	volumemagnify.ReadData("face.avi");

/*	VideoMagnification<float> video(30, 5, spatialFilter, GAUSS, 3, BUTTERWORTH);
	video.ReadAdjacentMatrix();
	video.Write2Video("adjacent.avi",DATAPYRAMID);*/

	Magnification<float> *pMagData;
	GraphMagnification<float> graph(30,LAPLACIAN,3,IDEAL);
	pMagData = &graph;
	pMagData->ReadData(std::string("sim4_2hz.avi"));


/*	VideoMagnification<float> video(30, 5, spatialFilter, GAUSS, 3, IDEAL);
	video.ReadData(std::string("sim4_2hz.avi"));
	int nWaves = 3;
	float temporalFL[4] = {2.8, 4.5, 6.5};
	float temporalFH[4] = {3.2, 5.5, 7.5};
	float magnifyRatio[4] = {10, 10, 10};
	video.SetIdealFilter(nWaves, temporalFL, temporalFH, magnifyRatio);
	video.MagnifyWithIdealFilter();
	video.Write2Video("sss.avi",MAGNIFIEDPYRAMID);*/

/*	int rows,cols;
	rows = 3;
	cols = 7;
	int optimalRows = getOptimalDFTSize( rows );
	int optimalCols = getOptimalDFTSize( cols );
	Mat paddedRe(optimalRows, optimalCols, CV_32FC1, Scalar::all(0));
	Mat paddedIm(optimalRows, optimalCols, CV_32FC1, Scalar::all(0));//∏µ¿Ô“∂±‰ªª–È≤ø
	float* data;
	data = paddedRe.ptr<float>(0);
	data[0] = 0; data[1] = 1; data[2] =2; data[3] = 3; data[4] = 2; data[5] =1; data[6] =0;  
	data = paddedRe.ptr<float>(1);
	data[0] = 1; data[1] = 1; data[2] =1; data[3] = -1; data[4] = -1; data[5] = -1; data[6] =0;  
	data = paddedRe.ptr<float>(2);
	data[0] = -5; data[1] = -4; data[2] = -3; data[3] = -2; data[4] = -1; data[5] =0; data[6] =1;  
	Mat planes[] = {paddedRe, paddedIm};
	Mat dftSrc, dftDst(optimalRows, optimalCols, CV_32FC2);
	merge(planes, 2, dftSrc); 
	dft(dftSrc, dftDst, CV_DXT_ROWS, 0);

	float *ff, a1, a2;
	for(int i = 0; i < optimalRows; i ++){
		ff = dftSrc.ptr<float>(i);
		for(int j = 0; j < optimalCols; j ++){
			a1 = ff[j * 2];
			a2 = ff[j * 2 + 1];
		}
	}

	Mat fData(optimalRows, optimalCols, CV_32FC2, Scalar::all(0));
	float* fd;
	for(int i = 0; i < optimalRows; i ++){
		fd = fData.ptr<float>(i);
		data = dftDst.ptr<float>(i);
		for(int j = 0; j < optimalCols; j ++){
			fd[j * 2] = data[j * 2];
			fd[j * 2 + 1] = data[j * 2 + 1];
		}
	}
	Mat invFData(optimalRows, optimalCols, CV_32FC2);
	dft(fData, invFData, CV_DXT_INVERSE_SCALE|CV_DXT_ROWS, rows);

	for(int i = 0; i < optimalRows; i ++){
		ff = invFData.ptr<float>(i);
		for(int j = 0; j < optimalCols; j ++){
			a1 = ff[j * 2];
			a2 = ff[j * 2 + 1];
		}
	}*/

	return 0;
}
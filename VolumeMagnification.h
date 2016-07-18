#pragma once
#include "GridMagnification.h"

template<typename T> class VolumeMagnification:
public GridMagnification<T, 3>{
public:
	VolumeMagnification(unsigned int filterLeng, float* filter, float* magFactor, PYRAMID_TYPE pType = Gauss, unsigned int nLevels = 1)
		:GridMagnification(filterLeng, filter, magFactor, pType, nLevels)
	{ };

	virtual void ReadData(std::string fileName);
};

template<typename T>
void VolumeMagnification<T>::ReadData(std::string fileName)
{
	dataName = fileName;
	cv::VideoCapture capture(fileName);
	if(!capture.isOpened()){
		std::cout<<"Cannot open the video.";
		exit(-1);
	}

	frameRate = capture.get(CV_CAP_PROP_FPS);
	int delay = 1000/frameRate;
	nFrames = 1;

/*	//视频时间维在第0维
	dataLength[0][0] = static_cast<unsigned int> (capture.get(CV_CAP_PROP_FRAME_COUNT));
	dataLength[0][1] = static_cast<unsigned int> (capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	dataLength[0][2] = static_cast<unsigned int> (capture.get(CV_CAP_PROP_FRAME_WIDTH));;
	
	cv::Mat frame;
	cv::namedWindow("video");
	bool stop = false;
	cv::Mat_<T>::iterator it, itend;
	T* frameData = new T[dataLength[0][0] * dataLength[0][1] * dataLength[0][2]];
	int p = 0;
	int nf = 0;
	while(!stop){
		if(!capture.read(frame))
			break;
		itend= frame.end<T>();
		for (it= frame.begin<T>() ; it!= itend; ++it) {  
			frameData[p++] = (*it);
		} 
		cv::imshow("video",frame);
		if(cv::waitKey(delay) >= 0)
			stop = true;

		nf ++;
		if(nf >= 100){
			stop = true;
			dataLength[0][0] = nf;
		}
	}

	capture.release();

	data.push_back(frameData);
	BuildPyramid();


	cv::Mat result;
	result.create(dataLength[1][1],dataLength[1][2],frame.type());
	int nRow = result.rows;
	int nCol = result.cols;
	int nPixel = nRow * nCol;
	cv::namedWindow("ShowResult");

	for(int f = 0; f < dataLength[1][0]; f ++){
		for(int i = 0; i < nRow; i ++){
			for(int j = 0; j < nCol; j ++){
				result.at<T>(i,j) = dataPyramid[0][f * nPixel + i * nCol + j];
			}
		}
		cv::imshow("ShowResult",result);
		if(cv::waitKey(delay) >= 0)
			stop = true;
	}*/

	//视频时间维在第2维
	dataLength[0][2] = static_cast<unsigned int> (capture.get(CV_CAP_PROP_FRAME_COUNT));
	dataLength[0][0] = static_cast<unsigned int> (capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	dataLength[0][1] = static_cast<unsigned int> (capture.get(CV_CAP_PROP_FRAME_WIDTH));;
	
	int nf = 100;
	dataLength[0][2] = nf;
	cv::Mat frame;
	bool stop = false;
	cv::Mat_<T>::iterator it, itend;
	T* frameData = new T[dataLength[0][0] * dataLength[0][1] * dataLength[0][2]];
	int p = 0;
	int pagestep = dataLength[0][1] * dataLength[0][2];
	while(!stop){
		if(!capture.read(frame))
			break;
		it= frame.begin<T>();
		for (int i = 0; i < dataLength[0][0]; i ++){  
			for(int j = 0; j < dataLength[0][1]; j ++){
				frameData[i * pagestep + j * dataLength[0][2] + p] = (*it);
				it ++;
			}
		} 
		p ++;

		if(cv::waitKey(delay) >= 0)
			stop = true;

		if(p >= nf){
			stop = true;
		}
	}

	capture.release();
	data.push_back(frameData);

	cv::Mat result;
	result.create(dataLength[0][1],dataLength[0][2],frame.type());
	int nRow = result.rows;
	int nCol = result.cols;
	int nPixel = nRow * nCol;
	cv::namedWindow("ShowResult");

	for(int f = 0; f < dataLength[0][0]; f ++){
		for(int i = 0; i < nRow; i ++){
			for(int j = 0; j < nCol; j ++){
				result.at<T>(i,j) = data[0][f * nPixel + i * nCol + j];
			}
		}
		cv::imshow("ShowResult",result);
		if(cv::waitKey(delay) >= 0)
			   
			stop = true;
	}

	BuildPyramid();

	int level = 1;
	result.create(dataLength[level][0],dataLength[level][1],frame.type());
	nRow = result.rows;
	nCol = result.cols;
	nPixel = dataLength[level][1] * dataLength[level][2];

	int dataSize = 0;
	for(int i = 0; i < level; i ++){
		dataSize += pyrSize[i];
	}
	for(int f = 0; f < dataLength[level][2]; f ++){
		for(int i = 0; i < nRow; i ++){
			for(int j = 0; j < nCol; j ++){
				result.at<T>(i,j) = dataPyramid[0][dataSize + i * nPixel + j * dataLength[level][2] + f];
			}
		}
	//	if(f ==50)
		cv::imshow("ShowResult",result);
		if(cv::waitKey(delay*10) >= 0)
			stop = true;
		
	}

	cv::waitKey(0);

}
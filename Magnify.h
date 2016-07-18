#pragma once
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <fstream>
#include "cv.h"
#include "highgui.h"
using namespace cv;
using namespace std;
#define PI 3.1415926
#define MAX_DFT_SIZE 16384

typedef enum{LAPLACIAN, GAUSS} PYRAMID_TYPE;
typedef enum{IDEAL, BUTTERWORTH} TEMPORAL_FILTER_TYPE;
typedef enum{ORIGINAL, FOURIER} PYRAMID_DATA;
typedef enum{ORIGINALDATA, DATAPYRAMID, MAGNIFIEDPYRAMID} DATA_SOURCE;

class Filter{
public:
	Filter(){};
	Filter(int nL, float *low, float* high, float* mag){SetLobes(nL, low, high, mag);};
	~Filter(){};
	float lowBoundary[10];
	float highBoundary[10];
	float magnifyRatio[10];
	unsigned int nLobes;

	void SetLobes(int nL, float* low, float* high, float *mag){
		nLobes = nL;
		for(int i = 0; i < nLobes; i ++){
			lowBoundary[i] = low[i];
			highBoundary[i] = high[i];
			magnifyRatio[i] = mag[i];
		}
	};

	void CreateMask(float* mask, int nFrames, float samplingRate = 1)
	{
		for(int i = 0; i < nFrames; i ++){
			mask[i] = 0;
		}
		for(int w = 0; w < nLobes; w ++){
			int begin = lowBoundary[w] * nFrames / samplingRate;
			int end = (highBoundary[w]+0.000001) * nFrames / samplingRate;
			for(int i = begin; i < end; i ++){
				mask[i] = magnifyRatio[w];
			}
		}
	};
};

typedef enum{RECTANGLE, ELLIPSE} AREATYPE; 
class Area{
public:
	Area(){};
	~Area(){};
	void SetRectangle(unsigned int xl, unsigned int xh, unsigned int yl,unsigned int yh, Filter f){
		xlow = xl; xhigh = xh; ylow = yl; yhigh = yh;
		type = RECTANGLE;
		filter = f;
	};
	unsigned int xlow,xhigh,ylow,yhigh;
	AREATYPE type;
	Filter filter;
};

class TreeNode
{
public:
	unsigned int dataIndex;//此节点对于的数据标号
	unsigned int nChildren;
	unsigned int startChild, endChild;
};


template<typename T> class Magnification
{
public:
/*	Magnification(){
		pyrType = GAUSS;
		nPyrLevels = 1;
	};*/
	Magnification(float samplingRate = 1, PYRAMID_TYPE pType = GAUSS, unsigned int nLevels = 1, TEMPORAL_FILTER_TYPE tfType = IDEAL)
	{
		SetPyramidAttrib(pType, nLevels);
		SetTemporalAttrib(samplingRate, tfType);
		SetMagnifyInLevels();
		dataArea = NULL;
		nArea = 0;
	};
	~Magnification(){};


	virtual void SetPyramidAttrib(PYRAMID_TYPE pType, unsigned int nL)
	{
		pyrType = pType;
		nPyrLevels = nL;
	};

	void SetTemporalAttrib(float samplingRate, TEMPORAL_FILTER_TYPE tfType)
	{ 
		frameRate = samplingRate;
		temporalFilterType = tfType;
	};

	virtual void SetMagnifyInLevels(float *mag = NULL)
	{
		for(int i = 0; i < 10; i ++){
			magnifyInLevels[i] =  1;
		}
	}
 
	void BuildPyramid();
	string GetPyramidName(string ext = "");
	virtual void GetPyrSize() = 0; 
	virtual void DownSampling(T* data, T* result, unsigned int level = 0, T* tempBuffer = NULL) = 0;
	virtual void UpSampling(T* src, T* dst, unsigned int level) = 0;
	void BuildDataPyramid();
	void BuildFourierPyramid();
	bool ReadPyramid(string pyrName, PYRAMID_DATA pyrDataType);
	bool WritePyramid(string pyrName, PYRAMID_DATA pyrDataType);
	virtual void ReadSpecificInfo(ifstream& infile){};
	virtual void WriteSpecificInfo(ofstream& outfile){};
	void DataDifference(T* src1, T* src2, unsigned int n);
	void DataAdd(T* src1, T* src2, unsigned int n);
	void CollapsePyramid(T *pyramid, T* dst, T* buffer = 0);
	virtual void CreateDataArea() = 0;
	void CreateMagnifiedData();
//	std::vector<T*> pyramid;
	std::vector<T*> dataPyramid;//原始数据的金字塔。
	std::vector<T*> magnifiedPyramid;
	float* fourierPyramid;//傅里叶变换后的金字塔数据, 存放时以像素在所有帧的数据连续存储	
//	std::vector<T*> magnifiedPyramid;//进行时域放大处理后的金字塔(也可能是傅里叶域或者原始域的)
	PYRAMID_TYPE pyrType;
	TEMPORAL_FILTER_TYPE temporalFilterType;
	unsigned int nPyrLevels;//金字塔层数
	unsigned int pyrSize[10];//各层数据量
	unsigned int totalPyrSize;
	unsigned int nFrames;//帧数
	float frameRate;
	string dataName;
	unsigned int nVertexs;//从GraphMagnification子类移动到此父类中
//	IdealFilter idealFilter;
	float magnifyInLevels[10];//各层的放大倍数可以不同

//	TreeNode *dataTree[100];
	unsigned char *dataArea;
	Area areaList[10];
	unsigned int nArea;

	std::vector<T*> data;
	std::vector<T*> magnifiedData;//有了放大效果后的数据
	virtual void ReadData(std::string fileName)
	{
		dataName = fileName;
		std::cout<<"ReadData() of the base class Magnification"<<std::endl;
	};
	void MagnifyWithIdealFilter();
//	void SetIdealFilter(IdealFilter &filter){idealFilter = filter;};
//	void SetIdealFilter(int nW, float* low, float* high, float *mag)
//	{
//		idealFilter.SetParameters(nW, low, high, mag);
//	};

	void UpdateAreas(unsigned int nA, Area* alist)
	{
		nArea = nA;
		for(int i = 0; i < nArea; i ++){
			areaList[i] = alist[i];
		}
		CreateDataArea();
		MagnifyWithIdealFilter();
		CreateMagnifiedData();
	};
};

template<typename T> 
string Magnification<T>::GetPyramidName(string ext = "")
{
	string dataPyrName;
	unsigned dotpos=dataName.find_last_of(".");

	if(ext != ""){
		dataPyrName = dataName.substr(0,dotpos) + ext;
		return dataPyrName;
	}

	if(pyrType == GAUSS){
		if(temporalFilterType == IDEAL){
			dataPyrName = dataName.substr(0,dotpos) + ".fgpyr";
		}
		else{
			dataPyrName = dataName.substr(0,dotpos) + ".gpyr";
		}
	}
	else{
		if(temporalFilterType == IDEAL){
			dataPyrName = dataName.substr(0,dotpos) + ".flpyr";
		}
		else{
			dataPyrName = dataName.substr(0,dotpos) + ".lpyr";
		}
	}
	return dataPyrName;
}

template<typename T> 
void Magnification<T>::DataDifference(T* src1, T* src2, unsigned int n)
{
	for(int i = 0; i < n; i ++){
		src1[i] = src1[i] - src2[i];
	}
}

template<typename T> 
void Magnification<T>::DataAdd(T* src1, T* src2, unsigned int n)
{
	for(int i = 0; i < n; i ++){
		src1[i] = src1[i] + src2[i];
	}
}

template<typename T> 
void Magnification<T>:: BuildPyramid()
{
	int i, j, f, leng[10][5];
	T *origin, *result, *temp, *levelData, *resultCopy;

	GetPyrSize();

	if(!dataArea){
		dataArea = new unsigned char[totalPyrSize];
	}
	memset(dataArea, 0, totalPyrSize * sizeof(unsigned char));

	string pyrName;

	if(temporalFilterType == IDEAL){
		pyrName = GetPyramidName();//取傅里叶金字塔文件名字
		if(ReadPyramid(pyrName, FOURIER)){//假如有金字塔数据文件存在，则读入
			return;
		}

		//从数据金字塔产生傅里叶金字塔，首先读入或者产生数据金字塔
		string dataPyrName;
		if(pyrType == GAUSS)
			dataPyrName = GetPyramidName(".gpyr");
		else
			dataPyrName = GetPyramidName(".lpyr");

		if(!ReadPyramid(dataPyrName, ORIGINAL)){
			BuildDataPyramid();
			//将原始数据的金字塔数据写入文件 
			WritePyramid(dataPyrName, ORIGINAL);
		}
		//产生fourier金字塔
		BuildFourierPyramid();
		//将傅里叶金字塔写入文件
		WritePyramid(pyrName, FOURIER);
	}
	else{
		pyrName = GetPyramidName();
		if(!ReadPyramid(pyrName, ORIGINAL)){//假如金字塔数据文件不存在，则产生并写入文件
			BuildDataPyramid();
			//将原始数据的金字塔数据写入文件 
			WritePyramid(pyrName, ORIGINAL);
		}
	}
}

template<typename T>
void Magnification<T>::BuildDataPyramid()
{
	T *origin, *result, *temp;
	T* samplingBuffer = new T[(unsigned int)(pyrSize[0])];// * 0.51)];
	switch(pyrType){
	case GAUSS:
		if(nPyrLevels < 2){
			cout<<"The number of Gauss levels is too small.";
			return;
		}
		T *temp1, *temp2, *levelData;
		temp1 = new T[(unsigned int)(pyrSize[0])];// * 0.51)];//用于存放建立金字塔时的中间结果。最大需要数据的一半大小;
		temp2 = new T[(unsigned int)(pyrSize[0])];// * 0.13)];
		for(int f = 0; f < nFrames; f ++){//对每帧处理
			DownSampling(data[f], temp1, 0, samplingBuffer);
			origin = temp1;
			result = temp2;
			for(int i = 1; i < nPyrLevels - 1; i ++){
				DownSampling(origin, result, i, samplingBuffer);
				temp = origin;
				origin = result;
				result = temp;
			}

			levelData = new T[totalPyrSize];
			memcpy(levelData, origin, totalPyrSize * sizeof(T));  
			dataPyramid.push_back(levelData);
		}
	
		delete []temp1;
		delete []temp2;
		break;
		
	case LAPLACIAN:
		T *resultCopy;
		temp = new T[pyrSize[0]];
		for(int f = 0; f < nFrames; f ++){//对每帧处理
			resultCopy = new T[totalPyrSize];
			result = resultCopy + pyrSize[0];
			origin = data[f];
			for(int i = 0; i < nPyrLevels - 1; i ++){
				DownSampling(origin, result, i, samplingBuffer);
				origin = result;
				result = result + pyrSize[i + 1];
			}
			
			memcpy(resultCopy, data[f], pyrSize[0]*sizeof(T));
			result = resultCopy;
			for(int i = 0; i < nPyrLevels - 1; i ++){
				levelData = result + pyrSize[i];
				UpSampling(levelData, temp, i + 1);
				DataDifference(result, temp, pyrSize[i]);
				result = levelData;
			}

			dataPyramid.push_back(resultCopy);
		}
		delete []temp;
		break;
	default:
		std::cout<<"wrong pyramid type";
	}

	delete []samplingBuffer;
}

template<typename T>
void Magnification<T>::BuildFourierPyramid()
{
	int i, j, t;
	int nDFTTimes, remains;
	int optimalRows = getOptimalDFTSize( MAX_DFT_SIZE );
	int optimalCols = getOptimalDFTSize( nFrames );

	fourierPyramid = new float[totalPyrSize * nFrames * 2];
	nDFTTimes = totalPyrSize / optimalRows;
	remains = totalPyrSize % optimalRows;

	Mat paddedRe(optimalRows, optimalCols, CV_32FC1);
	Mat paddedIm(optimalRows, optimalCols, CV_32FC1, Scalar::all(0));//傅里叶变换虚部

	float* data;
	unsigned int dataStart;
	for(t = 0; t < nDFTTimes; t ++){
		dataStart = t * optimalRows;
		paddedRe.setTo(0);
		for(i = 0; i < optimalRows; i ++){
			data = paddedRe.ptr<float>(i);
			for(j = 0; j < nFrames; j ++){
				data[j] = static_cast<float>(dataPyramid[j][i + dataStart]);
			}
		}
		Mat planes[] = {paddedRe, paddedIm};
		Mat dftSrc, dftDst;
		merge(planes, 2, dftSrc); 
		dft(dftSrc, dftDst, CV_DXT_ROWS, 0);
		//	split( complexI, planes);
		for(i = 0; i < optimalRows; i ++){
			memcpy(fourierPyramid + (dataStart + i) * 2 * nFrames, dftDst.ptr<float>(i), sizeof(float) * 2 * nFrames);
		}
	}

	if(remains != 0){
		paddedRe.setTo(0);
		dataStart = nDFTTimes * optimalRows;
		for(i = 0; i < remains; i ++){
			data = paddedRe.ptr<float>(i);
			for(j = 0; j < nFrames; j ++){
				data[j] = static_cast<float>(dataPyramid[j][i + dataStart]);
			}
		}
		Mat planes[] = {paddedRe, paddedIm};
		Mat dftSrc, dftDst;
		merge(planes, 2, dftSrc); 
		dft(dftSrc, dftDst, CV_DXT_ROWS, remains);
		for(i = 0; i < remains; i ++){
			memcpy(fourierPyramid + (dataStart + i) * 2 * nFrames, dftDst.ptr<float>(i), sizeof(float) * 2 * nFrames);
		}
	}

}


template<typename T> 
bool Magnification<T>::ReadPyramid(string pyrName, PYRAMID_DATA pyrDataType)
{
	ifstream infile;
	infile.open(pyrName.c_str(),ifstream::binary);
	if(infile){
		infile.read(reinterpret_cast<char*> (&pyrType), sizeof(PYRAMID_TYPE));//金字塔类型
		infile.read(reinterpret_cast<char*> (&nPyrLevels), sizeof(unsigned int));//金字塔层数
		infile.read(reinterpret_cast<char*> (&nFrames), sizeof(unsigned int));//帧数
		//infile.read(reinterpret_cast<char*> (dataLength), sizeof(unsigned int) * nDims * nPyrLevels);//金字塔各层的尺寸
		ReadSpecificInfo(infile);//读入跟各个派生类相关的数据

		GetPyrSize();
		if(pyrDataType == FOURIER){//用理想滤波器需要同时存储fourier变换后的实部和虚部
			float * pData;
			fourierPyramid = new float[totalPyrSize * 2 * nFrames];
			infile.read(reinterpret_cast<char*> (fourierPyramid), sizeof(float) * totalPyrSize * 2 * nFrames);
		}
		else{
			T *pData;
			for(int i = 0; i < nFrames; i ++){
				pData  = new T[totalPyrSize];
				infile.read(reinterpret_cast<char*> (pData), sizeof(T) * totalPyrSize);//金字塔各帧数据
				dataPyramid.push_back(pData);
			}
		}
		infile.close();
		

		return 1;
	}

	else{
		cout<<"Read pyramid file "<<pyrName<< ", but it is not found, so it need to be created"<<std::endl;
		return 0;
	}
}


template<typename T> 
bool Magnification<T>::WritePyramid(string pyrName, PYRAMID_DATA pyrDataType)
{
	ofstream outfile;
	outfile.open(pyrName.c_str(),ofstream::binary);
	if(outfile){
		outfile.write(reinterpret_cast<char*> (&pyrType), sizeof(PYRAMID_TYPE));//金字塔类型
		outfile.write(reinterpret_cast<char*> (&nPyrLevels), sizeof(unsigned int));//金字塔层数
		outfile.write(reinterpret_cast<char*> (&nFrames), sizeof(unsigned int));//帧数
		//outfile.write(reinterpret_cast<char*> (dataLength), sizeof(unsigned int) * nDims * nPyrLevels);//金字塔各层的尺寸
		WriteSpecificInfo(outfile);//写跟各个派生类相关的数据

		GetPyrSize();
		if(pyrDataType == FOURIER){//用理想滤波器需要同时存储fourier变换后的实部和虚部
			outfile.write(reinterpret_cast<char*> (fourierPyramid), sizeof(float) * totalPyrSize * 2 * nFrames);
		}
		else{
			for(int i = 0; i < nFrames; i ++){
				outfile.write(reinterpret_cast<char*> (dataPyramid[i]), sizeof(T) * totalPyrSize);
			}
		}
		outfile.close();
		
		return 1;
	}

	else{
		cout<<"error: cannot open the pyramid file: "<<pyrName;
		return 0;
	}
}

template<typename T> 
void Magnification<T>::MagnifyWithIdealFilter()
{
	int i, j, l, t;

	unsigned int dSize[10];//真正存储起来的每层的数据量，gauss金字塔只存储了最后一层数据
	if(pyrType == GAUSS){
		for(i = 0; i < nPyrLevels - 1; i ++){
			dSize[i] = 0;
		}
		dSize[nPyrLevels - 1] = pyrSize[nPyrLevels - 1];
	} 
	else{
		for(i = 0; i < nPyrLevels; i ++){
			dSize[i] = pyrSize[i];
		}
	}

	if(magnifiedPyramid.size() == 0){
		T *TData;
		for(i = 0; i < nFrames; i ++){
			TData = new T[totalPyrSize];
			magnifiedPyramid.push_back(TData);
		}
	}

	unsigned int nDFTTimes, remains;
	int optimalRows = getOptimalDFTSize( MAX_DFT_SIZE );
	int optimalCols = getOptimalDFTSize( nFrames );
	Mat fData(optimalRows, optimalCols, CV_32FC2, Scalar::all(0));
	Mat invFData(optimalRows, optimalCols, CV_32FC2);

	int levelStart = 0, tStart, fdataStart, dataPos, area;
	float *data;
	float *mask[10], *magMask[10];
	
	for(i = 0; i < nArea; i ++){
		mask[i] = new float[nFrames];
		magMask[i] = new float[nFrames];
		areaList[i].filter.CreateMask(mask[i], nFrames, frameRate);
	}

	for(l = 0; l < nPyrLevels; l ++){
		for(int a = 0; a <nArea; a ++){
			for(i = 0; i < nFrames; i ++){
				magMask[a][i] = mask[a][i] * magnifyInLevels[l] + 1;
			}
		}

		nDFTTimes = dSize[l] / optimalRows;
		remains = dSize[l] % optimalRows;

		fData.setTo(0); 
		for(t = 0; t < nDFTTimes; t ++){
			tStart = levelStart + t * optimalRows;
			for(i = 0; i < optimalRows; i ++){
				dataPos = tStart + i;
				fdataStart = dataPos * nFrames * 2;
				data = fData.ptr<float>(i);
				area = dataArea[dataPos];
			/*	if(area == 255){//没放大
					for(j = 0; j < nFrames; j ++){
						data[j * 2    ] = fourierPyramid[fdataStart + j * 2    ];
						data[j * 2 + 1] = fourierPyramid[fdataStart + j * 2 + 1];
					}
				}
				else{
					for(j = 0; j < nFrames; j ++){
						data[j * 2    ] = fourierPyramid[fdataStart + j * 2    ] * magMask[area][j];
						data[j * 2 + 1] = fourierPyramid[fdataStart + j * 2 + 1] * magMask[area][j];
					}
				}*/

				for(j = 0; j < nFrames; j ++){
					data[j * 2    ] = fourierPyramid[fdataStart + j * 2    ] * magMask[area][j];
					data[j * 2 + 1] = fourierPyramid[fdataStart + j * 2 + 1] * magMask[area][j];
				}
			}
			dft(fData, invFData,  CV_DXT_INVERSE_SCALE|CV_DXT_ROWS, 0);
			for(i = 0; i < optimalRows; i ++){
				data = invFData.ptr<float>(i);
				for(j = 0; j < nFrames; j ++){
					magnifiedPyramid[j][tStart + i] = (T)( data[j * 2] );
				}
			}
		}

		if(remains != 0){
			tStart = levelStart + nDFTTimes * optimalRows;
			for(i = 0; i < remains; i ++){
				data = fData.ptr<float>(i);
				dataPos = tStart + i;
				fdataStart = dataPos * nFrames * 2;
				area = dataArea[dataPos];
			/*	if(area == 255){
					for(j = 0; j < nFrames; j ++){
						data[j * 2]     = fourierPyramid[fdataStart + j * 2    ];
						data[j * 2 + 1] = fourierPyramid[fdataStart + j * 2 + 1];
					}
				}
				else{
					for(j = 0; j < nFrames; j ++){
						data[j * 2]     = fourierPyramid[fdataStart + j * 2    ] * magMask[area][j];
						data[j * 2 + 1] = fourierPyramid[fdataStart + j * 2 + 1] * magMask[area][j];
					}
				}*/
				for(j = 0; j < nFrames; j ++){
					data[j * 2]     = fourierPyramid[fdataStart + j * 2    ] * magMask[area][j];
					data[j * 2 + 1] = fourierPyramid[fdataStart + j * 2 + 1] * magMask[area][j];
				}
			}
			dft(fData, invFData,  CV_DXT_INVERSE_SCALE|CV_DXT_ROWS, remains);

			for(i = 0; i < remains; i ++){
				data = invFData.ptr<float>(i);
				for(j = 0; j < nFrames; j ++){
					magnifiedPyramid[j][tStart + i] = (T)( data[j * 2] );
				}
			}
		}

		levelStart += dSize[l];
	}

	for(i = 0; i < nArea; i ++){
		delete [](mask[i]);
		delete [](magMask[i]);
	}
}

/*
template<typename T> 
void Magnification<T>::MagnifyWithIdealFilter()
{
	int i, j, l, t;

	unsigned int dSize[10];//真正存储起来的每层的数据量，gauss金字塔只存储了最后一层数据
	if(pyrType == GAUSS){
		for(i = 0; i < nPyrLevels - 1; i ++){
			dSize[i] = 0;
		}
		dSize[nPyrLevels - 1] = pyrSize[nPyrLevels - 1];
	}
	else{
		for(i = 0; i < nPyrLevels; i ++){
			dSize[i] = pyrSize[i];
		}
	}

	if(magnifiedPyramid.size() == 0){
		T *TData;
		unsigned int pSize;
		if(pyrType == GAUSS)
			pSize = bottomLevelSize;
		else
			pSize = totalPyrSize;
		for(i = 0; i < nFrames; i ++){
			TData = new T[pSize];
			magnifiedPyramid.push_back(TData);
		}
	}

	unsigned int nDFTTimes, remains;
	int optimalRows = getOptimalDFTSize( MAX_DFT_SIZE );
	int optimalCols = getOptimalDFTSize( nFrames );
	Mat fData(optimalRows, optimalCols, CV_32FC2, Scalar::all(0));
	Mat invFData(optimalRows, optimalCols, CV_32FC2);

	int levelStart = 0, tStart, dataStart;
	float *data;
	float *mask = new float[nFrames];
	float *magMask = new float[nFrames];
	idealFilter.CreateMask(mask, nFrames, frameRate);

	for(l = 0; l < nPyrLevels; l ++){
		for(i = 0; i < nFrames; i ++){
			magMask[i] = mask[i] * magnifyInLevels[l] + 1;
		}

		nDFTTimes = dSize[l] / optimalRows;
		remains = dSize[l] % optimalRows;

		fData.setTo(0);
		for(t = 0; t < nDFTTimes; t ++){
			tStart = levelStart + t * optimalRows;
			for(i = 0; i < optimalRows; i ++){
				dataStart = (tStart + i) * nFrames * 2;
				data = fData.ptr<float>(i);
				for(j = 0; j < nFrames; j ++){
					data[j * 2    ] = fourierPyramid[dataStart + j * 2    ] * magMask[j];
					data[j * 2 + 1] = fourierPyramid[dataStart + j * 2 + 1] * magMask[j];
				}
			}
			dft(fData, invFData,  CV_DXT_INVERSE_SCALE|CV_DXT_ROWS, 0);
			for(i = 0; i < optimalRows; i ++){
				data = invFData.ptr<float>(i);
				for(j = 0; j < nFrames; j ++){
					magnifiedPyramid[j][tStart + i] = (T)( data[j * 2] );
				}
			}
		}

		if(remains != 0){
			tStart = levelStart + nDFTTimes * optimalRows;
			for(i = 0; i < remains; i ++){
				data = fData.ptr<float>(i);
				dataStart = (tStart + i) * nFrames * 2;
				for(j = 0; j < nFrames; j ++){
					data[j * 2] = fourierPyramid[dataStart + j * 2    ] * magMask[j];
					data[j * 2 + 1] = fourierPyramid[dataStart + j * 2 + 1] * magMask[j];
				}
			}
			dft(fData, invFData,  CV_DXT_INVERSE_SCALE|CV_DXT_ROWS, remains);

			for(i = 0; i < remains; i ++){
				data = invFData.ptr<float>(i);
				for(j = 0; j < nFrames; j ++){
					magnifiedPyramid[j][tStart + i] = (T)( data[j * 2] );
				}
			}
		}

		levelStart += dSize[l];
	}
}*/


template<typename T> 
void Magnification<T>::CollapsePyramid(T *pyramid, T* dst, T* buffer = 0)
{
	int i, dataSize;
	bool needFreeMem = 0;
	T *origin, *result, *temp;
	
	if(nPyrLevels == 1){//只有一层金字塔，则直接把金字塔数据拷贝给dst
		memcpy(dst, pyramid, pyrSize[0] * sizeof(T));
		return;
	}

	if(!buffer){
		buffer = new T[pyrSize[0]];
		needFreeMem = 1;
	}
	switch(pyrType) {
	case GAUSS:
		if(nPyrLevels % 2 == 1){//这个语句为了使最后一次Upsampling的结果刚好在dst这个内存中
			UpSampling(pyramid, buffer, nPyrLevels - 1);
			origin = buffer;
			result = dst;
		}
		else{
			UpSampling(pyramid, dst, nPyrLevels - 1);
			origin = dst;
			result = buffer;
		}
		
		for(i = nPyrLevels - 2; i > 0; i --){
			UpSampling(origin, result, i);
			temp = result;
			result = origin;
			origin = temp;
		}

		break;

	case LAPLACIAN:
		T* curPosPtr;
		curPosPtr = pyramid + (totalPyrSize - pyrSize[nPyrLevels - 1]) ;
		if(nPyrLevels % 2 == 1){//这个语句为了使最后一次Upsampling的结果刚好在dst这个内存中
			UpSampling(curPosPtr, buffer, nPyrLevels - 1);
			curPosPtr = curPosPtr - pyrSize[nPyrLevels - 2];
			DataAdd(buffer, curPosPtr, pyrSize[nPyrLevels - 2]);
			origin = buffer;
			result = dst;
		}
		else{
			UpSampling(curPosPtr, dst, nPyrLevels - 1);
			curPosPtr = curPosPtr - pyrSize[nPyrLevels - 2];
			DataAdd(dst, curPosPtr, pyrSize[nPyrLevels - 2]);
			origin = dst;
			result = buffer;
		}

		for(i = nPyrLevels - 2; i > 0; i --){
			UpSampling(origin, result, i);
			curPosPtr = curPosPtr - pyrSize[i - 1];
			DataAdd(result, curPosPtr, pyrSize[i - 1]);
			temp = result;
			result = origin;
			origin = temp;
		}
		break;
	default:
		std::cout<<"We have not realized the function for the pyramid of this type";
	}

	if(needFreeMem){
		delete []buffer;
		buffer = 0;
	}

}

template<typename T> 
void Magnification<T>::CreateMagnifiedData()
{
	if(magnifiedData.size() == 0){
		T *TData;
		for(int i = 0; i < nFrames; i ++){
			TData = new T[pyrSize[0]];
			magnifiedData.push_back(TData);
		}
	}

	if(magnifiedPyramid.size() == 0){//没有放大的金字塔数据。则置为０
		for (int f = 0; f < nFrames; f ++){
		//	memset(magnifiedData[f], 0, pyrSize[0]*sizeof(T) );
			memcpy(magnifiedData[f], data[f], pyrSize[0]*sizeof(T));
		}
		return;
	}

	T *buffer = new T[pyrSize[0]];

	for (int f = 0; f < nFrames; f ++)
	{
		CollapsePyramid(magnifiedPyramid[f], magnifiedData[f], buffer);
	}

	delete []buffer;
}

/*
template<typename T> 
void DataBlending()
{
	if(!blendData)
		blendData = new T[pyrSize[0]];

	for(int i = 0; i < pyrSize[0]; i ++){
		if(blendMask[i])
			blendData[i] = 
	}
}*/




/*/Qt读入数据后要调用以下程序：
//1,两个全局变量VideoMagnification<float> video(30,GAUSS,3,IDEAL)和GraphMagnification<float> graph(30,GAUSS,3,IDEAL);. 并使用一个基类指针Magnification<float> *pMagData;
//2,打开文件: 做死两个文件，一个wen.avi. 一个game，当读入的是wen.avi, pMagData = video. 当读入game， pMagData = graph.
//3,读入数据ReadData()并建立金字塔(graph的ReadData要用陈万烤的代码)
//4,交互操作有：
　//1)选择区域
  //2)改变滤波器
//1，缺省产生一个包含全部像素的无放大区域
//1，nArea, areaList, CreateDataArea();
*/
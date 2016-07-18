#include <iostream>
#include <assert.h>
#include <vector>

using namespace std;
//#include <stdarg.h>
//#include <malloc.h>

/*
//定义了一个二维数组
template<typename T> class Data2D
{
public:
	typedef T		DataType;
	typedef T**		RawData;

	Data2D() 
	{
		m = 0;
		n = 0;
	}
	~Data2D()
	{
		Free();
	}
	operator RawData()	{ return data;	}

	bool Alloc(int _m, int _n)
	{
		m = _m;
		n = _n;
		data = new DataType*[m];
		for (int i = 0; i < m; i++)
		{
			data[i] = new DataType[n];
		}
		return true;
	}
	void Free()
	{
		if (data == NULL)
			return ;
		for (int i = 0; i < m; i++)
		{
			delete[] (data[i]);
		}
		delete[] data;
		data = NULL;
		m = n = 0;
	}

	RawData		data;
	int			m;
	int			n;
};

template<typename T, int X>
class DataXD
{
friend class DataXD<T, X + 1>;
public:
	typedef T									value_type;
	typedef	T*									value_ptr;
	typedef typename DataXD<T, X-1>::data_type	upper_data_type;
	typedef upper_data_type*					data_type;
	static  const	int dim_size = X;
	
	DataXD()
	{
		Init();
	}
	~DataXD()
	{
		Free();
	}
	operator bool()			{ return data;	}
	operator data_type()	{ return data;	}

	void Alloc(unsigned int n0)
	{
		assert(dim_size == 1);
		unsigned int d[dim_size];
		d[0] = n0;
		Alloc(d);
	}

	void Alloc(unsigned int n0, unsigned int n1)
	{
		assert(dim_size == 2);
		unsigned int d[dim_size];
		d[0] = n0;
		d[1] = n1;
		Alloc(d);
	}

	void Alloc(unsigned int n0, unsigned int n1, unsigned int n2)
	{
		assert(dim_size == 3);
		unsigned int d[dim_size];
		d[0] = n0;
		d[1] = n1;
		d[2] = n2;
		Alloc(d);
	}

	void Alloc(unsigned int n0, unsigned int n1, unsigned int n2, unsigned int n3)
	{
		assert(dim_size == 4);
		unsigned int d[dim_size];
		d[0] = n0;
		d[1] = n1;
		d[2] = n2;
		d[3] = n3;
		Alloc(d);
	}

	void Alloc(unsigned int n0, unsigned int n1, unsigned int n2, unsigned int n3, unsigned int n4)
	{
		assert(dim_size == 5);
		unsigned int d[dim_size];
		d[0] = n0;
		d[1] = n1;
		d[2] = n2;
		d[3] = n3;
		d[5] = n4;
		Alloc(d);
	}

	void Alloc(unsigned int d[dim_size])
	{
		int i = 0;
		for (i = 0; i < dim_size; i++)
		{
			if (d[i] == 0)
			{
				assert(false);
			}
			dims[i] = d[i];
		}
		data = AllocData(dims);
	}

	void Free()
	{
		FreeData(data, dims);
		Init();
	}
	
protected:
	void Init()
	{
		for (int i = 0; i < dim_size; i++)
			dims[i] = 0;
		data = NULL;
	}
	
	static data_type AllocData(unsigned int d[dim_size])
	{
		data_type	pdata = new  upper_data_type[d[0]];
		if (dim_size > 1)
		{
			for (unsigned int i = 0; i < d[0]; i++)
			{
				pdata[i] = DataXD<T, X-1>::AllocData(d + 1);
			}
		}
		return pdata;
	}

	static void FreeData(data_type pdata, unsigned int d[dim_size])
	{
		if (dim_size > 1)
		{
			for (unsigned int i = 0; i < d[0]; i++)
			{
				DataXD<T, X-1>::FreeData(pdata[i], d + 1);
			}
		}
		delete[] pdata;
	}
public:
	unsigned int		dims[X];
	data_type			data;
};

template<typename T>
class DataXD<T, 0>
{
	friend class DataXD<T, 1>;
public:
	typedef T									value_type;
	typedef	T*									value_ptr;
	typedef T									data_type;
	static  const	int dim_size = 0;

protected:
	static data_type AllocData(unsigned int d[dim_size])
	{
		assert(false);		//不会调用，dummy
		return NULL;
	}
	static void FreeData(data_type pdata, unsigned int d[dim_size])
	{
		assert(false);		//不会调用，dummy
	}
};

typedef DataXD<int, 2>	DataInt2D;
typedef DataXD<int, 3>	DataInt3D;
typedef DataXD<int, 4>	DataInt4D;
typedef DataXD<int, 5>	DataInt5D;

typedef DataXD<float, 2>	DataFloat2D;
typedef DataXD<float, 3>	DataFloat3D;
typedef DataXD<float, 4>	DataFloat4D;
typedef DataXD<float, 5>	DataFloat5D;

typedef DataXD<Vec3UChar,2> DataUChar2D;*/

/*
void TestDataInt2D(int m, int n);
void TestDataFloat2D(int m, int n);
void TestDataInt3D(int m, int n, int p);
void TestDataFloat3D(int m, int n, int p);

int main()
{
	TestDataInt2D(6, 8);
	TestDataFloat2D(5, 4);
	TestDataInt3D(2, 3, 4);
	TestDataFloat3D(2, 3, 4);
	return 1;
}

void TestDataInt2D(int m, int n)
{
	cout << "TestDataInt2D(" << m << "," << n << ")" << endl;
	DataInt2D theData;
	theData.Alloc(m, n);
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
			theData[i][j] = i * n + j;
	}
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
			cout << theData[i][j] << "    ";
		cout << endl;
	}
}

void TestDataFloat2D(int m, int n)
{
	cout << "TestDataFloat2D(" << m << "," << n << ")" << endl;
	DataFloat2D theData;
	theData.Alloc(m, n);
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
			theData[i][j] = (i * n + j) * 0.1;
	}
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
			cout << theData[i][j] << "    ";
		cout << endl;
	}
}

void TestDataInt3D(int m, int n, int p)
{
	cout << "TestDataInt3D(" << m << "," << n << "," << p << ")" << endl;
	DataInt3D theData;
	theData.Alloc(m, n, p);
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < p; k++)
			{
				theData[i][j][k] = i * n * p + j * p + k;
			}
		}
	}
	for (int i = 0; i < m; i++)
	{
		cout << "-----------" << i << "------------" << endl;
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < p; k++)
			{
				cout << theData[i][j][k] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
}

void TestDataFloat3D(int m, int n, int p)
{
	cout << "TestDataFloat3D(" << m << "," << n << "," << p << ")" << endl;
	DataFloat3D theData;
	theData.Alloc(m, n, p);
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < p; k++)
			{
				theData[i][j][k] = (i * n * p + j * p + k) * 0.1;
			}
		}
	}
	for (int i = 0; i < m; i++)
	{
		cout << "-----------" << i << "------------" << endl;
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < p; k++)
			{
				cout << theData[i][j][k] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
}*/
typedef enum{Laplician, Gauss} PYRAMID_TYPE;

template<typename T> class Magnification
{
protected:
	Magnification(){
		pyrType = Gauss;
		nPyrLevel = 2;
		magnifyFactor[1] = 0;
	};
	Magnification(float* magFactor, PYRAMID_TYPE pType = Gauss, unsigned int nLevel = 1)
	{
		pyrType = pType;
		nPyrLevel = nLevel;

		for(i = 0; i < nLevel; i++){
			magnifyFactor[i] = magFactor[i];
		}

	};
	~Magnification(){};

	virtual void ReadData(std::string fileName)
	{
		std::cout<<"ReadData() of the base class Magnification"<<std::endl;
	};

	virtual void BuildPyramid() = 0;
	std::vector<T*> pyramid;
	std::vector<T*> data;
	PYRAMID_TYPE pyrType;
	unsigned int nPyrLevel;//金字塔层数
	unsigned int nFrames;//帧数
	float magnifyFactor[10];//金字塔每层的放大倍数
};

template<typename T, int nDim> class GridMagnification:
public Magnification<T>{  
	GridMagnification(unsigned int filterLeng, float* filter, float* magFactor, PYRAMID_TYPE pType = Gauss, unsigned int nLevel = 2)
		:Magnification(magFactor,pType,nLevel)
	{
		nDimension = nDim;
		filterLength = filterLeng;
		for(int i = 0; i < filterLength; i ++){
			oneDFilter[i] = filter[i];
		}
	}
	virtual void ReadData(std::string fileName)
	{
		std::cout<<"ReadData() of the class GridMagnification"<<std::endl;
	};
	virtual void BuildPyramid();
	void GetLengthInEachLevel();//获得每层数据的各维长度
	void BuildOneFramePyramid(int frame);
	void Convolution();


	unsigned int nLength[10][nDim]; //各层中各维的长度
	unsigned int nDimension; //维数
	unsigned int filterLength;
	unsigned int oneDFilter[100];
};  


template<typename T>
void Magnification<T>::BulidPyramid()
{
	
}

template<typename T, int nDims> 
void GridMagnification<T, nDims>:: BuildPyramid()
{
	int i;

	GetLengthInEachLevel();
	for(i = 0; i < nFrame; i ++){
		BuildOneFramePyramid(i);
	}

/*	T* oneFrame = new T[oneFrameMem];
	for(i = 0; i < nFrames; i ++){
		GetOneFrameData(T);
	}

	delete []T;*/
}

template<typename T, int nDim> 
void GridMagnification<T, nDim>:: BuildOneFramePyramid(int frame)
{
	T *levelData, temp1;
	unsigned int dataSize = 1;

	if(pyrType == Gauss){
		for(i = 0; i < nDimension; i ++){
			dataSize *= nLength[nLevel - 1][i];
		}
		temp1 = data[frame];
		levelData = new T[dataSize];
		for(i = 0; i < nLevel; i ++){
			Convolution(temp1,temp2);
			downSample(temp2, temp1);
		}

		for(i = 0; i < dataSize; i ++){
			levelData[i] = temp1[i];
		}
		pyramid.push_back(levelData);
	}

	else{
	}


	unsigned int dataSize = 1, ratio = 1;
	for(i = 0; i < nDimension; i ++){
		dataSize *= nLength[i];
		ratio *= 2;
	}

	T* curLevelBegin = pyramid[frame];
	T* nextLevelBegin = curLevelBegin + dataSize;
	for(int i = 1; i < nLevel; i ++)
	{

		dataSize /= ratio;
		curLevelBegin = nextLevelBegin;
		nextLevelBegin += dataSize;
	}
}

template<typename T, int nDim> 
void GridMagnification<T, nDim>:: GetLengthInEachLevel()
{
	for(i = 1; i < nPyrLevel; i ++){
		for(j = 0; j < nDimension; j ++){
			if(nLength[i-1][j] % 2 == 0)
				nLength[i][j] = nLength[i-1][j] / 2;
			else
				nLength[i][j] = nLength[i-1][j] / 2 + 1;
		}
	}
}

template<typename T, int nDim> 
void GridMagnification<T, nDim>::Convolution(T* data, T* result, unsigned int *size)
{
	int step[5], start, end, filtExtent, resultP, dataP, rowP, p; 
	float filterValue;

	start = filterLength / 2;
	if(start % 2 == 1) start ++;
	filtExtent = -filterLength / 2;

//	step[0] = 1;
//	for(i = 1; i < nDimension; i ++){
//		step[i] = size[i - 1] * step[i - 1];
//	}

	switch(nDimension){
	case 2:
		end = size[1] + 1 - filterLength / 2;
		if(end % 2 == 1) 
			end --;

		resultP = 0;
		for(i = 0; i < size[0]; i ++){//对每行进行一维滤波
			rowP = i * size[1];
			for(j = 0; j < start; j += 2){//每行开始的边缘区域
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < filterLength; k ++, p ++){//此段做一维滤波
					if(p < 0)
						dataP = rowP - p;
					else
						dataP = rowP + p;
					filterValue += oneDFilter[k] * data[dataP]
				}
				result[resultP ++] = filterValue;
			}

			for(j = start; j < end; j += 2){//中间
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < filterLength; k ++, p ++){//此段做一维滤波
					dataP = rowP + p;
					filterValue += oneDFilter[k] * data[dataP]
				}
				result[resultP ++] = filterValue;
			}

			for(j = end; j < size[1]; j += 2){//结尾边缘
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < filterLength; k ++, p ++){//此段做一维滤波
					if(p >= size[1])
						dataP = rowP + 2* size[1] - 2 - p;
					else
						dataP = rowP + p;
					filterValue += oneDFilter[k] * data[dataP]
				}
				result[resultP ++] = filterValue;
			}
		}

		//对每列做一维滤波
		if(size[1] % 2 == 1)//前面做行降采样了，所以列数减半
			size[1] = size[1] / 2 + 1;
		else 
			size[1] = size[1] / 2;

		end = size[0] + 1 - filterLength / 2;
		if(end % 2 == 1) 
			end --;

		for(i = 0; i < size[1]; i ++)
		{
			for(j = 0; j < start; j += 2){//每列开始的边缘区域
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < filterLength; k ++, p ++){//此段做一维滤波
					if(p < 0)
						dataP = i - p * size[1];
					else
						dataP = i + p * size[1];
					filterValue += oneDFilter[k] * result[dataP]
				}
				result[j * size[1] + i] = filterValue;
			}

			for(j = start; j < end; j += 2){//每列开始的边缘区域
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < filterLength; k ++, p ++){//此段做一维滤波
					dataP = i + p * size[1];
					filterValue += oneDFilter[k] * result[dataP]
				}
				result[j * size[1] + i] = filterValue;
			}

			for(j = end; j < size[0]; j += 2){//结尾边缘
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < filterLength; k ++, p ++){//此段做一维滤波
					if(p >= size[0])
						dataP = i + (2* size[0] - 2 - p) * size[1];
					else
						dataP = i + p * size[1];
					filterValue += oneDFilter[k] * result[dataP]
				}
				result[j * size[1] + i] = filterValue;
			}
		}

		//对列downsampling
		int row1, row2;
		for(i = 2; i < size[0]; i += 2){
			row1  = i * size[1];
			row2  = (i / 2) * size[1];
			for(j = 0; j < size[1]; j ++){
				result[row2 + j] = result[row1 + j];
			}
		}

	case 3:
	default:
		std::cout<<"the function Convolution in this dimension has not been realized yet";
		exit(-1);
	}
}
#pragma once
#include "Magnify.h"
#include <map>
#define FILTERCENTERRATIO 0.25

class Edge{
public:
	Edge(unsigned int v1, unsigned int v2,  float w, float fw = 0)
	{
		startVertex = v1; endVertex = v2; weight = w; filterWeight = fw;
	};
	Edge(){startVertex = 0; endVertex = 0; weight = 0; filterWeight = 0;};
	inline void setValue(unsigned int v1, unsigned int v2,  float w)
	{
		startVertex = v1; endVertex = v2; weight = w;
	};
	unsigned int startVertex, endVertex;
	float weight;
	float filterWeight;
};

template<typename T> class GraphMagnification:
public Magnification<T>{  
public:
	GraphMagnification(float samplingRate, PYRAMID_TYPE pType = Gauss, unsigned int nLevels = 1, TEMPORAL_FILTER_TYPE tfType = IDEAL)
		:Magnification(samplingRate, pType,nLevels,tfType){edges = NULL; degree = NULL;};
	GraphMagnification(){edges = NULL;  degree = NULL;};
	~GraphMagnification()
	{
		if(edges){
			for(int i = 0; i < nVertexs; i ++){
				if(edges[i]) {
					delete [](edges[i]);
				}
			}
			delete []edges;
		}
	};

	virtual void ReadData(std::string fileName);
	void BuildNeighbourhood();
	virtual void UpSampling(T* src, T* dst, unsigned int level);
	virtual void DownSampling(T* data, T* result, unsigned int level = 0, T* tempBuffer = NULL);
	virtual void GetPyrSize();
	virtual void CreateDataArea();
	unsigned int nEdges;
//	std::multimap<unsigned int, EdgeEndVertex> edges;
	Edge **edges;
	unsigned int *degree;//每个顶点的度数
};

template<typename T>
void GraphMagnification<T>:: ReadData(std::string fileName)
{
	Magnification::ReadData(fileName);

	/*int rows, cols, rowStart, curPos, i, j, r;	
	//视频例子
	cv::VideoCapture capture;
	capture.open(fileName);
	if(!capture.isOpened()){
		std::cout<<"Cannot open the video file "<<fileName<<std::endl;
		exit(-1);
	}

	frameRate = capture.get(CV_CAP_PROP_FPS);
	rows = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	cols = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	nFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
	Mat frame;
	vector<Mat> rgb(3);
	cv::namedWindow("Display");
	int delay = 1000 / frameRate;

	nVertexs = rows * cols;
	while (true)
	{
		if(!capture.read(frame))
			break;
		cv::imshow("display", frame);
		cv::waitKey(delay);
		frame.convertTo(frame, CV_32FC3, 1);
		split(frame, rgb);

		T *frameData = new T[nVertexs];
		T* dPtr;
		for(int i = 0; i < rows; i ++){
			dPtr = rgb[0].ptr<T>(i);
			memcpy(frameData + (i * cols), dPtr, cols * sizeof(T));
		}
		data.push_back(frameData);
	}


	//建立图结构
	int vindex = 0;
	nVertexs = rows * cols;
	edges = new Edge*[nVertexs];
	degree = new unsigned int[nVertexs];
	for(i = 0; i < nVertexs; i ++){
		edges[i] = NULL;
		degree[i] = 0;
	}
	
	//第一行
	degree[0] = 3;
	edges[0] = new Edge[3];
	edges[0][0].setValue(0, 1, 2);
	edges[0][1].setValue(0, cols, 2); 	edges[0][2].setValue(0, cols + 1, 1);
	for(i = 1; i < cols - 1; i ++){
		degree[i] = 5;
		edges[i] = new Edge[5];
		edges[i][0].setValue(i, i - 1, 2); edges[i][1].setValue(i, i + 1, 2); 
		edges[i][2].setValue(i, cols + i - 1, 1); edges[i][3].setValue(i, cols + i, 2); edges[i][4].setValue(i, cols + i + 1, 1); 
	}
	degree[cols - 1] = 3;
	edges[cols - 1] = new Edge[3];
	edges[cols - 1][0].setValue(cols - 1, cols - 2, 2);
	edges[cols - 1][1].setValue(cols - 1, cols * 2 - 2, 1);  edges[cols - 1][2].setValue(cols - 1, cols * 2 - 1, 2);

	//中间各行
	for(r = 1; r < rows - 1; r ++){
		rowStart = r * cols;
		degree[rowStart] = 5;
		edges[rowStart] = new Edge[5];
		edges[rowStart][0].setValue(rowStart, rowStart - cols, 2); edges[rowStart][1].setValue(rowStart, rowStart - cols + 1, 1);
		edges[rowStart][2].setValue(rowStart, rowStart + 1, 2);
		edges[rowStart][3].setValue(rowStart, rowStart + cols, 2); edges[rowStart][4].setValue(rowStart, rowStart + cols + 1, 1);
		
		for(i = 1; i < cols - 1; i ++){
			curPos = rowStart + i;
			degree[curPos] = 8;
			edges[curPos] = new Edge[8];
			edges[curPos][0].setValue(curPos, curPos - cols - 1, 1); edges[curPos][1].setValue(curPos, curPos - cols, 2); edges[curPos][2].setValue(curPos, curPos - cols + 1, 1);
			edges[curPos][3].setValue(curPos, curPos - 1, 2); edges[curPos][4].setValue(curPos, curPos + 1, 2);
			edges[curPos][5].setValue(curPos, curPos + cols - 1, 1); edges[curPos][6].setValue(curPos, curPos + cols, 2); edges[curPos][7].setValue(curPos, curPos + cols + 1, 1);
		}
		curPos = rowStart + cols - 1;
		degree[curPos] = 5;
		edges[curPos] = new Edge[5];
		edges[curPos][0].setValue(curPos, curPos - cols - 1, 1); edges[curPos][1].setValue(curPos, curPos - cols, 2);
		edges[curPos][2].setValue(curPos, curPos - 1, 2);
		edges[curPos][3].setValue(curPos, curPos + cols - 1, 1); edges[curPos][4].setValue(curPos, curPos + cols, 2); 
	}

	//最后一行
	curPos = (rows - 1) * cols;
	degree[curPos] = 3;
	edges[curPos] = new Edge[3];
	edges[curPos][0].setValue(curPos, curPos - cols, 2); edges[curPos][1].setValue(curPos, curPos - cols + 1, 1); 
	edges[curPos][2].setValue(curPos, curPos + 1, 2); 
	for(i = 1; i < cols - 1; i ++){
		curPos = (rows - 1) * cols + i;
		degree[curPos] = 5;
		edges[curPos] = new Edge[5];
		edges[curPos][0].setValue(curPos, curPos - cols - 1, 1); edges[curPos][1].setValue(curPos, curPos - cols, 2); edges[curPos][2].setValue(curPos, curPos - cols + 1, 1); 
		edges[curPos][3].setValue(curPos, curPos - 1, 2); edges[curPos][4].setValue(curPos, curPos + 1, 2); 
	}
	curPos = rows  * cols - 1;
	edges[curPos] = new Edge[3];
	degree[curPos] = 3;
	edges[curPos][0].setValue(curPos, curPos - cols - 1, 1); edges[curPos][1].setValue(curPos, curPos - cols, 2);
	edges[curPos][2].setValue(curPos, curPos - 1, 2);

	BuildNeighbourhood();
	BuildPyramid();
	CreateMagnifiedData();


	int nLobes = 3;
	float temporalFL[4] = {2.8, 4.5, 6.5};
	float temporalFH[4] = {3.2, 5.5, 7.5};
	float magnifyRatio[4] = {10, 10, 10};

	Filter filter;
	filter.SetLobes(nLobes, temporalFL, temporalFH, magnifyRatio);
	Area area[3];
	area[0].SetRectangle(0,cols,0,rows,filter);
	UpdateAreas(1, area);
	

	VideoWriter writer("s2.avi", CV_FOURCC('M','J','P','G'), 30.0, Size(cols,rows),1);
	Mat video_frame(rows, cols, CV_32FC3);//3通道视频帧，每一通道的值都相同
	
	for (int f = 0; f < nFrames; f ++)
	{
		vector<Mat> r_g_b(3);//r、g、b通道分别存储相同的灰度值
		Mat framedata(rows, cols, CV_32FC1);
		T *framePtr;
		int rowStart;
		for(int i = 0; i < rows; i ++){
			framePtr = framedata.ptr<T>(i);
			rowStart = i * cols;
			for(int j = 0; j < cols; j ++){
				framePtr[j] = magnifiedData[f][rowStart + j];
			}
		}
		
		r_g_b[0] = framedata; //r通道
		r_g_b[1] = framedata; //g通道
		r_g_b[2] = framedata;//b通道
		merge(r_g_b, video_frame);//将3个单通道合并为一个3通道的Mat

		Mat res;
		video_frame.convertTo(res, CV_8UC3, 1);//将3通道的Mat的数据类型转化为无符号字符型
		writer<<res;//写入视频中
	}*/

	int i;
	int begin = 0, end = 0, temp = 0, judge;
	int data_edge;
	int *allData;
	
	string line;
	ifstream inf;
	inf.open("E:/QtCode/Maginification2/230Data.txt", ifstream::in);
	getline(inf, line);
	nVertexs = atoi(line.c_str());//第一行所有顶点个数读取
	allData = new int[nVertexs * 2];
	degree = new unsigned int[nVertexs];
	edges = new Edge*[nVertexs];
	while (!inf.eof())
	{
		judge = 0;
		getline(inf, line);
		begin = line.find(',', 0);
		degree[temp] = atoi(line.substr(0, begin).c_str());
		while (begin<line.size())
		{
			end = line.find(',', begin + 1);
			data_edge = atoi(line.substr(begin + 1, end - begin - 1).c_str());
			allData[judge++] = data_edge;
			begin = end;
		}
		edges[temp] = new Edge[degree[temp]];
		for (i = 0; i<degree[temp]; i++)
		{
			edges[temp][i].startVertex = temp;
			edges[temp][i].endVertex = allData[i * 2];
			edges[temp][i].weight = allData[i * 2 + 1];
		}
		
		temp++;
	}
	inf.close();
	delete []allData;

	//帧数据读取
	//int nFrames;
	int *dataI = new int[nVertexs];
	int count;
	float *dataF = new float[nVertexs];
	T *frameData;

	ifstream inf1;
	judge = 0;
    begin = 0, end = 0, temp = 0;
	inf1.open("E:/QtCode/Maginification2/230czData.txt", ifstream::in);
	getline(inf1, line);
	nFrames = atoi(line.c_str());
	int ll=0;
	while (!inf1.eof())			//第二行开始读取数据
	{
		if(ll == 435)
			int lllll=0;
		getline(inf1, line);
		begin = line.find(',', 0);
		count = atoi(line.substr(0, begin).c_str());
		
		frameData = new T[nVertexs];
		memset(frameData, 0, nVertexs * sizeof(T));
		if (count != 0)
		{
			while (begin<line.size())
			{
				end = line.find(',', begin + 1);
				if (!judge)
				{
					dataI[temp] = atoi(line.substr(begin + 1, end - begin - 1).c_str());
					judge = 1;
				}
				else
				{
					dataF[temp++] = atof(line.substr(begin + 1, end - begin - 1).c_str());
					judge = 0;
				}
				begin = end;
			}

			for (i = 0; i<count; i++)
			{
				frameData[dataI[i]] = dataF[i];
			}
			
			judge = 0;
			temp = 0;
		}

		data.push_back(frameData);
		ll++;
	}

	inf1.close();
	delete []dataI;
	delete []dataF;

	int tt = 0;
	for(int f = 0; f < nFrames; f ++){
		for(i = 0; i < nVertexs; i ++){
			data[f][i] /= 100;
			if(data[f][i] > 1)
				data[f][i] = 1;
		}
	}

	BuildNeighbourhood();
	BuildPyramid();
	CreateMagnifiedData();
}

template<typename T>
void GraphMagnification<T>:: BuildNeighbourhood()
{
	int v, i;
	int nEdges;
	float sum;

	for(v = 0; v < nVertexs; v ++){
		nEdges = degree[v];
		sum = 0;
		for (i = 0; i < nEdges; i++){
			sum += edges[v][i].weight;
		}

		for (i = 0; i < nEdges; i++){
			edges[v][i].filterWeight = edges[v][i].weight / sum * (1 - FILTERCENTERRATIO);
		}
	}
}

template<typename T>
void GraphMagnification<T>::UpSampling(T* src, T* dst, unsigned int level)
{
	memcpy(dst, src, nVertexs * sizeof(T));
}


template<typename T>
void GraphMagnification<T>::DownSampling(T* data, T* result, unsigned int level = 0, T* tempBuffer = NULL)
{
	int i, v, nEdges;
	float value;
	for(v = 0; v < nVertexs; v ++){
		value = 0;
		nEdges = degree[v];
		for(i = 0; i < nEdges; i ++){
			value += data[edges[v][i].endVertex] * edges[v][i].filterWeight;
		}
		value += data[v] * FILTERCENTERRATIO;
		result[v] = value;
	}
}

template<typename T>
void GraphMagnification<T>::GetPyrSize()
{
	for(int i = 0; i < nPyrLevels; i ++){
		pyrSize[i]  = nVertexs;
	}

	if(pyrType == GAUSS){
		totalPyrSize = nVertexs;
	}
	else{
		totalPyrSize = nVertexs * nPyrLevels;
	}
}

template<typename T>
void GraphMagnification<T>::CreateDataArea()
{
	if(!dataArea){
		dataArea = new unsigned char[totalPyrSize];
	}

/*	if(nArea == 0)
		memset(dataArea, 255, totalPyrSize * sizeof(unsigned char));//255表示没放大的原区域
	else
		memset(dataArea, 0, totalPyrSize * sizeof(unsigned char));//对图数据现在暂时设置一个区域*/

	memset(dataArea, 0, totalPyrSize * sizeof(unsigned char));//全局区域为0区域
}

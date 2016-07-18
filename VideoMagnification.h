#pragma once
#include "GridMagnification.h"

template<typename T> class VideoMagnification:
public GridMagnification<T, 2>{
public:
	VideoMagnification(float samplingRate, unsigned int filterLeng, float* filter, PYRAMID_TYPE pType = Gauss, unsigned int nLevels = 1, TEMPORAL_FILTER_TYPE tfType = IDEAL)
		:GridMagnification(samplingRate, filterLeng, filter, pType, nLevels, tfType)
	{ };

	virtual void ReadData(std::string fileName);
	void Write2Video(std::string fileName, DATA_SOURCE source);
	void ReadAdjacentMatrix();
};

template<typename T>
void VideoMagnification<T>::ReadData(std::string fileName)
{
	Magnification::ReadData(fileName);
	/*dataName = fileName;
	cv::Mat image = cv::imread(fileName);
	cv::Mat grayImage;
	cv::namedWindow("ShowImage");
	cv::imshow("ShowImage",image);
	
	cvtColor(image, grayImage, CV_BGR2GRAY);
	
	grayImage.convertTo(grayImage, CV_32FC1, 1, 0);
	for(int i = 0; i < grayImage.rows; i ++){
		for(int j = 0; j < grayImage.cols; j ++){
			grayImage.at<float>(i,j) = grayImage.at<float>(i,j) / 255.0;
		}
	}
	cv::imshow("ShowImage",grayImage);
	cv::Mat_<float>::iterator it = grayImage.begin<float>();  
    cv::Mat_<float>::iterator itend = grayImage.end<float>();  
	
	dataLength[0][0] = grayImage.rows;
	dataLength[0][1] = grayImage.cols;
	T* frameData = new T[grayImage.rows * grayImage.cols];
	for (int i = 0; it!= itend; ++it, i ++) {  
		frameData[i] = (*it);
    }  
	
	T* tdata;
	nFrames = 11;
	int step = 0;
	int nPixels = grayImage.rows * grayImage.cols;
	for(int f = 0; f < nFrames; f ++){
		tdata = new T[nPixels];
		for (int i = 0; i < nPixels; i ++) {  
			tdata[i] = frameData[(i + step * grayImage.cols) % nPixels];
		}  
		data.push_back(tdata);
		step += 10;
	}
	BuildPyramid();*/

	/*int level = 2;
	result.create(dataLength[level][0],dataLength[level][1],grayImage.type());
	int nRow = result.rows;
	int nCol = result.cols;

	int start = 0;
	for(int i = 0; i < level; i++){
		start += pyrSize[i];
	}

	for(int i = 0; i < nRow; i ++){
		for(int j = 0; j < nCol; j ++){
			result.at<T>(i,j) = dataPyramid[5][start + i * nCol + j];
		}
	}*/

/*	result.create(grayImage.rows,grayImage.cols,grayImage.type());
	T * collapse = new T[grayImage.rows * grayImage.cols];
	CollapsePyramid(7, collapse);
	for(int i = 0; i < grayImage.rows; i ++){
		for(int j = 0; j < grayImage.cols; j ++){
			result.at<T>(i,j) = collapse[i * grayImage.cols + j];// dataPyramid[5][start + i * nCol + j];
		}
	}

	cv::namedWindow("ShowResult");
	cv::imshow("ShowResult",result);
	
	cv::waitKey(0);*/

	cv::VideoCapture capture;
	capture.open(fileName);
	if(!capture.isOpened()){
		std::cout<<"Cannot open the video file "<<fileName<<std::endl;
		exit(-1);
	}

	frameRate = capture.get(CV_CAP_PROP_FPS);
	dataLength[0][0] = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	dataLength[0][1] = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	nFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
	Mat frame;
	vector<Mat> rgb(3);
//	cv::namedWindow("Display");
	int delay = 1000 / frameRate;

	unsigned int nPixels = dataLength[0][0] * dataLength[0][1];
	while (true)
	{
		if(!capture.read(frame))
			break;
	//	cv::imshow("display", frame);
	//	cv::waitKey(delay);
		frame.convertTo(frame, CV_32FC3, 1);
		split(frame, rgb);

		T *frameData = new T[nPixels];
		T* dPtr;
		for(int i = 0; i < dataLength[0][0]; i ++){
			dPtr = rgb[0].ptr<T>(i);
			memcpy(frameData + (i * dataLength[0][1]), dPtr, dataLength[0][1] * sizeof(T));
		}
		data.push_back(frameData);

	}

	BuildPyramid();
	CreateMagnifiedData();
}

template<typename T>
void VideoMagnification<T>:: Write2Video(std::string fileName, DATA_SOURCE source)
{
	int frame_row = dataLength[0][0];
	int frame_col = dataLength[0][1];
	VideoWriter writer(fileName, CV_FOURCC('M','J','P','G'), 10.0, Size(frame_col,frame_row),1);
	Mat video_frame(frame_row, frame_col, CV_32FC3);//3通道视频帧，每一通道的值都相同
	
	int nPixels = frame_row * frame_col;
	T* collapse = new T[frame_row * frame_col];
	for (int f = 0; f < nFrames; f ++)
	{
		if(source == ORIGINALDATA)
			collapse = data[f];
		else if( source == DATAPYRAMID)
			CollapsePyramid(dataPyramid[f], collapse);
		else//MAGNIFIEDPYRAMID
			//CollapsePyramid(magnifiedPyramid[f], collapse);
			collapse = magnifiedData[f];
		vector<Mat> r_g_b(3);//r、g、b通道分别存储相同的灰度值
		Mat framedata(frame_row, frame_col, CV_32FC1);
		T *framePtr;
		int rowStart;
		for(int i = 0; i < frame_row; i ++){
			framePtr = framedata.ptr<T>(i);
			rowStart = i * frame_col;
			for(int j = 0; j < frame_col; j ++){
				framePtr[j] = collapse[rowStart + j];
			}
		}
		
		r_g_b[0] = framedata; //r通道
		r_g_b[1] = framedata; //g通道
		r_g_b[2] = framedata;//b通道

	/*	int xl = 30; 
		int xh = 100;
		int yl = 30;
		int yh = 100;
		T *gg, *bb;
		gg = r_g_b[1].ptr<T>(yl);
		bb = r_g_b[2].ptr<T>(yl);
		for(int i = xl; i < xh; i ++){
			gg[i] = bb[i] = 0;
		}

		gg = r_g_b[1].ptr<T>(yh);
		bb = r_g_b[2].ptr<T>(yh);
		for(int i = xl; i < xh; i ++){
			gg[i] = bb[i] = 0;
		}

		for(int i = yl; i < yh; i ++){
			gg = r_g_b[1].ptr<T>(i);
			bb = r_g_b[2].ptr<T>(i);
			gg[xl] = bb[xl] = 0;
		}

		for(int i = yl; i < yh; i ++){
			gg = r_g_b[1].ptr<T>(i);
			bb = r_g_b[2].ptr<T>(i);
			gg[xh] = bb[xh] = 0;
		}
		*/

		merge(r_g_b, video_frame);//将3个单通道合并为一个3通道的Mat

		Mat res;
		video_frame.convertTo(res, CV_8UC3, 1);//将3通道的Mat的数据类型转化为无符号字符型
		writer<<res;//写入视频中
	}

}

template<typename T>
void VideoMagnification<T>:: ReadAdjacentMatrix()
{
	std::string label[3000];
	ifstream inf;

	//先读写label
	inf.open("Alabel.txt",ifstream::in);
	string l;
	int j=0;
	string line;
	int begin=0,end=0;
	while (!inf.eof())
	{
		getline(inf,line);
		begin=line.find(',',0);
		label[j]=line.substr(0,begin).c_str();
		while (begin<line.size() && j!=1000)
		{
			end=line.find(',',begin+1);
			l=line.substr(begin+1,end-begin-1).c_str();
			j++;
			label[j]=l;
			begin=end;
		}
	}
	inf.close();

	//再读图的边的权重
	inf.open("Alldata.txt",ifstream::in);//文件读取
	int alldata[20000];
	int i=0,t=0,k=0;;
	int edgeSum=0,num=0;
	j = 0;
	begin = 0;
	end = 0;
	while (!inf.eof())
	{	
		getline(inf,line);
		begin=line.find(',',0);
		edgeSum=atoi(line.substr(0,begin).c_str());//取出第一个边数sum,数据总个数为sum*6	
		while (begin<line.size() && j!=edgeSum*6)//取出其他sum*6个数据
		{
			end=line.find(',',begin+1);
			num=atoi(line.substr(begin+1,end-begin-1).c_str());
			alldata[j]=num;
			j++;
			begin=end;
		}
		float *arr = new float[1000*1000];
		for(int i0=0;i0<1000*1000;i0++)
		{
			arr[i0]=0;
		}
		for(t=0,i=0;i<edgeSum*2;i++,t=t+3) 
		{	
			arr[1000*alldata[t]+alldata[t+1]]=alldata[t+2]*1.0;
			arr[1000*alldata[t+1]+alldata[t]]=alldata[t+2]*1.0;	
		}
		//cout<<edgeSum<<" ";
		data.push_back(arr);
		j=0;		
	}
	inf.close();

	int nPxs = 1000*1000;
	nFrames = data.size();
	for(i = 0; i < nFrames; i ++){
		for(j = 0; j < nPxs; j ++){
			data[i][j] = data[i][j] / 1;
		}
	}

	Magnification::ReadData("adjacent");
	frameRate = 30;
	dataLength[0][0] = 1000;
	dataLength[0][1] = 1000;
	BuildPyramid();

/*	int max2000 = 0, max1000 = 0, max900 = 0, max800 = 0, max700 = 0, max600 = 0, max500 = 0, max400 = 0, max300 = 0, max200 = 0, max100 = 0;
	
	for(i = 0; i < data.size(); i ++){
		for(j = 0; j < nPxs; j ++){
			if(data[i][j] < 10)
				max100 ++;
			if((data[i][j] >= 10)&&(data[i][j] < 20))
				max200 ++;
			if((data[i][j] >= 20)&&(data[i][j] < 30))
				max300 ++;
			if((data[i][j] >= 30)&&(data[i][j] < 40))
				max400 ++;
			if((data[i][j] >= 40)&&(data[i][j] < 50))
				max500 ++;
			if((data[i][j] >= 50)&&(data[i][j] < 60))
				max600 ++;
			if((data[i][j] >= 60)&&(data[i][j] < 70))
				max700 ++;
			if((data[i][j] >= 70)&&(data[i][j] < 80))
				max800 ++;
			if((data[i][j] >= 80)&&(data[i][j] < 90))
				max900 ++;
			if((data[i][j] >= 90)&&(data[i][j] < 100))
				max1000 ++;
			if(data[i][j] >= 100)
				max2000 ++;
		}
	}*/
}
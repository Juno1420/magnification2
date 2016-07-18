#include "Magnify.h"



Magnify::~Magnify()
{

}

/*

//此程序应该可以不用
void Magnify::Read_Video(char *video_name)//读取视频，并将视频中的每帧保存到originalData中
{
	VideoCapture src_video;
	src_video.open(video_name);

	while (true)
	{
		Mat frame;
		src_video>>frame;
		if(frame.empty())
			break;
		frame.convertTo(frame, CV_32FC3, 1);
		vector<Mat> r_g_b(3);
		split(frame, r_g_b);
		originalData.push_back(r_g_b[0]);
	}
	magnifyData = originalData;
}

}*/

void Magnify::write2video(vector<Mat> &src, char *out_name)
{
	int frame_row = src[0].rows;
	int frame_col = src[0].cols;
	VideoWriter writer(out_name, CV_FOURCC('M','J','P','G'), 30.0, Size(frame_col,frame_row),1);
	
	Mat video_frame(frame_row, frame_col, CV_32FC3);//3通道视频帧，每一通道的值都相同
	
	for (int i=0; i<src.size(); i++)
	{
		vector<Mat> r_g_b(3);//r、g、b通道分别存储相同的灰度值
		r_g_b[0] = src[i]; //r通道
		r_g_b[1] = src[i]; //g通道
		r_g_b[2] = src[i];//b通道
		merge(r_g_b, video_frame);//将3个单通道合并为一个3通道的Mat

		Mat res;
		video_frame.convertTo(res, CV_8UC3, 1);//将3通道的Mat的数据类型转化为无符号字符型
		writer<<res;//写入视频中
	}

}


void Magnify::createSimulateData()
{
	class Disk {
	public:
		float init_x;
		float init_y;
		float freq;
		float radius;
		float move_dis;
		float movedir_x;
		float movedir_y;
	};

	int frame_row = 400;
	int frame_col = 400;
	int nDisk = 2;
	int nFrames  = 300;
	
	Disk disk[5];
	disk[0].radius = 25;
	disk[0].init_x = 50;
	disk[0].init_y = 50;
	disk[0].freq = 2;
	disk[0].move_dis = 5;
	disk[0].movedir_x = 1;
	disk[0].movedir_y = 0;

	disk[1].radius = 25;
	disk[1].init_x = 50;
	disk[1].init_y = 150;
	disk[1].freq = 4;
	disk[1].move_dis = 5;
	disk[1].movedir_x = 1;
	disk[1].movedir_y = 0;

	vector<Mat> src;
	float background = 120;
	float foreground = 255;
	float boundary = 120;
	for(int f = 0; f < nFrames; f ++){
		Mat oneFrame(frame_row, frame_col, CV_32FC1);
		for(int i = 0; i < frame_row; i ++){
			for(int j = 0; j < frame_col; j ++){
				oneFrame.at<float>(i, j) = background;
			}
		}

		float center_x,center_y,cur_t,cycle,distance,begin_x,begin_y,end_x,end_y;
		cur_t = f / 30.0;
		for(int d  = 0; d < nDisk; d ++){
			cycle = cur_t * disk[d].freq;
			cycle = cycle - (int)cycle;
			if(cycle > 0.5) cycle = 1.0 - cycle;
			distance = cycle * disk[d].move_dis;
			center_x = disk[d].init_x + distance * disk[d].movedir_x;
			center_y = disk[d].init_y + distance * disk[d].movedir_y;

			begin_x = center_x - disk[d].radius - 1;
			end_x = center_x + disk[d].radius + 1;
			begin_y = center_y - disk[d].radius - 1;
			end_y = center_y + disk[d].radius + 1;
			for(int i = begin_y; i < end_y; i ++){
				for(int j = begin_x; j < end_x; j ++){
					distance  = sqrt((i - center_y)*(i - center_y)+(j - center_x)*(j - center_x));
					if(distance > disk[d].radius) continue;

					distance = 1.0 - distance/disk[d].radius;
					oneFrame.at<float>(i, j) = boundary + (foreground - boundary) * distance;
				}
			}
		}

		src.push_back(oneFrame);
	}

	write2video(src, "disk.avi");
}

Magnify::Magnify(double Lambda_c, double SRate, double ChAtten, int Level, int glvs)
{
	lambda_c = Lambda_c;
	samplingRate = SRate;
	chromAttenuation = ChAtten;
	lLevels = Level;
	gLevels = glvs;
}

void Magnify::butter(double Wn, double *a, double *b)
{
	double fs=2.0, data=-1.0;

	Wn = 2*fs*tan(PI*Wn/fs);
	data *= Wn;
	double t = 1.0/fs;
	double t1 = 1.0 + data* t/2.0;
	double t2 = 1.0 - data*t/2.0;
	data = t1/t2;

	b[0] = 1.0;
	b[1] = -data;

	a[0] = a[1] = 1;
	double kern[2] = {1, 1};
	double  temp = (kern[0]*b[0]+kern[1]*b[1])/(kern[0]*a[0]+kern[1]*a[1]);

	a[0] *= temp;
	a[1] *= temp;
}

void Magnify::Read_Video(char *video_name)//读取视频，并将视频中的每帧保存到originalData中
{
	VideoCapture src_video;
	src_video.open(video_name);

	while (true)
	{
		Mat frame;
		src_video>>frame;
		if(frame.empty())
			break;
		frame.convertTo(frame, CV_32FC3, 1);
		vector<Mat> r_g_b(3);
		split(frame, r_g_b);

		Mat original_frame = r_g_b[0].clone();   
		Mat magnify_frame = r_g_b[0].clone();

	//	Mat original_frame = r_g_b[0] * 256 * 256 + r_g_b[1] * 256 + r_g_b[2];
	//	Mat magnify_frame = r_g_b[0] * 256 * 256 + r_g_b[1] * 256 + r_g_b[2];

		originalData.push_back(original_frame);    //将视频图像保存到originalData、magnifyData中
		magnifyData.push_back(magnify_frame);     //magnifyData主要用来保存放大的结果，如果放大的结果不满意，可将magnifyData重置回originalData中的数据
		
		//获取视频图像的拉普拉斯金字塔
		/*vector<Mat> pyr_Mat;
		build_oneframe_Lpyr(r_g_b[0], pyr_Mat);
		video_Lpyr.push_back(pyr_Mat);*/

		//获取视频图像的高斯下采样
		
		/*Mat down=r_g_b[0].clone();    
		for(int i = 0; i < lLevels; i++) //levels为下采样的层次
		{
			pyrDown(r_g_b[0], down);//高斯金字塔下采样
			r_g_b[0] = down;
		}
		video_Gpyr.push_back(down);//pyrData保存高斯金字塔顶层
		*/
	}
	//magnifyData = originalData;

}

//重置magnifyData，将magnifyData变为原始数据
void Magnify::Reset_MagnifyData()
{
	magnifyData.clear();
	int sumOfFram = originalData.size();
	for (int i=0; i<sumOfFram; i++)
	{
		Mat temp = originalData[i].clone();
		magnifyData.push_back(temp);
	}
}

//获取视频每帧的拉普拉斯金字塔，将结果保存到video_Lpyr中
void Magnify::build_video_Lpyr()	
{
	int counts = originalData.size();
	for (int i=0; i<counts; i++)
	{
		vector<Mat> pyr_Mat;
		build_oneframe_Lpyr(originalData.at(i), pyr_Mat);
		video_Lpyr.push_back(pyr_Mat);
	}
}

//建立拉普拉斯金字塔，第一个参数为建立金字塔的源图像，第二个参数保存建立金字塔后的结果
void Magnify::build_oneframe_Lpyr(Mat frame, vector<Mat> &lowpass)
{															   
	Mat down, up;
	for(int i = 0; i < lLevels; i++)
	{
		pyrDown(frame, down);//高斯金字塔下采样
		pyrUp(down, up, Size(frame.cols, frame.rows));//高斯金字塔上采样

		if(i == lLevels -1) {
			Mat lap;
			lap = frame;
			lowpass.push_back(lap);
		}
		else{
			Mat lap = frame - up;//拉普拉斯金字塔
			lowpass.push_back(lap);
		}

		frame = down;
	}
}


//获取视频每帧的高斯金字塔顶层，将结果保存到video_Gpyr中
void Magnify::build_video_Gpyr() 
{
	int counts = originalData.size();
	for (int i=0; i<counts; i++)
	{
		Mat down = originalData[i].clone();
		Mat temp = originalData[i].clone();
		for (int j=0; j<gLevels; j++)
		{
			pyrDown(temp, down);
			temp = down;
		}
		video_Gpyr.push_back(down);
	}
}


//对滤波后的拉普拉斯金字塔进行重建，第一个参数为滤波后的金字塔，第二个参数为重建后的结果
void Magnify::reconLpyr(vector<Mat> filter, Mat &output)
{														
	int levels = filter.size()-1;						
	int i;
	Mat out = filter.at(levels);
	for (i = levels-1; i >= 0; i--)
	{
		Mat lap_up;
		pyrUp(out, lap_up, Size(filter.at(i).cols, filter.at(i).rows));
		out = lap_up + filter.at(i);
	}
	output = out.clone();
}

//巴特沃斯滤波器放大，fl-fh为放大的频率范围，alpha为放大倍数
void Magnify::Lpyr_Butter(double fl, double fh, double alpha)
{
	//获取巴特沃斯滤波器系数
	double low_a[2], low_b[2], high_a[2], high_b[2];
	butter(fl/samplingRate, low_a, low_b);
	butter(fh/samplingRate, high_a, high_b);

	vector<Mat> lowpass1, lowpass2, pyr_prev, pyr, filter(lLevels);
	int row = originalData[0].rows;
	int col = originalData[0].cols;
	int numOfFrame = originalData.size();

	for(int k=0; k<numOfFrame; k++)
	{	
		if(k==0) //初始化lowpass1, lowpass2, pyr_prev
		{
			build_oneframe_Lpyr(magnifyData[k], lowpass1);
			build_oneframe_Lpyr(magnifyData[k], lowpass2);
			build_oneframe_Lpyr(magnifyData[k], pyr_prev);
		}

		else
		{
			pyr = video_Lpyr.at(k);
			for (int i = 0; i < lLevels; i++)//滤波过程
			{
				lowpass1.at(i) = (-high_b[1])*lowpass1.at(i) + (high_a[0])*pyr.at(i) + (high_a[1])*pyr_prev.at(i);
				lowpass2.at(i) = (-low_b[1])*lowpass2.at(i) + (low_a[0])*pyr.at(i) + (low_a[1])*pyr_prev.at(i);
				Mat temp = lowpass1.at(i) - lowpass2.at(i);
				filter.at(i) = temp;
				pyr_prev.at(i) = pyr.at(i);
			}

			double delta =lambda_c/8/(1+alpha);
			double exaggeration_factor = 2.0;
			double lambda = sqrt((double)row*row + (double)col*col)/3;

			for (int i = lLevels - 1; i >= 0; i--)//对滤波进行放大
			{
				double currAlpha = (double)lambda/delta/8-1.0;
				currAlpha *= exaggeration_factor;

				if(i == lLevels - 1 || i == 0)
					filter.at(i) *= 0.0;
				else if(currAlpha > alpha)
					filter.at(i) *= alpha;
				else
					filter.at(i) *= currAlpha;
				lambda /= 2.0;
			}

			//由滤波后的拉普拉斯金字塔进行复原
			Mat out;
			reconLpyr(filter, out);

			//原图像加上滤波后的图像
			magnifyData[k] += out;

			//以视频方式显示结果，需将浮点型转化为uchar型才能正确显示
			//Mat img;
			//frame.convertTo(img, CV_8UC1);
			//imshow("win",img);
			//waitKey(30);

		}
	}

}

//理想带通滤波器进行放大，alpha为放大倍数，fl-fh为放大的频率范围，samplingRate为采样率
void Magnify::ideal_bandpassing(float fl, float fh, float alpha, int samplingRate)
{
	int guss_row = video_Gpyr[0].rows;
	int guss_col = video_Gpyr[0].cols;
	int sumOfFrame = video_Gpyr.size();
	Mat ideal_filt(sumOfFrame, guss_row*guss_col, CV_32FC1);//保存高斯下采样的所有帧的数据，一行为一帧

	//把video_Gpyr中的数据存入filt中，以便进行傅里叶变换
	for (int i = 0; i < sumOfFrame; i++)
	{
		int sum = 0;
		for (int j = 0; j < guss_row; j++)
		{
			for (int k = 0; k < guss_col; k++)
			{
				ideal_filt.at<float>(i, sum++) = video_Gpyr[i].at<float>(j, k);
			}
		}
	}

	ideal_filt = ideal_filt.t();//矩阵转置,作为傅里叶变换的实部
	Mat ideal_filt_im(ideal_filt.rows, ideal_filt.cols, CV_32FC1, Scalar::all(0));//傅里叶变换虚部

	vector<Mat> dft_mat(2);
	dft_mat[0] = ideal_filt;//实部 像素总和*帧 
	dft_mat[1] = ideal_filt_im;//虚部 像素总和*帧 
	Mat dft_src(ideal_filt.rows, ideal_filt.cols, CV_32FC2);//像素总和*帧 
	Mat dft_dst(ideal_filt.rows, ideal_filt.cols, CV_32FC2);//像素总和*帧 

	merge(dft_mat, dft_src);    //傅里叶变换的实部和虚部相结合为dft_src
	dft(dft_src, dft_dst, CV_DXT_ROWS, 0);    //将数据dft_src进行傅里叶变换，结果保存到dft_dst

	int *mask = new int[sumOfFrame];
	for (int i=0; i<sumOfFrame; i++)  //截取fl与fh之间的频率范围，在范围内的为1，不在范围内的为0，用mask保存结果
	{
		float flag = (float)i / sumOfFrame * samplingRate;//采样率
		if(flag-fl >= 0.0000001 && flag-fh <= 0.0000001)
			mask[i] = 1;
		else
			mask[i] = 0;
	}

	for (int i=0; i<dft_dst.rows; i++)//对傅里叶变换后进行频率截取
	{
		for (int j=0; j<dft_dst.cols; j++)
		{
			dft_dst.at<float>(i, 2*j) *= mask[j];
			dft_dst.at<float>(i, 2*j+1) *= mask[j];
		}
	}
	delete []mask;

	for (int i = 0; i < dft_dst.rows; i++)//对傅里叶变换后的结果进行逆向傅里叶变换，将逆变换后的结果保存到dft_src
	{
		dft(dft_dst.row(i), dft_src.row(i), CV_DXT_INVERSE_SCALE, 0);
	}
	split(dft_src, dft_mat);//分离逆变换后的实部和虚部，实部保存到dft_mat[0]中，虚部保存到dft_mat[1]中

	dft_mat[0] = dft_mat[0].t();
	dft_mat[0] *= alpha;//对频率截取的结果进行放大

	int filt_row = 0;
	for (int i=0; i<dft_mat[0].rows; i++)
	{
		Mat filtFrame(guss_row, guss_col, CV_32FC1);
		for (int j=0; j<dft_mat[0].cols; j++)//将频率截取后的结果一帧一帧的保存到filtFrame中
		{
			int k = j / guss_col;
			filtFrame.at<float>(k, filt_row++) = dft_mat[0].at<float>(i, j);
			if(filt_row % guss_col == 0)
				filt_row = 0;
		}
		Mat resize_filt;
		resize(filtFrame, resize_filt, Size(magnifyData[0].cols, magnifyData[0].rows));//将filtFrame还原为原始图像的大小
		magnifyData[i] += resize_filt;  //放大后的结果和原始图像相加

		imshow("win", magnifyData[i]/255.0);
		waitKey(30);
	}
}

void Magnify::ideal_Gpyr(float *fl, float *fh, float *alpha, int nFilters, int samplingRate)
{
	int guss_row = video_Gpyr[0].rows;
	int guss_col = video_Gpyr[0].cols;
	int sumOfFrame = video_Gpyr.size();
	Mat ideal_filt(sumOfFrame, guss_row*guss_col, CV_32FC1);//保存高斯下采样的所有帧的数据，一行为一帧

	//把video_Gpyr中的数据存入filt中，以便进行傅里叶变换
	for (int i = 0; i < sumOfFrame; i++)
	{
		int sum = 0;
		for (int j = 0; j < guss_row; j++)
		{
			for (int k = 0; k < guss_col; k++)
			{
				ideal_filt.at<float>(i, sum++) = video_Gpyr[i].at<float>(j, k);
			}
		}
	}

	ideal_filt = ideal_filt.t();//矩阵转置,作为傅里叶变换的实部
	Mat ideal_filt_im(ideal_filt.rows, ideal_filt.cols, CV_32FC1, Scalar::all(0));//傅里叶变换虚部

	vector<Mat> dft_mat(2);
	dft_mat[0] = ideal_filt;//实部 像素总和*帧 
	dft_mat[1] = ideal_filt_im;//虚部 像素总和*帧 
	Mat dft_src(ideal_filt.rows, ideal_filt.cols, CV_32FC2);//像素总和*帧 
	Mat dft_dst(ideal_filt.rows, ideal_filt.cols, CV_32FC2);//像素总和*帧 

	merge(dft_mat, dft_src);    //傅里叶变换的实部和虚部相结合为dft_src
	dft(dft_src, dft_dst, CV_DXT_ROWS, 0);    //将数据dft_src进行傅里叶变换，结果保存到dft_dst

	float *mask = new float[sumOfFrame];
	for(int i = 0; i < sumOfFrame; i ++){
		mask[i] = 0;
	}
	for(int f = 0; f < nFilters; f ++){
		int begin = fl[f] * sumOfFrame / samplingRate;
		int end = (fh[f]+0.000001) * sumOfFrame / samplingRate;
		for(int i = begin; i < end; i ++){
			mask[i] = alpha[f];
		}
	}

	for (int i=0; i<dft_dst.rows; i++)//对傅里叶变换后进行频率截取
	{
		for (int j=0; j<dft_dst.cols; j++)
		{
			dft_dst.at<float>(i, 2*j) *= mask[j];
			dft_dst.at<float>(i, 2*j+1) *= mask[j];
		}
	}
	delete []mask;

	for (int i = 0; i < dft_dst.rows; i++)//对傅里叶变换后的结果进行逆向傅里叶变换，将逆变换后的结果保存到dft_src
	{
		dft(dft_dst.row(i), dft_src.row(i), CV_DXT_INVERSE_SCALE, 0);
	}
	split(dft_src, dft_mat);//分离逆变换后的实部和虚部，实部保存到dft_mat[0]中，虚部保存到dft_mat[1]中

	dft_mat[0] = dft_mat[0].t();

	int filt_row = 0;
	for (int i=0; i<dft_mat[0].rows; i++)
	{
		Mat filtFrame(guss_row, guss_col, CV_32FC1);
		for (int j=0; j<dft_mat[0].cols; j++)//将频率截取后的结果一帧一帧的保存到filtFrame中
		{
			int k = j / guss_col;
			filtFrame.at<float>(k, filt_row++) = dft_mat[0].at<float>(i, j);
			if(filt_row % guss_col == 0)
				filt_row = 0;
		}
		Mat resize_filt;
		resize(filtFrame, resize_filt, Size(magnifyData[0].cols, magnifyData[0].rows));//将filtFrame还原为原始图像的大小
		magnifyData[i] += resize_filt;  //放大后的结果和原始图像相加

		imshow("win", magnifyData[i]/255.0);
		waitKey(30);
	}
}

void Magnify::Play_Video() //显示放大后的结果
{
	for (int i=0; i<(int)magnifyData.size(); i++)
	{			
		imshow("win",magnifyData[i]/255.0);
		waitKey(30);
	}
}

void Magnify::write2video(char *out_name)//将放大结果保存为视频，参数为保存的视频名称
{
	int frame_row = magnifyData[0].rows;
	int frame_col = magnifyData[0].cols;
	VideoWriter writer(out_name, CV_FOURCC('M','J','P','G'), 30.0, Size(frame_col,frame_row),1);

	Mat video_frame(frame_row, frame_col, CV_32FC3);//3通道视频帧，每一通道的值都相同

	vector<Mat> r_g_b(3);//r、g、b通道分别存储相同的灰度值
	for (int i=0; i<magnifyData.size(); i++)
	{
		r_g_b[0] = magnifyData[i]; //r通道
		r_g_b[1] = magnifyData[i]; //g通道
		r_g_b[2] = magnifyData[i];// b通道
		merge(r_g_b, video_frame);//将3个单通道合并为一个3通道的Mat

		Mat res;
		video_frame.convertTo(res, CV_8UC3, 1);//将3通道的Mat的数据类型转化为无符号字符型
		writer<<res;//写入视频中
	}

}

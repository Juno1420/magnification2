#include "Magnify.h"



Magnify::~Magnify()
{

}

/*

//�˳���Ӧ�ÿ��Բ���
void Magnify::Read_Video(char *video_name)//��ȡ��Ƶ��������Ƶ�е�ÿ֡���浽originalData��
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
	
	Mat video_frame(frame_row, frame_col, CV_32FC3);//3ͨ����Ƶ֡��ÿһͨ����ֵ����ͬ
	
	for (int i=0; i<src.size(); i++)
	{
		vector<Mat> r_g_b(3);//r��g��bͨ���ֱ�洢��ͬ�ĻҶ�ֵ
		r_g_b[0] = src[i]; //rͨ��
		r_g_b[1] = src[i]; //gͨ��
		r_g_b[2] = src[i];//bͨ��
		merge(r_g_b, video_frame);//��3����ͨ���ϲ�Ϊһ��3ͨ����Mat

		Mat res;
		video_frame.convertTo(res, CV_8UC3, 1);//��3ͨ����Mat����������ת��Ϊ�޷����ַ���
		writer<<res;//д����Ƶ��
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

void Magnify::Read_Video(char *video_name)//��ȡ��Ƶ��������Ƶ�е�ÿ֡���浽originalData��
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

		originalData.push_back(original_frame);    //����Ƶͼ�񱣴浽originalData��magnifyData��
		magnifyData.push_back(magnify_frame);     //magnifyData��Ҫ��������Ŵ�Ľ��������Ŵ�Ľ�������⣬�ɽ�magnifyData���û�originalData�е�����
		
		//��ȡ��Ƶͼ���������˹������
		/*vector<Mat> pyr_Mat;
		build_oneframe_Lpyr(r_g_b[0], pyr_Mat);
		video_Lpyr.push_back(pyr_Mat);*/

		//��ȡ��Ƶͼ��ĸ�˹�²���
		
		/*Mat down=r_g_b[0].clone();    
		for(int i = 0; i < lLevels; i++) //levelsΪ�²����Ĳ��
		{
			pyrDown(r_g_b[0], down);//��˹�������²���
			r_g_b[0] = down;
		}
		video_Gpyr.push_back(down);//pyrData�����˹����������
		*/
	}
	//magnifyData = originalData;

}

//����magnifyData����magnifyData��Ϊԭʼ����
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

//��ȡ��Ƶÿ֡��������˹����������������浽video_Lpyr��
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

//����������˹����������һ������Ϊ������������Դͼ�񣬵ڶ����������潨����������Ľ��
void Magnify::build_oneframe_Lpyr(Mat frame, vector<Mat> &lowpass)
{															   
	Mat down, up;
	for(int i = 0; i < lLevels; i++)
	{
		pyrDown(frame, down);//��˹�������²���
		pyrUp(down, up, Size(frame.cols, frame.rows));//��˹�������ϲ���

		if(i == lLevels -1) {
			Mat lap;
			lap = frame;
			lowpass.push_back(lap);
		}
		else{
			Mat lap = frame - up;//������˹������
			lowpass.push_back(lap);
		}

		frame = down;
	}
}


//��ȡ��Ƶÿ֡�ĸ�˹���������㣬��������浽video_Gpyr��
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


//���˲����������˹�����������ؽ�����һ������Ϊ�˲���Ľ��������ڶ�������Ϊ�ؽ���Ľ��
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

//������˹�˲����Ŵ�fl-fhΪ�Ŵ��Ƶ�ʷ�Χ��alphaΪ�Ŵ���
void Magnify::Lpyr_Butter(double fl, double fh, double alpha)
{
	//��ȡ������˹�˲���ϵ��
	double low_a[2], low_b[2], high_a[2], high_b[2];
	butter(fl/samplingRate, low_a, low_b);
	butter(fh/samplingRate, high_a, high_b);

	vector<Mat> lowpass1, lowpass2, pyr_prev, pyr, filter(lLevels);
	int row = originalData[0].rows;
	int col = originalData[0].cols;
	int numOfFrame = originalData.size();

	for(int k=0; k<numOfFrame; k++)
	{	
		if(k==0) //��ʼ��lowpass1, lowpass2, pyr_prev
		{
			build_oneframe_Lpyr(magnifyData[k], lowpass1);
			build_oneframe_Lpyr(magnifyData[k], lowpass2);
			build_oneframe_Lpyr(magnifyData[k], pyr_prev);
		}

		else
		{
			pyr = video_Lpyr.at(k);
			for (int i = 0; i < lLevels; i++)//�˲�����
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

			for (int i = lLevels - 1; i >= 0; i--)//���˲����зŴ�
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

			//���˲����������˹���������и�ԭ
			Mat out;
			reconLpyr(filter, out);

			//ԭͼ������˲����ͼ��
			magnifyData[k] += out;

			//����Ƶ��ʽ��ʾ������轫������ת��Ϊuchar�Ͳ�����ȷ��ʾ
			//Mat img;
			//frame.convertTo(img, CV_8UC1);
			//imshow("win",img);
			//waitKey(30);

		}
	}

}

//�����ͨ�˲������зŴ�alphaΪ�Ŵ�����fl-fhΪ�Ŵ��Ƶ�ʷ�Χ��samplingRateΪ������
void Magnify::ideal_bandpassing(float fl, float fh, float alpha, int samplingRate)
{
	int guss_row = video_Gpyr[0].rows;
	int guss_col = video_Gpyr[0].cols;
	int sumOfFrame = video_Gpyr.size();
	Mat ideal_filt(sumOfFrame, guss_row*guss_col, CV_32FC1);//�����˹�²���������֡�����ݣ�һ��Ϊһ֡

	//��video_Gpyr�е����ݴ���filt�У��Ա���и���Ҷ�任
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

	ideal_filt = ideal_filt.t();//����ת��,��Ϊ����Ҷ�任��ʵ��
	Mat ideal_filt_im(ideal_filt.rows, ideal_filt.cols, CV_32FC1, Scalar::all(0));//����Ҷ�任�鲿

	vector<Mat> dft_mat(2);
	dft_mat[0] = ideal_filt;//ʵ�� �����ܺ�*֡ 
	dft_mat[1] = ideal_filt_im;//�鲿 �����ܺ�*֡ 
	Mat dft_src(ideal_filt.rows, ideal_filt.cols, CV_32FC2);//�����ܺ�*֡ 
	Mat dft_dst(ideal_filt.rows, ideal_filt.cols, CV_32FC2);//�����ܺ�*֡ 

	merge(dft_mat, dft_src);    //����Ҷ�任��ʵ�����鲿����Ϊdft_src
	dft(dft_src, dft_dst, CV_DXT_ROWS, 0);    //������dft_src���и���Ҷ�任��������浽dft_dst

	int *mask = new int[sumOfFrame];
	for (int i=0; i<sumOfFrame; i++)  //��ȡfl��fh֮���Ƶ�ʷ�Χ���ڷ�Χ�ڵ�Ϊ1�����ڷ�Χ�ڵ�Ϊ0����mask������
	{
		float flag = (float)i / sumOfFrame * samplingRate;//������
		if(flag-fl >= 0.0000001 && flag-fh <= 0.0000001)
			mask[i] = 1;
		else
			mask[i] = 0;
	}

	for (int i=0; i<dft_dst.rows; i++)//�Ը���Ҷ�任�����Ƶ�ʽ�ȡ
	{
		for (int j=0; j<dft_dst.cols; j++)
		{
			dft_dst.at<float>(i, 2*j) *= mask[j];
			dft_dst.at<float>(i, 2*j+1) *= mask[j];
		}
	}
	delete []mask;

	for (int i = 0; i < dft_dst.rows; i++)//�Ը���Ҷ�任��Ľ������������Ҷ�任������任��Ľ�����浽dft_src
	{
		dft(dft_dst.row(i), dft_src.row(i), CV_DXT_INVERSE_SCALE, 0);
	}
	split(dft_src, dft_mat);//������任���ʵ�����鲿��ʵ�����浽dft_mat[0]�У��鲿���浽dft_mat[1]��

	dft_mat[0] = dft_mat[0].t();
	dft_mat[0] *= alpha;//��Ƶ�ʽ�ȡ�Ľ�����зŴ�

	int filt_row = 0;
	for (int i=0; i<dft_mat[0].rows; i++)
	{
		Mat filtFrame(guss_row, guss_col, CV_32FC1);
		for (int j=0; j<dft_mat[0].cols; j++)//��Ƶ�ʽ�ȡ��Ľ��һ֡һ֡�ı��浽filtFrame��
		{
			int k = j / guss_col;
			filtFrame.at<float>(k, filt_row++) = dft_mat[0].at<float>(i, j);
			if(filt_row % guss_col == 0)
				filt_row = 0;
		}
		Mat resize_filt;
		resize(filtFrame, resize_filt, Size(magnifyData[0].cols, magnifyData[0].rows));//��filtFrame��ԭΪԭʼͼ��Ĵ�С
		magnifyData[i] += resize_filt;  //�Ŵ��Ľ����ԭʼͼ�����

		imshow("win", magnifyData[i]/255.0);
		waitKey(30);
	}
}

void Magnify::ideal_Gpyr(float *fl, float *fh, float *alpha, int nFilters, int samplingRate)
{
	int guss_row = video_Gpyr[0].rows;
	int guss_col = video_Gpyr[0].cols;
	int sumOfFrame = video_Gpyr.size();
	Mat ideal_filt(sumOfFrame, guss_row*guss_col, CV_32FC1);//�����˹�²���������֡�����ݣ�һ��Ϊһ֡

	//��video_Gpyr�е����ݴ���filt�У��Ա���и���Ҷ�任
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

	ideal_filt = ideal_filt.t();//����ת��,��Ϊ����Ҷ�任��ʵ��
	Mat ideal_filt_im(ideal_filt.rows, ideal_filt.cols, CV_32FC1, Scalar::all(0));//����Ҷ�任�鲿

	vector<Mat> dft_mat(2);
	dft_mat[0] = ideal_filt;//ʵ�� �����ܺ�*֡ 
	dft_mat[1] = ideal_filt_im;//�鲿 �����ܺ�*֡ 
	Mat dft_src(ideal_filt.rows, ideal_filt.cols, CV_32FC2);//�����ܺ�*֡ 
	Mat dft_dst(ideal_filt.rows, ideal_filt.cols, CV_32FC2);//�����ܺ�*֡ 

	merge(dft_mat, dft_src);    //����Ҷ�任��ʵ�����鲿����Ϊdft_src
	dft(dft_src, dft_dst, CV_DXT_ROWS, 0);    //������dft_src���и���Ҷ�任��������浽dft_dst

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

	for (int i=0; i<dft_dst.rows; i++)//�Ը���Ҷ�任�����Ƶ�ʽ�ȡ
	{
		for (int j=0; j<dft_dst.cols; j++)
		{
			dft_dst.at<float>(i, 2*j) *= mask[j];
			dft_dst.at<float>(i, 2*j+1) *= mask[j];
		}
	}
	delete []mask;

	for (int i = 0; i < dft_dst.rows; i++)//�Ը���Ҷ�任��Ľ������������Ҷ�任������任��Ľ�����浽dft_src
	{
		dft(dft_dst.row(i), dft_src.row(i), CV_DXT_INVERSE_SCALE, 0);
	}
	split(dft_src, dft_mat);//������任���ʵ�����鲿��ʵ�����浽dft_mat[0]�У��鲿���浽dft_mat[1]��

	dft_mat[0] = dft_mat[0].t();

	int filt_row = 0;
	for (int i=0; i<dft_mat[0].rows; i++)
	{
		Mat filtFrame(guss_row, guss_col, CV_32FC1);
		for (int j=0; j<dft_mat[0].cols; j++)//��Ƶ�ʽ�ȡ��Ľ��һ֡һ֡�ı��浽filtFrame��
		{
			int k = j / guss_col;
			filtFrame.at<float>(k, filt_row++) = dft_mat[0].at<float>(i, j);
			if(filt_row % guss_col == 0)
				filt_row = 0;
		}
		Mat resize_filt;
		resize(filtFrame, resize_filt, Size(magnifyData[0].cols, magnifyData[0].rows));//��filtFrame��ԭΪԭʼͼ��Ĵ�С
		magnifyData[i] += resize_filt;  //�Ŵ��Ľ����ԭʼͼ�����

		imshow("win", magnifyData[i]/255.0);
		waitKey(30);
	}
}

void Magnify::Play_Video() //��ʾ�Ŵ��Ľ��
{
	for (int i=0; i<(int)magnifyData.size(); i++)
	{			
		imshow("win",magnifyData[i]/255.0);
		waitKey(30);
	}
}

void Magnify::write2video(char *out_name)//���Ŵ�������Ϊ��Ƶ������Ϊ�������Ƶ����
{
	int frame_row = magnifyData[0].rows;
	int frame_col = magnifyData[0].cols;
	VideoWriter writer(out_name, CV_FOURCC('M','J','P','G'), 30.0, Size(frame_col,frame_row),1);

	Mat video_frame(frame_row, frame_col, CV_32FC3);//3ͨ����Ƶ֡��ÿһͨ����ֵ����ͬ

	vector<Mat> r_g_b(3);//r��g��bͨ���ֱ�洢��ͬ�ĻҶ�ֵ
	for (int i=0; i<magnifyData.size(); i++)
	{
		r_g_b[0] = magnifyData[i]; //rͨ��
		r_g_b[1] = magnifyData[i]; //gͨ��
		r_g_b[2] = magnifyData[i];// bͨ��
		merge(r_g_b, video_frame);//��3����ͨ���ϲ�Ϊһ��3ͨ����Mat

		Mat res;
		video_frame.convertTo(res, CV_8UC3, 1);//��3ͨ����Mat����������ת��Ϊ�޷����ַ���
		writer<<res;//д����Ƶ��
	}

}

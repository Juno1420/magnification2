#pragma once
#include "Magnify.h"


template<typename T, int nDims> class GridMagnification:
public Magnification<T>{  
public:
	GridMagnification(float samplingRate, unsigned int filterLeng, float* filter, PYRAMID_TYPE pType = Gauss, unsigned int nLevels = 1, TEMPORAL_FILTER_TYPE tfType = IDEAL)
		:Magnification(samplingRate, pType,nLevels,tfType)
	{
		nDimensions = nDims;
		spatialFilterLength = filterLeng;
		for(int i = 0; i < spatialFilterLength; i ++){
			spatialFilter[i] = filter[i];
		}
	}
	virtual void ReadData(std::string fileName)
	{
		std::cout<<"ReadData() of the class GridMagnification"<<std::endl;
	};

	void SetSpatialAttrib(PYRAMID_TYPE pyrT, unsigned int nL, unsigned int fLength, float *filter){
		SetPyramidAttrib(pyrT, nL);
		spatialFilterLength = fLength;
		for(int i = 0; i < spatialFilterLength; i ++){
			spatialFilter[i] = fiter[i];
		}
	};

//	virtual void BuildPyramid();
	virtual void ReadSpecificInfo(ifstream& infile){infile.read(reinterpret_cast<char*> (dataLength), sizeof(unsigned int) * nDims * nPyrLevels);};
	virtual void WriteSpecificInfo(ofstream& outfile){outfile.write(reinterpret_cast<char*> (dataLength), sizeof(unsigned int) * nDims * nPyrLevels);};
	virtual void DownSampling(T* data, T* result, unsigned int level = 0, T* tempBuffer = NULL);
	virtual void UpSampling(T* src, T* dst, unsigned int level);
	virtual void GetPyrSize(); 
	virtual void CreateDataArea();
//	void CollapsePyramid(int frame, T* dst, T* buffer = 0);
//	void CollapsePyramid(T* pyramid, T* dst, T* buffer = 0);

	unsigned int dataLength[10][nDims]; //各层中各维的长度
	unsigned int nDimensions; //维数
	unsigned int spatialFilterLength;
	float spatialFilter[100];
};  


template<typename T, int nDims> 
void GridMagnification<T, nDims>::GetPyrSize()
{
	int i, j;
	pyrSize[0] = 1;
	for(i = 0; i < nDimensions; i ++){
		pyrSize[0] *= dataLength[0][i];
	}

	for(i = 1; i < nPyrLevels; i ++){
		pyrSize[i]  = 1;
		for(j = 0; j < nDimensions; j ++){
			if(dataLength[i - 1][j] % 2 == 0)
				dataLength[i][j] = dataLength[i - 1][j] / 2;
			else
				dataLength[i][j] = dataLength[i - 1][j] / 2 + 1;

			pyrSize[i] *= dataLength[i][j];
		}
	}

	if(pyrType == GAUSS){
		totalPyrSize = pyrSize[nPyrLevels - 1];
	}
	else{
		totalPyrSize = 0;
		for(i = 0; i < nPyrLevels; i ++){//各层数据放在一块连续的内存中
			totalPyrSize += pyrSize[i];
		}
	}
} 

template<typename T, int nDims> 
void GridMagnification<T, nDims>:: DownSampling(T* data, T* result, unsigned int level, T* tempBuffer)
{
	int step[5], start, end, filtExtent, resultP, dataP, rowP, colP, pointP;
	int i, j, k, pg, p;
	T filterValue, temp[1024];
	unsigned int *originSize, size[nDims];

	int dataSize;
	bool needDelete = 0;
	originSize = dataLength[level];
	if(!tempBuffer){//临时空间假如在程序外没有开，则在程序中需要开空间
		if(originSize[nDims - 1] % 2 == 0)//最后一维最先降采样
			dataSize = originSize[nDims - 1] / 2;
		else 
			dataSize = originSize[nDims - 1] / 2 + 1;

		for(i = 0; i < nDims - 1; i ++){
			dataSize *= originSize[i];
		}

		tempBuffer = new T[dataSize];
		needDelete = 1;
	}

	for(i = 0; i < nDims; i ++){
		size[i] = originSize[i];
	}

	start = spatialFilterLength / 2;
	if(start % 2 == 1) start ++;
	filtExtent = -(spatialFilterLength / 2);

	switch(nDimensions){
	case 2:
		end = size[1] + 1 - spatialFilterLength / 2;
		if(end % 2 == 1) 
			end --;

		resultP = 0;
		for(i = 0; i < size[0]; i ++){//对每行进行一维滤波
			rowP = i * size[1];
			for(j = 0; j < start; j += 2){//每行开始的边缘区域
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
					if(p < 0)
						dataP = rowP - p;
					else
						dataP = rowP + p;
					filterValue += (data[dataP] * spatialFilter[k]);
				}
				tempBuffer[resultP ++] = filterValue;
			}

			for(j = start; j < end; j += 2){//中间
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
					dataP = rowP + p;
					filterValue += (data[dataP] * spatialFilter[k]);
				}
				tempBuffer[resultP ++] = filterValue;
			}

			for(j = end; j < size[1]; j += 2){//结尾边缘
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
					if(p >= size[1])
						dataP = rowP + 2* size[1] - 2 - p;
					else
						dataP = rowP + p;
					filterValue += (data[dataP] * spatialFilter[k]);
				}
				tempBuffer[resultP ++] = filterValue;
			}
		}

		//对每列做一维滤波
		if(size[1] % 2 == 1)//前面做行降采样了，所以列数减半
			size[1] = size[1] / 2 + 1;
		else 
			size[1] = size[1] / 2;

		end = size[0] + 1 - spatialFilterLength / 2;
		if(end % 2 == 1) 
			end --;

		
		for(i = 0; i < size[1]; i ++)
		{
			resultP = 0;
			rowP = 0;
			for(j = 0; j < start; j += 2){//每列开始的边缘区域
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
					if(p < 0)
						dataP = i - p * size[1];
					else
						dataP = i + p * size[1];
					filterValue += (tempBuffer[dataP] * spatialFilter[k]);
				}
			//	temp[resultP] = filterValue;
			//	resultP += 2;
				result[rowP + i] = filterValue;
				rowP += size[1];
			}

			for(j = start; j < end; j += 2){//每列开始的边缘区域
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
					dataP = i + p * size[1];
					filterValue += (tempBuffer[dataP] * spatialFilter[k]);
				}
			//	temp[resultP] = filterValue;
			//	resultP += 2;
				result[rowP + i] = filterValue;
				rowP += size[1];
			}

			for(j = end; j < size[0]; j += 2){//结尾边缘
				filterValue = 0;
				for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
					if(p >= size[0])
						dataP = i + (2* size[0] - 2 - p) * size[1];
					else
						dataP = i + p * size[1];
					filterValue +=  (tempBuffer[dataP] * spatialFilter[k]);
				}
			//	temp[resultP] = filterValue;
			//	resultP += 2;
				result[rowP + i] = filterValue;
				rowP += size[1];
			}

		//	for(j = 0; j < size[0]; j += 2){
		//		result[j * size[1] + i] = temp[j];
		//	}
		}

		//对列downsampling
	/*	int row1, row2;
		for(i = 2; i < size[0]; i += 2){
			row1  = i * size[1];
			row2  = (i / 2) * size[1];
			for(j = 0; j < size[1]; j ++){
				result[row2 + j] = result[row1 + j];
			}
		}*/
		if(size[0] % 2 == 0)
			size[0] = size[0] / 2;
		else 
			size[0] = size[0] / 2 + 1;

		break;

	case 3:
		int pageStep, pageStart;

		////////下面对数据中的第三维进行一维滤波///////////////////
		end = size[2] + 1 - spatialFilterLength / 2;
		if(end % 2 == 1) 
			end --;
		pageStep = size[1] * size[2];
		resultP = 0;
		for(pg = 0; pg < size[0]; pg ++){
			pageStart = pg * pageStep;
			for(i = 0; i < size[1]; i ++){
				rowP = pageStart + i * size[2];
				for(j = 0; j < start; j += 2){//每行开始的边缘区域
					filterValue = 0;
					for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
						if(p < 0)
							dataP = rowP - p;
						else
							dataP = rowP + p;
						filterValue += (data[dataP] * spatialFilter[k]);
					}
					tempBuffer[resultP ++] = filterValue;
				}

				for(j = start; j < end; j += 2){//中间
					filterValue = 0;
					for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
						dataP = rowP + p;
						filterValue += (data[dataP] * spatialFilter[k]);
					}
					tempBuffer[resultP ++] = filterValue;
				}

				for(j = end; j < size[2]; j += 2){//结尾边缘
					filterValue = 0;
					for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
						if(p >= size[2])
							dataP = rowP + 2* size[2] - 2 - p;
						else
							dataP = rowP + p;
						filterValue += (data[dataP] * spatialFilter[k]);
					}
					tempBuffer[resultP ++] = filterValue;
				}
			}
		}
		if(size[2] % 2 == 1)//前面做降采样了，所以维数减半
			size[2] = size[2] / 2 + 1;
		else 
			size[2] = size[2] / 2;
		pageStep = size[1] * size[2];


		///////下面对数据的第二维做一维滤波	//////////
		end = size[1] + 1 - spatialFilterLength / 2;
		if(end % 2 == 1) 
			end --;
		
		for(pg = 0; pg < size[0]; pg ++){//对第二维进行一维滤波
			pageStart = pg * pageStep;
			for(i = 0; i < size[2]; i ++){
				resultP = 0;
				for(j = 0; j < start; j += 2){//每行开始的边缘区域
					filterValue = 0;
					colP = pageStart + i;
					for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
						if(p < 0)
							dataP = colP - p * size[2];
						else
							dataP = colP + p * size[2];
						filterValue += (tempBuffer[dataP] * spatialFilter[k]);
					}
					temp[resultP] = filterValue;
					resultP += 2;
				}

				for(j = start; j < end; j += 2){//每列开始的边缘区域
					filterValue = 0;
					for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
						dataP = colP + p * size[2];
						filterValue += (tempBuffer[dataP] * spatialFilter[k]);
					}
					temp[resultP] = filterValue;
					resultP += 2;
				}

				for(j = end; j < size[1]; j += 2){//结尾边缘
					filterValue = 0;
					for(k = 0, p  = j + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
						if(p >= size[1])
							dataP = colP + (2* size[1] - 2 - p) * size[2];
						else
							dataP = colP + p * size[2];
						filterValue +=  (tempBuffer[dataP] * spatialFilter[k]);
					}
					temp[resultP] = filterValue;
					resultP += 2;
				}

				for(j = 0; j < size[1]; j += 2){
					tempBuffer[colP + j * size[2]] = temp[j];
				}
			}
		}
		//对第二维downsampling
		resultP = 0;
		for(pg = 0; pg < size[0]; pg ++){
			pageStart = pageStep * pg;
			for(i = 0; i < size[1]; i += 2){
				rowP = i * size[2] + pageStart;
				for(j = 0; j < size[2]; j ++){
					tempBuffer[resultP ++ ] = tempBuffer[rowP + j];
				}
			}
		}

		if(size[1] % 2 == 1)//前面做降采样了，所以维数减半
			size[1] = size[1] / 2 + 1;
		else 
			size[1] = size[1] / 2;
		pageStep = size[1] * size[2];


		///////下面对数据的第一维做一维滤波	//////////
		end = size[0] + 1 - spatialFilterLength / 2;
		if(end % 2 == 1) 
			end --;
		
		for(i = 0; i < size[1]; i ++){
			for(j = 0; j < size[2]; j ++){
				pointP = i * size[2] + j;
			//	resultP = 0;
				resultP = pointP;
				for(pg = 0; pg < start; pg += 2){
					filterValue = 0;
					for(k = 0, p  = pg + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
						if(p < 0)
							dataP = pointP - p * pageStep;
						else
							dataP = pointP + p * pageStep;
						filterValue += (tempBuffer[dataP] * spatialFilter[k]);
					}
			//		temp[resultP] = filterValue;
			//		resultP += 2;
					result[resultP] = filterValue;
					resultP += pageStep;
				}

				for(pg = start; pg < end; pg += 2){
					filterValue = 0;
					for(k = 0, p  = pg + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
						dataP = pointP + p * pageStep;
						filterValue += (tempBuffer[dataP] * spatialFilter[k]);
					}
			//		temp[resultP] = filterValue;
			//		resultP += 2;
					result[resultP] = filterValue;
					resultP += pageStep;
				}

				for(pg = end; pg < size[0]; pg += 2){
					filterValue = 0;
					for(k = 0, p  = pg + filtExtent; k < spatialFilterLength; k ++, p ++){//此段做一维滤波
						if(p >= size[0])
							dataP = pointP + (2* size[0] - 2 - p) * pageStep;
						else
							dataP = pointP + p * pageStep;
						filterValue +=  (tempBuffer[dataP] * spatialFilter[k]);
					}
			//		temp[resultP] = filterValue;
			//		resultP += 2;
					result[resultP] = filterValue;
					resultP += pageStep;
				}

			//	for(pg = 0; pg < size[0]; pg += 2){
			//		result[pointP + pg * pageStep] = temp[pg];
			//	}
			}
		}

		//对第一维downsampling
	/*	resultP = pageStep;
		for(pg = 2; pg < size[0]; pg += 2){
			pageStart = pageStep * pg;
			for(i = 0; i < size[1]; i ++){
				rowP = i * size[2] + pageStart;
				for(j = 0; j < size[2]; j ++){
					result[resultP ++ ] = result[rowP + j];
				}
			}
		}*/

		if(size[0] % 2 == 1)//前面做降采样了，所以维数减半
			size[0] = size[0] / 2 + 1;
		else 
			size[0] = size[0] / 2;
		
		break;
	default:
		std::cout<<"the downsampling function in this dimension has not been realized yet";
		exit(-1);
	}

	if(needDelete){
		delete []tempBuffer;
		tempBuffer = 0;
	}
}


template<typename T, int nDims> 
void GridMagnification<T, nDims>:: UpSampling(T* src, T* dst, unsigned int level)
{
	int i, j, pg, srcRow, dstRow, dstPos;
	unsigned int *srcSize, *dstSize;
	srcSize = dataLength[level];
	dstSize = dataLength[level - 1];
	switch(nDims){
	case 2:
		//先拷贝原有格点上的数据
		for(i = 0; i < srcSize[0]; i ++){
			srcRow = i * srcSize[1];
			dstRow = i * 2 * dstSize[1];
			for(j = 0; j < srcSize[1]; j ++){
				dst[dstRow + j * 2] = src[srcRow + j];
			}
		}

		//再插值新产生的格点上的数据,先对每行插值，再对每列插值
		for(i = 0; i < dstSize[0]; i += 2){
			dstRow = i * dstSize[1];
			for(j = 1; j < dstSize[1] - 1; j += 2){
				dstPos = dstRow + j;
				dst[dstPos] = (dst[dstPos - 1] * 0.5 + dst[dstPos + 1] * 0.5);
			}
		}

		for(i = 1; i < dstSize[0] - 1; i += 2){
			dstRow = i * dstSize[1];
			for(j = 0; j < dstSize[1]; j ++){
				dstPos = dstRow + j;
				dst[dstPos] = (dst[dstPos - dstSize[1]] * 0.5 + dst[dstPos + dstSize[1]] * 0.5);
			}
		}

		//右侧边缘直接拷贝它左边像素的值
		if(srcSize[1] * 2 == dstSize[1]){
			for(i = 0; i < dstSize[0]; i ++){
				dstPos = (i + 1) * dstSize[1] - 1;
				dst[dstPos] = dst[dstPos - 1];
			}
		}
		//下侧边缘直接拷贝它上边像素的值
		if(srcSize[0] * 2 == dstSize[0]){
			dstPos = (dstSize[0] - 1) * dstSize[1];
			for(i = 0; i < dstSize[1]; i ++){
				dst[dstPos] = dst[dstPos - dstSize[1]];
				dstPos ++;
			}
		}

		break;
	case 3:
		//先拷贝原有格点上的数据
		int srcPgStart, dstPgStart, srcPgStep, dstPgStep;
		srcPgStep = srcSize[1] * srcSize[2];
		dstPgStep = dstSize[1] * dstSize[2];
		for(pg = 0; pg < srcSize[0]; pg ++){
			srcPgStart = srcPgStep * pg;
			dstPgStart = dstPgStep * pg * 2;
			for(i = 0; i < srcSize[1]; i ++){
				srcRow = srcPgStart + i * srcSize[2];
				dstRow = dstPgStart + i * 2 * dstSize[2];
				for(j = 0; j < srcSize[2]; j ++){
					dst[dstRow + j * 2] = src[srcRow + j];
				}
			}
		}

		//再插值新产生的格点上的数据
		//先在第三维上进行插值
		for(pg = 0; pg < dstSize[0]; pg += 2){
			dstPgStart = dstPgStep * pg;
			for(i = 0; i < dstSize[1]; i += 2){
				dstRow =dstPgStart + i * dstSize[2];
				for(j = 1; j < dstSize[2] - 1; j += 2){
					dstPos = dstRow + j;
					dst[dstPos] = (dst[dstPos - 1] * 0.5 + dst[dstPos + 1] * 0.5);
				}
			}
		}

		//然后在第二维上进行插值
		for(pg = 0; pg < dstSize[0]; pg += 2){
			dstPgStart = dstPgStep * pg;
			for(i = 1; i < dstSize[1] - 1; i += 2){
				dstRow = dstPgStart + i * dstSize[2];
				for(j = 0; j < dstSize[2]; j ++){
					dstPos = dstRow + j;
					dst[dstPos] = (dst[dstPos - dstSize[2]] * 0.5 + dst[dstPos + dstSize[2]] * 0.5);
				}
			}
		}

		//第三维边缘直接拷贝它前面像素的值
		if(srcSize[2] * 2 == dstSize[2]){
			for(pg = 0; pg < dstSize[0]; pg += 2){
				dstPgStart = dstPgStep * pg;
				for(i = 0; i < dstSize[1]; i ++){
					dstPos = dstPgStart + (i + 1) * dstSize[2] - 1;
					dst[dstPos] = dst[dstPos - 1];
				}
			}
		}
		//第二维边缘直接拷贝它上一行像素的值
		if(srcSize[1] * 2 == dstSize[1]){
			for(pg = 0; pg < dstSize[0]; pg += 2){
				dstPgStart = dstPgStep * pg;
				dstPos = dstPgStart + (dstSize[1] - 1) * dstSize[2];
				for(i = 0; i < dstSize[2]; i ++){
					dst[dstPos] = dst[dstPos - dstSize[2]];
					dstPos ++;
				}
			}
		}

		//再在第一维上进行插值
		for(pg = 1; pg < dstSize[0] - 1; pg += 2){
			dstPgStart = dstPgStep * pg;
			for(i = 0; i < dstSize[1]; i ++){
				dstRow =dstPgStart + i * dstSize[2];
				for(j = 0; j < dstSize[2]; j ++){
					dstPos = dstRow + j;
					dst[dstPos] = (dst[dstPos - dstPgStep] * 0.5 + dst[dstPos + dstPgStep] * 0.5);
				}
			}
		}

		//第一维边缘直接拷贝它前一页像素的值
		if(srcSize[0] * 2 == dstSize[0]){
			dstPgStart = dstPgStep * (dstSize[0] - 1);
			for(i = 0; i < dstSize[1]; i ++){
				dstRow =dstPgStart + i * dstSize[2];
				for(j = 0; j < dstSize[2]; j ++){
					dstPos = dstRow + j;
					dst[dstPos] = dst[dstPos - dstPgStep];
				}
			}
		}
		break;

	default:
		std::cout<<"the upsampling function Convolution in this dimension has not been realized yet";
		exit(-1);
	}
}

template<typename T, int nDims> 
void GridMagnification<T, nDims>::CreateDataArea()
{
	if(!dataArea){
		dataArea = new unsigned char[totalPyrSize];
	}

//	memset(dataArea, 255, totalPyrSize * sizeof(unsigned char));//置为无放大区域

	if(nDims == 2){
		for(int a = 0; a < nArea; a ++){
			switch(areaList[a].type){
			case RECTANGLE:
				int rstart, rend, cstart,cend,step;
				float ratio;
				
				if(pyrType == GAUSS){
					ratio= ((float)(dataLength[nPyrLevels - 1][0])) / dataLength[0][0];
					rstart = (int)(areaList[a].ylow * ratio);
					rend = (int)(areaList[a].yhigh * ratio);
					ratio= ((float)(dataLength[nPyrLevels - 1][1])) / dataLength[0][1];
					cstart = (int)(areaList[a].xlow * ratio);
					cend = (int)(areaList[a].xhigh * ratio);
					for(int r = rstart; r < rend; r ++){
						step = r * dataLength[nPyrLevels - 1][1];
						for(int c = cstart; c < cend; c++){
							dataArea[step + c] = a;
						}
					}
				}
				else{
					int lSize = 0;
					for(int l = 0; l < nPyrLevels; l ++){
						ratio= ((float)(dataLength[l][0])) / dataLength[0][0];
						rstart = (int)(areaList[a].ylow * ratio);
						rend = (int)(areaList[a].yhigh * ratio);
						ratio= ((float)(dataLength[l][1])) / dataLength[0][1];
						cstart = (int)(areaList[a].xlow * ratio);
						cend = (int)(areaList[a].xhigh * ratio);

						for(int r = rstart; r < rend; r ++){
							step = lSize + r * dataLength[l][1];
							for(int c = cstart; c < cend; c++){
								dataArea[step + c] = a;
							}
						}

						lSize += pyrSize[l];
					}
				}
				break;
			default:
				break;
			}
		}

		return;
	}

	std::cout<<"We haven't realized the function CreateDataArea for nDims = "<<nDims<<std::endl;
	exit(-1);
}

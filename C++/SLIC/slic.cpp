/*
* slic.cpp
*/

#include "slic.h"


pixel sRGBtoLAB(pixel srgb)
{
	/*将sRGB通道转换为LAB通道
	* 参数:
	* srgb  待转换像元
	* 
	* 返回:
	* lab  转换后的LAB通道像元
	* 
	* 参考网站:https://blog.csdn.net/bby1987/article/details/109522126
	* 
	*/


	pixel RGB = srgb / 255;	//sRGB转RGB
	double r, g, b, X, Y, Z, fx, fy, fz;

	const double Xn = 0.950456;
	const double Yn = 1.0;
	const double Zn = 1.088754;

	pixel lab = srgb;

	//RGB转XYZ

	if (RGB.value[0] > 0.04045) r = pow((RGB.value[0] + 0.055) / 1.055, 2.4);
	else r = pow((RGB.value[0] + 0.055) / 1.055, 2.4);
	if (RGB.value[1] > 0.04045) g = pow((RGB.value[1] + 0.055) / 1.055, 2.4);
	else g = pow((RGB.value[1] + 0.055) / 1.055, 2.4);
	if (RGB.value[2] > 0.04045) b = pow((RGB.value[2] + 0.055) / 1.055, 2.4);
	else b = pow((RGB.value[2] + 0.055) / 1.055, 2.4);
	
	X = r * 0.4124564 + g * 0.3575761 + b * 0.1804375;
	Y = r * 0.2126729 + g * 0.7151522 + b * 0.0721750;
	Z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;
	
	//XYZ转LAB

	if (X > pow(6.0 / 29.0, 3.0)) fx = pow(X, 1.0 / 3.0);
	else fx = (1.0 / 3.0) * pow(19.0 / 6.0, 2.0) * X + (4.0 / 29.0);
	if (Y > pow(6.0 / 29.0, 3.0)) fy = pow(Y, 1.0 / 3.0);
	else fy = (1.0 / 3.0) * pow(19.0 / 6.0, 2.0) * Y + (4.0 / 29.0);
	if (Z > pow(6.0 / 29.0, 3.0)) fz = pow(Z, 1.0 / 3.0);
	else fz = (1.0 / 3.0) * pow(19.0 / 6.0, 2.0) * Z + (4.0 / 29.0);

	lab.value[0] = 116 * fy - 16;
	lab.value[1] = 500 * (fx - fy);
	lab.value[2] = 200 * (fy - fz);

	return lab;

};

label Gradient(label* image, int* region, int width, int height, label pex, int* up_x, int* up_y)
{
	/*寻找像元周围梯度最小值
	* 
	* 参数:
	* image  影像数据
	* region  聚类的上下左右
	* width  影像数据的宽度
	* height  影像数据的高度
	* pex  像元数据
	* up_x,up_y  上一次返回梯度最小像元坐标
	* 
	* 返回:
	* min  像元周围梯度最小值
	*/

	const int nei_x[8] = { 1,0,0,-1,1,1,-1,-1 };
	const int nei_y[8] = { 0,1,-1,0,1,-1,1,-1 };
	int x_obj;
	int y_obj;
	int xx;
	int yy;
	int x = pex.center.x;
	int y = pex.center.y;
	double b1;
	double b2;
	double b3;
	int ind;
	double gradient;
	double min_gradient = DBL_MAX;
	label min = pex;

	for (int i = 0; i < 8; i++)
	{
		x_obj = x + nei_x[i];
		y_obj = y + nei_y[i];
		ind = x_obj + y_obj * width;
		if (x_obj >= region[0] && x_obj < region[1] && y_obj >= region[2] && y_obj < region[3])
		{
			b1 = image[ind].center.value[0] - pex.center.value[0];
			b2 = image[ind].center.value[1] - pex.center.value[1];
			b3 = image[ind].center.value[2] - pex.center.value[2];
			gradient = pow(b1 * b1 + b2 * b2 + b3 * b3, 0.5);
			if (gradient < min_gradient)
			{
				min = image[ind];
				xx = -nei_x[i];
				yy = -nei_y[i];
				min_gradient = gradient;
			}
			else if ((gradient == min_gradient)&&(nei_x[i]== *up_x)&&(nei_y[i] == *up_y))
			{
				
				min = image[ind];
				xx = -nei_x[i];
				yy = -nei_y[i];
				min_gradient = gradient;
			}
		}
	}
	*up_x = xx;
	*up_y = yy;
	return min;

};

label Find_center(label* image, int* region, int width, int height)
{
	/*寻找聚类中心
	* 参数:
	* image  影像数据
	* region  聚类的上下左右
	* width  影像数据的宽度
	* height  影像数据的高度
	* 
	* 返回:
	* min  聚类中心点
	*/

	int min_x = (region[0] + region[1]) / 2;
	int min_y = (region[2] + region[3]) / 2;
	int min_i = min_x + min_y * width;
	int upp_i = -1;
	int up_i = -1;
	int up_x = -2;
	int up_y = -2;
	label min = image[min_i];
	min = Gradient(image, region, width, height, min, &up_x, &up_y);

	/*
	* 寻找附近颜色梯度最低点
	while (upp_i != min_i)  //当上上次返回与此次返回为同一个像元时终止循环
	{
		upp_i = up_i;
		up_i = min_i;
		min = Gradient(image, region, width, height, min, &up_x, &up_y, &count);
		//if (count == 1) cout<<min.center<<endl;
		min_x = min.center.x;
		min_y = min.center.y;
		min_i = min_x + min_y * width;
		count ++;
	}
	*/
	return min;

}

void SLIC_main(double* image, int width, int height, int pxnum, double compactness, int* Labels, int* numLabel, int* len)
{
	/*SLIC算法计算主程序
	* 
	* 参数:
	* image  影像数据
	* width  影像数据的宽度
	* height  影像数据的高度
	* pxnum  需要分割聚类的块数
	* compactness  种子数，用于调整分割时对颜色的灵敏度，越多颜色距离重要性越大
	* 
	* 用于存放结果的参数:
	* Labels  存放结果的数组
	* numLabel  最终分割块数
	* len  最终聚类宽度
	*/


	//label* img = (label*)calloc(width * height, sizeof(label));
	int size = width * height;
	label center_obj;
	label* img = new label[size];	//分配一个动态内存并初始化，以存放影像数据
	int woffset;	//像元宽度除以聚类列数的余数
	int hoffset;	//像元高度除以聚类行数的余数
	int lnum;	//聚类数量
	int lwidth;	//聚类列数
	int lheight;	//聚类行数
	int llen;	//聚类边长
	double cdist;	//颜色距离
	double sdist;	//空间距离
	double dist;	//总距离
	int region[4] = {0,0,0,0};	//聚类上下左右
	int i;	//影像中像元的索引

	//sRGB转LAB

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			i = x + y * width;
			if (i == 215) cout << img[215].center << endl;
			img[i].center.x = x;
			img[i].center.y = y;
			img[i].center.value[0] = image[i];
			img[i].center.value[0] = image[i+ size];
			img[i].center.value[0] = image[i + 2 * size];
			img[i].center = sRGBtoLAB(img[i].center);
		}
	}

	//计算聚类信息

	llen = int(sqrt((double)size / pxnum));
	lwidth = width / llen;
	lheight = height / llen;
	lnum = lwidth * lheight;
	woffset = width - lwidth * llen;
	hoffset = height - lheight * llen;
	int ii;
	int m=0;
	
	//主要计算过程

	for (int w = 0; w < lwidth; w++)
	{
		if (w == (lwidth - 1)) region[1] += (llen + woffset);
		else region[1] += llen;
		region[2] = 0;
		region[3] = 0;
		for (int h = 0; h < lheight; h++)
		{
			m += 1;
			if (h == (lheight - 1)) region[3] += (llen + hoffset);
			else region[3] += llen;
			center_obj = Find_center(img, region, width, height);	//获取聚类中心
			for (int x = region[0] - llen / 2; x < (region[1] + llen / 2); x++)
			{
				if ((x<0) || (x>=width)) continue;
				for (int y = region[2] - llen / 2; y < (region[3] + llen / 2); y++)
				{
					if ((y<0) || (y>=height)) continue;
					ii = x + y * width;

					//计算距离
					cdist = center_obj.cdist2(img[ii].center);
					sdist = center_obj.sdist2(img[ii].center);
					dist = sqrt((cdist + sdist * (((double)compactness * (double)compactness) / ((double)llen * (double)llen))));

					if (dist < img[ii].dist)
					{
						img[ii].mark = m;
						img[ii].dist = dist;
						Labels[ii] = m;
					}
				}
			}
			if (h == (lheight - 1)) region[2] += (llen + hoffset);
			else region[2] += llen;
			
		}
		if (w == (lwidth - 1)) region[0] += (llen + woffset);
		else region[0] += llen;
	
	}
	delete[] img;
	*numLabel = lnum;
	*len = llen;
}












/*
* slic.h
* 
* 简单线性迭代聚类(SLIC)算法
* 
* 参考网站:https://blog.csdn.net/matrix_space/article/details/78618917
* 
* 此代码未进行迭代
* 
*/

#include<iostream>
#include <limits.h>
#include <math.h>
using namespace std;

//创建一个pixel类存放影像中像元的坐标和波段信息
class pixel
{
public:
	int x;
	int y;
	double value[3];
	pixel(int xx = 0, int yy = 0, double b1 = 0, double b2 = 0, double b3 = 0)
		//参数:
		//xx  x坐标
		//yy  y坐标
		// b1,b2,b3  像元的三个波段
	{
		x = xx;
		y = yy;
		value[0] = b1;
		value[1] = b2;
		value[2] = b3;
	};
	friend ostream& operator<<(ostream& output,		//用友元函数向pixel类中添加输出功能,实现:cout<<pixel
		const pixel& D)	
	{
		output << "(" << D.x << ", " << D.y << ", " << D.value[0] << ", " << D.value[1] << ", " << D.value[2]<<")";
		return output;
	}
};

//向pixel中添加四则运算符
pixel operator+(pixel p1, pixel p2)
{
	pixel temp;
	temp.x = p1.x + p2.x;
	temp.y = p1.y + p2.y;
	temp.value[0] = p1.value[0] + p2.value[0];
	temp.value[1] = p1.value[1] + p2.value[1];
	temp.value[2] = p1.value[2] + p2.value[2];
	return temp;
};

pixel operator-(pixel p1, pixel p2)
{
	pixel temp;
	temp.x = p1.x - p2.x;
	temp.y = p1.y - p2.y;
	temp.value[0] = p1.value[0] - p2.value[0];
	temp.value[1] = p1.value[1] - p2.value[1];
	temp.value[2] = p1.value[2] - p2.value[2];
	return temp;
};

pixel operator+(pixel p1, int p2)
{
	pixel temp;
	temp.x = p1.x;
	temp.y = p1.y;
	temp.value[0] = p1.value[0] + p2;
	temp.value[1] = p1.value[1] + p2;
	temp.value[2] = p1.value[2] + p2;
	return temp;
};

pixel operator-(pixel p1, int p2)
{
	pixel temp;
	temp.x = p1.x;
	temp.y = p1.y;
	temp.value[0] = p1.value[0] - p2;
	temp.value[1] = p1.value[1] - p2;
	temp.value[2] = p1.value[2] - p2;
	return temp;
};

pixel operator*(pixel p1, int p2)
{
	pixel temp;
	temp.x = p1.x;
	temp.y = p1.y;
	temp.value[0] = p1.value[0] * p2;
	temp.value[1] = p1.value[1] * p2;
	temp.value[2] = p1.value[2] * p2;
	return temp;
};

pixel operator/(pixel p1, int p2)
{
	pixel temp;
	temp.x = p1.x;
	temp.y = p1.y;
	temp.value[0] = p1.value[0] / p2;
	temp.value[1] = p1.value[1] / p2;
	temp.value[2] = p1.value[2] / p2;
	return temp;
};

pixel operator*(pixel p1, double p2)
{
	pixel temp;
	temp.x = p1.x;
	temp.y = p1.y;
	temp.value[0] = p1.value[0] * p2;
	temp.value[1] = p1.value[1] * p2;
	temp.value[2] = p1.value[2] * p2;
	return temp;
};

pixel operator/(pixel p1, double p2)
{
	pixel temp;
	temp.x = p1.x;
	temp.y = p1.y;
	temp.value[0] = p1.value[0] / p2;
	temp.value[1] = p1.value[1] / p2;
	temp.value[2] = p1.value[2] / p2;
	return temp;
};


//创建label类，存放聚类的中心点信息(center)、标识(mark)，或者像元到中心像元的距离(dist),定义两个函数分别计算像元到像元中心的颜色距离的平方和空间距离的平方
class label
{
public:
	pixel center;
	int mark;
	double dist;
	label(int xx = 0, int yy = 0, double b1 = 0, double b2 = 0, double b3 = 0, int m = 0, double d = INFINITY)	//d默认值为无穷大
	{
		pixel temp(xx, yy, b1, b2, b3);
		mark = m;
		dist = d;
		center = temp;
	}
	double sdist2(pixel pixel_obj);
	double cdist2(pixel pixel_obj);

};

//计算空间距离
double label::sdist2(pixel pixel_obj)
{
	pixel d = pixel_obj - center;
	return (double)d.x * (double)d.x + (double)d.y * (double)d.y;
}

//计算颜色距离
double label::cdist2(pixel pixel_obj)
{
	pixel d = pixel_obj - center;
	return d.value[0] * d.value[0] + d.value[1] * d.value[1] + d.value[2] * d.value[2];
}

//声明函数
pixel sRGBtoLAB(pixel srgb);

label Gradient(label* image, int* region, int width, int height, label pex, int* up_x, int* up_y, int* count);

label Find_center(label* image, int* region, int width, int height);

//使函数可被其他代码调用
extern "C" _declspec(dllexport) void SLIC_main(double* image, int width, int height, int pxnum, double compactness, int* Labels, int* numLabel, int* len);

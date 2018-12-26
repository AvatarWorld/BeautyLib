#include"blobDetector.h"

//���ؼ���������Ӿ���
//poi��ʾ����uv�����㼯��
//contourIndex����������������������
//length ����������������������
//mask ���ؼ��������ɰ�
Rect BlobDetector::detectArea(vector<Point> poi, int * contourIndex, int length,Mat &mask)
{
	Point contour[13];
	for (int p_index = 0; p_index < length; p_index++)
	{
		contour[p_index]=poi[contourIndex[p_index]];
	}
	
	const Point* pts[] = {contour };//ppt����ΪPoint*��pts����ΪPoint**���趨���const����
		int npt[] = { length };  //npt�����ͼ�Ϊint*
		//�������
		
	fillPoly(mask, pts, npt, 1, Scalar(255));//��������ɰ�
	polylines(mask, pts, npt, 1, 1, Scalar(120), 1);//���Ƽ�����������
	int left = contour[0].x;
	int top = contour[0].y;
	int right = contour[0].x;
	int down = contour[0].y;

	for (int i = 1; i < length; i++)
	{
		int now_x = contour[i].x;
		int now_y = contour[i].y;

		left = left > now_x ? now_x : left;
		top = top > now_y ? now_y : top;
		right = right < now_x ? now_x : right;
		down = down < now_y ? now_y : down;

	}

	Rect roiRect = Rect(left, top, right - left, down - top);
	return roiRect;//���ؼ��������������
}
//�����ߵ������
void BlobDetector::drawBlob(Mat& src, vector<vector<Point>> contours)//���ưߵ������
{
	for (int i = 0; i < contours.size(); i++)
	{
		drawContours(src, contours, i, Scalar(255),1,8);
	}
}

vector<vector<Point>> BlobDetector::contourDetect(Mat & src)//���ߵ������
{
	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;
	findContours(src, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE, Point());	
	vector<vector<Point>> in_contours;

	return contours;
	
	
}
//src ԭ��������ɫͼ
//result �ڰ׶�ֵͼ���׵Ĵ���ߵ�
void StainDetector::imageProcess(Mat & src, Mat& result)
{
	Mat image_gaussian;
	Mat image_equalize;
	Mat image_gray;

	cvtColor(src, image_gray, CV_BGR2GRAY);
	
	int blockSize = 49;
	int constValue =10;
	adaptiveThreshold(image_gray, result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
	return;
}
//removeStainȥ���ߵ㺯��
//src��ʾԭͼ
//poi��ʾԭͼ�����������㼯��
//image_roi skin2.jpg��Ƥ���ɰ�
void StainDetector::removeStain(Mat & src, vector<Point> poi,Mat image_roi)
{
	
	for (int c_index = 0; c_index < FACE_NUM; c_index++)//����ȥ���ߵ��������Ǽ��ߵ���Ǹ�����
	{
		Mat mask(src.size(), CV_8U, Scalar(0));
		Rect roiRect = this->detectArea(poi, this->faceContour[c_index], FACE_CONTOUR, mask);
		Mat roi(src, roiRect);
		Mat roi_mask(mask, roiRect);
		Mat result;
		this->imageProcess(roi, result);
		result.setTo(255, roi_mask != 0 & result != 0);
		result.setTo(0, roi_mask == 0 | result == 0);
		vector<vector<Point>> contours = this->contourDetect(result);
		contours = this->contourFilter(contours);//���ȼ�⵽�ߵ㣬���Ѱߵ����������װ��contours��
		int x0 = 4096, x1 = 0, y0 = 2048, y1 = 0;
		for (int i = 0; i < contours.size(); i++) {//��ÿһ��������İߵ�ȥ��
			x0 = 4096; x1 = 0; y0 = 2048; y1 = 0;
			for (int j = 0; j < contours[i].size(); j++) {
				Point p = contours[i][j];
				if (x0 > p.x)x0 = p.x;
				if (y0 > p.y)y0 = p.y;
				if (y1 < p.y)y1 = p.y;
				if (x1 < p.x)x1 = p.x;
			}


			Vec3b up, down; int flag = 1, u, d;

			for (int x = x0; x <= x1; x++) {//���ȶԳƣ��������Ƥ���۽���

				u = y0; d = y1;
				for (int y = 0; y <= 1 * (d - u) / 5; y++) {

					roi.at<Vec3b>(d - y, x) = roi.at<Vec3b>(d + y, x);



				}
				for (int y = 0; y <= 1 * (d - u) / 5; y++) {
					roi.at<Vec3b>(u + y, x) = roi.at<Vec3b>(u - y, x);
				}
				int u1 = u + (d - u) / 5;
				for (int y = 0; y <= 1 * (d - u) / 5; y++) {
					roi.at<Vec3b>(u1 + y, x) = roi.at<Vec3b>(u1 - y, x);
				}



				int s = (d - u) / 5;
				int l = (d - u) / 5 + (d - u) / 5;
				int m = (d - u) / 5 + (d - u) / 5 + 3;
				for (int y = 0; y <= (d - u) / 5 + (d - u) / 5 + 3; y++) {//�м�����������Թ���

					roi.at<Vec3b>(y + u + l, x)[0] = (1.0 - 1.0*y / m)*roi.at<Vec3b>(u + l - y, x)[0] + 1.0*y / m * roi.at<Vec3b>(d - s + m - y, x)[0];
					roi.at<Vec3b>(y + u + l, x)[1] = (1.0 - 1.0*y / m)*roi.at<Vec3b>(u + l - y, x)[1] + 1.0*y / m * roi.at<Vec3b>(d - s + m - y, x)[1];
					roi.at<Vec3b>(y + u + l, x)[2] = (1.0 - 1.0*y / m)*roi.at<Vec3b>(u + l - y, x)[2] + 1.0*y / m * roi.at<Vec3b>(d - s + m - y, x)[2];

				}			}
		}
		
		resize(image_roi, image_roi, result.size());//��Ƥ���ɰ�ĳ���Ҫ�ĳߴ�
		Point center((int)(result.cols / 2), (int)(result.rows / 2));
		seamlessClone(image_roi, roi, result, center, roi, NORMAL_CLONE);//Ƥ���ɰ��ԭ���ߵ�������в����ں�
		roi.setTo(Scalar(0, 255, 0), roi_mask == 120);//����ɫȦ�����ߵ������
		roi.copyTo(src(roiRect));//�޸ĺ�����򿽱���ԭͼ��

	}

	
}
//StainDetector::spotDetect���ɫ�ߵĺ���
//src��ʾԭͼ
//poi��ʾԭͼ�����������㼯��
//ratio �ߵ�����ռ����Ƥ���ı���
void StainDetector::spotDetect(Mat & src, vector<Point> poi, float& ratio)
{

	double spotArea = 0.0;
	double faceArea = 0.0;
	for (int c_index = 0; c_index < FACE_NUM; c_index++)
	{
		Mat mask(src.size(), CV_8U, Scalar(0));//��������ɰ�
		Rect roiRect = this->detectArea(poi, this->faceContour[c_index], FACE_CONTOUR,mask);//����������Ӿ���
		Mat roi(src, roiRect);//�ü�����
		Mat roi_mask(mask, roiRect);//�ü��ɰ�
		Mat result;	
		this->imageProcess(roi, result);//result�а�ɫ�ĵ��ǰߵ�
		
		result.setTo(255, roi_mask != 0&result!=0);//���ɰ潻������
		result.setTo(0, roi_mask == 0 | result == 0);
		
		vector<vector<Point>> contours = this->contourDetect(result);
		contours = this->contourFilter(contours);
		this->drawBlob(roi,contours);
		
     	spotArea += this->areaCalculate(contours);
		faceArea += roi.cols*roi.rows;
		roi.setTo(Scalar(0,255,0), roi_mask ==120);//���Ƽ������Ϊ��ɫ��
		roi.copyTo(src(roiRect));

	}

	ratio = spotArea / faceArea;//�ߵ�ռ���ľ��εı���
}
//�ۼӰߵ�����
double StainDetector::areaCalculate(vector<vector<Point>> contours)
{
	double totalArea = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		totalArea += area;
		
	}

	return totalArea;
}
//src ԭ�������Ĳ�ɫͼ
//result �ڰ׶�ֵͼ �׵Ĵ���ë��

void PoreDetector::imageProcess(Mat& src, Mat& result)
{
	Mat image_gary;
	cvtColor(src, image_gary, CV_BGR2GRAY);
	int blockSize = 5;
	int constValue = 5;
	adaptiveThreshold(image_gary, result, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
	Mat labels, stats, centroids;
	int nccomps=connectedComponentsWithStats(result, labels, stats, centroids);
	//ë�׼������ȥ���������20����ͨ��
	vector<int>colors(nccomps+1);
	for (int i = 1; i < nccomps; i++) {
		
		if (stats.at<int>(i, cv::CC_STAT_AREA) > 20)
			colors[i]=0;
		else colors[i] = 255;
	}
	
	for (int y = 0; y < result.rows; y++)
		for (int x = 0; x < result.cols; x++)
		{
			int label = labels.at<int>(y, x);
			CV_Assert(0 <= label && label <= nccomps);
			result.at<uchar>(y, x) = colors[label];
		}
	
	return;

}
//PoreDetector::spotDetect���ë�׵ĺ���
//src��ʾԭͼ
//poi��ʾԭͼ�����������㼯��
//avgDia ë�׵�����

void PoreDetector::spotDetect(Mat & src, vector<Point> poi, float& avgDia)
{
	int spotNum = 0;
	double totalDia = 0.0;

	Mat mask(src.size(), CV_8U, Scalar(0));
	Rect roiRect = this->detectArea(poi, this->noseContour, NOSE_CONTOUR,mask);
	Mat roi(src, roiRect);
	
	Mat roi_mask(mask, roiRect);
	Mat result;
	this->imageProcess(roi, result);
	roi.setTo(Scalar(255, 0, 0), result == 255 & roi_mask == 255);//ȡ������ë��������ɰ�Ľ���
	roi.setTo(Scalar(0, 255, 0), roi_mask == 120);//����ë�׼�����������
	for(int x=0;x<roi.cols;x++)
		for (int y = 0; y < roi.rows; y++) {
			if (roi.at<Vec3b>(y, x)[0] ==255&& roi.at<Vec3b>(y, x)[1] == 0&& roi.at<Vec3b>(y, x)[2] == 0)
				spotNum++;
		}
	avgDia = spotNum ;
	roi.copyTo(src(roiRect));
}

//�ߵ����ʱ����˵�С��ë��
vector<vector<Point>> StainDetector::contourFilter(vector<vector<Point>> contours, double filterArea)
{
	vector<vector<Point>> filterContours;

	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);

		if (area > filterArea)
		{
			filterContours.push_back(contours[i]);
		}
	}

	return filterContours;
}
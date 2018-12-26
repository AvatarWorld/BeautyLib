#include"blobDetector.h"

vector<Point> setPoint(string uvsPath)
{
	ifstream infile;
	infile.open(uvsPath);

	string s;
	getline(infile, s);

	vector<Point> poi;
	string coordinate;
	float x = 0;
	float y = 0;
	for (int i = 0;i < s.size();i++)
	{
		if (s[i] == '(')
		{
			coordinate.clear();
			continue;
		}
		if (s[i] == ',')
		{
			x = atof(coordinate.c_str());
			continue;
		}
		if (s[i] == ' ')
		{
			coordinate.clear();
			continue;
		}
		if (s[i] == ')')
		{
			y = atof(coordinate.c_str());

			Point point = Point((int)(x * 4096), (int)(y * 2048));
			poi.push_back(point);
			continue;
		}
		coordinate.push_back(s[i]);
	}

	return poi;


}


int main()
{
	string filename = "C:/Users/jin/Documents/wpic/head/";
	string filename2 = "C:/Users/jin/Documents/wpic/stain2/";
	for (int i = 0; i < 6; i++)
	{
		Mat sourceImage = imread(filename+char('1'+i)+".jpg");//��ȡԭ��ɫͼ

		StainDetector* stainDetector = new StainDetector();
		PoreDetector* poreDetector = new PoreDetector();

		float ratio = 0.0;//�������İߵ����ë�׵�����
		//stainDetetor�ǰߵ��⺯��
		stainDetector->spotDetect(sourceImage, setPoint(filename + char('1' + i) + ".txt"), ratio);
		//poreDetector��ë�׼�⺯��
	//	poreDetector->spotDetect(sourceImage, setPoint("C:/Users/jin/Documents/wpic/head/1.txt"), ratio);
		
		Mat image_roi = imread("C:/Users/jin/Pictures/eyebrow/skin2.jpg", 1);
		//removeStain��ȥ���ߵ㺯��
		//stainDetector->removeStain(sourceImage, setPoint(filename + char('1' + i) + ".txt"), image_roi);
		cout << ratio << endl;//ë����Ŀ�����ǰߵ�ռ��

		imwrite(filename2+char('1'+i)+".jpg", sourceImage);
	}
	system("pause");
}
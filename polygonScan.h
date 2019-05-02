#include<vector>
using namespace std;
/*
 点,边,多边形的数据结构头文件
*/
//点
class point {
public:
	int x;
	int y;
	point() {};
	point(int x, int y) {
		this->x = x;
		this->y = y;
	};
	
};
//边
class edge {
public:
	int ymax,ymin;
	float x;
	float dx;
};

class polygon {
public:
	vector<point> p;
};


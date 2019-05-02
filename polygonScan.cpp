#include<iostream>
#include<fstream>//读写文件
#include<string>
#include<vector>
#include<gl/glut.h>
#include"datastruct.h"

/*
利用OpenGL实现橡皮筋算法
*/

//窗口的宽度和高度
static const int screenWidth = 1000;
static const int screenHeight = 1000;

vector<point> p;//多边形顶点集向量
vector<polygon> s;//多边形类向量，用来保存已经画好了的多边形
int move_x, move_y;//鼠标当前的坐标值,在鼠标移动动态画线时使用
bool select = false;//多边形封闭状态判断变量,当按下鼠标右键 select=true

/**************填充算法***********/
void polygonScan(vector<point> vertices) {
	int y_max = 0, y_min = 0;//多边形的最高点和最低点
	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i].y > y_max)
			y_max = vertices[i].y;
		if (vertices[i].y < y_min)
			y_min = vertices[i].y;
	}
	//初始化边表和活性表
	vector<edge> ET, AET;
	edge E;
	int k = vertices.size();
	for (int i = 0; i < vertices.size(); i++) {
		E.x = (vertices[i].y > vertices[(i + 1) % k].y) ? vertices[(i + 1) % k].x : vertices[i].x;//ET的x表示下端点的x
		E.ymax = (vertices[i].y > vertices[(i + 1) % k].y) ? vertices[i].y : vertices[(i + 1) % k].y;
		E.ymin = (vertices[i].y > vertices[(i + 1) % k].y) ? vertices[(i + 1) % k].y : vertices[i].y;
		E.dx = (vertices[i].x - vertices[(i + 1) % k].x) * 1.0 / (vertices[i].y - vertices[(i + 1) % k].y);
		ET.push_back(E);//插入到ET中
	}
	// 开始扫描阶段
	k = ET.size();
	for (int y = y_min; y <= y_max; y++) {
		for (int i = 0; i < k; i++) {
			if (y > ET[i].ymin && y <= ET[i].ymax) {//在最高点和最低点间 取消等号,即不考虑线段的端点
				AET.push_back(ET[i]);
				ET[i].x = ET[i].x + ET[i].dx;//更新每条边的最低点
			}
		}
		//活性表交点排序
		int len = AET.size();
		for (int i = 0; i < len; i++)
		{
			for (int j = 0; j < len - i - 1; j++)
			{
				if (AET[j].x > AET[j + 1].x)
				{
					float temp = AET[j].x;
					AET[j].x = AET[j + 1].x;
					AET[j + 1].x = temp;
				}
			}
		}
		//排序结束

		//交点配对,画线
		if (AET.size() % 2 == 0) {//避免奇数个交点,应该不会出现
			for (int i = 1; i < AET.size(); ) {
				glColor3f(1.0f, 0.0f, 0.0f);
				glBegin(GL_LINES);
				glVertex2i(AET[i - 1].x, y);
				glVertex2i(AET[i].x, y);
				glEnd();
				i = i + 2;
			}
			//glFlush();
		}
		AET.clear();
	}
	glFlush();//每次填充一个多边形就刷新

}
//填充算法结束


void init() {
	glClearColor(1.0, 1.0, 1.0, 0);//设置背景底色RGBA(A透明度) 取值范围为0.0f-1.0f 常见的255,255,255白色,0,0,0黑色
	glMatrixMode(GL_PROJECTION);//GL_PROJECTION投影模式
	gluOrtho2D(0, screenWidth, 0, screenHeight);//用于截取世界坐标系相应区域 left,right,top,bottom

}

void lineSegment() {
	glClear(GL_COLOR_BUFFER_BIT);//用来清除屏幕颜色，即将屏幕的所有像素点都还原为 “底色 ”
	glColor3f(1.0, 0.0, 0.0);//设置颜色,线段颜色和填充颜色,float类型
	int i, j;
	if (!s.empty()) {//当存在已经画好的多边形时,把它重新画出来
		for (i = 0; i < s.size(); i++) {//对每一个多边形进行处理
			int k = s[i].p.size();
			for (j = 0; j < k; j++) {//多边形每条边绘制
				glBegin(GL_LINES);//前后两个点连接起来
				glVertex2i(s[i].p[j].x, s[i].p[j].y);
				glVertex2i(s[i].p[(j + 1) % k].x, s[i].p[(j + 1) % k].y);
				glEnd();
			}
			//注意:在此处给多边形填充颜色
			polygonScan(s[i].p);
		}
	}

	//给正在画的图形画边
	i = 0;
	j = p.size() - 1;
	for (i = 0; i < j; i++) {
		glBegin(GL_LINES);
		glVertex2i(p[i].x, p[i].y);
		glVertex2i(p[i + 1].x, p[i + 1].y);
		glEnd();
		//cout << p.size() - 1;
	}


	//画鼠标动态移动的连线
	j = p.size() - 1;//最后一个点
	if (!p.empty()) {
		glBegin(GL_LINES);
		glVertex2i(p[j].x, p[j].y);
		glVertex2i(move_x, move_y);
		glEnd();
		//glFlush();
	}


	//将图形封闭
	if (select == true) {//右键被按下时
		select = false;//重置为false
		if (!p.empty()) {
			glBegin(GL_LINES);
			glVertex2i(p[j].x, p[j].y);
			glVertex2i(p[0].x, p[0].y);
			glEnd();
			//glFlush();
			//将封闭后的多边形保持到多边形类
			polygon sq;
			for (i = 0; i < p.size(); i++) {
				sq.p.push_back(p[i]);
			}
			s.push_back(sq);
			//填充颜色
			polygonScan(sq.p);
		}
		p.clear();
	}
	glFlush();

}

void myMouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {//鼠标左键按下
		point v;//存鼠标左键的点
		v.x = x;
		v.y = screenHeight - y;
		p.push_back(v);
		glutPostRedisplay();//重绘窗口
	}
	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {//鼠标右键按下
		select = true;
		glutPostRedisplay();//重绘窗口;
	}

}

void myPassiveMotion(int x, int y) {//鼠标移动事件响应函数
	move_x = x;
	move_y = screenHeight - y;
	glutPostRedisplay();
}

/*将已经画好了的多边形保存到文件中*/
void printPolygon() {
	ofstream out;
	out.open("data.txt", ios::trunc);
	int polygonCount = s.size();
	for (int i = 0; i < polygonCount; i++) {
		int pointCount = s[i].p.size();
		for (int j = 0; j < pointCount; j++) {
			cout << s[i].p[j].x << " " << s[i].p[j].y << "\t";
			out << s[i].p[j].x << " " << s[i].p[j].y << " ";
		}
		out << endl;
		cout << endl;
	}
	out.close();

}
/*split函数*/
vector<string> split(const string & str, const string & delim) {
	vector<string> res;
	if ("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型  
	char* strs = new char[str.length() + 1]; //不要忘了  
	strcpy(strs, str.c_str());

	char* d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char* p = strtok(strs, d);
	while (p) {
		string s = p; //分割得到的字符串转换为string类型  
		res.push_back(s); //存入结果数组  
		p = strtok(NULL, d);
	}

	return res;
}

/*载入上次画好的多边形*/
void loadPolygon() {
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	s.clear();//清空已有图形
	p.clear();

	ifstream in;
	in.open("data.txt", ios::in);
	string line;
	while (getline(in, line))
	{
		vector<string> points = split(line, " ");
		point pointTemp;
		polygon polygonTemp;
		for (int i = 1; i < points.size(); i += 2) {
			cout << points[i - 1] << " " << points[i] << endl;
			pointTemp.x = atoi(points[i - 1].c_str());
			pointTemp.y = atoi(points[i].c_str());
			polygonTemp.p.push_back(pointTemp);
		}
		s.push_back(polygonTemp);
		polygonTemp.p.clear();
	}
	in.close();

	//重新填充上次图形
	for (int i = 0; i < s.size(); i++)
		polygonScan(s[i].p);



}



void KeyBoards(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's':
		cout << "save" << endl;
		printPolygon();
		break;
	case'c':
		cout << "clear" << endl;
		glClear(GL_COLOR_BUFFER_BIT);
		glFlush();
		s.clear();
		p.clear();
		break;
	case 'l':
		loadPolygon();
		break;
	}

}







int main(int argc, char** agrv) {

	glutInit(&argc, agrv);//初始化glut
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);//设置为单缓冲区和RGB模式
	glutInitWindowPosition(100, 50);//窗口显示位置
	glutInitWindowSize(screenWidth, screenHeight);//窗口大小
	glutCreateWindow("rubber_v1");

	/*设置opengl视图窗口*/
	init();

	glutMouseFunc(myMouse);//鼠标点击监控,若被点击则调用myMouse
	glutDisplayFunc(lineSegment);
	glutPassiveMotionFunc(myPassiveMotion);
	glutKeyboardFunc(KeyBoards);  //注册键盘事件
	glutMainLoop();//程序循环进行
	//glutDisplayFunc(&myDisplay);



	return 0;

}


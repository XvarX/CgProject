#include <cv.h>
#include <highgui.h>
#include <list>
#include <math.h>
#include <gl/glut.h>
const int n = 20;
const GLfloat R = 0.5f;
const GLfloat Pi = 3.1415926536f;
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat zRot = 0.0f;
static float tempAngle;
static bool beginDraw = false;
static int clicktimes = 2;

#define pi 3.14159
using namespace std;
IplImage *qImage;
IplImage *pImage;
struct pixel {
	double R;
	double G;
	double B;
};
struct drawPoint {
	float x;
	float y;
};
struct Point {
	int x;
	int y;
	Point() {

	}
	Point(int x_,int y_) {
		x = x_;
		y = y_;
	}
};
struct modelPoint {
	double x;
	double y;
	double z;
};

struct Circle {
	vector<modelPoint> pointList;
};
struct Obj {

};
vector<Point> dragPoints;
vector<Point> testPoints;
vector<Point> linePoints;
vector<Circle> model;
pixel image[1000][1000];
Point clickbegin;
Point clickend;
void getCircleCoorByCenter(Point center, float angle) {
	Point tempLeftPoint = center;
	Point LeftPointInLine;
	LeftPointInLine = tempLeftPoint;
	if (abs(tan(angle)) <= 9) {
		while (tempLeftPoint.x >= 0) {
			LeftPointInLine = tempLeftPoint;
			LeftPointInLine.y = tempLeftPoint.y - abs(tempLeftPoint.x - center.x)*tan(angle);
			int a = 0;
			if (LeftPointInLine.y < 0 || LeftPointInLine.x < 0) {
				return;
			}
			if (image[LeftPointInLine.y][LeftPointInLine.x].B == 0) {
				break;
			}
			else {
				tempLeftPoint.x--;
			}
		}
		if (tempLeftPoint.x < 0) {
			return;
		}
	}
	else {
		while (tempLeftPoint.y >= 0) {
			LeftPointInLine = tempLeftPoint;
			int a = 0;
			if (image[LeftPointInLine.y][LeftPointInLine.x].B == 0) {
				break;
			}
			else {
				tempLeftPoint.y--;
			}
		}
		if (tempLeftPoint.y < 0) {
			return;
		}
	}
	Point tempRightPoint = center;
	Point RightPointInLine = tempRightPoint;
	if (abs(tan(angle)) <= 9) {
		while (tempRightPoint.x >= 0) {
			RightPointInLine = tempRightPoint;
			RightPointInLine.y = tempRightPoint.y + abs(tempRightPoint.x - center.x)*tan(angle);
			if (RightPointInLine.y < 0 || RightPointInLine.x < 0) {
				return;
			}
			if (image[RightPointInLine.y][RightPointInLine.x].B == 0) {
				break;
			}
			else {
				tempRightPoint.x++;
			}
		}
	}
	else {
		while (tempRightPoint.y >= 0) {
			RightPointInLine = tempRightPoint;
			int a = 0;
			if (image[RightPointInLine.y][RightPointInLine.x].B == 0) {
				break;
			}
			else {
				tempRightPoint.y++;
			}
		}
		if (tempLeftPoint.y > pImage->height) {
			return;
		}
	}
	int redis = sqrt(
		        (RightPointInLine.x - LeftPointInLine.x)*(RightPointInLine.x - LeftPointInLine.x) + 
		        (RightPointInLine.y - LeftPointInLine.y)*(RightPointInLine.y - LeftPointInLine.y))/2;
	//redis = (tempRightPoint.x - tempLeftPoint.x)/2;
	int yuanxin = (tempRightPoint.x + tempLeftPoint.x) / 2;
	int pianyi = yuanxin - pImage->width/2;
	Circle newCircle;
	for (double theta = 0; theta < 2 * pi;) {
		modelPoint newPoint;
		theta += 0.1;
		newPoint.x = pianyi+redis*cos(theta);
		newPoint.y = redis*sin(theta);
		newPoint.z = center.y;

		//test
		auto alpha = angle;
		auto x = newPoint.x;
		auto z = newPoint.z;
		newPoint.x = (x-pianyi)*cos(alpha) - (z-center.y)*sin(alpha)+pianyi;
		newPoint.z = (x-pianyi)*sin(alpha) + (z-center.y)*cos(alpha)+center.y;
		newCircle.pointList.push_back(newPoint);
	}
	model.push_back(newCircle);
}
void DrawPoint(const drawPoint& p) {
	glBegin(GL_POINTS);
	glVertex2f(p.x, p.y);
	glEnd();
}
void getPixelPointByCor(drawPoint drawpoint) {
	Point point;
	
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void loadimage() {
	const char *pstrImageName = "finaltest.bmp";
	const char *pstrWindowsTitle = "CgProject";
	pImage = cvLoadImage(pstrImageName); 
	cvFlip(pImage, pImage,0);
	int indexBeginI = 106;
	int indexEndI = 245;
	CvScalar cs;                               //声明像素变量  
	for (int i = 0; i < pImage->height; i++) {
		for (int j = 0; j < pImage->width; j++)
		{
			cs = cvGet2D(pImage, i, j);   //获取像素  
			image[i][j].B = cs.val[0];             //对指定像素的RGB值进行重新设定  
			image[i][j].G = cs.val[1];
			image[i][j].R = cs.val[2];
			if (cs.val[0] == 0) {
				int a = i;
			}
		}
	}
}

void drawModel() {
	glPointSize(0.5);
	glColor3f(1.0, 0, 0);
	glBegin(GL_POINTS);
	for (int i = 2; i < model.size(); i++) {
		for (int j = 0; j < model[i].pointList.size(); j++) {
			glVertex3f(model[i].pointList[j].x / (pImage->width/2),
				model[i].pointList[j].z / (pImage->height / 2)-1,
				model[i].pointList[j].y / (pImage->width / 2)
				);
		}
	}
	glEnd();
}

void drawModelTraingle() {

}
void drawLine() {
	Point begin = dragPoints.front();
	Point end = dragPoints.back();
	float tempBeginX = (float)( begin.x- pImage->width / 2) / (pImage->width / 2);
	float tempBeginy = (float)(begin.y - pImage->height / 2) / (pImage->height / 2);
	float tempEndX = (float)(end.x - pImage->width / 2) / (pImage->width / 2);
	float tempEndy = (float)(end.y - pImage->height / 2) / (pImage->height / 2);
	drawPoint tempBegin;
	tempBegin.x = tempBeginX;
	tempBegin.y = tempBeginy;
	drawPoint tempEnd;
	tempEnd.x = tempEndX;
	tempEnd.y = tempEndy;
	glBegin(GL_LINES);
	glVertex2f(tempBegin.x, tempBegin.y);
	glVertex2f(tempEnd.x, tempEnd.y);
	glEnd();
}
float getSlopeByTwoPoint(Point A, Point B) {
	float slope;
	if (A.x == B.x) {
		return pi;
	}
	if (A.y == B.y) {
		return (0.5)*pi;
	}
	slope = (float)(A.x - B.x) / (float)(A.y - B.y);
	auto angle = atan(slope);
	angle = Pi - angle;
	return angle;
}
void LoadTestPoint() {
	testPoints.clear();
	struct Point tempVector[22];
	tempVector[0] = { 78,260 };
	tempVector[1] = { 78,250 };
	tempVector[2] = { 78,240 };
	tempVector[3] = { 78,230 };
	tempVector[4] = { 79,220 };
	tempVector[5] = { 79,210 };
	tempVector[6] = { 81,200 };
	tempVector[7] = { 82,190 };
	tempVector[8] = { 83,180 };
	tempVector[9] = { 85,170 };
	tempVector[10] = { 87,160 };
	tempVector[11] = { 90,150 };
	tempVector[12] = { 93,140 };
	tempVector[13] = { 99,130 };
	tempVector[14] = { 104,120 };
	tempVector[15] = { 110,110 };
	tempVector[16] = { 120,100 };
	tempVector[17] = { 134,90 };
	tempVector[18] = { 160,80 };
	tempVector[19] = { 209,70 };
	tempVector[20] = { 219,70 };
	tempVector[21] = { 229,70 };

	for (int i = 0; i < 22; i++) {
		Point newPoint = tempVector[i];
		newPoint.y = pImage->height - newPoint.y;
		testPoints.push_back(newPoint);
	}
	Point temp;
	if (!dragPoints.empty()) {
		temp = dragPoints[0];
	}
	float angle;
	for (int i = 0; i < dragPoints.size();) {
		angle = getSlopeByTwoPoint(temp, dragPoints[i]);
		getCircleCoorByCenter(temp, angle);
		temp = dragPoints[i];
		i += 1;
	}
}

void display(void)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();  //加载单位矩阵  
	//gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	static GLubyte* PixelData;
	PixelData = (GLubyte*)(pImage->imageData);
	glDrawPixels(pImage->width, pImage->height, GL_BGR_EXT, GL_UNSIGNED_BYTE, PixelData);
	
	if (beginDraw == false) {
		LoadTestPoint();
		dragPoints.clear();
		glPushMatrix();
		glRotatef(xRot, 1.0f, 0.0f, 0.0f);
		glRotatef(yRot, 0.0f, 1.0f, 0.0f);
		glRotatef(zRot, 0.0f, 0.0f, 1.0f);

		drawModel();
		glPopMatrix();// this will refresh the window, so, it works the same to call RenderScene() directly 
	}
	if (beginDraw == true) {
		drawLine();
	}
	glFlush();
}

void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		xRot -= 1.0f;

	if (key == GLUT_KEY_DOWN)
		xRot += 1.0f;

	if (key == GLUT_KEY_LEFT)
		yRot -= 1.0f;

	if (key == GLUT_KEY_RIGHT)
		yRot += 1.0f;

	if (xRot > 356.0f)
		xRot = 0.0f;

	if (xRot < -1.0f)
		xRot = 355.0f;

	if (yRot > 356.0f)
		yRot = 0.0f;

	if (yRot < -1.0f)
		yRot = 355.0f;

	if (zRot > 356.0f)
		zRot = 0.0f;

	if (zRot < -1.0f)
		zRot = 355.0f;

	// Refresh the Window

	glutPostRedisplay();
}

void init() {
	float AmbientLight[4] = { 1,1,1,1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glEnable(GL_LIGHT0);       //开启GL_LIGHT0光源
	glEnable(GL_LIGHTING);     //开启光照系统

	//glEnable(GL_DEPTH_TEST);
}



void getCirclePointByPoint() {
	if (dragPoints.empty()) {
		return;
	}
	Point temp = dragPoints[0];
	tempAngle = getSlopeByTwoPoint(dragPoints.front(), dragPoints.back());
	int num = abs(dragPoints.front().x - dragPoints.back().x) + abs(dragPoints.front().y- dragPoints.back().y);
	linePoints.push_back(dragPoints.front());
	auto distancex = dragPoints.back().x - dragPoints.front().x;
	auto distancey = dragPoints.back().y - dragPoints.front().y;
	for (int i = 0; i < num; i++) {
		Point newPoint;
		newPoint.x = dragPoints.front().x + distancex*i / num;
		newPoint.y = dragPoints.front().y + distancey*i / num;
		linePoints.push_back(newPoint);
	}
	linePoints.push_back(dragPoints.back());
	for (int i = 1; i < linePoints.size();) {
		getCircleCoorByCenter(temp, tempAngle);
		temp = linePoints[i];
		i += 1;
	}
}

void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		beginDraw = true;

		/*if (clicktimes == 0) {
			clickbegin.x = x;
			clickbegin.y = y;
			clicktimes++;
		}
		else if (clicktimes == 1) {
			clickend.x = x;
			clickend.y = y;
			glColor3f(255, 0, 0);
			float tempx = (float)(x - pImage->width / 2) / (pImage->width / 2);
			float tempy = -(float)(y - pImage->height / 2) / (pImage->height / 2);
			drawPoint temp;
			temp.x = tempx;
			temp.y = tempy;
			DrawPoint(temp);
			Point center;
			center.x = x;
			for (int height = pImage->height - y; height < pImage->height-clickbegin.y; height++) {
				center.y = height;
				center.x = center.x + 1;
				getCircleCoorByCenter(center,1.75*Pi);
			}
			clicktimes = 0;
			glutPostRedisplay();
		}*/
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		beginDraw = false;
		//getCirclePointByPoint();
		glutPostRedisplay();
		//dragPoints.clear();
	}
	return;
}

void mouseMotion(int x, int y) {
	Point newPoint;
	newPoint.x = x;
	newPoint.y = pImage->height - y;
	if (!dragPoints.empty()) {
		Point backPoint = dragPoints.back();
		if (abs(backPoint.x - newPoint.x) + abs(backPoint.y - newPoint.y) > 10) {
			dragPoints.push_back(newPoint);
		}
	}
	else {
		dragPoints.push_back(newPoint);
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);     // GLUT 环境初始化
	loadimage();
	glutInitWindowSize(pImage->width, pImage->height);       // 定义窗口大小  
	glutInitWindowPosition(100, 100);   // 定义窗口位置    
	glutCreateWindow(argv[0]);   // 显示窗口,窗口标题为执行函数名  
	//init();
	glutDisplayFunc(display);    // 注册 OpenGL 绘图函数(一种特殊的调用方式,下同)   
	//glutReshapeFunc(reshape);   // 注册窗口大小改变时的响应函数
	glutMouseFunc(&myMouse);
	glutMotionFunc(&mouseMotion);
	glutSpecialFunc(SpecialKeys);
	glutMainLoop();      // 进入 GLUT 消息循环，开始执行程序  
	return 0;
}
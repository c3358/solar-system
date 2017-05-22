
#define GLUT_DISABLE_ATEXIT_HACK    // 若无此句,将会出错

#include <windows.h>
#include "gl/glut.h"
#include "texture.h"

#define MAXTEXTURE 8    // 定义最大的纹理数目

// 定义窗口的标题、宽度、高度
#define WIN_TITLE "solar system"
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 500;
BOOL isFullScreen = FALSE;

// 用户变量定义

GLfloat ep_Angle;              // 地球公转的角度
GLfloat es_Angle;              // 地球自转的角度
GLfloat mp_Angle;              // 月亮公转的角度
GLfloat ms_Angle;              // 月亮自转的角度
GLuint texture[MAXTEXTURE];    // 纹理数组，保存纹理名字
GLUquadricObj *quadric;        // 建立二次曲面对象
GLfloat angle_Z;               // 星空旋转角度

GLfloat LightAmbient[4] = { 1.0f, 0.0f, 0.0f, 0.0f };      // 定义环境光的颜色
GLfloat LightDiffuse[4] = { 0.0f, 1.0f, 1.0f, 0.0f };      // 定义漫反射光的颜色
GLfloat LightPosition[4] = { 0.0f, 0.0f, 0.0f, 1.0f };     // 定义光源的位置



// OpenGL初始化
void InitGL(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(2.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// 载入纹理
	BuildTexture("Data/1.bmp", texture[0]);
	BuildTexture("Data/2.bmp", texture[1]);
	BuildTexture("Data/3.bmp", texture[2]);
	BuildTexture("Data/4.bmp", texture[3]);
	BuildTexture("Data/5.bmp", texture[4]);
	BuildTexture("Data/6.bmp", texture[5]);
	BuildTexture("Data/7.bmp", texture[6]);
	BuildTexture("Data/8.bmp", texture[7]);

	glEnable(GL_TEXTURE_2D);                             // 开启2D纹理映射

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);      // 设置环境光
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);      // 设置漫反射光

	quadric = gluNewQuadric();                           // 建立一个曲面对象指针
	gluQuadricTexture(quadric, GLU_TRUE);                // 建立纹理坐标
	gluQuadricDrawStyle(quadric, GLU_FILL);              // 用面填充
	glEnable(GL_LIGHTING);                               // 打开光照
	glEnable(GL_LIGHT1);                                 // 打开光源1

}

// 场景绘制函数
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(10, 1.0f ,0.0f, 0.0f);
	glEnable(GL_LIGHT0);  // 打开光源0

	/**********************************绘制背景星空*********************************************/
	glPushMatrix ();
	glTranslatef(-10.0f, 3.0f, 0.0f);
	glRotatef (angle_Z, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);       // 绑定星空纹理
	glBegin(GL_QUADS); 
	glNormal3f( 0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, -20.0f, -5.0f);
	glTexCoord2f(6.0f, 0.0f); glVertex3f( 20.0f, -20.0f, -5.0f);
	glTexCoord2f(6.0f, 6.0f); glVertex3f( 20.0f,  20.0f, -5.0f);
	glTexCoord2f(0.0f, 6.0f); glVertex3f(-20.0f,  20.0f, -5.0f);
	glEnd();
	glPopMatrix ();

	/**********************************绘制太阳*************************************************/
	glBindTexture(GL_TEXTURE_2D, texture[2]);       // 绑定纹理
	glEnable(GL_BLEND);             // 开启混合
	glDisable(GL_DEPTH_TEST);
	//绘制太阳光晕
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);               // 基于源象素alpha通道值的半透明混合函数
	glColor4f(1.0f, 1.0f, 1.0f, 0.4f);              // 设置RGBA值
	glBegin(GL_QUADS);
	glNormal3f( 0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();
	glDisable(GL_BLEND);            // 关闭混合
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);            // 开启光照
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);// 设置光源1的当前位置

	gluSphere(quadric, 0.3f, 32, 32);                // 绘制太阳球体
	// 当前模型视图矩阵出栈
	/**********************************绘制水星*************************************************/
	glDisable(GL_LIGHT0);
	glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);           // 将坐标系绕Y轴旋转ep_Angle角度  控制水星公转
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);             // 将坐标系绕X轴旋转-90度
	glEnable(GL_TEXTURE_2D );

	glTranslatef(0.5f, 0.0f, 0.0f);                  // 将坐标系右移0.5f
	glBindTexture(GL_TEXTURE_2D, texture[4]);        // 绑定纹理

	glPushMatrix ();
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);           // 将坐标系绕Z轴旋转es_Angle角度  控制水星自转
	gluSphere(quadric, 0.04f, 32, 32);               // 水星球体
	glPopMatrix ();
	/**********************************绘制金星*************************************************/
	glDisable(GL_LIGHT0);
	// glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);        // 将坐标系绕Y轴旋转ep_Angle角度  控制金星公转
	// glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);          // 将坐标系绕X轴旋转-90度

	glEnable(GL_TEXTURE_2D );

	glTranslatef(0.2f, 0.0f, 0.0f);                  // 将坐标系右移0.2f
	glBindTexture(GL_TEXTURE_2D, texture[5]);        // 绑定纹理

	glPushMatrix ();
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);           // 将坐标系绕Z轴旋转es_Angle角度  控制金星自转
	gluSphere(quadric, 0.07f, 32, 32);               // 金星球体
	glPopMatrix ();  

	/**********************************绘制火星*************************************************/
	glDisable(GL_LIGHT0);
	// glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);        // 将坐标系绕Y轴旋转ep_Angle角度  控制火星公转
	// glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);          // 将坐标系绕X轴旋转-90度

	glEnable(GL_TEXTURE_2D );

	glTranslatef(1.0f, 0.0f, 0.0f);                  // 将坐标系右移1.0f
	glBindTexture(GL_TEXTURE_2D, texture[6]);        // 绑定纹理

	glPushMatrix ();
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);           // 将坐标系绕Z轴旋转es_Angle角度  控制火星自转
	gluSphere(quadric, 0.04f, 32, 32);               // 火星球体
	glPopMatrix ();  
	/**********************************绘制木星*************************************************/
	glDisable(GL_LIGHT0);
	// glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);        // 将坐标系绕Y轴旋转ep_Angle角度  控制木星公转
	// glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);          // 将坐标系绕X轴旋转-90度

	glEnable(GL_TEXTURE_2D );

	glTranslatef(2.0f, 0.0f, 0.0f);                  // 将坐标系右移2.0f
	glBindTexture(GL_TEXTURE_2D, texture[7]);        // 绑定纹理

	glPushMatrix ();
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);           // 将坐标系绕Z轴旋转es_Angle角度  控制木星自转
	gluSphere(quadric, 1.0f, 32, 32);                // 木星球体
	glPopMatrix ();  
	/**********************************绘制地球*************************************************/
	glDisable(GL_LIGHT0);
	// glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);        // 将坐标系绕Y轴旋转ep_Angle角度  控制地球公转
	// glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);          // 将坐标系绕X轴旋转-90度

	glEnable(GL_TEXTURE_2D );

	glTranslatef(-2.5f, 0.0f, 0.0f);                 // 将坐标系左移2.5f
	glBindTexture(GL_TEXTURE_2D, texture[0]);        // 绑定纹理

	glPushMatrix ();
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);           // 将坐标系绕Z轴旋转es_Angle角度  控制地球自转
	glutSolidTeapot(0.1);
	//glutSolidCube(1.0);
	//gluSphere(quadric, 0.1f, 32, 32);              // 地球球体
	glPopMatrix ();

	/**********************************绘制月亮*************************************************/
	glRotatef(mp_Angle, 0.0f, 0.0f, 1.0f);           // 将坐标系绕Z轴旋转mp_Angle角度 控制月亮公转
	glBindTexture(GL_TEXTURE_2D, texture[3]);        // 绑定纹理
	glTranslatef(0.2f, 0.0f, 0.0f);                  // 左移0.2f
	glRotatef(ms_Angle, 0.0f, 0.0f, 1.0f);           // 将坐标系绕Z轴旋转ms_Angle角度 控制月亮自转
	gluSphere(quadric, 0.02, 32, 32);                // 绘制月亮星体

	// 变量更新
	ep_Angle += 0.5f;
	mp_Angle += 1.5f;
	es_Angle += 1.5f;
	ms_Angle += 1.5f;

	angle_Z += 0.01f;

	glutSwapBuffers();  // 交换双缓存
}

// 设置窗口改变大小时的处理函数
void Reshape(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	// 设置视口
	glViewport(0,0,width,height);

	// 设置投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// 键盘按键动作的处理函数（有对应ASCII码的按键）
void Keyboard (unsigned char key, int x, int y)
{
	switch(key) {
	case 27:    // 按ESC键时退出程序
		exit (0);
		break;
	default:
		break;
	}
}

// 键盘按键动作的处理函数（无对应ASCII码的按键）
void arrow_keys(int a_keys, int x, int y)
{
	switch(a_keys) {
	case GLUT_KEY_F1:  // 按F1键时切换窗口/全屏模式
		if(isFullScreen)
		{
			glutReshapeWindow(WIN_WIDTH, WIN_HEIGHT);
			glutPositionWindow(30, 30);
			isFullScreen = FALSE;
		}
		else
		{
			glutFullScreen();
			isFullScreen = TRUE;
		}
		break;
	default:
		break;
	}
}

// 程序主函数
void main(int argc, char** argv)
{
	glutInit(&argc, argv);                                     // 初始化GLUT库

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // 初始化显示模式

	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);                 // 初始化窗口大小

	glutInitWindowPosition(30,30);                             // 初始化窗口位置
	glutCreateWindow(WIN_TITLE);                               // 建立窗口

	InitGL();                   // 初始化OpenGL

	glutDisplayFunc(Display);   // 设置窗口显示时调用的处理函数

	glutReshapeFunc(Reshape);   // 设置窗口改变大小时的处理函数

	glutKeyboardFunc(Keyboard); // 设置键盘按键动作的处理函数（有对应ASCII码的按键）
	glutSpecialFunc(arrow_keys);// 设置键盘按键动作的处理函数（无对应ASCII码的按键）

	glutIdleFunc(Display);      // 设置窗口空闲时的处理函数

	glutMainLoop();             // 进入GLUT事件处理循环
}
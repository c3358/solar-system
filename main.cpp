
#define GLUT_DISABLE_ATEXIT_HACK    // ���޴˾�,�������

#include <windows.h>
#include "gl/glut.h"
#include "texture.h"

#define MAXTEXTURE 8    // ��������������Ŀ

// ���崰�ڵı��⡢��ȡ��߶�
#define WIN_TITLE "solar system"
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 500;
BOOL isFullScreen = FALSE;

// �û���������

GLfloat ep_Angle;              // ����ת�ĽǶ�
GLfloat es_Angle;              // ������ת�ĽǶ�
GLfloat mp_Angle;              // ������ת�ĽǶ�
GLfloat ms_Angle;              // ������ת�ĽǶ�
GLuint texture[MAXTEXTURE];    // �������飬������������
GLUquadricObj *quadric;        // ���������������
GLfloat angle_Z;               // �ǿ���ת�Ƕ�

GLfloat LightAmbient[4] = { 1.0f, 0.0f, 0.0f, 0.0f };      // ���廷�������ɫ
GLfloat LightDiffuse[4] = { 0.0f, 1.0f, 1.0f, 0.0f };      // ��������������ɫ
GLfloat LightPosition[4] = { 0.0f, 0.0f, 0.0f, 1.0f };     // �����Դ��λ��



// OpenGL��ʼ��
void InitGL(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(2.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// ��������
	BuildTexture("Data/1.bmp", texture[0]);
	BuildTexture("Data/2.bmp", texture[1]);
	BuildTexture("Data/3.bmp", texture[2]);
	BuildTexture("Data/4.bmp", texture[3]);
	BuildTexture("Data/5.bmp", texture[4]);
	BuildTexture("Data/6.bmp", texture[5]);
	BuildTexture("Data/7.bmp", texture[6]);
	BuildTexture("Data/8.bmp", texture[7]);

	glEnable(GL_TEXTURE_2D);                             // ����2D����ӳ��

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);      // ���û�����
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);      // �����������

	quadric = gluNewQuadric();                           // ����һ���������ָ��
	gluQuadricTexture(quadric, GLU_TRUE);                // ������������
	gluQuadricDrawStyle(quadric, GLU_FILL);              // �������
	glEnable(GL_LIGHTING);                               // �򿪹���
	glEnable(GL_LIGHT1);                                 // �򿪹�Դ1

}

// �������ƺ���
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(10, 1.0f ,0.0f, 0.0f);
	glEnable(GL_LIGHT0);  // �򿪹�Դ0

	/**********************************���Ʊ����ǿ�*********************************************/
	glPushMatrix ();
	glTranslatef(-10.0f, 3.0f, 0.0f);
	glRotatef (angle_Z, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);       // ���ǿ�����
	glBegin(GL_QUADS); 
	glNormal3f( 0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, -20.0f, -5.0f);
	glTexCoord2f(6.0f, 0.0f); glVertex3f( 20.0f, -20.0f, -5.0f);
	glTexCoord2f(6.0f, 6.0f); glVertex3f( 20.0f,  20.0f, -5.0f);
	glTexCoord2f(0.0f, 6.0f); glVertex3f(-20.0f,  20.0f, -5.0f);
	glEnd();
	glPopMatrix ();

	/**********************************����̫��*************************************************/
	glBindTexture(GL_TEXTURE_2D, texture[2]);       // ������
	glEnable(GL_BLEND);             // �������
	glDisable(GL_DEPTH_TEST);
	//����̫������
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);               // ����Դ����alphaͨ��ֵ�İ�͸����Ϻ���
	glColor4f(1.0f, 1.0f, 1.0f, 0.4f);              // ����RGBAֵ
	glBegin(GL_QUADS);
	glNormal3f( 0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();
	glDisable(GL_BLEND);            // �رջ��
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);            // ��������
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);// ���ù�Դ1�ĵ�ǰλ��

	gluSphere(quadric, 0.3f, 32, 32);                // ����̫������
	// ��ǰģ����ͼ�����ջ
	/**********************************����ˮ��*************************************************/
	glDisable(GL_LIGHT0);
	glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);           // ������ϵ��Y����תep_Angle�Ƕ�  ����ˮ�ǹ�ת
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);             // ������ϵ��X����ת-90��
	glEnable(GL_TEXTURE_2D );

	glTranslatef(0.5f, 0.0f, 0.0f);                  // ������ϵ����0.5f
	glBindTexture(GL_TEXTURE_2D, texture[4]);        // ������

	glPushMatrix ();
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);           // ������ϵ��Z����תes_Angle�Ƕ�  ����ˮ����ת
	gluSphere(quadric, 0.04f, 32, 32);               // ˮ������
	glPopMatrix ();
	/**********************************���ƽ���*************************************************/
	glDisable(GL_LIGHT0);
	// glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);        // ������ϵ��Y����תep_Angle�Ƕ�  ���ƽ��ǹ�ת
	// glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);          // ������ϵ��X����ת-90��

	glEnable(GL_TEXTURE_2D );

	glTranslatef(0.2f, 0.0f, 0.0f);                  // ������ϵ����0.2f
	glBindTexture(GL_TEXTURE_2D, texture[5]);        // ������

	glPushMatrix ();
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);           // ������ϵ��Z����תes_Angle�Ƕ�  ���ƽ�����ת
	gluSphere(quadric, 0.07f, 32, 32);               // ��������
	glPopMatrix ();  

	/**********************************���ƻ���*************************************************/
	glDisable(GL_LIGHT0);
	// glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);        // ������ϵ��Y����תep_Angle�Ƕ�  ���ƻ��ǹ�ת
	// glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);          // ������ϵ��X����ת-90��

	glEnable(GL_TEXTURE_2D );

	glTranslatef(1.0f, 0.0f, 0.0f);                  // ������ϵ����1.0f
	glBindTexture(GL_TEXTURE_2D, texture[6]);        // ������

	glPushMatrix ();
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);           // ������ϵ��Z����תes_Angle�Ƕ�  ���ƻ�����ת
	gluSphere(quadric, 0.04f, 32, 32);               // ��������
	glPopMatrix ();  
	/**********************************����ľ��*************************************************/
	glDisable(GL_LIGHT0);
	// glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);        // ������ϵ��Y����תep_Angle�Ƕ�  ����ľ�ǹ�ת
	// glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);          // ������ϵ��X����ת-90��

	glEnable(GL_TEXTURE_2D );

	glTranslatef(2.0f, 0.0f, 0.0f);                  // ������ϵ����2.0f
	glBindTexture(GL_TEXTURE_2D, texture[7]);        // ������

	glPushMatrix ();
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);           // ������ϵ��Z����תes_Angle�Ƕ�  ����ľ����ת
	gluSphere(quadric, 1.0f, 32, 32);                // ľ������
	glPopMatrix ();  
	/**********************************���Ƶ���*************************************************/
	glDisable(GL_LIGHT0);
	// glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);        // ������ϵ��Y����תep_Angle�Ƕ�  ���Ƶ���ת
	// glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);          // ������ϵ��X����ת-90��

	glEnable(GL_TEXTURE_2D );

	glTranslatef(-2.5f, 0.0f, 0.0f);                 // ������ϵ����2.5f
	glBindTexture(GL_TEXTURE_2D, texture[0]);        // ������

	glPushMatrix ();
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);           // ������ϵ��Z����תes_Angle�Ƕ�  ���Ƶ�����ת
	glutSolidTeapot(0.1);
	//glutSolidCube(1.0);
	//gluSphere(quadric, 0.1f, 32, 32);              // ��������
	glPopMatrix ();

	/**********************************��������*************************************************/
	glRotatef(mp_Angle, 0.0f, 0.0f, 1.0f);           // ������ϵ��Z����תmp_Angle�Ƕ� ����������ת
	glBindTexture(GL_TEXTURE_2D, texture[3]);        // ������
	glTranslatef(0.2f, 0.0f, 0.0f);                  // ����0.2f
	glRotatef(ms_Angle, 0.0f, 0.0f, 1.0f);           // ������ϵ��Z����תms_Angle�Ƕ� ����������ת
	gluSphere(quadric, 0.02, 32, 32);                // ������������

	// ��������
	ep_Angle += 0.5f;
	mp_Angle += 1.5f;
	es_Angle += 1.5f;
	ms_Angle += 1.5f;

	angle_Z += 0.01f;

	glutSwapBuffers();  // ����˫����
}

// ���ô��ڸı��Сʱ�Ĵ�����
void Reshape(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	// �����ӿ�
	glViewport(0,0,width,height);

	// ����ͶӰ����
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// ���̰��������Ĵ��������ж�ӦASCII��İ�����
void Keyboard (unsigned char key, int x, int y)
{
	switch(key) {
	case 27:    // ��ESC��ʱ�˳�����
		exit (0);
		break;
	default:
		break;
	}
}

// ���̰��������Ĵ��������޶�ӦASCII��İ�����
void arrow_keys(int a_keys, int x, int y)
{
	switch(a_keys) {
	case GLUT_KEY_F1:  // ��F1��ʱ�л�����/ȫ��ģʽ
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

// ����������
void main(int argc, char** argv)
{
	glutInit(&argc, argv);                                     // ��ʼ��GLUT��

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // ��ʼ����ʾģʽ

	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);                 // ��ʼ�����ڴ�С

	glutInitWindowPosition(30,30);                             // ��ʼ������λ��
	glutCreateWindow(WIN_TITLE);                               // ��������

	InitGL();                   // ��ʼ��OpenGL

	glutDisplayFunc(Display);   // ���ô�����ʾʱ���õĴ�����

	glutReshapeFunc(Reshape);   // ���ô��ڸı��Сʱ�Ĵ�����

	glutKeyboardFunc(Keyboard); // ���ü��̰��������Ĵ��������ж�ӦASCII��İ�����
	glutSpecialFunc(arrow_keys);// ���ü��̰��������Ĵ��������޶�ӦASCII��İ�����

	glutIdleFunc(Display);      // ���ô��ڿ���ʱ�Ĵ�����

	glutMainLoop();             // ����GLUT�¼�����ѭ��
}
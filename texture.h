
#pragma once

//
// OpenGL������
//
class CTexture
{
private:
	GLubyte*     m_pImageData;             // ͼ������ (���32bit)
	GLuint             m_nBpp;             // ÿһ���ص�ͼ����ɫ���
	GLuint           m_nWidth;             // ͼ����
	GLuint          m_nHeight;             // ͼ��߶�
	GLuint           m_nTexID;             // ����ID

	//��Ԫ����
	friend BOOL BuildTexture(char *szPathName, GLuint &texid);
	friend BOOL BuildTexture(char *filename, CTexture *texture);
};

// ����BMP,JPG,GIF���ļ�
BOOL BuildTexture(char *szPathName, GLuint &texid);

// ����TGA�ļ�
BOOL BuildTexture(char *filename, CTexture *texture);
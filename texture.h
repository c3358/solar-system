
#pragma once

//
// OpenGL纹理类
//
class CTexture
{
private:
	GLubyte*     m_pImageData;             // 图像数据 (最高32bit)
	GLuint             m_nBpp;             // 每一象素的图像颜色深度
	GLuint           m_nWidth;             // 图像宽度
	GLuint          m_nHeight;             // 图像高度
	GLuint           m_nTexID;             // 纹理ID

	//友元函数
	friend BOOL BuildTexture(char *szPathName, GLuint &texid);
	friend BOOL BuildTexture(char *filename, CTexture *texture);
};

// 载入BMP,JPG,GIF等文件
BOOL BuildTexture(char *szPathName, GLuint &texid);

// 载入TGA文件
BOOL BuildTexture(char *filename, CTexture *texture);
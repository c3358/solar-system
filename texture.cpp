#include <stdio.h>
#include <olectl.h>              // OLE控制库头文件
#include <math.h>
#include <gl\gl.h>
#include "texture.h"

// 载入图片并转换为纹理
BOOL BuildTexture(char *szPathName, GLuint &texid)
{
	HDC   hdcTemp;                 // DC用来保存位图
	HBITMAP  hbmpTemp;             // 保存临时位图
	IPicture *pPicture;            // 定义IPicture Interface
	OLECHAR  wszPath[MAX_PATH+1];  // 图片的完全路径
	char  szPath[MAX_PATH+1];      // 图片的完全路径
	long  lWidth;                  // 单位宽度
	long  lHeight;                 // 单位高度
	long  lWidthPixels;            // 像素的宽度
	long  lHeightPixels;           // 像素的高度
	GLint  glMaxTexDim ;           // 保存纹理的最大尺寸

	if (strstr(szPathName, "http://"))                     // 如果路径包含 http:// 则...
	{
		strcpy(szPath, szPathName);                        // 把路径拷贝到 szPath
	}
	else                                                   // 否则从文件导入图片
	{
		GetCurrentDirectory(MAX_PATH, szPath);             // 取得当前路径
		strcat(szPath, "\\");                              // 添加字符"\"
		strcat(szPath, szPathName);                        // 添加图片的相对路径
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);  // 把ASCII码转化为Unicode标准码
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if(FAILED(hr))
	{
		MessageBox (HWND_DESKTOP, "图片导入失败!", "出错", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	hdcTemp = CreateCompatibleDC(GetDC(0));
	if(!hdcTemp)
	{
		pPicture->Release();
		MessageBox (HWND_DESKTOP, "图片导入失败!", "出错", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);  // 取得支持的纹理最大尺寸

	pPicture->get_Width(&lWidth);                      // 取得IPicture 宽度 (转换为Pixels格式)
	lWidthPixels = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);                    // 取得IPicture 高度 (转换为Pixels格式)
	lHeightPixels = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// 调整图片到最好的效果
	if (lWidthPixels <= glMaxTexDim)
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
	else
		lWidthPixels = glMaxTexDim;

	if (lHeightPixels <= glMaxTexDim)
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
	else
		lHeightPixels = glMaxTexDim;

	// 建立一个临时位图
	BITMAPINFO bi = {0};                                // 位图的类型
	DWORD  *pBits = 0;                                  // 指向位图Bits的指针

	bi.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);   // 设置结构大小
	bi.bmiHeader.biBitCount  = 32;                      // 32 位
	bi.bmiHeader.biWidth  = lWidthPixels;               // 宽度像素值
	bi.bmiHeader.biHeight  = lHeightPixels;             // 高度像素值
	bi.bmiHeader.biCompression = BI_RGB;                // RGB 格式
	bi.bmiHeader.biPlanes  = 1;                         // 一个位平面

	// 建立一个位图这样我们可以指定颜色和深度 并访问每位的值
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);

	if(!hbmpTemp)
	{
		DeleteDC(hdcTemp);
		pPicture->Release();
		MessageBox (HWND_DESKTOP, "图片导入失败!", "出错", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	SelectObject(hdcTemp, hbmpTemp);

	// 在位图上绘制IPicture
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// 将BGR转换为RGB　将ALPHA值设为255
	for(long i = 0; i < lWidthPixels * lHeightPixels; i++)    // 循环遍历所有的像素
	{
		BYTE* pPixel = (BYTE*)(&pBits[i]);                    // 获取当前像素
		BYTE  temp  = pPixel[0];                              // 临时存储第一个颜色像素(蓝色)
		pPixel[0]  = pPixel[2];                               // 将红色值存到第一位
		pPixel[2]  = temp;                                    // 将蓝色值存到第三位
		pPixel[3]  = 255;                                     // ALPHA值设为255
	}

	glGenTextures(1, &texid);  // 创建纹理

	// 使用来自位图数据生成的典型纹理
	glBindTexture(GL_TEXTURE_2D, texid);                               // 绑定纹理
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 线形滤波
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // 线形滤波

	// 生成纹理
	glTexImage2D(GL_TEXTURE_2D, 0, 3, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);

	DeleteObject(hbmpTemp);
	DeleteDC(hdcTemp);

	pPicture->Release();

	return TRUE;
}

// 载入一个.TGA 文件到内存
BOOL BuildTexture(char *filename, CTexture *texture)
{    
	GLubyte  TGAheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};    // 没有压缩的TGA Header
	GLubyte  TGAcompare[12];                               // 用来比较 TGA Header
	GLubyte  header[6];                                    // Header里，头六个有用字节
	GLuint  bytesPerPixel;                                 // 保存TGA文件里每个像素用到的字节数
	GLuint  imageSize;                                     // 用来保存随机产生的图像的大小
	GLuint  temp;                                          // 临时变量
	GLuint  type = GL_RGBA;                                // 将默认的GL模式设置为RBGA (32 BPP)

	FILE *file = fopen(filename, "rb");

	if (file == NULL)
	{
		MessageBox (HWND_DESKTOP, "图片导入失败!", "出错", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (fread(TGAcompare,1,sizeof(TGAcompare),file) != sizeof(TGAcompare)// 是否有十二个字节可读?
		|| memcmp(TGAheader,TGAcompare,sizeof(TGAheader)) != 0           // header和我们想要的是否相符?
		|| fread(header,1,sizeof(header),file) != sizeof(header))        // 如果是读下六个字节
	{
		fclose(file);
		MessageBox (HWND_DESKTOP, "图片导入失败!", "出错", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	texture->m_nWidth  = header[1] * 256 + header[0];      // 确定的TGA 宽度 (高字节*256+低字节)
	texture->m_nHeight = header[3] * 256 + header[2];      // 确定的TGA 高度 (高字节*256+低字节)

	if (texture->m_nWidth <= 0 || texture->m_nHeight <= 0 ||(header[4] != 24 && header[4] != 32))
	{
		fclose(file);
		MessageBox (HWND_DESKTOP, "图片导入失败!", "出错", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	texture->m_nBpp = header[4];                           // 获取TGA每个像素的位(24 or 32)
	bytesPerPixel = texture->m_nBpp / 8;                   // 除以8以取得 每个像素的字节
	imageSize  = texture->m_nWidth*texture->m_nHeight*bytesPerPixel;  // 计算TAG数据所需要的内存

	texture->m_pImageData = (GLubyte *)malloc(imageSize);  // 开辟一个内存空间用来存储TGA数据

	if (texture->m_pImageData == NULL || fread(texture->m_pImageData, 1, imageSize, file) != imageSize)
	{
		if (texture->m_pImageData != NULL)
		{
			free(texture->m_pImageData);
		}
		fclose(file);
		MessageBox (HWND_DESKTOP, "图片导入失败!", "出错", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	for (GLuint i=0; i<int(imageSize); i+=bytesPerPixel)
	{// 交换第一和第三字节(红 和 蓝)
		temp=texture->m_pImageData[i];
		texture->m_pImageData[i] = texture->m_pImageData[i + 2];
		texture->m_pImageData[i + 2] = temp;
	}

	fclose (file);

	glGenTextures(1, &texture->m_nTexID);                             // 产生OpenGL纹理ID

	glBindTexture(GL_TEXTURE_2D, texture->m_nTexID);                  // 绑定纹理
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 线性滤波
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 线性滤波

	if (texture->m_nBpp == 24)
	{
		type = GL_RGB;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture->m_nWidth, texture->m_nHeight, 0, type, GL_UNSIGNED_BYTE, texture->m_pImageData);

	return TRUE;
}
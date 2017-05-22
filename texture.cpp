#include <stdio.h>
#include <olectl.h>              // OLE���ƿ�ͷ�ļ�
#include <math.h>
#include <gl\gl.h>
#include "texture.h"

// ����ͼƬ��ת��Ϊ����
BOOL BuildTexture(char *szPathName, GLuint &texid)
{
	HDC   hdcTemp;                 // DC��������λͼ
	HBITMAP  hbmpTemp;             // ������ʱλͼ
	IPicture *pPicture;            // ����IPicture Interface
	OLECHAR  wszPath[MAX_PATH+1];  // ͼƬ����ȫ·��
	char  szPath[MAX_PATH+1];      // ͼƬ����ȫ·��
	long  lWidth;                  // ��λ���
	long  lHeight;                 // ��λ�߶�
	long  lWidthPixels;            // ���صĿ��
	long  lHeightPixels;           // ���صĸ߶�
	GLint  glMaxTexDim ;           // ������������ߴ�

	if (strstr(szPathName, "http://"))                     // ���·������ http:// ��...
	{
		strcpy(szPath, szPathName);                        // ��·�������� szPath
	}
	else                                                   // ������ļ�����ͼƬ
	{
		GetCurrentDirectory(MAX_PATH, szPath);             // ȡ�õ�ǰ·��
		strcat(szPath, "\\");                              // ����ַ�"\"
		strcat(szPath, szPathName);                        // ���ͼƬ�����·��
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);  // ��ASCII��ת��ΪUnicode��׼��
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if(FAILED(hr))
	{
		MessageBox (HWND_DESKTOP, "ͼƬ����ʧ��!", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	hdcTemp = CreateCompatibleDC(GetDC(0));
	if(!hdcTemp)
	{
		pPicture->Release();
		MessageBox (HWND_DESKTOP, "ͼƬ����ʧ��!", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);  // ȡ��֧�ֵ��������ߴ�

	pPicture->get_Width(&lWidth);                      // ȡ��IPicture ��� (ת��ΪPixels��ʽ)
	lWidthPixels = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);                    // ȡ��IPicture �߶� (ת��ΪPixels��ʽ)
	lHeightPixels = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// ����ͼƬ����õ�Ч��
	if (lWidthPixels <= glMaxTexDim)
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
	else
		lWidthPixels = glMaxTexDim;

	if (lHeightPixels <= glMaxTexDim)
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
	else
		lHeightPixels = glMaxTexDim;

	// ����һ����ʱλͼ
	BITMAPINFO bi = {0};                                // λͼ������
	DWORD  *pBits = 0;                                  // ָ��λͼBits��ָ��

	bi.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);   // ���ýṹ��С
	bi.bmiHeader.biBitCount  = 32;                      // 32 λ
	bi.bmiHeader.biWidth  = lWidthPixels;               // �������ֵ
	bi.bmiHeader.biHeight  = lHeightPixels;             // �߶�����ֵ
	bi.bmiHeader.biCompression = BI_RGB;                // RGB ��ʽ
	bi.bmiHeader.biPlanes  = 1;                         // һ��λƽ��

	// ����һ��λͼ�������ǿ���ָ����ɫ����� ������ÿλ��ֵ
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);

	if(!hbmpTemp)
	{
		DeleteDC(hdcTemp);
		pPicture->Release();
		MessageBox (HWND_DESKTOP, "ͼƬ����ʧ��!", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	SelectObject(hdcTemp, hbmpTemp);

	// ��λͼ�ϻ���IPicture
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// ��BGRת��ΪRGB����ALPHAֵ��Ϊ255
	for(long i = 0; i < lWidthPixels * lHeightPixels; i++)    // ѭ���������е�����
	{
		BYTE* pPixel = (BYTE*)(&pBits[i]);                    // ��ȡ��ǰ����
		BYTE  temp  = pPixel[0];                              // ��ʱ�洢��һ����ɫ����(��ɫ)
		pPixel[0]  = pPixel[2];                               // ����ɫֵ�浽��һλ
		pPixel[2]  = temp;                                    // ����ɫֵ�浽����λ
		pPixel[3]  = 255;                                     // ALPHAֵ��Ϊ255
	}

	glGenTextures(1, &texid);  // ��������

	// ʹ������λͼ�������ɵĵ�������
	glBindTexture(GL_TEXTURE_2D, texid);                               // ������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // �����˲�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // �����˲�

	// ��������
	glTexImage2D(GL_TEXTURE_2D, 0, 3, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);

	DeleteObject(hbmpTemp);
	DeleteDC(hdcTemp);

	pPicture->Release();

	return TRUE;
}

// ����һ��.TGA �ļ����ڴ�
BOOL BuildTexture(char *filename, CTexture *texture)
{    
	GLubyte  TGAheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};    // û��ѹ����TGA Header
	GLubyte  TGAcompare[12];                               // �����Ƚ� TGA Header
	GLubyte  header[6];                                    // Header�ͷ���������ֽ�
	GLuint  bytesPerPixel;                                 // ����TGA�ļ���ÿ�������õ����ֽ���
	GLuint  imageSize;                                     // �����������������ͼ��Ĵ�С
	GLuint  temp;                                          // ��ʱ����
	GLuint  type = GL_RGBA;                                // ��Ĭ�ϵ�GLģʽ����ΪRBGA (32 BPP)

	FILE *file = fopen(filename, "rb");

	if (file == NULL)
	{
		MessageBox (HWND_DESKTOP, "ͼƬ����ʧ��!", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (fread(TGAcompare,1,sizeof(TGAcompare),file) != sizeof(TGAcompare)// �Ƿ���ʮ�����ֽڿɶ�?
		|| memcmp(TGAheader,TGAcompare,sizeof(TGAheader)) != 0           // header��������Ҫ���Ƿ����?
		|| fread(header,1,sizeof(header),file) != sizeof(header))        // ����Ƕ��������ֽ�
	{
		fclose(file);
		MessageBox (HWND_DESKTOP, "ͼƬ����ʧ��!", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	texture->m_nWidth  = header[1] * 256 + header[0];      // ȷ����TGA ��� (���ֽ�*256+���ֽ�)
	texture->m_nHeight = header[3] * 256 + header[2];      // ȷ����TGA �߶� (���ֽ�*256+���ֽ�)

	if (texture->m_nWidth <= 0 || texture->m_nHeight <= 0 ||(header[4] != 24 && header[4] != 32))
	{
		fclose(file);
		MessageBox (HWND_DESKTOP, "ͼƬ����ʧ��!", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	texture->m_nBpp = header[4];                           // ��ȡTGAÿ�����ص�λ(24 or 32)
	bytesPerPixel = texture->m_nBpp / 8;                   // ����8��ȡ�� ÿ�����ص��ֽ�
	imageSize  = texture->m_nWidth*texture->m_nHeight*bytesPerPixel;  // ����TAG��������Ҫ���ڴ�

	texture->m_pImageData = (GLubyte *)malloc(imageSize);  // ����һ���ڴ�ռ������洢TGA����

	if (texture->m_pImageData == NULL || fread(texture->m_pImageData, 1, imageSize, file) != imageSize)
	{
		if (texture->m_pImageData != NULL)
		{
			free(texture->m_pImageData);
		}
		fclose(file);
		MessageBox (HWND_DESKTOP, "ͼƬ����ʧ��!", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	for (GLuint i=0; i<int(imageSize); i+=bytesPerPixel)
	{// ������һ�͵����ֽ�(�� �� ��)
		temp=texture->m_pImageData[i];
		texture->m_pImageData[i] = texture->m_pImageData[i + 2];
		texture->m_pImageData[i + 2] = temp;
	}

	fclose (file);

	glGenTextures(1, &texture->m_nTexID);                             // ����OpenGL����ID

	glBindTexture(GL_TEXTURE_2D, texture->m_nTexID);                  // ������
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // �����˲�
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // �����˲�

	if (texture->m_nBpp == 24)
	{
		type = GL_RGB;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture->m_nWidth, texture->m_nHeight, 0, type, GL_UNSIGNED_BYTE, texture->m_pImageData);

	return TRUE;
}
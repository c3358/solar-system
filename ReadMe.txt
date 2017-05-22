定义CTexture对象，使用如下两个函数可以和OpenGL绑定纹理：

// 载入BMP,JPG,GIF等文件
BOOL BuildTexture(char *szPathName, GLuint &texid);

其中 szPathName 是图像所在文件夹，如："C:/Data/figure.bmp"

                甚至可以是互联网图像，类似于"http://***"

// 载入TGA文件
BOOL BuildTexture(char *filename, CTexture *texture);

其中 filename 是文件名，和 szPathName 类似。
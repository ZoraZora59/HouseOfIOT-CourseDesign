#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <termios.h>
#include <string.h>
#include <pthread.h>

#define serial1 "/dev/s3c2410_serial1"
#define serial2 "/dev/s3c2410_serial2"
#define serial3 "/dev/s3c2410_serial3"

#define DATA_LEN 9

int *plcd;

int fd_touch;
int fd_led;
int fd_beep;
int fd_file;
int fd_mp3;
int fd_smoke;
int fd_th;

char chw[8][72]={
{/*--  文字:  智  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xC0,0x00,0x01,0xC0,0x00,0x03,0x98,0x00,0x03,
0xF8,0x38,0x07,0xE7,0xFC,0x0E,0xE7,0x1C,0x0C,0xFF,0x38,0x0F,0xE3,0x38,0x7F,0xF3,
0xF8,0x01,0xBB,0x00,0x03,0x98,0x00,0x07,0x1F,0xC0,0x0E,0xF9,0xC0,0x1C,0xE1,0xC0,
0x00,0xFF,0xC0,0x00,0xFD,0xC0,0x00,0xE1,0xC0,0x00,0xE1,0xC0,0x00,0xFF,0xC0,0x00,
0xE3,0xC0,0x00,0x43,0x80,0x00,0x01,0x80},
{/*--  文字:  能  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x1E,0x00,0x03,0x8E,0x00,0x07,0x8E,0xC0,0x07,
0x0D,0xE0,0x0E,0xEF,0xC0,0x0C,0xEE,0x30,0x1F,0xFC,0x30,0x3F,0x7E,0x70,0x3C,0x0F,
0xF0,0x0F,0xE7,0xE0,0x0C,0xEE,0x00,0x0C,0xCE,0xE0,0x0F,0xCD,0xE0,0x0C,0xCF,0x80,
0x1F,0xCE,0x0C,0x1F,0xEC,0x0C,0x1C,0xEC,0x0C,0x1B,0xEE,0x1C,0x3B,0xCF,0xFC,0x19,
0xC7,0xF8,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  家  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x3C,0x60,0x06,0x1F,0xF8,0x0F,
0xFC,0x7C,0x0E,0x00,0xF0,0x1C,0x1F,0x00,0x1D,0xFF,0x00,0x18,0xF3,0x80,0x00,0xF3,
0xC0,0x03,0xE7,0x80,0x07,0x76,0x00,0x00,0xFC,0x00,0x01,0xFE,0x00,0x07,0xFF,0x80,
0x00,0xFB,0xF0,0x01,0xDD,0xFE,0x07,0x9C,0x70,0x0E,0x1C,0x00,0x00,0xF8,0x00,0x00,
0xF8,0x00,0x00,0x78,0x00,0x00,0x00,0x00},
{/*--  文字:  居  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xC0,0x01,0xFF,0xE0,0x01,0xFD,0xE0,0x01,
0xC1,0xC0,0x01,0xC3,0xC0,0x01,0xFF,0xC0,0x01,0xEE,0x00,0x03,0x87,0x00,0x03,0x87,
0x00,0x03,0x8F,0xFE,0x03,0xFF,0xFE,0x07,0xFE,0x00,0x07,0x0C,0x00,0x06,0x0C,0xE0,
0x0E,0xFF,0xF8,0x1C,0xFC,0xF8,0x1C,0xE0,0xF0,0x38,0x60,0xE0,0x70,0x7F,0xE0,0xE0,
0x7F,0xE0,0x00,0x60,0x00,0x00,0x00,0x00},
{/*--  文字:  控  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x02,0x00,0x07,0x83,0x80,0x07,0x03,0xC0,0x03,
0x01,0x80,0x03,0x00,0x7C,0x03,0x3F,0xFE,0x03,0xFC,0x1E,0x1F,0xF0,0x30,0x1F,0x7C,
0xF0,0x03,0xEF,0x78,0x03,0x8E,0x38,0x0F,0x1C,0x00,0x1F,0x33,0xE0,0x7B,0x1F,0xC0,
0x73,0x0F,0x80,0x03,0x03,0x00,0x03,0x03,0x1C,0x1F,0x3F,0xFE,0x0F,0xFF,0x84,0x06,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  制  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xC0,0x30,0x01,0xE0,0x38,0x00,0xC0,0x38,0x0E,
0xC0,0x38,0x0F,0xD8,0x38,0x0F,0xFB,0xB8,0x1F,0xE1,0xF8,0x18,0xFD,0xF8,0x07,0xFD,
0xB8,0x7F,0xC1,0xB8,0x38,0xD9,0xF8,0x01,0xFD,0xF8,0x0F,0xDD,0xB8,0x0E,0xDD,0xB8,
0x0E,0xDC,0x38,0x0E,0xFC,0x38,0x0F,0xFC,0x38,0x0D,0xD8,0xF8,0x01,0xC0,0x78,0x00,
0xC0,0x78,0x00,0x00,0x30,0x00,0x00,0x00},
{/*--  文字:  系  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x00,0x1F,0xC0,0x01,0xFE,0x00,0x07,
0xFC,0x00,0x00,0x39,0x80,0x00,0xF3,0x80,0x01,0xC7,0x80,0x03,0xFF,0x00,0x03,0xDC,
0x00,0x00,0x7B,0x80,0x01,0xE7,0xC0,0x07,0xFF,0xE0,0x07,0xF8,0xC0,0x00,0x1C,0x00,
0x03,0x1D,0x80,0x07,0x1D,0xE0,0x0F,0x1C,0xF0,0x0E,0x1C,0x70,0x0C,0x7C,0x00,0x00,
0x7C,0x00,0x00,0x38,0x00,0x00,0x10,0x00},
{/*--  文字:  统  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x80,0x07,0x83,0xC0,0x07,
0x01,0xC0,0x07,0x00,0xF0,0x0E,0xEF,0xF0,0x0F,0xFF,0x80,0x1D,0xC7,0x00,0x3F,0x87,
0xE0,0x3B,0x0E,0xF0,0x06,0x5F,0xF0,0x0F,0xFF,0x80,0x1F,0x1F,0x80,0x1C,0x4F,0x80,
0x03,0xDD,0x86,0x0F,0x39,0x86,0x3E,0x71,0x86,0x38,0xE1,0xDF,0x01,0x81,0xFF,0x00,
0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00}};

char chn[11][48]={
{/*--  文字:  0  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x80,0x3F,0xC0,0x39,0xC0,
0x70,0xE0,0x70,0xE0,0x70,0xE0,0xF0,0xE0,0xF0,0xE0,0xF0,0xE0,0xF0,0xE0,0x70,0xE0,
0x70,0xE0,0x70,0xE0,0x39,0xC0,0x3F,0xC0,0x1F,0x80,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  1  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x3F,0x00,0x3F,0x00,
0x07,0x00,0x07,0x00,0x07,0x00,0x07,0x00,0x07,0x00,0x07,0x00,0x07,0x00,0x07,0x00,
0x07,0x00,0x07,0x00,0x07,0x00,0x0F,0x00,0x3F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  2  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x80,0x3B,0xC0,0x70,0xE0,
0x70,0xE0,0x70,0xE0,0x30,0xE0,0x01,0xC0,0x01,0xC0,0x03,0x80,0x07,0x00,0x0E,0x00,
0x1C,0x00,0x38,0x60,0x70,0x60,0x7F,0xE0,0xFF,0xE0,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  3  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x80,0x3B,0xC0,0x71,0xC0,
0x71,0xE0,0x31,0xE0,0x01,0xC0,0x03,0x80,0x0F,0x80,0x03,0xC0,0x00,0xE0,0x00,0xE0,
0x30,0xE0,0x70,0xE0,0x70,0xE0,0x7B,0xC0,0x3F,0x80,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  4  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x03,0x80,0x07,0x80,
0x07,0x80,0x0F,0x80,0x1F,0x80,0x1B,0x80,0x33,0x80,0x73,0x80,0xE3,0x80,0xFF,0xF0,
0x03,0x80,0x03,0x80,0x03,0x80,0x03,0xC0,0x0F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  5  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xE0,0x3F,0xE0,0x30,0x00,
0x30,0x00,0x30,0x00,0x30,0x00,0x7F,0x80,0x79,0xC0,0x30,0xE0,0x00,0xE0,0x00,0xE0,
0x70,0xE0,0x70,0xE0,0x70,0xE0,0x7B,0xC0,0x3F,0x80,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  6  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xC0,0x1D,0xE0,0x39,0xE0,
0x70,0xC0,0x70,0x00,0x70,0x00,0xFF,0xC0,0xFD,0xE0,0xF0,0xE0,0xF0,0xE0,0xF0,0xE0,
0x70,0xE0,0x70,0xE0,0x78,0xE0,0x3D,0xC0,0x1F,0x80,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  7  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xE0,0x7F,0xE0,0x60,0xC0,
0x61,0xC0,0x01,0x80,0x03,0x80,0x03,0x00,0x07,0x00,0x07,0x00,0x0E,0x00,0x0E,0x00,
0x0E,0x00,0x0E,0x00,0x1E,0x00,0x1E,0x00,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  8  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x80,0x39,0xC0,0x70,0xE0,
0x70,0xE0,0x70,0xE0,0x78,0xE0,0x3D,0xC0,0x1F,0x80,0x3F,0xC0,0x71,0xE0,0x60,0xE0,
0xE0,0xE0,0xE0,0xE0,0x70,0xE0,0x79,0xC0,0x3F,0x80,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  9  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x80,0x3B,0xC0,0x70,0xE0,
0x70,0xE0,0xE0,0xE0,0xE0,0xE0,0xF0,0xE0,0x71,0xE0,0x7B,0xE0,0x3F,0xE0,0x00,0xE0,
0x00,0xE0,0x31,0xC0,0x79,0xC0,0x7B,0x80,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{/*--  文字:  .  宽x高=16x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x78,0x00,0x78,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

char chs[5][72]={
{/*--  文字:  烟  宽x高=24x24   --*/
0x00,0x00,0x00,0x0E,0x00,0x00,0x0E,0x20,0x0C,0x0E,0x7F,0xFE,0x0E,0x73,0x8E,0x0E,
0xF3,0xCE,0x0F,0xF1,0x8E,0x3F,0xF1,0x8E,0x3F,0x71,0xBE,0x3E,0x7F,0xFE,0x7E,0x75,
0x8E,0x7E,0x73,0x8E,0x0E,0x73,0x8E,0x0E,0x73,0x8E,0x0E,0x73,0xEE,0x0F,0x73,0x7E,
0x0F,0xF7,0x7E,0x1D,0xF6,0x7E,0x1D,0xFC,0x3E,0x18,0xFF,0xFE,0x30,0x70,0x0E,0x70,
0x70,0x0E,0x60,0x70,0x0C,0x00,0x00,0x00},
{/*--  文字:  雾  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x00,0xE0,0x0F,0xFF,0xE0,0x0A,0x18,0x08,0x1F,0xFF,0xFC,0x18,
0x18,0x1C,0x3F,0xFF,0xF0,0x30,0x18,0x00,0x07,0xFF,0xE0,0x01,0xD0,0x80,0x03,0xFF,
0xC0,0x07,0xC3,0xC0,0x0E,0x67,0x00,0x18,0x3E,0x00,0x01,0xFF,0xFE,0x1F,0xB1,0xFC,
0x70,0x38,0x40,0x07,0xFF,0xE0,0x02,0x70,0xE0,0x00,0x60,0xE0,0x01,0xE1,0xC0,0x07,
0x8F,0xC0,0x3E,0x03,0x80,0x00,0x00,0x00},
{/*--  文字:  浓  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x07,0x00,0x18,0x07,0x00,0x1E,0x06,0x00,0x0E,0x4E,0x08,0x0E,
0xFF,0xFC,0x03,0xCE,0x1C,0xC3,0xDE,0x38,0x77,0xDE,0x30,0x7E,0x1E,0x10,0x3E,0x3E,
0x38,0x3E,0x37,0x7C,0x0E,0x7B,0xE0,0x0C,0xFB,0xC0,0x0C,0xF9,0x80,0x1D,0xF9,0xC0,
0xFF,0xB8,0xE0,0x3F,0x38,0xF0,0x3E,0x38,0x7C,0x38,0x3F,0x3E,0x38,0x3E,0x1C,0x38,
0x3C,0x00,0x38,0x38,0x00,0x00,0x00,0x00},
{/*--  文字:  度  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x1C,0x00,0x18,0x0E,0x1C,0x1F,0xFF,0xFE,0x1C,
0x61,0x80,0x1C,0x71,0xE0,0x1C,0x71,0xD8,0x1F,0xFF,0xFC,0x1C,0x71,0xC0,0x1C,0x71,
0xC0,0x1C,0x7F,0xC0,0x1C,0x71,0xC0,0x1C,0x00,0x60,0x19,0xFF,0xF0,0x18,0x60,0xE0,
0x38,0x31,0xC0,0x38,0x1B,0x80,0x30,0x1F,0x00,0x30,0x1F,0x00,0x60,0x7F,0xF0,0x61,
0xE1,0xFE,0xDF,0x00,0x3C,0x00,0x00,0x00},
{/*--  文字:  ：  宽x高=24x24   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1E,0x00,0x00,0x3E,0x00,0x00,0x1E,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,0x3E,0x00,0x00,0x1E,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};


/*
	@brief 画一个像素点
*/
void draw_point(int x,int y,int color)
{
	*(plcd+y*800+x)=color; 
}


/*
	@brief	将LCD屏幕清屏
	@param	颜色
*/
void LCD_clear(int color)
{
	int x,y;
	for(x=1;x<800;x++)
	{
		for(y=1;y<480;y++)
		{
			draw_point(x,y,color);
		}
	}
}


/*
	@brief 画一个（100,100）开始边长100的实心正方形
	@param 起始位置坐标，长度
*/	
void solid_square(int x0,int y0,int w,int h)
{
	int x,y;
	for(x=x0;x<=x0+w;x++)
	{
		for(y=y0;y<=y0+h;y++)
		{
			*(plcd + 800*y + x) = 0xffffffff;
		}
	}
}	


/*
	@brief 在某一位置显示一张图片
	@param 图片路径，起始位置，图片宽，高
*/
void draw_bmp(char *file,int x0,int y0,int w,int h)
{
	int fd=open(file,O_RDWR);
	if(fd==-1)
	{
		printf("bmp %s error\n",file);
		return;
	}
	int len=54+w*h*3;
	char bit[len];
	read(fd,bit,len);
	close(fd);
	
	//遍历数组中的每一个字节，得出图片的每一个像素点
	int i=0,x,y;
	char b,g,r;
	int color;
	for(y=y0+h-1;y>=y0;y--)
	{
		for(x=x0;x<x0+w;x++)
		{
			b=bit[54+i];
			g=bit[54+i+1];
			r=bit[54+i+2];
			i+=3;
			//合成一个像素点
			color=(r<<16)|(g<<8)|(b<<0);
			*(plcd+y*800+x)=color;
		}
	}
}


/*	
	@brief 初始界面图片显示
*/
void draw_start_screen()
{
	draw_bmp("/home/bmp/music.bmp",100,150,240,240);
	draw_bmp("/home/bmp/play.bmp",180,230,80,80);
	draw_bmp("/home/bmp/light_off.bmp",400,150,144,240);
	draw_bmp("/home/bmp/tem.bmp",630,60,44,96);
	draw_bmp("/home/bmp/hum.bmp",610,200,80,80);
	draw_bmp("/home/bmp/atmos.bmp",600,320,100,80);
}


/*
	@brief 在指定地方显示一个汉字
	@param 起始位置x0，y0，字模数组，宽，高，颜色
*/
void draw_word(int x0,int y0,char ch[],int w,int h,int color)
{
	int i,index;
	int n=w/8;	//一行n个字节
	
	//遍历数组
	for(index=0;index<n*h;index++)	//一个汉字的字模是n个字节
	{
		for(i=7;i>=0;i--)
		{
			if(ch[index]&(1<<i))
			{
				draw_point(8*(index%n)+7-i+x0,index/n+y0,color);
			}
		}
	}
}


/*
	@brief 显示五位以内的数字
	@param 字模宽,高,颜色
*/
void draw_num(int num,int x0,int y0,int color)
{
	int i;
	int n[5]={0};
	n[0]=num/10000;
	n[1]=num%10000/1000;
	n[2]=num%1000/100;
	n[3]=num%100/10;
	n[4]=num%10;
	for(i=0;i<5;i++)
	{
		if(i==0&&n[0]==0);
		else if(i==1&&n[0]==0&&n[1]==0);
		else if(i==2&&n[0]==0&&n[1]==0&&n[2]==0);
		else draw_word(x0+i*20,y0,chn[n[i]],16,24,color);
	}
}
	
	
/*
	@brief 驱动led
	@param 控制的led，led的状态，0亮，1灭
*/	
void ctrl_led(char num,char stat)
{
	unsigned char cmd[2] = {num,stat};
	write(fd_led,cmd,2);
}


/*
	@brief 驱动beep
	@param 蜂鸣器状态，1响，0不响
*/	
void ctrl_beep(char stat)
{
	write(fd_beep,&stat,1);
}


/*
	@brief 初始化串口
	@param 文件描述符，波特率
*/
int init_serial(int fd, int baudrate)
{ 

	struct termios myserial;
	//清空结构体
	memset(&myserial, 0, sizeof (myserial));
	//O_RDWR               
	myserial.c_cflag |= (CLOCAL | CREAD);
	//设置控制模式状态，本地连接，接受使能
	//设置 数据位
	myserial.c_cflag &= ~CSIZE;   //清空数据位
	myserial.c_cflag &= ~CRTSCTS; //无硬件流控制
	myserial.c_cflag |= CS8;      //数据位:8

	myserial.c_cflag &= ~CSTOPB;//   //1位停止位
	myserial.c_cflag &= ~PARENB;  //不要校验
	//myserial.c_iflag |= IGNPAR;   //不要校验
	//myserial.c_oflag = 0;  //输入模式
	//myserial.c_lflag = 0;  //不激活终端模式

	switch (baudrate)
	{
		case 9600:
			cfsetospeed(&myserial, B9600);  //设置波特率
			cfsetispeed(&myserial, B9600);
			break;
		case 115200:
			cfsetospeed(&myserial, B115200);  //设置波特率
			cfsetispeed(&myserial, B115200);
			break;
		case 19200:
			cfsetospeed(&myserial, B19200);  //设置波特率
			cfsetispeed(&myserial, B19200);
			break;
	}
		/* 刷新输出队列,清楚正接受的数据 */
	tcflush(fd, TCIFLUSH);

	/* 改变配置 */
	tcsetattr(fd, TCSANOW, &myserial);
	
	return fd;
}


/*
	@brief MP3控制
	@param 命令序列号
*/
void ctrl_mp3(int flag)
{	
	init_serial(fd_mp3,9600);
	unsigned char cmd[6][6]={{0x7e,0x03,0x01,0x02,0xef},	//播放
							{0x7e,0x03,0x02,0x01,0xef},		//暂停
							{0x7e,0x03,0x03,0x00,0xef},		//下一曲
							{0x7e,0x03,0x04,0x07,0xef},		//上一曲
							{0x7e,0x03,0x05,0x06,0xef},		//音量加
							{0x7e,0x03,0x06,0x05,0xef}};	//音量减
	write(fd_mp3,&cmd[flag],5);
}	


/*
	@brief 烟雾传感器
*/
void *monitor_smoke()
{
	//初始化串口
	
	printf("smoke*******************************************\n");
	//定义一个数据缓存，用来存储串口数据
	unsigned char cmds[DATA_LEN+1]={0xff,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
	unsigned char asks[DATA_LEN+1];
    int ret,beep_stat=0;
    int i,smokescope;
	
   while(1)
   {	
	   ret=0;
	   	printf("smoke-------------------------------------\n");
		//发送命令给烟雾传感器
		write(fd_smoke,cmds,DATA_LEN);
		sleep(1);
		//检测烟雾浓度，实际上就是读取浓度烟雾传感器的数据
		//因为烟雾传感器的数据包是9个字节，每一次读取9个字节
        ret = read(fd_smoke,asks,DATA_LEN); 
		
        //如果读取到的数据和需要读取的数据是相同的，就是有效数据
		if(ret == DATA_LEN)
        {
            for(i=0;i<DATA_LEN;i++)
            {
                printf("%#x ",asks[i]);
            }
            printf("\n");
			
			if((asks[0]==0xff)&&(asks[1]==0x86))
			{
				//计算浓度值
				smokescope=(asks[2]<<8)|(asks[3]);
			}
			
			//把烟雾浓度值显示到开发板
			printf("smokescope=%d\n",smokescope);
			solid_square(700,0,100,100);	//清屏
			draw_num(smokescope,650,00,0x00000000);
			
			//浓度高于某一个值，报警，蜂鸣器响，达到安全值的时候，蜂鸣器不响
			if(smokescope>=50&&beep_stat==0)
			{
				beep_stat=1;
				ctrl_beep(1);
				printf("the smokescope beyond 50\n");
			}else
			{
				beep_stat=0;
				ctrl_beep(0);
			}	
			sleep(1);   //进程休眠1秒
        }
    } 
}


/*
	@brief 温湿度传感器
*/
void *monitor_th()
{
	//初始化串口
	printf("th*************************************\n");
	
	//定义一个数据缓存，用来存储串口数据
	char cmdt[3]={0xa5,0x82,0x27};//连续输出温度，气压，湿度
	char askt[15];
	int ret,i,T,P,Hum;

	while(1)
	{
		printf("th-------------------------------------\n");
		//发送命令给温湿度传感器
		write(fd_th,cmdt,3);
		sleep(1);
		//while(1)
		//{
			//读取温湿度传感器的数据
			ret = read(fd_th,askt,15);
			//检测有效数据
			if(askt[2]==0x45&&askt[14]!=0){
			/*{
				break;
			}*/
		//}
		
		printf("ask:");
		for(i=0;i<15;i++)
		{
			printf("%#x ",askt[i]);
		}
		printf("\n");
		
		//温度
		T=(askt[4]<<8)|askt[5];
		T=T/100;
		printf("T=%d\n",T);
		solid_square(600,160,100,30);
		draw_num(T,600,160,0x00ff000);
		//气压
		P=(askt[6]<<24)|(askt[7]<<16)|(askt[8]<<8)|askt[9];
		P=P/100/1000;
		printf("P=%dKPa\n",P);
		solid_square(600,400,100,30);
		draw_num(P,600,400,0x00ff000);
		//湿度
		Hum=(askt[10]<<8)|askt[11];
		Hum=Hum/100;
		printf("Hum=%d%\n",Hum);
		solid_square(600,280,100,30);
		draw_num(Hum,600,280,0x00ff0000);
		}
	}
}


/*
	@brief 触摸屏 
*/	
int stat2=0,stat1=0;
void *touch()
{
	printf("touch********************************\n");
	int flag=0;
	int x=-1,y=-1;
	
	struct input_event ev;
	while(1)
	{
		printf("touch-------------------------------------\n");
		read(fd_touch,&ev,sizeof(ev));
		
		if((ev.type==EV_ABS)&&(ev.code==ABS_X))
		{
			x=ev.value;
		}
		else if((ev.type==EV_ABS)&&(ev.code==ABS_Y))
		{
			y=ev.value;
		}

		if((ev.type==EV_ABS)&&(ev.code==ABS_PRESSURE)&&(ev.value==0))
		{		
			if((x>400&&x<544)&&(y>150&&y<390))
			{	
				if(stat2==0)
				{	//开灯
					draw_bmp("/home/bmp/light_on.bmp",400,150,144,240);
					stat2=1;
				}
				else if(stat2==1)
				{	//关灯
					draw_bmp("/home/bmp/light_off.bmp",400,150,144,240);
					stat2=0;
				}
			}else if((x>180&&x<260)&&(y>230&&y<310))
			{	
				if(stat1==0)	//播放
				{
					draw_bmp("/home/bmp/pause1.bmp",180,230,80,80);
					stat1=1;
					flag=0;
				}
				else	//暂停
				{
					draw_bmp("/home/bmp/play.bmp",180,230,80,80);
					stat1=0;
					flag=1;
				}
			}else if((x>260&&x<340)&&(y>230&&y<310))//下一曲
			{
					flag=2;
			}else if((x>100&&x<180)&&(y>230&&y<310))//上一曲
			{
					flag=3;
			}else if((x>180&&x<260)&&(y>150&&y<230))//音量加
			{
					flag=4;
			}else if((x>180&&x<260)&&(y>310&&y<390))//音量减
			{
					flag=5;
			}
			ctrl_led(4,!stat2);
			ctrl_mp3(flag);
		}
	}
	
}

	
int main()
{
	//打开文件
	fd_file=open("/dev/fb0",O_RDWR);
	if(fd_file==-1)
	{
		printf("file open error\n");
		return -1;
	}
	//LED灯
	fd_led = open("/dev/led",O_RDWR);
	if(-1 == fd_led)
	{
		printf("led open error\n");
		return -1;
	}
	//beep
	fd_beep = open("/dev/beep",O_RDWR);
	if(-1 == fd_beep)
	{
		printf("beep open error\n");
		return -1;
	}
	//触摸屏
	fd_touch=open("/dev/event0",O_RDWR);
	if(fd_touch==-1)
	{
		printf("touch open error\n");
		return -1;
	}
	//打开MP3串口
	fd_mp3=open(serial1,O_RDWR);
	if(fd_mp3==-1)
	{
		printf("mp3 open error\n");
		return -1;
	}
	//打开烟雾传感器串口
	fd_smoke= open(serial2,O_RDWR);
	if (fd_smoke == -1)
	{
		printf("open smoke device error:");
		return -1;
	}
	//打开温湿度传感器串口
	fd_th= open(serial3,O_RDWR);
	if (fd_th == -1)
	{
		perror("open monitor_th device error:");
		return -1;
	}
	
	//打开内存映射
	plcd=mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,fd_file,0);
	
	//LCD清屏
	LCD_clear(0xffffffff);
	
	//显示文字
	int i;
	for(i=0;i<8;i++)
	{
		draw_word(200+i*30,50,chw[i],24,24,0x0000ff00);
	}
	for(i=0;i<5;i++)
	{
		draw_word(575+i*25,0,chs[i],24,24,0x00000000);
	}
	
	//初始界面图片显示
	draw_start_screen();
	init_serial(fd_smoke,9600);
	init_serial(fd_th,9600);
	pthread_t pid1;
	pthread_t pid2;
	pthread_t pid3;
		pthread_create(&pid1,NULL,touch,NULL);
		pthread_create(&pid3,NULL,monitor_smoke,NULL);
		pthread_create(&pid2,NULL,monitor_th,NULL);
		
	
	//判断触摸改变图片和LED状态
	//touch();
	
	//温湿度传感器
	//monitor_th();

	//烟雾传感器
	//monitor_smoke();
	while(1);
	//解映射，关闭各类文件
	munmap(plcd,800*480*4);
	close(fd_th);
	close(fd_smoke);
	close(fd_mp3);
	close(fd_touch);
	close(fd_led);
	close(fd_beep);
	close(fd_file);
	
	return 0;
}

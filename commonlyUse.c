#include "function.h"


//**********
//获取一点坐标
//**********
int get_one_xy(int *x, int *y)
{	
	struct input_event tsbuf;
	bzero(&tsbuf, sizeof(tsbuf));

	//标准IO
	FILE *ts = fopen("/dev/input/event0", "r");
	if(ts == NULL)
	{
		perror("open ts failed \n");
	}
	
	
	while(1)
	{
		fread(&tsbuf, sizeof(tsbuf), 1, ts);
		
		// 判断信号是否触摸屏信息 
		if(tsbuf.type == EV_ABS)  
		{
			if(tsbuf.code == ABS_X)	*x = tsbuf.value;
			
			if(tsbuf.code == ABS_Y) *y = tsbuf.value;

			//是否压力弹开，压力值判断手离开屏幕  
			if(tsbuf.code == ABS_PRESSURE && tsbuf.value == 0) break;
		}
	
		//是否按键弹开，有些触摸屏的按下松开通过类按键来区分     
		if(tsbuf.type == EV_KEY && tsbuf.code == BTN_TOUCH && tsbuf.value == 0) break;	
	}
	
	fclose(ts);
	return 0;
}


//*****
//蜂鸣器
//*****
void button_beep()
{
	int fd = open("/dev/beep", O_RDWR);
    if(fd == 0)
	{
		perror("open beep failed \n");
        return;
	}
	
	ioctl(fd,0,1); //蜂鸣器响
	sleep(0.5);
	ioctl(fd,1,1); //蜂鸣器灭
	sleep(0.5);	

	close(fd);
}


//**********
//显示普通图片
//**********
int show_bmp_m(int bmp_w, int bmp_h, int x, int y, char* name)
{
	int fd_pic, fd_lcd;

	//可读可写方式打开lcd屏幕文件
	fd_lcd = open("/dev/fb0", O_RDWR);
	if(fd_lcd < 0)
	{
		perror("open lcd fail");
		exit(1);
	}
	//可读可写方式打开.bmp文件
	fd_pic = open(name, O_RDWR); 
	if(fd_pic < 0)
	{
		perror("open pic fail");
		exit(1);
	}
	
	
	//1.映射图片数据
	char *p;
	p = mmap(NULL, bmp_w * bmp_h * 3 + 54, PROT_READ | PROT_WRITE, MAP_SHARED, fd_pic, 0);
	if(p == MAP_FAILED)
	{
		perror("mmap pic fail");
		exit(1); 		
	}
	p += 54; //跳过图片头数据
	
	//2.映射LCD
	unsigned int *q; //把4个字节作为一个单位，是LCD中的1个像素点
	q = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_lcd, 0);
	if(q == MAP_FAILED)
	{
		perror("mmap lcd fail");
		exit(1);
	}
	
	//3.合成并写入数据
	int i, j;
	for(j = 0; j < bmp_h; j++)
	{
		for(i = 0; i < bmp_w; i++)
		{
			*(q + 800 * (bmp_h - 1 - j + y) + i + x) = (*(p+3*i+0+bmp_w*3*j)<<0) | (*(p+3*i+1+bmp_w*3*j)<<8) | (*(p+3*i+2+bmp_w*3*j)<<16);
		}
	}

	//4.撤销映射
	munmap(p,bmp_w * bmp_h * 3 + 54);
	munmap(q,800 * 480 * 4);
	
	//5.关闭文件
	close(fd_pic);
	close(fd_lcd);
}



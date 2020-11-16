#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <string.h>
#include <strings.h>
#include <time.h>
#include <pthread.h>

#include <linux/input.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include "function.h"
#include "font.h"


//**********
//**获取滑动坐标**
//**********
void* slide(void* arg) 
{
	int x, y;
	int act = 0; //滑动方向

	while (act != 4)
	{
		ts();
		x = end_pos.x - first_pos.x;	
		y = end_pos.y - first_pos.y;
	
		if (abs(x)<abs(y))
		{
			if (y < 0) act = 1; //上划
			else       act = 2; //下划
		}
		else
		{
			if (x < 0) act = 3; //左划
			else       act = 4; //右划
		}
	}

	finish = 1;
	pthread_exit("slide succeed. \n");
}


//**********
//**获取两点坐标**
//**********
int ts()
{
	struct input_event key;
	bzero(&key, sizeof(key)); //设置字符串字符为0
	struct xy pos;

	int key_fd;
	int flag1=1, flag2=1;

	//打开触摸屏文件
	key_fd = open("/dev/input/event0", O_RDONLY);
	if (key_fd < 0)
	{
		printf("open event0 failed! \n");
		return 0;
	}
	
	while (1)
	{
		//阻塞等待  
		read(key_fd, &key, sizeof(struct input_event));
		//fread(&key, sizeof(key), 1, key_fd);

		//判断事件类型是否触摸屏事件
		if (key.type == EV_ABS)
		{
			//判断是否x轴事件
			if (key.code == ABS_X)
			{
				pos.x = key.value; //获取x轴坐标
				
				if (flag1)
				{
					flag1 = 0;
					first_pos.x = key.value;
					printf("first_pos.x=%d \n", first_pos.x );
				}
			}
			//判断是否y轴事件
			if (key.code == ABS_Y)
			{
				pos.y = key.value; //获取y轴坐标
				
				if (flag2)
				{
					flag2 = 0;
					first_pos.y = key.value;
					printf("first_pos.y=%d \n", first_pos.y );
				}
			}
		}

		//判断是否按下并已经离开屏幕
		if(key.type == EV_KEY && key.code == BTN_TOUCH && key.value == 0)
		{
			end_pos.y = pos.y;
			end_pos.x = pos.x;
			printf("end_pos.x=%d \n", end_pos.x);
			printf("end_pos.y=%d \n", end_pos.y);
			break;
		}
	}

	close(key_fd);
	return 0;
}


//**********
//**获取一点坐标**
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
	
		//是否按键弹开，有些触摸屏的按下松开模式通过类按键来区分     
		if(tsbuf.type == EV_KEY && tsbuf.code == BTN_TOUCH && tsbuf.value == 0) break;	
	}
	
	fclose(ts);
	return 0;
}


//**********
//**蜂鸣器**
//**********
void button_beep()
{
	int fd = open("/dev/beep", O_RDWR);
	
	ioctl(fd,0,1); //蜂鸣器响
	sleep(0.5);
	ioctl(fd,1,1); //蜂鸣器灭
	sleep(0.5);	

	close(fd);
}


//**********
//**显示普通图片**
//**********
int show_bmp_m(int bmp_w, int bmp_h, int x, int y, char* name)
{
	int fd_pic, fd_lcd;

	//可读可写方式打开lcd屏幕文件
	fd_lcd = open("/dev/fb0",O_RDWR);
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
		exit(1); //直接退出当前程序		
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
			*(q + 800*(bmp_h - 1 - j + y) + i + x) = (*(p+3*i+0+bmp_w*3*j)<<0) | (*(p+3*i+1+bmp_w*3*j)<<8) | (*(p+3*i+2+bmp_w*3*j)<<16);
		}
	}

	//4.撤销映射
	munmap(p,bmp_w * bmp_h * 3 + 54);
	munmap(q,800 * 480 * 4);
	
	//5.关闭文件
	close(fd_pic);
	close(fd_lcd);
}


//**********
//**显示特效图片**
//**********
int show_bmp(int bmp_w, int bmp_h, int x, int y, char* name,int choice)
{
	int fd_pic, fd_lcd;

	//可读可写方式打开lcd屏幕文件
	fd_lcd = open("/dev/fb0",O_RDWR);
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
		exit(1); //直接退出当前程序		
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
	switch (choice)
	{
	case 1:
		//横向百叶窗
		for (j = bmp_h - 1; j >= bmp_h - 108; j--)
		{
			for (i = 0; i < bmp_w; i++)
			{
				*(q + 800 * (bmp_h - 1 - j + y) + i + x) = (*(p + 3 * i + 0 + bmp_w * 3 * j) << 0) | (*(p + 3 * i + 1 + bmp_w * 3 * j) << 8) | (*(p + 3 * i + 2 + bmp_w * 3 * j) << 16); 
				*(q + 800 * (bmp_h - 1 - j + y + 108) + i + x) = (*(p + 3 * i + 0 + bmp_w * 3 * (j - 108)) << 0) | (*(p + 3 * i + 1 + bmp_w * 3 * (j - 108)) << 8) | (*(p + 3 * i + 2 + bmp_w * 3 * (j - 108)) << 16); 
    			*(q + 800 * (bmp_h - 1 - j + y + 216) + i + x) = (*(p + 3 * i + 0 + bmp_w * 3 * (j - 216)) << 0) | (*(p + 3 * i + 1 + bmp_w * 3 * (j - 216)) << 8) | (*(p + 3 * i + 2 + bmp_w * 3 * (j - 216)) << 16); 
    			*(q + 800 * (bmp_h - 1 - j + y + 324) + i + x) = (*(p + 3 * i + 0 + bmp_w * 3 * (j - 324)) << 0) | (*(p + 3 * i + 1 + bmp_w * 3 * (j - 324)) << 8) | (*(p + 3 * i + 2 + bmp_w * 3 * (j - 324)) << 16); 		
			}
			usleep(3000);
		}break;

	case 2:	
		//从上往下
		for (j = bmp_h - 1; j >= 0; j--)
		{
			for (i = 0; i < bmp_w; i++)
			{
				*(q + 800 * (bmp_h - 1 - j + y) + i + x) = (*(p + 3 * i + 0 + bmp_w * 3 * j) << 0) | (*(p + 3 * i + 1 + bmp_w * 3 * j) << 8) | (*(p + 3 * i + 2 + bmp_w * 3 * j) << 16); 
			}
			usleep(100); 
		};break;

	case 3:	
		//从下往上
		for (j = 0; j < bmp_h; j++)
		{
			for (i = 0; i < bmp_w; i++)
			{
				*(q + 800 * (bmp_h - 1 - j + y) + i + x) = (*(p + 3 * i + 0 + bmp_w * 3 * j) << 0) | (*(p + 3 * i + 1 + bmp_w * 3 * j) << 8) | (*(p + 3 * i + 2 + bmp_w * 3 * j) << 16);
			}
			usleep(100); 
		};break;

	}

	//4.撤销映射
	munmap(p,bmp_w * bmp_h * 3 + 54);
	munmap(q,800 * 480 * 4);
	
	//5.关闭文件
	close(fd_pic);
	close(fd_lcd);
}


//**********
//**查找文件**
//**********
int readFileList(char* name1, char* name2, char arr_name[20][30])
{
	char basePath[1000];
	memset(basePath, '\0', sizeof(basePath));
	getcwd(basePath, 999); //获取当前路径

	int i = 0;
	DIR* dir;
	struct dirent* ptr;

	if ((dir = opendir(basePath)) == NULL)
	{
		perror("Open dir error...");
		exit(1);
	}

	//读取当前路径下的所有文件名
	while ((ptr = readdir(dir)) != NULL)
	{
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
			continue;
		else if (ptr->d_type == 8 && strstr(ptr->d_name, name1) && strstr(ptr->d_name, name2)) //筛选特定文件
		{
			strcpy(arr_name[i], ptr->d_name);
			i++;
		}
	}

	closedir(dir);
	return i;
}


//**********
//**日期时间**
//**********
void *time1(void *arg)
{
    char buf[128];
	struct tm *time_test;
	time_t tt;
	
	while(1)
	{
		time(&tt); //得到当前日历时间
		time_test = gmtime(&tt);

		sprintf(buf,"Data: %d-%d-%d\nTime:%02d:%02d:%02d ",
			time_test->tm_year+1905,time_test->tm_mon+11,time_test->tm_mday,time_test->tm_hour+15,time_test->tm_min,time_test->tm_sec);
		show_font(buf, 24);
		sleep(1);

		//printf("%s \r",buf);
		//fflush(stdout);
	} 
}


//**********
//**初始化Lcd**
//**********
struct LcdDevice *init_lcd(const char *device)
{
	//申请空间
	struct LcdDevice* lcd = malloc(sizeof(struct LcdDevice));
	if(lcd == NULL)
	{
		return NULL;
	} 

	//1打开设备
	lcd->fd = open(device, O_RDWR);
	if(lcd->fd < 0)
	{
		perror("open lcd fail");
		free(lcd);
		return NULL;
	}
	
	//映射
	lcd->mp = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd->fd,0);

	return lcd;
}


//**********
//**打印字体**
//**********
int show_font(char buf[],int size)
{
	
    //初始化Lcd
	struct LcdDevice* lcd = init_lcd("/dev/fb0");
			
	//打开字体	
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
	  
	//字体大小的设置
	fontSetSize(f,size);
	
	//创建一个画板（点阵图）
	bitmap *bm = createBitmapWithInit(133,48,4,getColor(100,255,255,255)); 
	
	//将字体写到点阵图上
	fontPrint(f,bm,0,0,buf,getColor(0,0,0,0),0);
	
	//把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd->mp,667,432,bm);
	
	//关闭字体，关闭画板
	fontUnload(f);
	destroyBitmap(bm);
    return 0;
}


//*******
//电子相册
//*******
#include "function.h"


int showpic()
{
	int x, y, i = 0, j = 0;
	show_bmp_m(800, 480, 0, 0, "./source/picture.bmp");

	char* name1 = "source/image";
	char* name2 = ".bmp";
	char pic_name[20][30];
	memset(pic_name, '\0', sizeof(pic_name)); //字符数组初始化为0
	int length = readFileList(name1, name2, pic_name);

    if (length == 0)
    {
        printf("没有找到指定的图片!!! \n");
        return 0;
    }

	while (1)
	{
		ts();

		if (first_pos.x > 710  && first_pos.x < 790 && first_pos.y > 200 && first_pos.y < 280)
		{
			i = roll(i, length, 1); //上一张
			show_bmp_m(672, 432, 20, 25, pic_name[i]); 
            continue;
		}
		else if (first_pos.x > 710  && first_pos.x < 790 && first_pos.y > 350 && first_pos.y < 430)
		{
			i = roll(i, length, 2); //下一张
			show_bmp_m(672, 432, 20, 25, pic_name[i]); 
            continue;
		}
		else if (first_pos.x > 710 && first_pos.x < 790 && first_pos.y > 50 && first_pos.y < 130)
		{
			break; //退出
		}
		else
		{
            //滑动切换，特效显示图片
			x = end_pos.x - first_pos.x;
			y = end_pos.y - first_pos.y;
			int act = 0;

			if (abs(x) < abs(y))
			{
				if (y < 0) act = 1; //上划
				else       act = 2; //下划
			}
			else
			{
				if (x < 0) act = 3; //左划
				else       act = 4; //右划
			}

			switch (act)
			{
			case 1:
            i = roll(i, length, 1);
            show_bmp(672, 432, 20, 25, pic_name[i], 3); //最后一个参数为选择的特效，1为横向百叶窗，2为从上往下，3为从下往上
            break;

			case 2:
            i = roll(i, length, 2);
            show_bmp(672, 432, 20, 25, pic_name[i], 2); 
            break; 

			case 3:
            i = roll(i, length, 2); 
            show_bmp(672, 432, 20, 25, pic_name[i], 1); 
            break;

			case 4:
            i = roll(i, length, 1);
            show_bmp(672, 432, 20, 25, pic_name[i], 1); 
            break;
			}
		}
	}

	return 0;
}


//循环显示图片
int roll(int i, int length, int direct)
{
    if (direct == 1)
    {
        if (i == 0) i = length;
		i--;
    }
    else
    {
        if (i == length - 1) i = -1;
		i++;
    }

    return i;
}


//查找文件
int readFileList(char* name1, char* name2, char arr_name[20][30])
{
	char basePath[1000];
	memset(basePath, '\0', sizeof(basePath)); //字符数组初始化为0
	getcwd(basePath, 999); //获取当前路径

	int i = 0;
	DIR* dir;
	struct dirent* ptr;

	if ((dir = opendir(basePath)) == NULL)
	{
		perror("Open dir error...");
		exit(1);
	}

	//读取当前路径下指定的所有文件名
	while ((ptr = readdir(dir)) != NULL)
	{
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) 
			continue;

		if (ptr->d_type == 8 && strstr(ptr->d_name, name1) && strstr(ptr->d_name, name2)) //筛选特定文件，文件类型为块设备文件
		{
			strcpy(arr_name[i], ptr->d_name);
			i++;
		}
	}

	closedir(dir);
	return i;
}


//显示特效图片
int show_bmp(int bmp_w, int bmp_h, int x, int y, char* name,int choice)
{
	int fd_pic, fd_lcd;

	//可读可写方式打开lcd屏幕文件
	fd_lcd = open("/dev/fb0",O_RDWR);
	if(fd_lcd < 0)
	{
		perror("Open lcd fail");
		exit(1);
	}
	//可读可写方式打开.bmp文件
	fd_pic = open(name, O_RDWR); 
	if(fd_pic < 0)
	{
		perror("Open pic fail");
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
		};break;

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
	munmap(p, bmp_w * bmp_h * 3 + 54);
	munmap(q, 800 * 480 * 4);
	
	//5.关闭文件
	close(fd_pic);
	close(fd_lcd);
}
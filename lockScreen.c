//****
//锁屏
//****
#include "function.h"

int lock()
{
	//设置为分离线程
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//创建线程
	pthread_t pid;
	pthread_create(&pid, &attr, slide, NULL);

    //动态壁纸效果
	while (1)
	{
		show_bmp_m(800, 480, 0, 0, "./source/unlock1.bmp");
		usleep(500000); //睡眠0.5s
		if (finish) break;

		show_bmp_m(800, 480, 0, 0, "./source/unlock2.bmp");
		usleep(500000);
		if (finish) break;
	}

	return 0;
}


//线程例程，获取滑动坐标
void* slide(void* arg) 
{
	int x, y;
	int act = 0; //滑动方向

	while (act != 4)
	{
		ts();
		x = end_pos.x - first_pos.x;	
		y = end_pos.y - first_pos.y;
	
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
	}

	finish = 1;
	pthread_exit("Slide succeed. \n");
}


//获取两点坐标
int ts()
{
	struct input_event key;
	bzero(&key, sizeof(key)); //设置字符串字符为0
	struct xy pos;
	int flag1=1, flag2=1; //用于判断是否为初始坐标

	//打开触摸屏文件
	int key_fd = open("/dev/input/event0", O_RDONLY);
	if (key_fd < 0)
	{
		printf("Open event0 failed! \n");
		return 0;
	}
	
	while (1)
	{
		//阻塞等待,获取输入事件 
		read(key_fd, &key, sizeof(struct input_event));
		//fread(&key, sizeof(key), 1, key_fd);

        //判断是否触摸屏事件
		if (key.type == EV_ABS) 
		{
			if (key.code == ABS_X) //是否x轴事件
			{
				pos.x = key.value; //获取x轴坐标
				
				if (flag1)
				{
					flag1 = 0;
					first_pos.x = key.value;
					printf("first_pos.x=%d \n", first_pos.x);
				}
			}

			if (key.code == ABS_Y) //是否y轴事件
			{
				pos.y = key.value; //获取y轴坐标
				
				if (flag2)
				{
					flag2 = 0;
					first_pos.y = key.value;
					printf("first_pos.y=%d \n", first_pos.y);
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
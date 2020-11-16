#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "function.h"
#include "font.h"


int main()
{
	printf("******** start to work ********** \n");
	
	Loop:
	quit_door = false;
	finish = 0;
	while (!quit_door)
	{
		lock(); //滑动解锁
		door(); //门禁系统
	}
	
	show_gif(); //开机动画
	show_bmp_m(800, 480, 0, 0, "./cover.bmp"); //主界面
	pthread_t tid;
    pthread_create(&tid, NULL, time1, NULL); //创建子线程，实时显示日期时间

	//各功能模块的重复选择
	int x, y;	
	while (1)
	{
		get_one_xy(&x, &y); //获取坐标
		button_beep(); 
		printf("x is %d\t y is %d\n", x, y);

		// 电子相册
		if (x > 50 && x < 150 && y > 350 && y < 420) 
		{
			showpic();
			show_bmp_m(800, 480, 0, 0, "./cover.bmp");
			continue;
		}
		// 音乐播放
		else if (x > 250 && x < 350 && y > 350 && y < 420) 
		{
			music();
			show_bmp_m(800, 480, 0, 0, "./cover.bmp");
			continue;
		}
		//退出主界面
		else if (x > 450 && x < 550 && y > 350 && y < 420)
		{
			pthread_cancel(tid); //收回显示日期时间的子线程
			goto Loop;
		}
		//远程聊天
		else if (x > 650 && x < 750 && y > 250 && y < 420)
		{
			wechat();
			show_bmp_m(800, 480, 0, 0, "./cover.bmp");
			continue;
		}
	}
	
	return 0;
}


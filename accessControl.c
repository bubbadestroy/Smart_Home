//****
//门禁
//****
#include "function.h"


int door()
{
	show_bmp_m(800, 480, 0, 0, "./source/door.bmp");
	char correct_pass[4] = {'1', '2', '3', '4'}; //设置密码
	char password[5] = {' ', ' ', ' ', ' '}; //新增的一位存放光标，防止越界

	int x, y, i = 0, j;
    int num; //已输入字符
	int status = 0; //状态，0为输入密码
	bool quit = false;//是否退出


	while (1)
	{
		get_one_xy(&x, &y); //获取一点坐标
		button_beep();
		printf("x is %d\t y is %d\n", x, y);

		switch (x / 90)
		{
		case 1:		
			switch ((y + 15) / 90)
			{
			case 1:num = '1'; break;
			case 2:num = '4'; break;
			case 3:num = '7'; break;
			case 4:num = '*'; break;
            //默认情况不可省去，当不点击按钮区域时不做任何操作
			default: continue; 
			}break;

		case 2:	
			switch ((y + 15) / 90)
			{
			case 1:num = '2'; break;
			case 2:num = '5'; break;
			case 3:num = '8'; break;
			case 4:num = '0'; break;
			default: continue;
			}break;

		case 3:
			switch ((y + 15) / 90)
			{
			case 1:num = '3'; break;
			case 2:num = '6'; break;
			case 3:num = '9'; break;
			case 4:num = '#'; break;
			default: continue;
			}break;

		case 5:
		case 6:
		case 7:
			switch ((y + 15) / 90)
			{
			case 1: status = 1; break; //退出
			case 2: status = 2; break; //删除
			case 3: status = 3; break; //重置
			case 4: status = 4; break; //确认
			default: continue;
			}break;

		default: continue;
		}
			
        //不同的状态作不同的处理
		switch (status)
		{
		case 0:
			password[i] = num;
			if (i <= 3) i++;
			switch (i) //动态显示密码情况
			{
			case 0: show_bmp_m(160, 25, 560, 30, "./source/pw0.bmp"); break;
			case 1: show_bmp_m(160, 25, 560, 30, "./source/pw1.bmp"); break;
			case 2: show_bmp_m(160, 25, 560, 30, "./source/pw2.bmp"); break; 
			case 3: show_bmp_m(160, 25, 560, 30, "./source/pw3.bmp"); break; 
			case 4: show_bmp_m(160, 25, 560, 30, "./source/pw4.bmp"); break; 
			}
			printf("[%c][%c][%c][%c] \n", password[0], password[1], password[2], password[3]);
			break;

		case 1:
			finish = 0;
			quit = true;
			break;	

		case 2:
			if (i > 0) i--; //光标前移一位，再置空;若为最前一位，不移动
			password[i] = ' ';
			switch (i)
			{
			case 0: show_bmp_m(160, 25, 560, 30, "./source/pw0.bmp"); break;
			case 1: show_bmp_m(160, 25, 560, 30, "./source/pw1.bmp"); break;
			case 2: show_bmp_m(160, 25, 560, 30, "./source/pw2.bmp"); break; 
			case 3: show_bmp_m(160, 25, 560, 30, "./source/pw3.bmp"); break; 
			case 4: show_bmp_m(160, 25, 560, 30, "./source/pw4.bmp"); break; 
			}
			printf("delete \n");
			printf("[%c][%c][%c][%c]\n", password[0], password[1], password[2], password[3]);
			status = 0;
			break;

		case 3:			
			for (j = 0; j < 4; j++) //每一位置空
				password[j] = ' ';

			show_bmp_m(160, 25, 560, 30, "./source/pw0.bmp");
			printf("reset \n");
			i = 0;
			status = 0;
			break;

		case 4:
			for (j = 0; j < 4; j++) //每一位校验
			{
				if (password[j] != correct_pass[j]) 
                    break;
			}

			if (j == 4)
			{
				printf("Password correct \n");
				show_bmp_m(300, 300, 250, 90, "./source/good.bmp");
				sleep(2);
				quit = true;
				quit_door = true;
			}
			else
			{
				printf("Password wrong \n");
				show_bmp_m(300, 300, 250, 90, "./source/bad.bmp");
				sleep(1);
				button_beep(); //蜂鸣器警告
				show_bmp_m(800, 480, 0, 0, "./source/door.bmp");
				
				for (j = 0; j < 4; j++) //每一位置空
					password[j] = ' ';
				i = 0;
			}	
            		
			status = 0;
			break;
		}

		if (quit) break;
	}

	return 0;
}
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
//**锁屏**
//**********
int lock()
{
	//设置线程属性为分离
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//创建线程
	pthread_t pid;
	pthread_create(&pid, &attr, slide, NULL);

	while (1)
	{
		show_bmp_m(800, 480, 0, 0, "./unlock1.bmp");
		usleep(500000); //睡眠0.5s
		if (finish) break;

		show_bmp_m(800, 480, 0, 0, "./unlock2.bmp");
		usleep(500000);
		if (finish) break;
	}

	return 0;
}


//**********
//**门禁**
//**********
int door()
{
	show_bmp_m(800, 480, 0, 0, "./door.bmp");
	int x, y, i = 0, j;
    int num; //已输入字符
	int status = 0; //状态，0为输入密码
	char password[5] = {' ', ' ', ' ', ' '}; //新增的一位存放光标，防止越界
	char correct_pass[4] = {'1', '2', '3', '4'}; //设置密码
	bool quit = false;//是否退出

	while (1)
	{
		get_one_xy(&x, &y);
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
			
	
		switch (status)
		{
		case 0:
			password[i] = num;
			if (i <= 3) i++;
			switch (i)
			{
			case 0: show_bmp_m(160, 25, 560, 30, "./pw0.bmp"); break;
			case 1: show_bmp_m(160, 25, 560, 30, "./pw1.bmp"); break;
			case 2: show_bmp_m(160, 25, 560, 30, "./pw2.bmp"); break; 
			case 3: show_bmp_m(160, 25, 560, 30, "./pw3.bmp"); break; 
			case 4: show_bmp_m(160, 25, 560, 30, "./pw4.bmp"); break; 
			}
			printf("[%c][%c][%c][%c]\n", password[0], password[1], password[2], password[3]);
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
			case 0: show_bmp_m(160, 25, 560, 30, "./pw0.bmp"); break;
			case 1: show_bmp_m(160, 25, 560, 30, "./pw1.bmp"); break;
			case 2: show_bmp_m(160, 25, 560, 30, "./pw2.bmp"); break; 
			case 3: show_bmp_m(160, 25, 560, 30, "./pw3.bmp"); break; 
			case 4: show_bmp_m(160, 25, 560, 30, "./pw4.bmp"); break; 
			}
			printf("delete \n");
			printf("[%c][%c][%c][%c]\n", password[0], password[1], password[2], password[3]);
			status = 0;
			break;

		case 3:			
			for (j = 0; j < 4; j++) //每一位置空
				password[j] = ' ';

			show_bmp_m(160, 25, 560, 30, "./pw0.bmp");
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
				printf("password correct \n");
				show_bmp_m(300, 300, 250, 90, "./good.bmp");
				sleep(2);
				quit = true;
				quit_door = true;
			}
			else
			{
				printf("password wrong \n");
				show_bmp_m(300, 300, 250, 90, "./bad.bmp");
				sleep(1);
				button_beep();
				show_bmp_m(800, 480, 0, 0, "./door.bmp");
				
				for (j = 0; j < 4; j++) //每一位置空
				{
					password[j] = ' ';
				}
				i = 0;
			}			
			status = 0;
			break;
		}

		if (quit)
			break;
	}

	return 0;
}


//**********
//**显示开机动画**
//**********
int show_gif()
{
	int i;
	char buf[30] = {0};
	for(i = 0; i < 35; i++)
	{
		printf("%s\n", buf);
		sprintf(buf, "./shuma2.2/Frame%d.bmp", i + 1);
		show_bmp_m(800, 480, 0, 0, buf);
		usleep(20000);
	}	
	
	return 0;
}


//**********
//**电子相册**
//**********
int showpic()
{
	int x, y, i = 0,j = 0;
	show_bmp_m(800, 480, 0, 0, "./picture.bmp");

	char* name1 = "image";
	char* name2 = ".bmp";
	char pic_name[20][30];
	int length = readFileList(name1, name2, pic_name);

	while (1)
	{
		ts();

		if (first_pos.x > 710  && first_pos.x < 790 && first_pos.y > 200 && first_pos.y < 280)
		{
			i = roll(i, length, 1);
			show_bmp_m(672, 432, 20, 25, pic_name[i]); //上一张
		}
		else if (first_pos.x > 710  && first_pos.x < 790 && first_pos.y > 350 && first_pos.y < 430)
		{
			i = roll(i, length, 2);
			show_bmp_m(672, 432, 20, 25, pic_name[i]); //下一张
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
            show_bmp(672, 432, 20, 25, pic_name[i], 3); 
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


//**********
//**循环显示图片**
//**********
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


//**********
//**音乐播放**
//**********
int music()
{
	show_bmp_m(800, 480, 0, 0, "./music_cover.bmp");
	int x, y, i = 0, p = 0;
	char Music_name[50];

    //搜索特定文件
	char* name1 = "music";
	char* name2 = ".bmp";
	char* name3 = ".mp3";
	char pic_name[20][30];	
	char mus_name[20][30];
	int pic_len = readFileList(name1, name2, pic_name);
	int mus_len = readFileList(name1, name3, mus_name);

	while (1)
	{
		get_one_xy(&x, &y);
		button_beep();
		printf("x is %d\t y is %d\n", x, y);

		if (x > 60 && x < 100 && y > 400 && y < 440) //上一首
		{
			system("killall madplay");
			if (i == 0) i = mus_len;
			i--;
		}
		else if (x > 260 && x < 300 && y > 400 && y < 440) //下一首
		{
			system("killall madplay");
			if (i == mus_len-1) i = -1;
			i++;
		}

		else if (x > 160 && x < 200 && y > 400 && y < 440) //播放暂停
		{
			if (p == 0)
			{
				system("killall -STOP madplay &");
				p = 1;
			}
			else
			{
				system("killall -CONT madplay &");
				p = 0;
			}
		}
		else if (x > 360 && x < 400 && y > 400 && y < 440) //退出
		{
			system("killall madplay");
			break;
		}

		sprintf(Music_name,"madplay -a -10 %s &",mus_name[i]);
		system(Music_name); 
        show_bmp_m(340, 240, 60, 60, pic_name[i]);
	}

	return 0;
}


//**********
//**远程聊天**
//**********
int wechat()
{
    show_bmp_m(800, 480, 0, 0, "./wechat.bmp");

    pthread_t tid;
    pthread_create(&tid, NULL, server, NULL);	
    sleep(5); //等待5秒让对方的服务端建立起来	 
    commute();

    pthread_cancel(tid);
    return 0;
}


//**********
//**远程聊天-客户端**
//**********
int commute()
{
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0); //使用ipv4，字节流tcp协议
	if(tcp_socket == -1)
	{
		printf("open socket failed \n");
	}
	
	// 定义结构体存储服务端IP和端口号
	struct sockaddr_in  addr_ser;
	addr_ser.sin_family = AF_INET;		
	addr_ser.sin_port = htons(1111); //绑定端口 htons把整形数据装换成网间数据
	addr_ser.sin_addr.s_addr = inet_addr("192.168.78.1"); //绑定IP
	
	socklen_t len = sizeof(addr_ser);
	// 给服务器发送连接请求 
	connect(tcp_socket, (struct sockaddr *)&addr_ser, len);
	
    //摩斯密码
	char mos[20][6];
	int i = 0, j = 0;
	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 6; j++)
		{
			mos[i][j] = ' ';
		}
	}

	
	int letter_cout = 0;
	int bit_count = 0;
	int x, y;
	while (1)
	{
		get_one_xy(&x, &y);
		button_beep();
		printf("x is %d\t y is %d\n", x, y);


		if (x > 125 && x < 275 && y > 400 && y < 440) //0
		{
			if (bit_count == 6) continue;
			mos[letter_cout][bit_count] = '0';
			bit_count++;
		}
		else if (x > 325 && x < 475 && y > 400 && y < 440) //1
		{
			if (bit_count == 6) continue;
			mos[letter_cout][bit_count] = '1';
			bit_count++;
		}
		else if (x > 525 && x < 675 && y > 400 && y < 440) //‘/’
		{
			if (letter_cout == 20) continue;
			letter_cout++;
			bit_count = 0;
		}

		else if (x > 125 && x < 355 && y > 315 && y < 355) //重置
		{
			for(j=0;j<20;j++)
			{
				for (i = 0; i < 6; i++)
				{
				mos[j][i] = ' ';
				}
			}
			letter_cout = 0;
			bit_count = 0;
		}
		else if (x > 445 && x < 675 && y > 315 && y < 355) //发送
		{
			write(tcp_socket, mos,sizeof(mos));
			printf("send: %s \n", mos);
			
		}
		else if (x > 730 && x < 780 && y > 20 && y < 70) //退出
		{
			break;
		}
	}
	
	close(tcp_socket);
	return 0;
}


//**********
//**远程聊天-服务端**
//**********
void* server()
{
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_socket == -1)
	{
		printf("open socket failed \n");
	}

	// 定义结构体存储本地IP和端口号
	struct sockaddr_in  addr_ser, addr_clent;
	addr_ser.sin_family = AF_INET;		
	addr_ser.sin_port = htons(6666);	// 本地端口  htons 把整形数据装换成网间数据
	addr_ser.sin_addr.s_addr = inet_addr("192.168.78.4"); //本地IP
	
	// 绑定本地url
	socklen_t len = sizeof(addr_ser);
	bind(tcp_socket, (struct sockaddr *)&addr_ser, len);

	// 限制监听人数 同时最多处理五个客户端请求  cpu处理器 内存等有关
	listen(tcp_socket, 5);

	// 等待别人发送请求  接收请求后 会创建一个fd 用于通信
	len = sizeof(addr_clent);
	bzero(&addr_clent, len);
	int new_fd = accept(tcp_socket, (struct sockaddr *)&addr_clent, &len);
	if(new_fd == -1)
	{
		printf("accept failed\n");
	}
	char ipbuf[64];
	bzero(ipbuf,64);
	char buf[128];

	// 打印出来客户端的IP 和 端口号
	printf("connect succeed. \n");
	printf("clent ip is %s\t port is %uh\n", inet_ntop(AF_INET, &addr_clent.sin_addr, ipbuf, 64),  ntohs(addr_clent.sin_port));
	
	while(1)
	{
        sleep(1);
        bzero(buf,128);		
        read(new_fd, buf, 128);
        show_font(buf, 32);
        printf("from chatter : %s \n", buf);
	}

	// 关闭函数
	close(tcp_socket);
	close(new_fd);
	return;
}
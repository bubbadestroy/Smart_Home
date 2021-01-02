//**********
//显示开机动画
//**********
#include "function.h"


int show_gif()
{
	char buf[30] = {0};
    int i;
	for(i = 0; i < 35; i++) //每一帧动画
	{
		sprintf(buf, "./source/shuma2.2/Frame%d.bmp", i + 1);
		show_bmp_m(800, 480, 0, 0, buf);
		printf("%s\n", buf);

		usleep(20000); //休眠0.02s
	}	
	
	return 0;
}
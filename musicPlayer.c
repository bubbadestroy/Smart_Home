//*********
//音乐播放器
//*********
#include "function.h"


int music()
{
	show_bmp_m(800, 480, 0, 0, "./source/music_cover.bmp");
	int x, y, i = 0, p = 0;
	char Music_name[50];

    //搜索特定的音乐文件
	char* name1 = "/source/music";
	char* name2 = ".bmp";
	char* name3 = ".mp3";
	char pic_name[20][30];	
	char mus_name[20][30];
	int pic_len = readFileList(name1, name2, pic_name);
	int mus_len = readFileList(name1, name3, mus_name);

    if (pic_len == 0)
    {
        printf("没有找到指定的图片!!! \n");
        return 0;
    }
    if (mus_len == 0)
    {
        printf("没有找到指定的音乐!!! \n");
        return 0;
    }

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

		sprintf(Music_name,"madplay -a -10 %s &", mus_name[i]);
		system(Music_name); 
        show_bmp_m(340, 240, 60, 60, pic_name[i]);
	}

	return 0;
}
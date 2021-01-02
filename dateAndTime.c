//*******
//日期时间
//*******
#include "function.h"


void *time1(void *arg)
{
    char buf[128];
	time_t tt;
	struct tm *time_test;
	
	while(1)
	{
		time(&tt); //获取当前日历时间
		time_test = gmtime(&tt);

		sprintf(buf,"Data: %d-%d-%d\nTime:%02d:%02d:%02d ",
			time_test->tm_year + 1905, time_test->tm_mon + 11, time_test->tm_mday, time_test->tm_hour + 15, time_test->tm_min, time_test->tm_sec);
		show_font(buf, 24);
		sleep(1);

		//printf("%s \r",buf);
		//fflush(stdout);
	} 
}


//打印字体
int show_font(char buf[], int size)
{	
    //初始化Lcd
	struct LcdDevice* lcd = init_lcd("/dev/fb0");
			
	//打开字体	
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
	  
	//字体大小的设置
	fontSetSize(f, size);
	
	//创建一个画板（点阵图）
	bitmap *bm = createBitmapWithInit(133,48,4,getColor(0,255,255,255)); //133,48为画板大小 
	
	//将字体写到点阵图上
	fontPrint(f,bm,0,0,buf,getColor(0,0,0,0),0);
	
	//把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd->mp,667,432,bm); //667,432为起始位置
	
	//关闭字体，关闭画板
	fontUnload(f);
	destroyBitmap(bm);
    return 0;
}


//初始化Lcd
struct LcdDevice *init_lcd(const char *device)
{
	//申请空间
	struct LcdDevice* lcd = malloc(sizeof(struct LcdDevice));
	if(lcd == NULL)
	{
		return NULL;
	} 

	//打开设备
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
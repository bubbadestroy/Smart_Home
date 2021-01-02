# Smart_Home

智能家居项目拥有丰富的模块功能，给用户提供多样化的体验。

使用这个项目，您需要的环境如下：
1.安装了Ubuntu、gcc、交叉编译器arm-linux-gcc的电脑。拥有vscode集成开发环境在编辑代码时更加方便，当然Linux系统自带的vim也基本满足需求。
2.拥有Linux内核的arm6818开发板，以及配套的各种接口和数据线，如串口、HDMI线、网线。
3.程序进行编译时需要用到相关的链接库，有字体库、数学库和多线程库。字体库已附在本项目中，后两个库为Linux系统自带。
4.程序运行时，音乐播放器需要音乐文件，电子相册需要一些图片文件，其他功能模块需要界面图片和图标。在此附上部分资源于source文件夹中。

每个c源文件都实现了单个模块的功能，以下是资源文件的介绍：
1.accessControl.c 实现门禁功能，需要输入正确密码才能进入主界面。
2.bootAnimation.c 实现播放开机动画。
3.commonlyUse.c 实现了常用的基本功能，如获取点击液晶屏时的坐标、蜂鸣器的使用以及普通图片的显示。
4.dateAndTime.c 实现了实时显示日期时间，包括年月日和时分秒。
5.elePhotoAlbum.c 实现电子相册功能，能够循环切换图片，且切换时展示百叶窗等简单特效。
6.lockScreen.c 实现锁屏功能，锁屏界面美观而简约，而且有向右动态移动的光标箭头指示解锁操作。
7.musicPlayer.c 实现音乐播放器功能，能偶切换歌曲，暂停播放，同时展示专辑封面。
8.remoteChat.c 和 wechat.c 实现远程聊天功能，前者为arm开发板所使用，后者为电脑所使用，每一方都有客户端和服务器，实现双向通信。
9.smartHome.c 为整个项目的主函数，实现了整个系统的使用流程和架构展示。
10..font.h 和 function.h 为代码所需要的头文件，里面包含了所有函数的声明和结构体的定义。ret 和 wechat 为编译后的可执行文件。

使用交叉编译器进行编译的指令如下：
arm-linux-gcc smartHome.c lockScreen.c accessControl.c bootAnimation.c elePhotoAlbum.c musicPlayer.c remoteChat.c dateAndTime.c commonlyUse.c -o ret -L./ -lfont -lm -lpthread
wechat.c用于电脑，因此使用gcc编译即可：
gcc wechat.c -o wechat -lpthread

最后，祝你0error，0warning！

//*******
//远程聊天
//*******
#include "function.h"


int wechat()
{
    show_bmp_m(800, 480, 0, 0, "./source/wechat.bmp");

    pthread_t tid;
    pthread_create(&tid, NULL, server, NULL);	
    sleep(3); //等待3秒让对方的服务端建立起来	 
    commute();

    pthread_cancel(tid);
    return 0;
}


//远程聊天-客户端
int commute()
{
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0); //使用ipv4，字节流tcp协议
	if(tcp_socket == -1)
	{
		printf("open socket failed \n");
	}
	
	//存储服务端IP和端口号
	struct sockaddr_in addr_ser;
	addr_ser.sin_family = AF_INET; //使用ipv4
    addr_ser.sin_addr.s_addr = inet_addr("192.168.78.1"); //绑定IP	
	addr_ser.sin_port = htons(1111); //绑定端口，端口从主机字节顺序转变成网络字节顺序，即小端模式变为大端模式
	
	
	socklen_t len = sizeof(addr_ser); 
	connect(tcp_socket, (struct sockaddr *)&addr_ser, len); //给服务器发送连接请求
	
    //摩斯密码
	char mos[20][6];
	int i = 0, j = 0;

    //初始化
	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 6; j++)
		{
			mos[i][j] = ' ';
		}
	}

	
	int letter_cout = 0; //密码个数
	int bit_count = 0; //每个密码的位数
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
		else if (x > 525 && x < 675 && y > 400 && y < 440) //‘/’表示输入下一个密码
		{
			if (letter_cout == 20) continue;
			letter_cout++;
			bit_count = 0;
		}

		else if (x > 125 && x < 355 && y > 315 && y < 355) //重置
		{
			for (i = 0; i < 20; i++)
            {
                for (j = 0; j < 6; j++)
                {
                    mos[i][j] = ' ';
                }
            }
			letter_cout = 0;
			bit_count = 0;
		}
		else if (x > 445 && x < 675 && y > 315 && y < 355) //发送
		{
			write(tcp_socket, mos, sizeof(mos));
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


//远程聊天-服务端
void* server()
{
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_socket == -1)
	{
		printf("open socket failed \n");
	}

	//存储本地IP和端口号
	struct sockaddr_in  addr_ser, addr_clent;
	addr_ser.sin_family = AF_INET;		
	addr_ser.sin_addr.s_addr = inet_addr("192.168.78.4"); //IP
	addr_ser.sin_port = htons(6666); //端口 
	
	//绑定本地url
	socklen_t len = sizeof(addr_ser);
	bind(tcp_socket, (struct sockaddr *)&addr_ser, len);

	//限制监听人数 同时最多处理五个客户端请求 与cpu处理器、内存等有关
	listen(tcp_socket, 5);

	//等待客户端发送请求 接收请求后创建fd用于通信
	len = sizeof(addr_clent);
	bzero(&addr_clent, len);
	int new_fd = accept(tcp_socket, (struct sockaddr *)&addr_clent, &len);
	if(new_fd == -1)
	{
		printf("accept failed \n");
	}

	char ipbuf[64];
	bzero(ipbuf,64);
	char buf[128];

	printf("connect succeed \n"); 
    //打印客户端的IP和端口号，将网络传输的二进制数值转化为成点分十进制的ip地址，端口从网络字节顺序转变成主机字节顺序，即大端模式变为小端模式
	printf("clent ip is %s\t port is %uh\n", inet_ntop(AF_INET, &addr_clent.sin_addr, ipbuf, 64), ntohs(addr_clent.sin_port));
	
	while(1)
	{
        sleep(1);
        bzero(buf, 128);		
        read(new_fd, buf, 128); //读取客户端发来的信息
        show_font(buf, 32);
        printf("from chatter1 : %s \n", buf);
	}

    
	close(tcp_socket);
	close(new_fd);
	return;
}

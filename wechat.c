#include <stdio.h>
#include <string.h>
#include <strings.h> 
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>        
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>

//客户端
void* client()
{
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_socket == -1)
	{
		printf("open socket failed\n");
	}
	
	// 定义结构体存储服务端IP和端口号
	struct sockaddr_in  addr_ser;
	addr_ser.sin_family = AF_INET;	
	addr_ser.sin_port = htons(1111);	// 绑定端口 htons把整形数据装换成网间数据
	addr_ser.sin_addr.s_addr = inet_addr("169.254.231.1"); // 绑定IP
	
	socklen_t len = sizeof(addr_ser);
	// 给服务器发送连接请求 
	connect(tcp_socket, (struct sockaddr *)&addr_ser, len);
	
	// 发送数据
	char buf[128];
	while(1)
	{
		bzero(buf, 128);
		fgets(buf, 128, stdin);
		
		write(tcp_socket, buf,strlen(buf));
	}

	close(tcp_socket);
}

//服务端
void* server()
{
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_socket == -1)
	{
		printf("open socket failed\n");
	}

	// 定义结构体存储本地IP和端口号
	struct sockaddr_in  addr_ser, addr_clent;
	addr_ser.sin_family = AF_INET;		
	addr_ser.sin_port = htons(6666);	// 绑定本地端口 htons把整形数据装换成网间数据
	addr_ser.sin_addr.s_addr = inet_addr("169.254.231.28"); // 绑定本地IP
	
	// 绑定本地url
	socklen_t len = sizeof(addr_ser);
	bind(tcp_socket , (struct sockaddr *)&addr_ser, len);

	// 限制监听人数 同时最多处理五个客户端请求  cpu处理器 内存等有关
	listen(tcp_socket, 5);

	// 等待别人发送请求，接收请求后创建一个fd用于通信
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

	//打印客户端的IP和端口号
	printf("connect wechat2 succeed. \n");
	printf("clent ip is %s\t port is %uh\n", inet_ntop(AF_INET, &addr_clent.sin_addr, ipbuf, 64),  ntohs(addr_clent.sin_port));
	

	while(1)
	{
		bzero(buf,128);		
		read(new_fd, buf, 128);
		printf("from wechat2 : %s\n", buf);
	}

	// 关闭函数
	close(tcp_socket);
	close(new_fd);
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, server, NULL);
	sleep(5);
    client();

	pthread_cancel(tid);
    return 0;
}
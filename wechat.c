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

void* client();
void* server();

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, server, NULL);
	sleep(5);
    client();

	pthread_cancel(tid);
    return 0;
}


//远程聊天-客户端
void* client()
{
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_socket == -1)
	{
		printf("open socket failed\n");
	}
	
	//存储服务端IP和端口号
	struct sockaddr_in addr_ser;
	addr_ser.sin_family = AF_INET;	
	addr_ser.sin_addr.s_addr = inet_addr("192.168.78.4"); //绑定IP	
	addr_ser.sin_port = htons(6666); //绑定端口，端口从主机字节顺序转变成网络字节顺序，即小端模式变为大端模式	
	
	socklen_t len = sizeof(addr_ser);
	connect(tcp_socket, (struct sockaddr *)&addr_ser, len); //给服务器发送连接请求 
	
	// 发送数据
	char buf[128];
	while(1)
	{
		bzero(buf, 128);
		fgets(buf, 128, stdin);
		
		write(tcp_socket, buf, strlen(buf));
	}

	close(tcp_socket);
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
	struct sockaddr_in addr_ser, addr_clent;
	addr_ser.sin_family = AF_INET;	
	addr_ser.sin_addr.s_addr = inet_addr("192.168.78.1"); //IP
	addr_ser.sin_port = htons(1111);	//端口
	
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
	bzero(ipbuf, 64);
	char buf[128];

	printf("connect succeed \n");
	//打印客户端的IP和端口号，将网络传输的二进制数值转化为成点分十进制的ip地址，端口从网络字节顺序转变成主机字节顺序，即大端模式变为小端模式
	printf("clent ip is %s\t port is %uh\n", inet_ntop(AF_INET, &addr_clent.sin_addr, ipbuf, 64), ntohs(addr_clent.sin_port));

	while(1)
	{
        sleep(1);
		bzero(buf, 128);		
		read(new_fd, buf, 128); //读取客户端发来的信息
        printf("from chatter2 : %s \n", buf);
	}


	close(tcp_socket);
	close(new_fd);
	return 0;
}


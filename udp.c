#include <stdio.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
 
/** Recv Message **/
void * recv_msg(void *arg);
 
int main(int argc,char *argv[])
{

	if(argc != 2)
	{
		printf("Please provide a port number!\nUsage: [exe] [port#]\n");
		return -1;
	}
 
	int port = atoi(argv[1]);
    /** Port number <= 1024 is usually allocated by the system
     *  Port number is up to 65535
    **/
	if( port<1025 || port>65535 ) 
	{
		printf("Port number should be ranged between 1025 -- 65535\n");
		return -1;
	}
	
	/** Create a socket for UDP **/
	int udp_socket_fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(udp_socket_fd < 0 )
	{
		perror("creat socket fail\n");
		return -1;
	}
 
	// Set UDP IP Address and Port # 
	struct sockaddr_in  local_addr = {0};
	local_addr.sin_family  = AF_INET; // Use IPv4
	local_addr.sin_port	= htons(port);   
	local_addr.sin_addr.s_addr = INADDR_ANY; //
 
	int ret = bind(udp_socket_fd,(struct sockaddr*)&local_addr,sizeof(local_addr));
	if(ret < 0)
	{
		perror("bind fail:");
		close(udp_socket_fd);
		return -1;
	}
	
	// Create a thread for recv
	pthread_t recv_thread;
	pthread_create(&recv_thread, NULL, recv_msg, (void*)&udp_socket_fd);
	
	
	// Set destination IP adress 
    struct sockaddr_in dest_addr = {0};
    dest_addr.sin_family = AF_INET;// Use IPv4
    
	int dest_port = 0;// Dest Port 
	char dest_ip[32] = {0}; // Dest IP
	char msg[1024] = {0};
	
	//looping 
	while(1)
	{
		printf("ip port msg\n");
 
		scanf("%s %d %s", dest_ip, &dest_port, msg);
		dest_addr.sin_port = htons(dest_port);// Set destination Port
		dest_addr.sin_addr.s_addr = inet_addr(dest_ip); //Set destination IP 
		
		sendto(udp_socket_fd, msg, strlen(msg), 0, (struct sockaddr *)&dest_addr,sizeof(dest_addr)); 
        if(strcmp(msg, "exit") == 0 || strcmp(msg, "") == 0)
		{
			pthread_cancel(recv_thread);
			break;
		}
		memset(msg,0,sizeof(msg));
		memset(dest_ip,0,sizeof(dest_ip));
	}

	// Close the Port
	close(udp_socket_fd);
}
 
void * recv_msg(void *arg)
{
	int ret = 0;
	int *socket_fd = (int *)arg;
	struct sockaddr_in  src_addr = {0};  
	int len = sizeof(src_addr);	
	char msg[1024] = {0};
	
	while(1)
	{
		ret = recvfrom(*socket_fd, msg, sizeof(msg), 0, (struct sockaddr *)&src_addr, &len);
		if(ret == -1)
		{
			break;
		}
		printf("[%s:%d]",inet_ntoa(src_addr.sin_addr),ntohs(src_addr.sin_port));
		printf("msg=%s\n",msg);
		if(strcmp(msg, "exit") == 0 || strcmp(msg, "") == 0)
		{
			break;
		}
		memset(msg, 0, sizeof(msg));
 
	}
	close(*socket_fd);
	return NULL;
}

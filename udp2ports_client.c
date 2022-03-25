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

#define LOCAL_IP_STRING  "127.0.0.1"
#define MAX_BUFFER       60000

uint8_t buffer[MAX_BUFFER];

int main(int argc, char const *argv[])
{
    
    if(argc != 5)
    {
        printf("Usage: [exe] [--r/s] [Port# 1] [--r/s] [Port# 2]\n");
        return -1;
    }

    if( (!strcmp(argv[1],"--r") && strcmp(argv[3],"--s")) ||
        (!strcmp(argv[1],"--s") && strcmp(argv[3],"--r")) ||
        (!strcmp(argv[3],"--r") && strcmp(argv[1],"--s")) ||
        (!strcmp(argv[3],"--s") && strcmp(argv[1],"--r")))
    {
        printf("Incorrect Flags:\n");
        printf("\t--r : recv port \n");
        printf("\t--s : send port \n");
        return -1;
    }

    uint16_t recv_port = 0;
    uint16_t send_port = 0;
    if(!strcmp(argv[1], "--r"))
    {
        recv_port = atoi(argv[2]);
        send_port = atoi(argv[4]);
    }
    else
    {
        recv_port = atoi(argv[4]);
        send_port = atoi(argv[2]);
    }

    if(recv_port == send_port)
    {
        perror("recv_port == send_port\n");
        return -1;
    }

	if( recv_port<1025 || recv_port>65535 ) 
	{
		printf("Recv Port number should be ranged between 1025 -- 65535\n");
		return -1;
	}

    if( send_port<1025 || send_port>65535 ) 
	{
		printf("Send Port number should be ranged between 1025 -- 65535\n");
		return -1;
	}

    printf("Recv Port: %d, Send Port: %d\n", recv_port, send_port);

    /** Create a recv socket **/
    int udp_recv_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(udp_recv_socket_fd < 0 )
	{
		perror("creat recv socket fail\n");
		return -1;
	}

	// Set UDP IP Address and Port # 
	struct sockaddr_in  recv_addr = {0};
	recv_addr.sin_family  = AF_INET; // Use IPv4
	recv_addr.sin_port	= htons(recv_port);   
	recv_addr.sin_addr.s_addr = inet_addr(LOCAL_IP_STRING);

    /** Create a send socket **/
    int udp_send_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(udp_send_socket_fd < 0 )
	{
		perror("creat send socket fail\n");
		return -1;
	}

	// Set UDP IP Address and Port # 
	struct sockaddr_in  send_addr = {0};
	send_addr.sin_family  = AF_INET; // Use IPv4
	send_addr.sin_port	= htons(send_port);   
	send_addr.sin_addr.s_addr = inet_addr(LOCAL_IP_STRING);


    // Set UDP IP Address and Port # for src address
	struct sockaddr_in  src_addr = {0};
	src_addr.sin_family  = AF_INET; // Use IPv4

    memset(buffer, 0, MAX_BUFFER);

    int num_sent = 0;
    int num_recv = 0;

	unsigned int len = sizeof(src_addr);

    while(1)
    {
		printf("Please input msg:");
        scanf("%s", (char *)buffer); /* get a msg from STDIN */
        if(strcmp((char *)buffer, "exit") == 0)
		{
			break;
		}
        num_sent = sendto(udp_send_socket_fd, buffer, strlen((char *)buffer), 0, (struct sockaddr *)&send_addr,sizeof(send_addr)); 

		printf("[%s:%d]",inet_ntoa(send_addr.sin_addr),ntohs(send_addr.sin_port));
        printf("num_sent: %d\n", num_sent);

        // num_sent = sendto(udp_recv_socket_fd, buffer, strlen((char *)buffer), 0, (struct sockaddr *)&recv_addr,sizeof(recv_addr)); 

		// printf("[%s:%d]",inet_ntoa(recv_addr.sin_addr),ntohs(recv_addr.sin_port));
        // printf("num_sent: %d\n", num_sent);

        num_recv = recvfrom(udp_send_socket_fd, (char *)buffer, MAX_BUFFER, 0, (struct sockaddr *)&src_addr, &len);
        buffer[num_recv] = '\0';

        printf("From the server: %s\n", buffer);
        memset(buffer, 0, MAX_BUFFER);

        sleep(1);
    }
    

    close(udp_recv_socket_fd);
    close(udp_send_socket_fd);

    return 0;
}
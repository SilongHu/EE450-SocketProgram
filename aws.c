// A AWS server with TCP and UDP in the internet domain 
// Created by SilongHu on 11/13/16 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>

#define Server_A_UDP_PORT 21371
#define Server_B_UDP_PORT 22371
#define Server_C_UDP_PORT 23371
#define AWS_UDP_PORT 24371
#define AWS_TCP_PORT 25371

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main()
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[3];
    struct sockaddr_in serv_addr, cli_addr;
	int n;
	printf("The AWS is up and running.\n");

//************** TCP Establish and receive reduction ********************
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(AWS_TCP_PORT);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
     	listen(sockfd,5);
     	clilen = sizeof(cli_addr);
     	newsockfd = accept(sockfd, 
            (struct sockaddr *) &cli_addr, 
            &clilen);
    if (newsockfd < 0) error("ERROR on accept");
     	bzero(buffer,4);
     	n = read(newsockfd,buffer,sizeof(buffer));
     	if (n < 0) error("ERROR reading from socket");
//****************TCP end*************

// Here we assign the UDP information
//This part is modified from http://www.linuxhowtos.org/C_C++/socket.htm	
	int sock_udp, n_udp;
	unsigned int length_udp;
	struct sockaddr_in server_C, from_C, server_A, from_A, server_B, from_B;
	struct hostent *hp;
	char buffer_udp[256];	

	sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_udp < 0) error("socket");
	server_C.sin_family = AF_INET;
	hp = gethostbyname("localhost");
	if(hp == 0) error("Unknown host");
	bcopy((char *)hp->h_addr,
		(char *)&server_C.sin_addr,
		hp->h_length);
	server_C.sin_port = htons(Server_C_UDP_PORT);
	length_udp = sizeof(struct sockaddr_in);
	bzero(buffer_udp,256);
	
	server_A.sin_family = AF_INET;
	bcopy((char *)hp -> h_addr,
		(char *)&server_A.sin_addr,
		hp -> h_length);
	server_A.sin_port = htons(Server_A_UDP_PORT);

	server_B.sin_family = AF_INET;
	bcopy((char *)hp -> h_addr,
		(char *)&server_B.sin_addr,
		hp -> h_length);
	server_B.sin_port = htons(Server_B_UDP_PORT);


// Receive the total counts and initial the array
	int received_counts = 0;
	n = read(newsockfd,&received_counts,sizeof(received_counts));
    if (n < 0 ) error("Error reading from socket");

    int received_nums[received_counts];
	int size = sizeof(received_nums);
    n = read(newsockfd,&received_nums,sizeof(received_nums));
    if (n < 0 ) error("Error reading from socket");
    printf("The AWS recevied %d numbers from the client using TCP over port %d \n",received_counts,htons(serv_addr.sin_port));
	
// Received the number and divide them into 3 parts

	int server_A_nums[received_counts/3];
	int server_B_nums[received_counts/3];
	int server_C_nums[received_counts/3];

	int divide_index = 0;
	int index_B = 0;
	int index_C = 0;
	for (; divide_index < received_counts/3; divide_index++){
		server_A_nums[divide_index] = received_nums[divide_index];
	}
	for (; divide_index < 2 * received_counts/3; divide_index++){
		server_B_nums[index_B++] = received_nums[divide_index];
	}
	for (; divide_index < received_counts ; divide_index++){
		server_C_nums[index_C++] = received_nums[divide_index];
	}
	int N = received_counts/3;
	
//Dealing with A:

	n_udp = sendto(sock_udp,&(N),sizeof(N),0,(const struct sockaddr *)&server_A,length_udp);
	if (n_udp < 0 ) error("sendto");

	n_udp = sendto(sock_udp,server_A_nums,sizeof(server_A_nums),0,(const struct sockaddr *)&server_A,length_udp);
	if (n_udp < 0 ) error("sendto");
	printf("The AWS has sent %d numbers to Backend-Server A\n",received_counts/3);
	
	n_udp = sendto(sock_udp,buffer,sizeof(buffer),0,(const struct sockaddr *)&server_A,length_udp);
	if (n_udp < 0 ) error("sendto");
	int result_A = 0;
	n_udp = recvfrom(sock_udp,&result_A,sizeof(result_A),0,(struct sockaddr *)&from_A,&length_udp);
	if(n_udp < 0) error("recvfrom");
	
//Dealing with B
	n_udp = sendto(sock_udp,&(N),sizeof(N),0,(const struct sockaddr *)&server_B,length_udp);
	if (n_udp < 0 ) error("sendto");

	n_udp = sendto(sock_udp,server_B_nums,sizeof(server_B_nums),0,(const struct sockaddr *)&server_B,length_udp);
	if (n_udp < 0 ) error("sendto");
	printf("The AWS has sent %d numbers to Backend-Server B\n",received_counts/3);
	
	n_udp = sendto(sock_udp,buffer,sizeof(buffer),0,(const struct sockaddr *)&server_B,length_udp);
	if (n_udp < 0 ) error("sendto");
	int result_B = 0;
	n_udp = recvfrom(sock_udp,&result_B,sizeof(result_B),0,(struct sockaddr *)&from_B,&length_udp);
	if(n_udp < 0) error("recvfrom");

// Dealing with Server C, send data first, then opearion
	n_udp = sendto(sock_udp,&(N),sizeof(N),0,(const struct sockaddr *)&server_C,length_udp);
	if (n_udp < 0 ) error("sendto");

	n_udp = sendto(sock_udp,server_C_nums,sizeof(server_C_nums),0,(const struct sockaddr *)&server_C,length_udp);
	if (n_udp < 0 ) error("sendto");
	printf("The AWS has sent %d numbers to Backend-Server C\n",received_counts/3);
	
	n_udp = sendto(sock_udp,buffer,sizeof(buffer),0,(const struct sockaddr *)&server_C,length_udp);
	if (n_udp < 0 ) error("sendto");
	int result_C = 0;
	n_udp = recvfrom(sock_udp,&result_C,sizeof(result_C),0,(struct sockaddr *)&from_C,&length_udp);
	if(n_udp < 0) error("recvfrom");

// A Operation print:

	printf("The AWS received reduction result of %s from Backend-Server A using UDP over port %d and it is %d\n",buffer,htons(from_A.sin_port),result_A);

// B Operation print:

	printf("The AWS received reduction result of %s from Backend-Server B using UDP over port %d and it is %d\n",buffer,htons(from_B.sin_port),result_B);

// C Operation print:

	printf("The AWS received reduction result of %s from Backend-Server C using UDP over port %d and it is %d\n",buffer,htons(from_C.sin_port),result_C);

//Dealing with the recevied numbers from Server A B & C
	int sum_send = 0,show = 0;
	if(strcmp(buffer,"sum")==0 ||strcmp(buffer,"sos")==0){
		sum_send  = htonl(result_C + result_A + result_B);
		show = result_C + result_A + result_B;
	}
	else if (strcmp(buffer,"max")==0){
		show = result_C > result_A ?(result_C > result_B ? result_C:result_B):(result_A > result_B ? result_A:result_B);
		sum_send = htonl(show);
	} 
	else if (strcmp(buffer,"min")==0){
		show = result_C < result_A ?(result_C < result_B ? result_C:result_B):(result_A < result_B ? result_A:result_B);
		sum_send = htonl(show);
	}
	else{printf("Input Error\n");}
	printf("The AWS has successfully finished the reduction %s : %d \n",buffer,show);

     n = write(newsockfd,&sum_send,sizeof(sum_send));
     if (n < 0) error("ERROR writing to socket");
	printf("The AWS has successfully finished sending the reduction value to client.\n");

	close(newsockfd);
	close(sockfd);
	close(sock_udp);
    return 0; 
}


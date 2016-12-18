// Created by SilongHu on 11/13/16 

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <stdio.h>

#define Server_C_UDP_PORT 23371

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main()
{
   int sock, length, n;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[1024];

   //****************UDP Establish **************
   //This part is modified from http://www.linuxhowtos.org/C_C++/socket.htm	
   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(Server_C_UDP_PORT);
   if (bind(sock,(struct sockaddr *)&server,length)<0) 
       error("binding");
   fromlen = sizeof(struct sockaddr_in);

	printf("The Server C is up and running using UDP on port %d\n",htons(server.sin_port));

// Received the total counts of numbers and the actual numbers
	int recv_counts = 0;
	n = recvfrom(sock,&recv_counts,sizeof(recv_counts),0,(struct sockaddr *)&from,&fromlen);
	if (n < 0) error("recvfrom");

	int recvnum[recv_counts];
	int size = sizeof(recvnum);
	n = recvfrom(sock,recvnum,size,0,(struct sockaddr *)&from,&fromlen);

    if (n < 0) error("recvfrom");

	printf("The server C has received %d numbers.\n",recv_counts);

//	Received the reduction and operating corresponding to that
	char op[3];
	n = recvfrom(sock,op,sizeof(op),0,(struct sockaddr *)&from,&fromlen);
	if (n < 0) error("recvfrom");
	int sum = recvnum[0];
	//Make sure print correctly, this part used from 450-Piazza Discussion session
	char type[4];
	strncpy(type,op,4);
	type[3] = 0;
	if (strcmp(type,"sum")==0){
	int i;
	for (i = 1; i < recv_counts ;i++)
		sum += recvnum[i];
	}
	else if (strcmp(type,"max")==0){
	int j;
	for (j = 1; j < recv_counts;j++)
		if(sum < recvnum[j]) sum = recvnum[j];
	}
	else if (strcmp(type,"min")==0){
	int l;
	for (l = 1; l < recv_counts;l++)
		if(sum > recvnum[l]) sum = recvnum[l];
	}
	else if (strcmp(type,"sos")==0){
	sum = 0;
	int m;
	for (m = 0; m < recv_counts;m++)
		sum +=  recvnum[m]*recvnum[m];
	}
	else{printf("Input Error!\n");}

	printf("The Server C has successfully finished the reduction %s : %d.\n",type,sum);
    n = sendto(sock,&sum,sizeof(sum),0,(struct sockaddr *)&from,fromlen);
    if (n  < 0) error("sendto");
	printf("The Server C has successfully finished sending the reduction value to AWS server.\n");

   return 0;
 }


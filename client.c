// Created by SilongHu on 11/13/16.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ctype.h>
#include <strings.h>

#define AWS_TCP_PORT 25371

void error(const char *msg)
{
    	perror(msg);
    	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	printf("The client is up and running.\n");
	
	//Buffer stores the nums.csv, reduction stores the function
    char buffer[1024];
    char reduction[3];

    strcpy(reduction,argv[1]);

    if (argc < 2) {
       	fprintf(stderr,"Please input the reduction type \n");
       	exit(0);
    }
	
	//Lowercase the input, if the result is not sum/min/max/sos, exit.
	int i;
	for(i = 0; i < 3; i++){
		reduction[i] = tolower(reduction[i]);
	}
	if(strcmp(reduction,"max")!=0 && strcmp(reduction,"min")!=0 && strcmp(reduction,"sum")!=0 && strcmp(reduction,"sos")!=0){
	printf("Please input the correct function! (min/max/sum/sos)\n");
	exit(0);
	}

//*********** Sending Reduction Type to AWS Using TCP*************
//*********** This part is modified from http://www.linuxhowtos.org/data/6/client.c *********
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)  error("ERROR opening socket");
    	server = gethostbyname("localhost");
    	if (server == NULL) {
        	fprintf(stderr,"ERROR, no such host\n");
        	exit(0);
		}
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
    serv_addr.sin_port = htons(AWS_TCP_PORT);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)  error("ERROR connecting");  	
	n = write(sockfd,reduction,strlen(reduction));
    if (n < 0) error("ERROR writing to socket");
	printf("The client has sent the reduction type %s to AWS.\n",reduction);

	
//**************Now reading the nums.csv **************
//First iteration to get the total number, Second for store.
	FILE *file = fopen("nums.csv", "r");
 
	if (!file) {
		fprintf(stderr, "Can't open file.\n");
		exit(EXIT_FAILURE);
	}
  	int numbers = 0;
  	while (fgets(buffer, sizeof buffer, file) != NULL)
		numbers++;
	fclose(file);
	int index = 0;
	int nums[numbers];
	FILE *file_again = fopen("nums.csv", "r");
 
	if (!file_again) {
		fprintf(stderr, "Can't open file.\n");
		exit(EXIT_FAILURE);
	}
	while (fgets(buffer, sizeof buffer, file_again) != NULL){
		nums[index++] = atoi(buffer);
	}
	fclose(file_again);


//***********Sending the numbers(300 in this case) firstly, then send integer array ***********
    n = write(sockfd,&numbers,sizeof(numbers));
    if (n < 0) error("ERROR writing to socket");

    n = write(sockfd,nums,sizeof(nums));
    if (n < 0) error("ERROR writing to socket");
	printf("The client has sent %d numbers to AWS.\n",numbers);

// The result of reduction
    int sum_rev = 0;

    n = read(sockfd,&sum_rev,sizeof(sum_rev));
    if (n < 0) error("ERROR reading from socket");
	
//Make sure print correctly, this part used from 450-Piazza Discussion session
	char type[4];
	strncpy(type,reduction,4);
	type[3] = 0;
	
	printf("The client has received reduction %s : %d.\n",type,ntohl(sum_rev));

    close(sockfd);
    return 0;
}

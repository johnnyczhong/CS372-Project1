/*
 * Programmer Name: Johnny Zhong
 * Program Name: chatclient.c
 * Program Description: A chat client that communicates with a server.
 * Course Name: CS372 - Introduction to Networks
 * Last Modified: 2019-02-04
** Citations: modeled after beej's guide to network programming
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define MAXDATASIZE 1024 // max number of bytes we can get at once 


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


// initiates the connection to the server
void initiate_connection(int sockfd, char* username)
{
	char initial_msg[MAXDATASIZE];
	sprintf(initial_msg, "%s%s", "Connecting to server. Username: ", username);
	send(sockfd, initial_msg, strlen(initial_msg), 0);
}


// function to send messages. returns 1 if client initiated close
int send_msg(int sockfd, char* username)
{
	char out_msg[MAXDATASIZE];
	char out_buffer[MAXDATASIZE];

	printf("Outgoing msg: ");
	fgets(out_buffer, MAXDATASIZE, stdin);
	out_buffer[strlen(out_buffer) - 1] = '\0'; // replace newline

	// append handle to message
	sprintf(out_msg, "%s> %s", username, out_buffer);

	// send message to server
	if (send(sockfd, out_msg, strlen(out_msg), 0) == -1) {
		perror("send");
		exit(1);
	}

	// detect if client initiated closing connection
	if (strcmp(out_buffer, "\\quit") == 0) {
		return 1;
	}

	return 0;

}


// function to receive messages. returns 1 if server requested to close
int recv_msg(int sockfd)
{
	int numbytes;
	char buf[MAXDATASIZE];

	// receive message from server
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}
	buf[numbytes] = '\0';

	// print server message to screen
	printf("%s\n",buf);

	// detect server closing connection
	if (strcmp(buf, "Server> \\quit") == 0) {
		return 1;
	}

	return 0;

}


int main(int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	char username[10];

    // char* out_buffer = calloc(MAXDATASIZE - 10, sizeof(char));
    // char* out_msg = calloc(MAXDATASIZE, sizeof(char));

	if (argc != 3) {
	    fprintf(stderr,"usage: server address and port number\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	// get username
	printf("Provide username: ");
	fgets(username, 10, stdin);
	username[strlen(username) - 1] = '\0'; // replace newline

	initiate_connection(sockfd, username);

	do
	{

		if (send_msg(sockfd, username) == 1){
			printf("Closing connection.\n");
			break;
		}
			
		if (recv_msg(sockfd) == 1){
			printf("Server closed connection.\n");
			break;
		}

	} while (1);

	close(sockfd);

	return 0;
}

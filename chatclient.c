/*
** client.c -- modeled after beej's guide to network programming
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

#define PORT "12000" // the port client will be connecting to 

#define MAXDATASIZE 1024 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	char username[10];

    char* out_buffer = calloc(MAXDATASIZE - 10, sizeof(char));
    char* out_msg = calloc(MAXDATASIZE, sizeof(char));


	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
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
	send(sockfd, username, strlen(username), 0);

	do
	{
		// get user's message to send
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
			printf("Closing connection.\n");
			break;
		}

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
			printf("Server closed connection.\n");
			break;
		}

	} while (1);

	close(sockfd);

	return 0;
}

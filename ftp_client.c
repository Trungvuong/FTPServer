#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

int main(int argc, char** argv)
{

	char portNum[16]; //port number
	char ipNum[16];   // ip number

	printf("Enter port number: "); //asks user for port number
	fgets(portNum, 16, stdin);
	
	// Checking port number size and argument
	if((atoi(portNum) > 65535) || (atoi(portNum) < 1) ){
		printf("Invalid port number.\n");
		return 1;
	}
	
	printf("Enter an IP number: "); // asks user to enter IP number
	fgets(ipNum, 16, stdin);
	
	
	int counter = 0;
	char *temp = malloc(16);
	char *tempIP = malloc(16);
	strcpy(tempIP, ipNum); //copies the IP number entered
	temp = strtok(tempIP, "."); // sets the ip number in proper format

	/**
	 * The temp variable will become null when the last token is 
	 * read or found.
	 */
	while(temp != NULL)
	{
		counter++;
		int i = 0;
		
		// checks for valid numbers in ip address
		while(i < strlen(temp) - 1)
		{
			// makes sure section is a number
			if (isdigit(temp[i])){  
				i++;
			}

			// if the section of the IP address is not a number
			else{
				printf("Not a valid IP Number.\n");
				return 1;
			}
		}
		// Each section should have a number greater than 0 and less than 255
		if((atoi(temp)) > 255 || atoi(temp) < 0){
			printf("Not a valid IP Number.\n");
			return 1;
		}
		temp = strtok(NULL, "."); // adds a dot for the section
	}

	// Should be 4 sections of numbers in an IP number.
	if (counter != 4){
		printf("Not a valid IP Number. Wrong number of sections.\n");

		free(temp);  // frees the allocated memory
		free(tempIP);
		return 1;
	}

	else{
		free(tempIP); //frees memory
		free(temp);
	}
	// Creating a socket
	int sockfd = socket(AF_INET,SOCK_STREAM,0);

	// error check for creating socket
	if(sockfd < 0){
		printf("Error. Could not create socket.\n");
		return 1;
	}

	// Setting up the socket address
	struct sockaddr_in serveraddr;
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(atoi(portNum));
	serveraddr.sin_addr.s_addr=inet_addr(ipNum);	

	int e = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	/* Check for errors with socket address*/
	if(e < 0){
		printf("There was an error with connecting\n");
		return 1;
	}
	// Buffers used in the loop
	char *line = malloc(5000);
	char *bufferSize = malloc(sizeof(char) * 10);
	char *buffer = malloc(sizeof(char) * 512);
	char *okBuffer = malloc(sizeof(char)* 1);
	
	while(1){
		
		printf("Enter the file path, or exit: ");
		fgets(line, 5000, stdin);
		if(strncmp(line, "exit", 4) == 0){
			printf("Leaving server.\n");
			send(sockfd, line, strlen(line), 0);

			// free allocated memory
			free(line);
			free(bufferSize);
			free(buffer);
			free(okBuffer);

			close(sockfd); //close socket
			return 0;
		}

		//Eliminating new line from end of input - makes finding file easier on server
		line = strtok(line, "\n");
	
		// Sending file path over socket
		send(sockfd, line, strlen(line), 0);
		
		// Buffer for response if file found on server
		// Check if file exists before creating anything
		recv(sockfd, okBuffer, 1, 0);
		if(strcmp(okBuffer, "1") == 0){
			printf("File not found\n");
			continue;
		}
	
	
		// Getting size of file from server
		int size = 0;
		int test = recv(sockfd, bufferSize, 10, 0);

		if(test < 0){
			printf("Error getting size.\n");
			close(sockfd);
			return 1;
		}
		size = atoi(bufferSize);

		// File 
		FILE *file = fopen(line, "w");
		
		//Need to receive in loop, as server will send in packets (either implicitly or explicitly)
		int read;
		int bytes = size;
		while(bytes > 0)
		{
			read = recv(sockfd, buffer, 512, 0);

			// Only write what was received, not extra bytes.
			fwrite(buffer, sizeof(char), read, file);
			bytes -=  read;
	
		}
		printf("Done copying file.\n");
	}
	return 0;
}                                                                                                                                                        169       1,1           Top

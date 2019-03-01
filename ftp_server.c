/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>

void error(char* msg);
char* list();
int retrieve(char* filename);
int store(char* filename, int newsockfd);
int sendMSG();
int writeClient(char* s, int newsockfd, int fileLeng);
char target[256];

void* sockThread(void* sock);
void sigHandC(int);

//socket info
int sockfd, portno, clilen;

//variable to transfer files
char buffer[256];

struct sockaddr_in serv_addr, cli_addr;
int n;

int main(int argc, char *argv[])
{
    int* newsockfd;
    int status;
    pthread_t newThread;
    signal(SIGINT, sigHandC);


    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    //creating socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    //binding socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    //listening
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    //accepting connection
    while(1){
        newsockfd = malloc(sizeof(int));
        *newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if (*newsockfd < 0)
            error("ERROR on accept");

        if ((status = pthread_create(&newThread, NULL, sockThread, newsockfd)) != 0) {
            fprintf(stderr, "thread create error %d: %s \n", status, strerror(status));
            free(newsockfd);
            exit(1);
        }
    }
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

char* list(){

    //char *tmp = "";
    struct dirent *de;// Pointer for directory entry

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir(".");

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return NULL;
    }

    while ((de = readdir(dr)) != NULL) {
        //printf("Trying to concat\n");
        //printf("%s\n", de->d_name);
        strcat(target, de->d_name);
        strcat(target, " ");
    }

    closedir(dr);
    return target;
}

int retrieve(char* filename){

    char max[100000];
    //printf("About to open\n");
    FILE *fp = fopen(filename, "r");

    size_t newLen = 0;
    if (fp != NULL) {
        newLen = fread(buffer, sizeof(char), 100000, fp);
        if ( ferror( fp ) != 0 ) {
            fputs("Error reading file", stderr);
        } else {
            max[newLen++] = '\0'; /* Just to be safe. */
        }

        fclose(fp);
    }

    //printf("Put file in buffer\n");
    return (int)newLen;
}

int store(char* filename, int newsockfd){
    //printf("About to open file\n");
    //printf("About to open: %s\n", filename);
    filename = strtok(filename, "\n");

    //TEST

    char *temp = "../clientDir/";

    char *actualFile = (char *) malloc(1 + strlen(filename)+ strlen(temp) );
    strcpy(actualFile, temp);
    strcat(actualFile, filename);

    //printf("About to open: %s\n", actualFile);
    //actualFile = strcat(actualFile, filename);
    //actualFile = strtok(actualFile, "\n");
    //printf("About to open: %s\n", actualFile);

    FILE *reading, *writing;
    char c;

    reading = fopen(actualFile, "r");
    if(reading == NULL){
        printf("ERROR read\n");
    }

    //printf("Opened\n");

    writing = fopen(filename, "w");
    if(reading == NULL){
        printf("ERROR writing\n");
    }

    //printf("Opened\n");

    // Read contents from file
    //c = fgetc(reading);
    int i = 0;
    while (1)
    {
        c = fgetc(reading);
        fputc(c, writing);

        i++;
        if(i > 79)
            break;
    }

    //printf("\nContents copied to %s", filename);

    fclose(reading);
    fclose(writing);
    return 0;
}



    //TEST
    /*
    
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }


    sleep(1);

    int total = 0;
    int bytesLeft = sizeof(buffer); // how many bytes left to send
    int bytesSent = 1;
    int temp;
    int first = 1;
    int size = 1;
    //how many bytes sent

    //reading from client

    printf("BUFFER BEFORE STORE: %s\n", buffer);
    //read(*(int*) sockThread, buffer, sizeof(buffer));
    while(total < size) {

        printf("BUFFER BEFORE READ: %s\n", buffer);
        //bytesSent = read(newsockfd, buffer, sizeof(buffer));
        //if (bytesSent == -1) {
            //error("ERROR reading from socket");
            //break;
        //}
        if(first){
            size = (int)buffer[79];
            first = 0;
        }

        //write file here
        printf("Printing to file: %s\n", buffer);
        fwrite(buffer, sizeof(char), sizeof(buffer), fp );
        printf("Printed to file: %s\n", buffer);

        total += bytesSent;
        temp = bytesSent;
        bytesLeft -= bytesSent;
        printf("Inwhile\n");
    }

    fclose(fp);
    bzero(buffer, 256);
    printf("Exited store while\n");
    return total;

} 
int sendMSG(){

    printf("Here is the message: %s\n", buffer);
    return 0;
}*/

int writeClient(char* s, int newsockfd, int fileLeng){
    int ssize = strlen(s);
    char* temp = malloc(ssize + sizeof(s)*sizeof(s));
    char temp2[80];
    sprintf(temp2, "%d", ssize );
    temp = strcat(temp2, s);

    int total = 0;
    int bytesLeft = fileLeng; // how many bytes left to send
    int bytesSent = 0; //how many bytes sent

    while(total < fileLeng) {
        bytesSent = write(newsockfd, temp + total, bytesLeft);
        if (bytesSent == -1) {
            error("ERROR writing to socket");
            break;
        }

        total += bytesSent;
        bytesLeft -= bytesSent;
    }
    return 0;
}

void* sockThread (void* sockThread){
    while(1) {
        char *command;
        char *param;
        char w[256];

        bzero(w, 256);
        bzero(buffer, 256);

        //reading from buffer
        n = read(*(int*) sockThread, buffer, sizeof(buffer));
        if (n == 0) {
            break;
        }
        if (n < 0)
            error("ERROR reading from socket");

        //}

        command = strtok(buffer, " ");
        param = strtok(NULL, " ");
        param = strtok(param, "\n");

        //printf("Command: %s\n", command);

        if(strcmp(command, "QUIT\n") == 0){
            printf("Sockets Closed\n");
            break;
        }

        else if(strcmp(command, "STORE") == 0){

            char* realparam = param;
            realparam = strtok(realparam, "\n");
            realparam = strtok(realparam, "\0");
            //read(*(int*) sockThread, buffer, sizeof(buffer));
            //printf("About to store: %s\n", realparam);
            store(realparam, *(int*) sockThread);
        }

        else if(strcmp(command, "RETRIEVE") == 0){
            //printf("RECEIVEING!\n");
            int length = retrieve(param);
            writeClient(buffer, *(int*) sockThread, length);
        }

        else if(strcmp(command, "LIST\n") == 0){
            char* l;
            l = list();
            writeClient(l, *(int*) sockThread, (strlen(l) + 1));
            bzero(target, 256);
        }

        else{
            //printf("Your input: %s\n", buffer);
            writeClient("Successfully Sent\n", *(int*) sockThread, 256);

        }
    }
    close(*(int*) sockThread);
    free(sockThread);
    return NULL;
}

void sigHandC(int sigC) {
    printf("Shutting down server.\n");
    close(sockfd);
    exit(0);
}

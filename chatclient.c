/*
Andres Dominguez
CS 372 - Intro to Computer Networks
Project 1
Due Date - 5/3/2015
*/
// Main source of reference: http://www.linuxhowtos.org/C_C++/socket.htm

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <signal.h>

// function prototype for my function that handles the actual chat system
void dostuff(int, char *);

// function to help handle error messages
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGCHLD,SIG_IGN);
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[500];
    char username[500];
    char temp[500];

    // statement to make sure the user has entered the correct number of arguments
    if (argc < 3) 
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    // get users username for the chat session
    printf("%s", "Please enter a username: ");
    bzero(username,500);
    fgets(username,500,stdin);

    printf("\n");

    // remove a pesky new line character inserted at the end of the username
    int len = strlen(username);
    username[len-1] = '\0';

    // create a holding place for the username so it can be reset before the
    // next dostuff() call
    strcpy(temp, username);

    // loop to keep the user in the chat session
    while(1)
    { 
        // get port number from command line and set the socket
        portno = atoi(argv[2]);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        
        // make sure the socket was opened correctly
        if (sockfd < 0) 
        {
            error("ERROR opening socket");
        }

        //server = gethostbyname("localhost");
        server = gethostbyname(argv[1]);
        
        // make sure the host entered actually exists
        if (server == NULL) 
        {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }

        // set the fields for the server address
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
             (char *)&serv_addr.sin_addr.s_addr,
             server->h_length);
        serv_addr.sin_port = htons(portno);
        
        // make sure we connect successfully 
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        {
            error("ERROR connecting");
        }
        
        // reset username before we enter the dostuff function
        bzero(username, 500);
        strcpy(username, temp);
        dostuff(sockfd, username);
    }   

    close(sockfd);
    return 0;
}

void dostuff(int sock, char* username)
{
    char buffer[500];
    int n;

    // get the users message to be sent
    bzero(buffer,500);
    printf("%s\n", "Enter your message: ");
    bzero(buffer,500);
    fgets(buffer,500,stdin);

    // add the users username to their message, so it is displayed correctly to the server
    strcat(username, "> " );
    strcat(username, buffer);

    // write the message to the other user and check to see if it wrote correctly
    n = write(sock,username,strlen(username));
    if (n < 0) 
    {
         error("ERROR writing to socket");
    }
    
    // empty the buffer
    bzero(buffer,500);

    // read the message from the other user and make sure the read was successful
    n = read(sock,buffer,500);
    if (n < 0) 
    {
         error("ERROR reading from socket");
    }
    
    // print the message that was read from the other user
    printf("\n");
    printf("%s\n",buffer);
}
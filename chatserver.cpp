/*
Andres Dominguez
CS 372 - Intro to Computer Networks
Project 1
Due Date - 5/3/2015
*/
// Main source of reference: http://www.linuxhowtos.org/C_C++/socket.htm

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

using namespace std;

// function prototype for my function that handles the actual chat system
void dostuff(int, char *); 

// function to help handle error messages
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     signal(SIGCHLD,SIG_IGN);
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     char username[500];

     // statement to make sure the user has entered the correct number of arguments
     if (argc < 2) 
     {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     // set up the socket
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     
     // make sure the socket opened correctly 
     if (sockfd < 0)
     { 
        error("ERROR opening socket");
     }

     // set up the server address
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     // check to see if the bind was successful
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
     {
              error("ERROR on binding");
     }

     // ask the user to enter their username for the chat session
     printf("%s", "Please enter a username: ");
     bzero(username,500);
     fgets(username,500,stdin);

     printf("\n");

     // remove a pesky new line character inserted at the end of the username
     int len = strlen(username);
     username[len-1] = '\0';
     
     // call to listen and wait for the client to connect 
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     
     // loop to keep the server open for connections
     while (1) 
     {
         // create a new socket
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

         // make sure the socket was accpeted
         if (newsockfd < 0) 
         {
             error("ERROR on accept");
         }

         // call to fork a new process
         pid = fork();
         
         // make sure fork was successful
         if (pid < 0)
         {
             error("ERROR on fork");
         }
         
         // check to see if we are in the child process
         if (pid == 0)  
         {
             // close the old socket and go into our dostuff function to being chat session
             close(sockfd);
             dostuff(newsockfd, username);
             exit(0);
         }
         else
         { 
             close(newsockfd);
         }   
     } 

     close(sockfd);

     return 0; 
}

void dostuff (int sock, char* username)
{
   int n;
   char buffer[500];
      
   bzero(buffer,500);
   
   // read in the information from the client and check to see if it was successful
   n = read(sock,buffer,500);
   if (n < 0) 
   {
      error("ERROR reading from socket");
   }

   // print what was read in from the other user
   printf("\n");
   printf("%s\n",buffer);

   // ask the user to enter their message they want sent
   printf("%s\n", "Enter your message: ");
   bzero(buffer,500);
   fgets(buffer,500,stdin);

   // add the user username to the message so it can be displayed on the other end
   strcat(username, "> " );
   strcat(username, buffer);

   // call to send the message that was entered to the other user and check if it could be written
   n = write(sock,username,strlen(username));
   if (n < 0) 
   {
      error("ERROR writing to socket");
   }
}
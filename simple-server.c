/* run using ./server <port> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>

#include <pthread.h>

void error(char *msg) {
  perror(msg);
  exit(1);
}

void * startFun(void *args){
  int newsockfd=*((int *)args);
  char buffer[256];
  int n;

  while (1)
  {
    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);
    if(strlen(buffer)==0){
      break;
    }
    if (n < 0)
      error("ERROR reading from socket");
    printf("Here is the message: %s", buffer);

    /* send reply to client */

    n = write(newsockfd,buffer,strlen(buffer));
    if (n < 0)
      error("ERROR writing to socket");
  }

}


int main(int argc, char *argv[]) {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  pthread_t tid;

  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");



  /* read message from client */
  listen(sockfd, 5);
  while (1)
  {
        /* listen for incoming connection requests */


    clilen = sizeof(cli_addr);

    /* accept a new request, create a newsockfd */

    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");

    pthread_create(&tid,NULL,startFun,&newsockfd);
    
    
  }
  
  close(sockfd);
  return 0;
}

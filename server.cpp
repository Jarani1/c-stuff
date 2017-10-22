#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>

using namespace std;





void *connection_handler(void *socketfdesc)
{
  std::cout << "Handeling thread" << '\n';

  int clisock = *(int*)socketfdesc;
  int read_ret;
  int end = 0;
  char *message, buffer[256];

  //send message to client
  //write takes socketfd to write to, message , length of message
  //send message pointer and length point on all
  // message = "Suck my penis hehhehehehe\n";
  // write(clisock,message, strlen(message));
  // message = "Type something you want me to ECHO, neeeeeeeeeeeerd.\n";
  // write(clisock,message,strlen(message));
  // fflush(stdout);

  //recieve message from client
  //read takes socketfd to read from, buffer, buffer size
  while((end == 0))
  {
    //read from client
    //if error here makes 1 client shut down everything
    if(read_ret=read(clisock,buffer,255)==-1)
    {
      std::cout << "error at read" << '\n';
    }
    std::cout << buffer << '\n';
    std::cout << "i just read something" << '\n';

    //echo back
    //the write is complete garbage
    write(clisock,buffer,strlen(buffer));
    if(memcmp(buffer, "RIGHT", strlen("RIGHT")) == 0)
    {


    }



    //this will basically be how i check my protocols
    if(memcmp(buffer, "quit", strlen("quit")) == 0)
    {
      std::cout << "In quit" << '\n';
      end=1;

    }

    bzero(buffer,256);
    fflush(stdout);
  }

  if(read_ret==-1)

  {
    std::cout << "recieve faild" << '\n';
  }

  std::cout << "bye client thread" << '\n';
  fflush(stdout);


  //free socket pointer (malloc) and close socket
  close(clisock);
  free(socketfdesc);
}






int main()
{
  std::cout << "Lets rock n roll" << '\n';

  socklen_t cliLen;
  int socketfd , client_sock, *new_sock;
  struct sockaddr_in server_addr , cli_addr;

  //Create socket
  //Takes protocolfamily, TCP, default
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if(socketfd == -1)
  {
    std::cout << "ERROR opening socket" << '\n';
  }
  std::cout << "Socket opened." << '\n';

  //prepare socketaddr_in struct
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("192.168.1.7");
  server_addr.sin_port = htons(2000);

  //bind socket to port with addr of localhost(server) and listend for connections
  //Takes filedesc address to bind to and length of addr structure
  if(bind(socketfd,(struct sockaddr *)&server_addr, sizeof(server_addr))<0)
  {
    std::cout << "Error at bind" << '\n';
  }
  std::cout << "Bind done." << '\n';

  //print port and addr server is running on
  printf("Server is running on %s on port %d\n",inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));


  //listen on the port we've bound to
  //Takes socket filedesc and max size in backlog que

  listen(socketfd, 5);


  //2017-10-19 01:35 basic setup complete
  //Accept connections and run threads
  //Accept will write the connecting clients addr info into addr struct and the size of the struct is cliLen
  //def: addr info of the remote host is written into remote host struct(client addr) and size of is written inte addr_length(cliLen)
  //also blocking

  cliLen = sizeof(cli_addr);






  while(true)
  {
    std::cout << "Waiting for connection" << '\n';
    client_sock = accept(socketfd, (struct sockaddr *)&cli_addr, &cliLen);
    std::cout << "Connection accepted" << '\n';
    //Start thread with new client_sock
    //pthread create takes thread, attr(null==default), start routtine, arg
    //In this case arg will be the socket just set up with the thread so the handler knows which client to talk to




    int ret;
    pthread_t connection;
    new_sock = (int*)malloc(1); //malloc is *void so have to type cast to int
    *new_sock = client_sock;

    ret = pthread_create(&connection, NULL, connection_handler, (void*)new_sock);
    if(ret!=0)
    {
      std::cout << "ERROR: pthread_create" << '\n';
    }


  }
  if(client_sock<0)
  {
    std::cout << "ERROR: on accept" << '\n';
  }

}



//gdb server.o gdb command then just run easy peasy

//compile as c++ gcc server.cpp (-pthread for added func)-lstdc++ -o server.o
//i need to learn this compile shit

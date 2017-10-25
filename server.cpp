#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <iterator>
#include <mutex>

using namespace std;


int portarr[8];
//we need a socket fd array for specific messages
int fdarray[8];
int clientCount;

void *connection_handler(void *socketfdesc)
{
  std::cout << "Handeling thread" << '\n';

  int clisock = *(int*)socketfdesc;
  int read_ret;
  int end = 0;
  char *message, buffer[256], sendbuffer[256], forbuffer[256];
  int init = 0;
  int myID;
  char idnum;
  //string ID to send always
  string idn;
  mutex mtx;

  //send message to client
  //write takes socketfd to write to, message , length of message
  //send message pointer and length point on all


  //recieve message from client
  //read takes socketfd to read from, buffer, buffer size
  while((end == 0))
  {

    //print out entire array loooollll

    // for(int i = 0; i<8; i++)
    // {
    //   std::cout << portarr[i] << '\n';
    // }

    //read from client
    //if error here makes 1 client shut down everything

    if(read_ret=read(clisock,buffer,255)==-1)
    {
      std::cout << "error at read" << '\n';
      end=1;
    }




    else if(memcmp(buffer, "ID", strlen("ID")) == 0)
    {
      //give me my ID
      std::cout << "got ID" << '\n';

      for(int i= 0; i<255;i++)
      {
        if(buffer[i]==':')
        {
          idnum = buffer[i+1]; // cant go above 9 players heheh
          break;
        }
      }

      myID = idnum - '0'; //ascii
      printf("My ID is: %d\n",myID );

      //now that i got ID send start
      idn = to_string(myID);
      string send = "JOINSE:"+idn;
      mtx.lock();
      //c_Str converts string to const char*
      strcpy(forbuffer, send.c_str());
      std::cout << "sending this: "<<send << '\n';
      for(int i = 0; i<8;i++)
      {
        if(i!=myID && fdarray[i]!=0)
        {
          write(fdarray[i],forbuffer,strlen(forbuffer));
          bzero(forbuffer,256);
        }
      }

      //inform yourself
      //Send join on everyones behalf lol

      for(int j = 0;j<clientCount;j++)
      {
        if(j!=myID)
        {
          string idnum = to_string(j);
          string infme = "JOINSE:"+idnum;
          strcpy(sendbuffer, infme.c_str());
          write(clisock,sendbuffer,strlen(sendbuffer));
          bzero(sendbuffer,256);
          fflush(stdout);
        }
      }


      mtx.unlock();



    }


    else if(memcmp(buffer, "CURRPOS", strlen("CURRPOS")) == 0)
    {
      string send = "DOWNOK";
      //c_Str converts string to const char*
      strcpy(sendbuffer, send.c_str());
      //need to check if pos avalible
      write(clisock,sendbuffer,strlen(sendbuffer));
    }




    else if(memcmp(buffer, "RIGHT", strlen("RIGHT")) == 0)
    {
      // //POSITION PART
      // string send = "RIGHTOK";
      // //c_Str converts string to const char*
      // strcpy(sendbuffer, send.c_str());
      // //need to check if pos avalible
      // write(clisock,sendbuffer,strlen(sendbuffer));


      //INFORM OTHER PLAYERS
      //send to other players
      string rsend = "CRIGHT:"+idn;
      std::cout << rsend << '\n';
      mtx.lock();
      strcpy(forbuffer, rsend.c_str());
      for(int i = 0; i<8;i++)
      {
        if(i!=myID && fdarray[i]!=0)
        {
          fflush(stdout);
          std::cout << "Buffer: " <<forbuffer<< '\n';
          write(fdarray[i],forbuffer,strlen(forbuffer));
          bzero(forbuffer,256);

        }
      }
      mtx.unlock();


    }

    else if(memcmp(buffer, "LEFT", strlen("LEFT")) == 0)
    {
      // string send = "LEFTOK";
      // //c_Str converts string to const char*
      // strcpy(sendbuffer, send.c_str());
      // //need to check if pos avalible
      // write(clisock,sendbuffer,strlen(sendbuffer));
      string rsend = "CLEFTT:"+idn;
      mtx.lock();
      strcpy(forbuffer, rsend.c_str());
      for(int i = 0; i<8;i++)
      {
        if(i!=myID && fdarray[i]!=0)
        {
          fflush(stdout);
          write(fdarray[i],forbuffer,strlen(forbuffer));
          bzero(forbuffer,256);

        }
      }
      mtx.unlock();
    }

    else if(memcmp(buffer, "UP", strlen("UP")) == 0)
    {
      // string send = "UPOK";
      //
      // //c_Str converts string to const char*
      // strcpy(sendbuffer, send.c_str());
      // //need to check if pos avalible
      // write(clisock,sendbuffer,strlen(sendbuffer));
      string rsend = "CUUPPP:"+idn;
      mtx.lock();
      strcpy(forbuffer, rsend.c_str());
      for(int i = 0; i<8;i++)
      {
        if(i!=myID && fdarray[i]!=0)
        {
          fflush(stdout);
          write(fdarray[i],forbuffer,strlen(forbuffer));
          bzero(forbuffer,256);

        }
      }
      mtx.unlock();
    }

    else if(memcmp(buffer, "DOWN", strlen("DOWN")) == 0)
    {
      // string send = "DOWNOK";
      // //c_Str converts string to const char*
      // strcpy(sendbuffer, send.c_str());
      // //need to check if pos avalible
      // write(clisock,sendbuffer,strlen(sendbuffer));
      string rsend = "CDOWNN:"+idn;
      mtx.lock();
      strcpy(forbuffer, rsend.c_str());
      for(int i = 0; i<8;i++)
      {
        if(i!=myID && fdarray[i]!=0)
        {
          fflush(stdout);
          write(fdarray[i],forbuffer,strlen(forbuffer));
          bzero(forbuffer,256);

        }
      }
      mtx.unlock();

    }



    //this will basically be how i check my protocols
    else if(memcmp(buffer, "quit", strlen("quit")) == 0)
    {
      std::cout << "In quit" << '\n';
      end=1;

    }
    bzero(sendbuffer,256);
    bzero(buffer,256);
    bzero(forbuffer,256);
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
//this is alot of garbage
  string iparr[256];
//  int portarr[256];
  double posiarr[256];
  char initbuffer[256];

  bzero(fdarray,256);


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
  server_addr.sin_addr.s_addr = inet_addr("192.168.1.3"); //ifconfig
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


    //We want to make a list of all the conneced socketFds
    fdarray[clientCount]=client_sock; //adding them to lists

    std::cout << "inc array" << '\n';
    for(int i=0;i<255;i++)
    {
      if(fdarray[i]!=0)
      {

        std::cout << fdarray[i] << '\n';
      }else
      {
        break;
      }
    }






    //Start thread with new client_sock
    //pthread create takes thread, attr(null==default), start routtine, arg
    //In this case arg will be the socket just set up with the thread so the handler knows which client to talk to

    printf("Client with IP: %s and PORT %d connected\n",inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));


    //add client to lists

    iparr[clientCount] = inet_ntoa(server_addr.sin_addr);
    portarr[clientCount]= ntohs(cli_addr.sin_port);
    std::cout << "client added to lists" << '\n';
    printf("Total connected clients: %d\n",clientCount+1);

    //send client its id number
    string idnum = to_string(clientCount);
    string send = "clntid:"+idnum;
    std::cout << send << '\n';
    //c_Str converts string to const char*
    strcpy(initbuffer, send.c_str());
    //need to check if pos avalible
    write(client_sock,initbuffer,strlen(initbuffer));

    clientCount++;
    std::cout << "creating thread" << '\n';

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

//split string at

// string sid;
// char * pch;
// pch = strtok(buffer,":");
// while(pch!=NULL)
// {
//   printf("%s\n",pch);
//   pch = strtok(NULL,":");
//   string sid (pch);
//
// }
// std::cout << "string ID" << '\n';
// std::cout << sid << '\n';


//gdb server.o gdb command then just run easy peasy

//compile as c++ gcc server.cpp (-pthread for added func)-lstdc++ -o server.o
//i need to learn this compile shit

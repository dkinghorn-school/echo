#pragma once

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

using namespace std;
class Message {
    public:
      string command;
      string params[2];
      string value;
      bool needed;
      string cach;
  };

class Server {
public:
    Server(int port);
    ~Server();

    void run();
    
private:
    void create();
    void close_socket();
    void serve();
    void handle(int);
    int writeFile(Message* message);
    string get_request(int, string cach);
    string finish_request(int client, string message, int length);
    bool send_response(int, string);
    Message parse_request(string);
    void get_value(int client, Message* message);
    int port_;
    int server_;
    int buflen_;
    char* buf_;
};

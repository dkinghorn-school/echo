#pragma once

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <string>
#include "serverController.h"

using namespace std;
class Message
{
    public:
    // Message();
    string message;
    string cach;
    string params[4];
    bool valid;
};
class Server
{
  public:
    Server(int port);
    ~Server();

    void run();

  private:
    Message getParams(string input);
    ServerController myController;
    void create();
    void close_socket();
    void serve();
    void handle(int);
    void finishMessage(int client, Message *message);
    string get_request(int);
    bool send_response(int, string);
    string handleInput(string input);
    int port_;
    int server_;
    int buflen_;
    char *buf_;
};
#pragma once

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class Client {
public:
    Client(string host, int port);
    ~Client();

    void run();

private:
    string handleInput(string input[3]);
    string handleSend(string input[3]);
    string handleList(string input[3]);
    string handleRead(string input[3]);
    virtual void create();
    virtual void close_socket();
    void echo();
    // bool is_number(string s);
    bool send_request(string);
    bool get_response();
    bool checkRequest(string request[3]);
    string host_;
    int port_;
    int server_;
    int buflen_;
    char* buf_;
};

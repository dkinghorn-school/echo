#pragma once

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "serverController.h"
#include "safeQueue.h"
#include "serverController.h"
#include "safeQueue.h"

using namespace std;
class Messagea
{
public:
  // Message();
  string message;
  string cach;
  string params[4];
  bool valid;
};
class MyThread
{
public:
  MyThread(SafeQueue<pair<int, string>> *, ServerController *, int port, bool debug);
  void run();

private:
  int port_;
  int server_;
  int buflen_;
  char *buf_;
  bool debug;
  void handle(int client, string cach);
  string get_request(int client);
  bool send_response(int client, string response);
  Messagea getParams(string request);
  void finishMessage(int client, Messagea *message);
  
  SafeQueue<pair<int, string>> *queue;
  ServerController *controller;
};
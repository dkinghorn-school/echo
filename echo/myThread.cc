#include "myThread.h"
#include <sstream>
#include <iostream>

MyThread::MyThread(SafeQueue<pair<int, string>> *queue, ServerController *controller, int port, bool debug)
{
  this->queue = queue;
  this->controller = controller;
  this->debug = debug;
  port_ = port;
  buflen_ = 1024;
  buf_ = new char[buflen_ + 1];
}

void MyThread::run()
{
  while (1)
  {
    pair<int, string> request = queue->get();
    handle(request.first, request.second);
  }
}
void MyThread::handle(int client, string cach)
{

  string request;
  // get a request
  if (cach.find("\n") == string::npos)
  {
    request = cach + get_request(client);
  }
  else
  {
    request = cach;
  }
  // break if client is done or an error occurred
  if (request.empty())
  {
    close(client);
     return;
  }
  //set cach

  if (debug)
    cout << "debug: " << request << endl;
  Messagea myMessage = getParams(request);
  string response = "";
  if (!myMessage.valid)
  {
    response = "error invalid input\n";
  }
  else if (myMessage.params[0] == "put")
  {
    finishMessage(client, &myMessage);
    if (!myMessage.valid)
    {
      response = "error \n";
    }
    else
    {
      response = controller->putMessage(myMessage.params[1], myMessage.params[2], myMessage.message);
    }
  }
  else if (myMessage.params[0] == "list")
  {
    response = controller->getList(myMessage.params[1]);
  }
  else if (myMessage.params[0] == "get")
  {
    response = controller->getMessage(myMessage.params[1], stoi(myMessage.params[2]));
  }
  else if (myMessage.params[0] == "reset")
  {
    controller->reset();
    response = "OK\n";
  }
  else
  {
    response = "error\n";
  }
  cach = myMessage.cach;

  // send response
  bool success = send_response(client, response);
  // break if an error occurred
  if (not success)
  {
    // break;
    close(client);
    return;
  }
  queue->put(pair<int,string>(client, cach));
}
string MyThread::get_request(int client)
{
  string request = "";
  // read until we get a newline
  while (request.find("\n") == string::npos)
  {
    int nread = recv(client, buf_, 1024, 0);
    if (nread < 0)
    {
      if (errno == EINTR)
        // the socket call was interrupted -- try again
        continue;
      else
        // an error occurred, so break out
        return "";
    }
    else if (nread == 0)
    {
      // the socket is closed
      return "";
    }
    // be sure to use append in case we have binary data
    request.append(buf_, nread);
  }
  // a better server would cut off anything after the newline and
  // save it in a cache
  return request;
}
bool MyThread::send_response(int client, string response)
{
  // prepare to send response
  const char *ptr = response.c_str();
  int nleft = response.length();
  int nwritten;
  // loop to be sure it is all sent
  while (nleft)
  {
    if ((nwritten = send(client, ptr, nleft, 0)) < 0)
    {
      if (errno == EINTR)
      {
        // the socket call was interrupted -- try again
        continue;
      }
      else
      {
        // an error occurred, so break out
        perror("write");
        return false;
      }
    }
    else if (nwritten == 0)
    {
      // the socket is closed
      return false;
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return true;
}
Messagea MyThread::getParams(string request)
{
  Messagea newMessage;
  newMessage.valid = true;
  newMessage.cach = request.substr(request.find("\n") + 1, request.length());
  request = request.substr(0, request.find("\n"));
  int i = 0;
  stringstream ssin(request);
  while (ssin.good() && i < 4)
  {
    ssin >> newMessage.params[i];
    ++i;
  }

  try
  {
    if (newMessage.params[0] == "put")
      int length = stoi(newMessage.params[3]);
    if (newMessage.params[0] == "get")
      int length = stoi(newMessage.params[2]);
    if (newMessage.params[1] == "" && newMessage.params[0] != "reset")
      newMessage.valid = false;
  }
  catch (std::invalid_argument &e)
  {
    newMessage.valid = false;
  }
  return newMessage;
}
void MyThread::finishMessage(int client, Messagea *message)
{
  int length = stoi(message->params[3]);

  message->message = message->cach;
  message->cach = "";
  if (debug)
    cout << "debug: ength: " << length << endl;

  while (message->message.length() < length)
  {
    // cout << client << "listening\n";
    int nread = recv(client, buf_, 1024, 0);
    if (nread < 0)
    {
      if (errno == EINTR)
        // the socket call was interrupted -- try again
        continue;
      else
        // an error occurred, so break out
        break;
    }
    else if (nread == 0)
    {
      // the socket is closed
      break;
    }
    // be sure to use append in case we have binary data
    message->message.append(buf_, nread);
  }
  message->cach = message->message.substr(length, message->message.length());
  message->message = message->message.substr(0, length);
}
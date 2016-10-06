#include "server.h"
#include <sstream>
#include <iostream>
using namespace std;
Server::Server(int port, bool debug)
{
    // setup variables
    port_ = port;
    buflen_ = 1024;
    buf_ = new char[buflen_ + 1];
    this->debug = debug;
}

Server::~Server()
{
    delete buf_;
}

void Server::run()
{
    // create and run the server
    create();
    serve();
}

void Server::create()
{
    struct sockaddr_in server_addr;

    // setup socket address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // create socket
    server_ = socket(PF_INET, SOCK_STREAM, 0);
    if (!server_)
    {
        perror("socket");
        exit(-1);
    }

    // set socket to immediately reuse port when the application closes
    int reuse = 1;
    if (setsockopt(server_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopt");
        exit(-1);
    } 

    // call bind to associate the socket with our local address and
    // port 
    if (::bind(server_, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(-1);
    }

    // convert the socket to listen for incoming connections
    if (listen(server_, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(-1);
    }
}

void Server::close_socket()
{
    close(server_);
}
void Server::runThread(){
    while(1){
        
    }
}
void Server::serve()
{
    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);
    //TODO: create threads
    // accept clients
    while ((client = accept(server_, (struct sockaddr *)&client_addr, &clientlen)) > 0)
    {
        
        handle(client);
    }
    close_socket();
}
string Server::handleInput(string input)
{

    return "";
}
Message Server::getParams(string request)
{

    Message newMessage;
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
        if(newMessage.params[0] == "put" )
        int length = stoi(newMessage.params[3]);
        if( newMessage.params[0] == "get")
        int length = stoi(newMessage.params[2]);
        if(newMessage.params[1] == "" && newMessage.params[0] != "reset")
            newMessage.valid = false;
    }
    catch (std::invalid_argument &e)
    {
        newMessage.valid = false;

    }
    return newMessage;
}
void Server::finishMessage(int client, Message *message)
{
    int length = stoi(message->params[3]);

    message->message = message->cach;
    message->cach = "";
    if(debug)
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
    //check if valid
}
void Server::handle(int client)
{
    //TODO: change to accept only one request then store it back at the end of the queue
    // loop to handle all requests
    string cach = "";
    int requestCount = 0;
    while (1)
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
            break;
        //set cach
        requestCount++;
        if(debug)
            cout << "debug: " << request << endl;
        Message myMessage = getParams(request);
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
                response = myController.putMessage(myMessage.params[1], myMessage.params[2], myMessage.message);
            }
        }
        else if (myMessage.params[0] == "list")
        {
            response = myController.getList(myMessage.params[1]);
        }
        else if (myMessage.params[0] == "get")
        {
            response = myController.getMessage(myMessage.params[1], stoi(myMessage.params[2]));
        }
        else if (myMessage.params[0] == "reset")
        {
            myController.reset();
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
            break;
    }
    close(client);
}

string
Server::get_request(int client)
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

bool Server::send_response(int client, string response)
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
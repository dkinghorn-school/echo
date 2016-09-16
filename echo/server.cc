#include "server.h"
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

Server::Server(int port)
{
    // setup variables
    port_ = port;
    buflen_ = 1024;
    buf_ = new char[buflen_ + 1];
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
    if (bind(server_, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
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

void Server::serve()
{
    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);

    // accept clients
    while ((client = accept(server_, (struct sockaddr *)&client_addr, &clientlen)) > 0)
    {

        handle(client);
    }
    close_socket();
}
int Server::writeFile(Message *message)
{
    cout << "writing file " << message->params[0] << endl;
    ofstream myfile(message->params[0] + ".txt");
    // myfile.put(message->value)
    // myfile.open("/" + message->params[0]);
    myfile << message->value;
    myfile.close();
    return 0;
}
Message Server::parse_request(string request)
{
    Message newMessage = Message();
    //start buffer
    string params = request.substr(0, request.find("\n"));

    string cach = request.substr(request.find("\n") + 1, request.length());
    // string file = request.substr(pos,request.length());

    //convert parameters
    string arr[4];
    int i = 0;
    stringstream ssin(params);
    while (ssin.good() && i < 4)
    {
        ssin >> arr[i];
        ++i;
    }

    // for (i = 0; i < 2; i++)
    // {
    //     cout << arr[i] << endl;
    // }
    newMessage.command = arr[0];
    newMessage.params[0] = arr[1];
    newMessage.params[1] = arr[2];
    newMessage.params[2] = arr[3];
    newMessage.value = cach;
    newMessage.needed = !(cach.length() == stoi(arr[2]));

    return newMessage;
}
void Server::get_value(int client, Message *message)
{

    while (message->value.length() < stoi(message->params[1]) - 1)
    {
        cout << "help1" << endl;
        // buf_ = new char[buflen_ + 1];
        int nread = recv(client, buf_, 10, 0);
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
        message->value.append(buf_, nread);
        // cout << message.value;
    }

    message->cach = message->value.substr(stoi(message->params[1]), message->value.length());
    message->value = message->value.substr(0, stoi(message->params[1]));
    cout << "cach:1 " << message->cach << endl;
    //  << "value:1 " << message->value << endl;
}
void Server::handle(int client)
{
    // loop to handle all requests

    int messageCount = 0;
    while (1)
    {
        string messageCach = "";
        do
        {
            cout << "messageCach" << messageCach << endl;
            // get a request
            string request = get_request(client);
            // break if client is done or an error occurred
            if (request.empty() && messageCach.empty())
                break;
            request = messageCach + request;
            Message message = parse_request(request);
            
            get_value(client, &message);

            messageCach = message.cach;
            if (messageCach.at(0) == '\n' || messageCach.at(0) == ' ')
            {
                messageCach = messageCach.substr(1, messageCach.length());
            }
            writeFile(&message);
        } while (1);
        // send response
        bool success = send_response(client, "message.value\n");
        break;
        // break if an error occurred
        if (not success)
            break;
    }
    close(client);
}
string
Server::finish_request(int client, string message, int length)
{
    cout << "length: " << length << endl;
    while (message.length() < length)
    {
        cout << client << "listening\n";
        int nread = recv(client, buf_, 1024, 0);
        cout << nread << endl;
        // if(newMessage.length() == 0)
        //     break;
        // message += newMessage;
    }
    return message.substr(0, length);
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

#include "client.h"
#include <sstream>

using namespace std;
Client::Client(string host, int port, bool debug)
{
    // setup variables
    host_ = host;
    port_ = port;
    buflen_ = 1024;
    buf_ = new char[buflen_ + 1];
    this->debug = debug;
}

Client::~Client()
{
}

void Client::run()
{
    // connect to the server and run echo program
    create();
    echo();
}

void Client::create()
{
    struct sockaddr_in server_addr;

    // use DNS to get IP address
    struct hostent *hostEntry;
    hostEntry = gethostbyname(host_.c_str());
    if (!hostEntry)
    {
        cout << "No such host name: " << host_ << endl;
        exit(-1);
    }

    // setup socket address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    memcpy(&server_addr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);

    // create socket
    server_ = socket(PF_INET, SOCK_STREAM, 0);
    if (!server_)
    {
        perror("socket");
        exit(-1);
    }

    // connect to server
    if (connect(server_, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        exit(-1);
    }
}

void Client::close_socket()
{
    close(server_);
}
// bool is_number(string s)
// {
//     return  (s.find_first_not_of( "0123456789" ) == string::npos);
// }
bool Client::checkRequest(string request[3])
{
    if (debug)
    {
        string message = "debug mode: " + request[0] + " " + request[1] + " " + request[2]; 
        cout << message;
        printf("\n");
    }
    if (request[0] == "quit")
    {
        return true;
    }
    if (request[1] == "")
    {
        return false;
    }
    if (request[0] == "send")
    {
        return !(request[2] == "");
    }
    else if (request[0] == "list")
    {
        return true;
    }
    else if (request[0] == "read")
    {
        return request[2].find_first_not_of("0123456789") == string::npos;
    }
    else
    {
        return "\n";
    }
    /*
    // if (request[0] == "put"){

    // } else if (request[0] == "list"){

    // } else if (request[0] == "get"){

    // } else if (request[0] == "reset"){
    //     return true;
    // } else if (request[0] == "read"){
    //     if(request[1] != "" && is_number(request[2])){
    //         return true;
    //     } else {
    //         return false;
    //     }
    // } else {
    //     return false;
    // }
    // return true;*/
}
string Client::handleSend(string request[3])
{
    string message = "";
    printf("- Type your message. End with a blank line -\n");
    string input = "";
    while (1)
    {
        input = "";
        getline(cin, input);
        if (input == "")
            break;

        message += input;
    }
    return "put " + request[1] +
           " " + request[2] +
           " " + to_string(message.size()) + "\n" +
           message;
}
string Client::handleList(string request[3])
{

    return "list " + request[1] + "\n";
}
string Client::handleRead(string request[3])
{
    return "get " + request[1] + " " + request[2] + "\n";
}
string Client::handleInput(string request[3])
{
    if (request[0] == "send")
    {
        return handleSend(request);
    }
    else if (request[0] == "list")
    {
        return handleList(request);
    }
    else if (request[0] == "read")
    {
        return handleRead(request);
    }
    else
    {
        return "\n";
    }
}
void Client::echo()
{
    string line;

    // loop to handle user interface
    while (getline(cin, line))
    {
        // append a newline

        string request[3];
        int i = 0;
        stringstream ssin(line);
        while (ssin.good() && i < 4)
        {
            ssin >> request[i];
            ++i;
        }
        // check input
        if (checkRequest(request))
        {
            if (request[0] == "quit")
                break;
            string message = handleInput(request);
            // send request
            if(debug){
                cout << "debug: " + message;
                printf("\n");
            }
            bool success = send_request(message);
            // break if an error occurred
            if (not success)
                break;
            // get a response
            success = get_response();
            // break if an error occurred
            if (not success)
                break;
        }
        else
        {
            printf("invalid input\n");
        }
    }
    close_socket();
}

bool Client::send_request(string request)
{
    // prepare to send request
    const char *ptr = request.c_str();
    int nleft = request.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft)
    {
        if ((nwritten = send(server_, ptr, nleft, 0)) < 0)
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

bool Client::get_response()
{
    string response = "";
    // read until we get a newline
    while (response.find("\n") == string::npos)
    {
        int nread = recv(server_, buf_, 1024, 0);
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
        response.append(buf_, nread);
    }
    // a better client would cut off anything after the newline and
    // save it in a cache
    cout << response;
    return true;
}

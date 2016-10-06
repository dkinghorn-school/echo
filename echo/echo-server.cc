#include <stdlib.h>
#include <unistd.h>

#include <iostream>

#include "server.h"

using namespace std;

int
main(int argc, char **argv)
{
    int option, port;
    bool debug = false;
    // setup default arguments
    port = 3000;

    // process command line options using getopt()
    // see "man 3 getopt"
    while ((option = getopt(argc,argv,"dp:")) != -1) {
        switch (option) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'd':
                debug = true;
                break;
            default:
                cout << "server [-p port]" << endl;
                exit(EXIT_FAILURE);
        }
    }

    Server server = Server(port, debug);
    server.run();
}

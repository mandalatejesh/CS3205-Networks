#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <ios>
#include <limits>
#define MAX 500
#define PORT 8080


using namespace std;
int main()
{
    char buf[MAX];
    // create a socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    socklen_t server_addr_sz;
    if (socketfd == -1)
    {
        cout << "Err: socket not created\n";
        return 1;
    }
    // connect to server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("10.42.82.252");
    server_addr.sin_port = htons(PORT);

    if (connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        cout << "server connection failed\n";
        return 0;
    }

    int a, b;
    for (;;)
    {
        memset(buf, 0, sizeof(buf));
        char ch = 'a';
        std::string cmd = "";
        bool br = false;
        while (ch != '\n') {
            int nbytes = read(socketfd, &ch, 1);
            if (nbytes == 0) {
                cout << "Sorry, your partner disconnected\n";
                br = true;
                break;
            }
            if (ch != '\n')
                cmd += ch;
        }
        if (br) {
            break;
        }
        // cout<<"\nreceived: %s\n", buf);
        if (cmd == "exit")
        {
            break;
        } else if (cmd == "input")
        {
            cout << "Enter (ROW, COL) for placing your mark: ";
            std::cin >> a >> b;
            while (!std::cin.good())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cout << "Incorrect input, please try again: ";
                std::cin >> a >> b;
            }
            // check a,b
            a = 10 * a + b;
            char inp[4];
            sprintf(inp, "%d", a);
            write(socketfd, inp, strlen(inp));
        } else if (cmd == "exit1")
        {
            cout << "Sorry, your partner disconnected\n";
            break;
        } else if (cmd == "wait") {
            cout << "Waiting for your partner's move...\n";
        } else if (cmd == "replay")
        {
            cout << "replay? [yes/no]";
            std::string s;
            std::cin >> s;
            if (s == "yes")
            {
                write(socketfd, "yes", 3);
            } else
            {
                write(socketfd, "no", 2);
            }
        } else
        {
            cout << cmd << endl;
        }
    }
    close(socketfd);
    return 1;
}
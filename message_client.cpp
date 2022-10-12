#include<iostream>
#include<sys/types.h>          // Also Used for sockets.
#include<sys/socket.h>
#include<netdb.h>              // getnameinfo  - address-to-name translation in protocol-independent manner
#include<string.h>             // Used for memset
#include<string>
#include<unistd.h>
#include<arpa/inet.h>           // Close 

using namespace std;

int main(int argc, char *argv[])
{
    int i;
    if(argc <1 )
    {
        cout << "Required command line input in \"g++ server.cpp <server_ip_address> <port_number>\" ";
        exit(1);
    }
    // int port_no = atoi(argv[2]);            // Convert string into its equivalent integer



    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1)
    {
        cerr << "Can't create a socket on client Side\n";
        exit(1);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    // server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    int cl_pton = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    // int cl_pton = inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
    if(cl_pton == -1)
    {
        cerr << "Invalid Address.\n";
        exit(2);
    }

    int client_connect = connect(listening, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(client_connect == -1)
    {
        cerr << "Connection Failed.\n";
        exit(3);
    }

    char buffer[4096];
    while(true)
    {
        // Clear the buffer
        memset(buffer, 0, 4096);

        fgets(buffer, 4096, stdin);
        
        int n = write(listening, buffer, strlen(buffer));
        if(n == -1)
        {
            cout << "Error in writing from client side.\n";
            exit(4);
        }

        memset(buffer, 0, 4096);
        n = read(listening, buffer, 4096);
        if(n == -1)
        {
            cout << "Error in reading from client side.\n";
            exit(4);
        }
        cout << "\nServer --> " << buffer;
        if(!strncmp("exit", buffer, 4)){
            break;
            exit(1);
        }

        // cout<< "It's your turn to write: ";
    }
    close(listening);

    return 0;
}
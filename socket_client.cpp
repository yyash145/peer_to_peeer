#include<iostream>
#include<sys/types.h>          // Also Used for sockets.
#include<sys/socket.h>
#include<netdb.h>              // getnameinfo  - address-to-name translation in protocol-independent manner
#include<string.h>             // Used for memset
#include<string>
#include<unistd.h>
#include<arpa/inet.h>           // Close 

using namespace std;

int main()
{
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1)
    {
        cerr << "Can't create a socket on client Side\n";
        exit(1);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    // int cl_pton = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    int cl_pton = inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
    if(cl_pton == -1)
    {
        cerr << "Invalid Address\n";
        exit(2);
    }

    int client_connect = connect(listening, (sockaddr*)&server_addr, sizeof(server_addr));
    if(client_connect == -1)
    {
        cerr << "Connection Failed\n";
        exit(3);
    }
    // // Accept a call
    // sockaddr_in client_addr;
    // char host[NI_MAXHOST];
    // char service[NI_MAXSERV]; 

    // socklen_t client_size = sizeof(client_addr);
    // int client_socket = accept(listening, (sockaddr*)&client_addr, &client_size);
    //                                                                 // The accept() call creates a new socket descriptor with the same properties as socket and returns it to the caller.
    //                                                                 // If the queue has no pending connection requests, accept() blocks the caller unless socket is in nonblocking mode.
    // if(client_socket == -1)
    // {
    //     cerr << "Probem with client connecting.\n";
    //     return -4;
    // }

    // Close the listening Socket
    close(listening);
    // memset(host, 0, NI_MAXHOST);            // Cleaning up the arrays
    // memset(service, 0, NI_MAXSERV);

    // int result = getnameinfo((sockaddr*)&server_addr, sizeof(server_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0);
    // if(result)
    // {
    //     cout << host << " connected on " << service << "\n";
    // }
    // else 
    // {                           // inet_ntop is opposite to inet_pton, i.e., converts an Internet address from its numeric binary form to its standard text format.
    //                             // ntohs is opposite to htons, i.e., takes a 16-bit number in TCP/IP network byte order (AF_INET) and returns a 16-bit number in host byte order.
    //     inet_ntop(AF_INET, &server_addr.sin_addr, host, NI_MAXHOST);
    //     cout << host << " connented on " << ntohs(server_addr.sin_port) << "\n";
    // }

    char buffer[4096];
    while(true)
    {
        // Clear the buffer
        memset(buffer, 0, 4096);

        // Wait for a msg
        int bytes_recieved = recv(client_connect, buffer, 4096, 0);
        if(bytes_recieved == 1)
        {
            cerr << "Sorry, there was a connection issue.\n";
            break;
        }
        else if (bytes_recieved == 0)
        {
            cout << "The client_addr is disconnected.\n";
            break;
        }

        // Display msg
        else{
            cout << "Recieved: " << string(buffer, 0, bytes_recieved) << "\n";
        }

        // Resend msg
        send(listening, buffer, bytes_recieved + 1, 0);
    }
    close(client_connect);

    return 0;
}
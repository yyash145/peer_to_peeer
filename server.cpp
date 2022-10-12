#include<iostream>
#include<sys/types.h>          // Also Used for sockets.
#include<sys/socket.h>
#include<netdb.h>              // getnameinfo  - address-to-name translation in protocol-independent manner
#include<string.h>             // Used for memset
#include<string>
#include<unistd.h>
#include<arpa/inet.h>           // Close 

using namespace std;

#define MAX_CLIENTS SOMAXCONN

int main(int argc, char *argv[])
{
    int i;
    sockaddr_in client_addr;
    sockaddr_in server_addr;

    if(argc <1 )
    {
        cout << "Required command line input in \"g++ server.cpp <port_number>\" ";
        exit(1);
    }
    // int port_no = atoi(argv[1]);            // Convert string into its equivalent integer



    // for(i=0; i<MAX_CLIENTS; i++)
    // {
    //     client_
    // }


    // Create a socket                      // int socket(int domain, int type, int protocol);
    int listening = socket(AF_INET, SOCK_STREAM, 0);    // AF_INET is used for IPv4
                                                        // Stream sockets allow processes to communicate using TCP.
                                                        // After the connection has been established, data can be read from and written to these sockets as a byte stream.
    if(listening == -1)                     // On success, a file descriptor for the new socket is returned. On error, -1 is returned
    {
        cerr << "Can't create a socket\n";
        return -1;
    }  

    // int opt = 1;
    // int set_socket = setsockopt(listening, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    // if(set_socket == -1)
    // {
    //     cerr << "Socket can't be set to port 8080";
    //     return -1;
    // }


    // Bind the socket to a IP/Port
    // sockaddr_in server_addr;                       // sockaddr_in is for IPv4
                                            // Sockaddr_in is for IPv6
    server_addr.sin_family = AF_INET;                  // sin is shorthand for the name of the struct (sockaddr_in) that the member-variables are a part of.
                                                // sin_family refers to an address family which in most of the cases is set to “AF_INET”.
    // server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(54000);       // The htons function takes a 16-bit number in host byte order and returns a 16-bit number in network byte order used in TCP/IP networks.
                                        // Intel supports little-endian format, which means the leats significant bytes are stored before the more significant bytes.
                                        // htons refers to host-to-network short.
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);      // The inet_pton() function converts an Internet address in its standard text format into its numeric binary form.
    // inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);      // The inet_pton() function converts an Internet address in its standard text format into its numeric binary form.

    int binding = bind(listening, (sockaddr *)&server_addr, sizeof(server_addr));       // The bind() function binds a unique local name to the socket with descriptor socket.
                    // Bind socket "listening" using format "AF_NET" to "server_addr" structure
    if(binding == -1)                           
    {
        cerr << "Can't bind the socket to IP / Port Address.\n";
        return -2;
    }


    // Mark the socket for listening in
    int socket_listening = listen(listening, SOMAXCONN);            // MAximum connections can be SOMAXCON(4096)
    if(socket_listening == -1)
    {
        cerr << "Can't Listen.\n";
        return -3;
    }


    // Accept a call
    // sockaddr_in client_addr;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV]; 

    socklen_t client_size = sizeof(client_addr);
    int client_socket = accept(listening, (sockaddr*)&client_addr, &client_size);
                                                                    // The accept() call creates a new socket descriptor with the same properties as socket and returns it to the caller.
                                                                    // If the queue has no pending connection requests, accept() blocks the caller unless socket is in nonblocking mode.
    if(client_socket == -1)
    {
        cerr << "Probem with client connecting.\n";
        return -4;
    }


    
    memset(host, 0, NI_MAXHOST);            // Cleaning up the arrays
    memset(service, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0);
    if(result)
    {
        cout << host << " connected on " << service << "\n";
    }
    else 
    {                           // inet_ntop is opposite to inet_pton, i.e., converts an Internet address from its numeric binary form to its standard text format.
                                // ntohs is opposite to htons, i.e., takes a 16-bit number in TCP/IP network byte order (AF_INET) and returns a 16-bit number in host byte order.
        inet_ntop(AF_INET, &client_addr.sin_addr, host, NI_MAXHOST);
        cout << host << " connented on " << ntohs(client_addr.sin_port) << "\n";
    }


    // While recieving, display the msg
    char buffer[4096];
    while(true)
    {
        // Clear the buffer
        memset(buffer, 0, 4096);

        // Read data from client
        int n = read(client_socket, buffer, 4096);
        if(n == -1)
        {
            cout << "Error in reading.\n";
            exit(1);
        }

        // Print client request data
        cout << "\nClient --> " << buffer;

        memset(buffer, 0, 4096);

        // Write data through client
        fgets(buffer, 4096, stdin);
        
        n = write(client_socket, buffer, 4096);
        if(n == -1)
        {
            cout << "Error in Writing.\n";
            exit(1);
        }
        if(!strncmp("exit", buffer, 4)){
            break;
            exit(1);
        }

        // cout<< "It's your turn to write: ";
    }


    // Close Socket 
    close(client_socket);
    // Close the listening Socket
    close(listening);

    return 0;
}




/* On terminal type command:
        telnet localhost 5400
*/
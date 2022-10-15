#include <iostream>
#include <sys/types.h> // Also Used for sockets.
#include <sys/socket.h>
#include <netdb.h>  // getnameinfo  - address-to-name translation in protocol-independent manner
#include <string.h> // Used for memset
#include <string>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <thread>
#include <algorithm> // find
#include <unordered_map>
#include <arpa/inet.h> // Close

using namespace std;

#define MAX_CLIENTS 30

int main(int argc, char *argv[])
{
    int i;

    if (argc != 3)
    {
        cout << "Required command line input in \" ./tracker tracker_info.txt tracker_no\" ";
        exit(1);
    }

    fstream tracker_info;
    tracker_info.open(argv[1]);
    string tracker_ip, tracker_port;
    tracker_info >> tracker_ip;
    tracker_info >> tracker_port;
    cout << "Tracker IP Address is " << tracker_ip << " and port number is " << tracker_port << "\n";

    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Can't create a socket\n";
        return -1;
    }
    cout << "Socket Created...\n";

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = (stoi(tracker_port.c_str()));
    inet_pton(AF_INET, tracker_ip.c_str(), &server_addr.sin_addr);

    int binding = bind(listening, (sockaddr *)&server_addr, sizeof(server_addr));
    if (binding == -1)
    {
        cerr << "Can't bind the socket to IP / Port Address.\n";
        return -2;
    }
    cout << "Socket Binded...\n";

    int socket_listening = listen(listening, SOMAXCONN);
    if (socket_listening == -1)
    {
        cerr << "Can't Listen.\n";
        return -3;
    }
    cout << "Socket Start Listening...\n";

    sockaddr_in client_addr;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    char buffer[4096];
    while (true)
    {
        memset(buffer, 0, 4096);

        socklen_t client_size = sizeof(client_addr);
        int client_socket = accept(listening, (sockaddr *)&client_addr, &client_size);

        if (client_socket == -1)
        {
            cerr << "Probem with client connecting.\n";
            return -4;
        }
        cout<<"Server and Client connected...\n";
        
        close(listening);
        memset(host, 0, NI_MAXHOST);
        memset(service, 0, NI_MAXSERV);

        int result = getnameinfo((sockaddr *)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0);
        if (result)
        {
            cout << host << " connected on " << service << "\n";
        }
        else
        {
            inet_ntop(AF_INET, &client_addr.sin_addr, host, NI_MAXHOST);
            cout << host << " connented on " << ntohs(client_addr.sin_port) << "\n";
        }

        int bytes_recieved = recv(client_socket, buffer, 4096, 0);
        if (bytes_recieved == 1)
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
        else
        {
            cout << "Recieved: " << string(buffer, 0, bytes_recieved) << "\n";
        }

        // Resend msg
        send(client_socket, buffer, bytes_recieved + 1, 0);
    }

    // Close Socket
    close(listening);

    return 0;
}

/* On terminal type command:
        telnet localhost 5400
*/
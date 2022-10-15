#include <iostream>
#include <sys/types.h> // Also Used for sockets.
#include <sys/socket.h>
#include <netdb.h>  // getnameinfo  - address-to-name translation in protocol-independent manner
#include <string.h> // Used for memset
#include <string>
#include <unistd.h>
#include <vector>
#include <thread>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <openssl/sha.h>
#include <arpa/inet.h> // Close

using namespace std;

string send_message(int listening, string str)
{
    char buffer[4096];
    memset(buffer, 0, 4096);
    // cout << "str = " << str << "\n";
    int dd = send(listening, str.c_str(), str.size(), 0);
    // cout << "dd = " << dd <<"\n";
    int read_msg = read(listening, buffer, 4096);
    string reply = buffer;
    // cout << "Reply = " << reply << endl;
    if (read_msg == -1)
    {
        cout << "Error in reading from client side.\n";
        exit(4);
    }
    return reply;
}

vector<string> downloading;
string username;
bool isLoggedIn;
unordered_map<string, string> fnameToPath;

void login(string str)
{
    username = str;
    isLoggedIn = 1;
}
void logout()
{
    username = "";
    isLoggedIn = 0;
    fnameToPath.clear();
}

void commands(string a, string b)
{
    return;
}


int main(int argc, char *argv[])
{
    int i;

    if (argc != 3)
    {
        cout << "Required command line input in \"./client <IP>:<PORT> tracker_info.txt0\" ";
        exit(1);
    }

    int ip_port = 0;
    string client_ip, tracker_ip, client_port, tracker_port;
    while (argv[1][ip_port] != ':')
    {
        client_ip.push_back(argv[1][ip_port]);
        ip_port++;
    }
    ip_port++;
    while (argv[1][ip_port] != '\0')
    {
        client_port.push_back(argv[1][ip_port]);
        ip_port++;
    }
    fstream tracker_info;
    tracker_info.open(argv[2]);
    tracker_info >> tracker_ip;
    tracker_info >> tracker_port;
    cout << "Tracker IP Address is " << tracker_ip << " and port number is " << tracker_port << "\n";

    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Can't create a socket on client Side\n";
        exit(1);
    }
    cout << "Client socket Created...\n";

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(stoi(tracker_port.c_str()));
    int cl_pton = inet_pton(AF_INET, "tracker_ip.c_str()", &server_addr.sin_addr);
    if (cl_pton == -1)
    {
        cerr << "Invalid Address.\n";
        exit(2);
    }

    int client_connect = connect(listening, (sockaddr *)&server_addr, sizeof(server_addr));
    if (client_connect == -1)
    {
        cerr << "Connection Failed.\n";
        exit(3);
    }
    cout << "Client Connected with server...\n";

    thread thr(commands, client_ip, client_port);
    char buffer[4096];
    while (true)
    {
        string s;
        getline(cin, s);
        stringstream ss(s);
        string word;
        string argument = s;

        vector<string> command; // Converting each letter from string s into commands[i]
        while (ss >> word)
        {
            command.push_back(word);
        }
        // for(auto it:command){
        //     cout << it <<endl;
        // }
        memset(buffer, 0, 4096);

        if (command.size() == 0)
            continue;

        if (strcmp(command[0].c_str(), "create_user") == 0)
        {
            if (command.size() != 3)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            string reply = send_message(listening, argument);
            cout << reply << "\n";

        }
        else if (strcmp(command[0].c_str(), "login") == 0)
        {
            if (command.size() != 3)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if(isLoggedIn == 1)
            {
                cout << username << "Logging out...\n";
                string reply = send_message(listening, "logout " + username);
                cout << reply << "\n";
                logout();
            }
            string argument =  command[0] + " " + command[1] + " " + command[2] + " " + client_ip + " " + client_port;
            string reply = send_message(listening, argument);
            if(reply[0] == 'H')
            {
                logout();
                login(command[1]);
                vector<string> reply_token;
                cout << command[1] << " logged in\n";
                char *token = strtok(const_cast<char*>(reply.c_str()), " ");
                while(token != NULL)
                {
                    string temp = token;
                    reply_token.push_back(token);
                    token = strtok(NULL, " ");
                }
                for(int i=2; i<reply_token.size(); i+=2)
                {
                    cout << reply_token[i] << " -> " << reply_token[i+1] << "\n";
                    fnameToPath[reply_token[i]] = reply_token[i+1];
                }
            }
            else   
                cout << reply <<"\n";

        }
        else if (strcmp(command[0].c_str(), "logout") == 0)
        {
            if(command.size() != 1){
                cout << "Invalid Argument\n";
                continue;
            }
            else{
                if(isLoggedIn == 0){
                    cout << "Login first to use logout command...\n";
                    continue;
                }
                else if(isLoggedIn == 1)
                {
                    string reply = send_message(listening, argument + " " + username);
                    cout << reply <<"\n";
                    logout();
                }
            }
        }
        
        else if (strcmp(command[0].c_str(), "create_group") == 0)
        {
            if(command.size() != 2)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if(isLoggedIn == 0){
                cout << "Login First..\n";
                continue;
            }
            else
            {
                string argument = "create_group " + command[1] + " by " + username;
                string reply = send_message(listening, argument);
			    cout<<reply<<endl;
            }
        }
        else if (strcmp(command[0].c_str(), "join_group") == 0)
        {
            if(command.size() != 2)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if(isLoggedIn == 0){
                cout << "Login First..\n";
                continue;
            }
            else
            {
                string argument = username + " Joined_group " + command[1];
                string reply = send_message(listening, argument);
			    cout<<reply<<endl;
            }
        }
        else if (strcmp(command[0].c_str(), "leave_group") == 0)
        {
            if(command.size() != 2)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if(isLoggedIn == 0){
                cout << "Login First..\n";
                continue;
            }
            else
            {
                string argument = "leave_group" + command[1] + " by " + username;
                string reply = send_message(listening, argument);
			    cout<<reply<<endl;
            }
        }
        else if (strcmp(command[0].c_str(), "list_requests") == 0)
        {
            if(command.size() != 2)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if(isLoggedIn == 0){
                cout << "Login First..\n";
                continue;
            }
            else
            {
                string argument = "list_requests of group " + command[1] + " by " + username;
                string reply = send_message(listening, argument);
			    cout<<reply<<endl;
            }
        }
        else if (strcmp(command[0].c_str(), "accept_request") == 0)
        {
            if(command.size() != 3)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if(isLoggedIn == 0){
                cout << "Login First..\n";
                continue;
            }
            else
            {
                string argument = "accept_requests of " + command[2] + " in group " + command[1] + " by " + username;
                string reply = send_message(listening, argument);
			    cout<<reply<<endl;
            }
        }
        else if (strcmp(command[0].c_str(), "list_groups") == 0)
        {
            if(command.size() != 1)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if(isLoggedIn == 0){
                cout << "Login First..\n";
                // continue;
            }
            else
            {
                string reply = send_message(listening, "list_groups " + username);
			    cout<<reply<<endl;
            }
        }
        else if (strcmp(command[0].c_str(), "list_files") == 0)
        {
            if(command.size() != 2)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if(isLoggedIn == 0){
                cout << "Login First..\n";
                // continue;
            }
            else
            {
                argument += " " + username;
                string reply = send_message(listening, argument);
			    cout<<reply<<endl;
            }
        }
        else if (strcmp(command[0].c_str(), "upload_file") == 0)
        {
        }
        else if (strcmp(command[0].c_str(), "download_file") == 0)
        {
        }
        else if (strcmp(command[0].c_str(), "show_downloads") == 0)
        {
        }
        else if (strcmp(command[0].c_str(), "stop_share") == 0)
        {
        }
        else
        {
            cout << "Wrong Input\n";
        }

        // int n = write(listening, buffer, strlen(buffer));
        // if(n == -1)
        // {
        //     cout << "Error in writing from client side.\n";
        //     exit(4);
        // }
        // memset(buffer, 0, 4096);
        // n = read(listening, buffer, 4096);
        // if(n == -1)
        // {
        //     cout << "Error in reading from client side.\n";
        //     exit(4);
        // }
        // cout << "\nServer --> " << buffer;
        // if(!strncmp("exit", buffer, 4)){
        //     close(listening);
        //     cout << "Socket Connection Closed...\n";
        //     // break;
        //     exit(1);
        // }
    }
    thr.join();
    close(listening);
    cout << "Socket Connection Closed...\n";

    return 0;
}

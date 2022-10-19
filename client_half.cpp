#include <sys/socket.h>
#include <sys/types.h> // Also Used for sockets.
#include <string.h>    // Used for memset
#include <iostream>
#include <netdb.h> // getnameinfo  - address-to-name translation in protocol-independent manner
#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h> // Close
#include <unordered_map>
#include <openssl/sha.h>

#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
#define KB(x) ((size_t)(x) << 10)
#define chunksize 524288
string home_dir = "/home/yash";
char ch[4100];
string cur_direc;

vector<string> split(string st, string slash)
{
    string str;
    size_t loc = 0;
    vector<string> res;
    while ((loc = st.find(slash)) != string::npos)
    {
        str = st.substr(0, loc);
        res.push_back(str);
        st.erase(0, loc + slash.length());
    }
    res.push_back(st);
    return res;
}

string absolute_path(string path)
{
    string pwd = cur_direc;
    string abs_path = "";
    if (path[0] == '~')
        abs_path = string(home_dir) + path.substr(1, path.length() - 1);

    else if (path[0] == '.' && path[1] == '/')
        abs_path = pwd + "/" + path.substr(2, path.length() - 2);

    else if (path[0] == '/')
        abs_path = path;

    else
        abs_path = pwd + "/" + path;

    return abs_path;
}

mode_t check_mode(string path)
{
    struct stat str;
    stat(path.c_str(), &str);
    mode_t mode = 0;

    mode = mode | ((str.st_mode & S_IRGRP) ? 0040 : 0);
    mode = mode | ((str.st_mode & S_IWGRP) ? 0020 : 0);
    mode = mode | ((str.st_mode & S_IXGRP) ? 0010 : 0);
    mode = mode | ((str.st_mode & S_IROTH) ? 0004 : 0);
    mode = mode | ((str.st_mode & S_IWOTH) ? 0002 : 0);
    mode = mode | ((str.st_mode & S_IXOTH) ? 0001 : 0);
    mode = mode | ((str.st_mode & S_IRUSR) ? 0400 : 0);
    mode = mode | ((str.st_mode & S_IWUSR) ? 0200 : 0);
    mode = mode | ((str.st_mode & S_IXUSR) ? 0100 : 0);

    return mode;
}

void copy_file(vector<string> str)
{
    getcwd(ch, sizeof(ch));
    cur_direc = ch;
    string dest = absolute_path(str[str.size() - 1]);
    int i;
    for (i = 1; i < str.size() - 1; i++)
    {
        string source = absolute_path(str[i]);
        vector<string> source_files = split(str[i], "/");
        string destination = dest + "/" + source_files[source_files.size() - 1];

        mode_t mode = check_mode(source);
        int src_open = open(source.c_str(), O_RDONLY);
        int des_open = open(dest.c_str(), O_CREAT | O_WRONLY, mode);
        char c;
        if (src_open == -1)
        {
            cout << "Error in Opening Source File";
            return;
        }
        if (des_open == -1)
        {
            cout << "File Already Exist";
            return;
        }
        while (read(src_open, &c, 1))
            write(des_open, &c, 1);

        close(src_open);
        close(des_open);
        return;
    }

    return;
}

string send_message(int listening, string str)
{
    char buffer[4096];
    memset(buffer, 0, 4096);
    // cout << "str = " << str << "\n";
    send(listening, str.c_str(), str.size(), 0);
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
unordered_map<string, string> filenameToPath;

void login(string str)
{
    username = str;
    isLoggedIn = 1;
}
void logout()
{
    username = "";
    isLoggedIn = 0;
    filenameToPath.clear();
}

void commands(string a, string b)
{
    return;
}

int main(int argc, char *argv[])
{
    // int i;

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
    cout << "Client IP Address is " << client_ip << " and port number is " << client_port << "\n";

    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Can't create a socket on client Side\n";
        exit(1);
    }
    cout << "Client socket Created...\n";

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    // server_addr.sin_port = htons(54000);
    // int cl_pton = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    server_addr.sin_port = htons(stoi(tracker_port.c_str()));
    int cl_pton = inet_pton(AF_INET, "tracker_ip.c_str()", &server_addr.sin_addr);
    // server_addr.sin_addr.s_addr = inet_addr(tracker_ip.c_str());
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
        // Clear the buffer
        // memset(buffer, 0, 4096);
        // fgets(buffer, 4096, stdin);
        // cout << "buffer = " << buffer << "\n";
        string argument = s;

        vector<string> command; // Converting each letter from string s into commands[i]
        while (ss >> word)
        {
            command.push_back(word);
        }
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
            if (isLoggedIn == 1)
            {
                cout << username << "Logging out...\n";
                string reply = send_message(listening, "logout " + username);
                cout << reply << "\n";
                logout();
            }
            string argument = command[0] + " " + command[1] + " " + command[2] + " " + client_ip + " " + client_port;
            string reply = send_message(listening, argument);
            if (reply[0] == 'H')
            {
                logout();
                login(command[1]);
                vector<string> reply_token;
                cout << command[1] << " logged in\n";
                char *token = strtok(const_cast<char *>(reply.c_str()), " ");
                while (token != NULL)
                {
                    string temp = token;
                    reply_token.push_back(token);
                    token = strtok(NULL, " ");
                }
                for (auto i = 2; i < reply_token.size(); i += 2)
                {
                    filenameToPath[reply_token[i]] = reply_token[i + 1];
                    cout << reply_token[i] << " -> " << reply_token[i + 1] << "\n";
                }
            }
            else
                cout << reply << "\n";
        }
        else if (strcmp(command[0].c_str(), "logout") == 0)
        {
            if (command.size() != 1)
            {
                cout << "Invalid Argument\n";
                continue;
            }
            else
            {
                if (isLoggedIn == 0)
                {
                    cout << "Login first to use logout command...\n";
                    continue;
                }
                else if (isLoggedIn == 1)
                {
                    string reply = send_message(listening, argument + " " + username);
                    cout << reply << "\n";
                    logout();
                }
            }
        }

        else if (strcmp(command[0].c_str(), "create_group") == 0)
        {
            if (command.size() != 2)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if (isLoggedIn == 0)
            {
                cout << "Login First..\n";
                continue;
            }
            else
            {
                string argument = "create_group " + command[1] + " by " + username;
                string reply = send_message(listening, argument);
                cout << reply << endl;
            }
        }
        else if (strcmp(command[0].c_str(), "join_group") == 0)
        {
            if (command.size() != 2)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if (isLoggedIn == 0)
            {
                cout << "Login First..\n";
                continue;
            }
            else
            {
                string argument = username + " Joined_group " + command[1];
                string reply = send_message(listening, argument);
                cout << reply << endl;
            }
        }
        else if (strcmp(command[0].c_str(), "leave_group") == 0)
        {
            if (command.size() != 2)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if (isLoggedIn == 0)
            {
                cout << "Login First..\n";
                continue;
            }
            else
            {
                string argument = "leave_group " + command[1] + " by " + username;
                string reply = send_message(listening, argument);
                cout << reply << endl;
            }
        }
        else if (strcmp(command[0].c_str(), "list_requests") == 0)
        {
            if (command.size() != 2)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if (isLoggedIn == 0)
            {
                cout << "Login First..\n";
                continue;
            }
            else
            {
                string argument = "list_requests of group " + command[1] + " by " + username;
                string reply = send_message(listening, argument);
                cout << reply << endl;
            }
        }
        else if (strcmp(command[0].c_str(), "accept_request") == 0)
        {
            if (command.size() != 3)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if (isLoggedIn == 0)
            {
                cout << "Login First..\n";
                continue;
            }
            else
            {           // accept_request <group_id> <user_id> username  
                string argument = "accept_request of " + command[2] + " in group " + command[1] + " by " + username;
                string reply = send_message(listening, argument);
                cout << reply << endl;
            }
        }
        else if (strcmp(command[0].c_str(), "list_groups") == 0)
        {
            if (command.size() != 1)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if (isLoggedIn == 0)
            {
                cout << "Login First..\n";
                // continue;
            }
            else
            {
                string reply = send_message(listening, "list_groups " + username );
                cout << reply << endl;
            }
        }
        else if (strcmp(command[0].c_str(), "list_files") == 0)
        {
            if (command.size() != 2)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if (isLoggedIn == 0)
            {
                cout << "Login First..\n";
                // continue;
            }
            else
            {
                argument += " " + username;
                string reply = send_message(listening, argument);
                cout << reply << endl;
            }
        }
        else if (strcmp(command[0].c_str(), "upload_file") == 0)
        {
            if (command.size() != 3)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if (isLoggedIn == 0)
                cout << "Login First..\n";
            else if (isLoggedIn == 1)
            {
                string file_path = command[1];
                string file_name = "";
                int size_of_file_path = file_path.size();
                int i = size_of_file_path - 1;
                while (file_path[i--] != '/')
                    ;
                i += 2;
                while (i < size_of_file_path)
                    file_name += file_path[i++];

                FILE *fp = fopen(file_path.c_str(), "r");
                if (fp == NULL)
                {
                    cout << "File not found..\n";
                    return -1;
                }
                fseek(fp, 0L, SEEK_END);
                long int file_size = ftell(fp);
                fclose(fp);
                cout << "File size of " << file_path << " is " << file_size << "\n";
                cout << "File Name is -> " << file_name << "\n";
                if (file_size >= 0)
                {
                    // size_t chunk_size = KB(512);
                    size_t chunk_size = chunksize;
                    cout << "Chunk Size = " << chunk_size << "\n";
                    char *chunk = new char[chunk_size];
                    filenameToPath[file_name] = file_path;

                    ifstream fin;
                    fin.open(file_path);
                    vector<string> sha1;
                    while (fin)
                    {
                        fin.read(chunk, chunk_size);
                        if (!fin.gcount()){
                            cout << "Error...\n";
                            break;
                        }
                        unsigned char hash[20];                                                        // char limit is 128, while unsigned limit is 256 and SHA is of length 160
                        SHA1(reinterpret_cast<const unsigned char *>(chunk), sizeof(chunk) - 1, hash); // reinterpret_cast is used to convert a pointer of some data type into a pointer of another data type, even if the data types before and after conversion are different.
                        string c_hash(reinterpret_cast<char *>(hash));
                        sha1.push_back(c_hash);
                        cout << "Pushed " << hash << "\n";
                    }
                    // upload_file <file_path> <group_id>
                    cout << argument << "\n";
                    argument += " " + username + " " + file_name + " " + file_path + " " + to_string(file_size) + " ";
                    for (auto i = 0; i < sha1.size(); i++)
                        argument += sha1[i] + " ";
                    string reply = send_message(listening, argument);
                    cout << reply << "\n";
                }
            }
        }
        else if (strcmp(command[0].c_str(), "download_file") == 0)
        {
        }
        else if (strcmp(command[0].c_str(), "show_downloads") == 0)
        {
            if (command.size() != 1)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if (isLoggedIn == 0)
                cout << "Login First..\n";
            else if (isLoggedIn == 1)
            {
                for(int i = 0; i < downloading.size(); i++)
                    cout << "[D] " << downloading[i] << "\n";
                for(auto i = filenameToPath.begin(); i != filenameToPath.end(); i++)
                    cout << "[C] " << i->first << "\n";
            }
        }
        else if (strcmp(command[0].c_str(), "stop_share") == 0)
        {
            if (command.size() != 3)
            {
                cout << "Invalid Arguments\n";
                continue;
            }
            if (isLoggedIn == 0)
                cout << "Login First..\n";
            else if (isLoggedIn == 1)
            {
                argument += " " + username;
                string reply = send_message(listening, argument);
            }
        }
        else
        {
            cout << "Wrong Input\n";
        }

    }
    thr.join();
    close(listening);
    cout << "Socket Connection Closed...\n";

    return 0;
}

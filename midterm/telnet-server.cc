#include "../common.h"
#include <iostream>
#include <string>
#include <dirent.h>
#include <vector>


static char cwd[PATH_MAX];

void initialize()
{
    if (getcwd(cwd, sizeof(cwd)) == NULL) 
    {
        fprintf(stderr, "getcwd() error\n");
        exit(1);
    }
}


std::vector<std::string> list_dir()
{
    std::vector<std::string> files;

    auto dir = opendir(cwd);
    
    if (dir != nullptr)
    {
        while (auto entry = readdir(dir))
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            files.push_back(entry->d_name);
        }
    }
    closedir(dir);

    return files;
}


void change_dir(std::string &str) 
{
    size_t first = str.find_first_of(' ');
    if (first != 2)
        return;
    
    size_t last = str.find_last_of(' ');
    if (last != first)
        return;
    
    std::string dir = str.substr(first+1, str.size()-3);
    
    if (dir == ".")
        return;
    else if (dir == "..")
    {
        chdir("..");
        getcwd(cwd, sizeof(cwd));
    }
    else
    {
        if (chdir(dir.c_str()) == 0)
        {
            getcwd(cwd, sizeof(cwd));
        }
    }
}


int main(int argc, char **argv)
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);

    // we can reuse the server addr if we ctrl-c the server side
    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0)
    {
        fprintf(stderr, "server bind failed\n");
        exit(1);
    }

    int rt2 = listen(listen_fd, LISTENQ);
    if (rt2 < 0)
    {
        fprintf(stderr, "server listen failed\n");
        exit(1);
    }

    int conn_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    char message[MAXLINE];

    initialize();
    
    // outer loop is for not stopping the server if client closes
    for (;;)
    {
        // clear the client_addr for a client to connect
        bzero(&client_addr, client_len);

        if ((conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) 
        {
            fprintf(stderr, "server accept client failed\n");
            exit(1);
        }

        fprintf(stdout, "one client connected\n");

        for (;;)
        {
            memset(&message[0], 0, MAXLINE);
            
            int n = read(conn_fd, message, MAXLINE);
            
            if (n < 0)
            {
                fprintf(stderr, "server read error\n");
                exit(1);
            }
            else if (n == 0)
            {
                break;
            }
            
            std::string command = std::string(message);
            
            if (command == "pwd")
            {
                int write_nc = send(conn_fd, cwd, strlen(cwd), 0);
                if (write_nc < 0)
                {
                    fprintf(stderr, "sending pwd failed\n");
                    exit(1);
                }
            }
            else if (command == "ls")
            {
                std::vector<std::string> files = list_dir();

                std::string rep = "";
                for (auto name : files)
                {
                    rep += name + "\n";
                }
                
                int write_nc = send(conn_fd, rep.c_str(), strlen(rep.c_str()), 0);

                if (write_nc < 0)
                {
                    fprintf(stderr, "sending ls failed\n");
                    exit(1);
                }
            }
            else if (command.size() > 3 && command.substr(0, 2) == "cd" && command.substr(2, 1) == " ")
            {
                change_dir(command);
            }
        }
        
        sleep(1);
    }
    
    return 0;
}

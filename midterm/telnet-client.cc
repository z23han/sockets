#include "../common.h"
#include <string>


std::string trim(const std::string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
        return str;

    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last-first+1);
}


int main(int argc, char **argv)
{
    if (argc != 3) 
    {
        fprintf(stderr, "usage: telnet-client <IPaddress> <Port>\n");
        exit(1);
    }

    const char *IP = argv[1];
    const int PORT = atoi(argv[2]);

    // create server socket
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // set up server_addr 
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, IP, &server_addr.sin_addr);

    // try to connect to server
    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *)&server_addr, server_len);
    if (connect_rt < 0) 
    {
        fprintf(stderr, "connect to server failed\n");
        exit(1);
   }

    char send_line[MAXLINE], recv_line[MAXLINE];
    int n;

    fd_set readmask;
    fd_set allreads;
    
    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);

    for (;;) 
    {
        // reset readmask every time in the loop
        readmask = allreads;
        
        int rc = select(socket_fd+1, &readmask, NULL, NULL, NULL);
        if (rc < 0)
        {
            fprintf(stderr, "select error\n");
            exit(1);
        } 
        
        if (FD_ISSET(socket_fd, &readmask)) 
        {
            n = read(socket_fd, recv_line, MAXLINE);
            if (n < 0)
            {
                fprintf(stderr, "read error\n");
                exit(1);
            }
            else if (n == 0)
            {
                fprintf(stdout, "server terminated\n");
                exit(0);
            }

            // display server response on screen
            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }

        if (FD_ISSET(0, &readmask))
        {
            // get user input from prompt 
            if (fgets(send_line, MAXLINE, stdin) != NULL)
            {
                size_t len = strlen(send_line);
                send_line[len-1] = ' ';
                
                // we need to do a trimming for the send_line 
                std::string input = trim(std::string(send_line));
                
                if (input == "quit")
                {
                    FD_CLR(0, &allreads);

                    if (shutdown(socket_fd, SHUT_RDWR))
                    {
                        fprintf(stderr, "quit failed\n");
                        exit(1);
                    }
                }

                size_t rt = write(socket_fd, input.c_str(), strlen(send_line));
                
                if (rt < 0) 
                {
                    fprintf(stderr, "client write error\n");
                    exit(1);
                }
            }
        }
    }

    close(socket_fd);

    return 0;
}

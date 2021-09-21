#include "../common.h"

static int count;

static void sig_int(int signo) {
    printf("\nrecevied %d datagrams\n", count);
    exit(0);
}


size_t read_message(int fd, char *buffer, size_t length);
size_t readn(int fd, void *buffer, size_t length);


int main(int argc, char **argv) {

    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        fprintf(stderr, "bind failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    int rt2 = listen(listenfd, LISTENQ);
    if (rt2 < 0) {
        fprintf(stderr, "listen failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    signal(SIGPIPE, SIG_IGN);
    
    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    if ((connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        fprintf(stderr, "connect failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    char buf[128];
    count = 0;

    while (1) {
        int n = read_message(connfd, buf, sizeof(buf));

        if (n < 0) {
            fprintf(stderr, "read error: %s (%d)\n", strerror(errno), errno);
            exit(1);
        } else if (n == 0) {
            fprintf(stdout, "client closed\n");
            exit(0);
        }

        buf[n] = 0;
        printf("received %d bytes: %s\n", n, buf);

        count++;
    }

    return 0;
}


size_t read_message(int fd, char *buffer, size_t length) {

    u_int32_t msg_length;
    u_int32_t msg_type;
    int rc;

    rc = readn(fd, (char *)&msg_length, sizeof(u_int32_t));
    if (rc != sizeof(u_int32_t)) {
        return rc < 0 ? -1 : 0;
    }
    msg_length = ntohl(msg_length);
    
    rc = readn(fd, (char *)&msg_type, sizeof(u_int32_t));
    if (rc != sizeof(u_int32_t)) {
        return rc < 0 ? -1 : 0;
    }

    if (msg_length > length) {
        return -1;
    }
    
    rc = readn(fd, buffer, msg_length);
    if (rc != msg_length) 
        return rc < 0 ? -1 : 0;

    return rc;
}


size_t readn(int fd, void *buffer, size_t length) {
    
    size_t count;
    ssize_t nread;
    char *ptr;
    
    ptr = (char *)buffer;
    count = length;

    while (count > 0) {
        nread = read(fd, ptr, count);
        
        if (nread < 0) {
            if (errno == EINTR) 
                continue;
            else
                return -1;
        } else if (nread == 0) {
            break;
        }

        count -= nread;
        ptr += nread;
    }
    return (length - count);
}


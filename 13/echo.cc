#include "../common.h"


void loop_echo(int fd)
{
    char outbuf[MAXLINE];
    size_t outbuf_used = 0;
    ssize_t result;

    while (1)
    {
        char ch;
        result = recv(fd, &ch, 1, 0);

        if (result == 0)
            break;
        else if (result == -1)
        {
            fprintf(stderr, "read failed\n");
            exit(1);
        }

        if (outbuf_used < sizeof(outbuf)) 
        {
            outbuf[outbuf_used++] = ch;
        }

        if (ch == '\n')
        {
            send(fd, outbuf, outbuf_used, 0);
            outbuf_used = 0;
            continue;
        }
    }
}


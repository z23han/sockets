#include "../common.h"

extern struct acceptor *acceptor_init(int);


int onConnectionCompleted(struct tcp_connection *tcpConnection)
{
    printf("connection completed\n");
    return 0;
}


int onMessage(struct buffer *input, struct tcp_connection *tcpConnection)
{
    printf("get message from tcp connection %s\n", tcpConnection->name);
    printf("%s", input->data);

    struct buffer *output = buffer_new();
    int size = buffer_readable_size(input);

    for (int i = 0; i < size; ++i)
    {
        buffer_append_char(output, buffer_read_char(input));
    }

    tcp_connection_send_buffer(tcpConnection, output);
    return 0;
}


int onWriteCompleted(struct tcp_connection *tcpConnection)
{
    printf("write completed\n");
    return 0;
}


int onConnectionClosed(struct tcp_connection *tcpConnection)
{
    printf("connection closed\n");
    return 0;
}



int main(int argc, char **argv)
{
    // initialize main thread - event loop
    struct event_loop *eventLoop = event_loop_init();

    // initialize acceptor
    struct acceptor *acceptor = acceptor_init(SERV_PORT);
    
    struct TCPserver *tcpServer = tcp_server_init(eventLoop, acceptor, onConnectionCompleted, onMessage, onWriteCompleted, onConnectionClosed, 0);
    
    tcp_server_start(tcpServer);

    event_loop_run(eventLoop);

    return 0;
}

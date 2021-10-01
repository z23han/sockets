#include "common.h"

int main() 
{
    void *ctx = zmq_ctx_new();
    void *requester = zmq_socket(ctx, ZMQ_REQ);
    
    zmq_connect(requester, frontend_addr.c_str());

    char message[16];

    for (int i = 0; i < 100; ++i) 
    {
        zmq_send(requester, "hello", 5, ZMQ_SNDMORE);
        
        memset(&message, 0, sizeof(message));

        zmq_recv(requester, &message, sizeof(message), ZMQ_DONTWAIT);

        std::cout << "received reply " << std::string(message) << std::endl;

        sleep(1);
    }
    
    return 0;
}

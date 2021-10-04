#ifndef BUFFER_H
#define BUFFER_H

#define MAX_LINE 1024
#define FD_INIT_SIZE 128


struct Buffer 
{
    int connect_fd;
    char buffer[MAX_LINE];
    size_t writeIndex;
    size_t readIndex;
    int readable;
};


struct Buffer *alloc_Buffer()
{
    struct Buffer *buffer = (struct Buffer *)malloc(sizeof(struct Buffer));
    
    if (!buffer)
        return nullptr;

    buffer->connect_fd = 0;
    buffer->writeIndex = buffer->readIndex = buffer->readable = 0;

    return buffer;
}


void free_Buffer(struct Buffer *buffer)
{
    free(buffer);
}


#endif // BUFFER_H

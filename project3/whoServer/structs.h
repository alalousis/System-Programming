struct arguments{
    int bufferSize;
    char workerIP[32];
};

struct circularBuffer{
        int *fds;
        int start;
        int end;
        int count;
};

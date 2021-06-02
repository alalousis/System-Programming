pthread_mutex_t mtx, mtx2;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
struct circularBuffer cirBuff;
int flag;
int numWorkers;
int *workerPorts;

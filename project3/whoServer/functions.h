/*  threads  */
void terror(char *, int);
void *threadRoutine(void *);
void *mainThreadRoutine(void *);

/*  Circular Buffer  */
void init(struct circularBuffer *, int);
void place(struct circularBuffer *, int, int);
int obtain(struct circularBuffer *, int);

/*  Sockets  */
int receiveStatistics(int);
int sendQuery(char *, int *, char *, int, int *);

/*  Receive Results  */
int diseaseFrequencyReceive(int *, int);
int topAgeRangesReceive(int *, int);
int searchByIDReceive(int *, int);
int numPatientAdmissionsReceive(int *, int);
int numPatientDischargesReceive(int *, int);

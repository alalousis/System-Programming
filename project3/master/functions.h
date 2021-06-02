/*  Input  */
void saveRecords(struct dateFileNode *, struct listNode **, int, char[64], char **, int);
void readRecords(char*, char *,FILE *, struct listNode **, int);
int countCountries(char[64]);
void saveCountries(char[64], char **);
int countryOfWorker(char **, int, char *);
void sortDateFiles(char **, int);

/*    Hash Functions  */
int recordsHash(struct listNode **, int, char[8][20]);
int checkRecordID(struct listNode **, int, char *, char *, char *);
void Hash(struct bucket **, int, char *, int, struct record *);
int hashFunction(char *, int);

/*  Date functions */
void getdate(struct date *, char *);
int datecmp(struct date, struct date);
int checkDate(char *, char *, char *, int);

/*  pipes */
void sendServerIPnPORT(int, char *, int, char *, char *, int **);
void receiveServerIPnPORT(char *, int, char *, int *, int);
void assignCountries(char *, int, char *, int, int **);
void decodeCountries(int, char *, int, char **, int);
void sendCommand(int **, fd_set *, int, char *, char *, int, int);
void receiveCommand(int, char [8][32]);

/*  sockets  */
void sendStatistics(int, int, char *, char *);

/* Tree Functions */
void insertTree(struct treeNode **, struct record *, struct date);
void balance(struct treeNode **);
int maxHeight(int, int);
struct treeNode * rotateR(struct treeNode **);
struct treeNode * rotateL(struct treeNode **);
struct treeNode * rotateRL(struct treeNode **);
struct treeNode * rotateLR(struct treeNode **);

/* Queue Functions */
void init(struct queueNode **);
void push(struct queueNode **, struct heapNode *);
struct heapNode * pop(struct queueNode **);
int isEmpty(struct queueNode *);

/* Heap Functions  */
void insertHeap(struct heapNode **root, int value, char *key);
struct heapNode * findHeapNode(struct heapNode *, struct queueNode **);
void heapify(struct heapNode **, int *);
struct heapNode * max(struct heapNode *, struct heapNode *);
void swap(struct heapNode **, struct heapNode **);
struct heapNode * top(struct heapNode **);

/* Signals Functions  */
void logfileParent(char[64], int, int, int);
void logfileChild(char **, int, int, int, int);
void sigusr1(struct dateFileNode *, char[64], char **, int, struct listNode **, int);
int dateOfWorker(struct dateFileNode *, char *);

/* Commands */
void statisticsSend(struct bucket **, int, struct bucket **, int, int, char *, int);
void statisticsReceive(int **, char *, int, int);
void listCountries(char **, int, char *, int);
int listCountriesReceive(int **, fd_set *, int, char *, int, int);
void diseaseFrequency(struct bucket **, int, int, char *, char *, char *, char *, char *, int);
int diseaseFrequencyReceive(int **, fd_set *, int, int , char *, int);
void topAgeRanges(struct bucket **, int, int, int, char*, char *, char *, char *, char *, int);
int topAgeRangesReceive(int **, fd_set *, int, int , char *, int);
void searchByID(struct listNode **, int, char *, char *, int);
int searchByIDReceive(int **, fd_set *, int, int, char *, int );
void numPatientAdmissions(struct bucket **, int, int, char *, char *, char *, char *, char *, int);
int numPatientAdmissionsReceive(int **, fd_set *, int, int, char *, int);
void numPatientDischarges(struct bucket **, int, int, char *, char *, char *, char *, char *, int);
int numPatientDischargesReceive(int **, fd_set *, int, int, char *, int);

void countTreeNodes(struct treeNode *, int *, struct date, struct date);
void countTreeNodesWithDisease(struct treeNode *, int *, struct date, struct date, char *);
void countTreeNodesInCountry(struct treeNode *, int *, struct date, struct date, char *);
void countTreeNodesInCountryInRange(struct treeNode *, int *, struct date, struct date, char *);
void countTreeNodesWithDiseaseInRange(struct treeNode *, int *, struct bucket **, int, int, char *, int);
void countDischarges(struct treeNode *, int *, struct date, struct date);
void countDischargesInCountry(struct treeNode *, int *, struct date, struct date, char *);

/* Exit Functions */
void freeRecordsHashTable(struct listNode **, int);
void freeHashTable(struct bucket **, int, int);
void freeBinaryTree(struct treeNode *);
void freeHeap(struct heapNode *);

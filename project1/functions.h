/* Hashing Functions */
int saveRecords(FILE *, struct listNode **, int);
int checkRecordID(struct listNode **, int, char *);
int recordsHash(struct listNode **, int, char[10][20]);
void Hash(struct bucket **, int, char *, int, struct record *);
int hashFunction(char *, int HashtableSize);

/* Tree Functions */
void insertTree(struct treeNode **, struct record *, struct date);
void balance(struct treeNode **);
int maxHeight(int, int);
struct treeNode * rotateR(struct treeNode **);
struct treeNode * rotateL(struct treeNode **);
struct treeNode * rotateRL(struct treeNode **);
struct treeNode * rotateLR(struct treeNode **);

/* Date Functions */
void getdate(struct date*, char*);
int datecmp(struct date, struct date);

/* Commands */
void globalDiseaseStats(struct bucket **, int, int, char *, char *);
void diseaseFrequency(struct bucket **, int, int, char *, char *, char *, char *);
void topDiseases(struct bucket **, int, int, int, char *, char *, char *);
void topCountries(struct bucket **, int, int, int, char *, char *, char *);
struct record * searchByID(struct listNode **, int, char *);
void currentPatients(struct bucket **, int, char *, int);
void countTreeNodesWithoutExitDate(struct treeNode *, int *);
void countTreeNodes(struct treeNode *, int *, struct date, struct date);
void countTreeNodesWithDisease(struct treeNode *, int *, struct date, struct date, char *);
void countTreeNodesInCountry(struct treeNode *, int *, struct date, struct date, char *);

/* Queue Functions */
void init(struct queueNode **);
void push(struct queueNode **, struct heapNode *);
struct heapNode * pop(struct queueNode **);
int isEmpty(struct queueNode *);

/* Heap Functions*/
void insertHeap(struct heapNode **root, int value, char *key);
struct heapNode * findHeapNode(struct heapNode *, struct queueNode **);
void heapify(struct heapNode **, int *);
struct heapNode * max(struct heapNode *, struct heapNode *);
void swap(struct heapNode **, struct heapNode **);
struct heapNode * top(struct heapNode **);

/* Exit Functions */
void freeRecordsHashTable(struct listNode **, int);
void freeHashTable(struct bucket **, int, int);
void freeBinaryTree(struct treeNode *);
void freeHeap(struct heapNode *);

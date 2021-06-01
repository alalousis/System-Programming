struct record{
    char* recordID;
    char* patientFirstName;
    char* patientLastName;
    char* diseaseID;
    char* country;
    char* entryDate;
    char* exitDate;
};

struct listNode{
    struct record *record;
    struct listNode *nextRecord;
};

struct bucket{
    char **keys;
    struct treeNode **values;
    struct bucket *nextBucket;
};

struct date{
    int day;
    int month;
    int year;
};

struct treeNode{
    struct date date;
    struct listNode *recordsList;
    struct treeNode *leftChild;
    struct treeNode *rightChild;
    int height;
};

struct heapNode{
    int value;
    char key[16];
    struct heapNode *leftChild;
    struct heapNode *rightChild;
};

struct queueNode{
    struct heapNode *heapNode;
    struct queueNode *nextQueueNode;
};

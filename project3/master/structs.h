struct record{
    char *recordID;
    char *condition;
    char *patientFirstName;
    char *patientLastName;
    char *disease;
    unsigned int age;
    char *country;
    char *entryDate;
    char *exitDate;
};

struct listNode{
    struct record *record;
    struct listNode *nextRecord;
};

struct dateFileNode{
    char *fileName;
    struct dateFileNode *nextDateFileNode;
};

struct date{
    int day;
    int month;
    int year;
};

struct bucket{
    char **keys;
    struct treeNode **values;
    struct bucket *nextBucket;
};

struct summary{
    char *country;
    char **diseases;
    char **dates;
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

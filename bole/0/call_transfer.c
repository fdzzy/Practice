#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef unsigned int uint32_t;

typedef struct transfer_t_ {
    int num;
    int to;
    uint32_t start;
    uint32_t length;
    int flag;   // see below flag defines
    int transCount;
    struct transfer_t_ *listNext;
    struct transfer_t_ *listPrev;
    struct transfer_t_ *hashNext;
    struct transfer_t_ *transNext;
} transfer_t; 

// transfer_t constants 
#define TRANSFER_TRANS_COUNT_INVALID -1
#define HAS_TRANSFER_PREV 1

transfer_t* trans_list = NULL;
transfer_t** trans_hash;

uint32_t    count = 0;
uint32_t    day;
int         hashTableSize = 0;
int         maxTransfer = 0;

int buildHashTable(int lines)
{
    if(hashTableSize)
        return hashTableSize;

    if(lines <= 0) {
        printf("Error: lines must be great than 0!\n");
        exit(1);
    } else if(lines < 100) {
        hashTableSize = 53;
    } else if(lines < 1000) {
        hashTableSize = 389;
    } else if(lines < 10000) {
        hashTableSize = 3079;
    } else if(lines < 100000) {
        hashTableSize = 24593;
    } else if(lines < 1000000) {
        hashTableSize = 393241;
    } else {
        hashTableSize = 3145739;
    }

    trans_hash = calloc(sizeof(transfer_t*), hashTableSize);
    if(!trans_hash) {
        printf("No enough memory!\n");
        exit(1);
    }

    return hashTableSize;
}

void insertIntoHashTable(transfer_t* trans)
{
    int hashIndex;
    
    if(!hashTableSize || !trans_hash) {
        printf("Hash table not established!\n");
        exit(1);
    }

    hashIndex = trans->num % hashTableSize;
    trans->hashNext = trans_hash[hashIndex]; 
    trans_hash[hashIndex] = trans;
}

transfer_t* hashFind(int number)
{
    transfer_t* trans;
    
    if(!hashTableSize) {
        printf("Hash table not established!\n");
        exit(1);
    }

    trans = trans_hash[(number % hashTableSize)]; 

    while(trans) {
        if(trans->num == number)
            return trans;
        trans = trans->hashNext;
    }

    // not found
    return NULL;
}

void init(void)
{
    int  i, lines;
    transfer_t *trans, *tmp;
    FILE *fp = fopen("input.txt", "r");

    if(!fp) {
        printf("Open file failed: %s\n", strerror(errno));
        exit(1);
    }

    fscanf(fp, "%d", &lines);
    count = lines;
    //printf("DEBUG: Total number of lines: %d\n", lines);
    for(i=0; i<lines ; i++) {
        trans = calloc(sizeof(transfer_t), 1); 
        fscanf(fp, "%d %d %d %d", &trans->num, &trans->to,
                    &trans->start, &trans->length);
        // set transCount to be invalid
        trans->transCount = TRANSFER_TRANS_COUNT_INVALID;
        // insert into transfer list
        trans->listNext = trans_list;   
        trans->listPrev = NULL;
        if(trans_list)
            trans_list->listPrev = trans;
        trans_list = trans;
    }
    fscanf(fp, "%d", &day);
    //printf("DEBUG: The day to consider: %d\n", day);

    fclose(fp);    

    // Remove all the transfers that don't include the specific day 
    // Also establish the hash table 
    buildHashTable(lines);
    trans = trans_list;
    while(trans) {
        // check if 'day' is in the range
        if( (day >= trans->start) && 
            (day < (trans->start + trans->length)) ) {
            insertIntoHashTable(trans);
            trans = trans->listNext;
        } else {
            // remove from transfer list
            if(trans->listPrev) {
                trans->listPrev->listNext = trans->listNext;
            } else {
                trans_list = trans->listNext;
            }
            if(trans->listNext) {
                trans->listNext->listPrev = trans->listPrev;
            }
            tmp = trans;
            trans = trans->listNext;
            free(tmp);
            --count;
        }
    }
    
}

void buildTransferChain()
{
    transfer_t *trans, *toTrans;
    
    trans = trans_list;
    while(trans) {
        toTrans = hashFind(trans->to); 
        if(toTrans) {
            trans->transNext = toTrans;
        } else {
            trans->transNext = NULL;
        }
        trans = trans->listNext;
    }
}

void checkRing()
{
    transfer_t *trans, *pTrans, *pTrans2;
    
    trans = trans_list;
    while(trans) {
        pTrans = pTrans2 = trans;
        while(pTrans2->transNext && pTrans2->transNext->transNext) {
            pTrans = pTrans->transNext;
            pTrans2 = pTrans2->transNext->transNext;
            if(pTrans == pTrans2) {
                printf("Error, found ring: Cross at number %d\n", pTrans->num);
                exit(1);
            }
        }
        trans = trans->listNext;
    }
}

int calcTransCount(transfer_t *trans, int has_trans_prev)
{
    if(has_trans_prev) {
        trans->flag = HAS_TRANSFER_PREV; 
    }

    if(trans->transCount != TRANSFER_TRANS_COUNT_INVALID)
        return trans->transCount;

    if(trans->transNext == NULL) {
        trans->transCount = 1;
        return 0;
    }

    trans->transCount = calcTransCount(trans->transNext, 1) + 1;

    return (trans->transCount);
}

void findMaxTransfer()
{
    int transCount;
    transfer_t *trans;
    
    trans = trans_list;
    while(trans) {
        if(!(trans->flag == HAS_TRANSFER_PREV)) {
            transCount = calcTransCount(trans, 0);
            if(transCount > maxTransfer) {
                maxTransfer = transCount;
            }
        }
        trans = trans->listNext;
    }
}

int main()
{
    init(); 

    printf("Day %d has %d transfer%s\n", day, count, (count==1) ? "" : "s");

    buildTransferChain();
    checkRing();
    findMaxTransfer();
    
    printf("Day %d has a maximum transfer count of %d\n", day, maxTransfer);
    

    return 0;
}

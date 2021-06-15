#ifndef __DEDUPE_H_
#define __DEDUPE_H_

#include <openssl/sha.h>
#include <zlib.h>


typedef unsigned char uc;
typedef unsigned long long int ulli;

struct Node
{
    ulli key;
    size_t value;
    char sha[161];//sha for varification
    size_t duplicate_count;//number of this chunk appear
    struct Node *next;
};

void FileRead(char* filename, uc buffer[]);
void HashTable_create();
void HashTable_insert(struct Node SHA_entry);
void HashTable_display();
ulli SHAtoHashCode(char SHA[]);
ulli HextoDec(char hashcode[]);
float getDedupeRate();
#endif

/*
ui crc = crc32(0L, Z_NULL, 0);//initialize
crc = crc32(crc, buffer, chunksize);//CRC computation
printf("CRC32 value = %lu", crc);
*/
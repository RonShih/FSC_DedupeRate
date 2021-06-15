#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dedupe.h"

//chunksize = {512, 1024, 2048, 4096, 8192, 16384}; //0.5kB, 1kB, 2kB, 4kB, 8kB, 16kB (Unit: Byte)
//gcc test.c -o test -L/usr/local/opt/openssl@1.1/lib -I/usr/local/opt/openssl@1.1/include -lcrypto -lz
//./test 0.5 <linux-5.3.5.tar.xz
//argv[1]: chunk size
//argv[2]: number of entries in the hash table
//argv[3]: file1 name
//tar: 0.5k: DedupeRate = 48.558384%% 4k: DedupeRate = 0.066370%% 8k: DedupeRate = 0.017488%%

struct Node** HashTable;//structural array for hash table
size_t chunksize = 0, numofEntries = 0, max_dup = 0, max_list = 0, *entry_total_chunks, *entry_unique_chunks; 
float total_chunks, unique_chunks;
uc digest[SHA_DIGEST_LENGTH];//SHA_DIGEST_LENGTH = 20 (Unit: Byte)

int main(int argc, char *argv[])
{
    chunksize = 1024 * atof(argv[1]);
    numofEntries = atof(argv[2]);
    uc buffer[chunksize];
    HashTable_create(numofEntries);
    FileRead(argv[3], buffer);
    //FileRead(argv[4], buffer);
    //FileRead(argv[5], buffer);
    //FileRead(argv[6], buffer);
    //FileRead(argv[7], buffer);
    HashTable_display(numofEntries);
    printf("DedupeRate = %f%%", getDedupeRate());
    //FileRead(chunksize, argv[3], buffer);
    return 0;
}

void FileRead(char* filename, uc buffer[])
{
    size_t chunknum = 0, index = 0;
    ulli hashcode = 0;
    struct Node SHA_entry = {0, 0, "", 0,NULL};
    FILE *fptr = fopen(filename, "r");
    if (!fptr) 
    {
        printf("File cannot be opened...\n");
        exit(1);
    }
    printf("Chunk size = %zu bytes\n", chunksize);
    while(!feof(fptr))
    {
        fread(buffer, sizeof(char), chunksize, fptr);//read every chunksized bytes of data
        chunknum++;
        SHA1(buffer, chunksize, digest);
        char mdString[SHA_DIGEST_LENGTH*2+1];//
        for(size_t i = 0; i < SHA_DIGEST_LENGTH; i++)
            sprintf(&mdString[i*2], "%02zx", (size_t)digest[i]);   
        //print SHA value 
        //printf("SHA1 value = %s\n", mdString);//sizeof(mdString) = 40 (160bits)
        hashcode = SHAtoHashCode(mdString);
        index = hashcode % numofEntries;//get the index of hash table
        //index = HashCodetoIndex(hashcode);
        //printf("hashtable index = %zu\n", index);
        SHA_entry.key = index;
        SHA_entry.value = hashcode;
        strcpy(SHA_entry.sha,mdString);
        SHA_entry.next = NULL;
        SHA_entry.duplicate_count = 1;
        HashTable_insert(SHA_entry);//insert each chunk to hash table by sha
    }
    printf("Number of chunks = %zu\n", chunknum);
}

ulli SHAtoHashCode(char SHA[])
{
    ulli dec = 0, index = 0;
    char hashcode[17] = {0};//each char is a 4-bit hex number, 16 chars in total (64bits)
    for(size_t i = 0; i < 16; i++)//fetch prior 16 char
        hashcode[i] = SHA[i];
    hashcode[16] = '\0';//the last one is NULL char
    return HextoDec(hashcode);
}
ulli HextoDec(char hex[])
{
    ulli dec = 0, d_value = 0, pow_of_16 = 0;
    size_t len = strlen(hex) - 1;
    ulli a;
    for(size_t i = 0; hex[i] != '\0'; i++, len--)
    {
        if(hex[i] >= '0' && hex[i] <= '9')
            d_value = hex[i] - 48;
        else if(hex[i] >= 'a' && hex[i] <= 'f')
            d_value = hex[i] - 97 + 10;
        else if(hex[i] >= 'A' && hex[i] <= 'F')
            d_value = hex[i] - 65 + 10;
        pow_of_16 = pow(16, len);
        dec += d_value * pow_of_16;
    }
    //printf("hashcode(hex) = %s\nhashcode(dec) = %llu\n", hex, dec);
    return dec;
}
void HashTable_create()
{
    HashTable = malloc(numofEntries * sizeof(struct Node*));
    for(int i = 0; i < numofEntries; i++)//every entry points to NULL
        HashTable[i] = NULL;
    entry_total_chunks = calloc(numofEntries, sizeof(size_t));
    entry_unique_chunks = calloc(numofEntries, sizeof(size_t));
}
void HashTable_insert(struct Node current_Node)
{
    size_t index = current_Node.key;//get the new index of hash table
    struct Node *new_Node = malloc(sizeof(struct Node));//create a new node for chaining
    struct Node *dup_check = HashTable[index];
    new_Node->key = current_Node.key;
    new_Node->value = current_Node.value;
    strcpy(new_Node->sha, current_Node.sha);
    new_Node->duplicate_count = current_Node.duplicate_count;
    new_Node->next = current_Node.next;
    entry_total_chunks[index]++;entry_unique_chunks[index]++;total_chunks++;unique_chunks++;
    if(HashTable[index] == NULL)
        HashTable[index] = new_Node;
    else
    {
        while(dup_check != NULL)
        {
            if(strcmp(dup_check->sha, new_Node->sha) != 0)//check if there is an identical chunk in link list
                dup_check = dup_check->next;
            else
            {
                dup_check->duplicate_count++;
                entry_unique_chunks[index]--;unique_chunks--;
                return;//no need to insert into the list
            }
        }
        struct Node *next = HashTable[index]->next;
        HashTable[index]->next = new_Node;//insert in front of the list
        new_Node->next = next;//maintain hit count for each entry
    }
}
void HashTable_display()
{
    for (size_t i = 0; i < numofEntries; i++)//visit every node in table 
    {    
        size_t count = 0;
        //printf("entry %zu: hit = %zu, unique chunks = %zu\n",i, entry_total_chunks[i], entry_unique_chunks[i]);
        struct Node *current = HashTable[i];
        while (current != NULL)
        {
            //printf("%s %zu\n", current->sha, current->duplicate_count);
            if(current->duplicate_count > max_dup)
                max_dup = current->duplicate_count;
            current = current->next;
            count++;
        }
        if(count > max_list)
            max_list = count;
    }
    
    printf("Total chunks = %f, Unique chunks = %f, max duplicate chunks = %zu, max list size = %zu\n", total_chunks, unique_chunks, max_dup, max_list);
}
float getDedupeRate()
{
    float DedupeRate = (1 - (unique_chunks)/(total_chunks)) * 100;
    return DedupeRate;
}
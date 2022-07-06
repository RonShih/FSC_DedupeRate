## Target
Derive data deduplication rate of a file in C.

## Run this program
1. run `make` or command as below:
```
gcc program.c -o program -L/usr/local/opt/openssl@1.1/lib -I/usr/local/opt/openssl@1.1/include -lcrypto -lz -lm
```
2. Execute this program with variable chunksize, hashtablesize, and file(s).  
Modify argv[ ] in main function to enable multiple files input.
```
./program chunksize(KB) hashtablesize file1 file2 ...
```
Example with only a file.
```
./dedupe 8 1000000 linux-5.3
```

## Steps:
1. Build a hash table with _hashtablesize_ entries
2. Read file(s) in _chunksize_ byte stream
3. Generate fingerprints from each chunk by SHA-1
4. Fetch prefix(32bits) as hashcode of each fingerprint(160bits)
5. Insert hashcode into hash table and store its fingerprint in lined list to avoid false positive
6. When collision happens, traverse the link and compare their fingerprints.
7. Derive dedup rate:
```
1 - (unique_chunks/total_chunks)
```

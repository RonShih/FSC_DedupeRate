# FSC_DedupeRate
Derive data deduplication rate by SHA-1 in hash table way with customized fixed-size chunking and entries of hash table

1. make
2. ./program chunksize(KB) hashtablesize file1 file2 ...

ex. ./dedupe 8 1000000 linux-5.3

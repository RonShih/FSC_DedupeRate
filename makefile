#gcc test.c -o test -L/usr/local/opt/openssl@1.1/lib -I/usr/local/opt/openssl@1.1/include -lcrypto -lz
# 可执行文件
TARGET = dedupe 
# 依赖目标
OBJS = dedupe.o 
# 指令编译器和选项
CC = gcc
CFLAGS = -lcrypto -lz
LIB = -L/usr/local/opt/openssl@1.1/lib
INC = -I/usr/local/opt/openssl@1.1/include

#TARGET: $@
#OBJS: $^
${TARGET}:${OBJS}
	${CC} -o $@ $^ ${LIB} ${INC} ${CFLAGS}

clean:
	rm -rf $(TARGET) $(OBJS)

#dedupe:dedupe.o
#	${CC} dedupe.o ${INC} ${LIB} ${CFLAGS} -o dedupe
#dedupe.o: dedupe.c dedupe.h
#	${CC} dedupe.c ${INC} ${LIB} ${CFLAGS} -lpthread -c
#clean:
#	rm -rf dedupe dedupe.o
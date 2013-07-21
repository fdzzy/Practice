# Makefile for dorm manager project
# author: joey zhang
# 2013/07/20

SRC = dorm.c dorm.h sqlite3.c sqlite3.h

all: $(SRC)
	rm -f dorm
	gcc -g -o dorm dorm.c sqlite3.c -I./ -lpthread -ldl

arm: $(SRC)
	rm -f dorm
	arm-linux-gcc -o dorm dorm.c sqlite3.c -I./ -lpthread -ldl

clean:
	rm -f dorm



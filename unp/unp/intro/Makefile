
OBJS1 = daytime_cli.o err.o
OBJS2 = daytime_svr.o err.o

all: daytime_cli daytime_svr

daytime_cli: $(OBJS1)
	gcc $(OBJS1) -o daytime_cli

daytime_svr: $(OBJS2)
	gcc $(OBJS2) -o daytime_svr

daytime_cli.o: daytime_cli.c
	gcc -g -c daytime_cli.c -o daytime_cli.o -I../include

daytime_svr.o: daytime_svr.c
	gcc -g -c daytime_svr.c -o daytime_svr.o -I../include

err.o: ../lib/err.c
	gcc -g -c ../lib/err.c -o err.o -I../include

clean:
	rm -f daytime_cli
	rm -f *.o

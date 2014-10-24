
OBJS1 = echo_cli.o err.o
OBJS2 = echo_svr.o err.o
OBJS3 = echo_svr_process.o err.o
OBJS4 = echo_svr_thread.o err.o
OBJS5 = des_otp_cli.o err.o
OBJS6 = des_otp_svr.o err.o

all: echo_cli echo_svr des_otp_cli des_otp_svr
process: echo_cli echo_svr_process
thread: echo_cli echo_svr_thread

echo_cli: $(OBJS1)
	gcc $(OBJS1) -o echo_cli

echo_svr: $(OBJS2)
	gcc $(OBJS2) -o echo_svr

des_otp_cli: $(OBJS5)
	gcc $(OBJS5) -o des_otp_cli

des_otp_svr: $(OBJS6)
	gcc $(OBJS6) -o des_otp_svr

echo_svr_process: $(OBJS3)
	gcc $(OBJS3) -o echo_svr_process

echo_svr_thread: $(OBJS4)
	gcc $(OBJS4) -o echo_svr_thread -lpthread

des_otp_cli.o: des_otp_cli.c
	gcc -g -c des_otp_cli.c -o des_otp_cli.o -I../include

des_otp_svr.o: des_otp_svr.c
	gcc -g -c des_otp_svr.c -o des_otp_svr.o -I../include

echo_cli.o: echo_cli.c
	gcc -g -c echo_cli.c -o echo_cli.o -I../include

echo_svr.o: echo_svr.c
	gcc -g -c echo_svr.c -o echo_svr.o -I../include

echo_svr_process.o: echo_svr_process.c
	gcc -g -c echo_svr_process.c -o echo_svr_process.o -I../include

echo_svr_thread.o: echo_svr_thread.c
	gcc -g -c echo_svr_thread.c -o echo_svr_thread.o -I../include

err.o: ../lib/err.c
	gcc -g -c ../lib/err.c -o err.o -I../include

clean:
	rm -f echo_cli
	rm -f *.o

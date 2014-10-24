
all: wait4mult join semaphor abc shoots

wait4mult: wait4mult.c
	gcc wait4mult.c -o wait4mult -Wall -lpthread

join: join.c
	gcc join.c -o join -Wall -lpthread -lm

semaphor: semaphor.c
	gcc semaphor.c -o semaphor -Wall -lpthread

abc: abc.c
	gcc abc.c -o abc -Wall -lpthread

shoots: shoots.c
	gcc shoots.c -o shoots -Wall -lpthread

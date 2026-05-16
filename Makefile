SRCS = *.c

all:
	gcc $(SRCS) -o main

debug:
	gcc -g $(SRCS) -o main

clean:
	rm main

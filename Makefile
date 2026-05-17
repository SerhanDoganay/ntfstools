SRCS = *.c

all:
	gcc $(SRCS) -o ntfstools

debug:
	gcc -g $(SRCS) -o ntfstools

clean:
	rm ntfstools

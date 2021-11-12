.default: all

all: producer-consumer

clean:
	rm -f producer-consumer *.o

producer-consumer: ProducerConsumer.o
	gcc -g -pthread -Wall -Werror -O -o $@ $^
%.o: %.c
	gcc -g -pthread -Wall -Werror -O -c $^
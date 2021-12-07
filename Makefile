all: server input

clean:
	@rm -rf *.o
	@rm -rf rs-server rs-input

input: input.o gpiolib.o
	gcc -o rs-input $^ -lcurl

input.o: input.c gpiolib.h
	gcc -c -o input.o input.c -lcurl

server: main.o httpd.o gpiolib.o
	gcc -o rs-server $^

main.o: main.c httpd.h gpiolib.h
	gcc -c -o main.o main.c

httpd.o: httpd.c httpd.h 
	gcc -c -o httpd.o httpd.c

gpiolib.o: gpiolib.c gpiolib.h 
	gcc -c -o gpiolib.o gpiolib.c

install: rs-input rs-server
	./systemd
	./install

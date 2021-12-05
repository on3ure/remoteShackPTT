all: server

clean:
	@rm -rf *.o
	@rm -rf server watchinput

watchinput: watchinput.o gpiolib.o
	gcc -o watchinput $^

server: main.o httpd.o gpiolib.o
	gcc -o rs-server $^

main.o: main.c httpd.h gpiolib.h
	gcc -c -o main.o main.c

httpd.o: httpd.c httpd.h 
	gcc -c -o httpd.o httpd.c

gpiolib.o: gpiolib.c gpiolib.h 
	gcc -c -o gpiolib.o gpiolib.c


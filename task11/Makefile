all: compile_server compile_client run_server

compile_server: server.c semaphore.c shared.c
	gcc server.c semaphore.c shar.c -o server.out -lpthread

compile_client: client.c semaphore.c shared.c
	gcc client.c semaphore.c shar.c -o client.out -lpthread

run_server: server.out
	./server.out

run_client: client.out
	./client.out

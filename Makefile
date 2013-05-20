CFLAGS=-Os --std=c99
all: synergy-uinput

synergy/protocol.o: synergy/protocol.h synergy/protocol1.4.c
	gcc ${CFLAGS} -o synergy/protocol.o -c synergy/protocol1.4.c

synergy/client.o: synergy/client.h synergy/client.c synergy/event.h log/log.h
	gcc ${CFLAGS} -o synergy/client.o -c synergy/client.c

uinput/uinput.o: uinput/uinput.h uinput/uinput.c
	gcc ${CFLAGS} -o uinput/uinput.o -c uinput/uinput.c

log/log.o: log/log.h log/log.c
	gcc ${CFLAGS} -o log/log.o -c log/log.c

synergy-uinput: synergy/protocol.o synergy/client.o uinput/uinput.o log/log.o main.c
	gcc ${CFLAGS} -o synergy-uinput log/log.o synergy/protocol.o uinput/uinput.o synergy/client.o main.c

clean:
	rm -f *.o */*.o synergy-uinput




build:
	gcc -Wall -L/usr/X11R6/lib  -lX11 exhaleconf.c exhalelist.c exhale.c -o exhale

debug:
	gcc -Wall -g -L/usr/X11R6/lib -lX11 exhaleconf.c exhalelist.c exhale.c -o exhale

clean:
	rm exhale
	rm *.o


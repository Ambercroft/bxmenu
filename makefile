


build:
	gcc -Wall -L/usr/X11R6/lib  -lX11 bxmenuconf.c bxmenulist.c bxmenu.c -o bxmenu 

debug:
	gcc -Wall -g -L/usr/X11R6/lib -lX11 bxmenuconf.c bxmenulist.c bxmenu.c -o bxmenu

clean:
	rm bxmenu
	rm *.o


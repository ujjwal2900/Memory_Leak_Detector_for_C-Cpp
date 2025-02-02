TARGET:exe

exe:mld.o app.o
	gcc -g -Wall -o exe -L . mld.o app.o
app.o: app.c
	gcc -g -Wall -c app.c -o app.o
mld.o: mld.c
	gcc -g -Wall -c mld.c -o mld.o
clean:
	rm mld.o
	rm app.o
	rm exe
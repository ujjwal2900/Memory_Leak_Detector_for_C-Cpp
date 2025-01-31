gcc -g -Wall -c mld.c -o mld.o
gcc -g -Wall -c app.c -o app.o
gcc -g -Wall -o exe mld.o app.o

all : T2R.o execute include.o

T2R.o : T2R.c
		gcc -Wall -c T2R.c

include.o : include.c
		gcc -Wall -c include.c

execute : T2R.o TicketToRideAPI.o clientAPI.o include.o
		gcc -o execute T2R.o TicketToRideAPI.o clientAPI.o include.o

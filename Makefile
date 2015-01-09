all: crypto keygen
keygen: keygen.c
	gcc -o keygen keygen.c -lgmp
rsalib.o: rsalib.c rsalib.h
	gcc -c -o rsalib.o rsalib.c
crypto: crypto.c rsalib.o
	gcc -o crypto crypto.c rsalib.o -lgmp
clean:
	rm -rf keygen
	rm -rf crypto
	rm -rf rsalib.o
	rm -rf rsa.pub rsa.pri

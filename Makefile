keygen: keygen.c
	gcc -o keygen keygen.c -lgmp
clean:
	rm -rf keygen
	rm -rf rsa.pub rsa.pri

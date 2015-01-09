#include "rsalib.h"

int main(int argc, char * * argv)
{
	FILE *f = fopen(argv[1], "rb"), *fin = fopen(argv[3], "rb"), *fout = fopen(argv[4], "wb");
	struct kbag *k = init_kbag();
	read_key(f, k);
	fclose(f);

	if (strcmp(argv[2], "-d") == 0)
		decrypt_file(fin, fout, k->exponent, k->modulo);
	else if (strcmp(argv[2], "-e") == 0)
		encrypt_file(fin, fout, k->exponent, k->modulo);
	else
		printf("Incorrect usage: %s <keyfile> -de <input> <output>\n", argv[0]);

	fclose(fin);
	fclose(fout);
	free_kbag(k);
}

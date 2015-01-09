#include "rsalib.h"

int main(int argc, char * * argv)
{
	FILE *f = fopen(argv[1], "rb");
	struct kbag *k = init_kbag();
	read_key(f, k);
	mpz_out_str(stdout, 10, k->exponent);
	putchar(10);
	mpz_out_str(stdout, 10, k->modulo);
	fclose(f);
	free_kbag(k);
}

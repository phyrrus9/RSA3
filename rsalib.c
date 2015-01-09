#include "rsalib.h"
#include <stdlib.h>

struct kbag *init_kbag()
{
	struct kbag *k = malloc(sizeof(struct kbag));
	mpz_init(k->exponent);
	mpz_init(k->modulo);
}

void free_kbag(struct kbag *k)
{
	mpz_clears(k->exponent, k->modulo, NULL);
	free(k);
}

void read_key(FILE *f, struct kbag *k)
{
	unsigned long l1, l2;
	void *buf;
	int i;
	fread(&l1, sizeof(unsigned long), 1, f);
	fread(&l2, sizeof(unsigned long), 1, f);
	l1 = ntohl(l1);
	l2 = ntohl(l2);
	buf = malloc(l1);
	fread(buf, l1, 1, f);
	for (i = 0; i < l1; i++) printf("%02x ", *(char *)(buf +i));
	printf("\n");
	mpz_import(k->modulo, l1, 1, 1, 1, 0, buf);
	free(buf);
	for (i = 0; i < l2; i++) printf("%02x ", *(char *)(buf + i));
	printf("\n");
	buf = malloc(l2);
	mpz_import(k->exponent, l2, 1, 1, 1, 0, buf);
}

#include "rsalib.h"
#include <stdlib.h>
#include <string.h>

struct kbag *init_kbag()
{
	struct kbag *k = malloc(sizeof(struct kbag));
	mpz_init(k->exponent);
	mpz_init(k->modulo);
	return k;
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
	mpz_import(k->modulo, 1, 1, l1, 1, 0, buf);

	free(buf);
	buf = malloc(l2);
	fread(buf, l2, 1, f);
	mpz_import(k->exponent, 1, 1, l2, 1, 0, buf);
}

void encrypt(mpz_t ret, mpz_t in, mpz_t e, mpz_t n)
{ mpz_powm(ret, in, e, n); }
void decrypt(mpz_t ret, mpz_t in, mpz_t d, mpz_t n)
{ encrypt(ret, in, d, n); }

unsigned long mpz_sizeinbytes(mpz_t r)
{
	size_t words_alloc;
	void *tmp = mpz_export(NULL, &words_alloc, 1, 1, 1, 0, r);
	free(tmp);
	return words_alloc;
}

void mpz_genbinary(mpz_t r, void *buf, size_t bufsize)
{
	size_t words_alloc;
	memset(buf, 0, bufsize);
	mpz_export(buf, &words_alloc, 1, 1, 1, 0, r);
}

int dry_run_encrypt(FILE *fin, mpz_t e, mpz_t n)
{
	unsigned char inp;
	unsigned long min_size = 0, tmp;
	mpz_t out, in;
	mpz_inits(out, in, NULL);
	while (fread(&inp, 1, 1, fin) > 0)
	{
		mpz_set_ui(in, inp);
		encrypt(out, in, e, n);
		tmp = mpz_sizeinbytes(out);
		if (tmp > min_size) min_size = tmp;
	}
	mpz_clears(out, in, NULL);
	fseek(fin, 0L, SEEK_SET);
	return min_size;
}

void encrypt_file(FILE *fin, FILE *fout, mpz_t e, mpz_t n)
{
	unsigned char inp;
	unsigned long size;
	void *buf;
	mpz_t out, in;
	mpz_inits(out, in, NULL);
	size = dry_run_encrypt(fin, e, n);
	size = htonl(size);
	fwrite(&size, sizeof(unsigned long), 1, fout);
	size = ntohl(size);
	buf = malloc(size + 1);
	while (fread(&inp, 1, 1, fin) > 0)
	{
		mpz_set_ui(in, inp);
		encrypt(out, in, e, n);
		mpz_genbinary(out, buf, size);
		fwrite(buf, size, 1, fout);
	}
	free(buf);
	fflush(fout);
	mpz_clears(out, in, NULL);
}

void decrypt_file(FILE *fin, FILE *fout, mpz_t d, mpz_t n)
{
	unsigned char oup;
	unsigned long size;
	void *buf;
	mpz_t out, in;
	mpz_inits(out, in, NULL);
	fread(&size, sizeof(unsigned long), 1, fin);
	size = ntohl(size);
	buf = malloc(size + 1);
	while (fread(buf, size, 1, fin) > 0)
	{
		mpz_import(in, 1, 1, size, 1, 0, buf);
		decrypt(out, in, d, n);
		oup = mpz_get_ui(out);
		fwrite(&oup, 1, 1, fout);
	}
	free(buf);
	fflush(fout);
	mpz_clears(out, in, NULL);
}

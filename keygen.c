#include <gmp.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define BITWIDTH 2048
#define BITWIDTH_E (BITWIDTH/6)
#define PROBABRUN ((unsigned long)((double)BITWIDTH/1.2))

#define DEBUG_ENABLE 0

gmp_randstate_t rstate;

void print_status(mpz_t p, const char *s)
{
#if DEBUG_ENABLE == 1
	printf("[STATUS] %s=0x", s);
	mpz_out_str(stdout, 16, p);
	printf("\n");
	fflush(stdout);
#endif
}

void totient(mpz_t ret, mpz_t p, mpz_t q)
{
	mpz_t pt, qt;
	mpz_inits(pt, qt, NULL);
	mpz_sub_ui(qt, q, 1);
	mpz_sub_ui(pt, p, 1);
	mpz_mul(ret, pt, qt);
	mpz_clears(pt, qt, NULL);
}

char isPrime(mpz_t p)
{
	mpz_t sqrtq, i, tmp;
	char ret = 1;
	int probab = mpz_probab_prime_p(p, PROBABRUN);

	if (probab == 2) return 1;
	else if (probab == 0) return 0;

	mpz_inits(sqrtq, i, tmp, NULL);
	mpz_sqrt(sqrtq, p);
	for (mpz_set_ui(i, 2); mpz_cmp(i, sqrtq) < 0 && ret == 1;mpz_add_ui(i, i, 1))
	{
		mpz_mod(tmp, p, i);
		if (mpz_cmp_ui(tmp, 1) == 0)
			goto isPrimeDone;
	}
isPrimeDone:
	mpz_clears(sqrtq, i, tmp, NULL);
	return ret;
}

void seed_randstate(gmp_randstate_t *rstate)
{
	gmp_randinit_default(*rstate);
	gmp_randseed_ui(*rstate, time(NULL));
}

void gen_pq(mpz_t r)
{
	do { mpz_urandomb(r, rstate, BITWIDTH); } while (!isPrime(r));
}

void gen_n(mpz_t n, mpz_t p, mpz_t q)
{
	mpz_mul(n, p, q); //n = p*q
}

void gen_x(mpz_t x, mpz_t p, mpz_t q)
{
	totient(x, p, q);
}

void gen_e(mpz_t e, mpz_t x)
{
	mpz_t i;
	mpz_init(i);

	mpz_urandomb(e, rstate, BITWIDTH_E);
	mpz_nextprime(e, e);
	for (; mpz_cmp(e, x); mpz_nextprime(e, e))
	{
		if (isPrime(e)) //this should be true anyways
		{
			mpz_mod(i, x, e);
			if (mpz_cmp_ui(i, 0) != 0)
				break;
			else if (mpz_cmp(e, x) > 0)
				mpz_urandomb(e, rstate, BITWIDTH_E);
		}
	}

	mpz_clear(i);
}

void gen_d(mpz_t d, mpz_t e, mpz_t x)
{
	mpz_invert(d, e, x);
}

void *gen_binary(mpz_t r, unsigned long *size)
{
	size_t words_alloc;
	void *ret = mpz_export(NULL, &words_alloc, 1, 1, 1, 0, r);
	*size = words_alloc;
	return ret;
}

void write_keyfile(char *fname, mpz_t r1, mpz_t r2)
{
	void *r1v, *r2v;
	unsigned long r1s, r2s;
	FILE *f;
	r1v = gen_binary(r1, &r1s);
	r2v = gen_binary(r2, &r2s);
//	printf("r1s=%lu\tr2s=%lu\n", r1s, r2s);
	r1s = htonl(r1s);
	r2s = htonl(r2s);
	f = fopen(fname, "wb");
	fwrite(&r1s, sizeof(unsigned long), 1, f);
	fwrite(&r2s, sizeof(unsigned long), 1, f);
	fwrite(r1v, ntohl(r1s), 1, f);
	fwrite(r2v, ntohl(r2s), 1, f);
	fflush(f);
	fclose(f);
	free(r1v);
	free(r2v);
}

int main()
{
	mpz_t p, q, x, n, d, e;
	mpz_inits(p, q, x, n, d, e, NULL);
	seed_randstate(&rstate);

	gen_pq(p);
	gen_pq(q);
	print_status(p, "P");
	print_status(q, "Q");

	gen_n(n, p, q);
	print_status(n, "N");

	gen_x(x, p, q);

	print_status(x, "t(n)");

	gen_e(e, x);
	print_status(e, "E");

	gen_d(d, e, x);
	print_status(d, "D");

	write_keyfile("rsa.pub", n, e);
	write_keyfile("rsa.pri", n, d);

	mpz_clears(p, q, x, n, d, e, NULL);
	gmp_randclear(rstate);
}

#include <gmp.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define BITWIDTH 128
#define BITWIDTH_E (BITWIDTH/4)

void print_status(mpz_t p, const char *s)
{
	printf("[STATUS] %s=0x", s);
	mpz_out_str(stdout, 16, p);
	printf("\n");
	fflush(stdout);
}

void modInverse(mpz_t ret, mpz_t a, mpz_t m)
{
	mpz_t atmp, tmp, tmpm;
	mpz_inits(atmp, tmp, tmpm, NULL);
	mpz_mod(atmp, a, m);
	for (mpz_set_ui(ret, 2); mpz_cmp(ret, m) < 0; mpz_add_ui(ret, ret, 1))
	{
		mpz_mul(tmp, atmp, ret);
		mpz_mod(tmp, tmp, m);
		if (mpz_cmp_ui(tmp, 1) == 0)
			goto modInverseDone;
	}
modInverseDone:
	mpz_clears(atmp, tmp, tmpm, NULL);
}

void modInverse2(mpz_t x1, mpz_t a, mpz_t b)
{
	mpz_t b0, t, q, x0, a0, b1;
	mpz_inits(b0, t, q, x0, a0, b1, NULL);
	mpz_set(b0, b);
	mpz_set(b1, b);
	mpz_set(a0, a);
	mpz_set_ui(x1, 1);
	if (mpz_cmp_ui(b, 1) > 0)
		goto modInverse2Done;
	while (mpz_cmp_ui(a0, 1) > 0)
	{
		mpz_div(q, a, b);
		mpz_set(t, b);
		mpz_mod(b1, a0, b1)
		mpz_set(a0, t);
		mpz_set(t, x0);
		mpz_sub(x1, x1, q);
		mpz_mul(x0, x1, x0);
		mpz_set(x1, t);
	}
	if (mpz_cmp_ui(x1, 0) < 0)
		mpz_add(x1, x1, b0);
modInverse2Done:
	mpz_clears(b0, t, q, x0, a0, b1, NULL);
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
	mpz_inits(sqrtq, i, tmp, NULL);
	char ret = 1;
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

void gen_pq(mpz_t r)
{
	gmp_randstate_t rstate;
	unsigned long seed;
	gmp_randinit_default(rstate);
	seed = time(NULL);
	gmp_randseed_ui(rstate, seed);

	do { mpz_urandomb(r, rstate, BITWIDTH); } while (!isPrime(r));

	gmp_randclear(rstate);
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
	gmp_randstate_t rstate;
	mpz_t i;
	unsigned long seed;
	mpz_init(i);

	gmp_randinit_default(rstate);
	seed = time(NULL);
	gmp_randseed_ui(rstate, seed);

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
	modInverse(d, e, x);
}

void *gen_binary(mpz_t r, unsigned long *size)
{
	size_t words_alloc;
	void *ret = mpz_export(NULL, &words_alloc, 1, 1, 1, 0, r);
	*size = words_alloc * 4;
	return ret;
}

void write_keyfile(char *fname, mpz_t r1, mpz_t r2)
{
	void *r1v, *r2v;
	unsigned long r1s, r2s;
	FILE *f;
	r1v = gen_binary(r1, &r1s);
	r2v = gen_binary(r2, &r2s);
	printf("r1s=%lu\tr2s=%lu\n", r1s, r2s);
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
}

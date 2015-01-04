#include <gmp.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define BITWIDTH 16
#define BITWIDTH_E (BITWIDTH/2)

void print_status(mpz_t p, const char *s)
{
	printf("[STATUS] %s=", s);
	mpz_out_str(stdout, 16, p);
	printf("\n");
	fflush(stdout);
}

void modInverse(mpz_t ret, mpz_t a, mpz_t m)
{
	mpz_t atmp, tmp;
	mpz_inits(atmp, tmp, NULL);
	mpz_mod(atmp, a, m);
	for (mpz_set_ui(ret, 2); mpz_cmp(ret, m) < 0; mpz_add_ui(ret, ret, 1))
	{
		mpz_mul(tmp, atmp, ret);
		mpz_mod(tmp, tmp, m);
		if (mpz_cmp_ui(tmp, 1) == 0)
			goto modInverseDone;
	}
modInverseDone:
	mpz_clears(atmp, tmp, NULL);
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

int main()
{
	mpz_t p, q, x, n, d, e, i;
	gmp_randstate_t rstate;
	unsigned long seed;

	mpz_inits(p, q, x, n, d, i, e, seed, NULL);
	//random init shit
	gmp_randinit_default(rstate);
	seed = time(NULL);
	gmp_randseed_ui(rstate, seed);

	do { mpz_urandomb(p, rstate, BITWIDTH); } while (!isPrime(p));
	do { mpz_urandomb(q, rstate, BITWIDTH);	} while (!isPrime(q));

	print_status(p, "P");
	print_status(q, "Q");

	mpz_mul(n, p, q); //n = p*q

	print_status(n, "N");

	totient(x, p, q);

	print_status(x, "t(n)");

	mpz_urandomb(e, rstate, BITWIDTH_E);
	mpz_nextprime(e, e);
	for (; mpz_cmp(e, x); mpz_nextprime(e, e))
	{
		if (isPrime(e)) //this should be true anyways
		{
			mpz_gcd(i, e, x);
			if (mpz_cmp_ui(i, 1) == 0)
				break;
			else if (mpz_cmp(e, x) > 0)
				mpz_urandomb(e, rstate, BITWIDTH_E);
		}
	}

	print_status(e, "E");

	modInverse(d, e, x);

	print_status(d, "D");
}

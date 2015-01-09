#ifndef RSA4LIB_H
#define RSA4LIB_H

#include <stdio.h>
#include <gmp.h>

struct kbag
{
	mpz_t exponent;
	mpz_t modulo;
};

struct kbag * init_kbag();
void free_kbag(struct kbag *k);
void read_key(FILE *f, struct kbag *k);
void encrypt(mpz_t ret, mpz_t in, mpz_t e, mpz_t n);
void decrypt(mpz_t ret, mpz_t in, mpz_t d, mpz_t n);
void encrypt_file(FILE *f, mpz_t e, mpz_t n);
void decrypt_file(FILE *f, mpz_t d, mpz_t n);

#endif

//151, 7351, 22499

#ifndef CRYPTO_H
#define CRYPTO_H

#include "bstream.h"

int SaveCrypto(const char *fileName, Chronos::BStream *stream);
int LoadCrypto(const char *fileName, Chronos::BStream *stream);
int CryptoString(char *string);
int UnCryptoString(char *string);

extern char *codePad;


#endif

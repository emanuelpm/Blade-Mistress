/*
* An implementation of PBKDF2 (Password-Based Key Derivation Function) using Crypto API
* (RFC2898 / PKCS#5)
*
* Copyright (c) 2010 Mounir IDRASSI <mounir.idrassi@idrix.fr>. All rights reserved.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.
*
*/

#pragma once

#ifndef _WIN32_WINNT         
#define _WIN32_WINNT 0x0501
#endif

#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>
#include <math.h>

// Pseudo Random Function (PRF) prototype

/* generic context used in HMAC calculation */
typedef struct
{
    DWORD	magic;			/* used to help check that we are using the correct context */
    void*	pParam;	      /* hold a custom pointer known to the implementation  */
} PRF_CTX;

typedef BOOL(WINAPI* PRF_HmacInitPtr)(
    PRF_CTX*       pContext,   /* PRF context used in HMAC computation */
    const unsigned char* pbKey,      /* pointer to authentication key */
    DWORD          cbKey       /* length of authentication key */
    );

typedef BOOL(WINAPI* PRF_HmacPtr)(
    PRF_CTX*       pContext,   /* PRF context initialized by HmacInit */
    unsigned char*  pbData,    /* pointer to data stream */
    DWORD          cbData,     /* length of data stream */
    unsigned char* pbDigest    /* caller digest to be filled in */
    );

typedef BOOL(WINAPI* PRF_HmacFreePtr)(
    PRF_CTX*       pContext	/* PRF context initialized by HmacInit */
    );


/* PRF type definition */
typedef struct
{
    PRF_HmacInitPtr   hmacInit;
    PRF_HmacPtr       hmac;
    PRF_HmacFreePtr	hmacFree;
    DWORD             cbHmacLength;
} PRF;


/* Implementation of HMAC-SHA1 using CAPI */

#define HMAC_SHA1_MAGIC 0x53484131

typedef struct
{
    HCRYPTPROV hProv;
    HCRYPTKEY hKey;
} CAPI_CTX_PARAM;

// Structure used by CAPI for HMAC computation
typedef struct {
    BLOBHEADER hdr;
    DWORD cbKeySize;
} HMAC_KEY_BLOB;

BOOL WINAPI hmacInit_sha1(
    PRF_CTX*       pContext,   /* PRF context used in HMAC computation */
    const unsigned char* pbKey,      /* pointer to authentication key */
    DWORD          cbKey       /* length of authentication key */
    );

BOOL WINAPI hmac_sha1(
    PRF_CTX*       pContext,               /* PRF context used in HMAC computation */
    unsigned char*  pbData,                /* pointer to data stream */
    DWORD           cbData,                /* length of data stream */
    unsigned char   pbDigest[20]           /* caller digest to be filled in */
    );


BOOL WINAPI hmacFree_sha1(
    PRF_CTX*       pContext          /* PRF context used in HMAC computation */
    );


/*
* PBKDF2 implementation
*/
BOOL PBKDF2(PRF pPrf,
    const unsigned char* pbPassword,
    DWORD cbPassword,
    const unsigned char* pbSalt,
    DWORD cbSalt,
    DWORD dwIterationCount,
    unsigned char* pbDerivedKey,
    DWORD          cbDerivedKey);


int TestPBKDF2();

/*
* Definition of the HMAC-SHA1 PRF
*/
extern PRF sha1Prf;
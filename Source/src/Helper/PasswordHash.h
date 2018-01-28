// Password hashing based on https://crackstation.net/hashing-security.htm

#include <windows.h>

#define SALT_BYTE_SIZE      32
#define HASH_BYTE_SIZE      32
#define OUT_HASH_SIZE       (sizeof(DWORD) + 1 + (SALT_BYTE_SIZE * 2) + 1 + (HASH_BYTE_SIZE * 2) + 1)

namespace PasswordHash
{
    bool CreateSerializableHash(const unsigned char *pszPass, unsigned char outPass[OUT_HASH_SIZE]);
    bool ValidateSerializablePassword(const unsigned char *pszPass, const unsigned char correctHash[OUT_HASH_SIZE]);

    bool CreateStandaloneHash(const unsigned char *pszPass, const unsigned char *pszSalt, unsigned int iters, unsigned char outHash[HASH_BYTE_SIZE]);
    bool ValidateStandalonePassword(const unsigned char *pszPass, const unsigned char *pszSalt, unsigned int iters, unsigned char correcthash[HASH_BYTE_SIZE]);

    void UnitTest();
};
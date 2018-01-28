// Password hashing based on https://crackstation.net/hashing-security.htm

#include "PasswordHash.h"

#include "stdio.h"
#include "string.h"
#include <malloc.h>
#include <assert.h>

#include "PBKDF2.h"

///////////////////////////////////////////////////////////////////////////////

namespace
{
    const DWORD PBKDF2_ITERATIONS   = 1024;


    // This is an interesting solution to a hacking attempt I wasn't aware of.  To prevent "timing attacks" where
    // an attacker can time a guess and figure out that the longer the server takes to return the more
    // characters in the front of the string match you use a compare algorithm that takes the same amount
    // of time to run regardless of how similar.
    bool SlowEquals(const unsigned char *a, unsigned int aLen, const unsigned char *b, unsigned int bLen)
    {
        // Using == generates a branch which can change code timing, so use XOR which has no branches
        unsigned int diff = aLen ^ bLen;
        for (unsigned int i = 0; i < aLen && i < bLen; ++i)
        {
            diff |= a[i] ^ b[i];
        }

        return diff == 0;
    }

    bool GenerateSalt(unsigned char salt[SALT_BYTE_SIZE])
    {
        HCRYPTPROV hProvider = 0;
        if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
        {
            return false;
        }

        if (!CryptGenRandom(hProvider, SALT_BYTE_SIZE, (BYTE*) &salt[0]))
        {
            CryptReleaseContext(hProvider, 0);
            return false;
        }

        if (!CryptReleaseContext(hProvider, 0))
        {
            return false;
        }

        return true;
    }

    // Does not prepend 0x
    bool ToHex(const unsigned char* pszSrc, unsigned int srcLen, unsigned char* pszOut, unsigned int outLen)
    {
        // Need at least 2x source size in output
        if (srcLen * 2 > outLen)
            return false;

        const unsigned char topNibble = 0xF0;
        const unsigned char botNibble = 0x0F;

        for (unsigned int i = 0; i < srcLen; ++i)
        {
            const unsigned char c = pszSrc[i];

            const unsigned char topC = (c & topNibble) >> 4;
            const unsigned char botC = c & botNibble;

            pszOut[i * 2 + 0] = topC < 10 ? '0' + topC : 'A' + (topC - 10);
            pszOut[i * 2 + 1] = botC < 10 ? '0' + botC : 'A' + (botC - 10);
        }

        return true;
    }

    // Expects the string doesn't start with 0x
    bool FromHex(const unsigned char* pszSrc, unsigned int srcLen, unsigned char* pszOut, unsigned int outLen)
    {
        // Need at least half the size of the source in output
        if (srcLen / 2 > outLen)
            return false;

        const unsigned char topNibble = 0xF0;
        const unsigned char botNibble = 0x0F;

        for (unsigned int i = 0; i < srcLen; i += 2)
        {
            unsigned char topC = pszSrc[i];
            if (!((topC >= '0' && topC <= '9') || (topC >= 'A' && topC <= 'F')))
            {
                return false;
            }

            topC = topC > '9' ? topC - 'A' + 0xA : topC - '0';

            unsigned char botC = pszSrc[i + 1];
            if (!((botC >= '0' && botC <= '9') || (botC >= 'A' && botC <= 'F')))
            {
                return false;
            }

            botC = botC > '9' ? botC - 'A' + 0xA : botC - '0';

            pszOut[i / 2] = topC << 4 | botC;
        }

        return true;
    }
}

///////////////////////////////////////////////////////////////////////////////

namespace PasswordHash
{
    bool CreateSerializableHash(const unsigned char* pszPass, unsigned char outPass[OUT_HASH_SIZE])
    {
        unsigned char salt[SALT_BYTE_SIZE];
        if (!GenerateSalt(salt))
        {
            return false;
        }

        unsigned char hash[HASH_BYTE_SIZE];
        if (!PBKDF2(sha1Prf, pszPass, strlen((char*)pszPass), salt, SALT_BYTE_SIZE, PBKDF2_ITERATIONS, hash, HASH_BYTE_SIZE))
        {
            return false;
        }

        unsigned char saltStr[SALT_BYTE_SIZE * 2 + 1];
        ToHex(salt, SALT_BYTE_SIZE, saltStr, SALT_BYTE_SIZE * 2 + 1);
        saltStr[SALT_BYTE_SIZE * 2] = 0;

        unsigned char hashStr[HASH_BYTE_SIZE * 2 + 1];
        ToHex(hash, HASH_BYTE_SIZE, hashStr, HASH_BYTE_SIZE * 2 + 1);
        hashStr[HASH_BYTE_SIZE * 2] = 0;

        sprintf_s((char*)&outPass[0], OUT_HASH_SIZE, "%u:%s:%s", PBKDF2_ITERATIONS, saltStr, hashStr);

        return true;
    }

    bool ValidateSerializablePassword(const unsigned char* pszPass, const unsigned char correctHash[OUT_HASH_SIZE])
    {
        unsigned int iters = 0;
        unsigned char savedSalt[SALT_BYTE_SIZE * 2 + 1];
        unsigned char savedHash[HASH_BYTE_SIZE * 2 + 1];
        int filled = sscanf((const char*)&correctHash[0], "%u:%[0-9A-F]:%[0-9A-F]", &iters, &savedSalt[0], &savedHash[0]);
        if (filled != 3)
            return false;

        
        unsigned char saltIn[SALT_BYTE_SIZE];
        if (!FromHex(savedSalt, SALT_BYTE_SIZE * 2, saltIn, SALT_BYTE_SIZE))
        {
            return false;
        }

        unsigned char hashIn[HASH_BYTE_SIZE];
        if (!FromHex(savedHash, HASH_BYTE_SIZE * 2, hashIn, HASH_BYTE_SIZE))
        {
            return false;
        }
        
        unsigned char hashOut[HASH_BYTE_SIZE];
        if (!PBKDF2(sha1Prf, pszPass, strlen((char*)pszPass), saltIn, SALT_BYTE_SIZE, iters, hashOut, HASH_BYTE_SIZE))
        {
            return false;
        }

        return SlowEquals(hashOut, HASH_BYTE_SIZE, hashIn, HASH_BYTE_SIZE);
    }

    bool CreateStandaloneHash(const unsigned char *pszPass, const unsigned char *pszSalt, unsigned int iters, unsigned char outHash[HASH_BYTE_SIZE])
    {
        if (!PBKDF2(sha1Prf, pszPass, strlen((char*)pszPass), pszSalt, strlen((const char*)pszSalt), iters, outHash, HASH_BYTE_SIZE))
        {
            return false;
        }

        return true;
    }

    bool ValidateStandalonePassword(const unsigned char *pszPass, const unsigned char *pszSalt, unsigned int iters, unsigned char correcthash[HASH_BYTE_SIZE])
    {
        unsigned char hash[HASH_BYTE_SIZE];
        if (!PBKDF2(sha1Prf, pszPass, strlen((char*)pszPass), pszSalt, strlen((const char*)pszSalt), iters, hash, HASH_BYTE_SIZE))
        {
            return false;
        }

        return memcmp(hash, correcthash, HASH_BYTE_SIZE) == 0;
    }

    void UnitTest()
    {
        // Serializable functions
        {
            unsigned char password[] = "TestPassword123";
            unsigned char passwordTestWrong[] = "TestPassword124";

            unsigned char hash0[OUT_HASH_SIZE];
            unsigned char hash1[OUT_HASH_SIZE];

            PasswordHash::CreateSerializableHash(password, hash0);
            PasswordHash::CreateSerializableHash(password, hash1);

            // Shouldn't match even with same password
            if (memcmp(hash0, hash1, OUT_HASH_SIZE))
            {
                printf("Two same password hashes in a row don't match, as they shouldn't!\n");
            }
            else
            {
                printf("Two same password hashes in a row matched, which is bad :(!\n");
            }

            if (PasswordHash::ValidateSerializablePassword(passwordTestWrong, hash0))
            {
                printf("Bad password matched :(!\n");
            }
            else
            {
                printf("Bad password didn't match!\n");
            }

            if (PasswordHash::ValidateSerializablePassword(password, hash0))
            {
                printf("Good password matched!\n");
            }
            else
            {
                printf("Good password didn't match :(!\n");
            }
        }

        // Standalone functions
        {
            unsigned char password1[] = "TestPassword123";
            unsigned char password2[] = "TestPassword124";

            unsigned char salt1[] = "DerSalt1";
            unsigned char salt2[] = "DerSaltySalt123";

            unsigned char hash0[HASH_BYTE_SIZE];
            unsigned char hash1[HASH_BYTE_SIZE];

            PasswordHash::CreateStandaloneHash(password1, salt1, 1000, hash0);
            PasswordHash::CreateStandaloneHash(password1, salt1, 1000, hash1);

            if (memcmp(hash0, hash1, HASH_BYTE_SIZE) == 0)
            {
                printf("CreateStandaloneHash is deterministic!\n");
            }
            else
            {
                printf("CreateStandaloneHash is non-deterministic :(!\n");
            }

            if (PasswordHash::ValidateStandalonePassword(password1, salt1, 1000, hash0))
            {
                printf("ValidateStandalonePassword works with correct arguments!\n");
            }
            else
            {
                printf("ValidateStandalonePassword doesn't work with correct arguments :(!\n");
            }

            if (!PasswordHash::ValidateStandalonePassword(password1, salt2, 1000, hash0))
            {
                printf("ValidateStandalonePassword works with wrong salt!\n");
            }
            else
            {
                printf("ValidateStandalonePassword doesn't work with wrong salt :(!\n");
            }

            if (!PasswordHash::ValidateStandalonePassword(password1, salt1, 1001, hash0))
            {
                printf("ValidateStandalonePassword works with wrong iters!\n");
            }
            else
            {
                printf("ValidateStandalonePassword doesn't work with wrong iters :(!\n");
            }

            if (!PasswordHash::ValidateStandalonePassword(password2, salt1, 1000, hash0))
            {
                printf("ValidateStandalonePassword works with wrong password!\n");
            }
            else
            {
                printf("ValidateStandalonePassword doesn't work with wrong password :(!\n");
            }

            PasswordHash::CreateStandaloneHash(password1, salt2, 1000, hash1);
            if (memcmp(hash0, hash1, HASH_BYTE_SIZE) != 0)
            {
                printf("CreateStandaloneHash with different salts return different hashes!\n");
            }
            else
            {
                printf("CreateStandaloneHash with different salts return same hashes :(!\n");
            }

            PasswordHash::CreateStandaloneHash(password2, salt1, 1000, hash1);
            if (memcmp(hash0, hash1, HASH_BYTE_SIZE) != 0)
            {
                printf("CreateStandaloneHash with different pass return different hashes!\n");
            }
            else
            {
                printf("CreateStandaloneHash with different pass return same hashes :(!\n");
            }

            PasswordHash::CreateStandaloneHash(password1, salt1, 1001, hash1);
            if (memcmp(hash0, hash1, HASH_BYTE_SIZE) != 0)
            {
                printf("CreateStandaloneHash with different iters return different hashes!\n");
            }
            else
            {
                printf("CreateStandaloneHash with different iters return same hashes :(!\n");
            }
        }
    }
}

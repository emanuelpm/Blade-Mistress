#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "crypto.h"
//#include "thomdebug.h"

char *codePad = NULL;

//***************************************************************
int SaveCrypto(const char *fileName, Chronos::BStream *stream)
{
    assert(stream);
    assert(fileName);

    // **** open file
    FILE *fp = fopen(fileName,"wb");
    if (!fp)
        return 1;  // FAILURE

    // **** create encryption buffer with one extra bit for chesksum
    long bufferSize = stream->used() + 1;
    char *buffer = new char[bufferSize];

    assert(buffer);

    // **** fill buffer with data to encrypt
    stream->setPosition(0);
    stream->read(buffer, stream->used());

    // **** write checksum
    unsigned char checksum = 0;
    for (int i = 0; i < stream->used(); ++i)
        checksum += buffer[i];

    buffer[stream->used()] = checksum;

    // **** encrypt entire buffer

    // byte-swap words
    for (int i = 0; i < stream->used()-1; i += 2)
    {
        char temp = buffer[i];
        buffer[i] = buffer[i+1];
        buffer[i+1] = temp;
    }

    // swap every second and seventh bytes
    for (int i = 0; i < stream->used()-7; i += 7)
    {
        char temp = buffer[i+1];
        buffer[i+1] = buffer[i+6];
        buffer[i+6] = temp;
    }

    // invert every third byte
    for (int i = 0; i < stream->used()-2; i += 3)
    {
        buffer[i] = ~(buffer[i]);
    }

    if (codePad)
    {
        // if codePad, use it to offset the bytes
        int padSize = strlen(codePad);
        assert (padSize > 0);

        int padIndex = 0;
        for (int i = 0; i < stream->used(); i += 1)
        {
            buffer[i] += codePad[padIndex];
            padIndex++;
            if (padIndex >= padSize)
                padIndex = 0;
        }

    }

    // byte-swap words again
    for (int i = 0; i < stream->used()-1; i += 2)
    {
        char temp = buffer[i];
        buffer[i] = buffer[i+1];
        buffer[i+1] = temp;
    }

    // **** write out crypto file tag
	long tag = 1000;
    fwrite(&tag,1,4,fp);

    // **** write out size
    fwrite(&bufferSize,1,4,fp);

    // **** write out buffer
    fwrite(buffer,1,bufferSize,fp);

    // **** finish up
    delete[] buffer;

    fclose(fp);

    return 0; // success
}

//***************************************************************
int LoadCrypto(const char *fileName, Chronos::BStream *stream)
{
    assert(stream);
    assert(fileName);

    // **** open file
    FILE *fp = fopen(fileName,"rb");
    if (!fp)
        return 1;  // FAILURE

    // **** read tag
    long tag;
    fread(&tag,1,4,fp);
	if (1000 != tag)
	{
	    fclose(fp);
        return 1;  // failure!
	}

    // **** read buffer length
    long bufferSize;
    fread(&bufferSize,1,4,fp);

    // **** create decryption buffer
    char *buffer = new char[bufferSize];

    assert(buffer);

    // **** load buffer from file
    fread(buffer,1,bufferSize,fp);

    int sUsed = bufferSize-1;

    // byte-swap words back
    for (int i = 0; i < sUsed-1; i += 2)
    {
        char temp = buffer[i];
        buffer[i] = buffer[i+1];
        buffer[i+1] = temp;
    }

    if (codePad)
    {
        // if codePad, use it to de-offset the bytes
        int padSize = strlen(codePad);
        assert (padSize > 0);

        int padIndex = 0;
        for (int i = 0; i < sUsed; i += 1)
        {
            buffer[i] -= codePad[padIndex];
            padIndex++;
            if (padIndex >= padSize)
                padIndex = 0;
        }

    }

    // invert every third byte
    for (int i = 0; i < sUsed-2; i += 3)
    {
        buffer[i] = ~(buffer[i]);
    }

    // swap every second and seventh bytes
    for (int i = 0; i < sUsed-7; i += 7)
    {
        char temp = buffer[i+1];
        buffer[i+1] = buffer[i+6];
        buffer[i+6] = temp;
    }

    // byte-swap words
    for (int i = 0; i < sUsed-1; i += 2)
    {
        char temp = buffer[i];
        buffer[i] = buffer[i+1];
        buffer[i+1] = temp;
    }

    // **** check checksum
    unsigned char checksum = 0;
    for (int i = 0; i < sUsed; ++i)
        checksum += buffer[i];

    if (buffer[sUsed] != (char) checksum)
	{
	    fclose(fp);
        return 1;  // failure!
	}
    // **** finish up

    stream->write(buffer,sUsed);

    delete[] buffer;

    fclose(fp);

    stream->setPosition(0);

    return 0; // success
}


//***************************************************************
int CryptoString(char *string)
{
   assert(string);

	char buff[256], *buffer;
	int len = strlen(string);
	assert(len > 0 && len < 256);

	buff[0] = len;
	buffer = &buff[1];

	sprintf(buffer, string);
   // **** encrypt entire buffer

   // byte-swap words
   for (int i = 0; i < len-1; i += 2)
   {
       char temp = buffer[i];
       buffer[i] = buffer[i+1];
       buffer[i+1] = temp;
   }

   // swap every second and seventh bytes
   for (int i = 0; i < len-7; i += 7)
   {
       char temp = buffer[i+1];
       buffer[i+1] = buffer[i+6];
       buffer[i+6] = temp;
   }

   // invert every byte
   for (int i = 0; i < len; i += 1)
   {
       buffer[i] = ~(buffer[i]);
   }

   if (codePad)
   {
       // if codePad, use it to offset the bytes
       int padSize = strlen(codePad);
       assert (padSize > 0);

       int padIndex = 0;
       for (int i = 0; i < len; i += 1)
       {
           buffer[i] += codePad[padIndex];
           padIndex++;
           if (padIndex >= padSize)
               padIndex = 0;
       }

   }

   // byte-swap words again
   for (int i = 0; i < len-1; i += 2)
   {
       char temp = buffer[i];
       buffer[i] = buffer[i+1];
       buffer[i+1] = temp;
   }

	// put buffer back into string
	memcpy(string, buff, len+1);

   return len; // success
}

//***************************************************************
int UnCryptoString(char *string)
{
   assert(string);

	int len = string[0];

	if(len < 0)
		return 0;

	char buff[256], *buffer;
	buffer = &buff[0];

	memcpy(buffer,&(string[1]),len);

   // byte-swap words back
   for (int i = 0; i < len-1; i += 2)
   {
       char temp = buffer[i];
       buffer[i] = buffer[i+1];
       buffer[i+1] = temp;
   }

   if (codePad)
   {
       // if codePad, use it to de-offset the bytes
       int padSize = strlen(codePad);
       assert (padSize > 0);

       int padIndex = 0;
       for (int i = 0; i < len; i += 1)
       {
           buffer[i] -= codePad[padIndex];
           padIndex++;
           if (padIndex >= padSize)
               padIndex = 0;
       }

   }

   // invert every byte
   for (int i = 0; i < len; i += 1)
   {
       buffer[i] = ~(buffer[i]);
   }

   // swap every second and seventh bytes
   for (int i = 0; i < len-7; i += 7)
   {
       char temp = buffer[i+1];
       buffer[i+1] = buffer[i+6];
       buffer[i+6] = temp;
   }

   // byte-swap words
   for (int i = 0; i < len-1; i += 2)
   {
       char temp = buffer[i];
       buffer[i] = buffer[i+1];
       buffer[i+1] = temp;
   }

	buffer[len] = 0;
	sprintf(string, buff);

   return len; // success
}


/* end of file */


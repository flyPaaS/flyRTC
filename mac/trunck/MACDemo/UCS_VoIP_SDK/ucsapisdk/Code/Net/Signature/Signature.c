#include "Signature.h"

//#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#include "rc4.h"
#include "SHA1.h"

#define KEY					"1a81160d06119b64f3ee64003f3cab4d8d9e5132244940137d2f013c1107008f"
#define KEY_SIZE			64
#define KEYT				"1122334455667788"
#define KEYT_SIZE			16

#define RC4_MAIN_KEY		"abcxyz"
#define RC4_MAIN_KEY_SIZE	6
#define RC4_KEY1			"qzpm!@#2013wxong"
#define RC4_KEY1_SIZE		16
#define RC4_KEY2			"ecin!@#2013rvubh"
#define RC4_KEY2_SIZE		16

#define SN_SIZE				16

char alphaMap[10] = {'d','e','y','b','i','p','v','k','z','o'};

// Get the real unencrypted key
void GetRealKey(unsigned char* pKey)
{
	// Get unencrypted key
	RC4_KEY rc4Key;

	// Get encrypted key
	/*unsigned char* pKeyEncrypted = (unsigned char*)malloc(KEY_SIZE + 1);
	memset(pKeyEncrypted, 0, KEY_SIZE + 1);

	RC4_set_key(&rc4Key, KEYT_SIZE, KEYT);
	RC4(&rc4Key, KEY_SIZE, KEY, pKeyEncrypted);*/

	unsigned char* pKeyBinary = (unsigned char*)malloc(KEY_SIZE / 2 + 1);
	memset(pKeyBinary, 0, KEY_SIZE / 2 + 1);

	SHA1Hex2Binary((const unsigned char *)KEY, KEY_SIZE, pKeyBinary, KEY_SIZE / 2 + 1);

	RC4_set_key(&rc4Key, KEYT_SIZE, (const unsigned char *)KEYT);
	RC4(&rc4Key, KEY_SIZE / 2, pKeyBinary, pKey);

	free(pKeyBinary);
	pKeyBinary = NULL;
}

//************************************
// Method:    Get the signature of the input data
// FullName:  Signature
// Returns:   void
// Qualifier: public
// Parameter: const unsigned char*
//				[IN] The data pointer to be signed
//			  const unsigned long
//				[IN] The data length
//			  unsigned char*
//				[OUT] The signature output (PS: The length of pSignature must be greater than 40 and set to 0)
//************************************
void Signature(const unsigned char* pData, const unsigned long nDataLen, unsigned char* pSignature)
{
	SHA1_CTX context;
	unsigned char digest[20] = {0};

	unsigned char* pKey = (unsigned char*)malloc(KEY_SIZE + 1);
	unsigned char* pBuffer = (unsigned char*)malloc(nDataLen + KEY_SIZE + 1);

	if (pData == NULL || pSignature == NULL || nDataLen == 0)
	{
		free(pKey);
		free(pBuffer);

		pKey = NULL;
		pBuffer = NULL;

		return;
	}

	memset(pKey, 0, KEY_SIZE + 1);

	GetRealKey(pKey);

	memset(pBuffer, 0, nDataLen + KEY_SIZE / 2 + 1);
	memcpy(pBuffer, pData, nDataLen);
	memcpy(pBuffer + nDataLen, pKey, KEY_SIZE / 2);

	SHA1Init(&context); 
	SHA1Update(&context, pBuffer, nDataLen + KEY_SIZE / 2);
	SHA1Final(digest, &context);

	SHA1Binary2Hex(digest, 20, pSignature, 40);

	free(pKey);
	free(pBuffer);

	pKey = NULL;
	pBuffer = NULL;
}

//************************************
// Method:    Get the security string
// FullName:  Security
// Returns:   void
// Qualifier: public
// Parameter: unsigned char*
//				[OUT] The security flag output (PS: The length of pSecurity must be greater than 8 and set to 0)
//************************************
void Security(unsigned char* pSecurity)
{
	unsigned long nX = 0;
	unsigned long nY = 0;
	unsigned long nZ = 0;

	unsigned long nNumberLen = 0;
	unsigned long nNumberIndex = 0;

	unsigned char pTemp[8] = {0};

	if (pSecurity == NULL)
	{
		return;
	}

	//srand(time(NULL));
	nX = 100 + rand() % 100;
	nY = 1 + rand() % 1000;
	nZ = nY * 256 + nX;

	sprintf(pTemp, "%d", nZ);

	memcpy(pSecurity, pTemp, 8);

	nNumberLen = strlen(pTemp);

	while (nNumberIndex < nNumberLen)
	{
		pSecurity[nNumberIndex] = alphaMap[(unsigned long)(pSecurity[nNumberIndex] - '0')];
		++nNumberIndex;
	}
}

//************************************
// Method:    Encrypt data using enhanced RC4 algorithm
// FullName:  RC4_s
// Returns:   void
// Qualifier: public
// Parameter: const unsigned char*
//				[IN] The data pointer to be encrypted
//			  const unsigned long
//				[IN] The data length
//			  unsigned char*
//				[OUT] The encrypted data output (PS: The length of pEncrypted should be greater than (nDataLen + RC4_MAIN_KEY_SIZE + SN_SIZE) * 2)
//************************************
void RC4_s(const unsigned char* pData, const unsigned long nDataLen, unsigned char* pEncrypted)
{
	//RC4( RC4(data, keyt1) + Key + random number, keyt2)
	RC4_KEY rc4Key;
	unsigned long nSN = 0;

	unsigned char pTemp[SN_SIZE] = {0};
	unsigned char* pDataEncrypted1 = (unsigned char*)malloc(nDataLen);
	unsigned char* pDataEncrypted2 = (unsigned char*)malloc(nDataLen + RC4_MAIN_KEY_SIZE + SN_SIZE);
	unsigned char* pDataEncryptedFinal = (unsigned char*)malloc(nDataLen + RC4_MAIN_KEY_SIZE + SN_SIZE);

	memset(pDataEncrypted1, 0, nDataLen);
	memset(pDataEncrypted2, 0, nDataLen + RC4_MAIN_KEY_SIZE + SN_SIZE);
	memset(pDataEncryptedFinal, 0, nDataLen + RC4_MAIN_KEY_SIZE + SN_SIZE);

	if (pData == NULL || pEncrypted == NULL || nDataLen == 0)
	{
		free(pDataEncrypted1);
		free(pDataEncrypted2);
		free(pDataEncryptedFinal);
		
		pDataEncrypted1 = NULL;
		pDataEncrypted2 = NULL;
		pDataEncryptedFinal = NULL;

		return;
	}

	//Get SN
	//srand(time(NULL));
	nSN = rand() % 0x7FFFFFFF;

	sprintf(pTemp, "%d", nSN);

	//Get Data Encrypted 1 data encrypted = RC4(data, key1)
	RC4_set_key(&rc4Key, RC4_KEY1_SIZE, RC4_KEY1);
	RC4(&rc4Key, nDataLen, pData, pDataEncrypted1);

	//Get Data Encrypted 2 final encrypted = RC4(data encrypted + main key + SN, key2)
	memcpy(pDataEncrypted2, pDataEncrypted1, nDataLen);
	memcpy(pDataEncrypted2 + nDataLen, RC4_MAIN_KEY, RC4_MAIN_KEY_SIZE);
	memcpy(pDataEncrypted2 + nDataLen + RC4_MAIN_KEY_SIZE, pTemp, strlen(pTemp));

	RC4_set_key(&rc4Key, RC4_KEY2_SIZE, RC4_KEY2);
	RC4(&rc4Key, nDataLen + RC4_MAIN_KEY_SIZE + strlen(pTemp), pDataEncrypted2, pDataEncryptedFinal);

	SHA1Binary2Hex(pDataEncryptedFinal, nDataLen + RC4_MAIN_KEY_SIZE + strlen(pTemp), pEncrypted, (nDataLen + RC4_MAIN_KEY_SIZE + strlen(pTemp)) * 2);

	free(pDataEncrypted1);
	free(pDataEncrypted2);
	free(pDataEncryptedFinal);

	pDataEncrypted1 = NULL;
	pDataEncrypted2 = NULL;
	pDataEncryptedFinal = NULL;
}
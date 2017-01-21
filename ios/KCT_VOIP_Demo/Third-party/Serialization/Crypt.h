/*
 *  Crypt.h
 *  NoteDemo
 *
 *  Created by andyguan on 10-9-26.
 *
 */
#pragma once

#include "typexp_xyssl.h"
#include "rbbuffer.h"

enum
{
	CRYPT_OK = 0,
	CRYPT_ERR_INVALID_KEY_N = 1,
	CRYPT_ERR_INVALID_KEY_E = 2,
	CRYPT_ERR_ENCRYPT_WITH_RSA_PUBKEY = 3,
	CRYPT_ERR_DECRYPT_WITH_RSA_PRIVKEY = 4,
	CRYPT_ERR_NO_MEMORY = 5,
	CRYPT_ERR_ENCRYPT_WITH_DES_KEY = 6,
	CRYPT_ERR_DECRYPT_WITH_DES_KEY = 7,
	CRYPT_ERR_INVALID_PARAM = 8,
	CRYPT_ERR_LOAD_RSA_PRIVATE_KEY = 9,
};

#ifdef linux

int RSADecrypt( const unsigned char *pInput,
				unsigned int uiInputLen, 
				RBBuffer& rbOutput,
				const char *pPrivKeyFile
	      	  );

#else  // other platform

int RSADecrypt( const unsigned char *pInput,
				unsigned int uiInputLen, 
				RBBuffer& rbOutput,
				const char *pPrivKeyFile
	      	  );

#endif

int RSAEncrypt(	const unsigned char *pInput, 
				unsigned int uiInputLen, 
				RBBuffer& rbOutput,
				const char *pPublicKeyN,
				const char *pPublicKeyE
			  );

int DESEncrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
			   const char* pKey, unsigned int uiKeyLen);

int DESDecrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
			   const char* pKey, unsigned int uiKeyLen);

//旧des加解密
namespace olddes {

int DESEncrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen);

int DESDecrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen);

}

//aes-cbc 128 pkcs7 加解密
int AESEncrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen);

int AESDecrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen);

//aes-ecb 128 pkcs7 加解密
int AESECBEncrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen, bool final);

int AESECBDecrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen, bool final);

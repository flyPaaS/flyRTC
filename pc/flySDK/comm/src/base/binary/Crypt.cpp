/*
 *  Crypt.cpp
 *  NoteDemo
 *
 *  Created by Lingol on 10-9-26.
 *
 */

#include "Crypt.h"
#include "rsa.h"
#include "CommonDefine.h"
#include "rbdes.h"
#include "oldrbdes.h"
#include <openssl/aes.h>

#include <stdio.h>
#include <string.h>
#ifdef linux

#include "iLog.h"
#include <openssl/evp.h>
#include <openssl/pem.h>

#include <stdio.h>
#include <string.h>

using namespace Comm;

EVP_PKEY* LoadPrivKey(const char *KeyFile)
{
	BIO *BioKey = NULL;
	EVP_PKEY *Key = NULL;
#if defined(TLV_ENC_ENABLED)
	BioKey = BIO_new(BIO_s_file());
	if (BioKey == NULL)
	{
		goto Error;
	}
	if (BIO_read_filename(BioKey, KeyFile) <= 0)
	{
		goto Error;
	}
	Key = PEM_read_bio_PrivateKey(BioKey, NULL, NULL, NULL);
Error:
	if (BioKey)
	{
		BIO_free(BioKey);
		BioKey = NULL;
	}
#endif
	return Key;
}


int RSADecrypt( const unsigned char* pInput,
				unsigned int uiInputLen,
				RBBuffer& rbOutput,
				const char *pPrivKeyFile
			  )			
{
#if defined(TLV_ENC_ENABLED)
	if(!pInput || !pPrivKeyFile || uiInputLen < 8 || uiInputLen % 8 != 0)
	{
		return CRYPT_ERR_INVALID_PARAM;
	}
	
	int ret = 0;
	
	// load priv key
	RSA *Rsa = NULL;

	EVP_PKEY *Key = LoadPrivKey(pPrivKeyFile);
	if (!Key)
	{
		LogErr( "Failed loading RSA private key file \"%s\"", pPrivKeyFile);
		return CRYPT_ERR_LOAD_RSA_PRIVATE_KEY;
	}

	Rsa = EVP_PKEY_get1_RSA(Key);
	EVP_PKEY_free(Key);
	if (!Rsa)
	{
		LogErr( "Failed, EVP_PKEY_get1_RSA");
		return CRYPT_ERR_LOAD_RSA_PRIVATE_KEY;
	}

	int iKeySize = RSA_size(Rsa);

	// decrypt
	unsigned char *pcPlainBuf = (unsigned char *)OPENSSL_malloc( uiInputLen );
	int iPlainSize = 0;

	if (uiInputLen > (unsigned int)iKeySize)
	{
		// 分段解密
		int iBlockCnt = uiInputLen / iKeySize;

		int iPos = 0;

		for (int i = 0; i < iBlockCnt; ++i)
		{
			int iBlockSize = 0;			
			ret = RSA_private_decrypt( iKeySize, 
									   pInput + i * iKeySize, 
									   pcPlainBuf + iPos, 
									   Rsa, RSA_PKCS1_PADDING ); 
			if (ret < 1)
			{
				LogErr( "RSA: MultiBlock RSA_private_decrypt fail on block %d of %d, blocksize %d\n", 
						i, iBlockCnt, iBlockSize
				    	);

				LogErr( "RSA_private_decrypt return %d\n", ret);
				OPENSSL_free(pcPlainBuf);

				return CRYPT_ERR_DECRYPT_WITH_RSA_PRIVKEY;
				
			}			
			iPos += ret;
		}

		iPlainSize = iPos;
	}
	else
	{
		// 单段解密
		ret = RSA_private_decrypt(
				iKeySize, 
				pInput, 
				pcPlainBuf, 
				Rsa, 
				RSA_PKCS1_PADDING);

		if (ret < 1)
		{
			LogErr( "RSA: SingleBlock RSA_private_decrypt fail\n");
			OPENSSL_free(pcPlainBuf);
			return CRYPT_ERR_DECRYPT_WITH_RSA_PRIVKEY;
		}

		iPlainSize = ret;
	}

	rbOutput.Reset();
	rbOutput.Write(pcPlainBuf, iPlainSize);
	OPENSSL_free(pcPlainBuf);
#endif
	return CRYPT_OK;
}

#else // other platform

int RSADecrypt( const unsigned char *pInput,
		unsigned int uiInputLen, 
		RBBuffer& rbOutput,
		const char *pPrivKeyFile)
{
	return CRYPT_OK;
}

#endif


int RSAEncrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput, 
			   const char* pPublicKeyN, const char *pPublicKeyE)
{
	if(!pInput || !pPublicKeyN || !pPublicKeyE)
	{
		return CRYPT_ERR_INVALID_PARAM;
	}
	rsa_context rsaContext;
	rsa_init(&rsaContext, RSA_PKCS_V15, 0, NULL, NULL);
	
	ON_NEQU_DO_RETURN(mpi_read_string(&rsaContext.N, 16, (char*)pPublicKeyN), 0, rsa_free(&rsaContext), CRYPT_ERR_INVALID_KEY_N);
	ON_NEQU_DO_RETURN(mpi_read_string(&rsaContext.E, 16, (char*)pPublicKeyE), 0, rsa_free(&rsaContext), CRYPT_ERR_INVALID_KEY_E);
	
	rsaContext.len = (mpi_msb(&rsaContext.N) + 7) >> 3;
	
	// Encrypt
	long lCryptedSize = 0;
	
	if ((int)uiInputLen >= rsaContext.len - 12)
	{
		// 鍒嗘鍔犲瘑
		int iBlockCnt = (uiInputLen / (rsaContext.len-12)) + (((uiInputLen % (rsaContext.len-12)) == 0)?0:1);
		lCryptedSize = iBlockCnt * rsaContext.len;		
		ON_NEQU_DO_RETURN(rbOutput.EnsureWriteSize(lCryptedSize), 0, rsa_free(&rsaContext), CRYPT_ERR_NO_MEMORY);
		
		for (int i = 0; i < iBlockCnt; ++i)
		{
			int iBlockSize = rsaContext.len - 12;
			if (i == iBlockCnt - 1)
			{
				iBlockSize = uiInputLen - i * (rsaContext.len - 12);
			}
			
			ON_NEQU_DO_RETURN(
							  rsa_pkcs1_encrypt(&rsaContext, 
												RSA_PUBLIC, 
												iBlockSize, 
												(unsigned char*)pInput + i * (rsaContext.len - 12), 
												rbOutput.GetWritePtr() + i * rsaContext.len),
							  0, rsa_free(&rsaContext), 
							  CRYPT_ERR_ENCRYPT_WITH_RSA_PUBKEY);
		}
		rbOutput.AddWritePos(lCryptedSize);
	}
	else
	{
		lCryptedSize = rsaContext.len;		
		ON_NEQU_DO_RETURN(rbOutput.EnsureWriteSize(lCryptedSize), 0, rsa_free(&rsaContext), CRYPT_ERR_NO_MEMORY);
		// 鍗曟鍔犲瘑
		ON_NEQU_DO_RETURN(
						  rsa_pkcs1_encrypt(&rsaContext, 
											RSA_PUBLIC, 
											uiInputLen, 
											(unsigned char*)pInput, 
											rbOutput.GetWritePtr()),
						  0, rsa_free(&rsaContext), 
						  CRYPT_ERR_ENCRYPT_WITH_RSA_PUBKEY);
		rbOutput.AddWritePos(lCryptedSize);
	}	
	rsa_free(&rsaContext);
	return CRYPT_OK;
}

namespace olddes {

int DESEncrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
			   const char* pKey, unsigned int uiKeyLen)
{
	if(!pInput || !pKey)
	{
		return CRYPT_ERR_INVALID_PARAM;
	}
	// Encrypt
	int nPaddingLen = 8 - (uiInputLen % 8);
	
	RBBuffer rbPaddedInput;
	rbPaddedInput.EnsureWriteSize(uiInputLen + nPaddingLen);
	rbPaddedInput.Write(pInput, uiInputLen);
	for (int i = 0; i < nPaddingLen; ++i)
	{
		*(rbPaddedInput.GetWritePtr() + i) = (unsigned char)nPaddingLen;
	}
	rbPaddedInput.AddWritePos(nPaddingLen);
	
	rbOutput.EnsureWriteSize(uiInputLen + nPaddingLen + 32);
	
	ON_EQU_RETURN(
				  olddes::Using_DES((char *)rbOutput.GetArray(),
							(const char *)rbPaddedInput.GetArray(), 
							uiInputLen + nPaddingLen, 
							uiKeyLen, 
							(const char *)pKey, 
							ENCRYPTION),
				  0,
				  CRYPT_ERR_ENCRYPT_WITH_DES_KEY);
	rbOutput.AddWritePos(uiInputLen + nPaddingLen + 8);
	return CRYPT_OK;
}

int DESDecrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
			   const char* pKey, unsigned int uiKeyLen)
{
	if(!pInput || !pKey || uiInputLen < 8 || uiInputLen % 8 != 0)
	{
		return CRYPT_ERR_INVALID_PARAM;
	}
	
	ON_EQU_RETURN(
				olddes::Using_DES((char *)rbOutput.GetWritePtr(uiInputLen),
						 (const char *)pInput, 
						 uiInputLen, 
						 uiKeyLen, 
						 (const char *)pKey, 
						 DECRYPTION),
				  0,
				  CRYPT_ERR_DECRYPT_WITH_DES_KEY);
	
	int nPaddingLen = (int)(*(rbOutput.GetWritePtr() + uiInputLen - 1 - 8));
	
	if (nPaddingLen <= 0 || nPaddingLen > 8)
	{
		return CRYPT_ERR_INVALID_PARAM;
	}
	
	rbOutput.AddWritePos(uiInputLen - nPaddingLen - 8);
	
	memset(rbOutput.GetWritePtr(), 0, rbOutput.GetWriteCapacity() - rbOutput.GetWritePos());
	
	return CRYPT_OK;
}

} //namespace olddes

int DESEncrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen)
{

    if(!pInput || !pKey)
    {
        return CRYPT_ERR_INVALID_PARAM;
    }
    // Encrypt
    int nPaddingLen = 8 - (uiInputLen % 8);

    RBBuffer rbPaddedInput;
    rbPaddedInput.EnsureWriteSize(uiInputLen + nPaddingLen);
    rbPaddedInput.Write(pInput, uiInputLen);
    for (int i = 0; i < nPaddingLen; ++i)
    {
        *(rbPaddedInput.GetWritePtr() + i) = (unsigned char)nPaddingLen;
    }
    rbPaddedInput.AddWritePos(nPaddingLen);

    rbOutput.EnsureWriteSize(uiInputLen + nPaddingLen + 32);

    ON_EQU_RETURN(
                  Using_DES((char *)rbOutput.GetArray(),
                            (const char *)rbPaddedInput.GetArray(),
                            uiInputLen + nPaddingLen,
                            uiKeyLen,
                            (const char *)pKey,
                            ENCRYPTION),
                  0,
                  CRYPT_ERR_ENCRYPT_WITH_DES_KEY);
    rbOutput.AddWritePos(uiInputLen + nPaddingLen + 8);
    return CRYPT_OK;
}

/*

int DESDecrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen)
{
    printf("aaaaaaaaaaa desdecrypt line:%d \n",__LINE__);	

    if(!pInput || !pKey || uiInputLen < 8 || uiInputLen % 8 != 0)
    {
	return CRYPT_ERR_INVALID_PARAM;
    }

    ON_EQU_RETURN(
                Using_DES((char *)rbOutput.GetWritePtr(uiInputLen),
                         (const char *)pInput,
                         uiInputLen,
                         uiKeyLen,
                         (const char *)pKey,
                         DECRYPTION),
                  0,
                  CRYPT_ERR_DECRYPT_WITH_DES_KEY);

    int nPaddingLen = (int)(*(rbOutput.GetWritePtr() + uiInputLen - 1 - 8));

    if (nPaddingLen <= 0 || nPaddingLen > 8)
    {
        return CRYPT_ERR_INVALID_PARAM;
    }

    rbOutput.AddWritePos(uiInputLen - nPaddingLen - 8);

    memset(rbOutput.GetWritePtr(), 0, rbOutput.GetWriteCapacity() - rbOutput.GetWritePos());

    return CRYPT_OK;
}
*/


int DESDecrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen)
{
    if(!pInput || !pKey || uiInputLen < 8 || uiInputLen % 8 != 0)
    {
        return CRYPT_ERR_INVALID_PARAM;
    }

    ON_EQU_RETURN(
                Using_DES((char *)rbOutput.GetWritePtr(uiInputLen),
                         (const char *)pInput,
                         uiInputLen,
                         uiKeyLen,
                         (const char *)pKey,
                         DECRYPTION),
                  0,
                  CRYPT_ERR_DECRYPT_WITH_DES_KEY);

    int nPaddingLen = (int)(*(rbOutput.GetWritePtr() + uiInputLen - 1 - 8));
    if (nPaddingLen <= 0 || nPaddingLen > 8)
    {
        return CRYPT_ERR_INVALID_PARAM;
    }
    rbOutput.AddWritePos(uiInputLen - nPaddingLen - 8);

    memset(rbOutput.GetWritePtr(), 0, rbOutput.GetWriteCapacity() - rbOutput.GetWritePos());
    return CRYPT_OK;
}


int AESECBEncrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen, bool final)
{
#if defined(TLV_ENC_ENABLED)
    if (!pInput || !pKey || uiInputLen == 0 || (!final && uiInputLen % 16 != 0))
    {
        return CRYPT_ERR_INVALID_PARAM;
    }
	
    //padding pkcs7
    RBBuffer rbPaddedInput;
    unsigned int nPaddingLen = final ? (16 - uiInputLen % 16) : 0;
    rbPaddedInput.EnsureWriteSize(uiInputLen + nPaddingLen);
    rbPaddedInput.Write(pInput, uiInputLen);

    memset(rbPaddedInput.GetWritePtr(), nPaddingLen, nPaddingLen);
    rbPaddedInput.AddWritePos(nPaddingLen);

    rbOutput.EnsureWriteSize(uiInputLen + nPaddingLen);

    //加密
    unsigned char key[16];
    memset(key, 0, sizeof(key));
    memcpy(key, pKey, uiKeyLen > 16U ? 16U : uiKeyLen);

    AES_KEY aesKey;
    AES_set_encrypt_key(key, 128, &aesKey);
	
	unsigned int iDone = 0;
	unsigned char* pcInput = rbPaddedInput.GetArray();
	unsigned char* pcOutPut = rbOutput.GetWritePtr();

	while (iDone < uiInputLen + nPaddingLen)
	{
		AES_ecb_encrypt( pcInput, pcOutPut, &aesKey, AES_ENCRYPT );
		pcInput += 16;
		pcOutPut += 16;
		iDone += 16;
	}

	rbOutput.AddWritePos(uiInputLen + nPaddingLen);
#endif
    return CRYPT_OK;
}

int AESECBDecrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen, bool final)
{
#if defined(TLV_ENC_ENABLED)
    if (!pInput || !pKey || uiInputLen % 16 != 0)
    {
        return CRYPT_ERR_INVALID_PARAM;
    }
	
    //解密
    unsigned char key[16];
    memset(key, 0, sizeof(key));
    memcpy(key, pKey, uiKeyLen > 16U ? 16U : uiKeyLen);

    AES_KEY decKey;
    AES_set_decrypt_key(key, 128, &decKey);
	
	unsigned int iDone = 0;
	unsigned char* pcOutPut = rbOutput.GetWritePtr(uiInputLen);

	while (iDone < uiInputLen)
	{
		AES_ecb_encrypt( pInput, pcOutPut, &decKey, AES_DECRYPT );
		pInput += 16;
		pcOutPut += 16;
		iDone += 16;
	}

	if (!final)
	{
		rbOutput.AddWritePos(uiInputLen);
	}
	else
	{
	    int nPaddingLen = *(rbOutput.GetWritePtr() + uiInputLen - 1);
	    if (nPaddingLen <= 0 || nPaddingLen > 16)
	    {
	        return CRYPT_ERR_INVALID_PARAM;
	    }

	    rbOutput.AddWritePos(uiInputLen - nPaddingLen);
	    memset(rbOutput.GetWritePtr(), 0, rbOutput.GetWriteCapacity() - rbOutput.GetWritePos());
	}
#endif
    return CRYPT_OK;
}

int AESEncrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen) {
#if defined(TLV_ENC_ENABLED)
    if (!pInput || !pKey)
    {
        return CRYPT_ERR_INVALID_PARAM;
    }

    //padding pkcs7
    RBBuffer rbPaddedInput;
    int nPaddingLen = 16 - (uiInputLen % 16);
    rbPaddedInput.EnsureWriteSize(uiInputLen + nPaddingLen);
    rbPaddedInput.Write(pInput, uiInputLen);

    memset(rbPaddedInput.GetWritePtr(), nPaddingLen, nPaddingLen);
    rbPaddedInput.AddWritePos(nPaddingLen);

    rbOutput.EnsureWriteSize(uiInputLen + nPaddingLen);

    //加密
    unsigned char key[16], iv[16];
    memset(key, 0, sizeof(key));
    memcpy(key, pKey, uiKeyLen > 16U ? 16U : uiKeyLen);
    memcpy(iv, key, sizeof(key));

    AES_KEY aesKey;
    AES_set_encrypt_key(key, 128, &aesKey);
    AES_cbc_encrypt((unsigned char*)rbPaddedInput.GetArray(),
            (unsigned char*)rbOutput.GetArray(),
            uiInputLen + nPaddingLen, &aesKey, iv, AES_ENCRYPT);

    rbOutput.AddWritePos(uiInputLen + nPaddingLen);
#endif    
    return CRYPT_OK;
}

int AESDecrypt(const unsigned char* pInput, unsigned int uiInputLen, RBBuffer& rbOutput,
               const char* pKey, unsigned int uiKeyLen) {
#if defined(TLV_ENC_ENABLED)
    if (!pInput || !pKey || uiInputLen < 16 || uiInputLen % 16 != 0)
    {
        return CRYPT_ERR_INVALID_PARAM;
    }

    //解密
    unsigned char key[16], iv[16];
    memset(key, 0, sizeof(key));
    memcpy(key, pKey, uiKeyLen > 16U ? 16U : uiKeyLen);
    memcpy(iv, key, sizeof(key));

    AES_KEY decKey;
    AES_set_decrypt_key(key, 128, &decKey);
    AES_cbc_encrypt((unsigned char*)pInput,
            (unsigned char*)rbOutput.GetWritePtr(uiInputLen),
            uiInputLen, &decKey, iv, AES_DECRYPT);

    int nPaddingLen = *(rbOutput.GetWritePtr() + uiInputLen - 1);
    if (nPaddingLen <= 0 || nPaddingLen > 16)
    {
        return CRYPT_ERR_INVALID_PARAM;
    }

    rbOutput.AddWritePos(uiInputLen - nPaddingLen);
    memset(rbOutput.GetWritePtr(), 0, rbOutput.GetWriteCapacity() - rbOutput.GetWritePos());
#endif
    return CRYPT_OK;
}




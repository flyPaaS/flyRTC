#ifndef UX_SIGNATURE_COMPONENT
#define UX_SIGNATURE_COMPONENT

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
//				[OUT] The signature output (PS: The length of pSignature must be greater than 20 and set to 0)
//************************************
void Signature(const unsigned char* pData, const unsigned long nDataLen, unsigned char* pSignature);

//************************************
// Method:    Get the security string
// FullName:  Security
// Returns:   void
// Qualifier: public
// Parameter: unsigned char*
//				[OUT] The security flag output (PS: The length of pSecurity must be greater than 8 and set to 0)
//************************************
void Security(unsigned char* pSecurity);

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
//				[OUT] The encrypted data output (PS: The length of pEncrypted should be greater than nDataLen + )
//************************************
void RC4_s(const unsigned char* pData, const unsigned long nDataLen, unsigned char* pEncrypted);

#endif
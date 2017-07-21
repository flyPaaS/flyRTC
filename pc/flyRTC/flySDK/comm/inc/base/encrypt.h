#ifndef _RC4_ENCRYPTPACKET_H_
#define _RC4_ENCRYPTPACKET_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum encrypt_type
{
	ENCRYPT_TGO = 0,
	ENCRYPT_UGO = 1
}encrypt_type_e; 

/************************************************************
Function	: encrypt_packet
Description : encrypt packet data
Input		: inbuf  => (input data buf)
			  inlen  => (input data length)
			  outbuf => (output data buf pointer, eg: buf[1024])
			  outlen => (output data buf max length pointer, eg:1024)
			  type   => (encrypt_type_e)
Output		: outbuf => (output data buf pointer)
			  outlen => (output data buf length pointer) 
Return	    : Successful return 0 , Failure returns -1
Remark      : here outbuf do not need to malloc buf 
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
int encrypt_packet(char *inbuf, int inlen, char *outbuf, int *outlen, encrypt_type_e type);

/************************************************************
Function	: decrypt_packet
Description : decrypt packet data
Input		: buf => (Original data buf)
			  len => (Original data buf length)
			  type   => (encrypt_type_e)
Output		: buf => (decrypt data buf)
			  len => (decrypt data buf length) 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
int decrypt_packet(char *buf, int *len, char *decrypted, encrypt_type_e type);

#ifdef __cplusplus
}
#endif

#endif
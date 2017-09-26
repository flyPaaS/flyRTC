#include <stdio.h>
#include <string.h>
#include "encrypt.h"
#include "rc4.h"
#ifdef WIN32
#include <Winsock2.h>
#endif

#ifdef __APPLE__
#include <netinet/in.h>
#endif

#ifndef _WIN32
#include <arpa/inet.h>
#endif

unsigned char key_data_ptr[]= "k1oET&Yh7@EQnp2XdTP1o/Vo=";

int encrypt_packet(char *inbuf, int inlen, char *outbuf, int *outlen, encrypt_type_e type)
{
	unsigned short contentlen = inlen;
	int offset = 0;
	RC4_KEY key;

	if (type == ENCRYPT_TGO)
	{
		char key_buf[128] = {0};
		unsigned short src_len = 0;

		key_buf[offset++] = '\x01'; // protocol version
		key_buf[offset++] = '\x02'; // command
		key_buf[offset++] = '\x01'; // type
		key_buf[offset++] = '\x01'; // cipher
		key_buf[offset++] = '\x01'; // atype

		memset(key_buf + offset, 0, 4); // dst.ip
		offset += 4;
		memset(key_buf + offset, 0, 2); // dst.port
		offset += 2;

		src_len = htons(inlen);
		memcpy(key_buf + offset, &src_len, 2);
		offset += 2;

		memcpy(outbuf, key_buf, offset);
		memcpy(outbuf + offset, inbuf, inlen);
		
		_RC4_set_key(&key, offset, (unsigned char *)(key_buf));
		_RC4(&key, contentlen, (unsigned char *)(outbuf)+offset, (unsigned char *)(outbuf)+offset);
		contentlen = inlen + 14;
	}
	else
	{
		_RC4_set_key(&key, strlen(key_data_ptr), key_data_ptr);
		_RC4(&key, contentlen, (unsigned char *)(inbuf), (unsigned char *)(outbuf));
	}

	outbuf[contentlen] = '\0';
	*outlen = contentlen;

	return 0;
}

int decrypt_packet(char *buf, int *len, char *decrypted, encrypt_type_e type)
{
	RC4_KEY key;
	char inbuf[1024] = {0};
	int index = 0;
	unsigned short contentlen = *len;
	*decrypted = 0;

	if (*len == 0 || buf == NULL)
		return -1;

	if (*len > 1430) // not large than MTU(1470) - sizeof(udp header)
		return 1;

	if (type == ENCRYPT_TGO)
	{
		if (buf[0] == 0x01 && buf[1] == 0x02 && buf[2] == 0x01 && buf[3] == 0x01) 
		{
			memcpy(&contentlen, buf + 11, 2);

			contentlen = ntohs(contentlen);
			if (contentlen > (*len - 13))
				return 1; // packet length error

			_RC4_set_key(&key, 13, (unsigned char *)buf); // packet header is key for rc4
			_RC4(&key, contentlen, (unsigned char *)buf + 13, (unsigned char *)buf);

			buf[contentlen] = 0;
			*decrypted = 1;
			*len = contentlen;
		}
	}
	else
	{
		if (buf[0] != '{' && buf[contentlen-1] != '}') 
		{
			memcpy(inbuf, buf, contentlen);
			inbuf[contentlen] = '\0';
			
			_RC4_set_key(&key, strlen(key_data_ptr), key_data_ptr);
			_RC4(&key, contentlen, (unsigned char *)inbuf, (unsigned char *)buf);

			buf[contentlen] = '\0';
			*decrypted = 1;
			*len = contentlen;
		}
	}

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ugo_test.h"
#include "../../../AMR/amr_interface.h"
#include "uc.h"

#define AMR_MAGIC_NUMBER "#!AMR\n"

int ugo_test_login(const char *ssid, int netmode)
{
	return uc_login(ssid, netmode);
}

int ugo_test_logout(void)
{
	return uc_logout();
}

int ugo_test_set_codecs(void* pcodeclist)
{
	return uc_set_codecs(pcodeclist);
}

int ugo_test_amr_record(const char* filepath, const char* filename)
{
//#ifndef VIGO_ENABLED 
#if 0
    short mode = MR515;
	short dtx = 1;
	FILE *file_speech = NULL;           /* File of speech data               */
	FILE *file_serial = NULL;           /* File of coded bits                */
	AMR_encinst_t_ *amr_enc;
	short buf[160];
	unsigned char outbuf[500];
	int n;
	int frame = 0;
	short returnvalue = 0;
    char strpath[128];
   
    sprintf(strpath, "%s/%s", filepath, filename);
	file_speech = fopen(strpath,"rb");
     printf("amr_src[%s]\n", strpath);
    
    sprintf(strpath, "%s/20131220_enc_515_2.amr", filepath);
	file_serial = fopen(strpath,"wb");
     printf("amr_dec[%s]\n", strpath);
    
    returnvalue = WebRtcAmr_CreateEnc(&(amr_enc));
	if(returnvalue)
	{
		printf("WebRtcAmr_CreateEnc Error");
	}
	returnvalue = WebRtcAmr_EncoderInit(amr_enc, dtx);
	if(returnvalue)
	{
		printf("WebRtcAmr_EncoderInit Error");
	}
	
	fwrite(AMR_MAGIC_NUMBER, sizeof(unsigned char), strlen(AMR_MAGIC_NUMBER), file_serial);
    
	while (fread (buf, sizeof (short), 160, file_speech) == 160)
	{
		frame++;
		n = WebRtcAmr_Encode(amr_enc, buf, 320, outbuf, mode);
		printf("%d\n", n);
		fwrite(outbuf, 1, n, file_serial);
	}
	returnvalue = WebRtcAmr_FreeEnc(amr_enc);
	if(returnvalue)
	{
		printf("WebRtcAmr_FreeEnc Error");
	}
	fclose(file_speech);
	fclose(file_serial);
#endif    
    
	return 0;
}
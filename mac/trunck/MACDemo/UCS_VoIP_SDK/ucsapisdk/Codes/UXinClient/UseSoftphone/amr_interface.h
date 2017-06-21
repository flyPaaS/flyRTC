/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : amr_interface.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/12/3
  Last Modified :
  Description   : audio codec amr_nb interface file
  Function List :
  History       :
  1.Date        : 2013/12/3
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef MODUL_AMR_NB_H
#define MODUL_AMR_NB_H

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

typedef char 			Word8;
typedef unsigned char 	UWord8;
typedef short 			Word16;
typedef long 			Word32;
typedef float 			Float32;
typedef double 			Float64;
typedef int 			Flag;

/*used  coder modes*/
enum Mode 
{ 
	MR475 = 0,
	MR515,            
	MR59,
	MR67,
	MR74,
	MR795,
	MR102,
	MR122,            
	MRDTX,
	N_MODES /* number of (SPC) modes */
};

/*amr encode inst*/
typedef struct
{
	void* encCtx;
	void* pidSyncCtx;
}AMR_encinst_t_;


/*amr decoder inst*/
typedef void AMR_decinst_t_;


#ifdef __cplusplus
extern "C" {
#endif


/************************************************************
 Function	 : WebRtcAmr_CreateEnc
 Description : create encoder inst
 Input      : enc => encoder inst point addr
 Output 	 : enc => encoder inst value 
 Return 	 : if 0 is success  else it is error
 Remark 	 : None
 Modified	 : 2013/1/16	V1.0.0	Rookie
 ************************************************************/
 Word16 WebRtcAmr_CreateEnc(OUT AMR_encinst_t_** enc);


 /************************************************************
  Function  : WebRtcAmr_EncoderInit
  Description : init encoder inst
  Input 	  : encInst => encoder inst point
  		    dtxMode = > dtx mode
  Output	  : None
  Return	  : if 0 is success  else it is error
  Remark	  : None
  Modified  : 2013/1/16	 V1.0.0  Rookie
  ************************************************************/
 Word16 WebRtcAmr_EncoderInit(IN AMR_encinst_t_* encInst, IN Word16 dtxMode);


 /************************************************************
  Function  : WebRtcAmr_Encode
  Description : encoder processs
  Input 	  : encInst => encoder inst point
  		    new_speech => speech input
  		    nSamples => input length(this option is not used in here)
  		    packed_bits => serial bit stream
  		    mode => used speech coder mode(see enum Mode defined)
  Output	  : enc => encoder inst value 
  Return	  : if 0 is success  else it is error
  Remark	  : None
  Modified  : 2013/1/16	 V1.0.0  Rookie
  ************************************************************/
 Word16 WebRtcAmr_Encode(IN AMR_encinst_t_* encInst, 
					IN Word16 *new_speech,  
					IN Word16 nSamples,	    
					OUT UWord8 *packed_bits,
					IN Word16 mode);     	


 /************************************************************
  Function  : WebRtcAmr_FreeEnc
  Description : free encode inst
  Input 	  : encInst => encoder inst point
  Output	  : None
  Return	  : if 0 is success  else it is error
  Remark	  : None
  Modified  : 2013/1/16	 V1.0.0  Rookie
  ************************************************************/
 Word16 WebRtcAmr_FreeEnc(IN AMR_encinst_t_* encInst);


 /************************************************************
  Function  : WebRtcAmr_CreateDec
  Description : create decoder inst
  Input 	  : enc => encoder inst point addr
  Output	  : enc => encoder inst value
  Return	  : if 0 is success  else it is error
  Remark	  : None
  Modified  : 2013/1/16	 V1.0.0  Rookie
  ************************************************************/
 Word16 WebRtcAmr_CreateDec(OUT AMR_decinst_t_** enc);


 /************************************************************
  Function  : WebRtcAmr_DecoderInit
  Description : init decoder inst
  Input 	  : decInst => encoder inst point 
  Output	  : None
  Return	  : if 0 is success  else it is error
  Remark	  : None
  Modified  : 2013/1/16	 V1.0.0  Rookie
  ************************************************************/
 Word16 WebRtcAmr_DecoderInit(IN AMR_decinst_t_* decInst);


 /************************************************************
  Function  : WebRtcAmr_Decode
  Description : decoder process
  Input 	  : decInst => encoder inst point 
  		    packed_bits => serial bit stream
  		    len => bit stream length(len != 0)
  		    synth => synthesis speech (postfiltered  output)
  		    speechType => speech type(this option not used in here)
  Output	  : None
  Return	  : if 0 is success  else it is error
  Remark	  : None
  Modified  : 2013/1/16	 V1.0.0  Rookie
  ************************************************************/
 Word16 WebRtcAmr_Decode(IN AMR_decinst_t_ *decInst, 
							    IN UWord8 *packed_bits, 
							    IN Word16 len,
							    OUT Word16 *synth,
							    IN Word16* speechType);


 /************************************************************
  Function  : WebRtcAmr_FreeDec
  Description : free decoder inst 
  Input 	  : decInst => encoder inst point 
  Output	  : None
  Return	  : if 0 is success  else it is error
  Remark	  : None
  Modified  : 2013/1/16	 V1.0.0  Rookie
  ************************************************************/
 Word16 WebRtcAmr_FreeDec(IN AMR_decinst_t_* decInst);


 /************************************************************
  Function  : WebRtcAmr_Version
  Description : free decoder inst 
  Input 	  : versionStr => version buf 
  		    len => max verson buf size
  Output	  : versionStr => amr version
  Return	  : if 0 is success  else it is error
  Remark	  : None
  Modified  : 2013/1/16	 V1.0.0  Rookie
  ************************************************************/
 void WebRtcAmr_Version(OUT char *versionStr, IN Word16 len);

#ifdef __cplusplus
}
#endif

#endif 
 



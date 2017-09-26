/******************************************************************************
  Copyright (C), 2001-2015, DCN Co., Ltd.
**************************************************************************
File Name     : ugo_auto_test.h
Version       : Initial Draft
Author        : Vinton.Liu
Created       : 2015/5/04
Last Modified :
Description   : voice auto test function
Function List :
History       :
1.Date        : 2015/5/04
  Author      : Vinton.Liu
  Modification: Created file

***************************************************************************/

#ifndef __UGO_AUTO_TEST_H__
#define __UGO_AUTO_TEST_H__

#include "cfg.h"

#ifdef AUTO_TEST_SWITCH

#include "ugo.h"

#define TIMER_PERIOD_MS			1000	//the time of thread to execute,ms;
#define MAX_NORMAL_TEST_FILES     10     //the count of test files

#define RTP_HEADER_OFFSET   38
#define RTP_BODY_LENGTH   40

#define PT_CONUT   15


enum RTPDirections
{
    kRtpIncoming = 0,
    kRtpOutgoing
};

//for auto test
enum eAuto_test_mode
{
    eNormal_test			= 0,	//same as normal calling
    eRound_trip			= 100,	//A call B, the media stream of A will come back in B, B don't send media stream self.
    eSelf_cycle			= 200,	//A call B, the media stream of A will come back in A, the media stream of B will come back in B
    
    eCallerID		= 1,	//caller
    ecalledID		= 2,	//called  
    eNoID			= 3,	//No  
};

enum eAuto_logic_crl
{
    eAuto_idle   = 0x1,
    eAuto_dialing   = 0x2,
    eAuto_inCalling   = 0x4,
    eAuto_calling   = 0x8,

    eAuto_playing   = 0x10,
    eAuto_recording   = 0x20,
    eAuto_dumpRcvRtp   = 0x40,
    eAuto_dumpSndRtp   = 0x80,
};


/*** dial config struct ***/
typedef union dial_cfg
{
    call_dialing_pm_t dialParam;
    conference_dialing_param_t confDialCfg;
} dial_cfg_u;

typedef struct auto_test_crtl_tag
{
    /***dial info***/
    dial_cfg_u dialCfg;

    /***test option***/
	char  fecCtrl; 		//fec function test switch.
	char  p2pCtrl;		//P2P function test switch.
	char  prtpCtrl;		//PRTP function test switch.
    char  emodelCtr;    //emodel function test switch.
	char  playCtrl;		//play function test switch.
	char  recCtrl;		//recording function test switch.
	char  logCtrl;		//log function test switch.
	char  p862Ctrl;	    //p862 function test switch. 
	char  encryptCtrl;    //encryption of rtp function test switch. 
	char  exlDevCtrl;    //external playing device function test switch. 
	char  videoCtrl;    //video function test switch. 

    /***call option***/
    int   callRole;      //call role, caller or called
    int   isConfCall;    // conference call or not
    int   confFileIndex; // conference test file index
    int   confRefFileIdx; // conference test reference file index
    int   callCnt;      //call count
    int   callTime;     //call time every time    
    int   callInterval; //call interval every time    
}auto_test_crtl_t;

typedef struct auto_test_state_tag
{
    int isTesting;      /* auto test is start */
    int callRoleId;     /* test call role, caller or called */
    int callCount;      /* counter for test calls */
    int testFileIdx;    /* current test file reference index */
    int callState;      /* test call state */
    int playCount;      /* counter for file play */
    double avgPesqValue; /* average pesq value of test calls */
}auto_test_state_t;

typedef struct auto_test_file_tag
{
	const char* pcRec;
	const char* pcPlay;
	const char* pcSntRTPDump;
	const char* pcrcvRTPDump;
} auto_test_file_t;

typedef struct conf_auto_ref_file_tag
{
    const char* pcMix;
} conf_auto_ref_file_t;

typedef struct auto_test_pt_key_tag
{
	const char* name;
	int pt;
} auto_test_pt_key_t;


void Start_auto_test(auto_test_crtl_t *pstAutoTestCtrl);
void Stop_auto_test(void);
void StartTestAssistant(void);
void StopTestAssistant(void);
void StartTestThread(void);
void StopTestThread(void);
void StartTestFunctionInCall(void);
void StartTestFunctionInCallEnd(int iFileSeq);
void StartTestFunctionInTestEnd();
void StopTestFunction();

#endif /* end of AUTO_TEST_SWITCH */
#endif /* end of __UGO_AUTO_TEST_H__ */


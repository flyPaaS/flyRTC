/*
 *  Copyright (c) 2015 The UcPaaS project authors. All Rights Reserved.
 * 
 */
 
#include "ugo_auto_test.h"
#include "comm.h"
#include "typedef.h"

#ifdef AUTO_TEST_SWITCH

static volatile int keepAlive = FALSE;
static ms_thread_t 		ethread_id = NULL;
static auto_test_crtl_t g_stAutoTestCtrl;
static auto_test_state_t g_stAutoTestState;

static FILE *pfTestLog = NULL;
static FILE *pfRecFile = NULL;
static FILE *pfRtpDumpSend = NULL;
static FILE *pfRtpDumpRcv = NULL;

volatile static int  g_iIncallStatusCnt = 0;  //状态/时间计数

static const auto_test_file_t g_stAutoTestFile[] = 
{
#ifdef _WIN32
	{ "E:\\testFiles\\dg105.wav", "E:\\testFiles\\or105.wav", "E:\\testFiles\\or105.sntRTPDump", "E:\\testFiles\\or105.rcvRTPDump"},
	{ "E:\\testFiles\\dg109.wav", "E:\\testFiles\\or109.wav", "E:\\testFiles\\or109.sntRTPDump", "E:\\testFiles\\or109.rcvRTPDump"},
	{ "E:\\testFiles\\dg114.wav", "E:\\testFiles\\or114.wav", "E:\\testFiles\\or114.sntRTPDump", "E:\\testFiles\\or114.rcvRTPDump"},
	{ "E:\\testFiles\\dg129.wav", "E:\\testFiles\\or129.wav", "E:\\testFiles\\or129.sntRTPDump", "E:\\testFiles\\or129.rcvRTPDump"},
	{ "E:\\testFiles\\dg134.wav", "E:\\testFiles\\or134.wav", "E:\\testFiles\\or134.sntRTPDump", "E:\\testFiles\\or134.rcvRTPDump"},
	{ "E:\\testFiles\\dg137.wav", "E:\\testFiles\\or137.wav", "E:\\testFiles\\or137.sntRTPDump", "E:\\testFiles\\or137.rcvRTPDump"},
	{ "E:\\testFiles\\dg145.wav", "E:\\testFiles\\or145.wav", "E:\\testFiles\\or145.sntRTPDump", "E:\\testFiles\\or145.rcvRTPDump"},
	{ "E:\\testFiles\\dg149.wav", "E:\\testFiles\\or149.wav", "E:\\testFiles\\or149.sntRTPDump", "E:\\testFiles\\or149.rcvRTPDump"},
	{ "E:\\testFiles\\dg152.wav", "E:\\testFiles\\or152.wav", "E:\\testFiles\\or152.sntRTPDump", "E:\\testFiles\\or152.rcvRTPDump"},
	{ "E:\\testFiles\\dg154.wav", "E:\\testFiles\\or154.wav", "E:\\testFiles\\or154.sntRTPDump", "E:\\testFiles\\or154.rcvRTPDump"},
    { "E:\\testFiles\\rA.wav",    "E:\\testFiles\\A.wav",     "E:\\testFiles\\snA.sntRTPDump",   "E:\\testFiles\\rcvA.rcvRTPDump"},
    { "E:\\testFiles\\rB.wav",    "E:\\testFiles\\B.wav",     "E:\\testFiles\\snB.sntRTPDump",   "E:\\testFiles\\rcvB.rcvRTPDump"},
    { "E:\\testFiles\\rC.wav",    "E:\\testFiles\\C.wav",     "E:\\testFiles\\snC.sntRTPDump",   "E:\\testFiles\\rcvC.rcvRTPDump"},
    { "E:\\testFiles\\rD.wav",    "E:\\testFiles\\D.wav",     "E:\\testFiles\\snD.sntRTPDump",   "E:\\testFiles\\rcvD.rcvRTPDump"},
    { "E:\\testFiles\\rE.wav",    "E:\\testFiles\\E.wav",     "E:\\testFiles\\snE.sntRTPDump",   "E:\\testFiles\\rcvE.rcvRTPDump"},
    { "E:\\testFiles\\rF.wav",    "E:\\testFiles\\F.wav",     "E:\\testFiles\\snF.sntRTPDump",   "E:\\testFiles\\rcvF.rcvRTPDump"},
    { "E:\\testFiles\\rG.wav",    "E:\\testFiles\\G.wav",     "E:\\testFiles\\snG.sntRTPDump",   "E:\\testFiles\\rcvG.rcvRTPDump"},
    { "E:\\testFiles\\rH.wav",    "E:\\testFiles\\H.wav",     "E:\\testFiles\\snH.sntRTPDump",   "E:\\testFiles\\rcvH.rcvRTPDump"},
    { "E:\\testFiles\\rI.wav",    "E:\\testFiles\\I.wav",     "E:\\testFiles\\snI.sntRTPDump",   "E:\\testFiles\\rcvI.rcvRTPDump"},
    { "E:\\testFiles\\rJ.wav",    "E:\\testFiles\\J.wav",     "E:\\testFiles\\snJ.sntRTPDump",   "E:\\testFiles\\rcvJ.rcvRTPDump"},
    { "E:\\testFiles\\rK.wav",    "E:\\testFiles\\K.wav",     "E:\\testFiles\\snK.sntRTPDump",   "E:\\testFiles\\rcvK.rcvRTPDump"},
#elif defined ANDROID
    { "/mnt/sdcard/testFiles/dg105.wav", "/mnt/sdcard/testFiles/or105.wav", "/mnt/sdcard/testFiles/or105.sntRTPDump", "/mnt/sdcard/testFiles/or105.rcvRTPDump"},
	{ "/mnt/sdcard/testFiles/dg109.wav", "/mnt/sdcard/testFiles/or109.wav", "/mnt/sdcard/testFiles/or109.sntRTPDump", "/mnt/sdcard/testFiles/or109.rcvRTPDump"},
	{ "/mnt/sdcard/testFiles/dg114.wav", "/mnt/sdcard/testFiles/or114.wav", "/mnt/sdcard/testFiles/or114.sntRTPDump", "/mnt/sdcard/testFiles/or114.rcvRTPDump"},
	{ "/mnt/sdcard/testFiles/dg129.wav", "/mnt/sdcard/testFiles/or129.wav", "/mnt/sdcard/testFiles/or129.sntRTPDump", "/mnt/sdcard/testFiles/or129.rcvRTPDump"},
	{ "/mnt/sdcard/testFiles/dg134.wav", "/mnt/sdcard/testFiles/or134.wav", "/mnt/sdcard/testFiles/or134.sntRTPDump", "/mnt/sdcard/testFiles/or134.rcvRTPDump"},
	{ "/mnt/sdcard/testFiles/dg137.wav", "/mnt/sdcard/testFiles/or137.wav", "/mnt/sdcard/testFiles/or137.sntRTPDump", "/mnt/sdcard/testFiles/or137.rcvRTPDump"},
	{ "/mnt/sdcard/testFiles/dg145.wav", "/mnt/sdcard/testFiles/or145.wav", "/mnt/sdcard/testFiles/or145.sntRTPDump", "/mnt/sdcard/testFiles/or145.rcvRTPDump"},
	{ "/mnt/sdcard/testFiles/dg149.wav", "/mnt/sdcard/testFiles/or149.wav", "/mnt/sdcard/testFiles/or149.sntRTPDump", "/mnt/sdcard/testFiles/or149.rcvRTPDump"},
	{ "/mnt/sdcard/testFiles/dg152.wav", "/mnt/sdcard/testFiles/or152.wav", "/mnt/sdcard/testFiles/or152.sntRTPDump", "/mnt/sdcard/testFiles/or152.rcvRTPDump"},
	{ "/mnt/sdcard/testFiles/dg154.wav", "/mnt/sdcard/testFiles/or154.wav", "/mnt/sdcard/testFiles/or154.sntRTPDump", "/mnt/sdcard/testFiles/or154.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rA.wav",    "/mnt/sdcard/testFiles/A.wav",     "/mnt/sdcard/testFiles/snA.sntRTPDump",   "/mnt/sdcard/testFiles/rcvA.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rB.wav",    "/mnt/sdcard/testFiles/B.wav",     "/mnt/sdcard/testFiles/snB.sntRTPDump",   "/mnt/sdcard/testFiles/rcvB.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rC.wav",    "/mnt/sdcard/testFiles/C.wav",     "/mnt/sdcard/testFiles/snC.sntRTPDump",   "/mnt/sdcard/testFiles/rcvC.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rD.wav",    "/mnt/sdcard/testFiles/D.wav",     "/mnt/sdcard/testFiles/snD.sntRTPDump",   "/mnt/sdcard/testFiles/rcvD.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rE.wav",    "/mnt/sdcard/testFiles/E.wav",     "/mnt/sdcard/testFiles/snE.sntRTPDump",   "/mnt/sdcard/testFiles/rcvE.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rF.wav",    "/mnt/sdcard/testFiles/F.wav",     "/mnt/sdcard/testFiles/snF.sntRTPDump",   "/mnt/sdcard/testFiles/rcvF.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rG.wav",    "/mnt/sdcard/testFiles/G.wav",     "/mnt/sdcard/testFiles/snG.sntRTPDump",   "/mnt/sdcard/testFiles/rcvG.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rH.wav",    "/mnt/sdcard/testFiles/H.wav",     "/mnt/sdcard/testFiles/snH.sntRTPDump",   "/mnt/sdcard/testFiles/rcvH.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rI.wav",    "/mnt/sdcard/testFiles/I.wav",     "/mnt/sdcard/testFiles/snI.sntRTPDump",   "/mnt/sdcard/testFiles/rcvI.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rJ.wav",    "/mnt/sdcard/testFiles/J.wav",     "/mnt/sdcard/testFiles/snJ.sntRTPDump",   "/mnt/sdcard/testFiles/rcvJ.rcvRTPDump"},
    { "/mnt/sdcard/testFiles/rK.wav",    "/mnt/sdcard/testFiles/K.wav",     "/mnt/sdcard/testFiles/snK.sntRTPDump",   "/mnt/sdcard/testFiles/rcvK.rcvRTPDump"},
#else
    { ".\\testFiles\\dg105.wav", ".\\testFiles\\or105.wav", ".\\testFiles\\or105.sntRTPDump", ".\\testFiles\\or105.rcvRTPDump"},
	{ ".\\testFiles\\dg109.wav", ".\\testFiles\\or109.wav", ".\\testFiles\\or109.sntRTPDump", ".\\testFiles\\or109.rcvRTPDump"},
	{ ".\\testFiles\\dg114.wav", ".\\testFiles\\or114.wav", ".\\testFiles\\or114.sntRTPDump", ".\\testFiles\\or114.rcvRTPDump"},
	{ ".\\testFiles\\dg129.wav", ".\\testFiles\\or129.wav", ".\\testFiles\\or129.sntRTPDump", ".\\testFiles\\or129.rcvRTPDump"},
	{ ".\\testFiles\\dg134.wav", ".\\testFiles\\or134.wav", ".\\testFiles\\or134.sntRTPDump", ".\\testFiles\\or134.rcvRTPDump"},
	{ ".\\testFiles\\dg137.wav", ".\\testFiles\\or137.wav", ".\\testFiles\\or137.sntRTPDump", ".\\testFiles\\or137.rcvRTPDump"},
	{ ".\\testFiles\\dg145.wav", ".\\testFiles\\or145.wav", ".\\testFiles\\or145.sntRTPDump", ".\\testFiles\\or145.rcvRTPDump"},
	{ ".\\testFiles\\dg149.wav", ".\\testFiles\\or149.wav", ".\\testFiles\\or149.sntRTPDump", ".\\testFiles\\or149.rcvRTPDump"},
	{ ".\\testFiles\\dg152.wav", ".\\testFiles\\or152.wav", ".\\testFiles\\or152.sntRTPDump", ".\\testFiles\\or152.rcvRTPDump"},
	{ ".\\testFiles\\dg154.wav", ".\\testFiles\\or154.wav", ".\\testFiles\\or154.sntRTPDump", ".\\testFiles\\or154.rcvRTPDump"},
    { ".\\testFiles\\rA.wav",    ".\\testFiles\\A.wav",     ".\\testFiles\\snA.sntRTPDump",   ".\\testFiles\\rcvA.rcvRTPDump"},
    { ".\\testFiles\\rB.wav",    ".\\testFiles\\B.wav",     ".\\testFiles\\snB.sntRTPDump",   ".\\testFiles\\rcvB.rcvRTPDump"},
    { ".\\testFiles\\rC.wav",    ".\\testFiles\\C.wav",     ".\\testFiles\\snC.sntRTPDump",   ".\\testFiles\\rcvC.rcvRTPDump"},
    { ".\\testFiles\\rD.wav",    ".\\testFiles\\D.wav",     ".\\testFiles\\snD.sntRTPDump",   ".\\testFiles\\rcvD.rcvRTPDump"},
    { ".\\testFiles\\rE.wav",    ".\\testFiles\\E.wav",     ".\\testFiles\\snE.sntRTPDump",   ".\\testFiles\\rcvE.rcvRTPDump"},
    { ".\\testFiles\\rF.wav",    ".\\testFiles\\F.wav",     ".\\testFiles\\snF.sntRTPDump",   ".\\testFiles\\rcvF.rcvRTPDump"},
    { ".\\testFiles\\rG.wav",    ".\\testFiles\\G.wav",     ".\\testFiles\\snG.sntRTPDump",   ".\\testFiles\\rcvG.rcvRTPDump"},
    { ".\\testFiles\\rH.wav",    ".\\testFiles\\H.wav",     ".\\testFiles\\snH.sntRTPDump",   ".\\testFiles\\rcvH.rcvRTPDump"},
    { ".\\testFiles\\rI.wav",    ".\\testFiles\\I.wav",     ".\\testFiles\\snI.sntRTPDump",   ".\\testFiles\\rcvI.rcvRTPDump"},
    { ".\\testFiles\\rJ.wav",    ".\\testFiles\\J.wav",     ".\\testFiles\\snJ.sntRTPDump",   ".\\testFiles\\rcvJ.rcvRTPDump"},
    { ".\\testFiles\\rK.wav",    ".\\testFiles\\K.wav",     ".\\testFiles\\snK.sntRTPDump",   ".\\testFiles\\rcvK.rcvRTPDump"}, 
#endif
};

//conference auto test reference files
static const conf_auto_ref_file_t g_stConfAutoRefFile[] = 
{
#ifdef _WIN32
    { "E:\\testFiles\\confRefiles\\A.wav" },
    { "E:\\testFiles\\confRefiles\\D.wav" },
    { "E:\\testFiles\\confRefiles\\AD.wav" },
    { "E:\\testFiles\\confRefiles\\DF.wav" },
    { "E:\\testFiles\\confRefiles\\BD.wav" },
    { "E:\\testFiles\\confRefiles\\AEC.wav" },
#elif defined ANDROID
    { "/mnt/sdcard/testFiles/confRefiles/A.wav"   },
    { "/mnt/sdcard/testFiles/confRefiles/D.wav"   },
    { "/mnt/sdcard/testFiles/confRefiles/AD.wav"  },
    { "/mnt/sdcard/testFiles/confRefiles/DF.wav"  },
    { "/mnt/sdcard/testFiles/confRefiles/BD.wav"  },
    { "/mnt/sdcard/testFiles/confRefiles/AEC.wav" },
#else
    { ".\\testFiles\\confRefiles\\A.wav"   },
    { ".\\testFiles\\confRefiles\\D.wav"   },
    { ".\\testFiles\\confRefiles\\AD.wav"  },
    { ".\\testFiles\\confRefiles\\DF.wav"  },
    { ".\\testFiles\\confRefiles\\BD.wav"  },
    { ".\\testFiles\\confRefiles\\AEC.wav" },
#endif
};

static const auto_test_pt_key_t g_stPtKey[] = 
{	
	{"G729",            18}, 
    {"CN",              13}, 
    {"CN",              98}, 
    {"CN",              99}, 
    {"",                105}, 
    {"telephone-event", 101}, 
    {"SILK",            106}, 
    {"AMR",             107}, 
	{"RED",             127}, 
	{"SILKWB",          115}, 
	{"PCMU",            110}, 
	{"OPUS",            111}, 
	{"NULL",            112}, 
	{"NULL",            113}, 
	{"NULL",            114}
};

#ifdef _DEBUG
#pragma comment(lib, "Debug\\P862.lib")
#else
#pragma comment(lib, "Release\\P862.lib")
#endif
extern "C"
{
   extern double calc_p862_lib (int argc, const char *argv []);
}

void encryption(ucpaas_uint8* in_data,
			        ucpaas_uint8* out_data,
			        ucpaas_int32 bytes_in,
			        ucpaas_int32* bytes_out)
{
    int i, j;

    for (i = 0, j=(bytes_in-1); i < bytes_in; i++, j--)
    {
        out_data[i] = in_data[j];
    }

    *bytes_out = bytes_in;
}

void decryption(ucpaas_uint8* in_data,
			        ucpaas_uint8* out_data,
			        ucpaas_int32 bytes_in,
			        ucpaas_int32* bytes_out)
{
    int i, j;
   
    for (i = 0, j=(bytes_in-1); i < bytes_in; i++, j--)
    {
        out_data[i] = in_data[j];
    }
    *bytes_out = bytes_in;
}

void initAutoTestState(int callRole)
{
    memset(&g_stAutoTestState, 0x00, sizeof(auto_test_state_t));
    g_stAutoTestState.callRoleId = callRole;
}

int testRole()
{
    return g_stAutoTestState.callRoleId;
}

/***start test***/
void Start_auto_test(auto_test_crtl_t *pstAutoTestCtrl)
{
    char logPath[128] = {0};

#ifdef _WIN32
    strcpy(logPath, "E:\\testFiles\\selfCheckLog.dat");
#elif defined ANDROID
    strcpy(logPath, "/mnt/sdcard/testFiles/selfCheckLog.log");
#else
    strcpy(logPath, ".\\testFiles\\selfCheckLog.dat");
#endif

    ms_report("Start_auto_test enter");
    if ( NULL == pfTestLog )
    {
    	pfTestLog = fopen(logPath, "w");
        if ( NULL == pfTestLog )
        {
            /* here must to return, else will be caused to crash. */
            ms_error("error: can't open file: %s!!!", logPath);
			return;
        }

    	if ( pstAutoTestCtrl == NULL )
        {
            fprintf(pfTestLog, "%s\n", "error: user info setting is wrong, please check it!");
            return;
        }

        if ( g_stAutoTestState.isTesting )
        {
            fprintf(pfTestLog, "%s\n", "error: can't start auto test because test running!");
            return;
        }

    	if ( UGo_get_state() != eUGo_STATE_WAIT )
        {
            fprintf(pfTestLog, "%s\n", "error: can't start auto test because call state is busy!");
            return;
        }   

        /*test info*/
        fprintf(pfTestLog, "/*********************************************************/\n");
        fprintf(pfTestLog, "/*                        test begin                     */\n");
        fprintf(pfTestLog, "/*********************************************************/\n\n");

        //update test control info 
        memcpy(&g_stAutoTestCtrl, pstAutoTestCtrl, sizeof(auto_test_crtl_t));

        //init test state
        initAutoTestState(g_stAutoTestCtrl.callRole);
        g_stAutoTestState.isTesting = TRUE;
        g_stAutoTestState.callState &= 0xfffffff0;
        g_stAutoTestState.callState |= eAuto_idle;
        
        g_iIncallStatusCnt = 0;

        //open or close encrypt for rtp
        if ( g_stAutoTestCtrl.encryptCtrl )
        {
            UGo_set_encrypt_cb(&encryption, &decryption);
        }
        else
        {
            UGo_set_encrypt_cb(NULL, NULL);
        }

        //use external audio device
        if (g_stAutoTestCtrl.exlDevCtrl)
        {       
            UGo_destroy();
            UGo_init(); //use external audio device. TODO
        }

        /*init environment*/
        StartTestThread();  //start thread to test
    }
}

void Stop_auto_test(void)
{
    ms_report("Stop_auto_test enter.");
    if ( NULL != pfTestLog )
    {
        /*detroy environment*/
        ms_report("Stop_auto_test: try stop thread.");
        StopTestThread();
        ms_report("Stop_auto_test: thread stopped. state: %d", UGo_get_state());

        if (eUGo_STATE_WAIT != UGo_get_state())
        {
            if ( eCallerID == testRole() )
            {
                StopTestAssistant();
            }
            UGo_call_hangup(eUGo_Reason_HungupMyself);        
        }

        StartTestFunctionInTestEnd();
        g_iIncallStatusCnt = 0;
        g_stAutoTestState.isTesting = FALSE;

        /*test info*/
        fprintf(pfTestLog, "/*********************************************************/\n");
        fprintf(pfTestLog, "/*                        test end                       */\n");
        fprintf(pfTestLog, "/*********************************************************/\n");

        fclose(pfTestLog);
        pfTestLog = NULL;
    }
}

void* Auto_test_thread(void *arg)
{
	while ( keepAlive )
	{ 
    	/*excute once every 1s*/
    	ms_usleep(TIMER_PERIOD_MS * 1000);

        if ( eCallerID == testRole() )
        {
            switch (UGo_get_state())
            {
        	case eUGo_STATE_WAIT:
                if ( g_stAutoTestState.callState & eAuto_calling )
                {
                    // sometimes would hang up by peer, prev state is connect
                    StopTestAssistant();
                
                    StartTestFunctionInCall();

                    StartTestFunctionInCallEnd(g_stAutoTestState.testFileIdx);
                    StopTestFunction();

                    fprintf(pfTestLog, \
                        "/***info: Dial end, hang up by peer or play file end, times: %d ***/\r\n",
                        g_stAutoTestState.callCount);

        			g_stAutoTestState.callCount++; //finished one file playing.
                    g_iIncallStatusCnt = 0;

                    //Log status
                    g_stAutoTestState.callState &= 0xfffffff0;
                    g_stAutoTestState.callState |= eAuto_idle;
                }                
                else if (( g_stAutoTestState.callCount == 0 )   //whether first call? no wait if yes, wait if others
                    ||(g_iIncallStatusCnt * TIMER_PERIOD_MS > g_stAutoTestCtrl.callInterval * 1000))
                {                   
                    if ( g_stAutoTestState.callState & eAuto_dialing )
                    {
                        fprintf(pfTestLog, "Error: Dial failed, stop test. pls check why can't dialing...\n");
                        /* mod by VintonLiu on 20141119 for thread block when call thread_join */
                        #if 0
                        //Stop_auto_test();
                        #else
                        keepAlive = FALSE;
                        #endif
                        /* VintonLiu mod end */
                    }                    
                    else if ( g_stAutoTestState.callCount < g_stAutoTestCtrl.callCnt )
            		{
                        /* mod by VintonLiu on 20141114 for add conference auto test */
                        if ( !g_stAutoTestCtrl.isConfCall )
                        {
                            ugo_call_dialing_para_t callparam;
                            memset(&callparam, 0x00, sizeof(ugo_call_dialing_para_t));
                            memcpy(&callparam, &g_stAutoTestCtrl.dialCfg.dialParam, sizeof(ugo_call_dialing_para_t));
                            UGo_call_dialing(&callparam);
                            g_stAutoTestState.testFileIdx = g_stAutoTestState.callCount % MAX_NORMAL_TEST_FILES;
                        }
                        else
                        {
                            /* conference call */
                            ugo_conf_dialing_para_t conference_param;
                            memset(&conference_param, 0x00, sizeof(ugo_conf_dialing_para_t));
                            memcpy(&conference_param, &g_stAutoTestCtrl.dialCfg.confDialCfg, sizeof(conference_dialing_param_t));
                            UGo_conference_call_dialing(&conference_param);
                            g_stAutoTestState.testFileIdx = g_stAutoTestCtrl.confFileIndex + 10;
                        }
                        /* VintonLiu mod end */
                        
                        g_iIncallStatusCnt = 0;
                        fprintf(pfTestLog, "/***info: Dial begin, times: %d ***/\n", g_stAutoTestState.callCount);

                        //Log dialing
                        g_stAutoTestState.callState &= 0xfffffff0;
                        g_stAutoTestState.callState |= eAuto_dialing;
            		}
            		else
            		{
                        fprintf(pfTestLog, "/**** auto test end ***/\r\n");
            			/* mod by VintonLiu on 20141119 for thread block when call thread_join */
                        #if 0
                        //Stop_auto_test();
                        #else
                        keepAlive = FALSE;
                        #endif
                        /* VintonLiu mod end */
            		}
                }
                
                g_iIncallStatusCnt++;
            	break;
        	case eUGo_STATE_CONNECT:

                //to play/record file in first time;
                if ( g_stAutoTestState.callState & eAuto_dialing )
                {
                    StartTestAssistant();
                   
                    //Log status
                    g_stAutoTestState.callState &= 0xfffffff0;
                    g_stAutoTestState.callState |= eAuto_calling;
                }
                else if (g_iIncallStatusCnt * TIMER_PERIOD_MS > g_stAutoTestCtrl.callTime * 1000)
                {
                    StopTestAssistant();
                    
                    StartTestFunctionInCall();
                    
                    UGo_call_hangup(eUGo_Reason_HungupMyself);

                    StartTestFunctionInCallEnd(g_stAutoTestState.testFileIdx);
                    StopTestFunction();

                    fprintf(pfTestLog, "/***info: Dial end, times: %d ***/\r\n", g_stAutoTestState.callCount);

        			g_stAutoTestState.callCount++;   //finished one file playing.
                    g_iIncallStatusCnt = 0;

                    //Log status
                    g_stAutoTestState.callState &= 0xfffffff0;
                    g_stAutoTestState.callState |= eAuto_idle;
                }

                g_iIncallStatusCnt++;
            	break;
            default: 
            	break;
            }
        }
        else
        {
            switch (UGo_get_state())
            {
            case eUGo_STATE_WAIT:
            {   
                if ( g_stAutoTestState.callCount >= g_stAutoTestCtrl.callCnt )
                {
                    keepAlive = FALSE;
                }
                // sometimes would hang up by peer, prev state is connect
                else if ( g_stAutoTestState.callState & eAuto_calling )
                {
                    g_iIncallStatusCnt = 0;
                    StopTestAssistant();
                    g_stAutoTestState.callCount++;   //finished one file playing.

                    //Log status
                    g_stAutoTestState.callState &= 0xfffffff0;
                    g_stAutoTestState.callState |= eAuto_idle;
                }
            }
            break;
            case eUGo_STATE_RINGING:
                UGo_call_answered();
                /* mod by VintonLiu on 20141114 for add conference auto test */
                if ( g_stAutoTestCtrl.isConfCall )
                {
                    UGo_set_testmode(eNormal_test);
                    g_stAutoTestState.testFileIdx = g_stAutoTestCtrl.confFileIndex + 10;
                }
                else
                {
                //    UGo_set_testmode(eRound_trip);
                    g_stAutoTestState.testFileIdx = g_stAutoTestState.callCount % MAX_NORMAL_TEST_FILES;
                }
                /* VintonLiu mod end */
                
                //Log status
                g_stAutoTestState.callState &= 0xfffffff0;
                g_stAutoTestState.callState |= eAuto_inCalling;
                g_iIncallStatusCnt = 0;
                g_iIncallStatusCnt++;

                fprintf(pfTestLog, "/***info: call incoming***/\n\n");

            	break;
        	case eUGo_STATE_CONNECT:

                //to play/record file in first time;
                if ( g_stAutoTestState.callState & eAuto_inCalling )
                {
                    StartTestAssistant();
                    
                    g_iIncallStatusCnt++;
                    //Log status
                    g_stAutoTestState.callState &= 0xfffffff0;
                    g_stAutoTestState.callState |= eAuto_calling;
                }
                else if (g_iIncallStatusCnt * TIMER_PERIOD_MS > g_stAutoTestCtrl.callTime * 1000)
                {
                    if (g_stAutoTestCtrl.isConfCall)
                    {
                        StopTestAssistant();
                        UGo_call_hangup(eUGo_Reason_HungupMyself);

                        g_stAutoTestState.callCount++;   //finished one file playing.
                        g_iIncallStatusCnt = 0;

                        //Log status
                        g_stAutoTestState.callState &= 0xfffffff0;
                        g_stAutoTestState.callState |= eAuto_idle;
                    }
                }
                else
                {
                    g_iIncallStatusCnt++;
                }
                break;
            default: 
            	break;                
            }
        }
    }
    
    //calc average value
    if ( testRole() == eCallerID && g_stAutoTestCtrl.p862Ctrl && g_stAutoTestState.callCount)
    {
        g_stAutoTestState.avgPesqValue = g_stAutoTestState.avgPesqValue / g_stAutoTestState.callCount;
        fprintf(pfTestLog, "callCnt = %d average pesq = %.3f\n", \
            g_stAutoTestState.callCount, g_stAutoTestState.avgPesqValue);
        ms_report("callCnt = %d average pesq = %.3f", \
            g_stAutoTestState.callCount, g_stAutoTestState.avgPesqValue );
    }
    
    g_stAutoTestState.isTesting = FALSE;
    ms_report("Auto_test_thread exit.");
	return NULL;
}

void printInfoB(const char* chLog)
{
    fprintf(pfTestLog, "/**************************/\n");
    fprintf(pfTestLog, "/***%s***/\n", chLog);
}

void printInfoE(const char* chLog)
{
    fprintf(pfTestLog, "/***%s***/\n", chLog);
    fprintf(pfTestLog, "/**************************/\n\n\n");
}

void basicInfo()
{
    int iLoopFlag;
    char chBuf = 0;
    int i = 0;
    int iLength = 0;

    printInfoB("basic information");

    if (pfRtpDumpSend == NULL)
    {
        fprintf(pfTestLog, "failed: test suspend, file: %s handle is null!!!\n");
        return;        
    }

    fseek(pfRtpDumpSend, 0L, SEEK_END);
    iLength = ftell(pfRtpDumpSend);
    if (iLength > RTP_HEADER_OFFSET)
    {
        fseek(pfRtpDumpSend, RTP_HEADER_OFFSET, SEEK_CUR);
    }
    else
    {
        fprintf(pfTestLog, "failed: file: %s size is smaller than RTP_HEADER_OFFSET, pls check it!!!\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcSntRTPDump);
        return;            
    }

    if (1 != fread(&chBuf, 1, 1, pfRtpDumpSend))// read version
    {
        fprintf(pfTestLog, "failed: read file: %s failed, pls check it!!!\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcSntRTPDump);
        return;
    }
    if (chBuf == 0xC0)
    {
        iLoopFlag = 1;
        if(1 != fread(&chBuf, 1, 1, pfRtpDumpSend)) // read pt
        {
            fprintf(pfTestLog, "failed: read file: %s failed, pls check it!!!\n", \
                g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcSntRTPDump);
            return;
        }

        chBuf &=0x3c; 
        chBuf >>= 2;

        fprintf(pfTestLog, "Codec: %s\n", g_stPtKey[chBuf].name);
    }
    else if(chBuf == 0x80)
    {
        if(1 != fread(&chBuf, 1, 1, pfRtpDumpSend)) // read pt
        {
           fprintf(pfTestLog, "failed: read file: %s failed, pls check it!!!\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcSntRTPDump);
           return;
        }
        iLoopFlag = 1;
        for (i = 0; i < PT_CONUT; i++)
        {
            if(g_stPtKey[i].pt == chBuf)
            {
                iLoopFlag = 0;
                fprintf(pfTestLog, "Codec: %s\n", g_stPtKey[i].name);
                break;
            }
        }
    }
    else
    {
        fprintf(pfTestLog, "failed: can't parse rtp vertion, file: %s, pls check it!!!\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcSntRTPDump);
        return;
    }

    printInfoE("basic information end");
}

void CalcP862()
{
    double dPesqValue;
    const char *argv [4];

    printInfoB("p862 test begin");
    ms_report("CalcP862: enter.\n");

    argv[1] = "+8000";
    if ( !g_stAutoTestCtrl.isConfCall )
    {
        argv[2] = g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcPlay;
    }
    else
    {
        argv[2] = g_stConfAutoRefFile[g_stAutoTestCtrl.confRefFileIdx].pcMix;
    }
    argv[3] = g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcRec;
    
    //dPesqValue = calc_p862_lib(4, argv);
    g_stAutoTestState.avgPesqValue += dPesqValue;
	fprintf(pfTestLog, "ref: %s\tdeg: %s\tpesq = %.3f\n", \
        argv[2], argv[3], dPesqValue);
    ms_report("CalcP862: ref: %s\tdeg: %s\tpesq = %.3f\n", \
        argv[2], argv[3], dPesqValue);

    printInfoE("p862 test end");
}

void encrypt_rtp()
{
    printInfoB("encryption of rtp test begin");
    if ( g_stAutoTestState.avgPesqValue > 1 )
    {
        fprintf(pfTestLog, "Successful!\n");
    }
    else
    {
        fprintf(pfTestLog, "failed: voice qulity is very bad, pls check it!!!\n");
    }
    printInfoE("encryption of rtp test end");
}

void exl_playing_dev()
{
    printInfoB("external playing test begin");
    if ( g_stAutoTestState.avgPesqValue > 1)
    {
        fprintf(pfTestLog, "Successful!\n");
    }
    else
    {
        fprintf(pfTestLog, "failed: voice qulity is very bad, pls check it!!!\n");
    }
    printInfoE("external playing test end");
}

void fec()
{
    char chBuf = 0;
    int iLength = 0;

    printInfoB("fec test begin");

    if (pfRtpDumpSend == NULL)
    {
        fprintf(pfTestLog, "failed: test suspend, file: %s handle is null!!!\n");
        return;
    }

    fseek(pfRtpDumpSend, 0L, SEEK_END);
    iLength = ftell(pfRtpDumpSend);
    if (iLength > RTP_HEADER_OFFSET)
    {
        fseek(pfRtpDumpSend, RTP_HEADER_OFFSET, SEEK_CUR);
    }
    else
    {
        fprintf(pfTestLog, "failed: file size is smaller than RTP_HEADER_OFFSET, pls check it!!!\n");
        return;            
    }
    
    if (1 != fread(&chBuf, 1, 1, pfRtpDumpSend))// read version
    {
       fprintf(pfTestLog, "failed: read file failed, pls check it!!!\n");
        return;
    }
    if (chBuf == 0xC0)
    {
        fprintf(pfTestLog, "failed: Don't support fec in prtp, pls check it!\n");
        return;
    }
    else if(chBuf == 0x80)
    {
        if(1 != fread(&chBuf, 1, 1, pfRtpDumpSend)) // read pt
        {
           fprintf(pfTestLog, "failed: read file failed, pls check it!!!\n");
            return;
        }
        if(127 == chBuf)
        {
            fprintf(pfTestLog, "Successful!\n");
        }
        else
        {
            fprintf(pfTestLog, "failed: Codec: %s\n", g_stPtKey[chBuf].name);
            return;
        }
    }
    else
    {
        fprintf(pfTestLog, "failed: can't parse rtp vertion, pls check it!!!\n");
        return;
    }

    printInfoE("fec test end");
}

void p2p()
{
    int port;
    int iRet;
    char ipAddr[64];

    memset(ipAddr, 0, 64);
    
    printInfoB("p2p test begin");

    iRet = UGo_get_localIP(port, ipAddr);
    if (iRet != 0)
    {
        fprintf(pfTestLog, "failed: can't local ip address, pls check it!!!\n");
        return;
    }
    else
    {
        fprintf(pfTestLog, "Local ip: %s, Port: %d\n", ipAddr, port);
    }
    

    iRet = UGo_get_destIP(port, ipAddr);
    if (iRet != 0)
    {
        fprintf(pfTestLog, "failed: can't destination's ip address, pls check it!!!\n");
        return;
    }
    else
    {
        fprintf(pfTestLog, "Dest ip: %s, Port: %d\n", ipAddr, port);
    }

    fprintf(pfTestLog, "please check ip info to decide whether success or fail for p2p function!\n");

    printInfoE("p2p test end");
}

void prtp()
{
    char chBuf = 0;
    int iLength;

    printInfoB("prtp test begin");

    if (pfRtpDumpSend == NULL)
    {
        fprintf(pfTestLog, "failed: test suspend, file: %s handle is null!!!\n");
        return;        
    }

    fseek(pfRtpDumpSend, 0L, SEEK_END);
    iLength = ftell(pfRtpDumpSend);

    if (iLength > RTP_HEADER_OFFSET)
    {
        fseek(pfRtpDumpSend, 0L, SEEK_SET);
        fseek(pfRtpDumpSend, RTP_HEADER_OFFSET, SEEK_CUR);
    }
    else
    {
        fprintf(pfTestLog, "failed: file size is smaller than RTP_HEADER_OFFSET, pls check it!!!\n");
        return;        
    }

    if (1 != fread(&chBuf, 1, 1, pfRtpDumpSend))
    {
       fprintf(pfTestLog, "failed: read file failed, pls check it!!!\n");
        return;        
    }
    
    if (chBuf != 0x0c)
    {
        fprintf(pfTestLog, "failed: file version = %d, pls check it!!!\n", chBuf);
        return;        
    }

    fprintf(pfTestLog, "Successful!\n");

    printInfoE("prtp test end");    
}

void emodel()
{
	ugo_emodel_info_t emodel_value;
	ME_emodel_calc_info_t *emodel_mos = &emodel_value.emodel.emodel_mos;   /*mos value, 0~4.5*/

    printInfoB("emodel test begin");    

    memset(&emodel_value, 0, sizeof(emodel_value));
	UGo_get_emodel_value(&emodel_value);

    if (emodel_mos->max > 0)
    {
        fprintf(pfTestLog, "Successful!\n");
    }
    else
    {
        fprintf(pfTestLog, "failed: mos is zero, pls check it!!!\n");
    }

    printInfoE("emodel test end");    
}

void logInfo()
{
    FILE *pfTestRec = NULL;
    int iCount = 0;

    printInfoB("log test begin");    

#if defined ANDROID
    pfTestRec  = fopen("/mnt/sdcard/testFiles/testLog.txt", "rb");
#else
    pfTestRec  = fopen("testLog.txt", "rb");
#endif
    if ( NULL == pfTestRec )
    {
        fprintf(pfTestLog, "Failed: can't open file: %s, pls check it!!!\r\n","testLog.txt");
        return;
    }

    fseek(pfTestRec, 0L, SEEK_END);
    iCount = ftell(pfTestRec);
    fclose(pfTestRec);

    if (iCount > 0)
    {
        fprintf(pfTestLog, "Successful!\n");
    }
    else
    {
        fprintf(pfTestLog, "failed: file size is zero, pls check it!!!\n");
    }

    printInfoE("log test end");    
}

void playFile()
{
    printInfoB("Play file test begin");    

    if ( g_stAutoTestState.playCount == g_stAutoTestState.callCount )
    {
        fprintf(pfTestLog, "Successful!\n");
    }
    else
    {
        fprintf(pfTestLog, "failed: pls refer detail info above and check it!!!\n");
    }

    printInfoE("Play file test end");    
}

void recFile()
{
    int iLength;

    printInfoB("record file test begin");    

    if (NULL == pfRecFile)
    {
        fprintf(pfTestLog, "failed: test suspend, file: %s handle is null!!!\n");
        return;
    }

    fseek(pfRecFile, 0L, SEEK_END);
    iLength = ftell(pfRecFile);
    if (iLength <= 0)
    {
        fprintf(pfTestLog, "Failed: file: %s size is zero, pls check it!!!\n");
        return;
    }

    fprintf(pfTestLog, "Successful!\n");

    printInfoE("record file test end");
}

void StartTestFunctionInCall(void)
{
    if (g_stAutoTestCtrl.p2pCtrl)
    {
        p2p();
    }

    if (g_stAutoTestCtrl.emodelCtr)
    {
        emodel();
    }
}

void StartTestFunctionInCallEnd(int iFileSeq)
{
    pfRtpDumpSend = fopen(g_stAutoTestFile[iFileSeq].pcSntRTPDump, "rb");
    if (NULL == pfRtpDumpSend)
    {
        fprintf(pfTestLog, "Failed: can't open file: %s, pls check it!!!", g_stAutoTestFile[iFileSeq].pcSntRTPDump);
    }

    pfRecFile = fopen(g_stAutoTestFile[iFileSeq].pcRec, "rb");
    if (NULL == pfRecFile)
    {
        fprintf(pfTestLog, "Failed: can't open file: %s, pls check it!!!\n", g_stAutoTestFile[iFileSeq].pcRec);
    }

    fflush(pfRecFile);
    fflush(pfRtpDumpSend);

    if (g_stAutoTestCtrl.prtpCtrl)
    {
        prtp();
    }

    if (g_stAutoTestCtrl.fecCtrl)
    {
        fec();
    }

    if (g_stAutoTestCtrl.playCtrl)
    {
        playFile();
    }

    if (g_stAutoTestCtrl.recCtrl)
    {
        recFile();
    }

    if (g_stAutoTestCtrl.p862Ctrl)
    {
        CalcP862();
    }

    if (g_stAutoTestCtrl.encryptCtrl)
    {
        encrypt_rtp();
    }

    if (g_stAutoTestCtrl.encryptCtrl)
    {
        exl_playing_dev();
    }

    basicInfo();    
}

void StartTestFunctionInTestEnd(void)
{
    if (g_stAutoTestCtrl.logCtrl)
    {
        logInfo();
    }
}

void StopTestFunction()
{
    if (NULL != pfRtpDumpSend)
    {
        fclose(pfRtpDumpSend);
    }

    if (NULL != pfRecFile)
    {
        fclose(pfRecFile);
    }
}

void StartTestAssistant(void)
{
    int iRet = 0;
    media_file_play_pm_t fileplay;
    memset(&fileplay, 0, sizeof(fileplay));
    fileplay.iDirect = 1;               //play to opposition
    fileplay.iFileFormat = 1;
    fileplay.iLoop = 0;                //play one time
    
    /* mod by VintonLiu on 20141114 for auto conference test */
#if 0
    strcpy(fileplay.filepath.filename, g_stAutoTestFile[g_icallCnt].pcPlay);
#else    
    strcpy(fileplay.filepath.filename, g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcPlay);
#endif    
    /* Vinton mod end */

    iRet = UGo_play_file(kME_FILE_PATHNAME, &fileplay);
    if(!iRet)
    {
        fprintf(pfTestLog, "StartTestAssistant: play file successfully: %s!\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcPlay);
        g_stAutoTestState.callState |= eAuto_playing;
        g_stAutoTestState.playCount++;
    }
    else
    {
        fprintf(pfTestLog, "StartTestAssistant: play file failed: %s! iRet = %d\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcPlay, iRet);
    }

    media_file_record_pm_t filerecord;
    memset(&filerecord,0,sizeof(filerecord));
    filerecord.iMode= eME_RECORD_SPEAKER;
    strcpy(filerecord.filepath, g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcRec);
    iRet = UGo_record_start(&filerecord);
    if(!iRet)
    {
        fprintf(pfTestLog, "StartTestAssistant: success, record file: %s!\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcRec);
        g_stAutoTestState.callState |= eAuto_recording;
    }
    else
    {
    	fprintf(pfTestLog, "StartTestAssistant: failed, record file: %s! iRet = %d\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcRec, iRet);
    }

    iRet = UGo_start_rtpdump(g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcSntRTPDump, kRtpOutgoing);
    if(!iRet)
    {
        fprintf(pfTestLog, "StartTestAssistant: success, Dump file: %s!\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcSntRTPDump);
        g_stAutoTestState.callState |= eAuto_dumpSndRtp;    	
    }      
    else
    {
        fprintf(pfTestLog, "StartTestAssistant: failed, dump send RTP file: %s! iRet = %d\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcSntRTPDump, iRet);
    }

    iRet = UGo_start_rtpdump(g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcrcvRTPDump, kRtpIncoming);
    if(!iRet)
    {
        fprintf(pfTestLog, "StartTestAssistant: success, dump recv file: %s!\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcrcvRTPDump);
        g_stAutoTestState.callState |= eAuto_dumpRcvRtp;
    }
    else
    {
    	fprintf(pfTestLog, "StartTestAssistant: failed, dump RTP file: %s! iRet = %d\n", \
            g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcrcvRTPDump, iRet);
    }

    if (g_stAutoTestCtrl.logCtrl)
    {
    	log_trace_pm_t logtrace;
        
		logtrace.level = kME_TraceReport;
#if defined ANDROID
        sprintf(logtrace.filepath, "/mnt/sdcard/testFiles/testLog.txt");
#else
		sprintf(logtrace.filepath, ".\\testLog.txt");
#endif
		UGo_set_log_file(&logtrace);
    }
}

void StopTestAssistant(void)
{
    int iRet = 0;
    if ( g_stAutoTestState.callState & eAuto_playing )
    {
        iRet = UGo_stop_file();
        if (0 != iRet)
        {
    		fprintf(pfTestLog, "stop play file failed: %s! iRet = %d\n", \
                g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcPlay, iRet);
        }
        else
        {
            g_stAutoTestState.callState &= ~eAuto_playing;
        }
    }

    if ( g_stAutoTestState.callState & eAuto_recording )
    {
        iRet = UGo_record_stop();
        if (0 != iRet)
        {
    		fprintf(pfTestLog, "stop record file failed: %s! iRet = %d\n", \
                g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcRec, iRet);
        }
        else
        {
            g_stAutoTestState.callState &= ~eAuto_recording;
        }
    }
    if ( g_stAutoTestState.callState & eAuto_dumpSndRtp )
    {

    	if( UGo_stop_rtpdump(kRtpOutgoing) )
        {
    		fprintf(pfTestLog, "stop dump RTP file failed: %s! iRet = %d\n", \
                g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcSntRTPDump, iRet);
        }
        else
        {
            g_stAutoTestState.callState &= ~eAuto_dumpSndRtp;
        }
    }
    if ( g_stAutoTestState.callState & eAuto_dumpRcvRtp )
    {
    	if ( UGo_stop_rtpdump(kRtpIncoming) )
        {
    		fprintf(pfTestLog, "stop dump RTP file failed: %s! iRet = %d\n", \
                g_stAutoTestFile[g_stAutoTestState.testFileIdx].pcrcvRTPDump, iRet);
        }
        else
        {
            g_stAutoTestState.callState &= ~eAuto_dumpRcvRtp;
        }
    }
}

void StartTestThread(void)
{
	keepAlive = TRUE;
	ms_thread_create(&ethread_id, NULL, Auto_test_thread, NULL);
}

void StopTestThread(void)
{
	int waiting_time = TIMER_PERIOD_MS;
	int *ptime = &waiting_time;

	keepAlive = FALSE;
	ms_thread_join(ethread_id, (void**)&ptime);
	ethread_id = NULL;
}

#endif /* end of AUTO_TEST_SWITCH */


#include "iTLVPickle.h"

#include "skpbpickle.h"

#include "skmetainfofinder.h"

#include "sktlvapp.h"
using namespace Comm;


static const SKMetaField_t g_tMetaFieldListIGGMicroMsgEchoInfo [] = {
    /*Name              Offset                                                                                     Type         IsPtr         IsRequired     ArraySize       ReferTo         Id     FieldSize                           ItemSize                  Status Reserved*/
    { "EchoLen",   SK_FIELD_OFFSET(IGGMicroMsgEchoInfo_t, iEchoLen),   eTypeSKUint32, 0,      1,          -1,         "",        1,  sizeof(unsigned int), sizeof(unsigned int), 1,     0 },
    { "EchoStr",   SK_FIELD_OFFSET(IGGMicroMsgEchoInfo_t, pcEchoStr),  eTypeSKChar,   1,      0,          -1,        "EchoLen",  2,  sizeof(char *),       sizeof(char),         1,     0 }
};

////////// AUTH //////////////
static const SKMetaField_t g_tMetaFieldIGGBaseRequest [] = {
    /*Name                         Offset                                                                             Type                 IsPtr  IsRequired     ArraySize     ReferTo    Id       FieldSize                                        ItemSize               status Reserved*/
	{ "SessionKey",     SK_FIELD_OFFSET(IGGBaseRequest_t, sSessionKey),     eTypeSKChar,    0,  1,          36,         "",     1, sizeof(char[36]),            sizeof(char),           1,  0 },
	{ "Uin",            SK_FIELD_OFFSET(IGGBaseRequest_t, iUin),            eTypeSKUint32,  0,  1,          -1,         "",     2, sizeof(unsigned int),        sizeof(unsigned int),   1,  0 },
	{ "DeviceID",       SK_FIELD_OFFSET(IGGBaseRequest_t, cDeviceID),       eTypeSKUint8,   0,  1,          16,	        "",     3, sizeof(unsigned char[16]),   sizeof(unsigned char),  1,  0 },
	{ "ClientVersion",  SK_FIELD_OFFSET(IGGBaseRequest_t, iClientVersion),  eTypeSKInt32,   0,  1,          -1,         "",     4, sizeof(int),                 sizeof(int),            1,  0 },
	{ "DeviceType",     SK_FIELD_OFFSET(IGGBaseRequest_t, sDeviceType),     eTypeSKChar,    0,  1,          132,	    "",     5, sizeof(char[132]),           sizeof(char),           1,  0 },
	{ "Scene",          SK_FIELD_OFFSET(IGGBaseRequest_t, iScene),          eTypeSKUint32,  0,  0,          -1,         "",     6, sizeof(unsigned int),        sizeof(unsigned int),   1,  0 },
	{ "Seq",            SK_FIELD_OFFSET(IGGBaseRequest_t, iSeq),            eTypeSKUint32,  0,  0,          -1,         "",     7, sizeof(unsigned int),        sizeof(unsigned int),   1,  0 }
};

static const SKMetaField_t g_tMetaFieldIGGAuthRequest [] = {
    /*Name                         Offset                                                                                  Type                            IsPtr  IsRequired     ArraySize     ReferTo       Id       FieldSize                                        ItemSize                                     Status Reserved*/
	{ "BaseRequest",      SK_FIELD_OFFSET(IGGAuthRequest_t, tBaseRequest),     eTypeBaseRequest,  0,    1,          -1,       "",       1,  sizeof(IGGBaseRequest_t),        sizeof(IGGBaseRequest_t),        1, 0 },
	{ "Account",          SK_FIELD_OFFSET(IGGAuthRequest_t, tAccount),         eTypeSKBuiltinString, 0,    1,          -1,       "",       2,  sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Pwd",              SK_FIELD_OFFSET(IGGAuthRequest_t, tPwd),             eTypeSKBuiltinString, 0,    1,          -1,       "",       3,  sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "TimeZone",         SK_FIELD_OFFSET(IGGAuthRequest_t, pcTimeZone),       eTypeSKChar,          1,    0,          -1,       "strlen", 4,  sizeof(char *),                  sizeof(char),                    0, 0 },
	{ "Language",         SK_FIELD_OFFSET(IGGAuthRequest_t, pcLanguage),       eTypeSKChar,          1,    0,          -1,       "strlen", 5,  sizeof(char *),                  sizeof(char),                    0, 0 },
	{ "AuthTicket",       SK_FIELD_OFFSET(IGGAuthRequest_t, pcAuthTicket),     eTypeSKChar,          1,    0,          -1,       "strlen", 6,  sizeof(char *),                  sizeof(char),                    0, 0 },
	{ "RealCountry",      SK_FIELD_OFFSET(IGGAuthRequest_t, pcRealCountry),    eTypeSKChar,          1,    0,          -1,       "strlen", 7,  sizeof(char *),                  sizeof(char),                    0, 0 },
	{ "RandomEncryKey",   SK_FIELD_OFFSET(IGGAuthRequest_t, tRandomEncryKey),  eTypeSKBuiltinString, 0,    1,          -1,       "",       8,  sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
	{ "DevSid",           SK_FIELD_OFFSET(IGGAuthRequest_t, tDevSid),          eTypeSKBuiltinString, 0,    1,          -1,       "",       9,  sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
	{ "DevPwd",           SK_FIELD_OFFSET(IGGAuthRequest_t, tDevPwd),          eTypeSKBuiltinString, 0,    1,          -1,       "",       10, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
	{ "LoginToken",       SK_FIELD_OFFSET(IGGAuthRequest_t, tLoginToken),      eTypeSKBuiltinString, 0,    1,          -1,       "",       11, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
};

static const SKMetaField_t g_tMetaFieldIGGBaseResponse [] = {
    /*Name                         Offset                                                                  Type                            IsPtr  IsRequired     ArraySize     ReferTo    Id       FieldSize                                                ItemSize                                        Status Reserved*/
	{ "Ret",        SK_FIELD_OFFSET(IGGBaseResponse_t, iRet),       eTypeSKInt32,           0,      1,      -1,		    "",     1,  sizeof(int),                        sizeof(int),                        1, 0 },
	{ "ErrMsg",     SK_FIELD_OFFSET(IGGBaseResponse_t, tErrMsg),    eTypeSKBuiltinString,   0,      1,      -1,	        "",     2,  sizeof(Comm::SKBuiltinString_t),    sizeof(Comm::SKBuiltinString_t),    1, 0 },
	{ "Seq",        SK_FIELD_OFFSET(IGGBaseResponse_t, iSeq),       eTypeSKUint32,          0,      0,      -1,	        "",     3,  sizeof(unsigned int),               sizeof(unsigned int),               1, 0 }
};

static const SKMetaField_t g_tMetaFieldIGGBirthday [] = {
    /*Name                         Offset                                              Type              IsPtr  IsRequired     ArraySize     ReferTo    Id       FieldSize                                        ItemSize           Status Reserved*/
	{ "Year",   SK_FIELD_OFFSET(IGGBirthday_t, iYear),  eTypeSKUint32,  0,  1,          -1,	        "",     1,  sizeof(unsigned int),   sizeof(unsigned int),   0,  0 },
	{ "Month",  SK_FIELD_OFFSET(IGGBirthday_t, iMonth), eTypeSKUint32,  0,  1,          -1,	        "",     2,  sizeof(unsigned int),   sizeof(unsigned int),   0,  0 },
	{ "Day",    SK_FIELD_OFFSET(IGGBirthday_t, iDay),   eTypeSKUint32,  0,  1,          -1,	        "",     3,  sizeof(unsigned int),   sizeof(unsigned int),   0,  0 }
};

static const SKMetaField_t g_tMetaFieldIGGWebProxyInfo [] = {
    /*Name                         Offset                                                                 Type                          IsPtr  IsRequired     ArraySize     ReferTo             Id       FieldSize                                               ItemSize                                          Status Reserved*/
	{ "IPCount",    SK_FIELD_OFFSET(IGGWebProxyInfo_t, iIPCount),   eTypeSKInt32,           0,  1,          -1,		    "",         1,  sizeof(int),                        sizeof(int),                        0,  0 },
	{ "IPList",     SK_FIELD_OFFSET(IGGWebProxyInfo_t, ptIPList),   eTypeSKBuiltinString,   1,  0,          -1,		    "IPCount",  2,  sizeof(Comm::SKBuiltinString_t *),  sizeof(Comm::SKBuiltinString_t),    0,  0 },
};

static const SKMetaField_t g_tMetaFieldIGGAuthResponse [] = {
    /*Name                         Offset                                                                                  Type                        IsPtr  IsRequired     ArraySize     ReferTo       Id       FieldSize                                        ItemSize                                                     Status Reserved*/
	{ "BaseResponse",   SK_FIELD_OFFSET(IGGAuthResponse_t, tBaseResponse),  eTypeBaseResponse,   0,  1,          -1,		    "",     1,      sizeof(IGGBaseResponse_t),          sizeof(IGGBaseResponse_t),          1, 0 },
	{ "Uin",            SK_FIELD_OFFSET(IGGAuthResponse_t, iUin),           eTypeSKUint32,          0,  1,          -1,		    "",     2,      sizeof(unsigned int),               sizeof(unsigned int),               1, 0 },
	{ "UserName",       SK_FIELD_OFFSET(IGGAuthResponse_t, tUserName),      eTypeSKBuiltinString,   0,  1,          -1,		    "",     3,      sizeof(Comm::SKBuiltinString_t),    sizeof(Comm::SKBuiltinString_t),    1, 0 },
	{ "NickName",       SK_FIELD_OFFSET(IGGAuthResponse_t, tNickName),      eTypeSKBuiltinString,   0,  1,          -1,		    "",     4,      sizeof(Comm::SKBuiltinString_t),    sizeof(Comm::SKBuiltinString_t),    1, 0 },
	{ "BindEmail",      SK_FIELD_OFFSET(IGGAuthResponse_t, tBindEmail),     eTypeSKBuiltinString,   0,  1,          -1,		    "",     5,      sizeof(Comm::SKBuiltinString_t),    sizeof(Comm::SKBuiltinString_t),    1, 0 },
	{ "BindMobile",     SK_FIELD_OFFSET(IGGAuthResponse_t, tBindMobile),    eTypeSKBuiltinString,   0,  1,          -1,		    "",     6,      sizeof(Comm::SKBuiltinString_t),    sizeof(Comm::SKBuiltinString_t),    1, 0 },
	{ "Status",         SK_FIELD_OFFSET(IGGAuthResponse_t, iStatus),        eTypeSKUint32,          0,  1,          -1,		    "",     7,      sizeof(unsigned int),               sizeof(unsigned int),               1, 0 },
	{ "SessionKey",     SK_FIELD_OFFSET(IGGAuthResponse_t, sSessionKey),    eTypeSKChar,            0,  1,          36,		    "",     8,      sizeof(char[36]),                   sizeof(char),                       1, 0 },
	{ "ImgBuf",         SK_FIELD_OFFSET(IGGAuthResponse_t, tImgBuf),        eTypeSKBuiltinBuffer,   0,  1,          -1,		    "",     9,      sizeof(Comm::SKBuiltinBuffer_t),    sizeof(Comm::SKBuiltinBuffer_t),    1, 0 },
	{ "NewVersion",     SK_FIELD_OFFSET(IGGAuthResponse_t, iNewVersion),    eTypeSKUint32,          0,  0,          -1,		    "",     10,     sizeof(unsigned int),               sizeof(unsigned int),               1, 0 },
	{ "RegType",        SK_FIELD_OFFSET(IGGAuthResponse_t, iRegType),       eTypeSKUint32,          0,  0,          -1,		    "",     11,     sizeof(unsigned int),               sizeof(unsigned int),               0, 0 },
	{ "ProfileFlag",    SK_FIELD_OFFSET(IGGAuthResponse_t, iProfileFlag),   eTypeSKUint32,          0,  0,          -1,		    "",     12,     sizeof(unsigned int),               sizeof(unsigned int),               0, 0 },
	{ "UserSex",        SK_FIELD_OFFSET(IGGAuthResponse_t, iUserSex),       eTypeSKUint32,          0,  0,          -1,		    "",     13,     sizeof(unsigned int),               sizeof(unsigned int),               0, 0 },
	{ "UserStatus",     SK_FIELD_OFFSET(IGGAuthResponse_t, iUserStatus),    eTypeSKUint32,          0,  0,          -1,		    "",     14,     sizeof(unsigned int),               sizeof(unsigned int),               0, 0 },	
	{ "FirstName",      SK_FIELD_OFFSET(IGGAuthResponse_t, tFirstName),     eTypeSKBuiltinString,   0,  1,          -1,		    "",     15,     sizeof(Comm::SKBuiltinString_t),    sizeof(Comm::SKBuiltinString_t),    1, 0 },
	{ "LastName",       SK_FIELD_OFFSET(IGGAuthResponse_t, tLastName),      eTypeSKBuiltinString,   0,  1,          -1,		    "",     16,     sizeof(Comm::SKBuiltinString_t),    sizeof(Comm::SKBuiltinString_t),    1, 0 },
	{ "Birthday",       SK_FIELD_OFFSET(IGGAuthResponse_t, tBirthday),      eTypeBirthday,       0,  0,          -1,		    "",     17,     sizeof(IGGBirthday_t),              sizeof(IGGBirthday_t),              0, 0 },
	{ "WebProxyInfo",   SK_FIELD_OFFSET(IGGAuthResponse_t, tWebProxyInfo),  eTypeWebProxyInfo,   0,  0,          -1,		    "",     18,     sizeof(IGGWebProxyInfo_t),          sizeof(IGGWebProxyInfo_t),          0, 0 },
	{ "SafeUserName",   SK_FIELD_OFFSET(IGGAuthResponse_t, tSafeUserName),  eTypeSKBuiltinString,   0,  1,          -1,		    "",     19,     sizeof(Comm::SKBuiltinString_t),    sizeof(Comm::SKBuiltinString_t),    1, 0 },
	{ "ClientNumber",   SK_FIELD_OFFSET(IGGAuthResponse_t, tClientNumber),  eTypeSKBuiltinString,   0,  1,          -1,		    "",     20,     sizeof(Comm::SKBuiltinString_t),    sizeof(Comm::SKBuiltinString_t),    1, 0 },
	{ "Appid",          SK_FIELD_OFFSET(IGGAuthResponse_t, tAppid),         eTypeSKBuiltinString,   0,  1,          -1,		    "",     21,     sizeof(Comm::SKBuiltinString_t),    sizeof(Comm::SKBuiltinString_t),    1, 0 },
};

static const SKMetaField_t g_tMetaFieldIGGAuthBySKRequest [] = {
    /*Name                      Offset                                                                                      Type                        IsPtr     IsRequired       ArraySize     ReferTo      Id       FieldSize                                   ItemSize                           Status Reserved*/
	{ "BaseRequest",    SK_FIELD_OFFSET(IGGAuthBySKRequest_t, tBaseRequest),    eTypeBaseRequest,   0,      1,          -1,		    "",     1,  sizeof(IGGBaseRequest_t),   sizeof(IGGBaseRequest_t),   1,  0 },
};

static const SKMetaField_t g_tMetaFieldIGGAuthBySKResponse [] = {
    /*Name                          Offset                                                                                      Type                         IsPtr     IsRequired       ArraySize     ReferTo      Id       FieldSize                                   ItemSize                           Status Reserved*/
	{ "BaseResponse",   SK_FIELD_OFFSET(IGGAuthBySKResponse_t, tBaseResponse),  eTypeBaseResponse,      0,      1,          -1,		    "",     1,  sizeof(IGGBaseResponse_t),  sizeof(IGGBaseResponse_t),  1, 0 },
	{ "Uin",            SK_FIELD_OFFSET(IGGAuthBySKResponse_t, iUin),           eTypeSKUint32,          0,      0,          -1,		    "",     2,  sizeof(unsigned int),       sizeof(unsigned int),       1, 0 },
	{ "NewVersion",     SK_FIELD_OFFSET(IGGAuthBySKResponse_t, iNewVersion),    eTypeSKUint32,          0,      0,          -1,		    "",     3,  sizeof(unsigned int),       sizeof(unsigned int),       1, 0 },
	{ "WebProxyInfo",   SK_FIELD_OFFSET(IGGAuthBySKResponse_t, tWebProxyInfo),  eTypeWebProxyInfo,   0,      0,          -1,		    "",     4,  sizeof(IGGWebProxyInfo_t),  sizeof(IGGWebProxyInfo_t),  0, 0 },
	{ "SysTime",        SK_FIELD_OFFSET(IGGAuthBySKResponse_t, iSysTime),       eTypeSKUint32,          0,      1,          -1,		    "",     5,  sizeof(unsigned int),       sizeof(unsigned int),       1, 0 },
};

///////// VoIP /////////////////
static const SKMetaField_t g_tMetaFieldRtpp[] = {
    /*Name            Offset                                                        Type                               IsPtr     IsRequired       ArraySize     ReferTo             Id       FieldSize                                   ItemSize                           Status Reserved*/
    { "Ip",       SK_FIELD_OFFSET(Rtpp_t, cIp),          eTypeSKBuiltinString,  0,     1,          -1,        "",           1,   sizeof(SKBuiltinString_t), sizeof(SKBuiltinString_t), 1,     0 },
    { "Delay",    SK_FIELD_OFFSET(Rtpp_t, iDelay),       eTypeSKInt32,          0,     0,          -1,        "",           2,   sizeof(int),               sizeof(int),               1,     0 },
    { "Lost",     SK_FIELD_OFFSET(Rtpp_t, iLost),        eTypeSKInt32,          0,     0,          -1,        "",           3,   sizeof(int),               sizeof(int),               1,     0 }
};

static const SKMetaField_t g_tMetaFieldRtppList[] = {
    /*Name            Offset                                                             Type                                   IsPtr     IsRequired       ArraySize     ReferTo             Id       FieldSize                                   ItemSize                                   Status Reserved*/
    { "Count",    SK_FIELD_OFFSET(RtppList_t, iCount),   eTypeSKInt32,          0,     1,          -1,        "",           1,   sizeof(int),               sizeof(int),               1,     0 },
    { "List",     SK_FIELD_OFFSET(RtppList_t, ptList),   eTypeRtpp,             1,     0,          -1,        "Count",      2,   sizeof(Rtpp_t*),           sizeof(Rtpp_t),            1,     0 },

};

static const SKMetaField_t g_tMetaFieldMedia[] = {
    /*Name            Offset                                                             Type                                    IsPtr    IsRequired       ArraySize     ReferTo             Id       FieldSize                                    ItemSize                                  Status   Reserved*/
    { "Pr",       SK_FIELD_OFFSET(Media_t, iPr),         eTypeSKInt16,           0,    0,          -1,        "",           1,   sizeof(short),             sizeof(short),               1,     0 },
    { "Pt",       SK_FIELD_OFFSET(Media_t, iPt),         eTypeSKInt16,           0,    1,          -1,        "",           2,   sizeof(short),             sizeof(short),               1,     0 },
    { "Type",     SK_FIELD_OFFSET(Media_t, cType),       eTypeSKChar,            0,    0,          -1,        "",           3,   sizeof(char),              sizeof(char),                1,     0 },
    { "Port",     SK_FIELD_OFFSET(Media_t, iPort),       eTypeSKInt32,           0,    0,          -1,        "",           4,   sizeof(int),               sizeof(int),                 1,     0 }
};

static const SKMetaField_t g_tMetaFieldMediaList[] = {
    /*Name            Offset                                                             Type                                    IsPtr    IsRequired      ArraySize     ReferTo               Id      FieldSize                                    ItemSize                                    Status   Reserved*/
    { "Count",    SK_FIELD_OFFSET(MediaList_t, iCount),  eTypeSKInt32,           0,    1,          -1,       "",            1,   sizeof(int),               sizeof(int),                1,     0 },
    { "List",     SK_FIELD_OFFSET(MediaList_t, ptList),  eTypeMedia,             1,    0,          -1,       "Count",       2,   sizeof(Media_t*),           sizeof(Media_t),             1,     0 },

};              

static const SKMetaField_t g_tMetaFieldSDP[] =  {
    /*Name            Offset                                                           Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"ConIp",     SK_FIELD_OFFSET(SDP_t, cConIp),       eTypeSKBuiltinString,   0,   0,          -1,       "",          1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"Ver",       SK_FIELD_OFFSET(SDP_t, Ver),          eTypeSKBuiltinString,   0,   0,          -1,       "",          2,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"Rtpplist",  SK_FIELD_OFFSET(SDP_t, tRtpplist),    eTypeRtppList,          0,   0,          -1,       "",          3,    sizeof(RtppList_t),             sizeof(RtppList_t),        0,     0 },
    {"Medialist", SK_FIELD_OFFSET(SDP_t, tMedialist),   eTypeMediaList,         0,   0,          -1,       "",          4,    sizeof(MediaList_t),            sizeof(MediaList_t),       0,     0 },
    {"Ice",       SK_FIELD_OFFSET(SDP_t, cIce),         eTypeSKBuiltinBuffer,   0,   0,          -1,       "",          5,    sizeof(SKBuiltinBuffer_t),      sizeof(SKBuiltinBuffer_t), 1,     0 },
    {"support_v", SK_FIELD_OFFSET(SDP_t, support_v),    eTypeSKUchar,           0,   0,          -1,       "",          6,    sizeof(char),                   sizeof(char),              1,     0 },
    {"through",   SK_FIELD_OFFSET(SDP_t, through),      eTypeSKBuiltinString,   0,   0,          -1,       "",          7,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
};

static const SKMetaField_t g_tMetaFieldCallReq[] =  {
    /*Name               Offset                                                                      Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(CallReq_t, callid),       eTypeSKBuiltinString,    0,   0,          -1,       "",          1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"callsvr_sdp", SK_FIELD_OFFSET(CallReq_t, callsvr_sdp),  eTypeSDP,                0,   0,          -1,       "",          2,    sizeof(SDP_t),                  sizeof(SDP_t),             1,     0 },
    {"type",        SK_FIELD_OFFSET(CallReq_t, type),         eTypeSKInt32,            0,   0,          -1,       "",          3,    sizeof(int),                    sizeof(int),               1,     0 },
    {"fuserid",     SK_FIELD_OFFSET(CallReq_t, fuserid),      eTypeSKBuiltinString,    0,   0,          -1,       "",          4,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"fuid",        SK_FIELD_OFFSET(CallReq_t, fuid),         eTypeSKBuiltinString,    0,   0,          -1,       "",          5,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"fphone",      SK_FIELD_OFFSET(CallReq_t, fphone),       eTypeSKBuiltinString,    0,   0,          -1,       "",          6,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"fnickname",   SK_FIELD_OFFSET(CallReq_t, fnickname),    eTypeSKBuiltinString,    0,   0,          -1,       "",          7,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"touerid",     SK_FIELD_OFFSET(CallReq_t, touserid),     eTypeSKBuiltinString,    0,   0,          -1,       "",          8,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"tophone",     SK_FIELD_OFFSET(CallReq_t, tophone),      eTypeSKBuiltinString,    0,   0,          -1,       "",          9,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"ver",         SK_FIELD_OFFSET(CallReq_t, ver),          eTypeSKBuiltinString,    0,   0,          -1,       "",          10,   sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"sflag",       SK_FIELD_OFFSET(CallReq_t, sflag),        eTypeSKInt32,            0,   0,          -1,       "",          11,   sizeof(int),                    sizeof(int),               1,     0 },
    {"through",     SK_FIELD_OFFSET(CallReq_t, through),      eTypeSKBuiltinString,    0,   0,          -1,       "",          12,   sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"userdata",    SK_FIELD_OFFSET(CallReq_t, userdata),     eTypeSKBuiltinString,    0,   0,          -1,       "",          13,   sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"meeting",     SK_FIELD_OFFSET(CallReq_t, meeting),      eTypeSKChar,             0,   0,          -1,       "",          14,   sizeof(char),                   sizeof(char),              1,     0 },
    {"fpv",         SK_FIELD_OFFSET(CallReq_t, fpv),          eTypeSKChar,             0,   0,          -1,       "",          15,   sizeof(char),                   sizeof(char),              1,     0 },
    {"fconntype",   SK_FIELD_OFFSET(CallReq_t, fconntype),    eTypeSKChar,             0,   0,          -1,       "",          16,   sizeof(char),                   sizeof(char),              1,     0 },
};

static const SKMetaField_t g_tMetaFieldCallRsp[] =  {
    /*Name               Offset                                                                      Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(CallRsp_t, callid),       eTypeSKBuiltinString,    0,   0,          -1,       "",          1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"callsvr_sdp", SK_FIELD_OFFSET(CallRsp_t, callsvr_sdp),  eTypeSDP,                0,   0,          -1,       "",          2,    sizeof(SDP_t),                  sizeof(SDP_t),             1,     0 },
    {"errcode",     SK_FIELD_OFFSET(CallRsp_t, errcode),      eTypeSKInt32,            0,   0,          -1,       "",          3,    sizeof(int),                    sizeof(int),               1,     0 },
    {"ver",         SK_FIELD_OFFSET(CallRsp_t, ver),          eTypeSKBuiltinString,    0,   0,          -1,       "",          4,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"sflag",       SK_FIELD_OFFSET(CallRsp_t, sflag),        eTypeSKInt32,            0,   0,          -1,       "",          5,    sizeof(int),                    sizeof(int),               1,     0 },
    {"through",     SK_FIELD_OFFSET(CallRsp_t, through),      eTypeSKBuiltinString,    0,   0,          -1,       "",          6,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
};

static const SKMetaField_t g_tMetaFieldCallAck[] =  {
    /*Name                Offset                                                                      Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(CallAck_t, callid),       eTypeSKBuiltinString,    0,    0,          -1,       "",         1,     sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"callsvr_sdp", SK_FIELD_OFFSET(CallAck_t, callsvr_sdp),  eTypeSDP,                0,    0,          -1,       "",         2,     sizeof(SDP_t),                  sizeof(SDP_t),             1,     0 },
    {"ice_mode",    SK_FIELD_OFFSET(CallAck_t, ice_mode),     eTypeSKInt32,            0,    0,          -1,       "",         3,     sizeof(int),                    sizeof(int), 1,     0 },
    {"ice_type",    SK_FIELD_OFFSET(CallAck_t, ice_type),     eTypeSKInt32,            0,    0,          -1,       "",         4,     sizeof(int),                    sizeof(int), 1,     0 },
};

static const SKMetaField_t g_tMetaFieldCallRing[] =  {
    /*Name               Offset                                                                      Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(CallRing_t, callid),       eTypeSKBuiltinString,    0,   0,          -1,       "",          1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"callsvr_sdp", SK_FIELD_OFFSET(CallRing_t, callsvr_sdp),  eTypeSDP,                0,   0,          -1,       "",          2,    sizeof(SDP_t),                  sizeof(SDP_t),             1,     0 },
    {"through",     SK_FIELD_OFFSET(CallRing_t, through),      eTypeSKBuiltinString,    0,   0,          -1,       "",          3,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"sflag",       SK_FIELD_OFFSET(CallRing_t, sflag),        eTypeSKInt32,            0,   0,          -1,       "",          4,    sizeof(int),                    sizeof(int),               1,     0 },
    
};

static const SKMetaField_t g_tMetaFieldByeReq[] =  {
    /*Name               Offset                                                                      Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(ByeReq_t, callid),       eTypeSKBuiltinString,    0,   0,          -1,       "",          1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"errcode",     SK_FIELD_OFFSET(ByeReq_t, errcode),      eTypeSKInt32,            0,   0,          -1,       "",          2,    sizeof(int),                    sizeof(int),               1,     0 },
    {"through",     SK_FIELD_OFFSET(ByeReq_t, through),      eTypeSKBuiltinString,    0,   0,          -1,       "",          3,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
};

static const SKMetaField_t g_tMetaFieldCallUpdate[] =  {
    /*Name               Offset                                                                          Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(CallUpdate_t, callid),       eTypeSKBuiltinString,    0,   0,          -1,       "",          1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"mip",         SK_FIELD_OFFSET(CallUpdate_t, mip),          eTypeSKBuiltinString,    0,   0,          -1,       "",          2,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"aport",       SK_FIELD_OFFSET(CallUpdate_t, aport),        eTypeSKInt32,            0,   0,          -1,       "",          3,    sizeof(int),                    sizeof(int),               1,     0 },
    {"vport",       SK_FIELD_OFFSET(CallUpdate_t, vport),        eTypeSKInt32,            0,   0,          -1,       "",          4,    sizeof(int),                    sizeof(int),               1,     0 },
    {"apt",         SK_FIELD_OFFSET(CallUpdate_t, apt),          eTypeSKBuiltinString,    0,   0,          -1,       "",          5,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"vpt",         SK_FIELD_OFFSET(CallUpdate_t, vpt),          eTypeSKBuiltinString,    0,   0,          -1,       "",          6,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"through",     SK_FIELD_OFFSET(CallUpdate_t, through),      eTypeSKBuiltinString,    0,   0,          -1,       "",          7,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
};

static const SKMetaField_t g_tMetaFieldCallUpdateRsp[] =  {
    /*Name               Offset                                                                          Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(CallUpdateRsp_t, callid),   eTypeSKBuiltinString,    0,   0,          -1,       "",          1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"errcode",     SK_FIELD_OFFSET(CallUpdateRsp_t, errcode),  eTypeSKInt32,            0,   0,          -1,       "",          2,    sizeof(int),                    sizeof(int),               1,     0 },
    {"ice_mode",    SK_FIELD_OFFSET(CallUpdateRsp_t, ice_mode), eTypeSKInt32,            0,   0,          -1,       "",          3,    sizeof(int),                    sizeof(int), 1,     0 },
    {"through",     SK_FIELD_OFFSET(CallUpdateRsp_t, through),  eTypeSKBuiltinString,    0,   0,          -1,       "",          4,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"apt",         SK_FIELD_OFFSET(CallUpdateRsp_t, apt),      eTypeSKBuiltinString,    0,   0,          -1,       "",          5,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"vpt",         SK_FIELD_OFFSET(CallUpdateRsp_t, vpt),      eTypeSKBuiltinString,    0,   0,          -1,       "",          6,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
};

static const SKMetaField_t g_tMetaFieldDirectNfy[] =  {
    /*Name               Offset                                                                         Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(DirectNfy_t, callid),       eTypeSKBuiltinString,    0,   0,          -1,       "",          1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"errcode",     SK_FIELD_OFFSET(DirectNfy_t, errcode),      eTypeSKInt32,            0,   0,          -1,       "",          2,    sizeof(int),                    sizeof(int),               1,     0 },
    {"through",     SK_FIELD_OFFSET(DirectNfy_t, through),      eTypeSKBuiltinString,    0,   0,          -1,       "",          3,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
};

static const SKMetaField_t g_tMetaFieldRouteUpdate[] =  {
    /*Name               Offset                                                                         Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(RouteUpdate_t, callid),       eTypeSKBuiltinString,    0,   0,          -1,       "",        1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"through",     SK_FIELD_OFFSET(RouteUpdate_t, through),      eTypeSKBuiltinString,    0,   0,          -1,       "",        2,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"userid",      SK_FIELD_OFFSET(RouteUpdate_t, userid),       eTypeSKBuiltinString,    0,   0,          -1,       "",        3,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },    
};

static const SKMetaField_t g_tMetaFieldDtmfInfo[] =  {
    /*Name               Offset                                                                         Type                                     IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(DtmfInfo_t, callid),       eTypeSKBuiltinString,    0,    0,          -1,       "",         1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"dtmf",        SK_FIELD_OFFSET(DtmfInfo_t, dtmf),         eTypeSKBuiltinString,    0,    0,          -1,       "",         2,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
};

/* add by Vinton.Liu on 2016-04-05 for add internal state passthrough signalling begin */
static const SKMetaField_t g_tMetaFieldInTransReq[] =  {
    /*Name               Offset                                                                         Type                           IsPtr   IsRequired      ArraySize   ReferTo       Id        FieldSize                                            ItemSize                               Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(InTransReq_t, callid),       eTypeSKBuiltinString,    0,   0,          -1,       "",        1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"through",     SK_FIELD_OFFSET(InTransReq_t, through),      eTypeSKBuiltinString,    0,   0,          -1,       "",        2,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
};

static const SKMetaField_t g_tMetaFieldInTransRsp[] =  {
    /*Name               Offset                                                                         Type                         IsPtr   IsRequired      ArraySize   ReferTo            Id        FieldSize                                            ItemSize                                   Status Reserved*/
    {"callid",      SK_FIELD_OFFSET(InTransRsp_t, callid),       eTypeSKBuiltinString,    0,   0,          -1,       "",          1,    sizeof(SKBuiltinString_t),      sizeof(SKBuiltinString_t), 1,     0 },
    {"errcode",     SK_FIELD_OFFSET(InTransRsp_t, errcode),      eTypeSKInt32,            0,   0,          -1,       "",          2,    sizeof(int),                    sizeof(int),               1,     0 },
};
/* Vinton.Liu add end */

static const SKMetaFunc_t g_tMetaSipexFuncList [] = {
	{ "SKGetSelfSvnInfo", eFuncIGGMicroMsgSKGetSelfSvnInfo,
		{ "", 0, 0 }, { "Response", eTypeSKBuiltinString, 1 } },
	{ "SKGetMetainfoFull", eFuncIGGMicroMsgSKGetMetainfoFull,
		{ "", 0, 0 }, { "Response", eTypeSKBuiltinString, 1 } },
	{ "SKGetMetainfo", eFuncIGGMicroMsgSKGetMetainfo,
		{ "", 0, 0 }, { "Response", eTypeSKBuiltinString, 1 } },
	{ "SKEcho", eFuncIGGMicroMsgSKEcho,
		{ "Request", eTypeIGGMicroMsgEchoInfo, 1 }, { "Response", eTypeIGGMicroMsgEchoInfo, 1 } }
};


static const SKMetaStruct_t g_tMetaSipexStructList [] = {
     /* Id                                             Name                          Size                                                  FieldCount                                                                                 FieldList                 */
    {eTypeIGGMicroMsgEchoInfo, "MicroMsgEchoInfo", sizeof(IGGMicroMsgEchoInfo_t), SK_ARRAY_SIZE(g_tMetaFieldListIGGMicroMsgEchoInfo), g_tMetaFieldListIGGMicroMsgEchoInfo },
    {eTypeBaseRequest,         "BaseRequest",      sizeof(IGGBaseRequest_t),  	  SK_ARRAY_SIZE(g_tMetaFieldIGGBaseRequest),          g_tMetaFieldIGGBaseRequest },
    {eTypeBaseResponse,        "BaseResponse",     sizeof(IGGBaseResponse_t),	  SK_ARRAY_SIZE(g_tMetaFieldIGGBaseResponse),         g_tMetaFieldIGGBaseResponse },
    {eTypeWebProxyInfo,        "WebProxyInfo",     sizeof(IGGWebProxyInfo_t),     SK_ARRAY_SIZE(g_tMetaFieldIGGWebProxyInfo),         g_tMetaFieldIGGWebProxyInfo },
    {eTypeBirthday,            "Birthday",         sizeof(IGGBirthday_t),		  SK_ARRAY_SIZE(g_tMetaFieldIGGBirthday),             g_tMetaFieldIGGBirthday },
    {eTypeAuthRequest,         "AuthRequest",      sizeof(IGGAuthRequest_t),	  SK_ARRAY_SIZE(g_tMetaFieldIGGAuthRequest),          g_tMetaFieldIGGAuthRequest },
    {eTypeAuthResponse,        "AuthResponse",     sizeof(IGGAuthResponse_t),	  SK_ARRAY_SIZE(g_tMetaFieldIGGAuthResponse),         g_tMetaFieldIGGAuthResponse },
    {eTypeAuthBySKRequest,     "AuthBySKRequest",  sizeof(IGGAuthBySKRequest_t),  SK_ARRAY_SIZE(g_tMetaFieldIGGAuthBySKRequest),      g_tMetaFieldIGGAuthBySKRequest },
    {eTypeAuthBySKResponse,    "AuthBySKResponse", sizeof(IGGAuthBySKResponse_t), SK_ARRAY_SIZE(g_tMetaFieldIGGAuthBySKResponse),     g_tMetaFieldIGGAuthBySKResponse },
    {eTypeRtpp,                "Rtpp",             sizeof(Rtpp_t),                SK_ARRAY_SIZE(g_tMetaFieldRtpp),                    g_tMetaFieldRtpp },
    {eTypeRtppList,			   "RtppList",		   sizeof(RtppList_t),			  SK_ARRAY_SIZE(g_tMetaFieldRtppList),				  g_tMetaFieldRtppList },
    {eTypeMedia,			   "Media",		       sizeof(Media_t),			      SK_ARRAY_SIZE(g_tMetaFieldMedia),				      g_tMetaFieldMedia },
    {eTypeMediaList,		   "MediaList",		   sizeof(MediaList_t),			  SK_ARRAY_SIZE(g_tMetaFieldMediaList),				  g_tMetaFieldMediaList },
    {eTypeSDP,			       "SDP",		       sizeof(SDP_t),			      SK_ARRAY_SIZE(g_tMetaFieldSDP),				      g_tMetaFieldSDP },
    {eTypeCallReq,			   "CallReq",		   sizeof(CallReq_t),			  SK_ARRAY_SIZE(g_tMetaFieldCallReq),				  g_tMetaFieldCallReq },
    {eTypeCallRsp,			   "CallRsp",		   sizeof(CallRsp_t),			  SK_ARRAY_SIZE(g_tMetaFieldCallRsp),				  g_tMetaFieldCallRsp },
    {eTypeCallAck,			   "CallAck",		   sizeof(CallAck_t),			  SK_ARRAY_SIZE(g_tMetaFieldCallAck),				  g_tMetaFieldCallAck },
    {eTypeCallRing,			   "CallRing",		   sizeof(CallRing_t),			  SK_ARRAY_SIZE(g_tMetaFieldCallRing),				  g_tMetaFieldCallRing },
    {eTypeByeReq,			   "ByeReq",		   sizeof(ByeReq_t),			  SK_ARRAY_SIZE(g_tMetaFieldByeReq),				  g_tMetaFieldByeReq },
    {eTypeUpdate,			   "Update",		   sizeof(CallUpdate_t),		  SK_ARRAY_SIZE(g_tMetaFieldCallUpdate),			  g_tMetaFieldCallUpdate },
    {eTypeUpdateRsp,		   "UpdateRsp",		   sizeof(CallUpdateRsp_t),	      SK_ARRAY_SIZE(g_tMetaFieldCallUpdateRsp),			  g_tMetaFieldCallUpdateRsp },
    {eTypeDirectNfy,		   "DirectNfy",		   sizeof(DirectNfy_t),		      SK_ARRAY_SIZE(g_tMetaFieldDirectNfy),				  g_tMetaFieldDirectNfy },
    {eTypeRouteUpdate,		   "RouteUpdate",	   sizeof(RouteUpdate_t),		  SK_ARRAY_SIZE(g_tMetaFieldRouteUpdate),			  g_tMetaFieldRouteUpdate },
    {eTypeDtmfInfo,			   "DtmfInfo",		   sizeof(DtmfInfo_t),			  SK_ARRAY_SIZE(g_tMetaFieldDtmfInfo),				  g_tMetaFieldDtmfInfo },
    /* add by Vinton.Liu on 2016-04-05 for add internal state passthrough signalling begin */
    {eTypeInTransReq,		   "InTransReq",	   sizeof(InTransReq_t),		  SK_ARRAY_SIZE(g_tMetaFieldInTransReq),			  g_tMetaFieldInTransReq },
    {eTypeInTransRsp,		   "InTransRsp",	   sizeof(InTransRsp_t),		  SK_ARRAY_SIZE(g_tMetaFieldInTransRsp),			  g_tMetaFieldInTransRsp },
    /* Vinton.Liu add end */
    g_ptSKBuiltinMetaInfo->ptStructList[0],

    g_ptSKBuiltinMetaInfo->ptStructList[1],

    g_ptSKBuiltinMetaInfo->ptStructList[2],

    g_ptSKBuiltinMetaInfo->ptStructList[3],

    g_ptSKBuiltinMetaInfo->ptStructList[4],

    g_ptSKBuiltinMetaInfo->ptStructList[5],

    g_ptSKBuiltinMetaInfo->ptStructList[6],

    g_ptSKBuiltinMetaInfo->ptStructList[7],

    g_ptSKBuiltinMetaInfo->ptStructList[8],

    g_ptSKBuiltinMetaInfo->ptStructList[9],

    g_ptSKBuiltinMetaInfo->ptStructList[10],

    g_ptSKBuiltinMetaInfo->ptStructList[11],

    g_ptSKBuiltinMetaInfo->ptStructList[12],

    g_ptSKBuiltinMetaInfo->ptStructList[13],

    g_ptSKBuiltinMetaInfo->ptStructList[14],
 
};



const SKMetaInfo_t g_tMetaSipexMsg = {
	"Sipex", "SipexMsg", 10000,
	SK_ARRAY_SIZE(g_tMetaSipexStructList), g_tMetaSipexStructList,
	SK_ARRAY_SIZE(g_tMetaSipexFuncList), g_tMetaSipexFuncList,
};


#include "iTLVPickle.h"

#include "skpbpickle.h"

#include "skmetainfofinder.h"

#include "IMmsgstruct.h"

using namespace Comm;

#if 1
static const SKMetaField_t g_tMetaFieldListIMMsgEchoInfo [] = {
	{ "EchoLen", static_cast<short>SK_FIELD_OFFSET(IMMsgEchoInfo_t, iEchoLen), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "EchoStr", static_cast<short>SK_FIELD_OFFSET(IMMsgEchoInfo_t, pcEchoStr), eTypeSKChar, 1, 0, -1,
		"EchoLen", 2, sizeof(char *), sizeof(char), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMBaseRequest [] = {
	{ "SessionKey", static_cast<short>SK_FIELD_OFFSET(IMBaseRequest_t, sSessionKey), eTypeSKChar, 0, 1, 36,
		"", 1, sizeof(char[36]), sizeof(char), 1, 0 },
	{ "Uin", static_cast<short>SK_FIELD_OFFSET(IMBaseRequest_t, iUin), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "DeviceID", static_cast<short>SK_FIELD_OFFSET(IMBaseRequest_t, cDeviceID), eTypeSKUint8, 0, 1, 16,
		"", 3, sizeof(unsigned char[16]), sizeof(unsigned char), 1, 0 },
	{ "ClientVersion", static_cast<short>SK_FIELD_OFFSET(IMBaseRequest_t, iClientVersion), eTypeSKInt32, 0, 1, -1,
		"", 4, sizeof(int), sizeof(int), 1, 0 },
	{ "DeviceType", static_cast<short>SK_FIELD_OFFSET(IMBaseRequest_t, sDeviceType), eTypeSKChar, 0, 1, 132,
		"", 5, sizeof(char[132]), sizeof(char), 1, 0 },
	{ "Scene", static_cast<short>SK_FIELD_OFFSET(IMBaseRequest_t, iScene), eTypeSKUint32, 0, 0, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Seq", static_cast<short>SK_FIELD_OFFSET(IMBaseRequest_t, iSeq), eTypeSKUint32, 0, 0, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMBuiltinIP [] = {
	{ "type", static_cast<short>SK_FIELD_OFFSET(IMBuiltinIP_t, iType), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "port", static_cast<short>SK_FIELD_OFFSET(IMBuiltinIP_t, iPort), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "IP", static_cast<short>SK_FIELD_OFFSET(IMBuiltinIP_t, sIP), eTypeSKChar, 0, 1, 128,
		"", 3, sizeof(char[128]), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMBuiltinIPList [] = {
	{ "LongConnectIPCount", static_cast<short>SK_FIELD_OFFSET(IMBuiltinIPList_t, iLongConnectIPCount), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ShortConnectIPCount", static_cast<short>SK_FIELD_OFFSET(IMBuiltinIPList_t, iShortConnectIPCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Seq", static_cast<short>SK_FIELD_OFFSET(IMBuiltinIPList_t, iSeq), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "LongConnectIPList", static_cast<short>SK_FIELD_OFFSET(IMBuiltinIPList_t, ptLongConnectIPList), eTypeIMBuiltinIP, 1, 0, -1,
		"LongConnectIPCount", 3, sizeof(IMBuiltinIP_t *), sizeof(IMBuiltinIP_t), 0, 0 },
	{ "ShortConnectIPList", static_cast<short>SK_FIELD_OFFSET(IMBuiltinIPList_t, ptShortConnectIPList), eTypeIMBuiltinIP, 1, 0, -1,
		"ShortConnectIPCount", 4, sizeof(IMBuiltinIP_t *), sizeof(IMBuiltinIP_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMCDNDnsInfo [] = {
	{ "Ver", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, iVer), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Uin", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, iUin), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ExpireTime", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, iExpireTime), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "FrontID", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, iFrontID), eTypeSKInt32, 0, 1, -1,
		"", 4, sizeof(int), sizeof(int), 0, 0 },
	{ "FrontIPCount", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, iFrontIPCount), eTypeSKInt32, 0, 1, -1,
		"", 5, sizeof(int), sizeof(int), 0, 0 },
	{ "FrontIPList", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, ptFrontIPList), eTypeSKBuiltinString, 1, 0, -1,
		"FrontIPCount", 6, sizeof(Comm::SKBuiltinString_t *), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "ZoneDomain", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, pcZoneDomain), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 0, 0 },
	{ "AuthKey", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, tAuthKey), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "ZoneID", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, iZoneID), eTypeSKInt32, 0, 1, -1,
		"", 9, sizeof(int), sizeof(int), 0, 0 },
	{ "ZoneIPCount", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, iZoneIPCount), eTypeSKInt32, 0, 1, -1,
		"", 10, sizeof(int), sizeof(int), 0, 0 },
	{ "ZoneIPList", static_cast<short>SK_FIELD_OFFSET(IMCDNDnsInfo_t, ptZoneIPList), eTypeSKBuiltinString, 1, 0, -1,
		"ZoneIPCount", 11, sizeof(Comm::SKBuiltinString_t *), sizeof(Comm::SKBuiltinString_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMWebProxyInfo [] = {
	{ "IPCount", static_cast<short>SK_FIELD_OFFSET(IMWebProxyInfo_t, iIPCount), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 0, 0 },
	{ "IPList", static_cast<short>SK_FIELD_OFFSET(IMWebProxyInfo_t, ptIPList), eTypeSKBuiltinString, 1, 0, -1,
		"IPCount", 2, sizeof(Comm::SKBuiltinString_t *), sizeof(Comm::SKBuiltinString_t), 0, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMAuthRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "Account", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, tAccount), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Pwd", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, tPwd), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "TimeZone", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, pcTimeZone), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 },
	{ "Language", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, pcLanguage), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "AuthTicket", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, pcAuthTicket), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 0, 0 },
	{ "RealCountry", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, pcRealCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 0, 0 },
	{ "RandomEncryKey", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, tRandomEncryKey), eTypeSKBuiltinString, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
	{ "DevSid", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, tDevSid), eTypeSKBuiltinString, 0, 1, -1,
		"", 9, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
	{ "DevPwd", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, tDevPwd), eTypeSKBuiltinString, 0, 1, -1,
		"", 10, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
	{ "LoginToken", static_cast<short>SK_FIELD_OFFSET(IMAuthRequest_t, tLoginToken), eTypeSKBuiltinString, 0, 1, -1,
		"", 11, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
};

static const SKMetaField_t g_tMetaFieldListIMRegRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMRegRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "Account", static_cast<short>SK_FIELD_OFFSET(IMRegRequest_t, tAccount), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Pwd", static_cast<short>SK_FIELD_OFFSET(IMRegRequest_t, tPwd), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Ticket", static_cast<short>SK_FIELD_OFFSET(IMRegRequest_t, tTicket), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "TimeZone", static_cast<short>SK_FIELD_OFFSET(IMRegRequest_t, pcTimeZone), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "Language", static_cast<short>SK_FIELD_OFFSET(IMRegRequest_t, pcLanguage), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMRegRequest_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 0, 0 },
	{ "RandomEncryKey", static_cast<short>SK_FIELD_OFFSET(IMRegRequest_t, tRandomEncryKey), eTypeSKBuiltinString, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
};

static const SKMetaField_t g_tMetaFieldListIMBaseResponse [] = {
	{ "Ret", static_cast<short>SK_FIELD_OFFSET(IMBaseResponse_t, iRet), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 1, 0 },
	{ "ErrMsg", static_cast<short>SK_FIELD_OFFSET(IMBaseResponse_t, tErrMsg), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Seq", static_cast<short>SK_FIELD_OFFSET(IMBaseResponse_t, iSeq), eTypeSKUint32, 0, 0, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMNetworkControl [] = {
	{ "PortList", static_cast<short>SK_FIELD_OFFSET(IMNetworkControl_t, pcPortList), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 0, 0 },
	{ "TimeoutList", static_cast<short>SK_FIELD_OFFSET(IMNetworkControl_t, pcTimeoutList), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMPluginKey [] = {
	{ "Key", static_cast<short>SK_FIELD_OFFSET(IMPluginKey_t, pcKey), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 0, 0 },
	{ "Id", static_cast<short>SK_FIELD_OFFSET(IMPluginKey_t, iId), eTypeSKUint32, 0, 0, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMPluginKeyList [] = {
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMPluginKeyList_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "List", static_cast<short>SK_FIELD_OFFSET(IMPluginKeyList_t, ptList), eTypeIMPluginKey, 1, 0, -1,
		"Count", 2, sizeof(IMPluginKey_t *), sizeof(IMPluginKey_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMHost [] = {
	{ "Origin", static_cast<short>SK_FIELD_OFFSET(IMHost_t, pcOrigin), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 0, 0 },
	{ "Substitute", static_cast<short>SK_FIELD_OFFSET(IMHost_t, pcSubstitute), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMHostList [] = {
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMHostList_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "List", static_cast<short>SK_FIELD_OFFSET(IMHostList_t, ptList), eTypeIMHost, 1, 0, -1,
		"Count", 2, sizeof(IMHost_t *), sizeof(IMHost_t), 0, 0 }
};
static const SKMetaField_t g_tMetaFieldListIMSendLocationRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, pcFromUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, pcToUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "pcContent", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, pcContent), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 1, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "DataLen", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, iDataLen), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Data", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, tData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 9, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "MsgSource", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, pcMsgSource), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 1, 0 },
	{ "MD5", static_cast<short>SK_FIELD_OFFSET(IMSendLocationRequest_t, pcMD5), eTypeSKChar, 1, 0, -1,
		"strlen", 11, sizeof(char *), sizeof(char), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMAuthResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "Uin", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, iUin), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tNickName), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "BindEmail", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tBindEmail), eTypeSKBuiltinString, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "BindMobile", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tBindMobile), eTypeSKBuiltinString, 0, 1, -1,
		"", 6, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Status", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, iStatus), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "SessionKey", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, sSessionKey), eTypeSKChar, 0, 1, 36,
		"", 8, sizeof(char[36]), sizeof(char), 1, 0 },
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tImgBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 9, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "NewVersion", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, iNewVersion), eTypeSKUint32, 0, 0, -1,
		"", 10, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "RegType", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, iRegType), eTypeSKUint32, 0, 0, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ProfileFlag", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, iProfileFlag), eTypeSKUint32, 0, 0, -1,
		"", 12, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserSex", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, iUserSex), eTypeSKUint32, 0, 0, -1,
		"", 13, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserStatus", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, iUserStatus), eTypeSKUint32, 0, 0, -1,
		"", 14, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },	
	{ "FirstName", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tFirstName), eTypeSKBuiltinString, 0, 1, -1,
		"", 15, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "LastName", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tLastName), eTypeSKBuiltinString, 0, 1, -1,
		"", 16, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Birthday", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tBirthday), eTypeIMBirthday, 0, 0, -1,
		"", 17, sizeof(IMBirthday_t), sizeof(IMBirthday_t), 0, 0 },
	{ "WebProxyInfo", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tWebProxyInfo), eTypeIMWebProxyInfo, 0, 0, -1,
		"", 18, sizeof(IMWebProxyInfo_t), sizeof(IMWebProxyInfo_t), 0, 0 },
	{ "SafeUserName", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tSafeUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 19, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "ClientNumber", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tClientNumber), eTypeSKBuiltinString, 0, 1, -1,
		"", 20, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Appid", static_cast<short>SK_FIELD_OFFSET(IMAuthResponse_t, tAppid), eTypeSKBuiltinString, 0, 1, -1,
		"", 21, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },

	
};

static const SKMetaField_t g_tMetaFieldListIMRegResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMRegResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "Uin", static_cast<short>SK_FIELD_OFFSET(IMRegResponse_t, iUin), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "SessionKey", static_cast<short>SK_FIELD_OFFSET(IMRegResponse_t, sSessionKey), eTypeSKChar, 0, 1, 36,
		"", 3, sizeof(char[36]), sizeof(char), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMRegResponse_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMUserInfoSetRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMUserInfoSetRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "FirstName", static_cast<short>SK_FIELD_OFFSET(IMUserInfoSetRequest_t, tFirstName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "LastName", static_cast<short>SK_FIELD_OFFSET(IMUserInfoSetRequest_t, tLastName), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMUserInfoSetRequest_t, tImgBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "UserSex", static_cast<short>SK_FIELD_OFFSET(IMUserInfoSetRequest_t, iUserSex), eTypeSKUint32, 0, 0, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Birthday", static_cast<short>SK_FIELD_OFFSET(IMUserInfoSetRequest_t, tBirthday), eTypeIMBirthday, 0, 0, -1,
		"", 6, sizeof(IMBirthday_t), sizeof(IMBirthday_t), 0, 0 },	
};

static const SKMetaField_t g_tMetaFieldListIMUserInfoSetResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMUserInfoSetResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMCmdItem [] = {
	{ "CmdId", static_cast<short>SK_FIELD_OFFSET(IMCmdItem_t, iCmdId), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 1, 0 },
	{ "CmdBuf", static_cast<short>SK_FIELD_OFFSET(IMCmdItem_t, tCmdBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMNewInitRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMNewInitRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMNewInitRequest_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "CurrentSynckey", static_cast<short>SK_FIELD_OFFSET(IMNewInitRequest_t, tCurrentSynckey), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "MaxSynckey", static_cast<short>SK_FIELD_OFFSET(IMNewInitRequest_t, tMaxSynckey), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "Language", static_cast<short>SK_FIELD_OFFSET(IMNewInitRequest_t, pcLanguage), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMNewInitResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMNewInitResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "CurrentSynckey", static_cast<short>SK_FIELD_OFFSET(IMNewInitResponse_t, tCurrentSynckey), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "MaxSynckey", static_cast<short>SK_FIELD_OFFSET(IMNewInitResponse_t, tMaxSynckey), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "ContinueFlag", static_cast<short>SK_FIELD_OFFSET(IMNewInitResponse_t, iContinueFlag), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "SelectBitmap", static_cast<short>SK_FIELD_OFFSET(IMNewInitResponse_t, iSelectBitmap), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "CmdCount", static_cast<short>SK_FIELD_OFFSET(IMNewInitResponse_t, iCmdCount), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "CmdList", static_cast<short>SK_FIELD_OFFSET(IMNewInitResponse_t, ptCmdList), eTypeIMCmdItem, 1, 0, -1,
		"CmdCount", 7, sizeof(IMCmdItem_t *), sizeof(IMCmdItem_t), 0, 0 },
	{ "Ratio", static_cast<short>SK_FIELD_OFFSET(IMNewInitResponse_t, iRatio), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSnsUserInfo [] = {
	{ "SnsFlag", static_cast<short>SK_FIELD_OFFSET(IMSnsUserInfo_t, iSnsFlag), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "SnsBGImgID", static_cast<short>SK_FIELD_OFFSET(IMSnsUserInfo_t, pcSnsBGImgID), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "SnsBGObjectID", static_cast<short>SK_FIELD_OFFSET(IMSnsUserInfo_t, llSnsBGObjectID), eTypeSKUint64, 0, 0, -1,
		"", 3, sizeof(unsigned long long), sizeof(unsigned long long), 0, 0 },
	{ "SnsFlagEx", static_cast<short>SK_FIELD_OFFSET(IMSnsUserInfo_t, iSnsFlagEx), eTypeSKUint32, 0, 0, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMCustomizedInfo [] = {
	{ "BrandFlag", static_cast<short>SK_FIELD_OFFSET(IMCustomizedInfo_t, iBrandFlag), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ExternalInfo", static_cast<short>SK_FIELD_OFFSET(IMCustomizedInfo_t, pcExternalInfo), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "BrandInfo", static_cast<short>SK_FIELD_OFFSET(IMCustomizedInfo_t, pcBrandInfo), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 },
	{ "BrandIconURL", static_cast<short>SK_FIELD_OFFSET(IMCustomizedInfo_t, pcBrandIconURL), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSearchContactRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMSearchContactRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMSearchContactRequest_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSearchContactItem [] = {
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, tNickName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "PYInitial", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, tPYInitial), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "QuanPin", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, tQuanPin), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, iSex), eTypeSKInt32, 0, 1, -1,
		"", 5, sizeof(int), sizeof(int), 1, 0 },
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, tImgBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 6, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "Province", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcProvince), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 0, 0 },
	{ "City", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcCity), eTypeSKChar, 1, 0, -1,
		"strlen", 8, sizeof(char *), sizeof(char), 0, 0 },
	{ "Signature", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcSignature), eTypeSKChar, 1, 0, -1,
		"strlen", 9, sizeof(char *), sizeof(char), 0, 0 },
	{ "PersonalCard", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, iPersonalCard), eTypeSKUint32, 0, 0, -1,
		"", 10, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyFlag", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, iVerifyFlag), eTypeSKUint32, 0, 0, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyInfo", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcVerifyInfo), eTypeSKChar, 1, 0, -1,
		"strlen", 12, sizeof(char *), sizeof(char), 0, 0 },
	{ "NeedVerify", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, iNeedVerify), eTypeSKInt32, 0, 1, -1,
		"", 13, sizeof(int), sizeof(int), 1, 0 },
	{ "Weibo", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcWeibo), eTypeSKChar, 1, 0, -1,
		"strlen", 14, sizeof(char *), sizeof(char), 0, 0 },
	{ "Alias", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcAlias), eTypeSKChar, 1, 0, -1,
		"strlen", 15, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboNickname", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcWeiboNickname), eTypeSKChar, 1, 0, -1,
		"strlen", 16, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboFlag", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, iWeiboFlag), eTypeSKUint32, 0, 0, -1,
		"", 17, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "AlbumStyle", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, iAlbumStyle), eTypeSKInt32, 0, 0, -1,
		"", 18, sizeof(int), sizeof(int), 0, 0 },
	{ "AlbumFlag", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, iAlbumFlag), eTypeSKInt32, 0, 0, -1,
		"", 19, sizeof(int), sizeof(int), 0, 0 },
	{ "AlbumBGImgID", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcAlbumBGImgID), eTypeSKChar, 1, 0, -1,
		"strlen", 20, sizeof(char *), sizeof(char), 0, 0 },
	{ "SnsUserInfo", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, tSnsUserInfo), eTypeIMSnsUserInfo, 0, 0, -1,
		"", 21, sizeof(IMSnsUserInfo_t), sizeof(IMSnsUserInfo_t), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 22, sizeof(char *), sizeof(char), 0, 0 },
	{ "MyBrandList", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcMyBrandList), eTypeSKChar, 1, 0, -1,
		"strlen", 23, sizeof(char *), sizeof(char), 0, 0 },
	{ "CustomizedInfo", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, tCustomizedInfo), eTypeIMCustomizedInfo, 0, 0, -1,
		"", 24, sizeof(IMCustomizedInfo_t), sizeof(IMCustomizedInfo_t), 0, 0 },
	{ "BigHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcBigHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 25, sizeof(char *), sizeof(char), 0, 0 },
	{ "SmallHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, pcSmallHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 26, sizeof(char *), sizeof(char), 0, 0 },
	{ "SafeUserName", static_cast<short>SK_FIELD_OFFSET(IMSearchContactItem_t, tSafeUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 27, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMSearchContactResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, tNickName), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "PYInitial", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, tPYInitial), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "QuanPin", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, tQuanPin), eTypeSKBuiltinString, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, iSex), eTypeSKInt32, 0, 1, -1,
		"", 6, sizeof(int), sizeof(int), 1, 0 },
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, tImgBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 7, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "Province", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcProvince), eTypeSKChar, 1, 0, -1,
		"strlen", 8, sizeof(char *), sizeof(char), 0, 0 },
	{ "City", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcCity), eTypeSKChar, 1, 0, -1,
		"strlen", 9, sizeof(char *), sizeof(char), 0, 0 },
	{ "Signature", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcSignature), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 0, 0 },
	{ "PersonalCard", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, iPersonalCard), eTypeSKUint32, 0, 0, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyFlag", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, iVerifyFlag), eTypeSKUint32, 0, 0, -1,
		"", 12, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyInfo", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcVerifyInfo), eTypeSKChar, 1, 0, -1,
		"strlen", 13, sizeof(char *), sizeof(char), 0, 0 },
	{ "NeedVerify", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, iNeedVerify), eTypeSKInt32, 0, 1, -1,
		"", 14, sizeof(int), sizeof(int), 1, 0 },
	{ "Weibo", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcWeibo), eTypeSKChar, 1, 0, -1,
		"strlen", 15, sizeof(char *), sizeof(char), 0, 0 },
	{ "Alias", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcAlias), eTypeSKChar, 1, 0, -1,
		"strlen", 16, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboNickname", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcWeiboNickname), eTypeSKChar, 1, 0, -1,
		"strlen", 17, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboFlag", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, iWeiboFlag), eTypeSKUint32, 0, 0, -1,
		"", 18, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "AlbumStyle", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, iAlbumStyle), eTypeSKInt32, 0, 0, -1,
		"", 19, sizeof(int), sizeof(int), 0, 0 },
	{ "AlbumFlag", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, iAlbumFlag), eTypeSKInt32, 0, 0, -1,
		"", 20, sizeof(int), sizeof(int), 0, 0 },
	{ "AlbumBGImgID", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcAlbumBGImgID), eTypeSKChar, 1, 0, -1,
		"strlen", 21, sizeof(char *), sizeof(char), 0, 0 },
	{ "SnsUserInfo", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, tSnsUserInfo), eTypeIMSnsUserInfo, 0, 0, -1,
		"", 22, sizeof(IMSnsUserInfo_t), sizeof(IMSnsUserInfo_t), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 23, sizeof(char *), sizeof(char), 0, 0 },
	{ "MyBrandList", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcMyBrandList), eTypeSKChar, 1, 0, -1,
		"strlen", 24, sizeof(char *), sizeof(char), 0, 0 },
	{ "CustomizedInfo", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, tCustomizedInfo), eTypeIMCustomizedInfo, 0, 0, -1,
		"", 25, sizeof(IMCustomizedInfo_t), sizeof(IMCustomizedInfo_t), 0, 0 },
	{ "ContactCount", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, iContactCount), eTypeSKUint32, 0, 0, -1,
		"", 26, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ContactList", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, ptContactList), eTypeIMSearchContactItem, 1, 0, -1,
		"ContactCount", 27, sizeof(IMSearchContactItem_t *), sizeof(IMSearchContactItem_t), 0, 0 },
	{ "BigHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcBigHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 28, sizeof(char *), sizeof(char), 0, 0 },
	{ "SmallHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, pcSmallHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 29, sizeof(char *), sizeof(char), 0, 0 },
	{ "SafeUserName", static_cast<short>SK_FIELD_OFFSET(IMSearchContactResponse_t, tSafeUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 30, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMVerifyUser [] = {
	{ "Value", static_cast<short>SK_FIELD_OFFSET(IMVerifyUser_t, pcValue), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 0, 0 },
	{ "VerifyUserTicket", static_cast<short>SK_FIELD_OFFSET(IMVerifyUser_t, pcVerifyUserTicket), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMVerifyUserRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMVerifyUserRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "Opcode", static_cast<short>SK_FIELD_OFFSET(IMVerifyUserRequest_t, iOpcode), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyUserListSize", static_cast<short>SK_FIELD_OFFSET(IMVerifyUserRequest_t, iVerifyUserListSize), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyUserList", static_cast<short>SK_FIELD_OFFSET(IMVerifyUserRequest_t, ptVerifyUserList), eTypeIMVerifyUser, 1, 0, -1,
		"VerifyUserListSize", 4, sizeof(IMVerifyUser_t *), sizeof(IMVerifyUser_t), 0, 0 },
	{ "VerifyContent", static_cast<short>SK_FIELD_OFFSET(IMVerifyUserRequest_t, pcVerifyContent), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "SceneListCount", static_cast<short>SK_FIELD_OFFSET(IMVerifyUserRequest_t, iSceneListCount), eTypeSKUint32, 0, 0, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "SceneList", static_cast<short>SK_FIELD_OFFSET(IMVerifyUserRequest_t, piSceneList), eTypeSKUint32, 1, 0, -1,
		"SceneListCount", 7, sizeof(unsigned int *), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMVerifyUserResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMVerifyUserResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "Opcode", static_cast<short>SK_FIELD_OFFSET(IMVerifyUserResponse_t, iOpcode), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMVerifyUserResponse_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMPossibleFriend [] = {
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 1, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, pcNickName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
	{ "PYInitial", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, pcPYInitial), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
	{ "QuanPin", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, pcQuanPin), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, iSex), eTypeSKInt32, 0, 1, -1,
		"", 5, sizeof(int), sizeof(int), 1, 0 },
	{ "ImgFlag", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, iImgFlag), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ContactType", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, iContactType), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "DomainList", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, pcDomainList), eTypeSKChar, 1, 0, -1,
		"strlen", 8, sizeof(char *), sizeof(char), 1, 0 },
	{ "Source", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, pcSource), eTypeSKChar, 1, 0, -1,
		"strlen", 9, sizeof(char *), sizeof(char), 1, 0 },
	{ "FriendScene", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, iFriendScene), eTypeSKUint32, 0, 1, -1,
		"", 10, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ThirdID", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, pcThirdID), eTypeSKChar, 1, 0, -1,
		"strlen", 11, sizeof(char *), sizeof(char), 1, 0 },
	{ "NeedVerify", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, iNeedVerify), eTypeSKInt32, 0, 1, -1,
		"", 12, sizeof(int), sizeof(int), 1, 0 },
	{ "FBUserName", static_cast<short>SK_FIELD_OFFSET(IMPossibleFriend_t, pcFBUserName), eTypeSKChar, 1, 0, -1,
				"strlen", 13, sizeof(char *), sizeof(char), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMSyncFriendRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMSyncFriendRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMSyncFriendRequest_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "SyncKey", static_cast<short>SK_FIELD_OFFSET(IMSyncFriendRequest_t, iSyncKey), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Scene", static_cast<short>SK_FIELD_OFFSET(IMSyncFriendRequest_t, iScene), eTypeSKInt32, 0, 1, -1,
		"", 4, sizeof(int), sizeof(int), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSyncFriendResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMSyncFriendResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "NewSyncKey", static_cast<short>SK_FIELD_OFFSET(IMSyncFriendResponse_t, iNewSyncKey), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ContinueFlag", static_cast<short>SK_FIELD_OFFSET(IMSyncFriendResponse_t, iContinueFlag), eTypeSKInt32, 0, 1, -1,
		"", 3, sizeof(int), sizeof(int), 1, 0 },
	{ "CmdCount", static_cast<short>SK_FIELD_OFFSET(IMSyncFriendResponse_t, iCmdCount), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "CmdList", static_cast<short>SK_FIELD_OFFSET(IMSyncFriendResponse_t, ptCmdList), eTypeIMCmdItem, 1, 0, -1,
		"CmdCount", 5, sizeof(IMCmdItem_t *), sizeof(IMCmdItem_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMRoomInfo [] = {
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMRoomInfo_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMRoomInfo_t, tNickName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMChatRoomMember [] = {
	{ "MemberName", static_cast<short>SK_FIELD_OFFSET(IMChatRoomMember_t, tMemberName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "MemberSafeName", static_cast<short>SK_FIELD_OFFSET(IMChatRoomMember_t, tMemberSafeName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMChatRoomMember_t, tNickName), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMChatRoomMember_t, iSex), eTypeSKInt32, 0, 1, -1,
		"", 4, sizeof(int), sizeof(int), 0, 0 },
	{ "Signature", static_cast<short>SK_FIELD_OFFSET(IMChatRoomMember_t, pcSignature), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMChatRoomMember_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 0, 0 },
	{ "Birthday", static_cast<short>SK_FIELD_OFFSET(IMChatRoomMember_t, tBirthday), eTypeIMBirthday, 0, 0, -1,
		"", 7, sizeof(IMBirthday_t), sizeof(IMBirthday_t), 0, 0 }
};
#if 0
static const SKMetaField_t g_tMetaFieldListIMModContact1 [] = {
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tNickName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "PYInitial", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tPYInitial), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "QuanPin", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tQuanPin), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iSex), eTypeSKInt32, 0, 1, -1,
		"", 5, sizeof(int), sizeof(int), 1, 0 },
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tImgBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 6, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "BitMask", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iBitMask), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "BitVal", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iBitVal), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ImgFlag", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iImgFlag), eTypeSKUint32, 0, 1, -1,
		"", 9, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Remark", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tRemark), eTypeSKBuiltinString, 0, 0, -1,
		"", 10, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "RemarkPYInitial", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tRemarkPYInitial), eTypeSKBuiltinString, 0, 0, -1,
		"", 11, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "RemarkQuanPin", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tRemarkQuanPin), eTypeSKBuiltinString, 0, 0, -1,
		"", 12, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "ContactType", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iContactType), eTypeSKUint32, 0, 0, -1,
		"", 13, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "RoomMemberCount", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iRoomMemberCount), eTypeSKUint32, 0, 0, -1,
		"", 14, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "RoomMemberList", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, ptRoomMemberList), eTypeIMChatRoomMember, 1, 0, -1,
		"RoomMemberCount", 15, sizeof(IMChatRoomMember_t *), sizeof(IMChatRoomMember_t), 1, 0 },
	{ "DomainList", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tDomainList), eTypeSKBuiltinString, 0, 0, -1,
		"", 16, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "ChatRoomNotify", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iChatRoomNotify), eTypeSKUint32, 0, 0, -1,
		"", 17, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "AddContactScene", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iAddContactScene), eTypeSKUint32, 0, 0, -1,
		"", 18, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Province", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcProvince), eTypeSKChar, 1, 0, -1,
		"strlen", 19, sizeof(char *), sizeof(char), 0, 0 },
	{ "City", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcCity), eTypeSKChar, 1, 0, -1,
		"strlen", 20, sizeof(char *), sizeof(char), 0, 0 },
	{ "Signature", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcSignature), eTypeSKChar, 1, 0, -1,
		"strlen", 21, sizeof(char *), sizeof(char), 0, 0 },
	{ "PersonalCard", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iPersonalCard), eTypeSKUint32, 0, 0, -1,
		"", 22, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "HasWeiXinHdHeadImg", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iHasWeiXinHdHeadImg), eTypeSKUint32, 0, 0, -1,
		"", 23, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyFlag", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iVerifyFlag), eTypeSKUint32, 0, 0, -1,
		"", 24, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyInfo", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcVerifyInfo), eTypeSKChar, 1, 0, -1,
		"strlen", 25, sizeof(char *), sizeof(char), 0, 0 },
	{ "Level", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iLevel), eTypeSKInt32, 0, 0, -1,
		"", 26, sizeof(int), sizeof(int), 0, 0 },
	{ "Source", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iSource), eTypeSKUint32, 0, 0, -1,
		"", 27, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Weibo", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcWeibo), eTypeSKChar, 1, 0, -1,
		"strlen", 28, sizeof(char *), sizeof(char), 0, 0 },
	{ "VerifyContent", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcVerifyContent), eTypeSKChar, 1, 0, -1,
		"strlen", 29, sizeof(char *), sizeof(char), 0, 0 },
	{ "Alias", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcAlias), eTypeSKChar, 1, 0, -1,
		"strlen", 30, sizeof(char *), sizeof(char), 0, 0 },
	{ "ChatRoomOwner", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcChatRoomOwner), eTypeSKChar, 1, 0, -1,
		"strlen", 31, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboNickname", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcWeiboNickname), eTypeSKChar, 1, 0, -1,
		"strlen", 32, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboFlag", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iWeiboFlag), eTypeSKUint32, 0, 0, -1,
		"", 33, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "AlbumStyle", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iAlbumStyle), eTypeSKInt32, 0, 0, -1,
		"", 34, sizeof(int), sizeof(int), 0, 0 },
	{ "AlbumFlag", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iAlbumFlag), eTypeSKInt32, 0, 0, -1,
		"", 35, sizeof(int), sizeof(int), 0, 0 },
	{ "AlbumBGImgID", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcAlbumBGImgID), eTypeSKChar, 1, 0, -1,
		"strlen", 36, sizeof(char *), sizeof(char), 0, 0 },
	{ "SnsUserInfo", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tSnsUserInfo), eTypeIMSnsUserInfo, 0, 0, -1,
		"", 37, sizeof(IMSnsUserInfo_t), sizeof(IMSnsUserInfo_t), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 38, sizeof(char *), sizeof(char), 0, 0 },
	{ "BigHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcBigHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 39, sizeof(char *), sizeof(char), 0, 0 },
	{ "SmallHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcSmallHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 40, sizeof(char *), sizeof(char), 0, 0 },
	{ "MyBrandList", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcMyBrandList), eTypeSKChar, 1, 0, -1,
		"strlen", 41, sizeof(char *), sizeof(char), 0, 0 },
	{ "CustomizedInfo", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tCustomizedInfo), eTypeIMCustomizedInfo, 0, 0, -1,
		"", 42, sizeof(IMCustomizedInfo_t), sizeof(IMCustomizedInfo_t), 0, 0 },
	{ "ChatRoomData", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcChatRoomData), eTypeSKChar, 1, 0, -1,
		"strlen", 43, sizeof(char *), sizeof(char), 0, 0 },
	{ "HeadImgMd5", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcHeadImgMd5), eTypeSKChar, 1, 0, -1,
		"strlen", 44, sizeof(char *), sizeof(char), 0, 0 },
	{ "SafeUserName", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tSafeUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 45, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Birthday", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tBirthday), eTypeIMBirthday, 0, 0, -1,
		"", 46, sizeof(IMBirthday_t), sizeof(IMBirthday_t), 0, 0 },
	{ "Age", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iAge), eTypeSKUint32, 0, 0, -1,
		"", 47, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Hobby", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tHobby), eTypeIMHobby, 0, 0, -1,
		"", 48, sizeof(IMHobby_t), sizeof(IMHobby_t), 0, 0 },
	{ "IntentionFlag", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iIntentionFlag), eTypeSKUint32, 0, 0, -1,
		"", 49, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MaxMemberCount", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iMaxMemberCount), eTypeSKUint32, 0, 0, -1,
		"", 50, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Mobile", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcMobile), eTypeSKChar, 1, 0, -1,
		"strlen", 51, sizeof(char *), sizeof(char), 0, 0 },
	{ "FBUserID", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, llFBUserID), eTypeSKUint64, 0, 0, -1,
		"", 52, sizeof(unsigned long long), sizeof(unsigned long long), 0, 0 }	
};
#endif
static const SKMetaField_t g_tMetaFieldListIMModContact [] = {
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tNickName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "PYInitial", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tPYInitial), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "QuanPin", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tQuanPin), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iSex), eTypeSKInt32, 0, 1, -1,
		"", 5, sizeof(int), sizeof(int), 1, 0 },
	{ "BitMask", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iBitMask), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "BitVal", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iBitVal), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Remark", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tRemark), eTypeSKBuiltinString, 0, 0, -1,
		"", 8, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "RemarkPYInitial", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tRemarkPYInitial), eTypeSKBuiltinString, 0, 0, -1,
		"", 9, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "RemarkQuanPin", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tRemarkQuanPin), eTypeSKBuiltinString, 0, 0, -1,
		"", 10, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "ContactType", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iContactType), eTypeSKUint32, 0, 0, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Source", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iSource), eTypeSKUint32, 0, 0, -1,
		"", 12, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Signature", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcSignature), eTypeSKChar, 1, 0, -1,
		"strlen", 13, sizeof(char *), sizeof(char), 0, 0 },
	{ "HeadImgMd5", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcHeadImgMd5), eTypeSKChar, 1, 0, -1,
		"strlen", 14, sizeof(char *), sizeof(char), 0, 0 },
	{ "CoverImgMD5", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcCoverImgMD5), eTypeSKChar, 1, 0, -1,
		"strlen", 15, sizeof(char *), sizeof(char), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 16, sizeof(char *), sizeof(char), 0, 0 },
	{ "Province", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcProvince), eTypeSKChar, 1, 0, -1,
		"strlen", 17, sizeof(char *), sizeof(char), 0, 0 },
	{ "City", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcCity), eTypeSKChar, 1, 0, -1,
		"strlen", 18, sizeof(char *), sizeof(char), 0, 0 },
	{ "VerifyFlag", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iVerifyFlag), eTypeSKUint32, 0, 0, -1,
		"", 19, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "SafeUserName", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tSafeUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 20, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Birthday", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, tBirthday), eTypeIMBirthday, 0, 0, -1,
		"", 21, sizeof(IMBirthday_t), sizeof(IMBirthday_t), 0, 0 },
	{ "Mobile", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcMobile), eTypeSKChar, 1, 0, -1,
		"strlen", 22, sizeof(char *), sizeof(char), 0, 0 },
	{ "FBUserID", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, llFBUserID), eTypeSKUint64, 0, 0, -1,
		"", 23, sizeof(unsigned long long), sizeof(unsigned long long), 0, 0 },
	{ "RoomMemberCount", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iRoomMemberCount), eTypeSKUint32, 0, 0, -1,
		"", 24, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "RoomMemberList", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, ptRoomMemberList), eTypeIMChatRoomMember, 1, 0, -1,
		"RoomMemberCount", 25, sizeof(IMChatRoomMember_t *), sizeof(IMChatRoomMember_t), 1, 0 },
	{ "ChatRoomOwner", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, pcChatRoomOwner), eTypeSKChar, 1, 0, -1,
		"strlen", 26, sizeof(char *), sizeof(char), 0, 0 },
	{ "MaxMemberCount", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iMaxMemberCount), eTypeSKUint32, 0, 0, -1,
		"", 27, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Type", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, iType), eTypeSKInt32, 0, 0, -1,
		"", 28, sizeof(int), sizeof(int), 0, 0 },
	{ "Longitude", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, fLongitude), eTypeSKFloat32, 0, 1, -1,
		"", 29, sizeof(float), sizeof(float), 0, 0 },
	{ "Latitude", static_cast<short>SK_FIELD_OFFSET(IMModContact_t, fLatitude), eTypeSKFloat32, 0, 1, -1,
		"", 30, sizeof(float), sizeof(float), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMGetContactRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMGetContactRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "UserCount", static_cast<short>SK_FIELD_OFFSET(IMGetContactRequest_t, iUserCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserNameList", static_cast<short>SK_FIELD_OFFSET(IMGetContactRequest_t, ptUserNameList), eTypeSKBuiltinString, 1, 0, -1,
		"UserCount", 3, sizeof(Comm::SKBuiltinString_t *), sizeof(Comm::SKBuiltinString_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMGetContactResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMGetContactResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "ContactCount", static_cast<short>SK_FIELD_OFFSET(IMGetContactResponse_t, iContactCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ContactList", static_cast<short>SK_FIELD_OFFSET(IMGetContactResponse_t, ptContactList), eTypeIMModContact, 1, 0, -1,
		"ContactCount", 3, sizeof(IMModContact_t *), sizeof(IMModContact_t), 0, 0 },
	{ "Ret", static_cast<short>SK_FIELD_OFFSET(IMGetContactResponse_t, piRet), eTypeSKInt32, 1, 0, -1,
		"ContactCount", 4, sizeof(int *), sizeof(int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDelContact [] = {
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMDelContact_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMAuthBySKRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMAuthBySKRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMAuthBySKResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMAuthBySKResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "Uin", static_cast<short>SK_FIELD_OFFSET(IMAuthBySKResponse_t, iUin), eTypeSKUint32, 0, 0, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "NewVersion", static_cast<short>SK_FIELD_OFFSET(IMAuthBySKResponse_t, iNewVersion), eTypeSKUint32, 0, 0, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "WebProxyInfo", static_cast<short>SK_FIELD_OFFSET(IMAuthBySKResponse_t, tWebProxyInfo), eTypeIMWebProxyInfo, 0, 0, -1,
		"", 4, sizeof(IMWebProxyInfo_t), sizeof(IMWebProxyInfo_t), 0, 0 },
	{ "SysTime", static_cast<short>SK_FIELD_OFFSET(IMAuthBySKResponse_t, iSysTime), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMUploadVideoRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, pcFromUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, pcToUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "ThumbTotalLen", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, iThumbTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ThumbStartPos", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, iThumbStartPos), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ThumbData", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, tThumbData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 7, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "VideoTotalLen", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, iVideoTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "VideoStartPos", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, iVideoStartPos), eTypeSKUint32, 0, 1, -1,
		"", 9, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "VideoData", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, tVideoData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 10, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "PlayLength", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, iPlayLength), eTypeSKUint32, 0, 1, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "NetworkEnv", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, iNetworkEnv), eTypeSKUint32, 0, 0, -1,
		"", 12, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "CameraType", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, iCameraType), eTypeSKUint32, 0, 0, -1,
		"", 13, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "FuncFlag", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, iFuncFlag), eTypeSKUint32, 0, 0, -1,
		"", 14, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "MsgSource", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, pcMsgSource), eTypeSKChar, 1, 0, -1,
		"strlen", 15, sizeof(char *), sizeof(char), 1, 0 },
	{ "CDNVideoUrl", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, pcCDNVideoUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 16, sizeof(char *), sizeof(char), 1, 0 },
	{ "AESKey", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, pcAESKey), eTypeSKChar, 1, 0, -1,
		"strlen", 17, sizeof(char *), sizeof(char), 1, 0 },
	{ "EncryVer", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoRequest_t, iEncryVer), eTypeSKInt32, 0, 0, -1,
		"", 18, sizeof(int), sizeof(int), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMUploadVideoResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoResponse_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoResponse_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ThumbStartPos", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoResponse_t, iThumbStartPos), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "VideoStartPos", static_cast<short>SK_FIELD_OFFSET(IMUploadVideoResponse_t, iVideoStartPos), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDownloadVideoRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoRequest_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoRequest_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoRequest_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "NetworkEnv", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoRequest_t, iNetworkEnv), eTypeSKUint32, 0, 0, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "MxPackSize", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoRequest_t, iMxPackSize), eTypeSKUint32, 0, 0, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDownloadVideoResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoResponse_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoResponse_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoResponse_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Data", static_cast<short>SK_FIELD_OFFSET(IMDownloadVideoResponse_t, tData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDownloadMsgImgRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgRequest_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgRequest_t, pcFromUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgRequest_t, pcToUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgRequest_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgRequest_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "DataLen", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgRequest_t, iDataLen), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "CompressType", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgRequest_t, iCompressType), eTypeSKUint32, 0, 0, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDownloadMsgImgResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgResponse_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgResponse_t, pcFromUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgResponse_t, pcToUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgResponse_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgResponse_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "DataLen", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgResponse_t, iDataLen), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Data", static_cast<short>SK_FIELD_OFFSET(IMDownloadMsgImgResponse_t, tData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMUploadMsgImgRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, pcFromUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, pcToUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "DataLen", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, iDataLen), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Data", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, tData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "MsgType", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, iMsgType), eTypeSKUint32, 0, 0, -1,
		"", 9, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "MsgSource", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, pcMsgSource), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 1, 0 },
	{ "CompressType", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, iCompressType), eTypeSKUint32, 0, 0, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "NetType", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, iNetType), eTypeSKInt32, 0, 0, -1,
		"", 12, sizeof(int), sizeof(int), 0, 0 },
	{ "PhotoFrom", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, iPhotoFrom), eTypeSKInt32, 0, 0, -1,
		"", 13, sizeof(int), sizeof(int), 0, 0 },
	{ "MediaId", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, pcMediaId), eTypeSKChar, 1, 0, -1,
		"strlen", 14, sizeof(char *), sizeof(char), 1, 0 },
	{ "CDNBigImgUrl", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, pcCDNBigImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 15, sizeof(char *), sizeof(char), 1, 0 },
	{ "CDNMidImgUrl", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, pcCDNMidImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 16, sizeof(char *), sizeof(char), 1, 0 },
	{ "AESKey", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, pcAESKey), eTypeSKChar, 1, 0, -1,
		"strlen", 17, sizeof(char *), sizeof(char), 1, 0 },
	{ "EncryVer", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, iEncryVer), eTypeSKInt32, 0, 0, -1,
		"", 18, sizeof(int), sizeof(int), 1, 0 },
	{ "CDNBigImgSize", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, iCDNBigImgSize), eTypeSKInt32, 0, 0, -1,
		"", 19, sizeof(int), sizeof(int), 1, 0 },
	{ "CDNMidImgSize", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, iCDNMidImgSize), eTypeSKInt32, 0, 0, -1,
		"", 20, sizeof(int), sizeof(int), 1, 0 },
	{ "MD5", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgRequest_t, pcMD5), eTypeSKChar, 1, 0, -1,
		"strlen", 21, sizeof(char *), sizeof(char), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMUploadMsgImgResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgResponse_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgResponse_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgResponse_t, pcFromUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgResponse_t, pcToUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 1, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgResponse_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgResponse_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "DataLen", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgResponse_t, iDataLen), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "CreateTime", static_cast<short>SK_FIELD_OFFSET(IMUploadMsgImgResponse_t, iCreateTime), eTypeSKUint32, 0, 1, -1,
		"", 9, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};


//ÂèëÈ?ÅÊ∂àÊÅ?
static const SKMetaField_t g_tMetaFieldListIMMsgRequest [] = {
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMMsgRequest_t, tFromUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMMsgRequest_t, tToUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Type", static_cast<short>SK_FIELD_OFFSET(IMMsgRequest_t, iType), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Content", static_cast<short>SK_FIELD_OFFSET(IMMsgRequest_t, pcContent), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "CreateTime", static_cast<short>SK_FIELD_OFFSET(IMMsgRequest_t, iCreateTime), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMMsgRequest_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 1, 0 },
	{ "MsgSource", static_cast<short>SK_FIELD_OFFSET(IMMsgRequest_t, pcMsgSource), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 1, 0 },
	{ "EmojiFlag", static_cast<short>SK_FIELD_OFFSET(IMMsgRequest_t, iEmojiFlag), eTypeSKUint32, 0, 0, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMSendMsgRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMSendMsgRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMSendMsgRequest_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "List", static_cast<short>SK_FIELD_OFFSET(IMSendMsgRequest_t, ptList), eTypeIMMsgRequest, 1, 0, -1,
		"Count", 3, sizeof(IMMsgRequest_t *), sizeof(IMMsgRequest_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMMsgResponse [] = {
	{ "Ret", static_cast<short>SK_FIELD_OFFSET(IMMsgResponse_t, iRet), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 1, 0 },
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMMsgResponse_t, tFromUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMMsgResponse_t, tToUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMMsgResponse_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMMsgResponse_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 1, 0 },
	{ "CreateTime", static_cast<short>SK_FIELD_OFFSET(IMMsgResponse_t, iCreateTime), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Type", static_cast<short>SK_FIELD_OFFSET(IMMsgResponse_t, iType), eTypeSKUint32, 0, 0, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSendMsgResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMSendMsgResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMSendMsgResponse_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "List", static_cast<short>SK_FIELD_OFFSET(IMSendMsgResponse_t, ptList), eTypeIMMsgResponse, 1, 0, -1,
		"Count", 3, sizeof(IMMsgResponse_t *), sizeof(IMMsgResponse_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMCustomMsgRequest [] = {
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMCustomMsgRequest_t, tFromUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMCustomMsgRequest_t, tToUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Type", static_cast<short>SK_FIELD_OFFSET(IMCustomMsgRequest_t, iType), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "data", static_cast<short>SK_FIELD_OFFSET(IMCustomMsgRequest_t, tData), eTypeSKBuiltinBuffer, 1, 0, -1,
		"", 4, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "CreateTime", static_cast<short>SK_FIELD_OFFSET(IMCustomMsgRequest_t, iCreateTime), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMCustomMsgRequest_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 1, 0 },
	{ "MsgSource", static_cast<short>SK_FIELD_OFFSET(IMCustomMsgRequest_t, pcMsgSource), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 1, 0 },

};


static const SKMetaField_t g_tMetaFieldListIMSendCustomMsgRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMSendCustomMsgRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMSendCustomMsgRequest_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "List", static_cast<short>SK_FIELD_OFFSET(IMSendCustomMsgRequest_t, ptList), eTypeIMCustomMsgRequest, 1, 0, -1,
		"Count", 3, sizeof(IMCustomMsgRequest_t *), sizeof(IMCustomMsgRequest_t), 1, 0 }
};
/*
static const SKMetaField_t g_tMetaFieldListIMMsgResponse [] = {
	{ "Ret", SK_FIELD_OFFSET(IMMsgResponse_t, iRet), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 1, 0 },
	{ "FromUserName", SK_FIELD_OFFSET(IMMsgResponse_t, tFromUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "ToUserName", SK_FIELD_OFFSET(IMMsgResponse_t, tToUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "MsgId", SK_FIELD_OFFSET(IMMsgResponse_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ClientMsgId", SK_FIELD_OFFSET(IMMsgResponse_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 1, 0 },
	{ "CreateTime", SK_FIELD_OFFSET(IMMsgResponse_t, iCreateTime), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Type", SK_FIELD_OFFSET(IMMsgResponse_t, iType), eTypeSKUint32, 0, 0, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSendMsgResponse [] = {
	{ "BaseResponse", SK_FIELD_OFFSET(IMSendMsgResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "Count", SK_FIELD_OFFSET(IMSendMsgResponse_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "List", SK_FIELD_OFFSET(IMSendMsgResponse_t, ptList), eTypeIMMsgResponse, 1, 0, -1,
		"Count", 3, sizeof(IMMsgResponse_t *), sizeof(IMMsgResponse_t), 1, 0 }
};

*/


//同�?�消�?

static const SKMetaField_t g_tMetaFieldListIMAddMsg [] = {
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, iMsgId), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 1, 0 },
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, tFromUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, tToUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "MsgType", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, iMsgType), eTypeSKInt32, 0, 1, -1,
		"", 4, sizeof(int), sizeof(int), 1, 0 },
	{ "Content", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, tContent), eTypeSKBuiltinString, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Status", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, iStatus), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ImgStatus", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, iImgStatus), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, tImgBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "CreateTime", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, iCreateTime), eTypeSKUint32, 0, 1, -1,
		"", 9, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "MsgSource", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, pcMsgSource), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 1, 0 },
	{ "PushContent", static_cast<short>SK_FIELD_OFFSET(IMAddMsg_t, pcPushContent), eTypeSKChar, 1, 0, -1,
		"strlen", 11, sizeof(char *), sizeof(char), 0, 0 }
};



static const SKMetaField_t g_tMetaFieldListIMCmdList [] = {
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMCmdList_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "List", static_cast<short>SK_FIELD_OFFSET(IMCmdList_t, ptList), eTypeIMCmdItem, 1, 0, -1,
		"Count", 2, sizeof(IMCmdItem_t *), sizeof(IMCmdItem_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMOpCmdResp [] = {
	{ "CmdId", static_cast<short>SK_FIELD_OFFSET(IMOpCmdResp_t, iCmdId), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 1, 0 },
	{ "Ret", static_cast<short>SK_FIELD_OFFSET(IMOpCmdResp_t, iRet), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMOpCmdRespList [] = {
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMOpCmdRespList_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "List", static_cast<short>SK_FIELD_OFFSET(IMOpCmdRespList_t, ptList), eTypeIMOpCmdResp, 1, 0, -1,
		"Count", 2, sizeof(IMOpCmdResp_t *), sizeof(IMOpCmdResp_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMNewSyncRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMNewSyncRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "Oplog", static_cast<short>SK_FIELD_OFFSET(IMNewSyncRequest_t, tOplog), eTypeIMCmdList, 0, 1, -1,
		"", 2, sizeof(IMCmdList_t), sizeof(IMCmdList_t), 0, 0 },
	{ "Selector", static_cast<short>SK_FIELD_OFFSET(IMNewSyncRequest_t, iSelector), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "KeyBuf", static_cast<short>SK_FIELD_OFFSET(IMNewSyncRequest_t, tKeyBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "Scene", static_cast<short>SK_FIELD_OFFSET(IMNewSyncRequest_t, iScene), eTypeSKUint32, 0, 0, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMNewSyncResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMNewSyncResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "Ret", static_cast<short>SK_FIELD_OFFSET(IMNewSyncResponse_t, iRet), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 0, 0 },
	{ "CmdList", static_cast<short>SK_FIELD_OFFSET(IMNewSyncResponse_t, tCmdList), eTypeIMCmdList, 0, 1, -1,
		"", 3, sizeof(IMCmdList_t), sizeof(IMCmdList_t), 0, 0 },
	{ "ContinueFlag", static_cast<short>SK_FIELD_OFFSET(IMNewSyncResponse_t, iContinueFlag), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "KeyBuf", static_cast<short>SK_FIELD_OFFSET(IMNewSyncResponse_t, tKeyBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "Status", static_cast<short>SK_FIELD_OFFSET(IMNewSyncResponse_t, iStatus), eTypeSKUint32, 0, 0, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "OpCmdResp", static_cast<short>SK_FIELD_OFFSET(IMNewSyncResponse_t, tOpCmdResp), eTypeIMOpCmdRespList, 0, 1, -1,
		"", 7, sizeof(IMOpCmdRespList_t), sizeof(IMOpCmdRespList_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSyncRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMSyncRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMSyncRequest_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "SyncKey", static_cast<short>SK_FIELD_OFFSET(IMSyncRequest_t, iSyncKey), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Scene", static_cast<short>SK_FIELD_OFFSET(IMSyncRequest_t, iScene), eTypeSKInt32, 0, 1, -1,
		"", 4, sizeof(int), sizeof(int), 1, 0 },
	{ "Option", static_cast<short>SK_FIELD_OFFSET(IMSyncRequest_t, iOption), eTypeSKInt32, 0, 1, -1,
		"", 5, sizeof(int), sizeof(int), 1, 0 },
	{ "Oplog", static_cast<short>SK_FIELD_OFFSET(IMSyncRequest_t, tOplog), eTypeIMCmdList, 0, 1, -1,
		"", 6, sizeof(IMCmdList_t), sizeof(IMCmdList_t), 1, 0 },
	{ "KeyBuf", static_cast<short>SK_FIELD_OFFSET(IMSyncRequest_t, pcKeyBuf), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSyncResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMSyncResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "NewSyncKey", static_cast<short>SK_FIELD_OFFSET(IMSyncResponse_t, iNewSyncKey), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ContinueFlag", static_cast<short>SK_FIELD_OFFSET(IMSyncResponse_t, iContinueFlag), eTypeSKInt32, 0, 1, -1,
		"", 3, sizeof(int), sizeof(int), 1, 0 },
	{ "CmdCount", static_cast<short>SK_FIELD_OFFSET(IMSyncResponse_t, iCmdCount), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "CmdList", static_cast<short>SK_FIELD_OFFSET(IMSyncResponse_t, ptCmdList), eTypeIMCmdItem, 1, 0, -1,
		"CmdCount", 5, sizeof(IMCmdItem_t *), sizeof(IMCmdItem_t), 1, 0 },
	{ "OOBCount", static_cast<short>SK_FIELD_OFFSET(IMSyncResponse_t, iOOBCount), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "OOBList", static_cast<short>SK_FIELD_OFFSET(IMSyncResponse_t, ptOOBList), eTypeIMCmdItem, 1, 0, -1,
		"OOBCount", 7, sizeof(IMCmdItem_t *), sizeof(IMCmdItem_t), 1, 0 },
	{ "KeyBuf", static_cast<short>SK_FIELD_OFFSET(IMSyncResponse_t, pcKeyBuf), eTypeSKChar, 1, 0, -1,
		"strlen", 8, sizeof(char *), sizeof(char), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMBatchGetHeadImgRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMBatchGetHeadImgRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMBatchGetHeadImgRequest_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "UserNameList", static_cast<short>SK_FIELD_OFFSET(IMBatchGetHeadImgRequest_t, ptUserNameList), eTypeSKBuiltinString, 1, 0, -1,
		"Count", 3, sizeof(Comm::SKBuiltinString_t *), sizeof(Comm::SKBuiltinString_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMImgPair [] = {
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMImgPair_t, tImgBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "Username", static_cast<short>SK_FIELD_OFFSET(IMImgPair_t, tUsername), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMBatchGetHeadImgResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMBatchGetHeadImgResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMBatchGetHeadImgResponse_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ImgPairList", static_cast<short>SK_FIELD_OFFSET(IMBatchGetHeadImgResponse_t, ptImgPairList), eTypeIMImgPair, 1, 0, -1,
		"Count", 3, sizeof(IMImgPair_t *), sizeof(IMImgPair_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMUploadHDHeadImgRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "HeadImgType", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, iHeadImgType), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Data", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, tData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "ImgHash", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, pcImgHash), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 0, 0 },
	{ "MD5", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, pcMD5), eTypeSKChar, 1, 0, -1,
		"strlen", 8, sizeof(char *), sizeof(char), 0, 0 },
	{ "CDNImgUrl", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, pcCDNImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 9, sizeof(char *), sizeof(char), 1, 0 },
	{ "AESKey", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, pcAESKey), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 1, 0 },
	{ "EncryVer", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, iEncryVer), eTypeSKInt32, 0, 0, -1,
		"", 11, sizeof(int), sizeof(int), 1, 0 },
	{ "CDNImgSize", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgRequest_t, iCDNImgSize), eTypeSKInt32, 0, 0, -1,
		"", 12, sizeof(int), sizeof(int), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMUploadHDHeadImgResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgResponse_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgResponse_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "FinalImgMd5sum", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgResponse_t, pcFinalImgMd5sum), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 },
	{ "ImgHash", static_cast<short>SK_FIELD_OFFSET(IMUploadHDHeadImgResponse_t, pcImgHash), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMGetHDHeadImgRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgRequest_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "ImgWidth", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgRequest_t, iImgWidth), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ImgHeight", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgRequest_t, iImgHeight), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ImgFormat", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgRequest_t, pcImgFormat), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgRequest_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgRequest_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "HeadImgType", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgRequest_t, iHeadImgType), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMGetHDHeadImgResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgResponse_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgResponse_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgResponse_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Data", static_cast<short>SK_FIELD_OFFSET(IMGetHDHeadImgResponse_t, tData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMUploadVoiceRequest [] = {
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, pcFromUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, pcToUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
	{ "Offset", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, iOffset), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Length", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, iLength), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 1, 0 },
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "VoiceLength", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, iVoiceLength), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Data", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, tData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "EndFlag", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, iEndFlag), eTypeSKUint32, 0, 1, -1,
		"", 9, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 10, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "CancelFlag", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, iCancelFlag), eTypeSKUint32, 0, 0, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MsgSource", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, pcMsgSource), eTypeSKChar, 1, 0, -1,
		"strlen", 12, sizeof(char *), sizeof(char), 1, 0 },
	{ "VoiceFormat", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, iVoiceFormat), eTypeSKUint32, 0, 0, -1,
		"", 13, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UICreateTime", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, iUICreateTime), eTypeSKUint32, 0, 0, -1,
		"", 14, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ForwardFlag", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceRequest_t, iForwardFlag), eTypeSKUint32, 0, 0, -1,
		"", 15, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMUploadVoiceResponse [] = {
	{ "FromUserName", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, pcFromUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 1, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, pcToUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
	{ "Offset", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, iOffset), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Length", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, iLength), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "CreateTime", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, iCreateTime), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 1, 0 },
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "VoiceLength", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, iVoiceLength), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "EndFlag", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, iEndFlag), eTypeSKUint32, 0, 1, -1,
		"", 9, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 10, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "CancelFlag", static_cast<short>SK_FIELD_OFFSET(IMUploadVoiceResponse_t, iCancelFlag), eTypeSKUint32, 0, 0, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDownloadVoiceRequest [] = {
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceRequest_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Offset", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceRequest_t, iOffset), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Length", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceRequest_t, iLength), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceRequest_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 5, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDownloadVoiceResponse [] = {
	{ "MsgId", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceResponse_t, iMsgId), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Offset", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceResponse_t, iOffset), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "Length", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceResponse_t, iLength), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "VoiceLength", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceResponse_t, iVoiceLength), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ClientMsgId", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceResponse_t, pcClientMsgId), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 1, 0 },
	{ "Data", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceResponse_t, tData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 7, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 1, 0 },
	{ "EndFlag", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceResponse_t, iEndFlag), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 9, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "CancelFlag", static_cast<short>SK_FIELD_OFFSET(IMDownloadVoiceResponse_t, iCancelFlag), eTypeSKUint32, 0, 0, -1,
		"", 10, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMKeyVal [] = {
	{ "Key", static_cast<short>SK_FIELD_OFFSET(IMKeyVal_t, iKey), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Val", static_cast<short>SK_FIELD_OFFSET(IMKeyVal_t, iVal), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSyncKey [] = {
	{ "KeyCount", static_cast<short>SK_FIELD_OFFSET(IMSyncKey_t, iKeyCount), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Key", static_cast<short>SK_FIELD_OFFSET(IMSyncKey_t, ptKey), eTypeIMKeyVal, 1, 0, -1,
		"KeyCount", 2, sizeof(IMKeyVal_t *), sizeof(IMKeyVal_t), 0, 0 }
};

///////////////////////////
static const SKMetaField_t g_tMetaFieldListIMEmojiUploadInfoReq [] = {
	{ "MD5", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoReq_t, pcMD5), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoReq_t, iStartPos), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoReq_t, iTotalLen), eTypeSKInt32, 0, 1, -1,
		"", 3, sizeof(int), sizeof(int), 0, 0 },
	{ "EmojiBuffer", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoReq_t, tEmojiBuffer), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "Type", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoReq_t, iType), eTypeSKInt32, 0, 1, -1,
		"", 5, sizeof(int), sizeof(int), 0, 0 },
	{ "ToUserName", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoReq_t, pcToUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 0, 0 },
	{ "ExternXML", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoReq_t, pcExternXML), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 0, 0 },
	{ "Report", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoReq_t, pcReport), eTypeSKChar, 1, 0, -1,
		"strlen", 8, sizeof(char *), sizeof(char), 0, 0 },
	{ "ClientMsgID", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoReq_t, pcClientMsgID), eTypeSKChar, 1, 0, -1,
		"strlen", 9, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMUploadEmojiRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMUploadEmojiRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "EmojiItemCount", static_cast<short>SK_FIELD_OFFSET(IMUploadEmojiRequest_t, iEmojiItemCount), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 0, 0 },
	{ "EmojiItem", static_cast<short>SK_FIELD_OFFSET(IMUploadEmojiRequest_t, ptEmojiItem), eTypeIMEmojiUploadInfoReq, 1, 0, -1,
		"EmojiItemCount", 3, sizeof(IMEmojiUploadInfoReq_t *), sizeof(IMEmojiUploadInfoReq_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMEmojiUploadInfoResp [] = {
	{ "Ret", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoResp_t, iRet), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoResp_t, iStartPos), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoResp_t, iTotalLen), eTypeSKInt32, 0, 1, -1,
		"", 3, sizeof(int), sizeof(int), 0, 0 },
	{ "MD5", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoResp_t, pcMD5), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 },
	{ "ClientMsgID", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoResp_t, pcClientMsgID), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "MsgID", static_cast<short>SK_FIELD_OFFSET(IMEmojiUploadInfoResp_t, iMsgID), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMUploadEmojiResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMUploadEmojiResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "EmojiItemCount", static_cast<short>SK_FIELD_OFFSET(IMUploadEmojiResponse_t, iEmojiItemCount), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 0, 0 },
	{ "EmojiItem", static_cast<short>SK_FIELD_OFFSET(IMUploadEmojiResponse_t, ptEmojiItem), eTypeIMEmojiUploadInfoResp, 1, 0, -1,
		"EmojiItemCount", 3, sizeof(IMEmojiUploadInfoResp_t *), sizeof(IMEmojiUploadInfoResp_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMEmojiDownloadInfoReq [] = {
	{ "ID", static_cast<short>SK_FIELD_OFFSET(IMEmojiDownloadInfoReq_t, pcID), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMEmojiDownloadInfoReq_t, iStartPos), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMEmojiDownloadInfoReq_t, iTotalLen), eTypeSKInt32, 0, 1, -1,
		"", 3, sizeof(int), sizeof(int), 0, 0 },
	{ "MD5", static_cast<short>SK_FIELD_OFFSET(IMEmojiDownloadInfoReq_t, pcMD5), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDownloadEmojiRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMDownloadEmojiRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "EmojiItemCount", static_cast<short>SK_FIELD_OFFSET(IMDownloadEmojiRequest_t, iEmojiItemCount), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 0, 0 },
	{ "EmojiItem", static_cast<short>SK_FIELD_OFFSET(IMDownloadEmojiRequest_t, ptEmojiItem), eTypeIMEmojiDownloadInfoReq, 1, 0, -1,
		"EmojiItemCount", 3, sizeof(IMEmojiDownloadInfoReq_t *), sizeof(IMEmojiDownloadInfoReq_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMEmojiDownloadInfoResp [] = {
	{ "Ret", static_cast<short>SK_FIELD_OFFSET(IMEmojiDownloadInfoResp_t, iRet), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMEmojiDownloadInfoResp_t, iStartPos), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMEmojiDownloadInfoResp_t, iTotalLen), eTypeSKInt32, 0, 1, -1,
		"", 3, sizeof(int), sizeof(int), 0, 0 },
	{ "EmojiBuffer", static_cast<short>SK_FIELD_OFFSET(IMEmojiDownloadInfoResp_t, tEmojiBuffer), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "MD5", static_cast<short>SK_FIELD_OFFSET(IMEmojiDownloadInfoResp_t, pcMD5), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "ID", static_cast<short>SK_FIELD_OFFSET(IMEmojiDownloadInfoResp_t, pcID), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDownloadEmojiResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMDownloadEmojiResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "EmojiItemCount", static_cast<short>SK_FIELD_OFFSET(IMDownloadEmojiResponse_t, iEmojiItemCount), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 0, 0 },
	{ "EmojiItem", static_cast<short>SK_FIELD_OFFSET(IMDownloadEmojiResponse_t, ptEmojiItem), eTypeIMEmojiDownloadInfoResp, 1, 0, -1,
		"EmojiItemCount", 3, sizeof(IMEmojiDownloadInfoResp_t *), sizeof(IMEmojiDownloadInfoResp_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDisturbTimeSpan [] = {
	{ "BeginTime", static_cast<short>SK_FIELD_OFFSET(IMDisturbTimeSpan_t, iBeginTime), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "EndTime", static_cast<short>SK_FIELD_OFFSET(IMDisturbTimeSpan_t, iEndTime), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDisturbSetting [] = {
	{ "NightSetting", static_cast<short>SK_FIELD_OFFSET(IMDisturbSetting_t, iNightSetting), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "NightTime", static_cast<short>SK_FIELD_OFFSET(IMDisturbSetting_t, tNightTime), eTypeIMDisturbTimeSpan, 0, 1, -1,
		"", 2, sizeof(IMDisturbTimeSpan_t), sizeof(IMDisturbTimeSpan_t), 0, 0 },
	{ "AllDaySetting", static_cast<short>SK_FIELD_OFFSET(IMDisturbSetting_t, iAllDaySetting), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "AllDayTime", static_cast<short>SK_FIELD_OFFSET(IMDisturbSetting_t, tAllDayTime), eTypeIMDisturbTimeSpan, 0, 1, -1,
		"", 4, sizeof(IMDisturbTimeSpan_t), sizeof(IMDisturbTimeSpan_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMGmailInfo [] = {
	{ "GmailAcct", static_cast<short>SK_FIELD_OFFSET(IMGmailInfo_t, pcGmailAcct), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 0, 0 },
	{ "GmailSwitch", static_cast<short>SK_FIELD_OFFSET(IMGmailInfo_t, iGmailSwitch), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "GmailErrCode", static_cast<short>SK_FIELD_OFFSET(IMGmailInfo_t, iGmailErrCode), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMGmailList [] = {
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMGmailList_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "List", static_cast<short>SK_FIELD_OFFSET(IMGmailList_t, ptList), eTypeIMGmailInfo, 1, 0, -1,
		"Count", 2, sizeof(IMGmailInfo_t *), sizeof(IMGmailInfo_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMFacebookInfo [] = {
	{ "Uid", static_cast<short>SK_FIELD_OFFSET(IMFacebookInfo_t, llUid), eTypeSKUint64, 0, 1, -1,
		"", 1, sizeof(unsigned long long), sizeof(unsigned long long), 0, 0 },
	{ "Name", static_cast<short>SK_FIELD_OFFSET(IMFacebookInfo_t, pcName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "Token", static_cast<short>SK_FIELD_OFFSET(IMFacebookInfo_t, pcToken), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSafeDevice [] = {
	{ "Name", static_cast<short>SK_FIELD_OFFSET(IMSafeDevice_t, pcName), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 0, 0 },
	{ "Uuid", static_cast<short>SK_FIELD_OFFSET(IMSafeDevice_t, pcUuid), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "DeviceType", static_cast<short>SK_FIELD_OFFSET(IMSafeDevice_t, pcDeviceType), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 },
	{ "CreateTime", static_cast<short>SK_FIELD_OFFSET(IMSafeDevice_t, iCreateTime), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSafeDeviceList [] = {
	{ "Count", static_cast<short>SK_FIELD_OFFSET(IMSafeDeviceList_t, iCount), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "List", static_cast<short>SK_FIELD_OFFSET(IMSafeDeviceList_t, ptList), eTypeIMSafeDevice, 1, 0, -1,
		"Count", 2, sizeof(IMSafeDevice_t *), sizeof(IMSafeDevice_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMUserInfoExt [] = {
	{ "SnsUserInfo", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, tSnsUserInfo), eTypeIMSnsUserInfo, 0, 1, -1,
		"", 1, sizeof(IMSnsUserInfo_t), sizeof(IMSnsUserInfo_t), 0, 0 },
	{ "MyBrandList", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, pcMyBrandList), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "MsgPushSound", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, pcMsgPushSound), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 },
	{ "VoipPushSound", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, pcVoipPushSound), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 },
	{ "BigChatRoomSize", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, iBigChatRoomSize), eTypeSKUint32, 0, 0, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "BigChatRoomQuota", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, iBigChatRoomQuota), eTypeSKUint32, 0, 0, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "BigChatRoomInvite", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, iBigChatRoomInvite), eTypeSKUint32, 0, 0, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "SafeMobile", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, pcSafeMobile), eTypeSKChar, 1, 0, -1,
		"strlen", 8, sizeof(char *), sizeof(char), 0, 0 },
	{ "BigHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, pcBigHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 9, sizeof(char *), sizeof(char), 0, 0 },
	{ "SmallHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, pcSmallHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 0, 0 },
	{ "MainAcctType", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, iMainAcctType), eTypeSKUint32, 0, 0, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ExtXml", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, tExtXml), eTypeSKBuiltinString, 0, 0, -1,
		"", 12, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "SafeDeviceList", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, tSafeDeviceList), eTypeIMSafeDeviceList, 0, 0, -1,
		"", 13, sizeof(IMSafeDeviceList_t), sizeof(IMSafeDeviceList_t), 0, 0 },
	{ "SafeDevice", static_cast<short>SK_FIELD_OFFSET(IMUserInfoExt_t, iSafeDevice), eTypeSKUint32, 0, 0, -1,
		"", 14, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMModUserInfo [] = {
	{ "BitFlag", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iBitFlag), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, tNickName), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "BindUin", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iBindUin), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "BindEmail", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, tBindEmail), eTypeSKBuiltinString, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "BindMobile", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, tBindMobile), eTypeSKBuiltinString, 0, 1, -1,
		"", 6, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "Status", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iStatus), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ImgLen", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iImgLen), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcImgBuf), eTypeSKChar, 1, 0, -1,
		"ImgLen", 9, sizeof(char *), sizeof(char), 1, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iSex), eTypeSKInt32, 0, 0, -1,
		"", 10, sizeof(int), sizeof(int), 0, 0 },
	{ "Province", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcProvince), eTypeSKChar, 1, 0, -1,
		"strlen", 11, sizeof(char *), sizeof(char), 0, 0 },
	{ "City", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcCity), eTypeSKChar, 1, 0, -1,
		"strlen", 12, sizeof(char *), sizeof(char), 0, 0 },
	{ "Signature", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcSignature), eTypeSKChar, 1, 0, -1,
		"strlen", 13, sizeof(char *), sizeof(char), 0, 0 },
	{ "PersonalCard", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iPersonalCard), eTypeSKUint32, 0, 0, -1,
		"", 14, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "DisturbSetting", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, tDisturbSetting), eTypeIMDisturbSetting, 0, 0, -1,
		"", 15, sizeof(IMDisturbSetting_t), sizeof(IMDisturbSetting_t), 0, 0 },
	{ "PluginFlag", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iPluginFlag), eTypeSKUint32, 0, 0, -1,
		"", 16, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyFlag", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iVerifyFlag), eTypeSKUint32, 0, 0, -1,
		"", 17, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyInfo", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcVerifyInfo), eTypeSKChar, 1, 0, -1,
		"strlen", 18, sizeof(char *), sizeof(char), 0, 0 },
	{ "Point", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iPoint), eTypeSKInt32, 0, 0, -1,
		"", 19, sizeof(int), sizeof(int), 0, 0 },
	{ "Experience", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iExperience), eTypeSKInt32, 0, 0, -1,
		"", 20, sizeof(int), sizeof(int), 0, 0 },
	{ "Level", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iLevel), eTypeSKInt32, 0, 0, -1,
		"", 21, sizeof(int), sizeof(int), 0, 0 },
	{ "LevelLowExp", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iLevelLowExp), eTypeSKInt32, 0, 0, -1,
		"", 22, sizeof(int), sizeof(int), 0, 0 },
	{ "LevelHighExp", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iLevelHighExp), eTypeSKInt32, 0, 0, -1,
		"", 23, sizeof(int), sizeof(int), 0, 0 },
	{ "Weibo", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcWeibo), eTypeSKChar, 1, 0, -1,
		"strlen", 24, sizeof(char *), sizeof(char), 0, 0 },
	{ "PluginSwitch", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iPluginSwitch), eTypeSKUint32, 0, 0, -1,
		"", 25, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "GmailList", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, tGmailList), eTypeIMGmailList, 0, 0, -1,
		"", 26, sizeof(IMGmailList_t), sizeof(IMGmailList_t), 0, 0 },
	{ "Alias", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcAlias), eTypeSKChar, 1, 0, -1,
		"strlen", 27, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboNickname", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcWeiboNickname), eTypeSKChar, 1, 0, -1,
		"strlen", 28, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboFlag", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iWeiboFlag), eTypeSKUint32, 0, 0, -1,
		"", 29, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "FaceBookFlag", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iFaceBookFlag), eTypeSKUint32, 0, 0, -1,
		"", 30, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "FBUserID", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, llFBUserID), eTypeSKUint64, 0, 0, -1,
		"", 31, sizeof(unsigned long long), sizeof(unsigned long long), 0, 0 },
	{ "FBUserName", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcFBUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 32, sizeof(char *), sizeof(char), 0, 0 },
	{ "AlbumStyle", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iAlbumStyle), eTypeSKInt32, 0, 0, -1,
		"", 33, sizeof(int), sizeof(int), 0, 0 },
	{ "AlbumFlag", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iAlbumFlag), eTypeSKInt32, 0, 0, -1,
		"", 34, sizeof(int), sizeof(int), 0, 0 },
	{ "AlbumBGImgID", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcAlbumBGImgID), eTypeSKChar, 1, 0, -1,
		"strlen", 35, sizeof(char *), sizeof(char), 0, 0 },
	{ "TXNewsCategory", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iTXNewsCategory), eTypeSKUint32, 0, 0, -1,
		"", 36, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "FBToken", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcFBToken), eTypeSKChar, 1, 0, -1,
		"strlen", 37, sizeof(char *), sizeof(char), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 38, sizeof(char *), sizeof(char), 0, 0 },
	{ "GrayscaleFlag", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iGrayscaleFlag), eTypeSKUint32, 0, 0, -1,
		"", 39, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	//add by yihuang
	{ "Birthday", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, tBirthday), eTypeIMBirthday, 0, 0, -1,
		"", 40, sizeof(IMBirthday_t), sizeof(IMBirthday_t), 0, 0 },
	{ "Age", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iAge), eTypeSKUint32, 0, 0, -1,
		"", 41, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Hobby", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, tHobby), eTypeIMHobby, 0, 0, -1,
		"", 42, sizeof(IMHobby_t), sizeof(IMHobby_t), 0, 0 },
	{ "IntentionFlag", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iIntentionFlag), eTypeSKUint32, 0, 0, -1,
		"", 43, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "PhotoCount", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iPhotoCount), eTypeSKUint32, 0, 0, -1,
		"", 44, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "FriendCount", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iFriendCount), eTypeSKUint32, 0, 0, -1,
		"", 45, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "GroupCount", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iGroupCount), eTypeSKUint32, 0, 0, -1,
		"", 46, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MatchCount", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iMatchCount), eTypeSKUint32, 0, 0, -1,
		"", 47, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "LBSVisibleState", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iLBSVisibleState), eTypeSKUint32, 0, 0, -1,
		"", 48, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "LBSDistanceUnit", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, iLBSDistanceUnit), eTypeSKUint32, 0, 0, -1,
		"", 49, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Language", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, pcLanguage), eTypeSKChar, 1, 0, -1,
		"strlen", 50, sizeof(char *), sizeof(char), 0, 0 },
	{ "FirstName", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, tFirstName), eTypeSKBuiltinString, 0, 0, -1,
		"", 51, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "LastName", static_cast<short>SK_FIELD_OFFSET(IMModUserInfo_t, tLastName), eTypeSKBuiltinString, 0, 0, -1,
		"", 52, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 }
	//
};

static const SKMetaField_t g_tMetaFieldListIMGetProfileRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMGetProfileRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMGetProfileRequest_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMGetProfileResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMGetProfileResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "UserInfo", static_cast<short>SK_FIELD_OFFSET(IMGetProfileResponse_t, tUserInfo), eTypeIMModUserInfo, 0, 1, -1,
		"", 2, sizeof(IMModUserInfo_t), sizeof(IMModUserInfo_t), 0, 0 },
	{ "UserInfoExt", static_cast<short>SK_FIELD_OFFSET(IMGetProfileResponse_t, tUserInfoExt), eTypeIMUserInfoExt, 0, 1, -1,
		"", 3, sizeof(IMUserInfoExt_t), sizeof(IMUserInfoExt_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMUploadCoverImgRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Data", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, tData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "ImgHash", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, pcImgHash), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 0, 0 },
	{ "MD5", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, pcMD5), eTypeSKChar, 1, 0, -1,
		"strlen", 8, sizeof(char *), sizeof(char), 0, 0 },
	{ "CDNImgUrl", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, pcCDNImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 9, sizeof(char *), sizeof(char), 1, 0 },
	{ "AESKey", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, pcAESKey), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 1, 0 },
	{ "EncryVer", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, iEncryVer), eTypeSKInt32, 0, 0, -1,
		"", 11, sizeof(int), sizeof(int), 1, 0 },
	{ "CDNImgSize", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgRequest_t, iCDNImgSize), eTypeSKInt32, 0, 0, -1,
		"", 12, sizeof(int), sizeof(int), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMUploadCoverImgResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgResponse_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgResponse_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "FinalImgMd5sum", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgResponse_t, pcFinalImgMd5sum), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 },
	{ "ImgHash", static_cast<short>SK_FIELD_OFFSET(IMUploadCoverImgResponse_t, pcImgHash), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMGetCoverImgRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgRequest_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "ImgWidth", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgRequest_t, iImgWidth), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ImgHeight", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgRequest_t, iImgHeight), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ImgFormat", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgRequest_t, pcImgFormat), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgRequest_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgRequest_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMGetCoverImgResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgResponse_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "TotalLen", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgResponse_t, iTotalLen), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "StartPos", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgResponse_t, iStartPos), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Data", static_cast<short>SK_FIELD_OFFSET(IMGetCoverImgResponse_t, tData), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMHobby [] = {
	{ "ArtFlag", static_cast<short>SK_FIELD_OFFSET(IMHobby_t, iArtFlag), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "SportsFlag", static_cast<short>SK_FIELD_OFFSET(IMHobby_t, iSportsFlag), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "SocialActivitiesFlag", static_cast<short>SK_FIELD_OFFSET(IMHobby_t, iSocialActivitiesFlag), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "TechnologyFlag", static_cast<short>SK_FIELD_OFFSET(IMHobby_t, iTechnologyFlag), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Lifestyle", static_cast<short>SK_FIELD_OFFSET(IMHobby_t, iLifestyleFlag), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMBirthday [] = {
	{ "Year", static_cast<short>SK_FIELD_OFFSET(IMBirthday_t, iYear), eTypeSKUint32, 0, 1, -1,
	"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Month", static_cast<short>SK_FIELD_OFFSET(IMBirthday_t, iMonth), eTypeSKUint32, 0, 1, -1,
	"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Day", static_cast<short>SK_FIELD_OFFSET(IMBirthday_t, iDay), eTypeSKUint32, 0, 1, -1,
	"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMFunctionSwitch [] = {
	{ "FunctionId", static_cast<short>SK_FIELD_OFFSET(IMFunctionSwitch_t, iFunctionId), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "SwitchValue", static_cast<short>SK_FIELD_OFFSET(IMFunctionSwitch_t, iSwitchValue), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};

static const SKMetaFunc_t g_tMetaIMFuncList [] = {
	{ "SKGetSelfSvnInfo", eFuncIMMsgSKGetSelfSvnInfo,
		{ "", 0, 0 }, { "Response", eTypeSKBuiltinString, 1 } },
	{ "SKGetMetainfoFull", eFuncIMMsgSKGetMetainfoFull,
		{ "", 0, 0 }, { "Response", eTypeSKBuiltinString, 1 } },
	{ "SKGetMetainfo", eFuncIMMsgSKGetMetainfo,
		{ "", 0, 0 }, { "Response", eTypeSKBuiltinString, 1 } },
	{ "SKEcho", eFuncIMMsgSKEcho,
		{ "Request", eTypeIMMsgEchoInfo, 1 }, { "Response", eTypeIMMsgEchoInfo, 1 } }
};

static const SKMetaField_t g_tMetaFieldListIMLbsRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "OpCode", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, iOpCode), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Longitude", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, fLongitude), eTypeSKFloat32, 0, 1, -1,
		"", 3, sizeof(float), sizeof(float), 0, 0 },
	{ "Latitude", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, fLatitude), eTypeSKFloat32, 0, 1, -1,
		"", 4, sizeof(float), sizeof(float), 0, 0 },
	{ "Precision", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, iPrecision), eTypeSKInt32, 0, 1, -1,
		"", 5, sizeof(int), sizeof(int), 0, 0 },
	{ "MacAddr", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, pcMacAddr), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 0, 0 },
	{ "CellId", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, pcCellId), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 0, 0 },
	{ "GPSSource", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, iGPSSource), eTypeSKInt32, 0, 0, -1,
		"", 8, sizeof(int), sizeof(int), 0, 0 },
	//add by yihuang
	{ "BeginAge", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, iBeginAge), eTypeSKInt32, 0, 0, -1,
		"", 9, sizeof(int), sizeof(int), 0, 0 },
	{ "EndAge", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, iEndAge), eTypeSKInt32, 0, 0, -1,
		"", 10, sizeof(int), sizeof(int), 0, 0 },
	{ "IntentionFlag", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, iIntentionFlag), eTypeSKInt32, 0, 0, -1,
		"", 11, sizeof(int), sizeof(int), 0, 0 },
	{ "SkipCount", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, iSkipCount), eTypeSKUint32, 0, 0, -1,
		"", 12, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "FetchCount", static_cast<short>SK_FIELD_OFFSET(IMLbsRequest_t, iFetchCount), eTypeSKUint32, 0, 0, -1,
		"", 13, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
	//
};

static const SKMetaField_t g_tMetaFieldListIMLbsContactInfo [] = {
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 0, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcNickName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "Province", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcProvince), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 },
	{ "City", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcCity), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 },
	{ "Signature", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcSignature), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "Distance", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcDistance), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 0, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, iSex), eTypeSKInt32, 0, 1, -1,
		"", 7, sizeof(int), sizeof(int), 0, 0 },
	{ "ImgStatus", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, iImgStatus), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyFlag", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, iVerifyFlag), eTypeSKUint32, 0, 0, -1,
		"", 9, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyInfo", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcVerifyInfo), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 0, 0 },
	{ "VerifyContent", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcVerifyContent), eTypeSKChar, 1, 0, -1,
		"strlen", 11, sizeof(char *), sizeof(char), 0, 0 },
	{ "Alias", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcAlias), eTypeSKChar, 1, 0, -1,
		"strlen", 12, sizeof(char *), sizeof(char), 0, 0 },
	{ "Weibo", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcWeibo), eTypeSKChar, 1, 0, -1,
		"strlen", 13, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboNickname", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcWeiboNickname), eTypeSKChar, 1, 0, -1,
		"strlen", 14, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboFlag", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, iWeiboFlag), eTypeSKUint32, 0, 0, -1,
		"", 15, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "HeadImgVersion", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, iHeadImgVersion), eTypeSKInt32, 0, 0, -1,
		"", 19, sizeof(int), sizeof(int), 0, 0 },
	{ "SnsUserInfo", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, tSnsUserInfo), eTypeIMSnsUserInfo, 0, 0, -1,
		"", 20, sizeof(IMSnsUserInfo_t), sizeof(IMSnsUserInfo_t), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 21, sizeof(char *), sizeof(char), 0, 0 },
	{ "BigHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcBigHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 22, sizeof(char *), sizeof(char), 0, 0 },
	{ "SmallHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcSmallHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 23, sizeof(char *), sizeof(char), 0, 0 },
	{ "MyBrandList", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcMyBrandList), eTypeSKChar, 1, 0, -1,
		"strlen", 24, sizeof(char *), sizeof(char), 0, 0 },
	{ "CustomizedInfo", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, tCustomizedInfo), eTypeIMCustomizedInfo, 0, 0, -1,
		"", 25, sizeof(IMCustomizedInfo_t), sizeof(IMCustomizedInfo_t), 0, 0 },
	{ "MutualHobbies", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, tMutualHobbies), eTypeIMHobby, 0, 0, -1,
		"", 26, sizeof(IMHobby_t), sizeof(IMHobby_t), 0, 0 },
	{ "MutualFriendCount", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, iMutualFriendCount), eTypeSKUint32, 0, 0, -1,
		"", 27, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MutualFriendList", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, ptMutualFriendList), eTypeIMLbsContactInfo, 1, 0, -1,
		"MutualFriendCount", 28, sizeof(IMLbsContactInfo_t *), sizeof(IMLbsContactInfo_t), 0, 0 },
	{ "MutualGroupCount", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, iMutualGroupCount), eTypeSKUint32, 0, 0, -1,
		"", 29, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MutualGroupList", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, ptMutualGroupList), eTypeIMLbsContactInfo, 1, 0, -1,
		"MutualGroupCount", 30, sizeof(IMLbsContactInfo_t *), sizeof(IMLbsContactInfo_t), 0, 0 },
	{ "Time", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcTime), eTypeSKChar, 1, 0, -1,
		"strlen", 31, sizeof(char *), sizeof(char), 0, 0 },
	{ "SafeUserName", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, pcSafeUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 32, sizeof(char *), sizeof(char), 0, 0 },
	{ "Birthday", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, tBirthday), eTypeIMBirthday, 0, 0, -1,
		"", 33, sizeof(IMBirthday_t), sizeof(IMBirthday_t), 0, 0 },
	{ "Age", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, iAge), eTypeSKUint32, 0, 0, -1,
		"", 34, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "IntentionFlag", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, iIntentionFlag), eTypeSKUint32, 0, 0, -1,
		"", 35, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MatchType", static_cast<short>SK_FIELD_OFFSET(IMLbsContactInfo_t, iMatchType), eTypeSKUint32, 0, 0, -1,
		"", 36, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMLbsResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMLbsResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "ContactCount", static_cast<short>SK_FIELD_OFFSET(IMLbsResponse_t, iContactCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ContactList", static_cast<short>SK_FIELD_OFFSET(IMLbsResponse_t, ptContactList), eTypeIMLbsContactInfo, 1, 0, -1,
		"ContactCount", 3, sizeof(IMLbsContactInfo_t *), sizeof(IMLbsContactInfo_t), 0, 0 },
	{ "State", static_cast<short>SK_FIELD_OFFSET(IMLbsResponse_t, iState), eTypeSKUint32, 0, 0, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "FlushTime", static_cast<short>SK_FIELD_OFFSET(IMLbsResponse_t, iFlushTime), eTypeSKUint32, 0, 0, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "IsShowRoom", static_cast<short>SK_FIELD_OFFSET(IMLbsResponse_t, iIsShowRoom), eTypeSKUint32, 0, 0, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "RoomMemberCount", static_cast<short>SK_FIELD_OFFSET(IMLbsResponse_t, iRoomMemberCount), eTypeSKUint32, 0, 0, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "SkipCount", static_cast<short>SK_FIELD_OFFSET(IMLbsResponse_t, iSkipCount), eTypeSKUint32, 0, 0, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMGetLocationRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMGetLocationRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "Longitude", static_cast<short>SK_FIELD_OFFSET(IMGetLocationRequest_t, iLongitude), eTypeSKInt32, 0, 1, -1,
		"", 3, sizeof(int), sizeof(int), 0, 0 },
	{ "Latitude", static_cast<short>SK_FIELD_OFFSET(IMGetLocationRequest_t, iLatitude), eTypeSKInt32, 0, 1, -1,
		"", 4, sizeof(int), sizeof(int), 0, 0 },
	{ "Precision", static_cast<short>SK_FIELD_OFFSET(IMGetLocationRequest_t, iPrecision), eTypeSKInt32, 0, 1, -1,
		"", 5, sizeof(int), sizeof(int), 0, 0 },
	{ "MacAddr", static_cast<short>SK_FIELD_OFFSET(IMGetLocationRequest_t, pcMacAddr), eTypeSKChar, 1, 0, -1,
		"strlen", 6, sizeof(char *), sizeof(char), 0, 0 },
	{ "CellId", static_cast<short>SK_FIELD_OFFSET(IMGetLocationRequest_t, pcCellId), eTypeSKChar, 1, 0, -1,
		"strlen", 7, sizeof(char *), sizeof(char), 0, 0 },
	{ "GPSSource", static_cast<short>SK_FIELD_OFFSET(IMGetLocationRequest_t, iGPSSource), eTypeSKInt32, 0, 0, -1,
		"", 8, sizeof(int), sizeof(int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMGetLocationResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMGetLocationResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "Longitude", static_cast<short>SK_FIELD_OFFSET(IMGetLocationResponse_t, iLongitude), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 0, 0 },
	{ "Latitude", static_cast<short>SK_FIELD_OFFSET(IMGetLocationResponse_t, iLatitude), eTypeSKInt32, 0, 1, -1,
		"", 3, sizeof(int), sizeof(int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMLbsMatchRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "OpCode", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchRequest_t, iOpCode), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MatchListSize", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchRequest_t, iMatchListSize), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserNameList", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchRequest_t, pUserNameList), eTypeSKBuiltinString, 1, 0, -1,
		"MatchListSize", 4, sizeof(SKBuiltinString_t*), sizeof(SKBuiltinString_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMLbsMatchResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "OpCode", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchResponse_t, iOpCode), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MatchListSize", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchResponse_t, iMatchListSize), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserNameList", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchResponse_t, pUserNameList), eTypeSKBuiltinString, 1, 0, -1,
		"MatchListSize", 4, sizeof(SKBuiltinString_t*), sizeof(SKBuiltinString_t), 0, 0 },
	{ "Ret", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchResponse_t, piRet), eTypeSKInt32, 1, 0, -1,
		"MatchListSize", 5, sizeof(int *), sizeof(int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSyncLbsMatchRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMSyncLbsMatchRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMSyncLbsMatchRequest_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },
	{ "SyncKey", static_cast<short>SK_FIELD_OFFSET(IMSyncLbsMatchRequest_t, iSyncKey), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSyncLbsMatchResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMSyncLbsMatchResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "NewSyncKey", static_cast<short>SK_FIELD_OFFSET(IMSyncLbsMatchResponse_t, iNewSyncKey), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "ContinueFlag", static_cast<short>SK_FIELD_OFFSET(IMSyncLbsMatchResponse_t, iContinueFlag), eTypeSKInt32, 0, 1, -1,
		"", 3, sizeof(int), sizeof(int), 1, 0 },
	{ "CmdCount", static_cast<short>SK_FIELD_OFFSET(IMSyncLbsMatchResponse_t, iCmdCount), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "CmdList", static_cast<short>SK_FIELD_OFFSET(IMSyncLbsMatchResponse_t, ptCmdList), eTypeIMCmdItem, 1, 0, -1,
		"CmdCount", 5, sizeof(IMCmdItem_t *), sizeof(IMCmdItem_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMLbsMatchContactInfo [] = {
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 0, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, pcNickName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "Province", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, pcProvince), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 },
	{ "City", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, pcCity), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 },
	{ "Signature", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, pcSignature), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, iSex), eTypeSKInt32, 0, 1, -1,
		"", 6, sizeof(int), sizeof(int), 0, 0 },
	{ "ImgStatus", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, iImgStatus), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "HeadImgVersion", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, iHeadImgVersion), eTypeSKInt32, 0, 0, -1,
		"", 8, sizeof(int), sizeof(int), 0, 0 },
	{ "SnsUserInfo", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, tSnsUserInfo), eTypeIMSnsUserInfo, 0, 0, -1,
		"", 9, sizeof(IMSnsUserInfo_t), sizeof(IMSnsUserInfo_t), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 0, 0 },
	{ "BigHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, pcBigHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 11, sizeof(char *), sizeof(char), 0, 0 },
	{ "SmallHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, pcSmallHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 12, sizeof(char *), sizeof(char), 0, 0 },
	{ "SafeUserName", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, pcSafeUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 13, sizeof(char *), sizeof(char), 0, 0 },
	{ "Birthday", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, tBirthday), eTypeIMBirthday, 0, 0, -1,
		"", 14, sizeof(IMBirthday_t), sizeof(IMBirthday_t), 0, 0 },
	{ "Age", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, iAge), eTypeSKUint32, 0, 0, -1,
		"", 15, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "IntentionFlag", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, iIntentionFlag), eTypeSKUint32, 0, 0, -1,
		"", 16, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MatchTime", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, iMatchTime), eTypeSKUint32, 0, 0, -1,
		"", 17, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Hobbies", static_cast<short>SK_FIELD_OFFSET(IMLbsMatchContactInfo_t, tHobbies), eTypeIMHobby, 0, 0, -1,
		"", 18, sizeof(IMHobby_t), sizeof(IMHobby_t), 0, 0 },
};

////////////////////////////// »∫œ‡πÿBegin //////////////////////////////////


static const SKMetaField_t g_tMetaFieldListIMModChatRoomNotify [] = {
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomNotify_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Status", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomNotify_t, iStatus), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMModChatRoomMember [] = {
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, tNickName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "PYInitial", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, tPYInitial), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "QuanPin", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, tQuanPin), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, iSex), eTypeSKInt32, 0, 1, -1,
		"", 5, sizeof(int), sizeof(int), 0, 0 },
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, tImgBuf), eTypeSKBuiltinString, 0, 1, -1,
		"", 6, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "ImgFlag", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, iImgFlag), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Remark", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, tRemark), eTypeSKBuiltinString, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "RemarkPYInitial", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, tRemarkPYInitial), eTypeSKBuiltinString, 0, 1, -1,
		"", 9, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "RemarkQuanPin", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, tRemarkQuanPin), eTypeSKBuiltinString, 0, 1, -1,
		"", 10, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "ContactType", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, iContactType), eTypeSKUint32, 0, 1, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Province", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcProvince), eTypeSKChar, 1, 0, -1,
		"strlen", 12, sizeof(char *), sizeof(char), 0, 0 },
	{ "City", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcCity), eTypeSKChar, 1, 0, -1,
		"strlen", 13, sizeof(char *), sizeof(char), 0, 0 },
	{ "Signature", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcSignature), eTypeSKChar, 1, 0, -1,
		"strlen", 14, sizeof(char *), sizeof(char), 0, 0 },
	{ "PersonalCard", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, iPersonalCard), eTypeSKUint32, 0, 1, -1,
		"", 15, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyFlag", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, iVerifyFlag), eTypeSKUint32, 0, 1, -1,
		"", 16, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyInfo", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcVerifyInfo), eTypeSKChar, 1, 0, -1,
		"strlen", 17, sizeof(char *), sizeof(char), 0, 0 },
	{ "Weibo", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcWeibo), eTypeSKChar, 1, 0, -1,
		"strlen", 18, sizeof(char *), sizeof(char), 0, 0 },
	{ "VerifyContent", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcVerifyContent), eTypeSKChar, 1, 0, -1,
		"strlen", 19, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboNickname", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcWeiboNickname), eTypeSKChar, 1, 0, -1,
		"strlen", 20, sizeof(char *), sizeof(char), 0, 0 },
	{ "WeiboFlag", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, iWeiboFlag), eTypeSKUint32, 0, 1, -1,
		"", 21, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "AlbumStyle", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, iAlbumStyle), eTypeSKInt32, 0, 1, -1,
		"", 22, sizeof(int), sizeof(int), 0, 0 },
	{ "AlbumFlag", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, iAlbumFlag), eTypeSKInt32, 0, 1, -1,
		"", 23, sizeof(int), sizeof(int), 0, 0 },
	{ "AlbumBGImgID", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcAlbumBGImgID), eTypeSKChar, 1, 0, -1,
		"strlen", 24, sizeof(char *), sizeof(char), 0, 0 },
	{ "SnsUserInfo", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, tSnsUserInfo), eTypeIMSnsUserInfo, 0, 1, -1,
		"", 25, sizeof(IMSnsUserInfo_t), sizeof(IMSnsUserInfo_t), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 26, sizeof(char *), sizeof(char), 0, 0 },
	{ "BigHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcBigHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 27, sizeof(char *), sizeof(char), 0, 0 },
	{ "SmallHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcSmallHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 28, sizeof(char *), sizeof(char), 0, 0 },
	{ "MyBrandList", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, pcMyBrandList), eTypeSKChar, 1, 0, -1,
		"strlen", 29, sizeof(char *), sizeof(char), 0, 0 },
	{ "CustomizedInfo", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMember_t, tCustomizedInfo), eTypeIMCustomizedInfo, 0, 1, -1,
		"", 30, sizeof(IMCustomizedInfo_t), sizeof(IMCustomizedInfo_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMMobileOptRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMMobileOptRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "CountryCode", static_cast<short>SK_FIELD_OFFSET(IMMobileOptRequest_t, pcCountryCode), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
	{ "Mobile", static_cast<short>SK_FIELD_OFFSET(IMMobileOptRequest_t, pcMobile), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
	{ "OptCode", static_cast<short>SK_FIELD_OFFSET(IMMobileOptRequest_t, iOptCode), eTypeSKInt32, 0, 1, -1,
		"", 4, sizeof(int), sizeof(int), 1, 0 },
	{ "Verifycode", static_cast<short>SK_FIELD_OFFSET(IMMobileOptRequest_t, pcVerifycode), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 1, 0 },
	{ "RandomEncryKey", static_cast<short>SK_FIELD_OFFSET(IMMobileOptRequest_t, tRandomEncryKey), eTypeSKBuiltinString, 0, 1, -1,
		"", 6, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
};

static const SKMetaField_t g_tMetaFieldListIMMobileOptResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMMobileOptResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "OptCode", static_cast<short>SK_FIELD_OFFSET(IMMobileOptResponse_t, iOptCode), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 1, 0 },
	{ "Ticket", static_cast<short>SK_FIELD_OFFSET(IMMobileOptResponse_t, pcTicket), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMEmailOptRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMEmailOptRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "Email", static_cast<short>SK_FIELD_OFFSET(IMEmailOptRequest_t, pcEmail), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
	{ "OptCode", static_cast<short>SK_FIELD_OFFSET(IMEmailOptRequest_t, iOptCode), eTypeSKInt32, 0, 1, -1,
		"", 3, sizeof(int), sizeof(int), 1, 0 },
	{ "VerifyCode", static_cast<short>SK_FIELD_OFFSET(IMEmailOptRequest_t, pcVerifyCode), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "RandomEncryKey", static_cast<short>SK_FIELD_OFFSET(IMEmailOptRequest_t, tRandomEncryKey), eTypeSKBuiltinString, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
};

static const SKMetaField_t g_tMetaFieldListIMEmailOptResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMEmailOptResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
	{ "OptCode", static_cast<short>SK_FIELD_OFFSET(IMEmailOptResponse_t, iOptCode), eTypeSKInt32, 0, 1, -1,
		"", 2, sizeof(int), sizeof(int), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMUploadPlugFriendsRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMUploadPlugFriendsRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "OptCode", static_cast<short>SK_FIELD_OFFSET(IMUploadPlugFriendsRequest_t, iOptCode), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },	
	{ "FriendType", static_cast<short>SK_FIELD_OFFSET(IMUploadPlugFriendsRequest_t, iFriendType), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "FriendCount", static_cast<short>SK_FIELD_OFFSET(IMUploadPlugFriendsRequest_t, iFriendCount), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 1, 0 },
	{ "FriendBuffer", static_cast<short>SK_FIELD_OFFSET(IMUploadPlugFriendsRequest_t, pcFriendBuffer), eTypeSKBuiltinString, 1, 0, -1,
		"FriendCount", 5, sizeof(Comm::SKBuiltinString_t *), sizeof(Comm::SKBuiltinString_t), 0, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMUploadPlugFriendsResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMUploadPlugFriendsResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 },
};


static const SKMetaField_t g_tMetaFieldListIMQuitChatRoom [] = {
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMQuitChatRoom_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMQuitChatRoom_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMQuitChatRoomRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMQuitChatRoomRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "QuitChatRoom", static_cast<short>SK_FIELD_OFFSET(IMQuitChatRoomRequest_t, tQuitChatRoom), eTypeIMQuitChatRoom, 0, 1, -1,
		"", 2, sizeof(IMQuitChatRoom_t), sizeof(IMQuitChatRoom_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMQuitChatRoomResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMQuitChatRoomResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "QuitChatRoom", static_cast<short>SK_FIELD_OFFSET(IMQuitChatRoomResponse_t, tQuitChatRoom), eTypeIMQuitChatRoom, 0, 1, -1,
		"", 2, sizeof(IMQuitChatRoom_t), sizeof(IMQuitChatRoom_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMQuitGroupRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMQuitGroupRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "QuitChatRoom", static_cast<short>SK_FIELD_OFFSET(IMQuitGroupRequest_t, tQuitChatRoom), eTypeIMQuitChatRoom, 0, 1, -1,
		"", 2, sizeof(IMQuitChatRoom_t), sizeof(IMQuitChatRoom_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMQuitGroupResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMQuitGroupResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "QuitChatRoom", static_cast<short>SK_FIELD_OFFSET(IMQuitGroupResponse_t, tQuitChatRoom), eTypeIMQuitChatRoom, 0, 1, -1,
		"", 2, sizeof(IMQuitChatRoom_t), sizeof(IMQuitChatRoom_t), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMModChatRoomTopic [] = {
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomTopic_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ChatRoomType", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomTopic_t, iChatRoomType), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ChatRoomName", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomTopic_t, tChatRoomName), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "ChatRoomTopic", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomTopic_t, tChatRoomTopic), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "NeedVerify", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomTopic_t, iNeedVerify), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMModGroupTopic [] = {
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMModGroupTopic_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ChatRoomTopic", static_cast<short>SK_FIELD_OFFSET(IMModGroupTopic_t, tChatRoomTopic), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMModChatRoomMemberDisplayName [] = {
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMemberDisplayName_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMemberDisplayName_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "DisplayName", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMemberDisplayName_t, pcDisplayName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMModChatRoomMemberFlag [] = {
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMemberFlag_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMemberFlag_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "FlagSwitch", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMemberFlag_t, iFlagSwitch), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Value", static_cast<short>SK_FIELD_OFFSET(IMModChatRoomMemberFlag_t, iValue), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMMemberReq [] = {
	{ "MemberName", static_cast<short>SK_FIELD_OFFSET(IMMemberReq_t, tMemberName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMMemberResp [] = {
	{ "MemberName", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, tMemberName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "MemberStatus", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, iMemberStatus), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "NickName", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, tNickName), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "PYInitial", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, tPYInitial), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "QuanPin", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, tQuanPin), eTypeSKBuiltinString, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "Sex", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, iSex), eTypeSKInt32, 0, 1, -1,
		"", 6, sizeof(int), sizeof(int), 0, 0 },
	{ "Remark", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, tRemark), eTypeSKBuiltinString, 0, 1, -1,
		"", 7, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "RemarkPYInitial", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, tRemarkPYInitial), eTypeSKBuiltinString, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "RemarkQuanPin", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, tRemarkQuanPin), eTypeSKBuiltinString, 0, 1, -1,
		"", 9, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "ContactType", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, iContactType), eTypeSKUint32, 0, 1, -1,
		"", 10, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Province", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, pcProvince), eTypeSKChar, 1, 0, -1,
		"strlen", 11, sizeof(char *), sizeof(char), 0, 0 },
	{ "City", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, pcCity), eTypeSKChar, 1, 0, -1,
		"strlen", 12, sizeof(char *), sizeof(char), 0, 0 },
	{ "Signature", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, pcSignature), eTypeSKChar, 1, 0, -1,
		"strlen", 13, sizeof(char *), sizeof(char), 0, 0 },
	{ "PersonalCard", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, iPersonalCard), eTypeSKUint32, 0, 1, -1,
		"", 14, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyFlag", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, iVerifyFlag), eTypeSKUint32, 0, 1, -1,
		"", 15, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VerifyInfo", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, pcVerifyInfo), eTypeSKChar, 1, 0, -1,
		"strlen", 16, sizeof(char *), sizeof(char), 0, 0 },
	{ "Country", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, pcCountry), eTypeSKChar, 1, 0, -1,
		"strlen", 17, sizeof(char *), sizeof(char), 0, 0 },
	{ "MemberSafeName", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, tMemberSafeName), eTypeSKBuiltinString, 0, 1, -1,
		"", 18, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "Birthday", static_cast<short>SK_FIELD_OFFSET(IMMemberResp_t, tBirthday), eTypeIMBirthday, 0, 0, -1,
		"", 19, sizeof(IMBirthday_t), sizeof(IMBirthday_t), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMDelMemberReq [] = {
	{ "MemberName", static_cast<short>SK_FIELD_OFFSET(IMDelMemberReq_t, tMemberName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMDelMemberResp [] = {
	{ "MemberName", static_cast<short>SK_FIELD_OFFSET(IMDelMemberResp_t, tMemberName), eTypeSKBuiltinString, 0, 1, -1,
		"", 1, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMCreateChatRoomRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "IntroDuce", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomRequest_t, tIntroDuce), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomRequest_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomRequest_t, ptMemberList), eTypeIMMemberReq, 1, 0, -1,
		"MemberCount", 4, sizeof(IMMemberReq_t*), sizeof(IMMemberReq_t), 0, 0 },
	{ "Longitude", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomRequest_t, fLongitude), eTypeSKFloat32, 0, 1, -1,
		"", 5, sizeof(float), sizeof(float), 0, 0 },
	{ "Latitude", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomRequest_t, fLatitude), eTypeSKFloat32, 0, 1, -1,
		"", 6, sizeof(float), sizeof(float), 0, 0 },
	{ "ChatRoomName", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomRequest_t, tChatRoomName), eTypeSKBuiltinString, 0, 1, -1,
		"", 7, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "Type", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomRequest_t, iType), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "NeedVerify", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomRequest_t, iNeedVerify), eTypeSKUint32, 0, 1, -1,
		"", 9, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "tChatRoomAddr", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomRequest_t, tChatRoomAddr), eTypeSKBuiltinString, 0, 1, -1,
		"", 10, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMCreateChatRoomResponse[] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "IntroDuce", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, tIntroDuce), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "PYInitial", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, tPYInitial), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "QuanPin", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, tQuanPin), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, ptMemberList), eTypeIMMemberResp, 1, 0, -1,
		"MemberCount", 6, sizeof(IMMemberResp_t*), sizeof(IMMemberResp_t), 0, 0 },
	{ "ChatRoomName", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, tChatRoomName), eTypeSKBuiltinString, 0, 1, -1,
		"", 7, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, tImgBuf), eTypeSKBuiltinBuffer, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinBuffer_t), sizeof(Comm::SKBuiltinBuffer_t), 0, 0 },
	{ "BigHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, pcBigHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 9, sizeof(char *), sizeof(char), 0, 0 },
	{ "SmallHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, pcSmallHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 0, 0 },
	{ "RoomId", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, iRoomId), eTypeSKUint32, 0, 1, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Type", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, iType), eTypeSKUint32, 0, 1, -1,
		"", 12, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "NeedVerify", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, iNeedVerify), eTypeSKUint32, 0, 1, -1,
		"", 13, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MaxMemberCount", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, iMaxMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 14, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "tChatRoomAddr", static_cast<short>SK_FIELD_OFFSET(IMCreateChatRoomResponse_t, tChatRoomAddr), eTypeSKBuiltinString, 0, 1, -1,
		"", 15, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMCreateGroupRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "IntroDuce", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupRequest_t, tIntroDuce), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupRequest_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupRequest_t, ptMemberList), eTypeIMMemberReq, 1, 0, -1,
		"MemberCount", 4, sizeof(IMMemberReq_t*), sizeof(IMMemberReq_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMCreateGroupResponse[] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "IntroDuce", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, tIntroDuce), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "PYInitial", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, tPYInitial), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "QuanPin", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, tQuanPin), eTypeSKBuiltinString, 0, 1, -1,
		"", 4, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, ptMemberList), eTypeIMMemberResp, 1, 0, -1,
		"MemberCount", 6, sizeof(IMMemberResp_t*), sizeof(IMMemberResp_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ImgBuf", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, tImgBuf), eTypeSKBuiltinString, 0, 1, -1,
		"", 8, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "BigHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, pcBigHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 9, sizeof(char *), sizeof(char), 0, 0 },
	{ "SmallHeadImgUrl", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, pcSmallHeadImgUrl), eTypeSKChar, 1, 0, -1,
		"strlen", 10, sizeof(char *), sizeof(char), 0, 0 },
	{ "MaxMemberCount", static_cast<short>SK_FIELD_OFFSET(IMCreateGroupResponse_t, iMaxMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};



static const SKMetaField_t g_tMetaFieldListIMSearchChatRoomRequest[] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMSearchChatRoomRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "SearchType", static_cast<short>SK_FIELD_OFFSET(IMSearchChatRoomRequest_t, iSearchType), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMSearchChatRoomRequest_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Longitude", static_cast<short>SK_FIELD_OFFSET(IMSearchChatRoomRequest_t, fLongitude), eTypeSKFloat32, 0, 1, -1,
		"", 4, sizeof(float), sizeof(float), 0, 0 },
	{ "Latitude", static_cast<short>SK_FIELD_OFFSET(IMSearchChatRoomRequest_t, fLatitude), eTypeSKFloat32, 0, 1, -1,
		"", 5, sizeof(float), sizeof(float), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMChatRoomResult[] = {
	{ "RoomID", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iRoomID), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ChatRoomName", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, sChatRoomName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 0, 0 },
	{ "Introduce", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, sIntroduce), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 },
	{ "CreateTime", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iCreateTime), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ActiveTime", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iActiveTime), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Status", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iStatus), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Seq", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iSeq), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Owner", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iOwner), eTypeSKUint32, 0, 1, -1,
		"", 8, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "OwnerIDC", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iOwnerIDC), eTypeSKUint32, 0, 1, -1,
		"", 9, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UpdateTime", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iUpdateTime), eTypeSKUint32, 0, 1, -1,
		"", 10, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Type", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iType), eTypeSKUint32, 0, 1, -1,
		"", 11, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Dis", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iDis), eTypeSKUint32, 0, 1, -1,
		"", 12, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MaxMemberCount", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iMaxMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 13, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "NeedVerify", static_cast<short>SK_FIELD_OFFSET(IMChatRoomResult_t, iNeedVerify), eTypeSKUint32, 0, 1, -1,
		"", 14, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMSearchChatRoomResponse[] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMSearchChatRoomResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "ChatRoomResult", static_cast<short>SK_FIELD_OFFSET(IMSearchChatRoomResponse_t, tChatRoomResult), eTypeIMChatRoomResult, 0, 1, -1,
		"", 2, sizeof(IMChatRoomResult_t), sizeof(IMChatRoomResult_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMSearchChatRoomResponse_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMSearchChatRoomResponse_t, ptMemberList), eTypeIMChatRoomMember, 1, 0, -1,
		"MemberCount", 4, sizeof(IMChatRoomMember_t *), sizeof(IMChatRoomMember_t), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMAddChatRoomMemberRequest[] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMAddChatRoomMemberRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMAddChatRoomMemberRequest_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMAddChatRoomMemberRequest_t, ptMemberList), eTypeIMMemberReq, 1, 0, -1,
		"MemberCount", 3, sizeof(IMMemberReq_t*), sizeof(IMMemberReq_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMAddChatRoomMemberRequest_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMAddChatRoomMemberResponse[] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMAddChatRoomMemberResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMAddChatRoomMemberResponse_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMAddChatRoomMemberResponse_t, ptMemberList), eTypeIMMemberResp, 1, 0, -1,
		"MemberCount", 3, sizeof(IMMemberResp_t*), sizeof(IMMemberResp_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMAddChatRoomMemberResponse_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMAddGroupMemberRequest[] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMAddGroupMemberRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMAddGroupMemberRequest_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMAddGroupMemberRequest_t, ptMemberList), eTypeIMMemberReq, 1, 0, -1,
		"MemberCount", 3, sizeof(IMMemberReq_t*), sizeof(IMMemberReq_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMAddGroupMemberRequest_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMAddGroupMemberResponse[] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMAddGroupMemberResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMAddGroupMemberResponse_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMAddGroupMemberResponse_t, ptMemberList), eTypeIMMemberResp, 1, 0, -1,
		"MemberCount", 3, sizeof(IMMemberResp_t*), sizeof(IMMemberResp_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMAddGroupMemberResponse_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};



static const SKMetaField_t g_tMetaFieldListIMDelChatRoomMemberRequest[] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMDelChatRoomMemberRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMDelChatRoomMemberRequest_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMDelChatRoomMemberRequest_t, ptMemberList), eTypeIMDelMemberReq, 1, 0, -1,
		"MemberCount", 3, sizeof(IMDelMemberReq_t*), sizeof(IMDelMemberReq_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMDelChatRoomMemberRequest_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDelChatRoomMemberResponse[] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMDelChatRoomMemberResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMDelChatRoomMemberResponse_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMDelChatRoomMemberResponse_t, ptMemberList), eTypeIMDelMemberResp, 1, 0, -1,
		"MemberCount", 3, sizeof(IMDelMemberResp_t*), sizeof(IMDelMemberResp_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMDelChatRoomMemberResponse_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDelGroupMemberRequest[] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMDelGroupMemberRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMDelGroupMemberRequest_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMDelGroupMemberRequest_t, ptMemberList), eTypeIMDelMemberReq, 1, 0, -1,
		"MemberCount", 3, sizeof(IMDelMemberReq_t*), sizeof(IMDelMemberReq_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMDelGroupMemberRequest_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMDelGroupMemberResponse[] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMDelGroupMemberResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMDelGroupMemberResponse_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMDelGroupMemberResponse_t, ptMemberList), eTypeIMDelMemberResp, 1, 0, -1,
		"MemberCount", 3, sizeof(IMDelMemberResp_t*), sizeof(IMDelMemberResp_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMDelGroupMemberResponse_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMLbsChatRoomRequest[] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoomRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "Longitude", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoomRequest_t, fLongitude), eTypeSKFloat32, 0, 1, -1,
		"", 2, sizeof(float), sizeof(float), 0, 0 },
	{ "Latitude", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoomRequest_t, fLatitude), eTypeSKFloat32, 0, 1, -1,
		"", 3, sizeof(float), sizeof(float), 0, 0 },
	{ "GPSSource", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoomRequest_t, iGPSSource), eTypeSKInt32, 0, 0, -1,
		"", 4, sizeof(int), sizeof(int), 0, 0 },
	{ "SkipCount", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoomRequest_t, iSkipCount), eTypeSKUint32, 0, 0, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "FetchCount", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoomRequest_t, iFetchCount), eTypeSKUint32, 0, 0, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMLbsChatRoom [] = {
	{ "ChatRoomInfo", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoom_t, tChatRoomInfo), eTypeIMChatRoomResult, 0, 1, -1,
		"", 1, sizeof(IMChatRoomResult_t), sizeof(IMChatRoomResult_t), 0, 0 },
	{ "MemberCount", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoom_t, iMemberCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "MemberList", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoom_t, ptMemberList), eTypeIMChatRoomMember, 1, 0, -1,
		"MemberCount", 3, sizeof(IMChatRoomMember_t *), sizeof(IMChatRoomMember_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMLbsChatRoomResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoomResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "ChatRoomCount", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoomResponse_t, iChatRoomCount), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ChatRoomList", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoomResponse_t, tChatRoomList), eTypeIMLbsChatRoom, 1, 0, -1,
		"ChatRoomCount", 3, sizeof(IMLbsChatRoom_t *), sizeof(IMLbsChatRoom_t), 0, 0 },
	{ "SkipCount", static_cast<short>SK_FIELD_OFFSET(IMLbsChatRoomResponse_t, iSkipCount), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMVerifyChatRoomMemberRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMVerifyChatRoomMemberRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "Opcode", static_cast<short>SK_FIELD_OFFSET(IMVerifyChatRoomMemberRequest_t, iOpcode), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Name", static_cast<short>SK_FIELD_OFFSET(IMVerifyChatRoomMemberRequest_t, pcName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 },
	{ "VerifyMemberTicket", static_cast<short>SK_FIELD_OFFSET(IMVerifyChatRoomMemberRequest_t, pcVerifyMemberTicket), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 },
	{ "VerifyContent", static_cast<short>SK_FIELD_OFFSET(IMVerifyChatRoomMemberRequest_t, pcVerifyContent), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMVerifyChatRoomMemberRequest_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMVerifyChatRoomMemberResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMVerifyChatRoomMemberResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "Opcode", static_cast<short>SK_FIELD_OFFSET(IMVerifyChatRoomMemberResponse_t, iOpcode), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Name", static_cast<short>SK_FIELD_OFFSET(IMVerifyChatRoomMemberResponse_t, pcName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMVerifyChatRoomMemberResponse_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMInviteChatRoomMemberRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "Opcode", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberRequest_t, iOpcode), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Name", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberRequest_t, pcName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 },
	{ "InviteMemberTicket", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberRequest_t, pcInviteMemberTicket), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 0, 0 },
	{ "InviteContent", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberRequest_t, pcInviteContent), eTypeSKChar, 1, 0, -1,
		"strlen", 5, sizeof(char *), sizeof(char), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberRequest_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 6, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserCount", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberRequest_t, iUserCount), eTypeSKUint32, 0, 1, -1,
		"", 7, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserList", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberRequest_t, ptUserList), eTypeSKBuiltinString, 1, 0, -1,
		"UserCount", 8, sizeof(Comm::SKBuiltinString_t*), sizeof(Comm::SKBuiltinString_t), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMInviteChatRoomMemberResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "Opcode", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberResponse_t, iOpcode), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Name", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberResponse_t, pcName), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberResponse_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserCount", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberResponse_t, iUserCount), eTypeSKUint32, 0, 1, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "UserList", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberResponse_t, ptUserList), eTypeSKBuiltinString, 1, 0, -1,
		"UserCount", 6, sizeof(Comm::SKBuiltinString_t*), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "StatusList", static_cast<short>SK_FIELD_OFFSET(IMInviteChatRoomMemberResponse_t, piStatusList), eTypeSKInt32, 1, 0, -1,
		"UserCount", 7, sizeof(int*), sizeof(int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMCloseChatRoomRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMCloseChatRoomRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMCloseChatRoomRequest_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "Pwd", static_cast<short>SK_FIELD_OFFSET(IMCloseChatRoomRequest_t, tPwd), eTypeSKBuiltinString, 0, 1, -1,
		"", 3, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMCloseChatRoomResponse[] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMCloseChatRoomResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 0, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMCloseChatRoomResponse_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};


//////////////////////////// »∫œ‡πÿEnd /////////////////////////////////////


//////////////////////////// ”¶”√…Ë÷√ /////////////////////////////////////

static const SKMetaField_t g_tMetaFieldListIMPushSettingRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMPushSettingRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "OptCode", static_cast<short>SK_FIELD_OFFSET(IMPushSettingRequest_t, iOptCode), eTypeSKUint32, 0, 0, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },	
	{ "Token", static_cast<short>SK_FIELD_OFFSET(IMPushSettingRequest_t, pcToken), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
	{ "Sound", static_cast<short>SK_FIELD_OFFSET(IMPushSettingRequest_t, pcSound), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "Status", static_cast<short>SK_FIELD_OFFSET(IMPushSettingRequest_t, iStatus), eTypeSKUint32, 0, 0, -1,
		"", 5, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "VoipSound", static_cast<short>SK_FIELD_OFFSET(IMPushSettingRequest_t, pcVoipSound), eTypeSKChar, 1, 0, -1,
        "strlen", 6, sizeof(char *), sizeof(char), 0, 0 },
    { "isDev", static_cast<short>SK_FIELD_OFFSET(IMPushSettingRequest_t, isDev), eTypeSKUint32, 0, 1, -1,
        "", 7, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
    
};

static const SKMetaField_t g_tMetaFieldListIMPushSettingResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMPushSettingResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMModUserNameRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMModUserNameRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMModUserNameRequest_t, pcUserName), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMModUserNameResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMModUserNameResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMModPwdRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMModPwdRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "OldPwd", static_cast<short>SK_FIELD_OFFSET(IMModPwdRequest_t, pcOldPwd), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },
	{ "NewPwd", static_cast<short>SK_FIELD_OFFSET(IMModPwdRequest_t, pcNewPwd), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
};

static const SKMetaField_t g_tMetaFieldListIMModPwdResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMModPwdResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMModPwdByMobileRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMModPwdByMobileRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "Mobile", static_cast<short>SK_FIELD_OFFSET(IMModPwdByMobileRequest_t, pcMobile), eTypeSKChar, 1, 0, -1,
		"strlen", 2, sizeof(char *), sizeof(char), 1, 0 },	
	{ "Ticket", static_cast<short>SK_FIELD_OFFSET(IMModPwdByMobileRequest_t, pcTicket), eTypeSKChar, 1, 0, -1,
		"strlen", 3, sizeof(char *), sizeof(char), 1, 0 },
	{ "NewPwd", static_cast<short>SK_FIELD_OFFSET(IMModPwdByMobileRequest_t, pcNewPwd), eTypeSKChar, 1, 0, -1,
		"strlen", 4, sizeof(char *), sizeof(char), 1, 0 },
	{ "RandomEncryKey", static_cast<short>SK_FIELD_OFFSET(IMModPwdByMobileRequest_t, tRandomEncryKey), eTypeSKBuiltinString, 0, 1, -1,
		"", 5, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 1, 0 },	
};

static const SKMetaField_t g_tMetaFieldListIMModPwdByMobileResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMModPwdByMobileResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 }
};

//////////////////////////// ”¶”√…Ë÷√End /////////////////////////////////////

//////////////////////////// æŸ±®Begin //////////////////////////////////////////


static const SKMetaField_t g_tMetaFieldListIMReportProfileRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMReportProfileRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "UserName", static_cast<short>SK_FIELD_OFFSET(IMReportProfileRequest_t, tUserName), eTypeSKBuiltinString, 0, 1, -1,
		"", 2, sizeof(Comm::SKBuiltinString_t), sizeof(Comm::SKBuiltinString_t), 0, 0 },
	{ "ReportReason", static_cast<short>SK_FIELD_OFFSET(IMReportProfileRequest_t, iReportReason), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ReportTime", static_cast<short>SK_FIELD_OFFSET(IMReportProfileRequest_t, iReportTime), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMReportProfileResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMReportProfileResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 }
};


static const SKMetaField_t g_tMetaFieldListIMReportGroupRequest [] = {
	{ "BaseRequest", static_cast<short>SK_FIELD_OFFSET(IMReportGroupRequest_t, tBaseRequest), eTypeIMBaseRequest, 0, 1, -1,
		"", 1, sizeof(IMBaseRequest_t), sizeof(IMBaseRequest_t), 1, 0 },
	{ "ChatRoomId", static_cast<short>SK_FIELD_OFFSET(IMReportGroupRequest_t, iChatRoomId), eTypeSKUint32, 0, 1, -1,
		"", 2, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ReportReason", static_cast<short>SK_FIELD_OFFSET(IMReportGroupRequest_t, iReportReason), eTypeSKUint32, 0, 1, -1,
		"", 3, sizeof(unsigned int), sizeof(unsigned int), 0, 0 },
	{ "ReportTime", static_cast<short>SK_FIELD_OFFSET(IMReportGroupRequest_t, iReportTime), eTypeSKUint32, 0, 1, -1,
		"", 4, sizeof(unsigned int), sizeof(unsigned int), 0, 0 }
};

static const SKMetaField_t g_tMetaFieldListIMReportGroupResponse [] = {
	{ "BaseResponse", static_cast<short>SK_FIELD_OFFSET(IMReportGroupResponse_t, tBaseResponse), eTypeIMBaseResponse, 0, 1, -1,
		"", 1, sizeof(IMBaseResponse_t), sizeof(IMBaseResponse_t), 1, 0 }
};

//////////////////////////// æŸ±®End //////////////////////////////////////////



#ifndef MACOS
extern const SKMetaInfo_t * Comm::g_ptSKBuiltinMetaInfo;
#endif


static const SKMetaStruct_t g_tMetaIMStructList [] = {
	{ eTypeIMMsgEchoInfo, "MsgEchoInfo", sizeof(IMMsgEchoInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMMsgEchoInfo), g_tMetaFieldListIMMsgEchoInfo },

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
	{ eTypeIMBaseRequest, "BaseRequest", sizeof(IMBaseRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMBaseRequest), g_tMetaFieldListIMBaseRequest },
	{ eTypeIMBuiltinIP, "BuiltinIP", sizeof(IMBuiltinIP_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMBuiltinIP), g_tMetaFieldListIMBuiltinIP },
	{ eTypeIMBuiltinIPList, "BuiltinIPList", sizeof(IMBuiltinIPList_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMBuiltinIPList), g_tMetaFieldListIMBuiltinIPList },
	{ eTypeIMCDNDnsInfo, "CDNDnsInfo", sizeof(IMCDNDnsInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCDNDnsInfo), g_tMetaFieldListIMCDNDnsInfo },
	{ eTypeIMAuthRequest, "AuthRequest", sizeof(IMAuthRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMAuthRequest), g_tMetaFieldListIMAuthRequest },
	{ eTypeIMRegRequest, "RegRequest", sizeof(IMRegRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMRegRequest), g_tMetaFieldListIMRegRequest },
	{ eTypeIMBaseResponse, "BaseResponse", sizeof(IMBaseResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMBaseResponse), g_tMetaFieldListIMBaseResponse },
	{ eTypeIMNetworkControl, "NetworkControl", sizeof(IMNetworkControl_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMNetworkControl), g_tMetaFieldListIMNetworkControl },
	{ eTypeIMPluginKey, "PluginKey", sizeof(IMPluginKey_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMPluginKey), g_tMetaFieldListIMPluginKey },
	{ eTypeIMPluginKeyList, "PluginKeyList", sizeof(IMPluginKeyList_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMPluginKeyList), g_tMetaFieldListIMPluginKeyList },
	{ eTypeIMHost, "Host", sizeof(IMHost_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMHost), g_tMetaFieldListIMHost },
	{ eTypeIMHostList, "HostList", sizeof(IMHostList_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMHostList), g_tMetaFieldListIMHostList },
	{ eTypeIMAuthResponse, "AuthResponse", sizeof(IMAuthResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMAuthResponse), g_tMetaFieldListIMAuthResponse },
	{ eTypeIMRegResponse, "RegResponse", sizeof(IMRegResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMRegResponse), g_tMetaFieldListIMRegResponse },
	{ eTypeIMUserInfoSetRequest, "UserInfoSetRequest", sizeof(IMUserInfoSetRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUserInfoSetRequest), g_tMetaFieldListIMUserInfoSetRequest },
	{ eTypeIMUserInfoSetResponse, "UserInfoSetResponse", sizeof(IMUserInfoSetResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUserInfoSetResponse), g_tMetaFieldListIMUserInfoSetResponse },
	{ eTypeIMCmdItem, "CmdItem", sizeof(IMCmdItem_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCmdItem), g_tMetaFieldListIMCmdItem },
	{ eTypeIMNewInitRequest, "NewInitRequest", sizeof(IMNewInitRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMNewInitRequest), g_tMetaFieldListIMNewInitRequest },
	{ eTypeIMNewInitResponse, "NewInitResponse", sizeof(IMNewInitResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMNewInitResponse), g_tMetaFieldListIMNewInitResponse },
	{ eTypeIMSnsUserInfo, "SnsUserInfo", sizeof(IMSnsUserInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSnsUserInfo), g_tMetaFieldListIMSnsUserInfo },
	{ eTypeIMCustomizedInfo, "CustomizedInfo", sizeof(IMCustomizedInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCustomizedInfo), g_tMetaFieldListIMCustomizedInfo },
	{ eTypeIMSearchContactRequest, "SearchContactRequest", sizeof(IMSearchContactRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSearchContactRequest), g_tMetaFieldListIMSearchContactRequest },
	{ eTypeIMSearchContactItem, "SearchContactItem", sizeof(IMSearchContactItem_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSearchContactItem), g_tMetaFieldListIMSearchContactItem },
	{ eTypeIMSearchContactResponse, "SearchContactResponse", sizeof(IMSearchContactResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSearchContactResponse), g_tMetaFieldListIMSearchContactResponse },
	{ eTypeIMVerifyUser, "VerifyUser", sizeof(IMVerifyUser_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMVerifyUser), g_tMetaFieldListIMVerifyUser },
	{ eTypeIMVerifyUserRequest, "VerifyUserRequest", sizeof(IMVerifyUserRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMVerifyUserRequest), g_tMetaFieldListIMVerifyUserRequest },
	{ eTypeIMVerifyUserResponse, "VerifyUserResponse", sizeof(IMVerifyUserResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMVerifyUserResponse), g_tMetaFieldListIMVerifyUserResponse },
	{ eTypeIMPossibleFriend, "PossibleFriend", sizeof(IMPossibleFriend_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMPossibleFriend), g_tMetaFieldListIMPossibleFriend },
	{ eTypeIMSyncFriendRequest, "SyncFriendRequest", sizeof(IMSyncFriendRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSyncFriendRequest), g_tMetaFieldListIMSyncFriendRequest },
	{ eTypeIMSyncFriendResponse, "SyncFriendResponse", sizeof(IMSyncFriendResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSyncFriendResponse), g_tMetaFieldListIMSyncFriendResponse },
	{ eTypeIMRoomInfo, "RoomInfo", sizeof(IMRoomInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMRoomInfo), g_tMetaFieldListIMRoomInfo },
	{ eTypeIMModContact, "ModContact", sizeof(IMModContact_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModContact), g_tMetaFieldListIMModContact },
	{ eTypeIMChatRoomMember, "LbsChatRoomMember", sizeof(IMChatRoomMember_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMChatRoomMember), g_tMetaFieldListIMChatRoomMember },
	{ eTypeIMGetContactRequest, "GetContactRequest", sizeof(IMGetContactRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGetContactRequest), g_tMetaFieldListIMGetContactRequest },
	{ eTypeIMGetContactResponse, "GetContactResponse", sizeof(IMGetContactResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGetContactResponse), g_tMetaFieldListIMGetContactResponse },
	{ eTypeIMDelContact, "DelContact", sizeof(IMDelContact_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDelContact), g_tMetaFieldListIMDelContact },

	{ eTypeIMAuthBySKRequest, "AuthBySKRequest", sizeof(IMAuthBySKRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMAuthBySKRequest), g_tMetaFieldListIMAuthBySKRequest },
	{ eTypeIMAuthBySKResponse, "AuthBySKResponse", sizeof(IMAuthBySKResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMAuthBySKResponse), g_tMetaFieldListIMAuthBySKResponse },
	{ eTypeIMMsgRequest, "MsgRequest", sizeof(IMMsgRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMMsgRequest), g_tMetaFieldListIMMsgRequest },
	{ eTypeIMSendMsgRequest, "SendMsgRequest", sizeof(IMSendMsgRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSendMsgRequest), g_tMetaFieldListIMSendMsgRequest },
	{ eTypeIMMsgResponse, "MsgResponse", sizeof(IMMsgResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMMsgResponse), g_tMetaFieldListIMMsgResponse },
	{ eTypeIMSendMsgResponse, "SendMsgResponse", sizeof(IMSendMsgResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSendMsgResponse), g_tMetaFieldListIMSendMsgResponse },
		
//ÂêåÊ?•Ê∂àÊÅ?		
	{ eTypeIMCmdList, "CmdList", sizeof(IMCmdList_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCmdList), g_tMetaFieldListIMCmdList },
	{ eTypeIMOpCmdResp, "OpCmdResp", sizeof(IMOpCmdResp_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMOpCmdResp), g_tMetaFieldListIMOpCmdResp },
	{ eTypeIMOpCmdRespList, "OpCmdRespList", sizeof(IMOpCmdRespList_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMOpCmdRespList), g_tMetaFieldListIMOpCmdRespList },
	{ eTypeIMAddMsg, "AddMsg", sizeof(IMAddMsg_t),
	SK_ARRAY_SIZE(g_tMetaFieldListIMAddMsg), g_tMetaFieldListIMAddMsg },
	{ eTypeIMKeyVal, "KeyVal", sizeof(IMKeyVal_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMKeyVal), g_tMetaFieldListIMKeyVal },
	{ eTypeIMSyncKey, "SyncKey", sizeof(IMSyncKey_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSyncKey), g_tMetaFieldListIMSyncKey },
	{ eTypeIMNewSyncRequest, "NewSyncRequest", sizeof(IMNewSyncRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMNewSyncRequest), g_tMetaFieldListIMNewSyncRequest },
	{ eTypeIMNewSyncResponse, "NewSyncResponse", sizeof(IMNewSyncResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMNewSyncResponse), g_tMetaFieldListIMNewSyncResponse },
	{ eTypeIMSyncRequest, "SyncRequest", sizeof(IMSyncRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSyncRequest), g_tMetaFieldListIMSyncRequest },
	{ eTypeIMSyncResponse, "SyncResponse", sizeof(IMSyncResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSyncResponse), g_tMetaFieldListIMSyncResponse },
		
//Â§¥ÂÉè	
	{ eTypeIMBatchGetHeadImgRequest, "BatchGetHeadImgRequest", sizeof(IMBatchGetHeadImgRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMBatchGetHeadImgRequest), g_tMetaFieldListIMBatchGetHeadImgRequest },
	{ eTypeIMImgPair, "ImgPair", sizeof(IMImgPair_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMImgPair), g_tMetaFieldListIMImgPair },
	{ eTypeIMBatchGetHeadImgResponse, "BatchGetHeadImgResponse", sizeof(IMBatchGetHeadImgResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMBatchGetHeadImgResponse), g_tMetaFieldListIMBatchGetHeadImgResponse },
	{ eTypeIMUploadHDHeadImgRequest, "UploadHDHeadImgRequest", sizeof(IMUploadHDHeadImgRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadHDHeadImgRequest), g_tMetaFieldListIMUploadHDHeadImgRequest },
	{ eTypeIMUploadHDHeadImgResponse, "UploadHDHeadImgResponse", sizeof(IMUploadHDHeadImgResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadHDHeadImgResponse), g_tMetaFieldListIMUploadHDHeadImgResponse },
	{ eTypeIMGetHDHeadImgRequest, "GetHDHeadImgRequest", sizeof(IMGetHDHeadImgRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGetHDHeadImgRequest), g_tMetaFieldListIMGetHDHeadImgRequest },
	{ eTypeIMGetHDHeadImgResponse, "GetHDHeadImgResponse", sizeof(IMGetHDHeadImgResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGetHDHeadImgResponse), g_tMetaFieldListIMGetHDHeadImgResponse },
//ËØ?Èü?
	{ eTypeIMUploadVoiceRequest, "UploadVoiceRequest", sizeof(IMUploadVoiceRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadVoiceRequest), g_tMetaFieldListIMUploadVoiceRequest },
	{ eTypeIMUploadVoiceResponse, "UploadVoiceResponse", sizeof(IMUploadVoiceResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadVoiceResponse), g_tMetaFieldListIMUploadVoiceResponse },
	{ eTypeIMDownloadVoiceRequest, "DownloadVoiceRequest", sizeof(IMDownloadVoiceRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDownloadVoiceRequest), g_tMetaFieldListIMDownloadVoiceRequest },
	{ eTypeIMDownloadVoiceResponse, "DownloadVoiceResponse", sizeof(IMDownloadVoiceResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDownloadVoiceResponse), g_tMetaFieldListIMDownloadVoiceResponse },		
//Ê∂àÊÅØÂõæÁâá		
	{ eTypeIMUploadMsgImgRequest, "UploadMsgImgRequest", sizeof(IMUploadMsgImgRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadMsgImgRequest), g_tMetaFieldListIMUploadMsgImgRequest },
	{ eTypeIMUploadMsgImgResponse, "UploadMsgImgResponse", sizeof(IMUploadMsgImgResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadMsgImgResponse), g_tMetaFieldListIMUploadMsgImgResponse },
	{ eTypeIMDownloadMsgImgRequest, "GetMsgImgRequest", sizeof(IMDownloadMsgImgRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDownloadMsgImgRequest), g_tMetaFieldListIMDownloadMsgImgRequest },
	{ eTypeIMDownloadMsgImgResponse, "GetMsgImgResponse", sizeof(IMDownloadMsgImgResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDownloadMsgImgResponse), g_tMetaFieldListIMDownloadMsgImgResponse },
//ËßÜÈ??
	{ eTypeIMUploadVideoRequest, "UploadVideoRequest", sizeof(IMUploadVideoRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadVideoRequest), g_tMetaFieldListIMUploadVideoRequest },
	{ eTypeIMUploadVideoResponse, "UploadVideoResponse", sizeof(IMUploadVideoResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadVideoResponse), g_tMetaFieldListIMUploadVideoResponse },
	{ eTypeIMDownloadVideoRequest, "DownloadVideoRequest", sizeof(IMDownloadVideoRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDownloadVideoRequest), g_tMetaFieldListIMDownloadVideoRequest },
	{ eTypeIMDownloadVideoResponse, "DownloadVideoResponse", sizeof(IMDownloadVideoResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDownloadVideoResponse), g_tMetaFieldListIMDownloadVideoResponse },

// ÈôÑËøëÁöÑ‰∫∫
	{ eTypeIMLbsRequest, "LbsRequest", sizeof(IMLbsRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMLbsRequest), g_tMetaFieldListIMLbsRequest },
	{ eTypeIMLbsContactInfo, "LbsContactInfo", sizeof(IMLbsContactInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMLbsContactInfo), g_tMetaFieldListIMLbsContactInfo },
	{ eTypeIMLbsResponse, "LbsResponse", sizeof(IMLbsResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMLbsResponse), g_tMetaFieldListIMLbsResponse },
	{ eTypeIMGetLocationRequest, "GetLocationRequest", sizeof(IMGetLocationRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGetLocationRequest), g_tMetaFieldListIMGetLocationRequest },
	{ eTypeIMGetLocationResponse, "GetLocationResponse", sizeof(IMGetLocationResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGetLocationResponse), g_tMetaFieldListIMGetLocationResponse },
	{ eTypeIMLbsMatchRequest, "LbsMatchRequest", sizeof(IMLbsMatchRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMLbsMatchRequest), g_tMetaFieldListIMLbsMatchRequest },
	{ eTypeIMLbsMatchResponse, "LbsMatchResponse", sizeof(IMLbsMatchResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMLbsMatchResponse), g_tMetaFieldListIMLbsMatchResponse },
	{ eTypeIMSyncLbsMatchRequest, "SyncLbsMatchRequest", sizeof(IMSyncLbsMatchRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSyncLbsMatchRequest), g_tMetaFieldListIMSyncLbsMatchRequest },
	{ eTypeIMSyncLbsMatchResponse, "SyncLbsMatchResponse", sizeof(IMSyncLbsMatchResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSyncLbsMatchResponse), g_tMetaFieldListIMSyncLbsMatchResponse },
	{ eTypeIMLbsMatchContactInfo, "LbsMatchContactInfo", sizeof(IMLbsMatchContactInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMLbsMatchContactInfo), g_tMetaFieldListIMLbsMatchContactInfo },

//Ëá?ÂÆö‰πâË°®Ê??	
{ eTypeIMEmojiUploadInfoReq, "UploadEmojiRequest", sizeof(IMEmojiUploadInfoReq_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMEmojiUploadInfoReq), g_tMetaFieldListIMEmojiUploadInfoReq },
	{ eTypeIMUploadEmojiRequest, "UploadEmojiRequest", sizeof(IMUploadEmojiRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadEmojiRequest), g_tMetaFieldListIMUploadEmojiRequest },
	{ eTypeIMEmojiUploadInfoResp, "EmojiUploadInfoResp", sizeof(IMEmojiUploadInfoResp_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMEmojiUploadInfoResp), g_tMetaFieldListIMEmojiUploadInfoResp },
	{ eTypeIMUploadEmojiResponse, "UploadEmojiResponse", sizeof(IMUploadEmojiResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadEmojiResponse), g_tMetaFieldListIMUploadEmojiResponse },
	{ eTypeIMEmojiDownloadInfoReq, "EmojiDownloadInfoReq", sizeof(IMEmojiDownloadInfoReq_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMEmojiDownloadInfoReq), g_tMetaFieldListIMEmojiDownloadInfoReq },
	{ eTypeIMDownloadEmojiRequest, "DownloadEmojiRequest", sizeof(IMDownloadEmojiRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDownloadEmojiRequest), g_tMetaFieldListIMDownloadEmojiRequest },
	{ eTypeIMEmojiDownloadInfoResp, "EmojiDownloadInfoResp", sizeof(IMEmojiDownloadInfoResp_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMEmojiDownloadInfoResp), g_tMetaFieldListIMEmojiDownloadInfoResp },
	{ eTypeIMDownloadEmojiResponse, "DownloadEmojiResponse", sizeof(IMDownloadEmojiResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDownloadEmojiResponse), g_tMetaFieldListIMDownloadEmojiResponse },
//////////////////////////////// »∫œ‡πÿBegin ////////////////////////////////////
	{ eTypeIMModChatRoomNotify, "ModChatRoomNotify", sizeof(IMModChatRoomNotify_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModChatRoomNotify), g_tMetaFieldListIMModChatRoomNotify },
	{ eTypeIMModChatRoomMember, "ModChatRoomMember", sizeof(IMModChatRoomMember_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModChatRoomMember), g_tMetaFieldListIMModChatRoomMember },
	{ eTypeIMQuitChatRoom, "QuitChatRoom", sizeof(IMQuitChatRoom_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMQuitChatRoom), g_tMetaFieldListIMQuitChatRoom },
	{ eTypeIMQuitChatRoomRequest, "QuitChatRoomRequest", sizeof(IMQuitChatRoomRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMQuitChatRoomRequest), g_tMetaFieldListIMQuitChatRoomRequest },
	{ eTypeIMQuitChatRoomResponse, "QuitChatRoomResponse", sizeof(IMQuitChatRoomResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMQuitChatRoomResponse), g_tMetaFieldListIMQuitChatRoomResponse },
	{ eTypeIMModChatRoomTopic, "ModChatRoomTopic", sizeof(IMModChatRoomTopic_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModChatRoomTopic), g_tMetaFieldListIMModChatRoomTopic },	
	{ eTypeIMModGroupTopic, "ModGroupTopic", sizeof(IMModGroupTopic_t),
			SK_ARRAY_SIZE(g_tMetaFieldListIMModGroupTopic), g_tMetaFieldListIMModGroupTopic },
	{ eTypeIMModChatRoomMemberDisplayName, "ModChatRoomMemberDisplayName", sizeof(IMModChatRoomMemberDisplayName_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModChatRoomMemberDisplayName), g_tMetaFieldListIMModChatRoomMemberDisplayName },
	{ eTypeIMModChatRoomMemberFlag, "ModChatRoomMemberFlag", sizeof(IMModChatRoomMemberFlag_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModChatRoomMemberFlag), g_tMetaFieldListIMModChatRoomMemberFlag },
	{ eTypeIMMemberReq, "MemberReq", sizeof(IMMemberReq_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMMemberReq), g_tMetaFieldListIMMemberReq },
	{ eTypeIMMemberResp, "MemberResp", sizeof(IMMemberResp_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMMemberResp), g_tMetaFieldListIMMemberResp },
	{ eTypeIMDelMemberReq, "DelMemberReq", sizeof(IMDelMemberReq_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDelMemberReq), g_tMetaFieldListIMDelMemberReq },
	{ eTypeIMDelMemberResp, "DelMemberResp", sizeof(IMDelMemberResp_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDelMemberResp), g_tMetaFieldListIMDelMemberResp },
	{ eTypeIMCreateChatRoomRequest, "CreateChatRoomRequest", sizeof(IMCreateChatRoomRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCreateChatRoomRequest), g_tMetaFieldListIMCreateChatRoomRequest },
	{ eTypeIMCreateChatRoomResponse, "CreateChatRoomResponse", sizeof(IMCreateChatRoomResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCreateChatRoomResponse), g_tMetaFieldListIMCreateChatRoomResponse },
	{ eTypeIMSearchChatRoomRequest, "SearchChatRoomRequest", sizeof(IMSearchChatRoomRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSearchChatRoomRequest), g_tMetaFieldListIMSearchChatRoomRequest },
	{ eTypeIMChatRoomResult, "ChatRoomResult", sizeof(IMChatRoomResult_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMChatRoomResult), g_tMetaFieldListIMChatRoomResult },
	{ eTypeIMSearchChatRoomResponse, "SearchChatRoomResponse", sizeof(IMSearchChatRoomResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSearchChatRoomResponse), g_tMetaFieldListIMSearchChatRoomResponse },
	{ eTypeIMAddChatRoomMemberRequest, "AddChatRoomMemberRequest", sizeof(IMAddChatRoomMemberRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMAddChatRoomMemberRequest), g_tMetaFieldListIMAddChatRoomMemberRequest },
	{ eTypeIMAddChatRoomMemberResponse, "AddChatRoomMemberResponse", sizeof(IMAddChatRoomMemberResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMAddChatRoomMemberResponse), g_tMetaFieldListIMAddChatRoomMemberResponse },
	{ eTypeIMDelChatRoomMemberRequest, "DelChatRoomMemberRequest", sizeof(IMDelChatRoomMemberRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDelChatRoomMemberRequest), g_tMetaFieldListIMDelChatRoomMemberRequest },
	{ eTypeIMDelChatRoomMemberResponse, "DelChatRoomMemberResponse", sizeof(IMDelChatRoomMemberResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDelChatRoomMemberResponse), g_tMetaFieldListIMDelChatRoomMemberResponse },
	{ eTypeIMLbsChatRoomRequest, "LbsChatRoomRequest", sizeof(IMLbsChatRoomRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMLbsChatRoomRequest), g_tMetaFieldListIMLbsChatRoomRequest },
	{ eTypeIMLbsChatRoom, "LbsChatRoom", sizeof(IMLbsChatRoom_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMLbsChatRoom), g_tMetaFieldListIMLbsChatRoom },
	{ eTypeIMLbsChatRoomResponse, "LbsChatRoomResponse", sizeof(IMLbsChatRoomResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMLbsChatRoomResponse), g_tMetaFieldListIMLbsChatRoomResponse },
	{ eTypeIMVerifyChatRoomMemberRequest, "VerifyChatRoomMemberRequest", sizeof(IMVerifyChatRoomMemberRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMVerifyChatRoomMemberRequest), g_tMetaFieldListIMVerifyChatRoomMemberRequest },
	{ eTypeIMVerifyChatRoomMemberResponse, "VerifyChatRoomMemberResponse", sizeof(IMVerifyChatRoomMemberResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMVerifyChatRoomMemberResponse), g_tMetaFieldListIMVerifyChatRoomMemberResponse },
	{ eTypeIMInviteChatRoomMemberRequest, "InviteChatRoomMemberRequest", sizeof(IMInviteChatRoomMemberRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMInviteChatRoomMemberRequest), g_tMetaFieldListIMInviteChatRoomMemberRequest },
	{ eTypeIMInviteChatRoomMemberResponse, "InviteChatRoomMemberResponse", sizeof(IMInviteChatRoomMemberResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMInviteChatRoomMemberResponse), g_tMetaFieldListIMInviteChatRoomMemberResponse },
	{ eTypeIMCloseChatRoomRequest, "CloseChatRoomRequest", sizeof(IMCloseChatRoomRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCloseChatRoomRequest), g_tMetaFieldListIMCloseChatRoomRequest },
	{ eTypeIMCloseChatRoomResponse, "CloseChatRoomResponse", sizeof(IMCloseChatRoomResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCloseChatRoomResponse), g_tMetaFieldListIMCloseChatRoomResponse },

	{ eTypeIMQuitGroupRequest, "QuitGroupRequest", sizeof(IMQuitGroupRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMQuitGroupRequest), g_tMetaFieldListIMQuitGroupRequest },
	{ eTypeIMQuitGroupResponse, "QuitGroupResponse", sizeof(IMQuitGroupResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMQuitGroupResponse), g_tMetaFieldListIMQuitGroupResponse },
	{ eTypeIMAddGroupMemberRequest, "AddGroupMemberRequest", sizeof(IMAddGroupMemberRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMAddGroupMemberRequest), g_tMetaFieldListIMAddGroupMemberRequest },
	{ eTypeIMAddGroupMemberResponse, "AddGroupMemberResponse", sizeof(IMAddGroupMemberResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMAddGroupMemberResponse), g_tMetaFieldListIMAddGroupMemberResponse },
	{ eTypeIMDelGroupMemberRequest, "DelGroupMemberRequest", sizeof(IMDelGroupMemberRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDelGroupMemberRequest), g_tMetaFieldListIMDelGroupMemberRequest },
	{ eTypeIMDelGroupMemberResponse, "DelGroupMemberResponse", sizeof(IMDelGroupMemberResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDelGroupMemberResponse), g_tMetaFieldListIMDelGroupMemberResponse },
	{ eTypeIMCreateGroupRequest, "CreateGroupRequest", sizeof(IMCreateGroupRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCreateGroupRequest), g_tMetaFieldListIMCreateGroupRequest },
	{ eTypeIMCreateGroupResponse, "CreateGroupResponse", sizeof(IMCreateGroupResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCreateGroupResponse), g_tMetaFieldListIMCreateGroupResponse },

//////////////////////////////// »∫œ‡πÿEnd //////////////////////////////////////

//Profile, add by yihuang
	{ eTypeIMDisturbTimeSpan, "DisturbTimeSpan", sizeof(IMDisturbTimeSpan_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDisturbTimeSpan), g_tMetaFieldListIMDisturbTimeSpan },
	{ eTypeIMDisturbSetting, "DisturbSetting", sizeof(IMDisturbSetting_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMDisturbSetting), g_tMetaFieldListIMDisturbSetting },
	{ eTypeIMGmailInfo, "GmailInfo", sizeof(IMGmailInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGmailInfo), g_tMetaFieldListIMGmailInfo },
	{ eTypeIMGmailList, "GmailList", sizeof(IMGmailList_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGmailList), g_tMetaFieldListIMGmailList },
	{ eTypeIMFacebookInfo, "FacebookInfo", sizeof(IMFacebookInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMFacebookInfo), g_tMetaFieldListIMFacebookInfo },
	{ eTypeIMSafeDevice, "SafeDevice", sizeof(IMSafeDevice_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSafeDevice), g_tMetaFieldListIMSafeDevice },
	{ eTypeIMSafeDeviceList, "SafeDeviceList", sizeof(IMSafeDeviceList_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMSafeDeviceList), g_tMetaFieldListIMSafeDeviceList },
	{ eTypeIMUserInfoExt, "UserInfoExt", sizeof(IMUserInfoExt_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUserInfoExt), g_tMetaFieldListIMUserInfoExt },
	{ eTypeIMModUserInfo, "ModUserInfo", sizeof(IMModUserInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModUserInfo), g_tMetaFieldListIMModUserInfo },
	{ eTypeIMGetProfileRequest, "GetProfileRequest", sizeof(IMGetProfileRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGetProfileRequest), g_tMetaFieldListIMGetProfileRequest },
	{ eTypeIMGetProfileResponse, "GetProfileResponse", sizeof(IMGetProfileResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGetProfileResponse), g_tMetaFieldListIMGetProfileResponse },
	{ eTypeIMUploadCoverImgRequest, "UploadCoverImgRequest", sizeof(IMUploadCoverImgRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadCoverImgRequest), g_tMetaFieldListIMUploadCoverImgRequest },
	{ eTypeIMUploadCoverImgResponse, "UploadCoverImgResponse", sizeof(IMUploadCoverImgResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadCoverImgResponse), g_tMetaFieldListIMUploadCoverImgResponse },
	{ eTypeIMGetCoverImgRequest, "GetCoverImgRequest", sizeof(IMGetCoverImgRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGetCoverImgRequest), g_tMetaFieldListIMGetCoverImgRequest },
	{ eTypeIMGetCoverImgResponse, "GetCoverImgResponse", sizeof(IMGetCoverImgResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMGetCoverImgResponse), g_tMetaFieldListIMGetCoverImgResponse },
	{ eTypeIMHobby, "Hobby", sizeof(IMHobby_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMHobby), g_tMetaFieldListIMHobby },
	{ eTypeIMBirthday, "Birthday", sizeof(IMBirthday_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMBirthday), g_tMetaFieldListIMBirthday },
	{ eTypeIMFunctionSwitch, "FunctionSwitch", sizeof(IMFunctionSwitch_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMFunctionSwitch), g_tMetaFieldListIMFunctionSwitch },
	//
	{ eTypeIMMobileOptRequest, "MobileOptRequest", sizeof(IMMobileOptRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMMobileOptRequest), g_tMetaFieldListIMMobileOptRequest },
	{ eTypeIMMobileOptResponse, "MobileOptResponse", sizeof(IMMobileOptResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMMobileOptResponse), g_tMetaFieldListIMMobileOptResponse },

	{ eTypeIMEmailOptRequest, "EmailOptRequest", sizeof(IMEmailOptRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMEmailOptRequest), g_tMetaFieldListIMEmailOptRequest },
	{ eTypeIMEmailOptResponse, "EmailOptResponse", sizeof(IMEmailOptResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMEmailOptResponse), g_tMetaFieldListIMEmailOptResponse },

	{ eTypeIMUploadPlugFriendsRequest, "UploadPlugFriendsRequest", sizeof(IMUploadPlugFriendsRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadPlugFriendsRequest), g_tMetaFieldListIMUploadPlugFriendsRequest },
	{ eTypeIMUploadPlugFriendsResponse, "UploadPlugFriendsResponse", sizeof(IMUploadPlugFriendsResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMUploadPlugFriendsResponse), g_tMetaFieldListIMUploadPlugFriendsResponse },

	{ eTypeIMPushSettingRequest, "PushSettingRequest", sizeof(IMPushSettingRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMPushSettingRequest), g_tMetaFieldListIMPushSettingRequest },
	{ eTypeIMPushSettingResponse, "PushSettingResponse", sizeof(IMPushSettingResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMPushSettingResponse), g_tMetaFieldListIMPushSettingResponse },

	{ eTypeIMModUserNameRequest, "ModUserNameRequest", sizeof(IMModUserNameRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModUserNameRequest), g_tMetaFieldListIMModUserNameRequest },
	{ eTypeIMModUserNameResponse, "ModUserNameResponse", sizeof(IMModUserNameResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModUserNameResponse), g_tMetaFieldListIMModUserNameResponse },

	{ eTypeIMModPwdRequest, "ModPwdRequest", sizeof(IMModPwdRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModPwdRequest), g_tMetaFieldListIMModPwdRequest },
	{ eTypeIMModPwdResponse, "ModPwdResponse", sizeof(IMModPwdResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModPwdResponse), g_tMetaFieldListIMModPwdResponse },

	{ eTypeIMModPwdByMobileRequest, "ModPwdByMobileRequest", sizeof(IMModPwdByMobileRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModPwdByMobileRequest), g_tMetaFieldListIMModPwdByMobileRequest },
	{ eTypeIMModPwdByMobileResponse, "ModPwdByMobileResponse", sizeof(IMModPwdByMobileResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMModPwdByMobileResponse), g_tMetaFieldListIMModPwdByMobileResponse },

	{ eTypeIMReportProfileRequest, "ReportProfileRequest", sizeof(IMReportProfileRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMReportProfileRequest), g_tMetaFieldListIMReportProfileRequest },
	{ eTypeIMReportProfileResponse, "ReportProfileResponse", sizeof(IMReportProfileResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMReportProfileResponse), g_tMetaFieldListIMReportProfileResponse },

	{ eTypeIMReportGroupRequest, "ReportGroupRequest", sizeof(IMReportGroupRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMReportGroupRequest), g_tMetaFieldListIMReportGroupRequest },
	{ eTypeIMReportGroupResponse, "ReportGroupResponse", sizeof(IMReportGroupResponse_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMReportGroupResponse), g_tMetaFieldListIMReportGroupResponse },

	{ eTypeIMSendLocationRequest, "SendLocationRequest", sizeof(IMSendLocationRequest_t),
			SK_ARRAY_SIZE(g_tMetaFieldListIMSendLocationRequest), g_tMetaFieldListIMSendLocationRequest },
			/*
	{ eTypeIMSendLocationResponse, "SendLocationResponse", sizeof(IMSendLocationResponse_t),
			SK_ARRAY_SIZE(g_tMetaFieldListIMSendLocationResponse), g_tMetaFieldListIMSendLocationResponse },
*/
	{ eTypeIMSendCustomMsgRequest, "SendCustomMsgRequest", sizeof(IMSendCustomMsgRequest_t),
				SK_ARRAY_SIZE(g_tMetaFieldListIMSendCustomMsgRequest), g_tMetaFieldListIMSendCustomMsgRequest },
				/*
	{ eTypeIMSendCustomMsgResponse, "SendCustomMsgResponse", sizeof(IMSendCustomMsgResponse_t),
				SK_ARRAY_SIZE(g_tMetaFieldListIMSendCustomMsgResponse), g_tMetaFieldListIMSendCustomMsgResponse },
				*/
	{ eTypeIMCustomMsgRequest, "CustomMsgRequest", sizeof(IMCustomMsgRequest_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMCustomMsgRequest), g_tMetaFieldListIMCustomMsgRequest },
	{ eTypeIMWebProxyInfo, "WebProxyInfo", sizeof(IMWebProxyInfo_t),
		SK_ARRAY_SIZE(g_tMetaFieldListIMWebProxyInfo), g_tMetaFieldListIMWebProxyInfo },
};

static const SKMetaInfo_t g_tMetaIMMsg = {
	"MM", "Msg", 10000,
	SK_ARRAY_SIZE(g_tMetaIMStructList), g_tMetaIMStructList,
	SK_ARRAY_SIZE(g_tMetaIMFuncList), g_tMetaIMFuncList
};

const SKMetaInfo_t * g_ptIMMsgMetaInfo = &g_tMetaIMMsg;
const static SKMetaInfoFinder g_objIMMsgMetaInfoFinder(g_ptIMMsgMetaInfo);
const SKMetaInfoFinder *  g_ptIMMsgMetaInfoFinder=&g_objIMMsgMetaInfoFinder;

//============================================================

#ifdef MACOS
int IMMsgPickle :: m_iNetworkByteOrder = 0;
int IMMsgPickle :: m_iPickleType = ePickleTypeTlv;
int IMMsgPickle :: m_iPickleStatus = 0;
#else
 int IMMsgPickle :: m_iNetworkByteOrder = 0;
 int IMMsgPickle :: m_iPickleType = ePickleTypeTlv;
 int IMMsgPickle :: m_iPickleStatus = 0;
#endif
void IMMsgPickle :: SetNetworkByteOrder( int iNetworkByteOrder )
{
	m_iNetworkByteOrder = iNetworkByteOrder;
}

void IMMsgPickle :: SetPickleType( int iPickleType )
{
	m_iPickleType = iPickleType;
}

int IMMsgPickle :: GetPickleType()
{
	return m_iPickleType;
}

void IMMsgPickle :: SetPickleStatus( int iStatus )
{
	m_iPickleStatus = iStatus;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMMsgEchoInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMsgEchoInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMsgEchoInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMsgEchoInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMMsgEchoInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMsgEchoInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMMsgEchoInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMMsgEchoInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMMsgEchoInfo_t * pDestStruct, const IMMsgEchoInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMMsgEchoInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMMsgEchoInfo_t * ptStruct1, IMMsgEchoInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMMsgEchoInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMMsgEchoInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMsgEchoInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMsgEchoInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMsgEchoInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMMsgEchoInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMsgEchoInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMBaseRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBaseRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBaseRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBaseRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMBaseRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBaseRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMBaseRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMBaseRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMBaseRequest_t * pDestStruct, const IMBaseRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMBaseRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMBaseRequest_t * ptStruct1, IMBaseRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMBaseRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMBaseRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBaseRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBaseRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBaseRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMBaseRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBaseRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMBuiltinIP_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBuiltinIP, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBuiltinIP, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBuiltinIP_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMBuiltinIP, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBuiltinIP, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMBuiltinIP_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMBuiltinIP, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMBuiltinIP_t * pDestStruct, const IMBuiltinIP_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMBuiltinIP, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMBuiltinIP_t * ptStruct1, IMBuiltinIP_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMBuiltinIP, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMBuiltinIP_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBuiltinIP, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBuiltinIP, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBuiltinIP_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMBuiltinIP, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBuiltinIP, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMBuiltinIPList_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBuiltinIPList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBuiltinIPList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBuiltinIPList_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMBuiltinIPList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBuiltinIPList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMBuiltinIPList_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMBuiltinIPList, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMBuiltinIPList_t * pDestStruct, const IMBuiltinIPList_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMBuiltinIPList, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMBuiltinIPList_t * ptStruct1, IMBuiltinIPList_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMBuiltinIPList, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMBuiltinIPList_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBuiltinIPList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBuiltinIPList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBuiltinIPList_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMBuiltinIPList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBuiltinIPList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMCDNDnsInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCDNDnsInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCDNDnsInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCDNDnsInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCDNDnsInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCDNDnsInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMCDNDnsInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCDNDnsInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCDNDnsInfo_t * pDestStruct, const IMCDNDnsInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCDNDnsInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCDNDnsInfo_t * ptStruct1, IMCDNDnsInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCDNDnsInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCDNDnsInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCDNDnsInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCDNDnsInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCDNDnsInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCDNDnsInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCDNDnsInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMAuthRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAuthRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAuthRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAuthRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMAuthRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAuthRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMAuthRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMAuthRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMAuthRequest_t * pDestStruct, const IMAuthRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMAuthRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMAuthRequest_t * ptStruct1, IMAuthRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMAuthRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMAuthRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAuthRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAuthRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAuthRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMAuthRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAuthRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMRegRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMRegRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMRegRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMRegRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMRegRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMRegRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMRegRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMRegRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMRegRequest_t * pDestStruct, const IMRegRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMRegRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMRegRequest_t * ptStruct1, IMRegRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMRegRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMRegRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMRegRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMRegRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMRegRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMRegRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMRegRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMBaseResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBaseResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBaseResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBaseResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMBaseResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBaseResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMBaseResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMBaseResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMBaseResponse_t * pDestStruct, const IMBaseResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMBaseResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMBaseResponse_t * ptStruct1, IMBaseResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMBaseResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMBaseResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBaseResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBaseResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBaseResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMBaseResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBaseResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMNetworkControl_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMNetworkControl, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMNetworkControl, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMNetworkControl_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMNetworkControl, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMNetworkControl, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMNetworkControl_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMNetworkControl, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMNetworkControl_t * pDestStruct, const IMNetworkControl_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMNetworkControl, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMNetworkControl_t * ptStruct1, IMNetworkControl_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMNetworkControl, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMNetworkControl_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMNetworkControl, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMNetworkControl, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMNetworkControl_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMNetworkControl, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMNetworkControl, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMPluginKey_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMPluginKey, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMPluginKey, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMPluginKey_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMPluginKey, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMPluginKey, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMPluginKey_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMPluginKey, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMPluginKey_t * pDestStruct, const IMPluginKey_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMPluginKey, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMPluginKey_t * ptStruct1, IMPluginKey_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMPluginKey, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMPluginKey_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMPluginKey, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMPluginKey, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMPluginKey_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMPluginKey, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMPluginKey, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMPluginKeyList_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMPluginKeyList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMPluginKeyList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMPluginKeyList_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMPluginKeyList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMPluginKeyList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMPluginKeyList_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMPluginKeyList, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMPluginKeyList_t * pDestStruct, const IMPluginKeyList_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMPluginKeyList, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMPluginKeyList_t * ptStruct1, IMPluginKeyList_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMPluginKeyList, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMPluginKeyList_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMPluginKeyList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMPluginKeyList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMPluginKeyList_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMPluginKeyList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMPluginKeyList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMHost_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMHost, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMHost, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMHost_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMHost, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMHost, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMHost_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMHost, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMHost_t * pDestStruct, const IMHost_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMHost, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMHost_t * ptStruct1, IMHost_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMHost, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMHost_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMHost, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMHost, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMHost_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMHost, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMHost, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMHostList_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMHostList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMHostList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMHostList_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMHostList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMHostList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMHostList_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMHostList, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMHostList_t * pDestStruct, const IMHostList_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMHostList, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMHostList_t * ptStruct1, IMHostList_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMHostList, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMHostList_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMHostList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMHostList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMHostList_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMHostList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMHostList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMAuthResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAuthResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAuthResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAuthResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMAuthResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAuthResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMAuthResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMAuthResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMAuthResponse_t * pDestStruct, const IMAuthResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMAuthResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMAuthResponse_t * ptStruct1, IMAuthResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMAuthResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMAuthResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAuthResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAuthResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAuthResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMAuthResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAuthResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMRegResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMRegResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMRegResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMRegResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMRegResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMRegResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMRegResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMRegResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMRegResponse_t * pDestStruct, const IMRegResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMRegResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMRegResponse_t * ptStruct1, IMRegResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMRegResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMRegResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMRegResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMRegResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMRegResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMRegResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMRegResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMUserInfoSetRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUserInfoSetRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUserInfoSetRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUserInfoSetRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUserInfoSetRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUserInfoSetRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMUserInfoSetRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUserInfoSetRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUserInfoSetRequest_t * pDestStruct, const IMUserInfoSetRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUserInfoSetRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUserInfoSetRequest_t * ptStruct1, IMUserInfoSetRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUserInfoSetRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUserInfoSetRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUserInfoSetRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUserInfoSetRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUserInfoSetRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUserInfoSetRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUserInfoSetRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMUserInfoSetResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUserInfoSetResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUserInfoSetResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUserInfoSetResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUserInfoSetResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUserInfoSetResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMUserInfoSetResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUserInfoSetResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUserInfoSetResponse_t * pDestStruct, const IMUserInfoSetResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUserInfoSetResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUserInfoSetResponse_t * ptStruct1, IMUserInfoSetResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUserInfoSetResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUserInfoSetResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUserInfoSetResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUserInfoSetResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUserInfoSetResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUserInfoSetResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUserInfoSetResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMCmdItem_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCmdItem, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCmdItem, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCmdItem_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCmdItem, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCmdItem, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMCmdItem_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCmdItem, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCmdItem_t * pDestStruct, const IMCmdItem_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCmdItem, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCmdItem_t * ptStruct1, IMCmdItem_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCmdItem, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCmdItem_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCmdItem, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCmdItem, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCmdItem_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCmdItem, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCmdItem, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMNewInitRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMNewInitRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMNewInitRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMNewInitRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMNewInitRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMNewInitRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMNewInitRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMNewInitRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMNewInitRequest_t * pDestStruct, const IMNewInitRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMNewInitRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMNewInitRequest_t * ptStruct1, IMNewInitRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMNewInitRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMNewInitRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMNewInitRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMNewInitRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMNewInitRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMNewInitRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMNewInitRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMNewInitResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMNewInitResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMNewInitResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMNewInitResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMNewInitResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMNewInitResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMNewInitResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMNewInitResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMNewInitResponse_t * pDestStruct, const IMNewInitResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMNewInitResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMNewInitResponse_t * ptStruct1, IMNewInitResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMNewInitResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMNewInitResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMNewInitResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMNewInitResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMNewInitResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMNewInitResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMNewInitResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMSnsUserInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSnsUserInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSnsUserInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSnsUserInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSnsUserInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSnsUserInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMSnsUserInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSnsUserInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSnsUserInfo_t * pDestStruct, const IMSnsUserInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSnsUserInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSnsUserInfo_t * ptStruct1, IMSnsUserInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSnsUserInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSnsUserInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSnsUserInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSnsUserInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSnsUserInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSnsUserInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSnsUserInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMCustomizedInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCustomizedInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCustomizedInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCustomizedInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCustomizedInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCustomizedInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMCustomizedInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCustomizedInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCustomizedInfo_t * pDestStruct, const IMCustomizedInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCustomizedInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCustomizedInfo_t * ptStruct1, IMCustomizedInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCustomizedInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCustomizedInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCustomizedInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCustomizedInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCustomizedInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCustomizedInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCustomizedInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMSearchContactRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSearchContactRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSearchContactRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSearchContactRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSearchContactRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSearchContactRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMSearchContactRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSearchContactRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSearchContactRequest_t * pDestStruct, const IMSearchContactRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSearchContactRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSearchContactRequest_t * ptStruct1, IMSearchContactRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSearchContactRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSearchContactRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSearchContactRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSearchContactRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSearchContactRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSearchContactRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSearchContactRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMSearchContactItem_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSearchContactItem, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSearchContactItem, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSearchContactItem_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSearchContactItem, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSearchContactItem, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMSearchContactItem_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSearchContactItem, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSearchContactItem_t * pDestStruct, const IMSearchContactItem_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSearchContactItem, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSearchContactItem_t * ptStruct1, IMSearchContactItem_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSearchContactItem, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSearchContactItem_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSearchContactItem, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSearchContactItem, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSearchContactItem_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSearchContactItem, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSearchContactItem, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMSearchContactResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSearchContactResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSearchContactResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSearchContactResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSearchContactResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSearchContactResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMSearchContactResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSearchContactResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSearchContactResponse_t * pDestStruct, const IMSearchContactResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSearchContactResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSearchContactResponse_t * ptStruct1, IMSearchContactResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSearchContactResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSearchContactResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSearchContactResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSearchContactResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSearchContactResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSearchContactResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSearchContactResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMVerifyUser_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMVerifyUser, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMVerifyUser, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMVerifyUser_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMVerifyUser, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMVerifyUser, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMVerifyUser_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMVerifyUser, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMVerifyUser_t * pDestStruct, const IMVerifyUser_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMVerifyUser, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMVerifyUser_t * ptStruct1, IMVerifyUser_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMVerifyUser, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMVerifyUser_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMVerifyUser, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMVerifyUser, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMVerifyUser_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMVerifyUser, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMVerifyUser, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMVerifyUserRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMVerifyUserRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMVerifyUserRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMVerifyUserRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMVerifyUserRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMVerifyUserRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMVerifyUserRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMVerifyUserRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMVerifyUserRequest_t * pDestStruct, const IMVerifyUserRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMVerifyUserRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMVerifyUserRequest_t * ptStruct1, IMVerifyUserRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMVerifyUserRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMVerifyUserRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMVerifyUserRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMVerifyUserRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMVerifyUserRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMVerifyUserRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMVerifyUserRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMVerifyUserResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMVerifyUserResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMVerifyUserResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMVerifyUserResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMVerifyUserResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMVerifyUserResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMVerifyUserResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMVerifyUserResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMVerifyUserResponse_t * pDestStruct, const IMVerifyUserResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMVerifyUserResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMVerifyUserResponse_t * ptStruct1, IMVerifyUserResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMVerifyUserResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMVerifyUserResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMVerifyUserResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMVerifyUserResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMVerifyUserResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMVerifyUserResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMVerifyUserResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================

int IMMsgPickle :: ToBuffer( const IMDelContact_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelContact, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelContact, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelContact_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDelContact, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelContact, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMDelContact_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDelContact, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDelContact_t * pDestStruct, const IMDelContact_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDelContact, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDelContact_t * ptStruct1, IMDelContact_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDelContact, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDelContact_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelContact, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelContact, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelContact_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDelContact, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelContact, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}

//=====================================================

int IMMsgPickle :: ToBuffer( const IMSyncFriendRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncFriendRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncFriendRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncFriendRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncFriendRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncFriendRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMSyncFriendRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSyncFriendRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSyncFriendRequest_t * pDestStruct, const IMSyncFriendRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSyncFriendRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSyncFriendRequest_t * ptStruct1, IMSyncFriendRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSyncFriendRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSyncFriendRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncFriendRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncFriendRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncFriendRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncFriendRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncFriendRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMSyncFriendResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncFriendResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncFriendResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncFriendResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncFriendResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncFriendResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMSyncFriendResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSyncFriendResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSyncFriendResponse_t * pDestStruct, const IMSyncFriendResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSyncFriendResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSyncFriendResponse_t * ptStruct1, IMSyncFriendResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSyncFriendResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSyncFriendResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncFriendResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncFriendResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncFriendResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncFriendResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncFriendResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}

int IMMsgPickle :: ToBuffer( const IMPossibleFriend_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMPossibleFriend, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMPossibleFriend, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMPossibleFriend_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMPossibleFriend, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMPossibleFriend, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMPossibleFriend_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMPossibleFriend, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMPossibleFriend_t * pDestStruct, const IMPossibleFriend_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMPossibleFriend, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMPossibleFriend_t * ptStruct1, IMPossibleFriend_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMPossibleFriend, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMPossibleFriend_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMPossibleFriend, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMPossibleFriend, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMPossibleFriend_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMPossibleFriend, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMPossibleFriend, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMRoomInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMRoomInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMRoomInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMRoomInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMRoomInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMRoomInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMRoomInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMRoomInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMRoomInfo_t * pDestStruct, const IMRoomInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMRoomInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMRoomInfo_t * ptStruct1, IMRoomInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMRoomInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMRoomInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMRoomInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMRoomInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMRoomInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMRoomInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMRoomInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMModContact_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModContact, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModContact, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModContact_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModContact, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModContact, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMModContact_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModContact, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModContact_t * pDestStruct, const IMModContact_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModContact, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModContact_t * ptStruct1, IMModContact_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModContact, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModContact_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModContact, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModContact, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModContact_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModContact, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModContact, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMGetContactRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetContactRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetContactRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetContactRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGetContactRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetContactRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMGetContactRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGetContactRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGetContactRequest_t * pDestStruct, const IMGetContactRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGetContactRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGetContactRequest_t * ptStruct1, IMGetContactRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGetContactRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGetContactRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetContactRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetContactRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetContactRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGetContactRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetContactRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}
//=====================================================
int IMMsgPickle :: ToBuffer( const IMGetContactResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetContactResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetContactResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetContactResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGetContactResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetContactResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMGetContactResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGetContactResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGetContactResponse_t * pDestStruct, const IMGetContactResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGetContactResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGetContactResponse_t * ptStruct1, IMGetContactResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGetContactResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGetContactResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetContactResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetContactResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetContactResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGetContactResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetContactResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}

//=====================================================
int IMMsgPickle :: ToBuffer( const IMAuthBySKRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAuthBySKRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAuthBySKRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAuthBySKRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMAuthBySKRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAuthBySKRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMAuthBySKRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMAuthBySKRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMAuthBySKRequest_t * pDestStruct, const IMAuthBySKRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMAuthBySKRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMAuthBySKRequest_t * ptStruct1, IMAuthBySKRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMAuthBySKRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMAuthBySKRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAuthBySKRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAuthBySKRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAuthBySKRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMAuthBySKRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAuthBySKRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}

//=====================================================
int IMMsgPickle :: ToBuffer( const IMAuthBySKResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAuthBySKResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAuthBySKResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAuthBySKResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMAuthBySKResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAuthBySKResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMAuthBySKResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMAuthBySKResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMAuthBySKResponse_t * pDestStruct, const IMAuthBySKResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMAuthBySKResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMAuthBySKResponse_t * ptStruct1, IMAuthBySKResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMAuthBySKResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMAuthBySKResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAuthBySKResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAuthBySKResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAuthBySKResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMAuthBySKResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAuthBySKResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}

//ÂèëÈ?ÅÊ∂àÊÅ?
int IMMsgPickle :: ToBuffer( const IMMsgRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMsgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMsgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMsgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMMsgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMsgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMMsgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMMsgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMMsgRequest_t * pDestStruct, const IMMsgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMMsgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMMsgRequest_t * ptStruct1, IMMsgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMMsgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMMsgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMsgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMsgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMsgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMMsgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMsgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMSendMsgRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendMsgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendMsgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendMsgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSendMsgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendMsgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSendMsgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSendMsgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSendMsgRequest_t * pDestStruct, const IMSendMsgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSendMsgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSendMsgRequest_t * ptStruct1, IMSendMsgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSendMsgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSendMsgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendMsgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendMsgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendMsgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSendMsgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendMsgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMMsgResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMsgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMsgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMsgResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMMsgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMsgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMMsgResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMMsgResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMMsgResponse_t * pDestStruct, const IMMsgResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMMsgResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMMsgResponse_t * ptStruct1, IMMsgResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMMsgResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMMsgResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMsgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMsgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMsgResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMMsgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMsgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMSendMsgResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendMsgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendMsgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendMsgResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSendMsgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendMsgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSendMsgResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSendMsgResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSendMsgResponse_t * pDestStruct, const IMSendMsgResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSendMsgResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSendMsgResponse_t * ptStruct1, IMSendMsgResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSendMsgResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSendMsgResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendMsgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendMsgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendMsgResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSendMsgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendMsgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMAddMsg_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAddMsg, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAddMsg, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAddMsg_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMAddMsg, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAddMsg, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMAddMsg_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMAddMsg, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMAddMsg_t * pDestStruct, const IMAddMsg_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMAddMsg, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMAddMsg_t * ptStruct1, IMAddMsg_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMAddMsg, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMAddMsg_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAddMsg, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAddMsg, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAddMsg_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMAddMsg, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAddMsg, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMKeyVal_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMKeyVal, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMKeyVal, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMKeyVal_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMKeyVal, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMKeyVal, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMKeyVal_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMKeyVal, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMKeyVal_t * pDestStruct, const IMKeyVal_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMKeyVal, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMKeyVal_t * ptStruct1, IMKeyVal_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMKeyVal, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMKeyVal_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMKeyVal, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMKeyVal, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMKeyVal_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMKeyVal, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMKeyVal, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMSyncKey_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncKey, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncKey, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncKey_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncKey, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncKey, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSyncKey_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSyncKey, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSyncKey_t * pDestStruct, const IMSyncKey_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSyncKey, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSyncKey_t * ptStruct1, IMSyncKey_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSyncKey, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSyncKey_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncKey, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncKey, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncKey_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncKey, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncKey, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMNewSyncRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMNewSyncRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMNewSyncRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMNewSyncRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMNewSyncRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMNewSyncRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMNewSyncRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMNewSyncRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMNewSyncRequest_t * pDestStruct, const IMNewSyncRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMNewSyncRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMNewSyncRequest_t * ptStruct1, IMNewSyncRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMNewSyncRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMNewSyncRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMNewSyncRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMNewSyncRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMNewSyncRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMNewSyncRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMNewSyncRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMNewSyncResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMNewSyncResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMNewSyncResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMNewSyncResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMNewSyncResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMNewSyncResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMNewSyncResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMNewSyncResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMNewSyncResponse_t * pDestStruct, const IMNewSyncResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMNewSyncResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMNewSyncResponse_t * ptStruct1, IMNewSyncResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMNewSyncResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMNewSyncResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMNewSyncResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMNewSyncResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMNewSyncResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMNewSyncResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMNewSyncResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMSyncRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSyncRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSyncRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSyncRequest_t * pDestStruct, const IMSyncRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSyncRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSyncRequest_t * ptStruct1, IMSyncRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSyncRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSyncRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMSyncResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSyncResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSyncResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSyncResponse_t * pDestStruct, const IMSyncResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSyncResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSyncResponse_t * ptStruct1, IMSyncResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSyncResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSyncResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMCmdList_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCmdList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCmdList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCmdList_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCmdList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCmdList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMCmdList_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCmdList, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCmdList_t * pDestStruct, const IMCmdList_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCmdList, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCmdList_t * ptStruct1, IMCmdList_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCmdList, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCmdList_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCmdList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCmdList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCmdList_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCmdList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCmdList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//=============================================

int IMMsgPickle :: ToBuffer( const IMOpCmdResp_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMOpCmdResp, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMOpCmdResp, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMOpCmdResp_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMOpCmdResp, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMOpCmdResp, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMOpCmdResp_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMOpCmdResp, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMOpCmdResp_t * pDestStruct, const IMOpCmdResp_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMOpCmdResp, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMOpCmdResp_t * ptStruct1, IMOpCmdResp_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMOpCmdResp, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMOpCmdResp_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMOpCmdResp, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMOpCmdResp, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMOpCmdResp_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMOpCmdResp, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMOpCmdResp, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//==============================================

int IMMsgPickle :: ToBuffer( const IMOpCmdRespList_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMOpCmdRespList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMOpCmdRespList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMOpCmdRespList_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMOpCmdRespList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMOpCmdRespList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMOpCmdRespList_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMOpCmdRespList, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMOpCmdRespList_t * pDestStruct, const IMOpCmdRespList_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMOpCmdRespList, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMOpCmdRespList_t * ptStruct1, IMOpCmdRespList_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMOpCmdRespList, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMOpCmdRespList_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMOpCmdRespList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMOpCmdRespList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMOpCmdRespList_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMOpCmdRespList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMOpCmdRespList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//=========================================

int IMMsgPickle :: ToBuffer( const IMBatchGetHeadImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBatchGetHeadImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBatchGetHeadImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBatchGetHeadImgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMBatchGetHeadImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBatchGetHeadImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMBatchGetHeadImgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMBatchGetHeadImgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMBatchGetHeadImgRequest_t * pDestStruct, const IMBatchGetHeadImgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMBatchGetHeadImgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMBatchGetHeadImgRequest_t * ptStruct1, IMBatchGetHeadImgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMBatchGetHeadImgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMBatchGetHeadImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBatchGetHeadImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBatchGetHeadImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBatchGetHeadImgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMBatchGetHeadImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBatchGetHeadImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMImgPair_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMImgPair, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMImgPair, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMImgPair_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMImgPair, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMImgPair, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMImgPair_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMImgPair, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMImgPair_t * pDestStruct, const IMImgPair_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMImgPair, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMImgPair_t * ptStruct1, IMImgPair_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMImgPair, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMImgPair_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMImgPair, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMImgPair, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMImgPair_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMImgPair, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMImgPair, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMBatchGetHeadImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBatchGetHeadImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBatchGetHeadImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBatchGetHeadImgResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMBatchGetHeadImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBatchGetHeadImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMBatchGetHeadImgResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMBatchGetHeadImgResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMBatchGetHeadImgResponse_t * pDestStruct, const IMBatchGetHeadImgResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMBatchGetHeadImgResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMBatchGetHeadImgResponse_t * ptStruct1, IMBatchGetHeadImgResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMBatchGetHeadImgResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMBatchGetHeadImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBatchGetHeadImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBatchGetHeadImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBatchGetHeadImgResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMBatchGetHeadImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBatchGetHeadImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMUploadHDHeadImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadHDHeadImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadHDHeadImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadHDHeadImgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadHDHeadImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadHDHeadImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadHDHeadImgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadHDHeadImgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadHDHeadImgRequest_t * pDestStruct, const IMUploadHDHeadImgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadHDHeadImgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadHDHeadImgRequest_t * ptStruct1, IMUploadHDHeadImgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadHDHeadImgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadHDHeadImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadHDHeadImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadHDHeadImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadHDHeadImgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadHDHeadImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadHDHeadImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMUploadHDHeadImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadHDHeadImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadHDHeadImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadHDHeadImgResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadHDHeadImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadHDHeadImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadHDHeadImgResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadHDHeadImgResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadHDHeadImgResponse_t * pDestStruct, const IMUploadHDHeadImgResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadHDHeadImgResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadHDHeadImgResponse_t * ptStruct1, IMUploadHDHeadImgResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadHDHeadImgResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadHDHeadImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadHDHeadImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadHDHeadImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadHDHeadImgResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadHDHeadImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadHDHeadImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMGetHDHeadImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetHDHeadImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetHDHeadImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetHDHeadImgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGetHDHeadImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetHDHeadImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMGetHDHeadImgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGetHDHeadImgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGetHDHeadImgRequest_t * pDestStruct, const IMGetHDHeadImgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGetHDHeadImgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGetHDHeadImgRequest_t * ptStruct1, IMGetHDHeadImgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGetHDHeadImgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGetHDHeadImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetHDHeadImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetHDHeadImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetHDHeadImgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGetHDHeadImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetHDHeadImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMGetHDHeadImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetHDHeadImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetHDHeadImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetHDHeadImgResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGetHDHeadImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetHDHeadImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMGetHDHeadImgResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGetHDHeadImgResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGetHDHeadImgResponse_t * pDestStruct, const IMGetHDHeadImgResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGetHDHeadImgResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGetHDHeadImgResponse_t * ptStruct1, IMGetHDHeadImgResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGetHDHeadImgResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGetHDHeadImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetHDHeadImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetHDHeadImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetHDHeadImgResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGetHDHeadImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetHDHeadImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMUploadVoiceRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadVoiceRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadVoiceRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadVoiceRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadVoiceRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadVoiceRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadVoiceRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadVoiceRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadVoiceRequest_t * pDestStruct, const IMUploadVoiceRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadVoiceRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadVoiceRequest_t * ptStruct1, IMUploadVoiceRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadVoiceRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadVoiceRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadVoiceRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadVoiceRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadVoiceRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadVoiceRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadVoiceRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMUploadVoiceResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadVoiceResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadVoiceResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadVoiceResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadVoiceResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadVoiceResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadVoiceResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadVoiceResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadVoiceResponse_t * pDestStruct, const IMUploadVoiceResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadVoiceResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadVoiceResponse_t * ptStruct1, IMUploadVoiceResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadVoiceResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadVoiceResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadVoiceResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadVoiceResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadVoiceResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadVoiceResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadVoiceResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMDownloadVoiceRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadVoiceRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadVoiceRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadVoiceRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadVoiceRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadVoiceRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDownloadVoiceRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDownloadVoiceRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDownloadVoiceRequest_t * pDestStruct, const IMDownloadVoiceRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDownloadVoiceRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDownloadVoiceRequest_t * ptStruct1, IMDownloadVoiceRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDownloadVoiceRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDownloadVoiceRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadVoiceRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadVoiceRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadVoiceRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadVoiceRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadVoiceRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMDownloadVoiceResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadVoiceResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadVoiceResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadVoiceResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadVoiceResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadVoiceResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDownloadVoiceResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDownloadVoiceResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDownloadVoiceResponse_t * pDestStruct, const IMDownloadVoiceResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDownloadVoiceResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDownloadVoiceResponse_t * ptStruct1, IMDownloadVoiceResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDownloadVoiceResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDownloadVoiceResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadVoiceResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadVoiceResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadVoiceResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadVoiceResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadVoiceResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMUploadVideoRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadVideoRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadVideoRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadVideoRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadVideoRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadVideoRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadVideoRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadVideoRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadVideoRequest_t * pDestStruct, const IMUploadVideoRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadVideoRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadVideoRequest_t * ptStruct1, IMUploadVideoRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadVideoRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadVideoRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadVideoRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadVideoRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadVideoRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadVideoRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadVideoRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMUploadVideoResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadVideoResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadVideoResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadVideoResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadVideoResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadVideoResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadVideoResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadVideoResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadVideoResponse_t * pDestStruct, const IMUploadVideoResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadVideoResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadVideoResponse_t * ptStruct1, IMUploadVideoResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadVideoResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadVideoResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadVideoResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadVideoResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadVideoResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadVideoResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadVideoResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMDownloadVideoRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadVideoRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadVideoRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadVideoRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadVideoRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadVideoRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDownloadVideoRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDownloadVideoRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDownloadVideoRequest_t * pDestStruct, const IMDownloadVideoRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDownloadVideoRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDownloadVideoRequest_t * ptStruct1, IMDownloadVideoRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDownloadVideoRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDownloadVideoRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadVideoRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadVideoRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadVideoRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadVideoRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadVideoRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMDownloadVideoResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadVideoResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadVideoResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadVideoResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadVideoResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadVideoResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDownloadVideoResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDownloadVideoResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDownloadVideoResponse_t * pDestStruct, const IMDownloadVideoResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDownloadVideoResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDownloadVideoResponse_t * ptStruct1, IMDownloadVideoResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDownloadVideoResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDownloadVideoResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadVideoResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadVideoResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadVideoResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadVideoResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadVideoResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMUploadMsgImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadMsgImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadMsgImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadMsgImgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadMsgImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadMsgImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadMsgImgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadMsgImgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadMsgImgRequest_t * pDestStruct, const IMUploadMsgImgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadMsgImgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadMsgImgRequest_t * ptStruct1, IMUploadMsgImgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadMsgImgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadMsgImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadMsgImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadMsgImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadMsgImgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadMsgImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadMsgImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMUploadMsgImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadMsgImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadMsgImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadMsgImgResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadMsgImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadMsgImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadMsgImgResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadMsgImgResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadMsgImgResponse_t * pDestStruct, const IMUploadMsgImgResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadMsgImgResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadMsgImgResponse_t * ptStruct1, IMUploadMsgImgResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadMsgImgResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadMsgImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadMsgImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadMsgImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadMsgImgResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadMsgImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadMsgImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMDownloadMsgImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadMsgImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadMsgImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadMsgImgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadMsgImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadMsgImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDownloadMsgImgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDownloadMsgImgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDownloadMsgImgRequest_t * pDestStruct, const IMDownloadMsgImgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDownloadMsgImgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDownloadMsgImgRequest_t * ptStruct1, IMDownloadMsgImgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDownloadMsgImgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDownloadMsgImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadMsgImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadMsgImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadMsgImgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadMsgImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadMsgImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMDownloadMsgImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadMsgImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadMsgImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadMsgImgResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadMsgImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadMsgImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDownloadMsgImgResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDownloadMsgImgResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDownloadMsgImgResponse_t * pDestStruct, const IMDownloadMsgImgResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDownloadMsgImgResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDownloadMsgImgResponse_t * ptStruct1, IMDownloadMsgImgResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDownloadMsgImgResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDownloadMsgImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadMsgImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadMsgImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadMsgImgResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadMsgImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadMsgImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMEmojiUploadInfoReq_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmojiUploadInfoReq, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmojiUploadInfoReq, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmojiUploadInfoReq_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMEmojiUploadInfoReq, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmojiUploadInfoReq, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMEmojiUploadInfoReq_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMEmojiUploadInfoReq, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMEmojiUploadInfoReq_t * pDestStruct, const IMEmojiUploadInfoReq_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMEmojiUploadInfoReq, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMEmojiUploadInfoReq_t * ptStruct1, IMEmojiUploadInfoReq_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMEmojiUploadInfoReq, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMEmojiUploadInfoReq_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmojiUploadInfoReq, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmojiUploadInfoReq, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmojiUploadInfoReq_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMEmojiUploadInfoReq, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmojiUploadInfoReq, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMUploadEmojiRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadEmojiRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadEmojiRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadEmojiRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadEmojiRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadEmojiRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadEmojiRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadEmojiRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadEmojiRequest_t * pDestStruct, const IMUploadEmojiRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadEmojiRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadEmojiRequest_t * ptStruct1, IMUploadEmojiRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadEmojiRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadEmojiRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadEmojiRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadEmojiRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadEmojiRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadEmojiRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadEmojiRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMEmojiUploadInfoResp_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmojiUploadInfoResp, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmojiUploadInfoResp, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmojiUploadInfoResp_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMEmojiUploadInfoResp, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmojiUploadInfoResp, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMEmojiUploadInfoResp_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMEmojiUploadInfoResp, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMEmojiUploadInfoResp_t * pDestStruct, const IMEmojiUploadInfoResp_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMEmojiUploadInfoResp, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMEmojiUploadInfoResp_t * ptStruct1, IMEmojiUploadInfoResp_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMEmojiUploadInfoResp, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMEmojiUploadInfoResp_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmojiUploadInfoResp, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmojiUploadInfoResp, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmojiUploadInfoResp_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMEmojiUploadInfoResp, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmojiUploadInfoResp, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMUploadEmojiResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadEmojiResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadEmojiResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadEmojiResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadEmojiResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadEmojiResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadEmojiResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadEmojiResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadEmojiResponse_t * pDestStruct, const IMUploadEmojiResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadEmojiResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadEmojiResponse_t * ptStruct1, IMUploadEmojiResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadEmojiResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadEmojiResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadEmojiResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadEmojiResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadEmojiResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadEmojiResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadEmojiResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMEmojiDownloadInfoReq_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmojiDownloadInfoReq, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmojiDownloadInfoReq, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmojiDownloadInfoReq_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMEmojiDownloadInfoReq, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmojiDownloadInfoReq, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMEmojiDownloadInfoReq_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMEmojiDownloadInfoReq, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMEmojiDownloadInfoReq_t * pDestStruct, const IMEmojiDownloadInfoReq_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMEmojiDownloadInfoReq, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMEmojiDownloadInfoReq_t * ptStruct1, IMEmojiDownloadInfoReq_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMEmojiDownloadInfoReq, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMEmojiDownloadInfoReq_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmojiDownloadInfoReq, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmojiDownloadInfoReq, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmojiDownloadInfoReq_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMEmojiDownloadInfoReq, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmojiDownloadInfoReq, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMDownloadEmojiRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadEmojiRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadEmojiRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadEmojiRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadEmojiRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadEmojiRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDownloadEmojiRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDownloadEmojiRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDownloadEmojiRequest_t * pDestStruct, const IMDownloadEmojiRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDownloadEmojiRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDownloadEmojiRequest_t * ptStruct1, IMDownloadEmojiRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDownloadEmojiRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDownloadEmojiRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadEmojiRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadEmojiRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadEmojiRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadEmojiRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadEmojiRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMEmojiDownloadInfoResp_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmojiDownloadInfoResp, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmojiDownloadInfoResp, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmojiDownloadInfoResp_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMEmojiDownloadInfoResp, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmojiDownloadInfoResp, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMEmojiDownloadInfoResp_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMEmojiDownloadInfoResp, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMEmojiDownloadInfoResp_t * pDestStruct, const IMEmojiDownloadInfoResp_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMEmojiDownloadInfoResp, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMEmojiDownloadInfoResp_t * ptStruct1, IMEmojiDownloadInfoResp_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMEmojiDownloadInfoResp, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMEmojiDownloadInfoResp_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmojiDownloadInfoResp, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmojiDownloadInfoResp, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmojiDownloadInfoResp_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMEmojiDownloadInfoResp, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmojiDownloadInfoResp, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMDownloadEmojiResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadEmojiResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadEmojiResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadEmojiResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadEmojiResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadEmojiResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDownloadEmojiResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDownloadEmojiResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDownloadEmojiResponse_t * pDestStruct, const IMDownloadEmojiResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDownloadEmojiResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDownloadEmojiResponse_t * ptStruct1, IMDownloadEmojiResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDownloadEmojiResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDownloadEmojiResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDownloadEmojiResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDownloadEmojiResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDownloadEmojiResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDownloadEmojiResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDownloadEmojiResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMLbsRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMLbsRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMLbsRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMLbsRequest_t * pDestStruct, const IMLbsRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMLbsRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMLbsRequest_t * ptStruct1, IMLbsRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMLbsRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMLbsRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMLbsContactInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsContactInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsContactInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsContactInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsContactInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsContactInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMLbsContactInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMLbsContactInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMLbsContactInfo_t * pDestStruct, const IMLbsContactInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMLbsContactInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMLbsContactInfo_t * ptStruct1, IMLbsContactInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMLbsContactInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMLbsContactInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsContactInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsContactInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsContactInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsContactInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsContactInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMLbsResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMLbsResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMLbsResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMLbsResponse_t * pDestStruct, const IMLbsResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMLbsResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMLbsResponse_t * ptStruct1, IMLbsResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMLbsResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMLbsResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMGetLocationRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetLocationRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetLocationRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetLocationRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGetLocationRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetLocationRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMGetLocationRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGetLocationRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGetLocationRequest_t * pDestStruct, const IMGetLocationRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGetLocationRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGetLocationRequest_t * ptStruct1, IMGetLocationRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGetLocationRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGetLocationRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetLocationRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetLocationRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetLocationRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGetLocationRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetLocationRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMGetLocationResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetLocationResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetLocationResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetLocationResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGetLocationResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetLocationResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMGetLocationResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGetLocationResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGetLocationResponse_t * pDestStruct, const IMGetLocationResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGetLocationResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGetLocationResponse_t * ptStruct1, IMGetLocationResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGetLocationResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGetLocationResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetLocationResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetLocationResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetLocationResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGetLocationResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetLocationResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMLbsMatchRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsMatchRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsMatchRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsMatchRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsMatchRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsMatchRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMLbsMatchRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMLbsMatchRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMLbsMatchRequest_t * pDestStruct, const IMLbsMatchRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMLbsMatchRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMLbsMatchRequest_t * ptStruct1, IMLbsMatchRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMLbsMatchRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMLbsMatchRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsMatchRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsMatchRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsMatchRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsMatchRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsMatchRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMLbsMatchResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsMatchResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsMatchResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsMatchResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsMatchResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsMatchResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMLbsMatchResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMLbsMatchResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMLbsMatchResponse_t * pDestStruct, const IMLbsMatchResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMLbsMatchResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMLbsMatchResponse_t * ptStruct1, IMLbsMatchResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMLbsMatchResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMLbsMatchResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsMatchResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsMatchResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsMatchResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsMatchResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsMatchResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMSyncLbsMatchRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncLbsMatchRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncLbsMatchRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncLbsMatchRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncLbsMatchRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncLbsMatchRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSyncLbsMatchRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSyncLbsMatchRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSyncLbsMatchRequest_t * pDestStruct, const IMSyncLbsMatchRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSyncLbsMatchRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSyncLbsMatchRequest_t * ptStruct1, IMSyncLbsMatchRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSyncLbsMatchRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSyncLbsMatchRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncLbsMatchRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncLbsMatchRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncLbsMatchRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncLbsMatchRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncLbsMatchRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMSyncLbsMatchResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncLbsMatchResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncLbsMatchResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncLbsMatchResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncLbsMatchResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncLbsMatchResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSyncLbsMatchResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSyncLbsMatchResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSyncLbsMatchResponse_t * pDestStruct, const IMSyncLbsMatchResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSyncLbsMatchResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSyncLbsMatchResponse_t * ptStruct1, IMSyncLbsMatchResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSyncLbsMatchResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSyncLbsMatchResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSyncLbsMatchResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSyncLbsMatchResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSyncLbsMatchResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSyncLbsMatchResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSyncLbsMatchResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMLbsMatchContactInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsMatchContactInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsMatchContactInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsMatchContactInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsMatchContactInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsMatchContactInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMLbsMatchContactInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMLbsMatchContactInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMLbsMatchContactInfo_t * pDestStruct, const IMLbsMatchContactInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMLbsMatchContactInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMLbsMatchContactInfo_t * ptStruct1, IMLbsMatchContactInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMLbsMatchContactInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMLbsMatchContactInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsMatchContactInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsMatchContactInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsMatchContactInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsMatchContactInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsMatchContactInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



/////////////////////////// »∫œ‡πÿ Begin ///////////////////////////

int IMMsgPickle :: ToBuffer( const IMModChatRoomNotify_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModChatRoomNotify, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModChatRoomNotify, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModChatRoomNotify_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModChatRoomNotify, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModChatRoomNotify, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModChatRoomNotify_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModChatRoomNotify, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModChatRoomNotify_t * pDestStruct, const IMModChatRoomNotify_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModChatRoomNotify, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModChatRoomNotify_t * ptStruct1, IMModChatRoomNotify_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModChatRoomNotify, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModChatRoomNotify_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModChatRoomNotify, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModChatRoomNotify, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModChatRoomNotify_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModChatRoomNotify, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModChatRoomNotify, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMModChatRoomMember_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModChatRoomMember, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModChatRoomMember, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModChatRoomMember_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModChatRoomMember, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModChatRoomMember, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModChatRoomMember_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModChatRoomMember, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModChatRoomMember_t * pDestStruct, const IMModChatRoomMember_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModChatRoomMember, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModChatRoomMember_t * ptStruct1, IMModChatRoomMember_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModChatRoomMember, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModChatRoomMember_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModChatRoomMember, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModChatRoomMember, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModChatRoomMember_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModChatRoomMember, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModChatRoomMember, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}




int IMMsgPickle :: ToBuffer( const IMQuitChatRoom_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMQuitChatRoom, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMQuitChatRoom, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMQuitChatRoom_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMQuitChatRoom, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMQuitChatRoom, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMQuitChatRoom_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMQuitChatRoom, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMQuitChatRoom_t * pDestStruct, const IMQuitChatRoom_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMQuitChatRoom, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMQuitChatRoom_t * ptStruct1, IMQuitChatRoom_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMQuitChatRoom, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMQuitChatRoom_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMQuitChatRoom, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMQuitChatRoom, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMQuitChatRoom_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMQuitChatRoom, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMQuitChatRoom, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMQuitChatRoomRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMQuitChatRoomRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMQuitChatRoomRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMQuitChatRoomRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMQuitChatRoomRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMQuitChatRoomRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMQuitChatRoomRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMQuitChatRoomRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMQuitChatRoomRequest_t * pDestStruct, const IMQuitChatRoomRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMQuitChatRoomRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMQuitChatRoomRequest_t * ptStruct1, IMQuitChatRoomRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMQuitChatRoomRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMQuitChatRoomRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMQuitChatRoomRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMQuitChatRoomRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMQuitChatRoomRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMQuitChatRoomRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMQuitChatRoomRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}




int IMMsgPickle :: ToBuffer( const IMQuitChatRoomResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMQuitChatRoomResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMQuitChatRoomResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMQuitChatRoomResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMQuitChatRoomResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMQuitChatRoomResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMQuitChatRoomResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMQuitChatRoomResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMQuitChatRoomResponse_t * pDestStruct, const IMQuitChatRoomResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMQuitChatRoomResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMQuitChatRoomResponse_t * ptStruct1, IMQuitChatRoomResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMQuitChatRoomResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMQuitChatRoomResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMQuitChatRoomResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMQuitChatRoomResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMQuitChatRoomResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMQuitChatRoomResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMQuitChatRoomResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMQuitGroupRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMQuitGroupRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMQuitGroupRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMQuitGroupRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMQuitGroupRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMQuitGroupRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMQuitGroupRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMQuitGroupRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMQuitGroupRequest_t * pDestStruct, const IMQuitGroupRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMQuitGroupRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMQuitGroupRequest_t * ptStruct1, IMQuitGroupRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMQuitGroupRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMQuitGroupRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMQuitGroupRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMQuitGroupRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMQuitGroupRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMQuitGroupRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMQuitGroupRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}




int IMMsgPickle :: ToBuffer( const IMQuitGroupResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMQuitGroupResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMQuitGroupResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMQuitGroupResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMQuitGroupResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMQuitGroupResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMQuitGroupResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMQuitGroupResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMQuitGroupResponse_t * pDestStruct, const IMQuitGroupResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMQuitGroupResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMQuitGroupResponse_t * ptStruct1, IMQuitGroupResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMQuitGroupResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMQuitGroupResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMQuitGroupResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMQuitGroupResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMQuitGroupResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMQuitGroupResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMQuitGroupResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}





int IMMsgPickle :: ToBuffer( const IMModChatRoomTopic_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModChatRoomTopic, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModChatRoomTopic, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModChatRoomTopic_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModChatRoomTopic, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModChatRoomTopic, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModChatRoomTopic_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModChatRoomTopic, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModChatRoomTopic_t * pDestStruct, const IMModChatRoomTopic_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModChatRoomTopic, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModChatRoomTopic_t * ptStruct1, IMModChatRoomTopic_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModChatRoomTopic, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModChatRoomTopic_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModChatRoomTopic, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModChatRoomTopic, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModChatRoomTopic_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModChatRoomTopic, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModChatRoomTopic, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}





int IMMsgPickle :: ToBuffer( const IMModGroupTopic_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModGroupTopic, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModGroupTopic, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModGroupTopic_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModGroupTopic, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModGroupTopic, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModGroupTopic_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModGroupTopic, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModGroupTopic_t * pDestStruct, const IMModGroupTopic_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModGroupTopic, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModGroupTopic_t * ptStruct1, IMModGroupTopic_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModGroupTopic, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModGroupTopic_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModGroupTopic, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModGroupTopic, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModGroupTopic_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModGroupTopic, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModGroupTopic, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}





int IMMsgPickle :: ToBuffer( const IMModChatRoomMemberDisplayName_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModChatRoomMemberDisplayName, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModChatRoomMemberDisplayName, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModChatRoomMemberDisplayName_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModChatRoomMemberDisplayName, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModChatRoomMemberDisplayName, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModChatRoomMemberDisplayName_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModChatRoomMemberDisplayName, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModChatRoomMemberDisplayName_t * pDestStruct, const IMModChatRoomMemberDisplayName_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModChatRoomMemberDisplayName, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModChatRoomMemberDisplayName_t * ptStruct1, IMModChatRoomMemberDisplayName_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModChatRoomMemberDisplayName, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModChatRoomMemberDisplayName_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModChatRoomMemberDisplayName, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModChatRoomMemberDisplayName, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModChatRoomMemberDisplayName_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModChatRoomMemberDisplayName, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModChatRoomMemberDisplayName, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMModChatRoomMemberFlag_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModChatRoomMemberFlag, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModChatRoomMemberFlag, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModChatRoomMemberFlag_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModChatRoomMemberFlag, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModChatRoomMemberFlag, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModChatRoomMemberFlag_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModChatRoomMemberFlag, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModChatRoomMemberFlag_t * pDestStruct, const IMModChatRoomMemberFlag_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModChatRoomMemberFlag, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModChatRoomMemberFlag_t * ptStruct1, IMModChatRoomMemberFlag_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModChatRoomMemberFlag, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModChatRoomMemberFlag_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModChatRoomMemberFlag, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModChatRoomMemberFlag, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModChatRoomMemberFlag_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModChatRoomMemberFlag, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModChatRoomMemberFlag, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}





int IMMsgPickle :: ToBuffer( const IMMemberReq_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMemberReq, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMemberReq, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMemberReq_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMMemberReq, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMemberReq, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMMemberReq_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMMemberReq, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMMemberReq_t * pDestStruct, const IMMemberReq_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMMemberReq, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMMemberReq_t * ptStruct1, IMMemberReq_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMMemberReq, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMMemberReq_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMemberReq, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMemberReq, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMemberReq_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMMemberReq, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMemberReq, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}




int IMMsgPickle :: ToBuffer( const IMMemberResp_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMemberResp, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMemberResp, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMemberResp_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMMemberResp, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMemberResp, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMMemberResp_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMMemberResp, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMMemberResp_t * pDestStruct, const IMMemberResp_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMMemberResp, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMMemberResp_t * ptStruct1, IMMemberResp_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMMemberResp, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMMemberResp_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMemberResp, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMemberResp, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMemberResp_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMMemberResp, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMemberResp, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}




int IMMsgPickle :: ToBuffer( const IMDelMemberReq_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelMemberReq, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelMemberReq, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelMemberReq_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDelMemberReq, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelMemberReq, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDelMemberReq_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDelMemberReq, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDelMemberReq_t * pDestStruct, const IMDelMemberReq_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDelMemberReq, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDelMemberReq_t * ptStruct1, IMDelMemberReq_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDelMemberReq, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDelMemberReq_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelMemberReq, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelMemberReq, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelMemberReq_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDelMemberReq, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelMemberReq, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}




int IMMsgPickle :: ToBuffer( const IMDelMemberResp_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelMemberResp, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelMemberResp, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelMemberResp_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDelMemberResp, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelMemberResp, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDelMemberResp_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDelMemberResp, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDelMemberResp_t * pDestStruct, const IMDelMemberResp_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDelMemberResp, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDelMemberResp_t * ptStruct1, IMDelMemberResp_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDelMemberResp, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDelMemberResp_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelMemberResp, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelMemberResp, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelMemberResp_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDelMemberResp, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelMemberResp, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMCreateChatRoomRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCreateChatRoomRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCreateChatRoomRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCreateChatRoomRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCreateChatRoomRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCreateChatRoomRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMCreateChatRoomRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCreateChatRoomRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCreateChatRoomRequest_t * pDestStruct, const IMCreateChatRoomRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCreateChatRoomRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCreateChatRoomRequest_t * ptStruct1, IMCreateChatRoomRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCreateChatRoomRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCreateChatRoomRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCreateChatRoomRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCreateChatRoomRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCreateChatRoomRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCreateChatRoomRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCreateChatRoomRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}






int IMMsgPickle :: ToBuffer( const IMCreateChatRoomResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCreateChatRoomResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCreateChatRoomResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCreateChatRoomResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCreateChatRoomResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCreateChatRoomResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMCreateChatRoomResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCreateChatRoomResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCreateChatRoomResponse_t * pDestStruct, const IMCreateChatRoomResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCreateChatRoomResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCreateChatRoomResponse_t * ptStruct1, IMCreateChatRoomResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCreateChatRoomResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCreateChatRoomResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCreateChatRoomResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCreateChatRoomResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCreateChatRoomResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCreateChatRoomResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCreateChatRoomResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMCreateGroupRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCreateGroupRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCreateGroupRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCreateGroupRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCreateGroupRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCreateGroupRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMCreateGroupRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCreateGroupRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCreateGroupRequest_t * pDestStruct, const IMCreateGroupRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCreateGroupRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCreateGroupRequest_t * ptStruct1, IMCreateGroupRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCreateGroupRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCreateGroupRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCreateGroupRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCreateGroupRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCreateGroupRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCreateGroupRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCreateGroupRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMCreateGroupResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCreateGroupResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCreateGroupResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCreateGroupResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCreateGroupResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCreateGroupResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMCreateGroupResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCreateGroupResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCreateGroupResponse_t * pDestStruct, const IMCreateGroupResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCreateGroupResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCreateGroupResponse_t * ptStruct1, IMCreateGroupResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCreateGroupResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCreateGroupResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCreateGroupResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCreateGroupResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCreateGroupResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCreateGroupResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCreateGroupResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMSearchChatRoomRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSearchChatRoomRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSearchChatRoomRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSearchChatRoomRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSearchChatRoomRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSearchChatRoomRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSearchChatRoomRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSearchChatRoomRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSearchChatRoomRequest_t * pDestStruct, const IMSearchChatRoomRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSearchChatRoomRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSearchChatRoomRequest_t * ptStruct1, IMSearchChatRoomRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSearchChatRoomRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSearchChatRoomRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSearchChatRoomRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSearchChatRoomRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSearchChatRoomRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSearchChatRoomRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSearchChatRoomRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}





int IMMsgPickle :: ToBuffer( const IMChatRoomResult_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMChatRoomResult, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMChatRoomResult, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMChatRoomResult_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMChatRoomResult, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMChatRoomResult, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMChatRoomResult_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMChatRoomResult, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMChatRoomResult_t * pDestStruct, const IMChatRoomResult_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMChatRoomResult, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMChatRoomResult_t * ptStruct1, IMChatRoomResult_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMChatRoomResult, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMChatRoomResult_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMChatRoomResult, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMChatRoomResult, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMChatRoomResult_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMChatRoomResult, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMChatRoomResult, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMSearchChatRoomResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSearchChatRoomResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSearchChatRoomResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSearchChatRoomResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSearchChatRoomResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSearchChatRoomResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSearchChatRoomResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSearchChatRoomResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSearchChatRoomResponse_t * pDestStruct, const IMSearchChatRoomResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSearchChatRoomResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSearchChatRoomResponse_t * ptStruct1, IMSearchChatRoomResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSearchChatRoomResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSearchChatRoomResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSearchChatRoomResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSearchChatRoomResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSearchChatRoomResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSearchChatRoomResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSearchChatRoomResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}




int IMMsgPickle :: ToBuffer( const IMAddChatRoomMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAddChatRoomMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAddChatRoomMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAddChatRoomMemberRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMAddChatRoomMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAddChatRoomMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMAddChatRoomMemberRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMAddChatRoomMemberRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMAddChatRoomMemberRequest_t * pDestStruct, const IMAddChatRoomMemberRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMAddChatRoomMemberRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMAddChatRoomMemberRequest_t * ptStruct1, IMAddChatRoomMemberRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMAddChatRoomMemberRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMAddChatRoomMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAddChatRoomMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAddChatRoomMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAddChatRoomMemberRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMAddChatRoomMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAddChatRoomMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMAddChatRoomMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAddChatRoomMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAddChatRoomMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAddChatRoomMemberResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMAddChatRoomMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAddChatRoomMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMAddChatRoomMemberResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMAddChatRoomMemberResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMAddChatRoomMemberResponse_t * pDestStruct, const IMAddChatRoomMemberResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMAddChatRoomMemberResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMAddChatRoomMemberResponse_t * ptStruct1, IMAddChatRoomMemberResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMAddChatRoomMemberResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMAddChatRoomMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAddChatRoomMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAddChatRoomMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAddChatRoomMemberResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMAddChatRoomMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAddChatRoomMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMDelChatRoomMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelChatRoomMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelChatRoomMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelChatRoomMemberRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDelChatRoomMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelChatRoomMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDelChatRoomMemberRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDelChatRoomMemberRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDelChatRoomMemberRequest_t * pDestStruct, const IMDelChatRoomMemberRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDelChatRoomMemberRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDelChatRoomMemberRequest_t * ptStruct1, IMDelChatRoomMemberRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDelChatRoomMemberRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDelChatRoomMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelChatRoomMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelChatRoomMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelChatRoomMemberRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDelChatRoomMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelChatRoomMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMDelChatRoomMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelChatRoomMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelChatRoomMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelChatRoomMemberResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDelChatRoomMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelChatRoomMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDelChatRoomMemberResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDelChatRoomMemberResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDelChatRoomMemberResponse_t * pDestStruct, const IMDelChatRoomMemberResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDelChatRoomMemberResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDelChatRoomMemberResponse_t * ptStruct1, IMDelChatRoomMemberResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDelChatRoomMemberResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDelChatRoomMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelChatRoomMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelChatRoomMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelChatRoomMemberResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDelChatRoomMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelChatRoomMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int IMMsgPickle :: ToBuffer( const IMAddGroupMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAddGroupMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAddGroupMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAddGroupMemberRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMAddGroupMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAddGroupMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMAddGroupMemberRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMAddGroupMemberRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMAddGroupMemberRequest_t * pDestStruct, const IMAddGroupMemberRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMAddGroupMemberRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMAddGroupMemberRequest_t * ptStruct1, IMAddGroupMemberRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMAddGroupMemberRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMAddGroupMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAddGroupMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAddGroupMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAddGroupMemberRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMAddGroupMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAddGroupMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMAddGroupMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAddGroupMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAddGroupMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAddGroupMemberResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMAddGroupMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAddGroupMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMAddGroupMemberResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMAddGroupMemberResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMAddGroupMemberResponse_t * pDestStruct, const IMAddGroupMemberResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMAddGroupMemberResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMAddGroupMemberResponse_t * ptStruct1, IMAddGroupMemberResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMAddGroupMemberResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMAddGroupMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMAddGroupMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMAddGroupMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMAddGroupMemberResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMAddGroupMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMAddGroupMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMDelGroupMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelGroupMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelGroupMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelGroupMemberRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDelGroupMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelGroupMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDelGroupMemberRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDelGroupMemberRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDelGroupMemberRequest_t * pDestStruct, const IMDelGroupMemberRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDelGroupMemberRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDelGroupMemberRequest_t * ptStruct1, IMDelGroupMemberRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDelGroupMemberRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDelGroupMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelGroupMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelGroupMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelGroupMemberRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDelGroupMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelGroupMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMDelGroupMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelGroupMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelGroupMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelGroupMemberResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDelGroupMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelGroupMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDelGroupMemberResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDelGroupMemberResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDelGroupMemberResponse_t * pDestStruct, const IMDelGroupMemberResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDelGroupMemberResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDelGroupMemberResponse_t * ptStruct1, IMDelGroupMemberResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDelGroupMemberResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDelGroupMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDelGroupMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDelGroupMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDelGroupMemberResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDelGroupMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDelGroupMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int IMMsgPickle :: ToBuffer( const IMLbsChatRoomRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsChatRoomRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsChatRoomRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsChatRoomRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsChatRoomRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsChatRoomRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMLbsChatRoomRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMLbsChatRoomRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMLbsChatRoomRequest_t * pDestStruct, const IMLbsChatRoomRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMLbsChatRoomRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMLbsChatRoomRequest_t * ptStruct1, IMLbsChatRoomRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMLbsChatRoomRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMLbsChatRoomRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsChatRoomRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsChatRoomRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsChatRoomRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsChatRoomRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsChatRoomRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//============================
int IMMsgPickle :: ToBuffer( const IMChatRoomMember_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMChatRoomMember, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMChatRoomMember, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMChatRoomMember_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMChatRoomMember, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMChatRoomMember, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMChatRoomMember_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMChatRoomMember, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMChatRoomMember_t * pDestStruct, const IMChatRoomMember_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMChatRoomMember, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMChatRoomMember_t * ptStruct1, IMChatRoomMember_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMChatRoomMember, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMChatRoomMember_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMChatRoomMember, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMChatRoomMember, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMChatRoomMember_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMChatRoomMember, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMChatRoomMember, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//============================
int IMMsgPickle :: ToBuffer( const IMLbsChatRoom_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsChatRoom, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsChatRoom, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsChatRoom_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsChatRoom, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsChatRoom, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMLbsChatRoom_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMLbsChatRoom, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMLbsChatRoom_t * pDestStruct, const IMLbsChatRoom_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMLbsChatRoom, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMLbsChatRoom_t * ptStruct1, IMLbsChatRoom_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMLbsChatRoom, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMLbsChatRoom_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsChatRoom, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsChatRoom, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsChatRoom_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsChatRoom, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsChatRoom, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
//================================

int IMMsgPickle :: ToBuffer( const IMLbsChatRoomResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsChatRoomResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsChatRoomResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsChatRoomResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsChatRoomResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsChatRoomResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMLbsChatRoomResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMLbsChatRoomResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMLbsChatRoomResponse_t * pDestStruct, const IMLbsChatRoomResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMLbsChatRoomResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMLbsChatRoomResponse_t * ptStruct1, IMLbsChatRoomResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMLbsChatRoomResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMLbsChatRoomResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMLbsChatRoomResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMLbsChatRoomResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMLbsChatRoomResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMLbsChatRoomResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMLbsChatRoomResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//==============================================

int IMMsgPickle :: ToBuffer( const IMVerifyChatRoomMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMVerifyChatRoomMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMVerifyChatRoomMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMVerifyChatRoomMemberRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMVerifyChatRoomMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMVerifyChatRoomMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMVerifyChatRoomMemberRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMVerifyChatRoomMemberRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMVerifyChatRoomMemberRequest_t * pDestStruct, const IMVerifyChatRoomMemberRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMVerifyChatRoomMemberRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMVerifyChatRoomMemberRequest_t * ptStruct1, IMVerifyChatRoomMemberRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMVerifyChatRoomMemberRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMVerifyChatRoomMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMVerifyChatRoomMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMVerifyChatRoomMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMVerifyChatRoomMemberRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMVerifyChatRoomMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMVerifyChatRoomMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//==================================================

int IMMsgPickle :: ToBuffer( const IMVerifyChatRoomMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMVerifyChatRoomMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMVerifyChatRoomMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMVerifyChatRoomMemberResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMVerifyChatRoomMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMVerifyChatRoomMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMVerifyChatRoomMemberResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMVerifyChatRoomMemberResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMVerifyChatRoomMemberResponse_t * pDestStruct, const IMVerifyChatRoomMemberResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMVerifyChatRoomMemberResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMVerifyChatRoomMemberResponse_t * ptStruct1, IMVerifyChatRoomMemberResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMVerifyChatRoomMemberResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMVerifyChatRoomMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMVerifyChatRoomMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMVerifyChatRoomMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMVerifyChatRoomMemberResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMVerifyChatRoomMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMVerifyChatRoomMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//==========================================================

int IMMsgPickle :: ToBuffer( const IMInviteChatRoomMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMInviteChatRoomMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMInviteChatRoomMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMInviteChatRoomMemberRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMInviteChatRoomMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMInviteChatRoomMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMInviteChatRoomMemberRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMInviteChatRoomMemberRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMInviteChatRoomMemberRequest_t * pDestStruct, const IMInviteChatRoomMemberRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMInviteChatRoomMemberRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMInviteChatRoomMemberRequest_t * ptStruct1, IMInviteChatRoomMemberRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMInviteChatRoomMemberRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMInviteChatRoomMemberRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMInviteChatRoomMemberRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMInviteChatRoomMemberRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMInviteChatRoomMemberRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMInviteChatRoomMemberRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMInviteChatRoomMemberRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//==================================================

int IMMsgPickle :: ToBuffer( const IMInviteChatRoomMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMInviteChatRoomMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMInviteChatRoomMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMInviteChatRoomMemberResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMInviteChatRoomMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMInviteChatRoomMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMInviteChatRoomMemberResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMInviteChatRoomMemberResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMInviteChatRoomMemberResponse_t * pDestStruct, const IMInviteChatRoomMemberResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMInviteChatRoomMemberResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMInviteChatRoomMemberResponse_t * ptStruct1, IMInviteChatRoomMemberResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMInviteChatRoomMemberResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMInviteChatRoomMemberResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMInviteChatRoomMemberResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMInviteChatRoomMemberResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMInviteChatRoomMemberResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMInviteChatRoomMemberResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMInviteChatRoomMemberResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//======================================================

int IMMsgPickle :: ToBuffer( const IMCloseChatRoomRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCloseChatRoomRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCloseChatRoomRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCloseChatRoomRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCloseChatRoomRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCloseChatRoomRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMCloseChatRoomRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCloseChatRoomRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCloseChatRoomRequest_t * pDestStruct, const IMCloseChatRoomRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCloseChatRoomRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCloseChatRoomRequest_t * ptStruct1, IMCloseChatRoomRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCloseChatRoomRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCloseChatRoomRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCloseChatRoomRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCloseChatRoomRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCloseChatRoomRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCloseChatRoomRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCloseChatRoomRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

//============================================================

int IMMsgPickle :: ToBuffer( const IMCloseChatRoomResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCloseChatRoomResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCloseChatRoomResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCloseChatRoomResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCloseChatRoomResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCloseChatRoomResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMCloseChatRoomResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCloseChatRoomResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCloseChatRoomResponse_t * pDestStruct, const IMCloseChatRoomResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCloseChatRoomResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCloseChatRoomResponse_t * ptStruct1, IMCloseChatRoomResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCloseChatRoomResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCloseChatRoomResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCloseChatRoomResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCloseChatRoomResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCloseChatRoomResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCloseChatRoomResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCloseChatRoomResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


////////////////////////// »∫œ‡πÿ End //////////////////////////////
int IMMsgPickle :: ToBuffer( const IMDisturbTimeSpan_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDisturbTimeSpan, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDisturbTimeSpan, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDisturbTimeSpan_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDisturbTimeSpan, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDisturbTimeSpan, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDisturbTimeSpan_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDisturbTimeSpan, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDisturbTimeSpan_t * pDestStruct, const IMDisturbTimeSpan_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDisturbTimeSpan, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDisturbTimeSpan_t * ptStruct1, IMDisturbTimeSpan_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDisturbTimeSpan, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDisturbTimeSpan_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDisturbTimeSpan, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDisturbTimeSpan, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDisturbTimeSpan_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDisturbTimeSpan, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDisturbTimeSpan, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMDisturbSetting_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDisturbSetting, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDisturbSetting, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDisturbSetting_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMDisturbSetting, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDisturbSetting, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMDisturbSetting_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMDisturbSetting, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMDisturbSetting_t * pDestStruct, const IMDisturbSetting_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMDisturbSetting, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMDisturbSetting_t * ptStruct1, IMDisturbSetting_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMDisturbSetting, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMDisturbSetting_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMDisturbSetting, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMDisturbSetting, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMDisturbSetting_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMDisturbSetting, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMDisturbSetting, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMGmailInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGmailInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGmailInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGmailInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGmailInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGmailInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMGmailInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGmailInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGmailInfo_t * pDestStruct, const IMGmailInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGmailInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGmailInfo_t * ptStruct1, IMGmailInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGmailInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGmailInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGmailInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGmailInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGmailInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGmailInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGmailInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMGmailList_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGmailList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGmailList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGmailList_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGmailList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGmailList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMGmailList_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGmailList, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGmailList_t * pDestStruct, const IMGmailList_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGmailList, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGmailList_t * ptStruct1, IMGmailList_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGmailList, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGmailList_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGmailList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGmailList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGmailList_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGmailList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGmailList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMFacebookInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMFacebookInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMFacebookInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMFacebookInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMFacebookInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMFacebookInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMFacebookInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMFacebookInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMFacebookInfo_t * pDestStruct, const IMFacebookInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMFacebookInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMFacebookInfo_t * ptStruct1, IMFacebookInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMFacebookInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMFacebookInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMFacebookInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMFacebookInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMFacebookInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMFacebookInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMFacebookInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMSafeDevice_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSafeDevice, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSafeDevice, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSafeDevice_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSafeDevice, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSafeDevice, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSafeDevice_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSafeDevice, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSafeDevice_t * pDestStruct, const IMSafeDevice_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSafeDevice, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSafeDevice_t * ptStruct1, IMSafeDevice_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSafeDevice, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSafeDevice_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSafeDevice, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSafeDevice, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSafeDevice_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSafeDevice, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSafeDevice, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMSafeDeviceList_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSafeDeviceList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSafeDeviceList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSafeDeviceList_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSafeDeviceList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSafeDeviceList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSafeDeviceList_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSafeDeviceList, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSafeDeviceList_t * pDestStruct, const IMSafeDeviceList_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSafeDeviceList, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSafeDeviceList_t * ptStruct1, IMSafeDeviceList_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSafeDeviceList, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSafeDeviceList_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSafeDeviceList, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSafeDeviceList, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSafeDeviceList_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSafeDeviceList, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSafeDeviceList, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMUserInfoExt_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUserInfoExt, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUserInfoExt, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUserInfoExt_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUserInfoExt, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUserInfoExt, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUserInfoExt_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUserInfoExt, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUserInfoExt_t * pDestStruct, const IMUserInfoExt_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUserInfoExt, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUserInfoExt_t * ptStruct1, IMUserInfoExt_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUserInfoExt, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUserInfoExt_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUserInfoExt, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUserInfoExt, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUserInfoExt_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUserInfoExt, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUserInfoExt, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMModUserInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModUserInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModUserInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModUserInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModUserInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModUserInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModUserInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModUserInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModUserInfo_t * pDestStruct, const IMModUserInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModUserInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModUserInfo_t * ptStruct1, IMModUserInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModUserInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModUserInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModUserInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModUserInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModUserInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModUserInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModUserInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMGetProfileRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetProfileRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetProfileRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetProfileRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGetProfileRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetProfileRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMGetProfileRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGetProfileRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGetProfileRequest_t * pDestStruct, const IMGetProfileRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGetProfileRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGetProfileRequest_t * ptStruct1, IMGetProfileRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGetProfileRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGetProfileRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetProfileRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetProfileRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetProfileRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGetProfileRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetProfileRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMGetProfileResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetProfileResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetProfileResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetProfileResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGetProfileResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetProfileResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMGetProfileResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGetProfileResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGetProfileResponse_t * pDestStruct, const IMGetProfileResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGetProfileResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGetProfileResponse_t * ptStruct1, IMGetProfileResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGetProfileResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGetProfileResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetProfileResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetProfileResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetProfileResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGetProfileResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetProfileResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMUploadCoverImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadCoverImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadCoverImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadCoverImgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadCoverImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadCoverImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadCoverImgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadCoverImgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadCoverImgRequest_t * pDestStruct, const IMUploadCoverImgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadCoverImgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadCoverImgRequest_t * ptStruct1, IMUploadCoverImgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadCoverImgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadCoverImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadCoverImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadCoverImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadCoverImgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadCoverImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadCoverImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMUploadCoverImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadCoverImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadCoverImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadCoverImgResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadCoverImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadCoverImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadCoverImgResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadCoverImgResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadCoverImgResponse_t * pDestStruct, const IMUploadCoverImgResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadCoverImgResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadCoverImgResponse_t * ptStruct1, IMUploadCoverImgResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadCoverImgResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadCoverImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadCoverImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadCoverImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadCoverImgResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadCoverImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadCoverImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMGetCoverImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetCoverImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetCoverImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetCoverImgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGetCoverImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetCoverImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMGetCoverImgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGetCoverImgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGetCoverImgRequest_t * pDestStruct, const IMGetCoverImgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGetCoverImgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGetCoverImgRequest_t * ptStruct1, IMGetCoverImgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGetCoverImgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGetCoverImgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetCoverImgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetCoverImgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetCoverImgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGetCoverImgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetCoverImgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMGetCoverImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetCoverImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetCoverImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetCoverImgResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMGetCoverImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetCoverImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMGetCoverImgResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMGetCoverImgResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMGetCoverImgResponse_t * pDestStruct, const IMGetCoverImgResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMGetCoverImgResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMGetCoverImgResponse_t * ptStruct1, IMGetCoverImgResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMGetCoverImgResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMGetCoverImgResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMGetCoverImgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMGetCoverImgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMGetCoverImgResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMGetCoverImgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMGetCoverImgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMHobby_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMHobby, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMHobby, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMHobby_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMHobby, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMHobby, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMHobby_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMHobby, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMHobby_t * pDestStruct, const IMHobby_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMHobby, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMHobby_t * ptStruct1, IMHobby_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMHobby, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMHobby_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMHobby, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMHobby, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMHobby_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMHobby, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMHobby, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMBirthday_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBirthday, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBirthday, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBirthday_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMBirthday, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBirthday, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMBirthday_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMBirthday, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMBirthday_t * pDestStruct, const IMBirthday_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMBirthday, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMBirthday_t * ptStruct1, IMBirthday_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMBirthday, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMBirthday_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMBirthday, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMBirthday, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMBirthday_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMBirthday, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMBirthday, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMFunctionSwitch_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMFunctionSwitch, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMFunctionSwitch, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMFunctionSwitch_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMFunctionSwitch, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMFunctionSwitch, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMFunctionSwitch_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMFunctionSwitch, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMFunctionSwitch_t * pDestStruct, const IMFunctionSwitch_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMFunctionSwitch, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMFunctionSwitch_t * ptStruct1, IMFunctionSwitch_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMFunctionSwitch, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMFunctionSwitch_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMFunctionSwitch, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMFunctionSwitch, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMFunctionSwitch_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMFunctionSwitch, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMFunctionSwitch, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


//////////////////////////////////////////////////

int IMMsgPickle :: ToBuffer( const IMMobileOptRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMobileOptRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMobileOptRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMobileOptRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMMobileOptRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMobileOptRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMMobileOptRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMMobileOptRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMMobileOptRequest_t * pDestStruct, const IMMobileOptRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMMobileOptRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMMobileOptRequest_t * ptStruct1, IMMobileOptRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMMobileOptRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMMobileOptRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMobileOptRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMobileOptRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMobileOptRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMMobileOptRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMobileOptRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}


int IMMsgPickle :: ToBuffer( const IMMobileOptResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMobileOptResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMobileOptResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMobileOptResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMMobileOptResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMobileOptResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMMobileOptResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMMobileOptResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMMobileOptResponse_t * pDestStruct, const IMMobileOptResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMMobileOptResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMMobileOptResponse_t * ptStruct1, IMMobileOptResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMMobileOptResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMMobileOptResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMobileOptResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMobileOptResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMMobileOptResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMMobileOptResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMMobileOptResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}

////////////////////////////////////////


int IMMsgPickle :: ToBuffer( const IMEmailOptRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmailOptRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmailOptRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmailOptRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMEmailOptRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmailOptRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMEmailOptRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMEmailOptRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMEmailOptRequest_t * pDestStruct, const IMEmailOptRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMEmailOptRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMEmailOptRequest_t * ptStruct1, IMEmailOptRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMEmailOptRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMEmailOptRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmailOptRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmailOptRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmailOptRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMEmailOptRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmailOptRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}


int IMMsgPickle :: ToBuffer( const IMEmailOptResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmailOptResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmailOptResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmailOptResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMEmailOptResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmailOptResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMEmailOptResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMEmailOptResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMEmailOptResponse_t * pDestStruct, const IMEmailOptResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMEmailOptResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMEmailOptResponse_t * ptStruct1, IMEmailOptResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMEmailOptResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMEmailOptResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMEmailOptResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMEmailOptResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMEmailOptResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMEmailOptResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMEmailOptResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}


int IMMsgPickle :: ToBuffer( const IMUploadPlugFriendsRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadPlugFriendsRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadPlugFriendsRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadPlugFriendsRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadPlugFriendsRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadPlugFriendsRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadPlugFriendsRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadPlugFriendsRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadPlugFriendsRequest_t * pDestStruct, const IMUploadPlugFriendsRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadPlugFriendsRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadPlugFriendsRequest_t * ptStruct1, IMUploadPlugFriendsRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadPlugFriendsRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadPlugFriendsRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadPlugFriendsRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadPlugFriendsRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadPlugFriendsRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadPlugFriendsRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadPlugFriendsRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}


int IMMsgPickle :: ToBuffer( const IMUploadPlugFriendsResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadPlugFriendsResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadPlugFriendsResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadPlugFriendsResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadPlugFriendsResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadPlugFriendsResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;
}

int IMMsgPickle :: FreeStructField( IMUploadPlugFriendsResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMUploadPlugFriendsResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMUploadPlugFriendsResponse_t * pDestStruct, const IMUploadPlugFriendsResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMUploadPlugFriendsResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMUploadPlugFriendsResponse_t * ptStruct1, IMUploadPlugFriendsResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMUploadPlugFriendsResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMUploadPlugFriendsResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMUploadPlugFriendsResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMUploadPlugFriendsResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}
	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMUploadPlugFriendsResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMUploadPlugFriendsResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMUploadPlugFriendsResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}
	return -1;

}

int IMMsgPickle :: ToBuffer( const IMPushSettingRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMPushSettingRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMPushSettingRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMPushSettingRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMPushSettingRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMPushSettingRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMPushSettingRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMPushSettingRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMPushSettingRequest_t * pDestStruct, const IMPushSettingRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMPushSettingRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMPushSettingRequest_t * ptStruct1, IMPushSettingRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMPushSettingRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMPushSettingRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMPushSettingRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMPushSettingRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMPushSettingRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMPushSettingRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMPushSettingRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMPushSettingResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMPushSettingResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMPushSettingResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMPushSettingResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMPushSettingResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMPushSettingResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMPushSettingResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMPushSettingResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMPushSettingResponse_t * pDestStruct, const IMPushSettingResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMPushSettingResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMPushSettingResponse_t * ptStruct1, IMPushSettingResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMPushSettingResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMPushSettingResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMPushSettingResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMPushSettingResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMPushSettingResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMPushSettingResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMPushSettingResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

///////////////////////////////

int IMMsgPickle :: ToBuffer( const IMModUserNameRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModUserNameRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModUserNameRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModUserNameRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModUserNameRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModUserNameRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModUserNameRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModUserNameRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModUserNameRequest_t * pDestStruct, const IMModUserNameRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModUserNameRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModUserNameRequest_t * ptStruct1, IMModUserNameRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModUserNameRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModUserNameRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModUserNameRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModUserNameRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModUserNameRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModUserNameRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModUserNameRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMModUserNameResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModUserNameResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModUserNameResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModUserNameResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModUserNameResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModUserNameResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModUserNameResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModUserNameResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModUserNameResponse_t * pDestStruct, const IMModUserNameResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModUserNameResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModUserNameResponse_t * ptStruct1, IMModUserNameResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModUserNameResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModUserNameResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModUserNameResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModUserNameResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModUserNameResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModUserNameResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModUserNameResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

////////////////////

int IMMsgPickle :: ToBuffer( const IMModPwdRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModPwdRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModPwdRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModPwdRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModPwdRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModPwdRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModPwdRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModPwdRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModPwdRequest_t * pDestStruct, const IMModPwdRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModPwdRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModPwdRequest_t * ptStruct1, IMModPwdRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModPwdRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModPwdRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModPwdRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModPwdRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModPwdRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModPwdRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModPwdRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMModPwdResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModPwdResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModPwdResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModPwdResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModPwdResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModPwdResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModPwdResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModPwdResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModPwdResponse_t * pDestStruct, const IMModPwdResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModPwdResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModPwdResponse_t * ptStruct1, IMModPwdResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModPwdResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModPwdResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModPwdResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModPwdResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModPwdResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModPwdResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModPwdResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

////////////////////

int IMMsgPickle :: ToBuffer( const IMModPwdByMobileRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModPwdByMobileRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModPwdByMobileRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModPwdByMobileRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModPwdByMobileRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModPwdByMobileRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModPwdByMobileRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModPwdByMobileRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModPwdByMobileRequest_t * pDestStruct, const IMModPwdByMobileRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModPwdByMobileRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModPwdByMobileRequest_t * ptStruct1, IMModPwdByMobileRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModPwdByMobileRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModPwdByMobileRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModPwdByMobileRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModPwdByMobileRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModPwdByMobileRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModPwdByMobileRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModPwdByMobileRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMModPwdByMobileResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModPwdByMobileResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModPwdByMobileResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModPwdByMobileResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMModPwdByMobileResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModPwdByMobileResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMModPwdByMobileResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMModPwdByMobileResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMModPwdByMobileResponse_t * pDestStruct, const IMModPwdByMobileResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMModPwdByMobileResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMModPwdByMobileResponse_t * ptStruct1, IMModPwdByMobileResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMModPwdByMobileResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMModPwdByMobileResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMModPwdByMobileResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMModPwdByMobileResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMModPwdByMobileResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMModPwdByMobileResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMModPwdByMobileResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

/////////////////////////////////////////////////////


int IMMsgPickle :: ToBuffer( const IMReportProfileRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMReportProfileRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMReportProfileRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMReportProfileRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMReportProfileRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMReportProfileRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMReportProfileRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMReportProfileRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMReportProfileRequest_t * pDestStruct, const IMReportProfileRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMReportProfileRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMReportProfileRequest_t * ptStruct1, IMReportProfileRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMReportProfileRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMReportProfileRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMReportProfileRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMReportProfileRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMReportProfileRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMReportProfileRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMReportProfileRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMReportProfileResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMReportProfileResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMReportProfileResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMReportProfileResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMReportProfileResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMReportProfileResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMReportProfileResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMReportProfileResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMReportProfileResponse_t * pDestStruct, const IMReportProfileResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMReportProfileResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMReportProfileResponse_t * ptStruct1, IMReportProfileResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMReportProfileResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMReportProfileResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMReportProfileResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMReportProfileResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMReportProfileResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMReportProfileResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMReportProfileResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

/////////////////////////////////////////////////////

int IMMsgPickle :: ToBuffer( const IMReportGroupRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMReportGroupRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMReportGroupRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMReportGroupRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMReportGroupRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMReportGroupRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMReportGroupRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMReportGroupRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMReportGroupRequest_t * pDestStruct, const IMReportGroupRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMReportGroupRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMReportGroupRequest_t * ptStruct1, IMReportGroupRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMReportGroupRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMReportGroupRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMReportGroupRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMReportGroupRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMReportGroupRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMReportGroupRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMReportGroupRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: ToBuffer( const IMReportGroupResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMReportGroupResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMReportGroupResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMReportGroupResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMReportGroupResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMReportGroupResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMReportGroupResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMReportGroupResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMReportGroupResponse_t * pDestStruct, const IMReportGroupResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMReportGroupResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMReportGroupResponse_t * ptStruct1, IMReportGroupResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMReportGroupResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMReportGroupResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMReportGroupResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMReportGroupResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMReportGroupResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMReportGroupResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMReportGroupResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

/////////////////////////////////////////////////////


int IMMsgPickle :: ToBuffer( const IMWebProxyInfo_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMWebProxyInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMWebProxyInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMWebProxyInfo_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMWebProxyInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMWebProxyInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMWebProxyInfo_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMWebProxyInfo, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMWebProxyInfo_t * pDestStruct, const IMWebProxyInfo_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMWebProxyInfo, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMWebProxyInfo_t * ptStruct1, IMWebProxyInfo_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMWebProxyInfo, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMWebProxyInfo_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMWebProxyInfo, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMWebProxyInfo, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMWebProxyInfo_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMWebProxyInfo, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMWebProxyInfo, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}




int IMMsgPickle :: ToBuffer( const IMSendLocationRequest_t* ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendLocationRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendLocationRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendLocationRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSendLocationRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendLocationRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle::FreeStructField( IMSendLocationRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSendLocationRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSendLocationRequest_t * pDestStruct, const IMSendLocationRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSendLocationRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSendLocationRequest_t * ptStruct1, IMSendLocationRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSendLocationRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSendLocationRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendLocationRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendLocationRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendLocationRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSendLocationRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendLocationRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}





/*

int IMMsgPickle :: ToBuffer( const IMSendLocationResponse_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendLocationResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendLocationResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendLocationResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSendLocationResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendLocationResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSendLocationResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSendLocationResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSendLocationResponse_t * pDestStruct, const IMSendLocationResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSendLocationResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSendLocationResponse_t * ptStruct1, IMSendLocationResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSendLocationResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSendLocationResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendLocationResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendLocationResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendLocationResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSendLocationResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendLocationResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
*/


int IMMsgPickle :: ToBuffer( const IMCustomMsgRequest_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMMsgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMMsgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCustomMsgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMCustomMsgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCustomMsgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMCustomMsgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMCustomMsgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMCustomMsgRequest_t * pDestStruct, const IMCustomMsgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMCustomMsgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMCustomMsgRequest_t * ptStruct1, IMCustomMsgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMCustomMsgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMCustomMsgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMCustomMsgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMCustomMsgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMCustomMsgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMCustomMsgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMCustomMsgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}





int IMMsgPickle :: ToBuffer( const IMSendCustomMsgRequest_t* ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendCustomMsgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendCustomMsgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendCustomMsgRequest_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSendCustomMsgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendCustomMsgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSendCustomMsgRequest_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSendCustomMsgRequest, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSendCustomMsgRequest_t * pDestStruct, const IMSendCustomMsgRequest_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSendCustomMsgRequest, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSendCustomMsgRequest_t * ptStruct1, IMSendCustomMsgRequest_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSendCustomMsgRequest, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSendCustomMsgRequest_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendCustomMsgRequest, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendCustomMsgRequest, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendCustomMsgRequest_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSendCustomMsgRequest, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendCustomMsgRequest, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}





/*

int IMMsgPickle :: ToBuffer( const IMSendCustomMsgResponse_t* ptStruct, Comm::SKBuffer * poBuffer )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( m_iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendCustomMsgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendCustomMsgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendCustomMsgResponse_t * ptStruct )
{
	if( ePickleTypeProtoBuf == m_iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeIMSendCustomMsgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == m_iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendCustomMsgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int IMMsgPickle :: FreeStructField( IMSendCustomMsgResponse_t & tStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );
	return Allocator.FreeField(
		eTypeIMSendCustomMsgResponse, &tStruct, sizeof( tStruct ) );
}

int IMMsgPickle :: DeepCopy( IMSendCustomMsgResponse_t * pDestStruct, const IMSendCustomMsgResponse_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.DeepCopy(
		eTypeIMSendCustomMsgResponse, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int IMMsgPickle :: Diff( IMSendCustomMsgResponse_t * ptStruct1, IMSendCustomMsgResponse_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptIMMsgMetaInfo );

	return Allocator.Diff(
		eTypeIMSendCustomMsgResponse, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int IMMsgPickle :: ToBuffer( const IMSendCustomMsgResponse_t * ptStruct, Comm::SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeIMSendCustomMsgResponse, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Struct2Buffer(
			eTypeIMSendCustomMsgResponse, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int IMMsgPickle :: FromBuffer( Comm::SKBuffer * poBuffer, IMSendCustomMsgResponse_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objIMMsgMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Buffer2Struct(
			eTypeIMSendCustomMsgResponse, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptIMMsgMetaInfo, m_iNetworkByteOrder );

		return tlvPickle.Buffer2Struct(
			eTypeIMSendCustomMsgResponse, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
*/
#endif


 

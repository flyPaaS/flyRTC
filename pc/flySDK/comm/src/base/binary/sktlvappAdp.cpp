
#include "sktlvapp.h"
#include "sktlvappAdp.h"
#include "iTLVPickle.h"
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus  
extern "C"  {
#endif 

#include "../../../dprint.h"

#ifdef __cplusplus  
} 
#endif


using namespace Comm;
#define min(a,b)  (((a)<(b))?(a):(b))


int sdptlv2struct(SDP_t *sdp_tlv, callsvr_sdp_t* sdp_struct)
{
	int i = 0;
	
	strncpy(sdp_struct->ip, sdp_tlv->cConIp.pcBuff, sizeof(sdp_struct->ip) - 1);
	sprintf(sdp_struct->version, "%d", sdp_tlv->iVer);
	sdp_struct->m = sdp_tlv->tMedialist.iCount;
	for(i = 0; i < sdp_struct->m; i++)
	{
		sprintf(sdp_struct->media[i].pr, "%d", sdp_tlv->tMedialist.ptList[i].iPr);
		sdp_struct->media[i].type[0] = sdp_tlv->tMedialist.ptList[i].cType;
		sprintf(sdp_struct->media[i].codec, "%d", sdp_tlv->tMedialist.ptList[i].iPt);
		sprintf(sdp_struct->media[i].port, "%d", sdp_tlv->tMedialist.ptList[i].iPort);
	}
	
	sdp_struct->r = sdp_tlv->tRtpplist.iCount;
	for(i = 0; i < sdp_struct->r; i++)
	{
		strncpy(sdp_struct->rtplist[i].ip, sdp_tlv->tRtpplist.ptList[i].cIp.pcBuff, MAX_IP_STRING_LEN - 1);
		sprintf(sdp_struct->rtplist[i].delay, "%d", sdp_tlv->tRtpplist.ptList[i].iDelay);
		sprintf(sdp_struct->rtplist[i].lost, "%d", sdp_tlv->tRtpplist.ptList[i].iLost);
	}

	strncpy(sdp_struct->acIce, sdp_tlv->cIce.pcBuff, sizeof(sdp_struct->acIce) - 1);
	sdp_struct->support_v = sdp_tlv->support_v;
	strncpy(sdp_struct->through, sdp_tlv->through.pcBuff, sizeof(sdp_struct->through) - 1);


}

int pack_call_bye_body(bye_req_t* bye_req, char *buf, int *plen)
{
	SKBuffer keyBuff;
	ByeReq_t byeReq = {0};
	int iret;

	if (bye_req == NULL || buf == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}


	byeReq.callid.pcBuff = bye_req->callid;
	byeReq.errcode = bye_req->errcode;
	byeReq.through.pcBuff = bye_req->through;
	
	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iret = tlvPickle.Struct2Buffer(eTypeByeReq, &byeReq, sizeof(byeReq), &keyBuff );
	if (iret != 0)
	{
		LM_ERR("Struct2Buffer failed\n");
		return iret;
	}

	if ( keyBuff.GetLen() >  MAX_VOIP_PACKET_LEN)
	{
		LM_ERR("msg too long, len=%d, max len = %d\n", keyBuff.GetLen(), MAX_VOIP_PACKET_LEN);
		return -1;
	}
	
	memcpy(buf, keyBuff.GetBuffer(), keyBuff.GetLen());

    if(	plen != NULL)
    {
		*plen = keyBuff.GetLen();
    }
	return iret;
}


int unpack_bye_req_body(char *buf, int len, bye_req_t* bye_req)
{
	SKBuffer keyBuff;
	ByeReq_t byeReq = {0};
	int iRet;

	if (buf == NULL || bye_req == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}

	keyBuff.Write(buf, len);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iRet = tlvPickle.Buffer2Struct(eTypeByeReq,  &keyBuff, &byeReq, sizeof(byeReq));
	if(iRet != 0)
	{
		LM_ERR("Buffer2Struct failed\n");
		return iRet;
	}

	strncpy(bye_req->callid, byeReq.callid.pcBuff, sizeof(bye_req->callid) - 1);
	strncpy(bye_req->through, byeReq.through.pcBuff, sizeof(bye_req->through) - 1);
	bye_req->errcode = byeReq.errcode;


	SKAllocator Allocator(&g_tMetaSipexMsg );
	iRet = Allocator.FreeField(eTypeByeReq, &byeReq, sizeof(byeReq));
	if(iRet != 0)
	{
		LM_ERR("FreeField failed\n");
	}

	return 0;

}


int pack_dtmf_info_body(dtmf_info_t* dtmf_info, char *buf, int *plen)
{
	SKBuffer keyBuff;
	DtmfInfo_t dtmfInfo = {0};
	int iret;

	if (dtmf_info == NULL || buf == NULL)
	{
		LM_ERR("para error");
		return -1;
	}


	dtmfInfo.callid.pcBuff = dtmf_info->callid;
	dtmfInfo.dtmf.pcBuff = dtmf_info->dtmf;
	
	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iret = tlvPickle.Struct2Buffer(eTypeDtmfInfo, &dtmfInfo, sizeof(dtmfInfo), &keyBuff );
	if (iret != 0)
	{
		LM_ERR("Struct2Buffer failed");
		return iret;
	}

	if ( keyBuff.GetLen() >  MAX_VOIP_PACKET_LEN)
	{
		LM_ERR("msg too long, len=%d, max len = %d", keyBuff.GetLen(), MAX_VOIP_PACKET_LEN);
		return -1;
	}
	
	memcpy(buf, keyBuff.GetBuffer(), keyBuff.GetLen());

    if(	plen != NULL)
    {
		*plen = keyBuff.GetLen();
    }
	return iret;
}


int unpack_dtmf_info_body(char *buf, int len, dtmf_info_t* dtmf_info)
{
	SKBuffer keyBuff;
	DtmfInfo_t dtmfInfo = {0};
	int iRet;

	if (buf == NULL || dtmf_info == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}

	keyBuff.Write(buf, len);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iRet = tlvPickle.Buffer2Struct(eTypeDtmfInfo,  &keyBuff, &dtmfInfo, sizeof(dtmfInfo));
	if(iRet != 0)
	{
		LM_ERR("Buffer2Struct failed");
		return iRet;
	}

	strncpy(dtmf_info->callid, dtmfInfo.callid.pcBuff, sizeof(dtmf_info->callid) - 1);
	strncpy(dtmf_info->dtmf, dtmfInfo.dtmf.pcBuff, sizeof(dtmf_info->dtmf) - 1);


	SKAllocator Allocator(&g_tMetaSipexMsg );
	iRet = Allocator.FreeField(eTypeDtmfInfo, &dtmfInfo, sizeof(dtmfInfo));
	if(iRet != 0)
	{
		LM_WARN("FreeField failed");
	}

	return 0;

}



int pack_route_update_body(route_update_t* route_update, char *buf, int *plen)
{
	SKBuffer keyBuff;
	RouteUpdate_t routeUpdate = {0};
	int iret;

	if (route_update == NULL || buf == NULL)
	{
		LM_ERR("para error");
		return -1;

	}

	routeUpdate.callid.pcBuff = route_update->callid;
	routeUpdate.through.pcBuff = route_update->through;
	
	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iret = tlvPickle.Struct2Buffer(eTypeRouteUpdate, &routeUpdate, sizeof(routeUpdate), &keyBuff );
	if (iret != 0)
	{
		LM_ERR("Struct2Buffer failed");
		return iret;
	}

	if ( keyBuff.GetLen() >  MAX_VOIP_PACKET_LEN)
	{
		LM_ERR("msg too long, len=%d, max len = %d", keyBuff.GetLen(), MAX_VOIP_PACKET_LEN);
		return -1;
	}
	
	memcpy(buf, keyBuff.GetBuffer(), keyBuff.GetLen());

	if(plen != NULL)
	{
		*plen = keyBuff.GetLen();
	}
	
	return iret;
}


int unpack_route_update_body(char *buf, int len, route_update_t* route_update)
{
	SKBuffer keyBuff;
	RouteUpdate_t routeUpdate = {0};
	int iRet;

	if (buf == NULL || route_update == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}

	keyBuff.Write(buf, len);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iRet = tlvPickle.Buffer2Struct(eTypeRouteUpdate,  &keyBuff, &routeUpdate, sizeof(routeUpdate));
	if(iRet != 0)
	{
		LM_ERR("Buffer2Struct failed");
		return iRet;
	}

	strncpy(route_update->callid, routeUpdate.callid.pcBuff, sizeof(route_update->callid) - 1);
	strncpy(route_update->through, routeUpdate.through.pcBuff, sizeof(route_update->through) - 1);


	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeRouteUpdate, &routeUpdate, sizeof(routeUpdate));
	if(iRet != 0)
	{
		LM_WARN("FreeField failed");
	}

	return 0;

}


int pack_call_notify_body(call_notify_t* call_notify, char *buf, int *plen)
{
	SKBuffer keyBuff;
	DirectNfy_t directNfy = {0};
	int iret;

	if (call_notify == NULL || buf == NULL)
	{
		LM_ERR("para error");
		return -1;

	}

	directNfy.callid.pcBuff = call_notify->callid;
	directNfy.through.pcBuff = call_notify->through;
	directNfy.errcode = call_notify->errcode;
	
	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iret = tlvPickle.Struct2Buffer(eTypeDirectNfy, &directNfy, sizeof(directNfy), &keyBuff );
	if (iret != 0)
	{
		LM_ERR("Struct2Buffer failed");
		return iret;
	}

	if ( keyBuff.GetLen() >  MAX_VOIP_PACKET_LEN)
	{
		LM_ERR("msg too long, len=%d, max len = %d", keyBuff.GetLen(), MAX_VOIP_PACKET_LEN);
		return -1;
	}
	
	memcpy(buf, keyBuff.GetBuffer(), keyBuff.GetLen());

	if(plen != NULL)
	{
		*plen = keyBuff.GetLen();
	}
	
	return iret;
}


int unpack_call_notify_body(char *buf, int len, call_notify_t* call_notify)
{
	SKBuffer keyBuff;
	DirectNfy_t directNfy = {0};
	int iRet;

	if (buf == NULL || call_notify == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}

	keyBuff.Write(buf, len);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iRet = tlvPickle.Buffer2Struct(eTypeDirectNfy, &keyBuff, &directNfy, sizeof(directNfy));
	if(iRet != 0)
	{
		LM_ERR("Buffer2Struct failed");
		return iRet;
	}

	strncpy(call_notify->callid, directNfy.callid.pcBuff, sizeof(call_notify->callid) - 1);
	strncpy(call_notify->through, directNfy.through.pcBuff, sizeof(call_notify->through) - 1);
	call_notify->errcode = directNfy.errcode;


	SKAllocator Allocator(&g_tMetaSipexMsg );
	iRet = Allocator.FreeField(eTypeDirectNfy, &directNfy, sizeof(directNfy));
	if(iRet != 0)
	{
		LM_WARN("FreeField failed");
	}

	return 0;

}



int pack_call_update_rsp_body(call_update_rsp_t* call_update_rsp, char *buf, int *plen)
{
	SKBuffer keyBuff;
	CallUpdateRsp_t callUpdateRsp = {0};
	int iret;

	if (call_update_rsp == NULL || buf == NULL)
	{
		LM_ERR("para error\n");
		return -1;

	}

	callUpdateRsp.callid.pcBuff = call_update_rsp->callid;
	callUpdateRsp.errcode = call_update_rsp->errcode;
	callUpdateRsp.ice_mode.pcBuff = call_update_rsp->ice_mode;
	callUpdateRsp.apt.pcBuff = call_update_rsp->apt;
	callUpdateRsp.vpt.pcBuff = call_update_rsp->vpt;
	callUpdateRsp.through.pcBuff = call_update_rsp->through;
	
	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iret = tlvPickle.Struct2Buffer(eTypeUpdateRsp, &callUpdateRsp, sizeof(callUpdateRsp), &keyBuff );
	if (iret != 0)
	{
		LM_ERR("Struct2Buffer failed\n");
		return iret;
	}

	if ( keyBuff.GetLen() >  MAX_VOIP_PACKET_LEN)
	{
		LM_ERR("msg too long, len=%d, max len = %d\n", keyBuff.GetLen(), MAX_VOIP_PACKET_LEN);
		return -1;
	}
	
	memcpy(buf, keyBuff.GetBuffer(), keyBuff.GetLen());

	if(plen != NULL)
	{
		*plen = keyBuff.GetLen();
	}
	
	return iret;
}


int unpack_call_update_rsp_body(char *buf, int len, call_update_rsp_t* call_update_rsp)
{
	SKBuffer keyBuff;
	CallUpdateRsp_t callUpdateRsp = {0};
	int iRet;

	if (buf == NULL || call_update_rsp == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}

	keyBuff.Write(buf, len);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iRet = tlvPickle.Buffer2Struct(eTypeUpdateRsp, &keyBuff, &callUpdateRsp, sizeof(callUpdateRsp));
	if(iRet != 0)
	{
		LM_ERR("Buffer2Struct failed\n");
		return iRet;
	}

	strncpy(call_update_rsp->callid, callUpdateRsp.callid.pcBuff, sizeof(call_update_rsp->callid) - 1);
	call_update_rsp->errcode = callUpdateRsp.errcode;
	strncpy(call_update_rsp->ice_mode, callUpdateRsp.ice_mode.pcBuff, sizeof(call_update_rsp->ice_mode) - 1);
	strncpy(call_update_rsp->through, callUpdateRsp.through.pcBuff, sizeof(call_update_rsp->through) - 1);
	strncpy(call_update_rsp->apt, callUpdateRsp.vpt.pcBuff, sizeof(call_update_rsp->apt) - 1);
	strncpy(call_update_rsp->vpt, callUpdateRsp.vpt.pcBuff, sizeof(call_update_rsp->apt) - 1);

	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeUpdateRsp, &callUpdateRsp, sizeof(callUpdateRsp));
	if(iRet != 0)
	{
		LM_ERR("FreeField failed\n");
	}

	return 0;

}

int pack_call_update_body(call_update_t* call_update, char *buf, int *plen)
{
	SKBuffer keyBuff;
	CallUpdate_t callUpdate = {0};
	SDP_t *pSDP = NULL;
	callsvr_sdp_t *pCallsvrSdp = NULL;
	Media_t aMedia[MAX_MEDIA_NUM] = {{0}};
	Rtpp_t aRtpp[MAX_RTPLIST_NUM] = {{0}};
	int i = 0;	
	int iret;

	if (call_update == NULL || buf == NULL)
	{
		LM_ERR("para error\n");
		return -1;

	}

	callUpdate.callid.pcBuff = call_update->callid;



	pSDP = &(callUpdate.callsvr_sdp);
	pCallsvrSdp = &(call_update->callsvr_sdp);
	
	pSDP->cConIp.pcBuff = pCallsvrSdp->ip;
	pSDP->iVer = atoi(pCallsvrSdp->version);

	for (i = 0; i < min(pCallsvrSdp->r, MAX_RTPLIST_NUM); i++)
	{
		aRtpp[i].cIp.pcBuff = pCallsvrSdp->rtplist[i].ip;
		aRtpp[i].iDelay = atoi(pCallsvrSdp->rtplist[i].delay);
		aRtpp[i].iLost = atoi(pCallsvrSdp->rtplist[i].lost);
	}
	pSDP->tRtpplist.iCount = pCallsvrSdp->r;
	pSDP->tRtpplist.ptList = aRtpp;

	for (i = 0; i < min(pCallsvrSdp->m, MAX_MEDIA_NUM); i++)
	{
		aMedia[i].iPr = atoi(pCallsvrSdp->media[i].pr);
		aMedia[i].iPt = atoi(pCallsvrSdp->media[i].codec);
		//aMedia[i].cType = atoi(pCallsvrSdp->media[i].type);
		aMedia[i].cType = pCallsvrSdp->media[i].type[0];
		aMedia[i].iPort =  atoi(pCallsvrSdp->media[i].port);
	}
	pSDP->tMedialist.iCount = pCallsvrSdp->m;
	pSDP->tMedialist.ptList = aMedia;

	pSDP->cIce.iLen = strlen(pCallsvrSdp->acIce)+1;
	pSDP->cIce.pcBuff = pCallsvrSdp->acIce;
	pSDP->support_v = pCallsvrSdp->support_v;
	pSDP->through.pcBuff = pCallsvrSdp->through;


	
	callUpdate.through.pcBuff = call_update->through;
	
	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iret = tlvPickle.Struct2Buffer(eTypeUpdate, &callUpdate, sizeof(callUpdate), &keyBuff );
	if (iret != 0)
	{
		LM_ERR("Struct2Buffer failed\n");
		return iret;
	}

	if ( keyBuff.GetLen() >  MAX_VOIP_PACKET_LEN)
	{
		LM_ERR("msg too long, len=%d, max len = %d\n", keyBuff.GetLen(), MAX_VOIP_PACKET_LEN);
		return -1;
	}
	
	memcpy(buf, keyBuff.GetBuffer(), keyBuff.GetLen());

	if(plen != NULL)
	{
		*plen = keyBuff.GetLen();
	}
	
	return iret;
}


int unpack_call_update_body(char *buf, int len, call_update_t* call_update)
{
	SKBuffer keyBuff;
	CallUpdate_t callUpdate = {0};
	int iRet;

	if (buf == NULL || call_update == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}

	keyBuff.Write(buf, len);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iRet = tlvPickle.Buffer2Struct(eTypeUpdate, &keyBuff, &callUpdate, sizeof(callUpdate));
	if(iRet != 0)
	{
		LM_ERR("Buffer2Struct failed\n");
		return iRet;
	}

	strncpy(call_update->callid, callUpdate.callid.pcBuff, sizeof(call_update->callid) - 1);

	sdptlv2struct(&(callUpdate.callsvr_sdp), &(call_update->callsvr_sdp));

	strncpy(call_update->through, callUpdate.through.pcBuff, sizeof(call_update->through) - 1);

	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeUpdate, &callUpdate, sizeof(callUpdate));
	if(iRet != 0)
	{
		LM_ERR("FreeField failed\n");
	}

	return 0;

}

int pack_call_ring_body(call_ring_rsp_t* call_ring, char *buf, int *plen)
{
	SKBuffer keyBuff;
	CallRing_t callRing = {0};
	SDP_t *pSDP = NULL;
	callsvr_sdp_t *pCallsvrSdp = NULL;
	Media_t aMedia[MAX_MEDIA_NUM] = {{0}};
	Rtpp_t aRtpp[MAX_RTPLIST_NUM] = {{0}};
	int i = 0;	
	int iret;

	if (call_ring == NULL || buf == NULL)
	{
		LM_ERR("para error\n");
		return -1;

	}

	callRing.callid.pcBuff = call_ring->callid;



	pSDP = &(callRing.callsvr_sdp);
	pCallsvrSdp = &(call_ring->callsvr_sdp);
	
	pSDP->cConIp.pcBuff = pCallsvrSdp->ip;
	pSDP->iVer = atoi(pCallsvrSdp->version);

	for (i = 0; i < min(pCallsvrSdp->r, MAX_RTPLIST_NUM); i++)
	{
		aRtpp[i].cIp.pcBuff = pCallsvrSdp->rtplist[i].ip;
		aRtpp[i].iDelay = atoi(pCallsvrSdp->rtplist[i].delay);
		aRtpp[i].iLost = atoi(pCallsvrSdp->rtplist[i].lost);
	}
	pSDP->tRtpplist.iCount = pCallsvrSdp->r;
	pSDP->tRtpplist.ptList = aRtpp;


	for (i = 0; i < min(pCallsvrSdp->m, MAX_MEDIA_NUM); i++)
	{
		aMedia[i].iPr = atoi(pCallsvrSdp->media[i].pr);
		aMedia[i].iPt = atoi(pCallsvrSdp->media[i].codec);
		aMedia[i].cType = pCallsvrSdp->media[i].type[0];
		aMedia[i].iPort =  atoi(pCallsvrSdp->media[i].port);
	}
	pSDP->tMedialist.iCount = pCallsvrSdp->m;
	pSDP->tMedialist.ptList = aMedia;

	pSDP->cIce.iLen = strlen(pCallsvrSdp->acIce)+1;
	pSDP->cIce.pcBuff = pCallsvrSdp->acIce;
	pSDP->support_v = pCallsvrSdp->support_v;
	pSDP->through.pcBuff = pCallsvrSdp->through;


	
	callRing.through.pcBuff = call_ring->through;
	callRing.sflag = call_ring->sflag;

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iret = tlvPickle.Struct2Buffer(eTypeCallRing, &callRing, sizeof(callRing), &keyBuff );
	if (iret != 0)
	{
		LM_ERR("Struct2Buffer failed\n");
		return iret;
	}

	if ( keyBuff.GetLen() >  MAX_VOIP_PACKET_LEN)
	{
		LM_ERR("msg too long, len=%d, max len = %d\n", keyBuff.GetLen(), MAX_VOIP_PACKET_LEN);
		return -1;
	}
	
	memcpy(buf, keyBuff.GetBuffer(), keyBuff.GetLen());

	if(plen != NULL)
	{
		*plen = keyBuff.GetLen();
	}
	
	return iret;
}


int unpack_call_ring_body(char *buf, int len, call_ring_rsp_t* call_ring)
{
	SKBuffer keyBuff;
	CallRing_t CallRing = {0};
	int iRet;

	if (buf == NULL || call_ring == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}

	keyBuff.Write(buf, len);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iRet = tlvPickle.Buffer2Struct(eTypeCallRing, &keyBuff, &CallRing, sizeof(CallRing));
	if(iRet != 0)
	{
		LM_ERR("Buffer2Struct failed\n");
		return iRet;
	}

	strncpy(call_ring->callid, CallRing.callid.pcBuff, sizeof(call_ring->callid) - 1);

	sdptlv2struct(&(CallRing.callsvr_sdp), &(call_ring->callsvr_sdp));

	strncpy(call_ring->through, CallRing.through.pcBuff, sizeof(call_ring->through) - 1);
	call_ring->sflag = CallRing.sflag;

	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeCallRing, &CallRing, sizeof(CallRing));
	if(iRet != 0)
	{
		LM_ERR("FreeField failed\n");
	}

	return 0;

}


int pack_call_ack_body(call_ack_t* call_ack, char *buf, int *plen)
{
	SKBuffer keyBuff;
	CallAck_t callAck = {0};
	SDP_t *pSDP = NULL;
	callsvr_sdp_t *pCallsvrSdp = NULL;
	Media_t aMedia[MAX_MEDIA_NUM] = {{0}};
	Rtpp_t aRtpp[MAX_RTPLIST_NUM] = {{0}};
	int i = 0;	
	int iret;

	if (call_ack == NULL || buf == NULL)
	{
		LM_ERR("para error\n");
		return -1;

	}

	callAck.callid.pcBuff = call_ack->callid;



	pSDP = &(callAck.callsvr_sdp);
	pCallsvrSdp = &(call_ack->callsvr_sdp);
	
	pSDP->cConIp.pcBuff = pCallsvrSdp->ip;
	pSDP->iVer = atoi(pCallsvrSdp->version);

	for (i = 0; i < min(pCallsvrSdp->r, MAX_RTPLIST_NUM); i++)
	{
		aRtpp[i].cIp.pcBuff = pCallsvrSdp->rtplist[i].ip;
		aRtpp[i].iDelay = atoi(pCallsvrSdp->rtplist[i].delay);
		aRtpp[i].iLost = atoi(pCallsvrSdp->rtplist[i].lost);
	}
	pSDP->tRtpplist.iCount = pCallsvrSdp->r;
	pSDP->tRtpplist.ptList = aRtpp;


	for (i = 0; i < min(pCallsvrSdp->m, MAX_MEDIA_NUM); i++)
	{
		aMedia[i].iPr = atoi(pCallsvrSdp->media[i].pr);
		aMedia[i].iPt = atoi(pCallsvrSdp->media[i].codec);
		aMedia[i].cType = pCallsvrSdp->media[i].type[0];
		aMedia[i].iPort =  atoi(pCallsvrSdp->media[i].port);
	}
	pSDP->tMedialist.iCount = pCallsvrSdp->m;
	pSDP->tMedialist.ptList = aMedia;

	pSDP->cIce.iLen = strlen(pCallsvrSdp->acIce)+1;
	pSDP->cIce.pcBuff = pCallsvrSdp->acIce;
	pSDP->support_v = pCallsvrSdp->support_v;
	pSDP->through.pcBuff = pCallsvrSdp->through;


	
	callAck.ice_mode.pcBuff = call_ack->ice_mode;
	callAck.ice_type.pcBuff = call_ack->ice_type;
	
	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iret = tlvPickle.Struct2Buffer(eTypeCallAck, &callAck, sizeof(callAck), &keyBuff );
	if (iret != 0)
	{
		LM_ERR("Struct2Buffer failed\n");
		return iret;
	}

	if ( keyBuff.GetLen() >  MAX_VOIP_PACKET_LEN)
	{
		LM_ERR("msg too long, len=%d, max len = %d\n", keyBuff.GetLen(), MAX_VOIP_PACKET_LEN);
		return -1;
	}
	
	memcpy(buf, keyBuff.GetBuffer(), keyBuff.GetLen());

	if(plen != NULL)
	{
		*plen = keyBuff.GetLen();
	}
	
	return iret;
}


int unpack_call_ack_body(char *buf, int len, call_ack_t* call_ack)
{
	SKBuffer keyBuff;
	CallAck_t CallAck = {0};
	int iRet;

	if (buf == NULL || call_ack == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}

	keyBuff.Write(buf, len);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iRet = tlvPickle.Buffer2Struct(eTypeCallAck, &keyBuff, &CallAck, sizeof(CallAck));
	if(iRet != 0)
	{
		LM_ERR("Buffer2Struct failed\n");
		return iRet;
	}

	strncpy(call_ack->callid, CallAck.callid.pcBuff, sizeof(call_ack->callid) - 1);

	sdptlv2struct(&(CallAck.callsvr_sdp), &(call_ack->callsvr_sdp));

	strncpy(call_ack->ice_mode, CallAck.ice_mode.pcBuff, sizeof(call_ack->ice_mode) - 1);
	strncpy(call_ack->ice_type, CallAck.ice_type.pcBuff, sizeof(call_ack->ice_type) - 1);
	

	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeCallAck, &CallAck, sizeof(CallAck));
	if(iRet != 0)
	{
		LM_ERR("FreeField failed\n");
	}

	return 0;

}

int pack_call_rsp_body(call_rsp_t* call_rsp, char *buf, int *plen)
{
	SKBuffer keyBuff;
	CallRsp_t callRsp = {0};
	SDP_t *pSDP = NULL;
	callsvr_sdp_t *pCallsvrSdp = NULL;
	Media_t aMedia[MAX_MEDIA_NUM] = {{0}};
	Rtpp_t aRtpp[MAX_RTPLIST_NUM] = {{0}};
	int i = 0;	
	int iret;

	if (call_rsp == NULL || buf == NULL)
	{
		LM_ERR("para error\n");
		return -1;

	}

	callRsp.callid.pcBuff = call_rsp->callid;



	pSDP = &(callRsp.callsvr_sdp);
	pCallsvrSdp = &(call_rsp->callsvr_sdp);
	
	pSDP->cConIp.pcBuff = pCallsvrSdp->ip;
	pSDP->iVer = atoi(pCallsvrSdp->version);

	for (i = 0; i < min(pCallsvrSdp->r, MAX_RTPLIST_NUM); i++)
	{
		aRtpp[i].cIp.pcBuff = pCallsvrSdp->rtplist[i].ip;
		aRtpp[i].iDelay = atoi(pCallsvrSdp->rtplist[i].delay);
		aRtpp[i].iLost = atoi(pCallsvrSdp->rtplist[i].lost);
	}
	pSDP->tRtpplist.iCount = pCallsvrSdp->r;
	pSDP->tRtpplist.ptList = aRtpp;


	for (i = 0; i < min(pCallsvrSdp->m, MAX_MEDIA_NUM); i++)
	{
		aMedia[i].iPr = atoi(pCallsvrSdp->media[i].pr);
		aMedia[i].iPt = atoi(pCallsvrSdp->media[i].codec);
		aMedia[i].cType = pCallsvrSdp->media[i].type[0];
		aMedia[i].iPort =  atoi(pCallsvrSdp->media[i].port);
	}
	pSDP->tMedialist.iCount = pCallsvrSdp->m;
	pSDP->tMedialist.ptList = aMedia;

	pSDP->cIce.iLen = strlen(pCallsvrSdp->acIce)+1;
	pSDP->cIce.pcBuff = pCallsvrSdp->acIce;
	pSDP->support_v = pCallsvrSdp->support_v;
	pSDP->through.pcBuff = pCallsvrSdp->through;


	callRsp.errcode = call_rsp->errcode;
	callRsp.ver.pcBuff = call_rsp->ver;
	callRsp.sflag = call_rsp->sflag;
	callRsp.through.pcBuff = call_rsp->through;

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iret = tlvPickle.Struct2Buffer(eTypeCallRsp, &callRsp, sizeof(callRsp), &keyBuff );
	if (iret != 0)
	{
		LM_ERR("Struct2Buffer failed\n");
		return iret;
	}

	if ( keyBuff.GetLen() >  MAX_VOIP_PACKET_LEN)
	{
		LM_ERR("msg too long, len=%d, max len = %d\n", keyBuff.GetLen(), MAX_VOIP_PACKET_LEN);
		return -1;
	}
	
	memcpy(buf, keyBuff.GetBuffer(), keyBuff.GetLen());

	if(plen != NULL)
	{
		*plen = keyBuff.GetLen();
	}
	
	return iret;
}


int unpack_call_rsp_body(char *buf, int len, call_rsp_t* call_rsp)
{
	SKBuffer keyBuff;
	CallRsp_t CallRsp = {0};
	int iRet;

	if (buf == NULL || call_rsp == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}

	keyBuff.Write(buf, len);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iRet = tlvPickle.Buffer2Struct(eTypeCallRsp, &keyBuff, &CallRsp, sizeof(CallRsp));
	if(iRet != 0)
	{
		LM_ERR("Buffer2Struct failed\n");
		return iRet;
	}

	strncpy(call_rsp->callid, CallRsp.callid.pcBuff, sizeof(call_rsp->callid) - 1);

	sdptlv2struct(&(CallRsp.callsvr_sdp), &(call_rsp->callsvr_sdp));

	strncpy(call_rsp->through, CallRsp.through.pcBuff, sizeof(call_rsp->through) - 1);
	strncpy(call_rsp->ver, CallRsp.ver.pcBuff, sizeof(call_rsp->ver) - 1);
	call_rsp->sflag = CallRsp.sflag;
	call_rsp->errcode = CallRsp.errcode;

	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeCallRsp, &CallRsp, sizeof(CallRsp));
	if(iRet != 0)
	{
		LM_ERR("FreeField failed\n");
	}

	return 0;

}


int pack_call_req_body(call_req_t* call_req, callsvr_hdr_t *hdr, char *buf, int *plen)
{
	SKBuffer keyBuff;
	CallReq_t callReq = {0};
	SDP_t *pSDP = NULL;
	callsvr_sdp_t *pCallsvrSdp = NULL;
	Media_t aMedia[MAX_MEDIA_NUM] = {{0}};
	Rtpp_t aRtpp[MAX_RTPLIST_NUM] = {{0}};
	int i = 0;	
	int iret;

	if (call_req == NULL || buf == NULL)
	{
		LM_ERR("para error\n");
		return -1;

	}

	callReq.callid.pcBuff = call_req->callid;



	pSDP = &(callReq.callsvr_sdp);
	pCallsvrSdp = &(call_req->callsvr_sdp);
	
	pSDP->cConIp.pcBuff = pCallsvrSdp->ip;
	pSDP->iVer = atoi(pCallsvrSdp->version);

	for (i = 0; i < min(pCallsvrSdp->r, MAX_RTPLIST_NUM); i++)
	{
		aRtpp[i].cIp.pcBuff = pCallsvrSdp->rtplist[i].ip;
		aRtpp[i].iDelay = atoi(pCallsvrSdp->rtplist[i].delay);
		aRtpp[i].iLost = atoi(pCallsvrSdp->rtplist[i].lost);
	}
	pSDP->tRtpplist.iCount = pCallsvrSdp->r;
	pSDP->tRtpplist.ptList = aRtpp;


	for (i = 0; i < min(pCallsvrSdp->m, MAX_MEDIA_NUM); i++)
	{
		aMedia[i].iPr = atoi(pCallsvrSdp->media[i].pr);
		aMedia[i].iPt = atoi(pCallsvrSdp->media[i].codec);
		aMedia[i].cType = pCallsvrSdp->media[i].type[0];
		aMedia[i].iPort =  atoi(pCallsvrSdp->media[i].port);
	}
	pSDP->tMedialist.iCount = pCallsvrSdp->m;
	pSDP->tMedialist.ptList = aMedia;

	pSDP->cIce.iLen = strlen(pCallsvrSdp->acIce)+1;
	pSDP->cIce.pcBuff = pCallsvrSdp->acIce;
	pSDP->support_v = pCallsvrSdp->support_v;
	pSDP->through.pcBuff = pCallsvrSdp->through;

	callReq.type = call_req->type;
	callReq.fuserid.pcBuff = call_req->fuserid;
	callReq.fuid.pcBuff = call_req->fuid;
	callReq.fphone.pcBuff = call_req->fphone;
	callReq.fnickname.pcBuff = call_req->fnickname;
	callReq.touerid.pcBuff = call_req->touerid;
	callReq.tophone.pcBuff = call_req->tophone;
	callReq.ver.pcBuff = call_req->ver;
	callReq.sflag = call_req->sflag;
	callReq.through.pcBuff = call_req->through;
	callReq.userdata.pcBuff = call_req->userdata;
	callReq.meeting = call_req->meeting;
	callReq.fpv = hdr->fpv;
	callReq.fconntype = hdr->fconntp;


	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iret = tlvPickle.Struct2Buffer(eTypeCallReq, &callReq, sizeof(callReq), &keyBuff );
	if (iret != 0)
	{
		LM_ERR("Struct2Buffer failed\n");
		return iret;
	}

	if ( keyBuff.GetLen() >  MAX_VOIP_PACKET_LEN)
	{
		LM_ERR("msg too long, len=%d, max len = %d\n", keyBuff.GetLen(), MAX_VOIP_PACKET_LEN);
		return -1;
	}
	
	memcpy(buf, keyBuff.GetBuffer(), keyBuff.GetLen());

	if(plen != NULL)
	{
		*plen = keyBuff.GetLen();
	}
	
	return iret;
}


int unpack_call_req_body(char *buf, int len, call_req_t* call_req, callsvr_hdr_t *hdr)
{
	SKBuffer keyBuff;
	CallReq_t CallReq = {0};
	int iRet;

	if (buf == NULL || call_req == NULL || hdr == NULL)
	{
		LM_ERR("para error\n");
		return -1;
	}

	keyBuff.Write(buf, len);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

	iRet = tlvPickle.Buffer2Struct(eTypeCallReq, &keyBuff, &CallReq, sizeof(CallReq));
	if(iRet != 0)
	{
		LM_ERR("Buffer2Struct failed\n");
		return iRet;
	}

	strncpy(call_req->callid, CallReq.callid.pcBuff, sizeof(call_req->callid) - 1);

	sdptlv2struct(&(CallReq.callsvr_sdp), &(call_req->callsvr_sdp));


	call_req->type = CallReq.type;
	strncpy(call_req->tophone, CallReq.tophone.pcBuff, sizeof(call_req->tophone) - 1);
	strncpy(call_req->fphone, CallReq.fphone.pcBuff, sizeof(call_req->fphone) - 1);
	strncpy(call_req->fnickname, CallReq.fnickname.pcBuff, sizeof(call_req->fnickname) - 1);
	strncpy(call_req->ver, CallReq.ver.pcBuff, sizeof(call_req->ver) - 1);
	call_req->sflag = CallReq.sflag;
	strncpy(call_req->through, CallReq.through.pcBuff, sizeof(call_req->through) - 1);
	strncpy(call_req->userdata, CallReq.userdata.pcBuff, sizeof(call_req->userdata) - 1);
	call_req->meeting = CallReq.meeting;
	strncpy(call_req->touerid, CallReq.touerid.pcBuff, sizeof(call_req->touerid) - 1);
	strncpy(call_req->fuserid, CallReq.fuserid.pcBuff, sizeof(call_req->fuserid) - 1);
	strncpy(call_req->fuid, CallReq.fuid.pcBuff, sizeof(call_req->fuid) - 1);

	hdr->fpv = CallReq.fpv;
	hdr->fconntp = CallReq.fconntype;

	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeCallReq, &CallReq, sizeof(CallReq));
	if(iRet != 0)
	{
		LM_ERR("FreeField failed\n");
	}

	return 0;

}



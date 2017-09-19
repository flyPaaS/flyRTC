#include "pcp_parser.h"

#define CHECK_POINTER_VALUE(p)  if (p == NULL) return PCP_ERR;


int str_init(str_t* pstr, int slen)
{
	if (pstr != NULL && slen != 0)
	{
		pstr->slen = slen;
		pstr->sbuf = pcp_malloc(slen);
	}

	return PCP_OK;
}

void str_release(str_t* pstr)
{
	if (pstr != NULL && pstr->sbuf)
	{
		pcp_free(pstr->sbuf);
		pstr->slen = 0;
		pstr->sbuf = NULL;
	}
}

int build_msg(IN pcp_msg_t* pmsg, OUT str_t* pstr)
{
	int offset = 0;
	int tmp = 0;
	int size = 0;
	char *pdata = NULL;
	
	if (NULL == pmsg || pstr == NULL)
		return PCP_ERR;

	pdata = pstr->sbuf;
	if (pdata == NULL)
		return PCP_ERR;

	/*build version*/
	if (pmsg->version != 0)
	{
		tmp = htonl(pmsg->version);
		size = sizeof(pcp_int32);
		memcpy(pdata, &tmp, size);
		offset += size;
	}
	else
	{
		/*build hmsg len*/
		tmp = htons(pmsg->hlen);
		size = sizeof(pcp_uint16);
		memcpy(pdata + offset, &tmp, size);
		offset += size;

		/*build bmsg len*/
		tmp = htons(pmsg->blen);
		size = sizeof(pcp_uint16);
		memcpy(pdata + offset, &tmp, size);
		offset += size;

		/*build hmsg body*/
		if (pmsg->hmsg != NULL && pmsg->hlen != 0)
		{
			memcpy(pdata + offset, pmsg->hmsg, pmsg->hlen);
			offset += pmsg->hlen;
		}

		/*build bmsg body*/
		if (pmsg->bmsg != NULL && pmsg->blen!= 0)
		{
			memcpy(pdata + offset, pmsg->bmsg, pmsg->blen);
			offset += pmsg->blen;
		}
	}

	/*save msg len*/
	pstr->slen = offset;

	return PCP_OK;
}


int parser_msg(IN str_t* msg, OUT str_t* hd, OUT str_t* bd)
{
	pcp_uint16 hd_len = 0;
	pcp_uint16 bd_len = 0;
	pcp_uint16 size = 0;
	char *p = msg->sbuf;
	int offset = 0;
	
	if (msg == NULL || msg->sbuf == NULL || msg->slen < MIN_MSG_SIZE || msg->slen > MAX_MSG_SIZE)
		return PCP_ERR;

	/*parser hmsg len*/
	size = sizeof(pcp_uint16);
	memcpy(&hd_len, p, size);
	hd->slen = ntohs(hd_len);
	offset += size;
	if (hd->slen > msg->slen)
		return PCP_ERR;

	/*parser bmsg len*/
	size = sizeof(pcp_uint16);
	memcpy(&bd_len, p+offset, size);
	bd->slen = ntohs(bd_len);
	offset += size;
	if (bd->slen > msg->slen)
		return PCP_ERR;

	/*parser hmsg body*/
	if (hd->slen != 0)
	{
		hd->sbuf = p + offset;
		offset += hd->slen;
	}

	/*parser bmsg body*/
	if (bd->slen != 0)
	{
		bd->sbuf = p + offset;
		offset += bd->slen;
	}

	/*check len*/
	if (offset != msg->slen)
		return PCP_ERR;

	return PCP_OK;
}


static json_t* build_sdp_json_body(IN pcp_sdp_info_t* psdp, IN pcp_int32 baudio_pt, IN pcp_int32 bvideo_pt);

pcp_bool check_media_available(IN pcp_media_option_t* pmedia, IN pcp_int32 baudio_pt, IN pcp_int32 bvideo_pt)
{	
	pcp_bool availbale = false;
	
	if (pmedia == NULL)
		return false;

	if (baudio_pt < 0 || pmedia->pt == baudio_pt)
		availbale = true;

	if (bvideo_pt < 0 || pmedia->pt == bvideo_pt)
		availbale = true;

	return availbale;
}

json_t* build_sdp_json_body(IN pcp_sdp_info_t* psdp, IN pcp_int32 baudio_pt, IN pcp_int32 bvideo_pt)
{
	json_t* jsdp = NULL;
	json_t* jmedia = NULL;
	json_t* jrtpp = NULL;
	json_t* jice = NULL;
	json_t* jobj = NULL;
	json_t* jtmp = NULL;
	char strtmp[128] = {0};
	int i = 0;

	/*create sdp json object*/
	jsdp = jsonapi_new_value(JSON_OBJECT, NULL);
	
	jtmp = jsonapi_new_value(JSON_STRING, psdp->c_ip);	
	jsonapi_append_value(jsdp, "c", jtmp);

	jtmp = jsonapi_new_value(JSON_STRING, psdp->v_version);	
	jsonapi_append_value(jsdp, "v", jtmp);

	/*create media array */
	if (psdp->media.media_num != 0)
	{
		jmedia = jsonapi_new_value(JSON_ARRAY, NULL);
		for (i = 0; i < psdp->media.media_num; i++)
		{	
			if (check_media_available(&psdp->media.media_arr[i], baudio_pt, bvideo_pt))
			{
				jobj = jsonapi_new_value(JSON_OBJECT, NULL);

				jtmp = jsonapi_new_value(JSON_STRING, psdp->media.media_arr[i].type);	
				jsonapi_append_value(jobj, "type", jtmp);
			
				sprintf(strtmp, "%d", psdp->media.media_arr[i].pr);
				jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
				jsonapi_append_value(jobj, "pr", jtmp);

				sprintf(strtmp, "%d", psdp->media.media_arr[i].pt);
				jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
				jsonapi_append_value(jobj, "pt", jtmp);

				sprintf(strtmp, "%d", psdp->media.media_arr[i].port);
				jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
				jsonapi_append_value(jobj, "port", jtmp);

				jsonapi_append_value(jmedia, NULL, jobj);
			}			
		}

		for (i = 0; i < psdp->media.media_num; i++)
		{	
			if (!check_media_available(&psdp->media.media_arr[i], baudio_pt, bvideo_pt))
			{
				jobj = jsonapi_new_value(JSON_OBJECT, NULL);

				jtmp = jsonapi_new_value(JSON_STRING, psdp->media.media_arr[i].type);	
				jsonapi_append_value(jobj, "type", jtmp);

				sprintf(strtmp, "%d", psdp->media.media_arr[i].pr);
				jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
				jsonapi_append_value(jobj, "pr", jtmp);

				sprintf(strtmp, "%d", psdp->media.media_arr[i].pt);
				jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
				jsonapi_append_value(jobj, "pt", jtmp);

				sprintf(strtmp, "%d", psdp->media.media_arr[i].port);
				jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
				jsonapi_append_value(jobj, "port", jtmp);

				jsonapi_append_value(jmedia, NULL, jobj);
			}
		}

		jsonapi_append_value(jsdp, "m", jmedia);
	}
	

	/*create rtpp array*/
	if (psdp->rtpp.rtpp_num != 0)
	{
		jrtpp = jsonapi_new_value(JSON_ARRAY, NULL);
		for (i = 0; i < psdp->rtpp.rtpp_num; i++)
		{
			jobj = jsonapi_new_value(JSON_OBJECT, NULL);

			sprintf(strtmp, "%d", psdp->rtpp.rtpp_arr[i].delay);
			jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
			jsonapi_append_value(jobj, "delay", jtmp);

			sprintf(strtmp, "%d", psdp->rtpp.rtpp_arr[i].lost);
			jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
			jsonapi_append_value(jobj, "lost", jtmp);

			jtmp = jsonapi_new_value(JSON_STRING, psdp->rtpp.rtpp_arr[i].ip);	
			jsonapi_append_value(jobj, "ip", jtmp);

			jsonapi_append_value(jrtpp, NULL, jobj);	
		}

		jsonapi_append_value(jsdp, "rtpplist", jrtpp);
	}

	/*create ice object*/
	if (psdp->ice.cdd_num != 0)
	{
		jice = jsonapi_new_value(JSON_OBJECT, NULL);
		
		jtmp = jsonapi_new_value(JSON_STRING, psdp->ice.ufrag);	
		jsonapi_append_value(jice, "ice-ufrag", jtmp);

		jtmp = jsonapi_new_value(JSON_STRING, psdp->ice.pwd);	
		jsonapi_append_value(jice, "ice-pwd", jtmp);

		jobj = jsonapi_new_value(JSON_ARRAY, NULL);
		for (i = 0; i < psdp->ice.cdd_num; i++)
		{
			jtmp = jsonapi_new_value(JSON_STRING, psdp->ice.cdd_arr[i].attr_desc);	
			jsonapi_append_value(jobj, NULL, jtmp);
		}
		jsonapi_append_value(jice, "candidates", jobj);

		jsonapi_append_value(jsdp, "ice", jice);
	}
    
    /*create throuth object*/
    if ( psdp->through.video_enabled )
    {
        char resolution[128]={0};
		sprintf(resolution,"d:%d,cw:%d,ch:%d,r:%d,f:%d,v:%d,b0:%d,b1:%d,b2:%d", \
		        psdp->through.resolution.uiMaxDecodeDefinition, 
		        psdp->through.resolution.camera_width,
			    psdp->through.resolution.camera_height, 
			    psdp->through.resolution.rotate,
			    psdp->through.decode_video_fps,
			    psdp->through.v0, 
			    psdp->through.start_bitrate, 
			    psdp->through.min_bitrate, 
			    psdp->through.max_bitrate);
        jtmp = jsonapi_new_value(JSON_STRING, resolution);
        jsonapi_append_value(jsdp, "through", jtmp);
    }
    /*end by luochengcong 20141009*/
    
	return jsdp;
}


/* ----------------------------------------------
   BUILDING
   --------------------------------------------- */

/*		build bson  header for message  		*/
int build_route_bson_header(IN const pcp_route_t* proute, OUT str_t *b_header)
{
	bson br;

    if ( proute == NULL )
    {
        return PCP_ERR;
    }
    
	bson_init(&br);

	bson_append_int(&br, "sn", proute->sn);
	bson_append_int(&br, "type", proute->type);
	bson_append_int(&br, "op", proute->op);
	
	/* mod by VintonLiu on 20150709 for IM3.0 */
	bson_append_int(&br, "atype", proute->atype);
	
	if ( proute->atype == 0 || proute->atype == 1 )
	{	    
	    bson_append_long(&br, "tuid", 0);
	    bson_append_long(&br, "fuid", 0);
	}
	else
	{
	    bson_append_long(&br, "tuid", proute->tuid);
	}
	/* VintonLiu mod end */
	
	bson_append_int(&br, "enc", proute->enc);

	if (proute->dsid > 0)
		bson_append_int(&br, "dsid", proute->dsid);

	bson_finish(&br);

	b_header->slen = bson_size(&br);
	memcpy(b_header->sbuf, bson_data(&br), b_header->slen);

	bson_destroy(&br);
	
	return PCP_OK;
}


/*	  	build json  body for message  	 	*/
int build_login_req_json_body(IN pcp_login_t* plogin, OUT str_t* j_body)
{
	json_t* jlogin = NULL;
	json_t* jtmp = NULL;
	char  strtmp[256] = {0};
	char* str_buf = NULL;

	/*build jlogin obejct*/
	jlogin = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, plogin->ssid);
	jsonapi_append_value(jlogin, "im_ssid", jtmp);

	jtmp = jsonapi_new_value(JSON_STRING, plogin->version);
	jsonapi_append_value(jlogin, "version", jtmp);
	
	sprintf(strtmp, "%d", plogin->netmode);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jlogin, "netmode", jtmp);

	sprintf(strtmp, "%d", plogin->randcode);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jlogin, "randcode", jtmp);

	/*convert jlogin to string */
	jsonapi_value_to_string(jlogin, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jlogin*/
	jsonapi_delete_value(&jlogin);
	
	return PCP_OK;
}

int build_call_req_json_body(IN pcp_call_req_t* pdialing, OUT str_t* j_body)
{
	json_t* jdialing = NULL;
	json_t* jsdp = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jdialing obejct*/
	jdialing = jsonapi_new_value(JSON_OBJECT, NULL);

	sprintf(strtmp, "%d", pdialing->callmode);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jdialing, "type", jtmp);

	jtmp = jsonapi_new_value(JSON_STRING, pdialing->call_id);
	jsonapi_append_value(jdialing, "callid", jtmp);

    if ( pdialing->atype == 0 || pdialing->atype == 1 )
    {
        jtmp = jsonapi_new_value(JSON_STRING, pdialing->fuserid);
        jsonapi_append_value(jdialing, "fuserid", jtmp);

        jtmp = jsonapi_new_value(JSON_STRING, pdialing->fuid);
        jsonapi_append_value(jdialing, "fuid", jtmp);
    }
    
	jtmp = jsonapi_new_value(JSON_STRING, pdialing->fphone);
	jsonapi_append_value(jdialing, "fphone", jtmp);

    if ( pdialing->atype == 0 || pdialing->atype == 1)
    {
        jtmp = jsonapi_new_value(JSON_STRING, pdialing->tuserid);
    	jsonapi_append_value(jdialing, "touserid", jtmp);
    }
    else
    {
    	jtmp = jsonapi_new_value(JSON_STRING, pdialing->tuid);
    	jsonapi_append_value(jdialing, "touid", jtmp);
	}

	jtmp = jsonapi_new_value(JSON_STRING, pdialing->tphone);
	jsonapi_append_value(jdialing, "tophone", jtmp);

	jtmp = jsonapi_new_value(JSON_STRING, pdialing->fnickname);
	jsonapi_append_value(jdialing, "fnickname", jtmp);

	jtmp = jsonapi_new_value(JSON_STRING, pdialing->user_data);
	jsonapi_append_value(jdialing, "user_data", jtmp);

	jtmp = jsonapi_new_value(JSON_STRING, pdialing->ver);
	jsonapi_append_value(jdialing, "ver", jtmp);

	sprintf(strtmp, "%d", pdialing->sflag);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jdialing, "sflag", jtmp);

    /*add by luochengcong 20140917*/
    sprintf(strtmp, "%d", pdialing->calltype);
    jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jdialing, "calltype", jtmp);
    /*add by luochengcong 20140917*/
    
	/*build sdp body*/
	jsdp = build_sdp_json_body(pdialing->sdp, -1, -1);
	jsonapi_append_value(jdialing, "sdp", jsdp);

	/*convert jdialing to string */
	jsonapi_value_to_string(jdialing, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jdialing*/
	jsonapi_delete_value(&jdialing);
	
	return PCP_OK;
}

int build_call_rsp_json_body(IN pcp_call_rsp_t* pdialing, OUT str_t* j_body)
{
	json_t* jdialing = NULL;
	json_t* jsdp = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jdialing obejct*/
	jdialing = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, pdialing->call_id);
	jsonapi_append_value(jdialing, "callid", jtmp);
	
	sprintf(strtmp, "%d", pdialing->reason);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jdialing, "errcode", jtmp);

	if (pdialing->reason == 0)
	{
		jtmp = jsonapi_new_value(JSON_STRING, pdialing->ver);
		jsonapi_append_value(jdialing, "ver", jtmp);

		sprintf(strtmp, "%d", pdialing->sflag);
		jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
		jsonapi_append_value(jdialing, "sflag", jtmp);

		/*build sdp body*/
		jsdp = build_sdp_json_body(pdialing->sdp, pdialing->baudio_pt, pdialing->bvideo_pt);
		jsonapi_append_value(jdialing, "sdp", jsdp);
	}

	/*convert jdialing to string */
	jsonapi_value_to_string(jdialing, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jdialing*/
	jsonapi_delete_value(&jdialing);
	
	return PCP_OK;
}

int build_push_rsp_json_body(IN pcp_push_rsp_t* ppush, OUT str_t* j_body)
{
	json_t* jpush = NULL;
	json_t* jsdp = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jpush obejct*/
	jpush = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, ppush->call_id);
	jsonapi_append_value(jpush, "callid", jtmp);
	
	sprintf(strtmp, "%d", ppush->reason);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jpush, "errcode", jtmp);

	/*build sdp body*/
	if (ppush->reason == 0 && ppush->sdp != NULL)
	{
		jsdp = build_sdp_json_body(ppush->sdp, -1, -1);
		jsonapi_append_value(jpush, "sdp", jsdp);
	}

	/*convert jpush to string */
	jsonapi_value_to_string(jpush, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jpush*/
	jsonapi_delete_value(&jpush);

	return PCP_OK;
}

int build_call_ack_json_body(IN pcp_call_ack_t* pack, OUT str_t* j_body)
{
	json_t* jack = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jack obejct*/
	jack = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, pack->call_id);
	jsonapi_append_value(jack, "callid", jtmp);
	
	sprintf(strtmp, "%d", pack->ice_type);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jack, "ice-type", jtmp);

	sprintf(strtmp, "%d", pack->ice_mode);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jack, "ice-mode", jtmp);

	/*convert jack to string */
	jsonapi_value_to_string(jack, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jack*/
	jsonapi_delete_value(&jack);
	
	return PCP_OK;
}

int build_bye_req_json_body(IN pcp_bye_req_t* pbye, OUT str_t* j_body)
{
	json_t* jbye = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jbye obejct*/
	jbye = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, pbye->call_id);
	jsonapi_append_value(jbye, "callid", jtmp);
	
	sprintf(strtmp, "%d", pbye->reason);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jbye, "errcode", jtmp);

	/*convert jbye to string */
	jsonapi_value_to_string(jbye, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jbye*/
	jsonapi_delete_value(&jbye);

	return PCP_OK;
}

int build_bye_rsp_json_body(IN pcp_bye_rsp_t* pbye, OUT str_t* j_body)
{
	json_t* jbye = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jbye obejct*/
	jbye = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, pbye->call_id);
	jsonapi_append_value(jbye, "callid", jtmp);
	
	sprintf(strtmp, "%d", pbye->reason);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jbye, "errcode", jtmp);

	/*convert json to string */
	jsonapi_value_to_string(jbye, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jbye*/
	jsonapi_delete_value(&jbye);

	return PCP_OK;
}

int build_update_req_json_body(IN pcp_update_req_t* pupdate, OUT str_t* j_body)
{
	json_t* jupdate = NULL;
	json_t* jsdp = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jupdate obejct*/
	jupdate = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, pupdate->call_id);
	jsonapi_append_value(jupdate, "callid", jtmp);

	jtmp = jsonapi_new_value(JSON_STRING, pupdate->mip);
	jsonapi_append_value(jupdate, "mip", jtmp);

	sprintf(strtmp, "%d", pupdate->aport);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jupdate, "aport", jtmp);

	sprintf(strtmp, "%d", pupdate->vport);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jupdate, "vport", jtmp);

	/*convert jupdate to string */
	jsonapi_value_to_string(jupdate, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jupdate*/
	jsonapi_delete_value(&jupdate);

	return PCP_OK;
}

int build_update_rsp_json_body(IN pcp_update_rsp_t* pupdate, OUT str_t* j_body)
{
	json_t* jupdate = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jupdate obejct*/
	jupdate = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, pupdate->call_id);
	jsonapi_append_value(jupdate, "callid", jtmp);

	sprintf(strtmp, "%d", pupdate->reason);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jupdate, "errcode", jtmp);

	sprintf(strtmp, "%d", pupdate->ice_mode);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jupdate, "ice-mode", jtmp);

	jtmp = jsonapi_new_value(JSON_STRING, pupdate->apt);
	jsonapi_append_value(jupdate, "apt", jtmp);

	jtmp = jsonapi_new_value(JSON_STRING, pupdate->vpt);
	jsonapi_append_value(jupdate, "vpt", jtmp);
	
	/*convert jupdate to string */
	jsonapi_value_to_string(jupdate, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jupdate*/
	jsonapi_delete_value(&jupdate);

	return PCP_OK;
}

int build_ringing_rsp_json_body(IN pcp_ring_rsp_t* pring, OUT str_t* j_body)
{
	json_t* jring = NULL;
	json_t* jsdp = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jring obejct*/
	jring = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, pring->call_id);
	jsonapi_append_value(jring, "callid", jtmp);

	/*build sdp body*/
	jsdp = build_sdp_json_body(pring->sdp, pring->baudio_pt, pring->bvideo_pt);
	jsonapi_append_value(jring, "sdp", jsdp);

	/*convert jring to string */
	jsonapi_value_to_string(jring, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jring*/
	jsonapi_delete_value(&jring);
	return PCP_OK;
}


int build_route_update_json_body(IN pcp_route_update_t* proute, OUT str_t* j_body)
{
	json_t* jroute = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jroute obejct*/
	jroute = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, proute->call_id);
	jsonapi_append_value(jroute, "callid", jtmp);

	/*convert jroute to string */
	jsonapi_value_to_string(jroute, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jroute*/
	jsonapi_delete_value(&jroute);

	return PCP_OK;
}

json_t* build_parties_json_body(IN pcp_conf_plist_t meet)
{
	json_t* jmeetarray = NULL;
	json_t* jobj = NULL;
	json_t* jtmp = NULL;
	char strtmp[128] = {0};
	int i = 0;

	if (meet.num != 0)
	{
		jmeetarray = jsonapi_new_value(JSON_ARRAY, NULL);
		for (i = 0; i < meet.num; i++)
		{	
			jobj = jsonapi_new_value(JSON_OBJECT, NULL);

			sprintf(strtmp, "%d", meet.user_attr[i].call_mode);
			jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
			jsonapi_append_value(jobj, "type", jtmp);		

			jtmp = jsonapi_new_value(JSON_STRING, meet.user_attr[i].tuid);	
			jsonapi_append_value(jobj, "uid", jtmp);

			jtmp = jsonapi_new_value(JSON_STRING, meet.user_attr[i].tphone);	
			jsonapi_append_value(jobj, "phone", jtmp);

			jsonapi_append_value(jmeetarray, NULL, jobj);
		}
	}

	return jmeetarray;
}

int build_m_call_req_json_body(IN pcp_conf_call_req_t* pdialing, OUT str_t* j_body)
{
	json_t* jdialing = NULL;
	json_t* jsdp = NULL;
	json_t* jmeet = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jdialing obejct*/
	jdialing = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, pdialing->call_id);
	jsonapi_append_value(jdialing, "callid", jtmp);

    if ( pdialing->atype == 0 || pdialing->atype == 1 )
    {
        jtmp = jsonapi_new_value(JSON_STRING, pdialing->fuid);
    	jsonapi_append_value(jdialing, "fuid", jtmp);

    	jtmp = jsonapi_new_value(JSON_STRING, pdialing->fuserid);
    	jsonapi_append_value(jdialing, "fuserid", jtmp);
    }
    
	jtmp = jsonapi_new_value(JSON_STRING, pdialing->fphone);
	jsonapi_append_value(jdialing, "fphone", jtmp);

	jtmp = jsonapi_new_value(JSON_STRING, pdialing->ver);
	jsonapi_append_value(jdialing, "ver", jtmp);

	sprintf(strtmp, "%d", pdialing->sflag);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jdialing, "sflag", jtmp);

	sprintf(strtmp, "%d", pdialing->conftype);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jdialing, "conftype", jtmp);

	/*build meet cand body*/
	jmeet = build_parties_json_body(pdialing->plist);
	jsonapi_append_value(jdialing, "parties", jmeet);

    jtmp = jsonapi_new_value(JSON_STRING, pdialing->roomname);
    jsonapi_append_value(jdialing, "roomname", jtmp);
    
    jtmp = jsonapi_new_value(JSON_STRING, pdialing->roompwd);
    jsonapi_append_value(jdialing, "roompwd", jtmp);
    
	/*build sdp body*/
	if ( !pdialing->conftype )
	{
    	jsdp = build_sdp_json_body(pdialing->sdp, -1, -1);
    	jsonapi_append_value(jdialing, "sdp", jsdp);
    }
    
	/*convert jdialing to string */
	jsonapi_value_to_string(jdialing, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jdialing*/
	jsonapi_delete_value(&jdialing);

	return PCP_OK;
}

int build_convert_conference_rsp_json_body(IN pcp_conf_convert_rsp_t* convert_rsp, OUT str_t* j_body)
{
	json_t* jconv = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;

	/*build jpush obejct*/
	jconv = jsonapi_new_value(JSON_OBJECT, NULL);

	jtmp = jsonapi_new_value(JSON_STRING, convert_rsp->call_id);
	jsonapi_append_value(jconv, "callid", jtmp);

	sprintf(strtmp, "%d", convert_rsp->reason);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jconv, "errcode", jtmp);

	/*convert jpush to string */
	jsonapi_value_to_string(jconv, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jpush*/
	jsonapi_delete_value(&jconv);

	return PCP_OK;
}

int build_m_inv_req_json_body(IN pcp_conf_inv_req_t* pdialing, OUT str_t* j_body)
{
    json_t* jdialing = NULL;
	json_t* jmeet = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;
	
    /*build jdialing obejct*/
    jdialing = jsonapi_new_value(JSON_OBJECT, NULL);

    jtmp = jsonapi_new_value(JSON_STRING, pdialing->callid);
    jsonapi_append_value(jdialing, "callid", jtmp);

    jtmp = jsonapi_new_value(JSON_STRING, pdialing->roomid);
    jsonapi_append_value(jdialing, "roomid", jtmp);

    jtmp = jsonapi_new_value(JSON_STRING, pdialing->fphone);
    jsonapi_append_value(jdialing, "fphone", jtmp);

    /*build meet cand body*/
	jmeet = build_parties_json_body(pdialing->plist);
	jsonapi_append_value(jdialing, "parties", jmeet);

	/*convert jdialing to string */
	jsonapi_value_to_string(jdialing, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jdialing*/
	jsonapi_delete_value(&jdialing);

	return PCP_OK;
}

int build_m_del_req_json_body(IN pcp_conf_del_req_t*pdel, OUT str_t* j_body)
{
    json_t* jdialing = NULL;
	json_t* jtmp = NULL;
	char  strtmp[64] = {0};
	char* str_buf = NULL;
	
    /*build jdialing obejct*/
    jdialing = jsonapi_new_value(JSON_OBJECT, NULL);

    jtmp = jsonapi_new_value(JSON_STRING, pdel->callid);
    jsonapi_append_value(jdialing, "callid", jtmp);

    jtmp = jsonapi_new_value(JSON_STRING, pdel->roomid);
    jsonapi_append_value(jdialing, "roomid", jtmp);

	/*convert jdialing to string */
	jsonapi_value_to_string(jdialing, &str_buf);
	j_body->slen = strlen(str_buf);
	memcpy(j_body->sbuf, str_buf, j_body->slen);
	free(str_buf);

	/*delete jdialing*/
	jsonapi_delete_value(&jdialing);

	return PCP_OK;
}

/* ----------------------------------------------
   PARSERING
   --------------------------------------------- */
static int parser_sdp_json_body(IN json_t* jroot, OUT pcp_sdp_info_t* psdp);
static int parser_media_json_body(IN json_t* jroot, OUT pcp_media_info_t* pmedia);
static int parser_rtpp_json_body(IN json_t* jroot, OUT pcp_rtpp_info_t* prtpp);
static int parser_ice_json_body(IN json_t* jroot, OUT pcp_ice_info_t* pice);
static int parser_through_json_body(IN char* through, OUT pcp_through_info_t* through_info);

int parser_media_json_body(IN json_t* jroot, OUT pcp_media_info_t* pmedia)
{
	json_t* jmedia = NULL;
	json_t *ita, *itb;
	int i = 0;

	jsonapi_parser_value(jroot, "m", &jmedia);
	
	if (jmedia != NULL)
	{
		/* iterate through children */	
		if (jmedia->child != NULL)	
		{		
			ita = jmedia->child;

			/*parser media info*/
			while (ita != NULL)		
			{			
				jsonapi_parser_number(ita, "pr", &pmedia->media_arr[i].pr);
				jsonapi_parser_number(ita, "pt", &pmedia->media_arr[i].pt);
				jsonapi_parser_number(ita, "port", &pmedia->media_arr[i].port);
				jsonapi_parser_string(ita, "type", pmedia->media_arr[i].type);
		
				itb = ita->next;			
				ita = itb;	
				i++;	
			}	
			
			pmedia->media_num = i;
		}
	}

	return PCP_OK;
}


int parser_rtpp_json_body(IN json_t* jroot, OUT pcp_rtpp_info_t* prtpp)
{
	json_t* jrtpp = NULL;
	json_t *ita, *itb;
	int i = 0;

	jsonapi_parser_value(jroot, "rtpplist", &jrtpp);
	
	if (jrtpp != NULL)
	{
		/* iterate through children */	
		if (jrtpp->child != NULL)	
		{		
			ita = jrtpp->child;

			/*parser rtpp list info*/
			while (ita != NULL)		
			{			
				jsonapi_parser_number(ita, "delay", &prtpp->rtpp_arr[i].delay);
				jsonapi_parser_number(ita, "lost", &prtpp->rtpp_arr[i].lost);
				jsonapi_parser_string(ita, "ip", prtpp->rtpp_arr[i].ip);
		
				itb = ita->next;			
				ita = itb;	
				i++;	
			}	
			
			prtpp->rtpp_num= i;
		}
	}

	return PCP_OK;
}


int parser_ice_json_body(IN json_t* jroot, OUT pcp_ice_info_t* pice)
{
	json_t* jice = NULL;
	json_t* jcandidate = NULL;
	json_t *ita, *itb;
	int i = 0;

	jsonapi_parser_value(jroot, "ice", &jice);

	if (jice != NULL)
	{
		jsonapi_parser_string(jice, "ice-ufrag", pice->ufrag);
		jsonapi_parser_string(jice, "ice-pwd", pice->pwd);
		jsonapi_parser_value(jice, "candidates", &jcandidate);

		if (jcandidate != NULL)
		{
			/* iterate through children */	
			if (jcandidate->child != NULL)	
			{		
				ita = jcandidate->child;
	
				/*parser candidate option info*/
				while (ita != NULL) 	
				{			
					jsonapi_parser_string(ita, NULL, pice->cdd_arr[i].attr_desc);
			
					itb = ita->next;			
					ita = itb;	
					i++;	
				}	
				
				pice->cdd_num = i;
			}
		}
	}

	return PCP_OK;
}

int parser_through_json_body(IN char* through, OUT pcp_through_info_t* through_info)
{
    char *p = NULL;
    char *q = NULL;

    if ( NULL == through || NULL == through_info )
    {
        ms_error("parser_through_json_body: NULL param.");
        return PCP_ERR;
    }

    if ( strlen(through) <= 0 )
    {
        ms_error("parser_through_json_body: no any through.");
        return PCP_ERR;
    }
    
    p = strtok(through,",");
    while(p!=NULL)
    {
        q=strstr(p,":");
        if (strncmp(p,"d",strlen("w"))==0) {
            through_info->resolution.uiMaxDecodeDefinition=atoi(q+1);
        }       
		else if(strncmp(p, "cw", strlen("cw"))==0){
			through_info->resolution.camera_width=atoi(q+1);
        }
		else if(strncmp(p, "ch", strlen("ch"))==0){
			through_info->resolution.camera_height=atoi(q+1);
        }
		else if(strncmp(p, "r", strlen("r"))==0){
			through_info->resolution.rotate=atoi(q+1);
        }
		else if(strncmp(p, "f", strlen("f"))==0){
			through_info->decode_video_fps=atoi(q+1);
        }
		else if(strncmp(p, "v", strlen("v"))==0){
			through_info->v0=atoi(q+1);
        }
		else if(strncmp(p, "b0", strlen("b0"))==0){
			through_info->start_bitrate=atoi(q+1);
        }
		else if(strncmp(p, "b1", strlen("b1"))==0){
			through_info->min_bitrate=atoi(q+1);
        }
		else if(strncmp(p, "b2", strlen("b2"))==0){
			through_info->max_bitrate=atoi(q+1);
        }
        else{
        //    return PCP_ERR;
        }
        
        p=strtok(NULL,",");
    }
    return PCP_OK;
}

int parser_sdp_json_body(IN json_t* jroot, OUT pcp_sdp_info_t* psdp)
{
	json_t* jsdp = NULL;
    char resolution[256] = {0};
    
	jsonapi_parser_value(jroot, "sdp", &jsdp);

	if (jsdp != NULL)
	{
		jsonapi_parser_string(jsdp, "c", psdp->c_ip);
		jsonapi_parser_string(jsdp, "v", psdp->v_version);
        jsonapi_parser_string(jsdp, "through", resolution);
        parser_through_json_body(resolution, &psdp->through);
		if (psdp->c_ip[0] == '\0')
			return PCP_ERR;

		/*parser media json body*/
		parser_media_json_body(jsdp, &psdp->media);

		/*parser rtpp json body*/
		parser_rtpp_json_body(jsdp, &psdp->rtpp);

		/*parser ice json body*/
		parser_ice_json_body(jsdp, &psdp->ice);        
	}

	return PCP_OK;
}



/*		parser bson  header for message  		*/
int parser_route_bson_header(IN str_t *b_header, OUT pcp_route_t* proute)
{
	bson br;
	bson_iterator it;

	if (b_header->sbuf == NULL || b_header->slen == 0)
		return PCP_ERR;
	
	if (bson_init_finished_data_with_copy2(&br, b_header->sbuf, b_header->slen) != BSON_OK)
		return PCP_ERR;
	
	bson_find(&it, &br, "sn");
	proute->sn = bson_iterator_int(&it);

	bson_find(&it, &br, "type");
	proute->type = bson_iterator_int(&it);
	
	bson_find(&it, &br, "op");
	proute->op = bson_iterator_int(&it);

	bson_find(&it, &br, "fpv");
	proute->fpv = bson_iterator_int(&it);

	bson_find(&it, &br, "tpv");
	proute->tpv = bson_iterator_int(&it);
	
	bson_find(&it, &br, "tuid");
	proute->tuid = bson_iterator_long(&it);

	bson_find(&it, &br, "fuid");
	proute->fuid = bson_iterator_long(&it);

	bson_find(&it, &br, "enc");
	proute->enc = bson_iterator_int(&it);

	bson_find(&it, &br, "ack");
	proute->ack = bson_iterator_int(&it);

	bson_find(&it, &br, "dsid");
	proute->dsid = bson_iterator_int(&it);

	bson_destroy(&br);
	
	return PCP_OK;
}


/*	  	parser json  body for message  	 	*/
int parser_version_rsp_json_body(IN str_t* j_body, OUT pcp_version_rsp_t* pversion)
{
	json_t* jversion = NULL;

	/*convert string to jversion*/
	jsonapi_string_to_value(&jversion, j_body->sbuf);

	CHECK_POINTER_VALUE(jversion);

	/*parser option from jversion*/
	jsonapi_parser_number(jversion, "randcode", &pversion->randcode);

	/*release jversion*/
	jsonapi_delete_value(&jversion);

	return PCP_OK;
}

int parser_login_rsp_json_body(IN str_t* j_body, OUT pcp_login_rsp_t* plogin)
{
	json_t* jlogin = NULL;

	/*convert string to jlogin*/
	jsonapi_string_to_value(&jlogin, j_body->sbuf);

	CHECK_POINTER_VALUE(jlogin);

	/*parser option from jlogin*/
	jsonapi_parser_number(jlogin, "result", &plogin->result);
	jsonapi_parser_string(jlogin, "description", plogin->description);

	/*release jlogin*/
	jsonapi_delete_value(&jlogin);

	return PCP_OK;
}


int parser_call_req_json_body(IN str_t* j_body, OUT pcp_call_req_t* pdialing)
{
	json_t* jdialing = NULL;

	/*convert string to jdialing*/
	jsonapi_string_to_value(&jdialing, j_body->sbuf);

	CHECK_POINTER_VALUE(jdialing);

	/*parser option from jdialing*/
	jsonapi_parser_number(jdialing, "type", &pdialing->callmode);
	jsonapi_parser_number(jdialing, "sflag", &pdialing->sflag);//add by wuzhaoyang20140607
	jsonapi_parser_number(jdialing, "meeting", &pdialing->meetingflag);//add by wuzhaoyang20140607
	jsonapi_parser_string(jdialing, "callid", pdialing->call_id);
	/* add by VintonLiu on 20150713 for im3.0 */
	jsonapi_parser_string(jdialing, "fuid", pdialing->fuid);
	jsonapi_parser_string(jdialing, "fuserid", pdialing->fuserid);
	jsonapi_parser_string(jdialing, "touserid", pdialing->tuserid);
	/* VintonLiu add end */
	jsonapi_parser_string(jdialing, "fphone", pdialing->fphone);
	jsonapi_parser_string(jdialing, "touid", pdialing->tuid);
	jsonapi_parser_string(jdialing, "tophone", pdialing->tphone);
	jsonapi_parser_string(jdialing, "ver", pdialing->ver);
	jsonapi_parser_string(jdialing, "fnickname", pdialing->fnickname);
	jsonapi_parser_string(jdialing, "user_data", pdialing->user_data);//add by wuzhaoyang20150202
	jsonapi_parser_number(jdialing, "calltype", &pdialing->calltype);//added by luochengcong 20140916
	/*parser sdp json body */
	parser_sdp_json_body(jdialing, pdialing->sdp);

	/*release jdialing*/
	jsonapi_delete_value(&jdialing);

	return PCP_OK;
}

int parser_call_rsp_json_body(IN str_t* j_body, OUT pcp_call_rsp_t* pdialing)
{
	json_t* jdialing = NULL;

	/*convert string to jdialing*/
	jsonapi_string_to_value(&jdialing, j_body->sbuf);

	CHECK_POINTER_VALUE(jdialing);

	/*parser option from jdialing*/
	jsonapi_parser_string(jdialing, "callid", pdialing->call_id);
	jsonapi_parser_number(jdialing, "errcode", &pdialing->reason);
	jsonapi_parser_number(jdialing, "sflag", &pdialing->sflag);
    jsonapi_parser_string(jdialing, "roomid", pdialing->roomid);

	/*parser sdp json body */
	if (pdialing->reason == 0)
	{
	    jsonapi_parser_string(jdialing, "ver", pdialing->ver);
		parser_sdp_json_body(jdialing, pdialing->sdp);
	}

	/*release jdialing*/
	jsonapi_delete_value(&jdialing);

	return PCP_OK;
}

int parser_push_rsp_json_body(IN str_t* j_body, OUT pcp_push_rsp_t* ppush)
{
	json_t* jpush = NULL;

	/*convert string to jpush*/
	jsonapi_string_to_value(&jpush, j_body->sbuf);

	CHECK_POINTER_VALUE(jpush);

	/*parser option from jpush*/
	jsonapi_parser_string(jpush, "callid", ppush->call_id);
	jsonapi_parser_number(jpush, "errcode", &ppush->reason);

	/*parser sdp json body */
	if (ppush->reason == 0)
		parser_sdp_json_body(jpush, ppush->sdp);

	/*release jpush*/
	jsonapi_delete_value(&jpush);

	return PCP_OK;
}

int parser_call_ack_json_body(IN str_t* j_body, OUT pcp_call_ack_t* pack)
{
	json_t* jack = NULL;

	/*convert string to jack*/
	jsonapi_string_to_value(&jack, j_body->sbuf);

	CHECK_POINTER_VALUE(jack);

	/*parser option from jack*/
	jsonapi_parser_string(jack, "callid", pack->call_id);
	jsonapi_parser_number(jack, "ice-type", &pack->ice_type);
	jsonapi_parser_number(jack, "ice-mode", &pack->ice_mode);

	/*release jack*/
	jsonapi_delete_value(&jack);

	return PCP_OK;
}

int parser_bye_req_json_body(IN str_t* j_body, OUT pcp_bye_req_t* pbye)
{
	json_t* jbye = NULL;

	/*convert string to jbye*/
	jsonapi_string_to_value(&jbye, j_body->sbuf);

	CHECK_POINTER_VALUE(jbye);

	/*parser option from jbye*/
	jsonapi_parser_string(jbye, "callid", pbye->call_id);
	jsonapi_parser_number(jbye, "errcode", &pbye->reason);

	/*release jbye*/
	jsonapi_delete_value(&jbye);

	return PCP_OK;
}

int parser_bye_rsp_json_body(IN str_t* j_body, OUT pcp_bye_rsp_t* pbye)
{
	json_t* jbye = NULL;

	/*convert string to jbye*/
	jsonapi_string_to_value(&jbye, j_body->sbuf);

	CHECK_POINTER_VALUE(jbye);

	/*parser option from jbye*/
	jsonapi_parser_string(jbye, "callid", pbye->call_id);
	jsonapi_parser_number(jbye, "errcode", &pbye->reason);

	/*release jbye*/
	jsonapi_delete_value(&jbye);

	return PCP_OK;
}

int parser_update_req_json_body(IN str_t* j_body, OUT pcp_update_req_t* pupdate)
{
	json_t* jupdate = NULL;

	/*convert string to jupdate*/
	jsonapi_string_to_value(&jupdate, j_body->sbuf);

	CHECK_POINTER_VALUE(jupdate);

	/*parser option from jupdate*/
	jsonapi_parser_string(jupdate, "callid", pupdate->call_id);
	jsonapi_parser_string(jupdate, "mip", pupdate->mip);
	jsonapi_parser_number(jupdate, "aport", &pupdate->aport);
	jsonapi_parser_number(jupdate, "vport", &pupdate->vport);
	jsonapi_parser_string(jupdate, "apt",pupdate->apt);
	jsonapi_parser_string(jupdate, "vpt",pupdate->vpt);
	/*release jupdate*/
	jsonapi_delete_value(&jupdate);
	
	return PCP_OK;
}

int parser_update_rsp_json_body(IN str_t* j_body, OUT pcp_update_rsp_t* pupdate)
{
	json_t* jupdate = NULL;

	/*convert string to jupdate*/
	jsonapi_string_to_value(&jupdate, j_body->sbuf);

	CHECK_POINTER_VALUE(jupdate);

	/*parser option from jupdate*/
	jsonapi_parser_string(jupdate, "callid", pupdate->call_id);
	jsonapi_parser_number(jupdate, "errcode", &pupdate->reason);
	jsonapi_parser_number(jupdate, "ice-mode", &pupdate->ice_mode);

	/*release jupdate*/
	jsonapi_delete_value(&jupdate);
	
	return PCP_OK;
}

int parser_ringing_rsp_json_body(IN str_t* j_body, OUT pcp_ring_rsp_t* pring)
{
	json_t* jring = NULL;

	/*convert string to jring*/
	jsonapi_string_to_value(&jring, j_body->sbuf);

	CHECK_POINTER_VALUE(jring);

	/*parser option from jring*/
	jsonapi_parser_string(jring, "callid", pring->call_id);

	/*parser sdp json body */
	parser_sdp_json_body(jring, pring->sdp);

	/*release jring*/
	jsonapi_delete_value(&jring);

	return PCP_OK;
}
	
int parser_direct_notify_json_body(IN str_t* j_body, OUT pcp_direct_notify_t* pnotify)
{
	json_t* jnotify = NULL;

	/*convert string to jnotify*/
	jsonapi_string_to_value(&jnotify, j_body->sbuf);

	CHECK_POINTER_VALUE(jnotify);

	/*parser option from jnotify*/
	jsonapi_parser_string(jnotify, "callid", pnotify->call_id);
	jsonapi_parser_number(jnotify, "errcode", &pnotify->reason);

	/*release jnotify*/
	jsonapi_delete_value(&jnotify);

	return PCP_OK;
}

int parser_dtmf_info_json_body(IN str_t* j_body, OUT pcp_dtmf_info_t*pdtmf)
{
    json_t* jdtmf = NULL;
    jsonapi_string_to_value(&jdtmf, j_body->sbuf);
    CHECK_POINTER_VALUE(jdtmf);

    /* parser option from jdtmf */
    jsonapi_parser_string(jdtmf, "callid", pdtmf->call_id);
    jsonapi_parser_string(jdtmf, "dtmf", pdtmf->dtmf_str);

    if ( strlen(pdtmf->dtmf_str) > 0 )
    {
        switch(pdtmf->dtmf_str[0])
        {
            case '0':
        		pdtmf->dtmf_int = PCP_DTMF_0;
        		break;
            case '1':
        		pdtmf->dtmf_int = PCP_DTMF_1;
        		break;
        	case '2':
        		pdtmf->dtmf_int = PCP_DTMF_2;
        		break;
        	case '3':
        		pdtmf->dtmf_int = PCP_DTMF_3;
        		break;
        	case '4':
        		pdtmf->dtmf_int = PCP_DTMF_4;
        		break;
        	case '5':
        		pdtmf->dtmf_int = PCP_DTMF_5;
        		break;
        	case '6':
        		pdtmf->dtmf_int = PCP_DTMF_6;
        		break;
        	case '7':
        		pdtmf->dtmf_int = PCP_DTMF_7;
        		break;
        	case '8':
        		pdtmf->dtmf_int = PCP_DTMF_8;
        		break;
        	case '9':
        		pdtmf->dtmf_int = PCP_DTMF_9;
        		break;
        	case '*':
        		pdtmf->dtmf_int = PCP_DTMF_STAR;
        		break;        	
        	case '#':
        		pdtmf->dtmf_int = PCP_DTMF_POUND;
        		break;
        	case 'A':
        	case 'a':
        		pdtmf->dtmf_int = PCP_DTMF_A;
        		break;	
        	case 'B':
        	case 'b':
        		pdtmf->dtmf_int = PCP_DTMF_B;
        		break;
        	case 'C':
        	case 'c':
        		pdtmf->dtmf_int = PCP_DTMF_C;
        		break;
        	case 'D':
        	case 'd':
        		pdtmf->dtmf_int = PCP_DTMF_D;
        		break;
        	case '!':
        		pdtmf->dtmf_int = PCP_FLASH;
        		break;
        	default:
        		return -1;
        }
    }
    
    /* release jdtmf */
    jsonapi_delete_value(&jdtmf);

    return PCP_OK;
}

//add by wuzhaoyang20140605 start
int parser_parties_json_body(IN json_t* jroot, OUT pcp_conf_info_t *pconf)
{
	json_t* jconf = NULL;
	json_t *ita, *itb;
	int i = 0;

	jsonapi_parser_value(jroot, "parties", &jconf);
	if (jconf != NULL)
	{
		/* iterate through children */	
		if (jconf->child != NULL)	
		{		
			ita = jconf->child;
			/*parser parties info*/
			while (ita != NULL)		
			{			
				jsonapi_parser_number(ita, "state", &pconf->pstate[i].call_state);
				jsonapi_parser_string(ita, "uid", pconf->pstate[i].uid);
				jsonapi_parser_string(ita, "phone", pconf->pstate[i].phone);
				jsonapi_parser_number(ita, "reason", &pconf->pstate[i].reason);
				itb = ita->next;			
				ita = itb;	
				i++;	
			}
			pconf->pnum = i;
		}
	}

	return PCP_OK;

}

int parser_m_notify_json_body(IN str_t* j_body, OUT pcp_conf_ntfy_msg_t* pdialing)
{
	json_t* jdialing = NULL;

	/*convert string to jdialing*/
	jsonapi_string_to_value(&jdialing, j_body->sbuf);

	CHECK_POINTER_VALUE(jdialing);

	/*parser option from jdialing*/
	jsonapi_parser_number(jdialing, "state", &pdialing->roomstate);
	jsonapi_parser_string(jdialing, "callid", pdialing->callid);
	jsonapi_parser_string(jdialing, "roomid", pdialing->roomid);

	/*parser sdp json body */
	parser_parties_json_body(jdialing, &(pdialing->parties));

	/*release jdialing*/
	jsonapi_delete_value(&jdialing);

	return PCP_OK;
}

int parser_convert_conference_json_body(IN str_t* j_body, OUT char* callid)
{
	json_t* jdialing = NULL;

	/*convert string to jdialing*/
	jsonapi_string_to_value(&jdialing, j_body->sbuf);

	CHECK_POINTER_VALUE(jdialing);

	/*parser option from jdialing*/
	jsonapi_parser_string(jdialing, "callid", callid);

	/*release jdialing*/
	jsonapi_delete_value(&jdialing);

	return PCP_OK;
}

int parser_m_nml_rsp_json_body(IN str_t* j_body, OUT pcp_conf_nml_rsp_t *pRsp)
{
    json_t* jNmlRsp = NULL;

	/*convert string to jnotify*/
	jsonapi_string_to_value(&jNmlRsp, j_body->sbuf);

	CHECK_POINTER_VALUE(jNmlRsp);

	/*parser option from jnotify*/
	jsonapi_parser_string(jNmlRsp, "callid", pRsp->callId);
	jsonapi_parser_string(jNmlRsp, "roomid", pRsp->roomId);	
	jsonapi_parser_number(jNmlRsp, "errcode", &pRsp->errCode);

	/*release jnotify*/
	jsonapi_delete_value(&jNmlRsp);

	return PCP_OK;
}


//add by wuzhaoyang20140605 end




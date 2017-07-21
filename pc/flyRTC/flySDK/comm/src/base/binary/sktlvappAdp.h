#ifndef _SK_TLV_APP_ADP_h_
#define _SK_TLV_APP_ADP_h_

//
//#include "../sipex_callsvr_msg.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define MAX_VOIP_PACKET_LEN  1536



int pack_dtmf_info_body(dtmf_info_t* dtmf_info, char *buf, int *plen);
int unpack_dtmf_info_body(char *buf, int len, dtmf_info_t* dtmf_info);

int pack_call_notify_body(call_notify_t* call_notify, char *buf, int *plen);
int unpack_call_notify_body(char *buf, int len, call_notify_t* call_notify);

int unpack_route_update_body(char *buf, int len, route_update_t* route_update);
int pack_route_update_body(route_update_t* route_update, char *buf, int *plen);

int pack_call_update_rsp_body(call_update_rsp_t* call_update_rsp, char *buf, int *plen);
int unpack_call_update_rsp_body(char *buf, int len, call_update_rsp_t* call_update_rsp);

int pack_call_update_body(call_update_t* call_update, char *buf, int *plen);
int unpack_call_update_body(char *buf, int len, call_update_t* call_update);

int pack_call_ring_body(call_ring_rsp_t* call_ring, char *buf, int *plen);
int unpack_call_ring_body(char *buf, int len, call_ring_rsp_t* call_ring);

int pack_call_ack_body(call_ack_t* call_ack, char *buf, int *plen);
int unpack_call_ack_body(char *buf, int len, call_ack_t* call_ack);

int pack_call_rsp_body(call_rsp_t* call_rsp, char *buf, int *plen);
int unpack_call_rsp_body(char *buf, int len, call_rsp_t* call_rsp);

int pack_call_req_body(call_req_t* call_req, callsvr_hdr_t *hdr, char *buf, int *plen);
int unpack_call_req_body(char *buf, int len, call_req_t* call_req, callsvr_hdr_t *hdr);

int pack_call_bye_body(bye_req_t* bye_req, char *buf, int *plen);
int unpack_bye_req_body(char *buf, int len, bye_req_t* bye_req);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif 



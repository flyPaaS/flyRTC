/***********************************************************************
 *
 * File:        ProxyProtocol.h
 * Author:      IGG
 * Revision:    $Id: ProxyProtocol.h $
 * Description: 数据包
 *
 ***********************************************************************/

#ifndef KCT_PROXY_PROXY_PROTOCOL_H
#define KCT_PROXY_PROXY_PROTOCOL_H

/**
 * 通讯协议
 */
struct ProxyProtocol {
	enum {
		PROTO_VER			= 1,	/**< 协议版本号 */
		MIN_HEADER_LEN		= 16, 	/**< 最短头部长度 */
		MAX_HEADER_LEN		= 16  	/**< 最长头部长度 */
	};

	enum {
		RESP_NONE					= 10000,		/**< 无响应 */


		REQ_AUTH					= 1,		/**< Auth 请求 */
		RESP_AUTH       			= 30001,  	/**< Auth 响应 */
		REQ_REAUTH       			= 2,		/**< ReAuth请求 使用SK */
		RESP_REAUTH       			= 30002,		/**< ReAuth响应 使用SK */

		REQ_SEND_MSG    			= 3,		/**< SendMsg 请求 */
		RESP_SEND_MSG   			= 30003,		/**< SendMsg 响应 */
		REQ_NEW_SYNC        		= 600011,		/**< New Sync 请求 */
		RESP_NEW_SYNC       		= 10600011,		/**< New Sync 响应 */

		REQ_NEW_NOTIFY  			= 600016,      /**< NewNotify 请求 */
		REQ_NEW_SYNCCHK 			= 6,      /**< NewSyncCheck 请求 */
		RESP_NEW_SYNCCHK			= 30006,		/**< NewSyncCheck 请求 */

		REQ_UPLOAD_VOICE  			= 600035,      /**< UploadVoice 请求 */
		RESP_UPLOAD_VOICE 			= 10600035,		/**< UploadVoice 响应 */
		REQ_DOWNLOAD_VOICE     		= 600036,      /**< DownloadVoice 请求 */
		RESP_DOWNLOAD_VOICE   		= 10600036,		/**< DownloadVoice 响应 */
		
		REQ_UPLOAD_MSGIMG  			= 600030,      /**< UploadMsgImg 请求 */
		RESP_UPLOAD_MSGIMG 			= 10600030,		/**< UploadMsgImg 响应 */
		REQ_DOWNLOAD_MSGIMG     	= 600031,      /**< DownloadMsgImg 请求 */
		RESP_DOWNLOAD_MSGIMG    	= 10600031,    /**< DownloadMsgImg 响应 */
		
		REQ_UPLOAD_VIDEO     		= 11,      /**< UploadVideo 请求 */
		RESP_UPLOAD_VIDEO    		= 30011,		/**< UploadVideo 响应 */
		REQ_DOWNLOAD_VIDEO     		= 12,      /**< DownloadVideo 请求 */
		RESP_DOWNLOAD_VIDEO    		= 30012,		/**< DownloadVideo 响应 */
		
		REQ_CREATE_GROUP			= 13,		/**< CreateGroup 请求*/
		RESP_CREATE_GROUP			= 30013,		/**< CreateGroup 响应*/

		REQ_ADD_GROUP_MEMBER		= 14,		/**< AddGroupMember 请求*/
		RESP_ADD_GROUP_MEMBER		= 30014,		/**< AddGroupMember 响应*/

		REQ_DEL_GROUP_MEMBER		= 15,		/**< DelGroupMember 请求*/
		RESP_DEL_GROUP_MEMBER		= 30015,		/**< DelGroupMember 响应*/
		
		REQ_QUIT_GROUP				= 16,		/**< QuitGroup 请求*/
		RESP_QUIT_GROUP				= 30016,		/**< QuitGroup 响应*/
        
        REQ_SEND_LOCATION		= 50,
        RESP_SEND_LOCATION		= 30050,
        
        REQ_SEND_CUSTOMMSG					= 51,
        RESP_SEND_CUSTOMMSG					= 30051,

		
		REQ_REG         			= 600001,      /**< Reg 请求 */
        RESP_REG        			= 10600001,		/**< Reg 响应 */


		REQ_INIT					= 600010,		/**< Init 请求 */
    	RESP_INIT					= 10600010,

		REQ_SYNC_FRIEND				= 600013,		/**< SyncFriend 请求*/
    	RESP_SYNC_FRIEND			= 10600013,		/**< SyncFriend 响应*/
		REQ_USERINFOSET 			= 600014,      /**< UserInfoSet 请求 */
    	RESP_USERINFOSET			= 10600014,		/**< UserInfoSet 响应 */
		REQ_GETPROFILE				= 600015,		/**< GetProfile 请求 */
		RESP_GETPROFILE				= 10600015,		/**< GetProfile 响应 */	


    	REQ_SEARCH_CONTACT 			= 600020, 		/**< SearchContact 请求*/
    	RESP_SEARCH_CONTACT			= 10600020,		/**< SearchContact 响应*/
    	REQ_VERIFY_USER				= 600021,		/**< VerifyUser 请求*/
    	RESP_VERIFY_USER			= 10600021,		/**< VerifyUser 响应*/
    	REQ_GET_CONTACT				= 600022,		/**< GetContact 请求*/
    	RESP_GET_CONTACT			= 10600022,		/**< GetContact 响应*/


		REQ_UPLOAD_HEAD_HDIMG  		= 600032,      /**< UploadHeadHDImg 请求 */
		RESP_UPLOAD_HEAD_HDIMG 		= 10600032,		/**< UploadHeadHDImg 响应 */
		REQ_GET_HEAD_HDIMG     		= 600033,      /**< GetHeadHDImg 请求 */
		RESP_GET_HEAD_HDIMG    		= 10600033,		/**< GetHeadHDImg 响应 */
		REQ_GET_HEAD_IMG     		= 600034,      /**< GetHeadImg 请求 */
		RESP_GET_HEAD_IMG    		= 10600034,		/**< GetHeadImg 响应 */


		REQ_UPLOAD_EMOJI     		= 600039,      /**< UploadEmoji 请求 */
		RESP_UPLOAD_EMOJI    		= 10600039,		/**< UploadEmoji 响应 */
		REQ_DOWNLOAD_EMOJI    		= 600040,      /**< DownloadEmoji 请求 */
		RESP_DOWNLOAD_EMOJI    		= 10600040,		/**< DownloadEmoji 响应 */
		REQ_UPLOADCOVERIMG			= 600041,		/**< UploadCoverImg 请求 */
		RESP_UPLOADCOVERIMG			= 10600041,		/**< UploadCoverImg 响应 */	
		REQ_GETCOVERIMG				= 600042,		/**< GetCoverImg 请求 */
		RESP_GETCOVERIMG			= 10600042,		/**< GetCoverImg 响应 */	
			
		REQ_LBSFIND					= 600060,		/**< LbsFind 请求 */
		RESP_LBSFIND				= 10600060,		/**< LbsFind 响应 */
		REQ_GETLOCATION				= 600061,		/**< GetLocation 请求 */
		RESP_GETLOCATION			= 10600061,		/**< GetLocation 响应 */
		REQ_LBSMATCH				= 600062,		/**< LbsMatch 请求 */
		RESP_LBSMATCH				= 10600062,		/**< LbsMatch 响应 */
		REQ_SYNCLBSMATCH			= 600063,		/**< SyncLbsMatch 请求 */
		RESP_SYNCLBSMATCH			= 10600063,		/**< SyncLbsMatch 响应 */

		//=========================================================

		REQ_MOBILE_OPT				= 600070,
		RESP_MOBILE_OPT				= 10600070,
		REQ_EMAIL_OPT				= 600071,
		RESP_EMAIL_OPT				= 10600071,
		REQ_UPLOAD_PLUGFRIENDS		= 600072,
		RESP_UPLOAD_PLUGFRIENDS		= 10600072,
		REQ_PUSHSETTING				= 600073,
		RESP_PUSHSETTING			= 10600073,

		REQ_MODUSERNAME				= 600074,
		RESP_MODUSERNAME			= 10600074,

		REQ_MODPWD					= 600075,
		RESP_MODPWD					= 10600075,

		REQ_MODPWD_BYMOBILE			= 600076,
		RESP_MODPWD_BYMOBILE		= 10600076,

		REQ_GET_DNS 				= 600077,      /**< 获取 ip 请求 */
		RESP_GET_DNS 				= 10600077,		/**< 获取 ip 响应 */
		
		//==========================群组相关开始==================

		REQ_CREATE_CHATROOM			= 600080,		/**< CreateChatRoom 请求*/
		RESP_CREATE_CHATROOM		= 10600080,		/**< CreateChatRoom 响应*/

		REQ_ADD_CHATROOM_MEMBER		= 600081,		/**< AddChatRoomMember 请求*/
		RESP_ADD_CHATROOM_MEMBER	= 10600081,		/**< AddChatRoomMember 响应*/

		REQ_DEL_CHATROOM_MEMBER		= 600082,		/**< DelChatRoomMember 请求*/
		RESP_DEL_CHATROOM_MEMBER	= 10600082,		/**< DelChatRoomMember 响应*/

		REQ_QUIT_CHATROOM			= 600083,		/**< QuitChatRomm 请求 */
		RESP_QUIT_CHATROOM			= 10600083,		/**< QuitChatRomm 响应*/

		REQ_SEARCH_CHATROOM			= 600084,		/**< SearchChatRoom 请求*/
		RESP_SEARCH_CHATROOM		= 10600084,		/**< SearchChatRoom 响应*/

		REQ_LBSCHATROOMFIND			= 600085,		/**< LbsChatRoomFind 请求 */
		RESP_LBSCHATROOMFIND		= 10600085,		/**< LbsChatRoomFind 响应 */

		REQ_VERIFYCHATROOMMEMBER	= 600086,		/**< VerifyChatRoomMember请求 */
		RESP_VERIFYCHATROOMMEMBER	= 10600086,		/**< VerifyChatRoomMember 响应 */
		
		REQ_INVITECHATROOMMEMBER	= 600087,		/**< InviteChatRoomMember请求 */
		RESP_INVITECHATROOMMEMBER	= 10600087,		/**< InviteChatRoomMember 响应 */
		
		REQ_CLOSECHATROOM			= 600088,		/**< CloseChatRoom请求 */
		RESP_CLOSECHATROOM			= 10600088,		/**< CloseChatRoom 响应 */


		//==========================群相关结束==================
		
		REQ_REPORT_PROFILE			= 600094,		/**< ReportProfile 请求*/
		RESP_REPORT_PROFILE			= 10600094,		/**< ReportProfile 响应*/
		
		REQ_REPORT_GROUP			= 600095,		/**< ReportGroup 请求*/
		RESP_REPORT_GROUP			= 10600095,		/**< ReportGroup 响应*/
		
		REQ_NOOP        			= 600100,      /**< Noop 请求 */
		RESP_NOOP       			= 10600100,		/**< Noop 响应 */
		REQ_QUIT        			= 600101,      /**< Quit 请求 */
		REQ_DIRECT_SEND 			= 600102,      /**< DirectSend 请求 */
		
		REQ_SYNC_CHECK		= 600103,
		RESP_SYNC_CHECK		= 10600103,
		REQ_FIX_SYNCCHK		= 600104,
		RESP_FIX_SYNCCHK	= 10600104,

		REQ_BULLETIN    			= 700000,      /**< 故障公告 */
		REQ_SPEED_TEST_NOTIFY 		= 700001,     	/**< 测速通知 */
		REQ_SPEED_TEST  			= 700002,      /**< 测速请求 */
		RESP_SPEED_TEST 			= 10700002,		/**< 测速响应 */
		REQ_RPT_SPEED_TEST 			= 700003,      /**< 上报测速结果请求 */
		RESP_RPT_SPEED_TEST 		= 10700003,		/**< 上报测速结果响应 */
		
		REQ_VOIP_INVITE_NOTIFY 		= 700010,   	/**< VoipInviteNotify 请求 */
		REQ_NEWSYNC_SYNCWAIT 		= 700011,		/**< 支持syncwait的newsync请求*/
		RESP_NEWSYNC_SYNCWAIT 		= 10700011,		/**< 支持syncwait的newsync响应*/
		REQ_NEWSYNC_NOTIFYDATA 		= 700012,		/**< newsync notifydata请求*/
		REQ_NEWSYNC_ACK 			= 10700012,		/**< newsync ack 相应*/

		RESP_VOIP_SYNC			= 10700013,
		RESP_SNS_SYNC			= 10700014,
		REQ_NOTIFY			= 700015,
		REQ_VOIP_NOTIFY			= 700016,

		REQ_CONN        			= 700300,   	/**< 内部指令: 连接 */
		REQ_DISCONN     			= 700301,   	/**< 内部指令: 断线 */
		REQ_ONLINE      			= 700302,   	/**< 内部指令: 上线 */
		REQ_OFFLINE     			= 700303,   	/**< 内部指令: 下线 */
		REQ_SWITCH      			= 700304,   	/**< 内部指令: 切换 */
		REQ_CLI_NOT_FOUND 			= 700305,      	/**< 内部指令: 客户端不存在 */
        
        
	};
};

#endif

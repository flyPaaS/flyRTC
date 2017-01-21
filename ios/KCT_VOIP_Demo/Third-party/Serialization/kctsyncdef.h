#ifndef  MMSYNC_DEFINE_H__
#define  MMSYNC_DEFINE_H__


enum
{
	KCTPKG_BESTSIZE=65536,
};

typedef enum
{
	IMReportErrTypeMasterConnect = 1,	//连接主IP失败
	IMReportErrTypeSlaveConnect,		//连接备IP失败
	IMReportErrTypeConnect,				//连接失败
	IMReportErrTypeSect,				//无法通过Sect 找到可用的服务器
	IMReportErrTypeLogic,				//cliProto.GetRespHeader()->iResult != 0
	IMReportErrTypeArgs,				//参数错误
	IMReportErrTypeMigrateConnect,		//数据迁移服务器连接失败
	IMReportErrTypeSys,					//ret != 0
	IMReportErrTypeMysqlQuery,			//MysqlQuery!=0
	IMReportErrTypeMysqlAffect,			//affectedRows <= 0
	IMReportErrTypeProxy,
}IMReportErrType;

#define IMReportMysqlOptInsert 	"insert"	//insert 操作
#define IMReportMysqlOptSelect 	"select"	//select 操作
#define IMReportMysqlOptUpdate 	"update"	//update 操作
#define IMReportMysqlOptDel 		"delete"	//de l操作

#define IMReportProxyConn 		"connect"		//connect 
#define IMReportProxyDisconn 	"disconnect"	//disconnect
#define IMReportProxyRecv	 	"recv"		//recv
#define IMReportProxySend 		"send"		//send


/////////////////////////// 错误码定义/////////////////////////////
enum
{
	// Every thing is ok
	KCT_OK = 0,
	// 服务器错误
	KCT_ERR_SYS = -1,
	// 参数错误
	KCT_ERR_ARG = -2,
	// 密码错误
	KCT_ERR_PASSWORD = -10,
	// 不存在该用户
	KCT_ERR_NOUSER = -11,
	// 权限不够
	KCT_ERR_ACCESS_DENIED = -12,
	// 需要输入验证码
	KCT_ERR_NEED_VERIFY = -13,
	// 用户名已经存在
	KCT_ERR_USEREXIST = -14,
	// 邮箱已经被绑定
	KCT_ERR_EMAILEXIST = -15,
	// 邮箱不存在
	IM_ERR_EMAIL_NOTEXIST = -16,
	// 发送贴图失败，详细错误信息参照list ret
	IM_ERR_EMOJI_SEND = -17,
	// 用户不存在高清头像
	KCT_ERR_NO_HDHEADIMG = -18,
	// 该手机已经被其他人绑定
	KCT_ERR_MOBILE_BINDED = -19,
	// 该手机不是你的绑定手机
	KCT_ERR_MOBILE_UNBINDED = -20,
	// 你已经绑定了这个手机
	KCT_ERR_USER_BIND_MOBILE = -21,
	// 验证码不存在
	KCT_ERR_VERIFYCODE_NOTEXIST = -22,
	// 验证码不正确
	KCT_ERR_VERIFYCODE_UNMATCH = -23,
	// 验证码已经过期
	KCT_ERR_VERIFYCODE_TIMEOUT = -24,
	// 手机注册的用户，不能解绑 
	KCT_ERR_UNBIND_REGBYMOBILE = -25,
	// 手机号码格式错误
	KCT_ERR_MOBILE_FORMAT = -26,
	//  该手机号码不存在
	IM_ERR_MOBILE_NOT_EXIST = -27,
	// 用户名已经修改过了，不允许再次修改
	IM_ERR_USERNAME_HADMOD = -28,
	// 验证票根不存在
	IM_ERR_TICKET_NOTEXIST = -29,
	// 验证票根不正确
	IM_ERR_TICKET_NOTMATCH = -30,
	// 发送文本消息失败，具体错误见list errcode
	IM_ERR_SENDMSG = -31,
	// 不能给自己发消息
	IM_ERR_MSG_SEND_SELF = -32,
	// 你在对方的黑名单中
	KCT_ERR_BLACKLIST = -33,
	//需要验证
	KCT_ERR_NEED_VERIFY_USER = -34,
	// 你不是这个群的成员
	KCT_ERR_NOTCHATROOMCONTACT = -35,
	// 不在配对列表中
	IM_ERR_NOTFIND_LBSMATCH = -36,
	// 自己是隐身的，不能示好
	KCT_ERR_LBSMATCH_INVISIBLE = -37,
	// 达到每日示好次数上限
	KCT_ERR_LBSMATCH_COUNT = -38,
	// 已经示好过了
	IM_ERR_HASLIKE = -39,
	// 已经配对成功，不能再示好
	IM_ERR_HASMATCH = -40,
	// 昵称已改过，现在是只读
	IM_ERR_READONLY = -41,
	// 群组成员数达到上限
	KCT_ERR_MEMBER_TOOMUCH = -42,
	// 群聊踢人，不是群主返回
	KCT_ERR_IS_NOT_OWNER = -43,

	//搜索群时，群不存在
	IM_CHATROOM_NOTEXIST = -44,
	//add ...

	//搜索群时，群已解散
	IM_ERR_CHATROOM_CLOSED = -45,

	// 附近的人或群达到搜索人数上限
	IM_ERR_LBSFIND_MAXTAKE = -46,

	//上传CDN服务器失败
	IM_ERR_CDN = -47,
	
	IM_ERR_SYSTEM_ACCOUNT = -48,

	//你已经是这个群的成员
	KCT_ERR_ISCHATROOMCONTACT = -49,

	//该FB账号已经被别人绑定过
	KCT_ERR_FACEBOOK_BINDED = -50,

	//客户端被封号
	KCT_ERR_FORBID_LOGIN = -51,

	//不能添加自己为好友
	KCT_ERR_ADD_SELF = -52,
	
		//网卡假死
	IM_CLI_TIMEOUT_A_DEATH =  -0xFFFE, 
	//客户端占坑使用
	IM_CLI_TIMEOUT = -0xFFFF,
};

// 邀请讨论组成员加入时的返回值
enum enKCTMemberStatus
{
	KCT_MEMBER_OK = 0,
	// 成员用户不存在
	KCT_MEMBER_NOUSER = 1,
	// 用户名无效
	KCT_MEMBER_USERNAMEINVALID = 2,
	// 在对方黑名单中
	KCT_MEMBER_BLACKLIST = 3,
	// 非好友
	KCT_MEMBER_NEEDVERIFYUSER = 4,
	// 已经是成员
	KCT_MEMBER_IS_MEMBER = 5,
	// 其他错误
	KCT_MEMBER_ERROR = 6,
	// 系统账号
	KCT_SYS_ACCOUNT = 7,
};

//===================以下是服务端备用的，客户端暂时不需要理会=====================
enum
{
	KCT_ERR_EMAILNOTVERIFY = -109,

	// username reserved
	KCT_ERR_USERRESERVED= -110,

	// nickname reserved
	KCT_ERR_NICKRESERVED= -111,

	// Uin already exist
	KCT_ERR_UINEXIST= -112,

	// sessionkey not exist, need auth 
	KCT_ERR_SESSIONTIMEOUT= -113,

	// username invalid
	KCT_ERR_USERNAMEINVALID= -114,

	// nickname invalid
	KCT_ERR_NICKNAMEINVALID= -115,

	// client need critical update
	KCT_ERR_CRITICALUPDATE = -116,

	// client need recommend update
	KCT_ERR_RECOMMENDEDUPDATE = -117,

	// do not have update info
	KCT_ERR_NOUPDATEINFO = -118,

	// do not open private msg
	KCT_ERR_NOTOPENPRIVATEMSG = -119,

	// do not have this microblog contact
	KCT_ERR_NOTMICROBLOGCONTACT = -120,

	KCT_ERR_SPAM = -124,

	// DOMAIN EMAIL has beed verifyed
	KCT_ERR_DOMAINVERIFIED=-125,

	// DOMAIN verified EMAIL reach limit
	KCT_ERR_DOMAINMAXLIMITED=-126,

	// DOMAIN verified EMAIL reach limit
	KCT_ERR_DOMAINDISABLE=-127,

	// email bad format
	KCT_ERR_BADEMAIL= -128,

	// oidb timeout
	KCT_ERR_OIDBTIMEOUT = -129,

	// can not auth, need reg
	KCT_ERR_NEEDREG = -130,

	// can not auth, need reg
	KCT_ERR_NEEDSECONDPWD= -131,

	// freq limited
	KCT_ERR_FREQ_LIMITED = -134,

	// bind | unbind | bindchecked opmode error
	KCT_ERR_INVALID_BIND_OPMODE = -137,

	// upload m contact opmode error
	KCT_ERR_INVALID_UPLOADMCONTACT_OPMODE = -138,

	KCT_ERR_MOBILE_NULL = -139,
	// bind check 's req 's mobile != bind read 's mobile
	KCT_ERR_UNMATCH_MOBILE = -140,

	//uploadmcontact , req's mobile not user's bind mobile
	KCT_ERR_USER_MOBILE_UNMATCH = -142,

	//batchgetcontactprofile err mode
	KCT_ERR_BATCHGETCONTACTPROFILE_MODE = -145,



	//ticket 
	KCT_ERR_TICKET_UNMATCH = -147, 

	//ticket not found
	KCT_ERR_TICKET_NOTFOUND = -148,



	//binduin binded by other
	KCT_ERR_BINDUIN_BINDED = -150,

	// some user is not existed in user's contact for groupcard 
	KCT_ERR_INVALID_GROUPCARD_CONTACT = -152,

	// the user has no hd headimg
	KCT_ERR_HAS_NO_HEADIMG = -153,

	// invalid hd headimg request, invalid totallen
	KCT_ERR_INVALID_HDHEADIMG_REQ_TOTAL_LEN = -154,

	// can't get bottle count
	KCT_ERR_NO_BOTTLECOUNT = -156,

	//send verify code by mobile failed
	KCT_ERR_SEND_VERIFYCODE = -157,

	//pick bottle no bottle
	KCT_ERR_PICKBOTTLE_NOBOTTLE = -158,

	//unsupport country for bind or reg
	KCT_ERR_UNSUPPORT_COUNTRY = -159,

	// gmail pwd wrong
	KCT_ERR_GMAIL_PWD = -160,

	// gmail online limite
	KCT_ERR_GMAIL_ONLINELIMITE = -161,

	// gmail need web login
	KCT_ERR_GMAIL_WEBLOGIN = -162,

	// gmail imap not open
	KCT_ERR_GMAIL_IMAP = -163, 

	// mail parse fail
	KCT_ERR_PARSE_MAIL = -164, 

	// facebook绑定 已绑定返回该值
	KCT_ERR_HAVE_BIND_FACEBOOK = -167,

	// facebook accesstoken不合法
	KCT_ERR_INVALID_TOKEN = -168,



	/* facebook getmfriend 未准备好(AsyncMq还在拉数据) */
	KCT_ERR_GETMFRIEND_NOT_READY = -170,

	/* MassSend to many users in ToList */
	KCT_ERR_TOLIST_LIMITED = -171,


	//错误手机号，但建议用户修正手机号码。放在BaseResponse的ErrMsg
	KCT_ERR_MOBILE_NEEDADJUST = -174,

	// 不在灰度版本的白名单内，不允许操作
	KCT_ERR_ALPHA_FORBIDDEN = -175,

	//uuid已经被绑定
	KCT_ERR_UUID_BINDED = -176,

	//session key 不正确
	KCT_ERR_WRONG_SESSION_KEY = -177,

	//手机格式错误 用户区分 -41 的返回 客户端上来的手机号已经格式错误了 还是 走oversea svr失败的时候格式错误
	KCT_ERR_SVR_MOBILE_FORMAT = -178,

	//新用户无法申请灰度白名单
	KCT_ERR_NEW_USER = -179,

	//微博推荐新格式消息转换失败，就不下发这条消息
	KCT_ERR_WEIBO_PUSH_TRANS = -180,

        //没有绑定号码
        KCT_ERR_NOTBIND = -181,

        //只剩下一种绑定关系，不能解绑
        KCT_ERR_ONE_BINDTYPE_LEFT = -182,

        //已经解绑，不能重复解绑
        KCT_ERR_HAS_UNBINDED = -183,

        //已经绑定，不能再绑
        KCT_ERR_HAS_BINDED = -184,

        //已经被别人绑定
        KCT_ERR_BINDED_BY_OTHER = -185,

	// cancel by user: shake tran img
	KCT_ERR_SHAKE_TRAN_IMG_CANCEL = -190,

	// there is nothing upload from plugin: shake tran img
	KCT_ERR_SHAKE_TRAN_IMG_NOTFOUND = -191,
	
	// continue : shake tran img
	KCT_ERR_SHAKE_TRAN_IMG_CONTINUE = -192,
	
	// ERR: shake tran img
	KCT_ERR_SHAKE_TRAN_IMG_OTHER = -193,

	// has auth another place
	KCT_ERR_AUTH_ANOTHERPLACE = -1100,

	KCT_ERR_NO_RETRY = -1101, //client don't retry opration more , and show hints from tBaseResponse.tErrMsg while was relied by svr

	KCT_ERR_CERT_EXPIRED = -1102,// client certification is expired. need to get certification again

	//答题次数用完
	KCT_ERR_ANSWER_COUNT = -1150,

	//提问次数用完
	KCT_ERR_QUESTION_COUNT = -1151,
	
	//没有问题
	KCT_ERR_NO_QUESTION = -1152,

	//关系不对劲
	KCT_ERR_QA_RELATION = -1153,

	//号码被冻结
	KCT_ERR_ACCOUNT_BAN = -1200,

	//号码被封
	KCT_ERR_BAN = -201,

	//解绑手机号，需要密码验证
	KCT_ERR_UNBIND_MOBILE_NEEDPWD = -202,

	KCT_ERR_NODATA = -203,

	//帐号相关
	KCT_ERR_OTHER_MAIN_ACCT =  -204, //需要其它主帐号登陆

	KCT_ERR_OK_NEED_MOBILE = -205, //验证OK，需要验证手机

	//解绑有关
	KCT_ERR_UNBIND_MAIN_ACCT = -206, //解绑号为主帐号

	KCT_ERR_TRYEMAILPWD = -207, //重试username+email密码

	//设置EMAIL为主帐号，当Email需要验证后主帐号生效
	KCT_ERR_SET_EMAIL_MAIN_ACCT_NEED_VERIFY = -208,
	KCT_ERR_MAIN_ACCT_HAS_SET = -209, //主帐号已经被设置了

	KCT_ERR_WX_PWD_EXPIRED = -210, //wx密码过期，不再有效

	KCT_ERR_NOT_MEET_REGION = -211 , //上传的不是需要的目标区数据
	/* big chatroom */
	KCT_ERR_BIGCHATROOM_NOQUOTA = -250,
	KCT_ERR_BIGCHATROOM_NONEED = -251,

        // This and all future requests should be directed to the given IDC Host and Builtin IP
        KCT_ERR_IDC_REDIRECT = -301,
        
	//通过好友验证时查不到两者发起验证的记录
	KCT_ERR_USER_NOT_VERIFYUSER = -302,

    	KCT_ERR_NEED_BIND_QRCODE = -350,

	KCT_ERR_PROXY_SHM_FULL = -400,

	//img has read to much
	KCT_ERR_IMG_READ = -1005,

	KCT_ERR_LBSDATANOTFOUND = -2000,

	// 由于被举报多次而屏蔽使用LBS
	KCT_ERR_LBSBANBYEXPOSE = -2001,

	// 由于被举报多次而屏蔽使用漂流瓶
	KCT_ERR_BOTTLEBANBYEXPOSE = -2002,

	// 由于被举报多次而屏蔽使用摇一摇
	KCT_ERR_SHAKEBANBYEXPOSE = -2003,

	// 由于被举报多次而屏蔽使用二维码打招呼
	KCT_ERR_QRCODEVERIFY_BANBYEXPOSE = -2004
};

#define KCTCONFIG_FILE_PATH "/opt/im/etc/kctconfig.conf"
#define KCTUPDATECONFIG_FILE_PATH "/opt/im/etc/kctupdateconfig.conf"
#define KCTPACKAGECONFIG_FILE_PATH "/opt/im/etc/kctpackageconfig.conf"
#define KCTSHAKEIMG_FILE_PATH "/opt/im/etc/kctshakeimglogic.conf"
#define KCTSEARCH_QRCODE_CONFIG_FILE_PATH "/opt/im/etc/kctsearchqrcodeconfig.conf"
#define KCTQRCODE_SCAN_CONFIG_FILE_PATH "/opt/im/etc/kctqrcodescanf.conf"
#define KCTMATCH_URL_CONFIG_FILE_PATH "/opt/im/etc/kctmatchurl.conf"

//FuncCtrl 配置文件路径
#define KCTFUNCCTRL_FILE_PATH "/opt/im/etc/client/kctfuncctrl.conf"

#define KCT_INVALID_TIME -1

// DeviceType 
#define DEVICE_TYPE_SYMBIAN60V3 "Symbian60V3"
#define DEVICE_TYPE_SYMBIAN60V5 "Symbian60V5"
#define DEVICE_TYPE_IPHONE_OS3 	"IPhone OS3.1.3"
#define DEVICE_TYPE_IPHONE_OS4 	"IPhone OS4.2"
#define DEVICE_TYPE_ANDROID1 	"android-1"
#define DEVICE_TYPE_ANDROID2	"android-2"
#define DEVICE_TYPE_ANDROID3	"android-3"
#define DEVICE_TYPE_ANDROID4	"android-4"
#define DEVICE_TYPE_ANDROID5	"android-5"
#define DEVICE_TYPE_ANDROID6	"android-6"
#define DEVICE_TYPE_ANDROID7	"android-7"
#define DEVICE_TYPE_ANDROID8	"android-8"
#define DEVICE_TYPE_ANDROID9	"android-9"
#define DEVICE_TYPE_ANDROID10	"android-10"
#define DEVICE_TYPE_ANDROIDM9	"android-m9"
#define DEVICE_TYPE_ANDROID_MIPS_JZ	"android-mips-jz"

#define DEVICE_TYPE_WINPHONE7	"wp7"
#define DEVICE_TYPE_BLACKBERRY  "blackberry"
#define DEVICE_TYPE_WINPHONE8	"wp8"

#define ANDROID_SIGNATURE "18c867f0717aa67b2ab7347505ba07ed"

enum enChatRoomStatus
{
	CHATROOM_STATUS_NO_MSG = 0,		// 刚建群，未有消息
	CHATROOM_STATUS_HAS_MSG = 1,  	// 已经插入过系统消息
	CHATROOM_STATUS_CLOSED = 2,		//群已被关闭
	CHATROOM_STATUS_NORMAL = 3,		// 正常
};


/* 用于push的CMD */
enum enPushCMD
{
	PUSH_CMD_TXT_MSG = 104,		// 普通消息
	PUSH_CMD_PRIVATE_MSG = 105, // 私信
	PUSH_CMD_CHATROOM_MSG = 106, // 群聊消息
	PUSH_CMD_MAIL = 107, 		// pushmail
	PUSH_CMD_OFF_MSG = 108, 	// 离线相关
	PUSH_CMD_BOTTLE = 109, 		// 漂流瓶
};

enum enUpdateType
{
	UPDATETYPE_CRITICAL = 1,      // 强制升级
	UPDATETYPE_RECOMMENDED = 2,   // 推荐升级
	UPDATETYPE_MANUAL = 3,        // 手动升级
	UPDATETYPE_INCREMENTAL = 4,        // 增量升级
};

enum enPushSoundType
{
	PUSHSOUND_TYPE_MSG = 1,     // msg
	PUSHSOUND_TYPE_VOIP = 2,   // voip

};

enum emiMMRespTypeSet
{
	KCT_RETCODE = 1, // Errcode from the stop step
	KCT_RESP = 2,
};

/// 操作功能号
enum emMMFunc
{
	KCTFunc_Auth = 101,			// 验证
	KCTFunc_Reg= 102,    // 注册
	KCTFunc_Sync= 103,    // 同步
	KCTFunc_SendMsg= 104, // 发送消息
	KCTFunc_IphoneReg = 105, // Iphone注册token
	KCTFunc_SearchContact = 106,   // 搜索Contact
	KCTFunc_GetVerifyImg = 107,   // 获取验证码
	KCTFunc_SendVerifyEmail = 108,  //验证注册邮箱
	KCTFunc_DownloadMsgImg = 109, //取消息中的大图片
	KCTFunc_UploadMsgImg = 110,  // 客户端上传消息中的大图片
	KCTFunc_ChatStatus = 111, // 发送或取消聊天状态
	KCTFunc_IphoneUnReg = 112, // Iphone注销token
	KCTFunc_GetUpdateInfo = 113, // 获取版本升级信息
	KCTFunc_GetUpdatePack = 114, // 获取版本文件
	KCTFunc_GetInviteFriend = 115, //获取邀请好友
	KCTFunc_SendInviteEmail = 116, // 发送邀请信
	KCTFunc_SendPrivateMsg = 117, //  发送微博私信
	KCTFunc_UploadPrivateMsgImg = 118, //发送微博图片
	KCTFunc_CreateChatRoom = 119, // 创建聊天群组
	KCTFunc_AddChatRoomMember = 120, //新增聊天群成员
	KCTFunc_Init = 121, // Init
	KCTFunc_SendDomainEmail= 122, // Init
	KCTFunc_BatchGetHeadImg = 123, //批量拉去头像
	KCTFunc_SearchFriend = 124, // 搜索好友
	KCTFunc_GetUserName = 125, //通过NickName生成UserName
	KCTFunc_NewReg = 126, //新注册协议
	KCTFunc_UploadVoice = 127,
	KCTFunc_DownloadVoice = 128,
	KCTFunc_SwithPushMail = 129,
	KCTFunc_UploadWeiboImg = 130,  // 往微博发送照片
	KCTFunc_SendCard = 131,  // 往微博发自己的名片 
	KCTFunc_MobileOpt = 132, // 手机操作
	KCTFunc_UploadMContact = 133, //上传手机联系人
	KCTFunc_ModifyHeadImg= 134, //修改用户头像
	KCTFunc_NewGetInviteFriend= 135, //拉取全部好友的cgi
	KCTFunc_SyncFriend=136, //看看谁在用
	KCTFunc_VerifyUser=137, //好友验证
	KCTFunc_NewSync=138, // newsync
	KCTFunc_NewInit=139, // newsync
	KCTFunc_BatchGetContactProfile=140,
	KCTFunc_GetPSMImg=141,  //获取PushSystemMsg里面的图片
	KCTFunc_GetMFriend=142,  //获取手机好友消息
	KCTFunc_GetGroup=143,  //获取好友
	KCTFunc_Bind=144,  //绑定号
	KCTFunc_BindMobileForReg=145, //手机号注册 验证
	KCTFunc_ShareFriendCard=146, //分享好友名片
	KCTFunc_GetResetPwdUrl=147, //下发重置密码url到用户手机
	KCTFunc_LbsFind=148, //LBS查找周围联系人
	KCTFunc_UploadVideo=149,
	KCTFunc_DownloadVideo=150,
	KCTFunc_AddGroupCard=151, //添加群名片
	KCTFunc_GetBottleCount=152, //获取瓶子数
	KCTFunc_SendFeedback =153, //发送用户反馈
	KCTFunc_ThrowBottle=154, //扔瓶子
	KCTFunc_PickBottle=155, //捡瓶子
	KCTFunc_OpenBottle=156, //开瓶子
	KCTFunc_UploadHDHeadImg=157, // 上传高清头像
	KCTFunc_GetHDHeadImg=158, // 获取高清头像
	KCTFunc_GetPackageList=159, // 获取主题列表
	KCTFunc_DownloadPackage=160, //  获取主题
	KCTFunc_ShakeReport=161,// 上报 Shake 信息
	KCTFunc_ShakeGet=162,// 获取 Shake 结果
	KCTFunc_ShakeMatch=163,// 请求 Shake 匹配
	KCTFunc_Echo=164, // 拨测专用CGI
	KCTFunc_ShakeImg=165,// 请求 shake 背景图片
	KCTFunc_Expose=166,// 举报
	KCTFunc_GetVUserInfo=167, //获取认证用户信息
	KCTFunc_GetQRCode=168, //获取用户二维码
	KCTFunc_GmailOper=169, //gmail帐号绑定与解绑
	KCTFunc_VoipInvite=170, //Voip邀请
	KCTFunc_VoipCancelInvite=171, //Voip取消邀请
	KCTFunc_VoipAnswer=172, //Voip应答
	KCTFunc_VoipShutDown=173, //Voip挂断
	KCTFunc_VoipSync=174, //Voip Sync
	KCTFunc_UploadEmoji =175, //发送自定义表情
	KCTFunc_DownloadEmoji =176, //下载自定义表情
	KCTFunc_GeneralSet = 177, //通用设置, 满足阻塞的需求，避免总是增加cgi 
	KCTFunc_VoipHeartBeat = 178, //Voip HeartBeat 
	KCTFunc_DelChatRoomMember = 179, //群主踢人
	KCTFunc_GetMailOAuthUrl = 180, // 获取邮件OAuth的URL
	KCTFunc_CollectChatRoom = 181, //群名片转化成群
	KCTFunc_GetContact = 182, //get contact
	KCTFunc_FaceBookAuth = 183, //facebook绑定/解绑CGI
	KCTFunc_GetAlbumPhotoListFP = 184, //拉取第一页个人相册photoinfo
	KCTFunc_BatchGetAlbumPhoto = 185, //批量拉取相册缩略图
	KCTFunc_BatchGetAlbumPhotoAttr = 186, //批量拉取相册图片附属信息
	KCTFunc_UploadAlbumPhoto = 187, //上传个人相册图片
	KCTFunc_DownloadAlbumPhoto = 188, //上传个人相册图片
	KCTFunc_DelAlbumPhoto = 189, //删除个人相册图片
	KCTFunc_AlbumPhotoComment = 190, //标红心
	KCTFunc_GetTLPhotoListFP = 191, //拉取timeline第一页
	KCTFunc_VoipStat = 192, //voip通话质量数据上报
	KCTFunc_MassSend = 193,
	KCTFunc_AlbumSync = 194, //个人相册Sync
	KCTFunc_SearchQRCode = 195, //二维码搜索
	KCTFunc_GetAlbumPhotoListNP = 196, //拉取下一页个人相册photoinfo
	KCTFunc_GetTLPhotoListNP = 197, //拉取下一页timeline photoinfo
	KCTFunc_GetLatestTLPhoto = 198, //拉取最新的timelinet photoid
	KCTFunc_GetPhotoCommentList = 199, //获取一个照片的评论 
	KCTFunc_AlbumLbs = 200, //给相册获取他们的lbs信息
	KCTFunc_WinphoneReg = 201, //winphonereg 配置长连
	KCTFunc_WinphoneUnReg = 202, //winphoneunreg 配置长连
	KCTFunc_AlbumOperation = 203, //相册照片的一些操作
	KCTFunc_ShakeSync = 204, // 摇一摇传图绑定
	KCTFunc_GetWeiBoURL = 205, //  获取微博链接，带登陆态
	KCTFunc_VoiceAddr = 206, //语音通讯录
	KCTFunc_SnsUpload = 207, //sns上传buffer
	KCTFunc_SnsDownload = 208, //sns下载buffer
	KCTFunc_SnsPost = 209, //sns发布事件
	KCTFunc_SnsObjectDetail = 210, //sns事件详情页
	KCTFunc_SnsTimeLine = 211, //sns获取timeline
	KCTFunc_SnsUserPage = 212, //sns个人页面
	KCTFunc_SnsComment = 213, //sns评论
	KCTFunc_SnsSync = 214, //sns消息系统同步
	KCTFunc_GetLocation = 215, //get location
	KCTFunc_SnsLbs = 216, //sns地理位置信息
	KCTFunc_SetPassWd = 217, //手机注册设置密码
	KCTFunc_SnsObjectOp = 218, //sns事件修改操作
	KCTFunc_VerifyPassWd = 219, //验证密码
	KCTFunc_UploadAppAttach = 220, // upload app attach
	KCTFunc_DownloadAppAttach = 221, // download app attach
	KCTFunc_SendAppMsg = 222, // send app msg 
	KCTFunc_ImportFriends = 230,	 // add or recommend friends after reg by facebook
	KCTFunc_GetAppInfo = 231,  //get app info, by iver
	KCTFunc_GetRecommendAppList = 232,  //get recommend app list, by iver
	KCTFunc_GetA8Key = 233,  //a2 key to a8 key
	KCTFunc_UploadMedia = 240, // upload media 
	KCTFunc_HandleImgMsg = 241, // handle img msg after uploading media
	KCTFunc_FreqBlock = 242, //频率拦截用
	KCTFunc_GetQuestion = 243, //公众平?ㄎ蚀? 获取问题
	KCTFunc_GetQACount = 244, //公众平台问答 获取提问or问题剩余次数
	KCTFunc_UserInfoSet = 245, //bingchen 2014.2.20 用户信息设置
	KCTFunc_ReAuth = 246,
	KCTFunc_NewSyncCheck = 247,
	KCTFunc_QuitChatRoom = 248, //???????
	KCTFunc_SearchChatRoom = 249, //???
	// add here

	KCTFunc_StatReport = 250,
	KCTFunc_StatusNotify = 251, //web和手机会话同步
	KCTFunc_PropertySurvey = 252,
	KCTFunc_UnBind = 253, //解除绑定号
	KCTFunc_CheckUnBind = 254, //判断能否解绑
	KCTFunc_QueryHasPasswd = 255, //判断是否需要设置密码
    	KCTFunc_EmailOpt = 256, //Email操作
	KCTFunc_CreateGroup = 257,//?????
	KCTFunc_AddGroupMember = 258, //?????
	KCTFunc_DelGroupMember = 259, //?????
	KCTFunc_QuitGroup = 260, //?????
	KCTFunc_CollectGroup = 261, //???????????

    // add here

	KCTFunc_LogOutWebIm = 281,
	KCTFunc_LogOut = 282,
	KCTFunc_WebImLoginConfirm = 283,

	// add here

	KCTFunc_SnsTagOption = 290, //sns tag 修改操作
	KCTFunc_SnsTagMemberOption = 291,//sns tag member 修改操作
	KCTFunc_SnsTagList = 292,// sns list 获取、推荐操作
	KCTFunc_SnsTagMemMutilSet = 293,// 将用户设置到许多分组

	// add here

	KCTFunc_UnbindMobileBy = 300,
	KCTFunc_SnsGetCity = 301,
	KCTFunc_GetProfile = 302,
	KCTFunc_VoipGetRoomInfo = 303,
	KCTFunc_SetPushSound = 304,
	KCTFunc_VoipAck = 305,
	KCTFunc_VoipInviteRemind = 306,
	KCTFunc_VoipGetDeviceInfo = 307,
	KCTFunc_ReportStrategyCtrl = 308,
	KCTFunc_ReportClntPerf = 309,
	KCTFunc_ReportKV = 310,
	KCTFunc_ReportAction = 311,
	KCTFunc_ReportCgiAccess = 312,
	KCTFunc_ReportCrash = 313,
	KCTFunc_BlackBerryReg = 314, //BlackBerryreg 配置长连
	KCTFunc_BlackBerryUnReg = 315, //BlackBerryunreg 配置长连
	KCTFunc_ShakeTranImgReport = 316, 
	KCTFunc_ShakeTranImgGet = 317, 
	KCTFunc_BatchGetShakeTranImg = 318, 
	KCTFunc_ShakeTranImgUnBind = 319, 
	KCTFunc_VoipStatReport = 320, 
	KCTFunc_BakChatUploadHead = 321,
	KCTFunc_BakChatUploadEnd = 322,
	KCTFunc_BakChatUploadMsg =323,
	KCTFunc_BakChatUploadMedia =324,
	KCTFunc_BakChatRecoverGetList  = 325,
	KCTFunc_BakChatRecoverHead  = 326,
	KCTFunc_BakChatRecoverData  = 327,
	KCTFunc_BakChatDelete = 328,
    KCTFunc_UploadVoiceRecognize = 329,
	KCTFunc_VoiceAddrReport = 330,
    KCTFunc_OpVoiceReminder = 331,

	KCTFunc_EnterTalkRoom = 332,
	KCTFunc_ExitTalkRoom = 333,
	KCTFunc_TalkMicAction = 334,
	KCTFunc_TalkNoop = 335,
	KCTFunc_GetTalkRoomMember = 336,

	KCTFunc_BakChatUploadDB = 337,
	KCTFunc_BakChatRecoverDB = 338,
	
	KCTFunc_GrantBigChatRoom = 339,
    KCTFunc_SendQRCodeByEmail = 340,

	KCTFunc_BindSafeMobile = 341,
	KCTFunc_AddSafeDevice = 342,
	KCTFunc_GetSetableMainAcct = 343,
	KCTFunc_SetMainAcct = 344,
	KCTFunc_SetEmailPwd = 345,

    KCTFunc_CreateTalkRoom = 346,
    KCTFunc_AddTalkRoomMember = 347,
    KCTFunc_DelTalkRoomMember = 348,

    KCTFunc_UploadInputVoice = 349,

    KCTFunc_CreateSubUser = 350, //客服号相关
    KCTFunc_UpdateSubUser= 351,
    KCTFunc_UnbindSubUser= 352,
    KCTFunc_GetSubUserList = 353,
    
    KCTFunc_GetCardList = 354,
    KCTFunc_GetCardInfo = 355,
    KCTFunc_InsertCard = 356,
    KCTFunc_DeleteCard = 357,
    KCTFunc_GetCardImg = 358,

    KCTFunc_ClickCommand = 359,

	KCTFunc_SubmsgSync = 360,

	KCTFunc_UpdateSafeDevice = 361,
	KCTFunc_DelSafeDevice = 362,

    KCTFunc_GetBrandList = 363,

	KCTFunc_TalkInvite = 364,

    KCTFunc_TransferCard = 365,
    KCTFunc_ParseCard = 366,

    KCTFunc_ShakeMusic = 367,

	//附近的生活
	KCTFunc_FindNBS = 368,
	KCTFunc_SearchNBS = 369,
	KCTFunc_GetNBSDetail = 370,
	KCTFunc_GetShowcaseInfo = 371,
	KCTFunc_GetObjectDetail = 372,

	KCTFunc_TalkStatReport = 373,

    KCTFunc_RegEquipment = 374,
    KCTFunc_ResetEquipment = 375,

	//附近的群  
	KCTFunc_JoinLbsRoom = 376,
	KCTFunc_GetRoomMember = 377,

    KCTFunc_AppComment = 378, // AppStore用户推荐评分  
	
    KCTFunc_GetCDNDns = 379, // 获取上传图片到cdn的 ip和key
    KCTFunc_NewAuth = 380,			//新验证
    KCTFunc_GetCert = 381, //获取证书
    KCTFunc_NewSetEmailPwd = 382,
    KCTFunc_NewSetPassWd = 383,
    KCTFunc_NewVerifyPassWd = 384,
    KCTFunc_LbsMatch = 385, // 附近的人配对
    KCTFunc_SyncLbsMatch = 386, // 同步配对列表

	KCTFunc_UploadCoverImg = 387,
	KCTFunc_GetCoverImg = 388,

	KCTFunc_PushSet = 389,

	KCTFunc_UploadPlugFriends = 390,		// 上传第三方联系人
	
	KCTFunc_LbsChatRoomFind = 391, //附近的群搜索/热门群搜索
	KCTFunc_VerifyChatRoomMember=392, //群成员添加验证

	//////////////////////////////////////////////////////////////////
	KCTFunc_ModUserName = 393, //修改用户名
	KCTFunc_ModPwd =394, //修改密码
	KCTFunc_ModPwdByMobile =394, //通过手机修改密码

	KCTFunc_InviteChatRoomMember=395, //邀请群成员
	KCTFunc_CloseChatRoom=396, //解散群

	KCTFunc_ReportProfile = 397, //举报用户
	KCTFunc_ReportGroup = 398, //举报群

	KCTFunc_FuzzySearchChatRoom = 399, //模糊搜索群

	KCTFunc_BanGroup = 400, //屏蔽群

	KCTFunc_SyncUploadPlugFriends = 401,		// 同步上传第三方联系人
	
	KCTFunc_AppUpdate = 402, //客户端查询新版本

	//KCTFunc_HotChatRoomFind = 403, //热门群搜索

	KCTFunc_Renewal = 404, //sessionkey 续期

	KCTFunc_AppNewUpdate = 405, //新版客户端查询版本


	KCTFunc_SendLocation = 406,
	KCTFunc_SendCustomMsg = 407,
	KCTFunc_NotExist
};

enum enGeneralSetType
{
	KCT_GENERAL_SETTYPE_ALIAS = 1, //别名
};


enum enNewRegMode
{
	KCT_REGMODE = 0,
	KCT_REGMODE_MOBILE = 1,
	KCT_REGMODE2 = 2,
	KCT_REGMODE_MOBILE_NOPWD = 4,
    KCT_REGMODE_EQUIPMENT = 5,
};

enum enGmailStatus
{
	KCT_GMAIL_OK = 0,

	KCT_GMAIL_PWD = 1,

	KCT_GMAIL_ONLINELIMITE = 2,

	KCT_GMAIL_WEBLOGIN = 3,

	KCT_GMAIL_IMAP = 4, 
};

enum enGmailOper
{
	KCT_GMAIL_BIND = 1,
	KCT_GMAIL_UNBIND = 2,
};

enum enGmailSwitch
{
	KCT_GMAIL_SWITCH_OPEN = 1,
	KCT_GMAIL_SWITCH_CLOSE = 2,
};

enum enSceneStatus
{
	KCT_SCENE_MANUALAUTH = 1,
	KCT_SCENE_AUTOAUTH = 2,
	KCT_SCENE_NORMALINIT = 3,
	KCT_SCENE_UPDATEINIT = 4,
	KCT_SCENE_SENDMSG = 5,
	KCT_SCENE_RETRYSENDMSG = 6,
	KCT_SCENE_REGINIT = 7,
	KCT_SCENE_MANUALUSERNAME = 8,
	KCT_SCENE_SYSTEMUSERNAME = 9,
	KCT_SCENE_FACEBOOK = 10,
	KCT_SCENE_AUTOAUTH_CHANGEUUID = 11, //客户端更改了uuid的自动登陆。ios6时可能出现这种情况 by jfd
	KCT_SCENE_AUTOAUTH_UPGRADE = 12, //客户端升级后第一次自动登陆 by jfd
	KCT_SCENE_WEB_IM = 13, //微信网页版使用场景
	KCT_SCENE_BROKER = 14, //mmbroker
	KCT_SCENE_REAUTH = 15,
	IM_SCENE_ETP_TOKEN = 16, //企业版TOKEN注册
	KCT_SCENE_TEST = 0XFF,
};

enum enTokenVersion
{
	IM_TOKEN_APPSTORE = 0XF1,
	IM_TOKEN_ENTERPRISE = 0XF2,
};

enum enNewRegReturnFlag
{
	KCT_NEWREG_RETURNFLAG_NEEDQFRAME = 0x1,
};

enum enRegType
{
	KCT_USERATTR_REG = 1,
	KCT_USERATTR_REG_MOBILE = 2,
	KCT_USERATTR_REG_EMAIL = 3,
	KCT_USERATTR_REG_BIZ = 4,
	KCT_USERATTR_REG_FACEBOOK = 5,
    KCT_USERATTR_REG_SUBUSER = 6,
    KCT_USERATTR_REG_EQUIPMENT = 7,
};

enum enAddContactScene 
{
	KCT_ADDSCENE_SEARCH = 1,       // 通过搜索昵称
	KCT_ADDSCENE_SEARCH_EMAIL = 2,    // 通过搜索Mail昵称
	KCT_ADDSCENE_SEARCH_IM = 3,   // 通过搜索微号
	KCT_ADDSCENE_PF = 4,           // 通过可能认识的好友 
	KCT_ADDSCENE_PF_EMAIL = 5,        // 通过可能认识的Mail好友
	KCT_ADDSCENE_PF_CONTACT = 6,      // 通过把我加到通讯录的人  
	KCT_ADDSCENE_PF_IM = 7,       // 通过可能认识的微信好友(二度关系)
	KCT_ADDSCENE_PF_GROUP = 8,        // 通过可能认识的群好友
	KCT_ADDSCENE_PF_UNKNOWN = 9,        // “可能认识的好友”（无法区分来源）
	KCT_ADDSCENE_PF_MOBILE = 10,        // 手机通讯录
	KCT_ADDSCENE_PF_MOBILE_EMAIL = 11,        // 邮箱匹配
	KCT_ADDSCENE_VIEW = 12,		//查看好友
	KCT_ADDSCENE_VIEW_MOBILE = 13,	//查看手机通讯录
	KCT_ADDSCENE_CHATROOM = 14,	//群动作
	KCT_ADDSCENE_SEARCH_PHONE = 15,	//搜索手机
	KCT_ADDSCENE_CORP_EMAIL = 16,	//工作邮箱
	KCT_ADDSCENE_SEND_CARD = 17,	//发名片
	KCT_ADDSCENE_LBS = 18,	//LBS加好友
	KCT_ADDSCENE_PF_MOBILE_REVERSE = 21,   // 手机通讯录反向好友
	KCT_ADDSCENE_PF_SHAKE_PHONE_PAIR = 22,   // sever加好友单对单 
	KCT_ADDSCENE_PF_SHAKE_PHONE_GROUP = 23,   // sever加好友群加 
	KCT_ADDSCENE_PF_SHAKE_PHONE_OPPSEX= 24,   // sever加好友1000米外异性
	KCT_ADDSCENE_BOTTLE = 25, // 漂流瓶打招呼加好友
	KCT_ADDSCENE_SHAKE_SCENE1 = 26, //shake, 3s, <= 1km
	KCT_ADDSCENE_SHAKE_SCENE2 = 27, //shake, <= 1km
	KCT_ADDSCENE_SHAKE_SCENE3 = 28, //shake <= 10km
	KCT_ADDSCENE_SHAKE_SCENE4 = 29, //shake, > 10km
	KCT_ADDSCENE_QRCode = 30,//二维码	
	KCT_ADDSCENE_FACEBOOK = 31, //facebook插件
	KCT_ADDSCENE_SNS = 32, //通过朋友圈推荐添加
	KCT_ADDSCENE_WEB = 33, // from web
	KCT_ADDSCENE_BRAND_QA = 34, //公众账号问答
    KCT_ADDSCENE_FUZZY_SEARCH = 35, //通过搜昵称（模糊搜索）
    KCT_ADDSCENE_LOGO_WALL = 36, //通过点击logo墙
    KCT_ADDSCENE_TIMELINE_BIZ = 37, // 在朋友圈中，点击公众账号名的方式查看关注了公众账号。
    KCT_ADDSCENE_PROMOTE_MSG = 38, //在会话中，点击公众账号下发消息所带的公众账号名查看关注了公众账号。
    KCT_ADDSCENE_SEARCH_BRAND = 39, //通过“查找微信公众号”入口关注的公众号。
	KCT_ADDSCENE_BIZ_CONFERENCE = 40, //添加开启了好友验证的会议公众号
	KCT_ADDSCENE_PROMOTE_BIZCARD = 41, //表示“公众平台下发的名片”而加到的好友。
	KCT_ADDSCENE_WEB_PROFILE_URL = 42, 
	KCT_ADDSCENE_WEB_OP_MENU = 43, 
	KCT_ADDSCENE_LBSROOM = 44,
	KCT_ADDSCENE_GOOGLE_PLUS = 45, // 通过 google+ 推荐
};

enum enFriendType
{
	KCT_FRIENDTYPE = 0,
	KCT_FRIENDTYPE_BLOG = 1,
	KCT_FRIENDTYPE_EMAIL = 2,
	KCT_FRIENDTYPE_IM = 3,
	KCT_FRIENDTYPE_SMS = 4,
	KCT_FRIENDTYPE_FACEBOOK = 5,
};

enum enChatStatus
{
	KCT_CHAT_TYPING = 1,
	KCT_CHAT_CANCELTYPING = 2,
	KCT_VOICE_TYPING = 3,
	KCT_VOICE_CANCELTYPING = 4,
};

enum enKCTStatus
{
	KCT_STATUS_OPEN=0x1,
	KCT_STATUS_EMAILVERIFY=0x2,
	KCT_STATUS_MOBILEVERIFY=0x4,
	KCT_STATUS_SEARCH_CLOSE=0x08,
	KCT_STATUS_PROMOTE_CLOSE=0x10,
	KCT_STATUS_VERIFY_USER =0x20,
	KCT_STATUS_QMSG=0x40,
	KCT_STATUS_KCTID_PROMOTE_TOME_CLOSE=0x80,
	KCT_STATUS_PROMOTE_TOME_CLOSE=0x100,
	KCT_STATUS_MOBILE_SEARCH_CLOSE=0x200,
	KCT_STATUS_ADD_CONTACT_CLOSE=0x400,
	KCT_STATUS_APNS_TIPS_CLOSE= 0x800,
	KCT_STATUS_FLOATBOTTLE_OPEN= 0x1000,
	KCT_STATUS_ONLINE_OPEN = 0x2000,
	KCT_STATUS_MEDIANOTE_OPEN = 0x4000,
	KCT_STATUS_BOTTLE_CHART_OPEN = 0x8000,
	KCT_STATUS_BOTTLE_CHART_STARTED = 0x10000,
	KCT_STATUS_UPLOADMCONTACT_CLOSE = 0x20000,
	// [0x1, 0x10000] 标志位放在 mmaccount ，再增加新的标志位，请加到 mmuserattr
};


// status can't not be change by moduserinfo
enum enKCTProfileFlag
{
	KCT_NEED_SET_PASSWD = 0x1,

};

enum enPluginSwitch
{
	KCT_STATUS_GMAIL_OPEN = 0x1, // gmail开关
	KCT_STATUS_TXWEIBO_OPEN = 0x2, // 微博url开关
	KCT_STATUS_RECFBFRIEND_OPEN = 0x4, // facebook好友推荐开关
	KCT_STATUS_READAPP_PUSH_OPEN = 0x8, // readerapp 推送开关 
	KCT_STATUS_READAPP_TXNEWS_OPEN = 0x10, // readerapp 腾讯新闻推送开关 
	KCT_STATUS_READAPP_WB_OPEN = 0x20, // 微博精选推送开关 
	KCT_STATUS_TXWEIBO_ICON_OPEN = 0x40, // 是否显示腾讯微博图标开关
	KCT_STATUS_MASSSEND_SHOW_IN_CHATLIST_OPEN = 0x80, // 群发主界面显示开关
	KCT_STATUS_MEISHI_CARD_OPEN = 0x100, // 美食特惠插件开关
	KCT_STATUS_USERNAME_SEARCH_CLOSE = 0x200, // search contact 中微信号开关 
	KCT_STATUS_NEWSAPP_TXNEWS_CLOSE = 0x400, // newsapp 腾讯新闻推送开关, 注意这个位置上后表示关闭 
	KCT_STATUS_PRIVATEMSG_RECV_CLOSE = 0x800, // 私信推送开关，注意这个位置上后表示关闭
	KCT_STATUS_PUSHMAIL_NOTIFY_CLOSE = 0x1000, // pushmail 推送开关， 注意这个位置上后表示关闭
	KCT_STATUS_WEBONLINE_PUSH_OPEN = 0x2000,  // webwx在线，是否打开PUSH
    KCT_STATUS_SAFEDEVICE_OPEN = 0x4000, // for safedevice function switch
};

enum enGrayscaleFlag
{
	KCT_GRAYSCALE_CDNIMG = 0x1, // 支持从cdn传图
};

enum enGrayscaleStatus
{
	KCT_GRAYSCALE_OPEN = 0x1,
	KCT_GRAYSCALE_CLOSE = 0x0,
};

enum enWebImPluginSwitch
{
	KCT_STATUS_TONE_OPEN = 0x1,
    KCT_STATUS_TABLE_WARN_OPEN = 0x2,
};

enum enPluginBitStatus
{
	KCT_PLUGIN_BIT_STATUS_SWITCH = 0x1,
	KCT_PLUGIN_BIT_STATUS_FLAG = 0x2,

	KCT_PLUGIN_BIT_STATUS_NONE = 0x80,
};

enum enPluginMsgID
{
	KCT_PLUGIN_MSG_ID_NEWS = 1000,
	KCT_PLUGIN_MSG_ID_PUSHSYSMSG = 1001,
    KCT_PLUGIN_MSG_ID_MASSSEND = 1002,
};

enum enKCTSyncMsg
{
	KCT_MSG_STATUS=0x01,
	KCT_MSG_CONTENT=0x02,
};

enum enKCTImgStatus
{
	KCT_IMGSTATUS_NOIMG=1,
	KCT_IMGSTATUS_HASIMG=2,
};

enum enKCTMsgStatus
{
	KCT_MSGSTATUS_SENDING=1,
	KCT_MSGSTATUS_SENT=2,
	KCT_MSGSTATUS_DELIVERED=3,
	KCT_MSGSTATUS_READ=4,
	KCT_MSGSTATUS_SENDFAIL=5,
};

enum enKCTScene
{
	KCT_SCENE_SYNCALL=1,
	KCT_SCENE_SYNCMSG=2,
};

enum enKCTDataType
{
	KCT_DATA_TEXT=1, // 文本类型
	//KCT_DATA_HTML=2, // 未使用,已废弃
	KCT_DATA_IMG=3, // 图片类型
	KCT_DATA_PRIVATEMSG_TEXT=11, // 私信文本
	//KCT_DATA_PRIVATEMSG_HTML=12, // 未使用,已废弃
	KCT_DATA_PRIVATEMSG_IMG=13, // 私信图片
	KCT_DATA_CHATROOMMSG_TEXT=21, // 被客户端误用，兼容 
	//KCT_DATA_CHATROOMMSG_HTML=22, // 未使用,已废弃
	KCT_DATA_CHATROOMMSG_IMG=23,  // 被客户端误用，兼容
	//KCT_DATA_EMAILMSG_TEXT=31,    // 未使用,已废弃
	//KCT_DATA_EMAILMSG_HTML=32,    // 未使用,已废弃
	//KCT_DATA_EMAILMSG_IMG=33,     // 未使用,已废弃
	KCT_DATA_VOICEMSG=34,  // 语音类型
	KCT_DATA_PUSHMAIL=35, // pushmail类型
	KCT_DATA_QMSG=36, // 离线消息文本
	KCT_DATA_VERIFYMSG=37, // 好友验证类型
	KCT_DATA_PUSHSYSTEMMSG=38, //广告消息类型
	KCT_DATA_LIXIANMSG_IMG=39, // 离线消息图片
	KCT_DATA_POSSIBLEFRIEND_MSG=40, // 好友推荐类型
	KCT_DATA_PUSHSOFTWARE=41, // 精品软件推荐类型
	KCT_DATA_SHARECARD=42, // 名片分享类型
	KCT_DATA_VIDEO=43, // 视频类型
	KCT_DATA_VIDEO_IPHONE_EXPORT=44, // 转发视频类型
	KCT_DATA_GMAIL_PUSHMAIL=45, // Gmail pushmail类型
	KCT_DATA_EMPTY=46,//客户端要求，占坑
	KCT_DATA_EMOJI=47, // 自定义表情类型
	KCT_DATA_LOCATION=48, // 位置消息类型
	KCT_DATA_APPMSG=49, // AppMsg
	KCT_DATA_VOIP_INVITE=50, //voipinvite
	KCT_DATA_STATUSNOTIFY=51, //web和手机状态同步消息
	KCT_DATA_WEIBOPUSH=52, // Weibo Push( MMReader )
	KCT_DATA_WEBIMVOIPNOTIFY =53, // webwx voip notify
    KCT_DATA_CHATROOMVERIFYMSG=54, //申请加入群验证消息
	KCT_DATA_BRAND_QA_ASK=55, //公众平台问答 提问
	KCT_DATA_TALK_SYSMSG=56, //对讲模式的系统消息
	KCT_DATA_BRAND_QA_MSG=57, //公众平台问答 普通消息
	KCT_DATA_OPEN_SUBSCRIBE=58, //开放平台 订阅消息
	KCT_DATA_OPEN_REPORT=59, //开放平台 报告消息
	KCT_DATA_OPEN_LINK=60, //开放平台 报告消息
	KCT_DATA_DEL_CONTACT = 61, // 删除好友消息
	KCT_DATA_VERIFY_CHATMEMBERMSG=62, // 群成员验证类型
	KCT_DATA_INVITE_CHATMEMBERMSG=63, // 邀请群成员类型
	KCT_DATA_LBSMATCH_SUCCESS=64, // 配对成功
	KCT_DATA_CHATROOMINFO_UPDATE=65, // 群资料修改成功
	KCT_DATA_CHATROOM_BEKICKOUT=66, // 被移出群
	KCT_DATA_CHATROOM_CLOSE=67, //群解散
	IM_DATA_PUSHSYSTEMMSG = 68, //IM广告类型
	KCT_DATA_SYSCMD_IPXX=9998, // 用于查询问题通知客户端上传日志
	KCT_DATA_SYSNOTICE=9999, // 系统通知类型
	KCT_DATA_SYS=10000, //系统消息，出现在会话中间
    KCT_DATA_SYSCMD_XML = 10001 , //系统命令XML消息，客户端只处理，不显示
    KCT_DATA_SYSCMD_NEWXML = 10002 , //系统命令XML消息，客户端只处理，不显示
};

enum enKCTStatusNotifyType
{
	KCT_STATUSNOTIFY_MARKMSGREAD = 1,
	KCT_STATUSNOTIFY_ENTERSESSION = 2,
	KCT_STATUSNOTIFY_CHATLISTREQ = 3,
	KCT_STATUSNOTIFY_CHATLISTRESP = 4,
	KCT_STATUSNOTIFY_QUITSESSION = 5,
	KCT_STATUSNOTIFY_CONFIRM = 6,
};

enum emMMEquipmentStatusNotifyType
{
    KCT_EQUIPMENT_STATUSNOTIFY_SET_EQUIPMENT_BEGIN = 1,
    KCT_EQUIPMENT_STATUSNOTIFY_SET_EQUIPMENT_SUCCESS = 2,
    KCT_EQUIPMENT_STATUSNOTIFY_SET_EQUIPMENT_FAIL = 3,
};

enum enKCTAppMsgInnerType
{
	KCT_APP_TEXT = 1,
	KCT_APP_IMG = 2,
	KCT_APP_MUSIC = 3,
	KCT_APP_VIDEO = 4,
	KCT_APP_URL = 5,
	KCT_APP_FILE = 6,
	KCT_APP_OPEN = 7,
	KCT_APP_EMOJI = 8,
	KCT_APP_VOICE_REMIND = 9,
};

enum enKCTAppMsgShowType
{
	KCT_APPMSG_SHOW_DEFAULT = 0,
	KCT_APPMSG_SHOW_READER = 1,
	KCT_APPMSG_SHAKETRANIMG_RESULT = 2, 
    KCT_APPMSG_VOICEREMIND_CONFIRM = 3,
    KCT_APPMSG_VOICEREMIND_REMIND = 4,
    KCT_APPMSG_VOICEREMIND_SYS = 5, // voice + text
	KCT_APPMSG_SHOW_SELF_PIC_TEXT = 6, // 自定义的图文消息
};

enum enKCTAppMsgItemShowType
{
	KCT_APPMSG_ITEM_SHOW_BIG_PIC_TEXT = 0,   // 大图展示的图文消息Item
	KCT_APPMSG_ITEM_SHOW_SMALL_PIC_TEXT = 1, // 小图展示的图文消息Item
	KCT_APPMSG_ITEM_SHOW_ALL_TEXT = 2,       // 全文本展示的消息Item, 仅仅显示摘要
	KCT_APPMSG_ITEM_SHOW_TABLE_TEXT = 3,     // 表格展示的消息Item,每个显示标题和摘要
};

enum enKCTAppMsgScene
{
	KCT_APP_FROMMOBILE = 0,
	KCT_APP_FROMWEB = 1,
};

enum enKCTBizDataType
{
	KCT_BIZ_DATA_TEXT=1,
	KCT_BIZ_DATA_IMG=2,
	KCT_BIZ_DATA_VOICE=3,
	KCT_BIZ_DATA_VIDEO=4,
	KCT_BIZ_DATA_APPMSG=10,
	KCT_BIZ_DATA_SHARECARD=42,
};

enum enKCTChatRoomNotifyStatus
{
	KCT_CHATROOMNOTIFY_OPEN = 0x1,
	KCT_CHATROOMNOTIFY_CLOSE = 0x0,
};

enum enKCTContactType
{
	KCT_CONTACT_IM=0,
	KCT_CONTACT_MICROBLOG=1,
	KCT_CONTACT_CHATROOM=2,
	KCT_CONTACT_EMAIL=3,
	KCT_CONTACT=4,
	KCT_CONTACT_BOTTLE=5,
	KCT_CONTACT_MASSSEND=6,
};

enum enKCTMicroBlogType
{
	KCT_MICROBLOG=1,
};

enum enKCTSexType
{
	KCT_SEX_UNKNOWN=0,
	KCT_SEX_MALE=1,
	KCT_SEX_FEMALE=2,
};

enum enKCTPersonalCardFlag
{
	KCT_PCFLAG_NONE = 0,
	KCT_PCFLAG_SET = 1,
};

enum enKCTDelFlagVal
{
	KCT_DELFLAG_EXIST=1,
	KCT_DELFLAG_DEL=2,
};

enum enKCTDomainEmailStatus
{
	KCT_DOMAINEMAIL_NOTVERIFY=1,
	KCT_DOMAINEMAIL_VERIFIED=2,
};

enum enKCTFunctionSwitchType
{
	KCT_FUNCTIONSWITCH_PUSHMAIL=1,
	KCT_FUNCTIONSWITCH_SEARCH_CLOSE=2,
	KCT_FUNCTIONSWITCH_PROMOTE_CLOSE=3,
	KCT_FUNCTIONSWITCH_VERIFY_USER=4,
	KCT_FUNCTIONSWITCH_MSG=5,
	KCT_FUNCTIONSWITCH_KCTID_PROMOTE_TOME_CLOSE=6,
	KCT_FUNCTIONSWITCH_PROMOTE_TOME_CLOSE=7,
	KCT_FUNCTIONSWITCH_MOBILE_SEARCH_CLOSE=8,
	KCT_FUNCTIONSWITCH_ADD_CONTACT_CLOSE=9,
	KCT_FUNCTIONSWITCH_APNS_TIPS_CLOSE= 10,
	KCT_FUNCTIONSWITCH_BOTTLE_OPEN=11,		
	KCT_FUNCTIONSWITCH_ONLINE_OPEN=12,		
	KCT_FUNCTIONSWITCH_MEDIANOTE_OPEN=13,		
	KCT_FUNCTIONSWITCH_BOTTLE_CHART_OPEN =14,
	KCT_FUNCTIONSWITCH_GMAIL_OPEN =15,
	KCT_FUNCTIONSWITCH_TXWEIBO_OPEN =16,
	KCT_FUNCTIONSWITCH_UPLOADMCONTACT_CLOSE =17,
	KCT_FUNCTIONSWITCH_RECFBFRIEND_OPEN =18,
	KCT_FUNCTIONSWITCH_READER_OPEN =19,
	KCT_FUNCTIONSWITCH_READER_TXNEWS_OPEN =20,
	KCT_FUNCTIONSWITCH_READER_WB_OPEN =21,
	KCT_FUNCTIONSWITCH_TXWEIBO_ICON_OPEN =22,
	KCT_FUNCTIONSWITCH_MEISHI_CARD_OPEN =23,
	KCT_FUNCTIONSWITCH_ALBUM_NOT_FOR_STRANGER =24,
	KCT_FUNCTIONSWITCH_USERNAME_SEARCH_CLOSE=25,
	KCT_FUNCTIONSWITCH_NEWSAPP_TXNEWS_CLOSE =26,
	KCT_FUNCTIONSWITCH_WEBONLINE_PUSH_OPEN = 27,
    KCT_FUNCTIONSWITCH_SAFEDEVICE_OPEN = 28, //safedevice
};

enum enKCTFunctionSwitchStatus
{
	KCT_FUNCTIONSWITCH_NOSUCHFUNCTION=0,
	KCT_FUNCTIONSWITCH_OPEN=1,
	KCT_FUNCTIONSWITCH_CLOSE=2,
};

enum enKCTWebImFunctionSwitchType
{
	KCT_WEBIMFUNCTIONSWITCH_TONE_OPEN = 1,
    KCT_WEBIMFUNCTIONSWITCH_TABLE_WARN_OPEN = 2,
};

enum enKCTebWxFunctionSwitchStatus
{
	KCT_WEBIMFUNCTIONSWITCH_NOSUCHFUNCTION=0,
	KCT_WEBIMFUNCTIONSWITCH_OPEN=1,
	KCT_WEBIMFUNCTIONSWITCH_CLOSE=2,
};

enum enKCTSyncCmd
{
	KCT_SYNCCMD_MODUINFO=1,
	KCT_SYNCCMD_MODCONTACT=2,
	KCT_SYNCCMD_MODCONTACTIMG=3,
	KCT_SYNCCMD_DELCONTACT=4,
	KCT_SYNCCMD_ADDMSG=5,
	KCT_SYNCCMD_MODMSGSTATUS=6,
	KCT_SYNCCMD_DELCHATCONTACT=7,
	KCT_SYNCCMD_DELCONTACTMSG=8,
	KCT_SYNCCMD_DELMSG=9,
	KCT_SYNCCMD_REPORT=10,
	KCT_SYNCCMD_OPENMICROBLOG=11, 
	KCT_SYNCCMD_CLOSEMICROBLOG=12,
	KCT_SYNCCMD_MODMICROBLOG=13,
	KCT_SYNCCMD_MODNOTIFYSTATUS=14,
	KCT_SYNCCMD_MODCHATROOMMEMBER=15,
	KCT_SYNCCMD_QUITCHATROOM=16,
	KCT_SYNCCMD_MODCONTACT_DOMAINEMAIL=17,
	KCT_SYNCCMD_MODUSER_DOMAINEMAIL=18,
	KCT_SYNCCMD_DELUSER_DOMAINEMAIL=19,
	KCT_SYNCCMD_MODCHATROOMNOTIFY=20,
	KCT_SYNCCMD_POSSIBLEFRIEND=21,
	KCT_SYNCCMD_INVITEFRIENDOPEN=22,
	KCT_SYNCCMD_FUNCTIONSWITCH=23,
	KCT_SYNCCMD_MODQCONTACT=24,
	KCT_SYNCCMD_MODTCONTACT=25,
	KCT_SYNCCMD_PSMSTAT=26,
	KCT_SYNCCMD_MODCHATROOMTOPIC=27,
	KCT_SYNCCMD_OFFICLAMSG=28,
	KCT_SYNCCMD_MEDIANOTE=29,
	KCT_SYNCCMD_UPDATESTAT=30,
	KCT_SYNCCMD_MODDISTURBSETTING=31,
	KCT_SYNCCMD_DELETEBOTTLE=32,
	KCT_SYNCCMD_MODBOTTLECONTACT=33,
	KCT_SYNCCMD_DELBOTTLECONTECT=34,
	KCT_SYNCCMD_MODUSERIMG=35,
	KCT_SYNCCMD_KVSTAT=36,
	KCT_SYNCCMD_THEME=37,
	KCT_SYNCCMD_GMAILSWITCH=38,
	KCT_SYNCCMD_PLUGININSTALL=39,
	KCT_ALBUM_SYNCCMD_PHOTOID = 40,
	KCT_ALBUM_SYNCCMD_COMMENTTIPS = 41,
	KCT_SYNCCMD_MODALBUMINFO = 42,
	KCT_SYNCCMD_MODTXNEWS_CATEGORY = 43,
	KCT_SYNCCMD_USERINFOEXT = 44,

	KCT_SNS_SYNCCMD_OBJECT = 45,
	KCT_SNS_SYNCCMD_ACTION = 46,

	KCT_SYNCCMD_BRAND_SETTING = 47,

    KCT_SYNCCMD_MODCHATROOMMEMBERDISPLAYNAME = 48,
    KCT_SYNCCMD_MODCHATROOMMEMBERFLAG = 49,

	KCT_SYNCCMD_WEBIMFUNCTIONSWITCH = 50,
	KCT_SYNCCMD_MODSNSUSERINFO = 51,
    KCT_SYNCCMD_MODSNSBLACKLIST = 52,
	KCT_SYNCCMD_NEWDELMSG=53,
	KCT_SYNCCMD_SYNCLBSMATCH=54,//附近的人配对列表
	KCT_SYNCCMD_DELLBSMATCH=55,//删除配对数据
	KCT_SYNCCMD_MODGROUPTOPIC=56,
	KCT_SYNCCMD_MODLBSMATCH=57,//修改配对的人数据
	
	// 出错时使用的虚拟cmd
	KCT_SYNCCMD_ERROR = 9999 
};

// 新闻分类ID
enum enKCTTXNewsCategory
{
	KCT_CATEGORY_NEWS = 0x1,  // 新闻
	KCT_CATEGORY_SPORTS = 0x2, // 体育
	KCT_CATEGORY_ENT = 0x4, // 娱乐
	KCT_CATEGORY_FINANCE = 0x8, //财经 
	KCT_CATEGORY_TECH = 0x10, // 科技
	KCT_CATEGORY_GAMES = 0x20, // 游戏
	KCT_CATEGORY_LADY = 0x40, // 女性
	KCT_CATEGORY_DIGI = 0x80, // 数码 
	KCT_CATEGORY_MIL = 0x100, // 军事
	KCT_CATEGORY_STOCK = 0x200, // 证券 
	KCT_CATEGORY_HOUSE = 0x400, // 房产
	KCT_CATEGORY_AUTO = 0x800, // 汽车
	KCT_CATEGORY_USSTOCK = 0x1000, //  美股
	KCT_CATEGORY_HKSTOCK = 0x2000, // 港股
};

enum enKCTNewSyncSelector
{
	KCT_NEWSYNC_PROFILE = 0x1,
	KCT_NEWSYNC_MSG = 0x2,
	KCT_NEWSYNC_IMCONTACT = 0x4,
	KCT_NEWSYNC_PMCONTACT = 0x8,
	KCT_NEWSYNC_CONTACT = 0x10,
	KCT_NEWSYNC_FRIEND = 0x20,
	KCT_NEWSYNC_BOTTLECONTACT = 0x40,
	KCT_NEWSYNC_ALBUMSYNCKEY = 0x80,
	KCT_NEWSYNC_SNSSYNCKEY = 0x100,
	KCT_NEWSYNC_VOIPSNYC = 0x200,
	KCT_NEWSYNC_VOIPINVITE = 0x400,
	KCT_NEWSYNC_SUBMSGSYNCKEY = 0x800,//客服信息同步
	KCT_NEWSYNC_LBSMATCH = 0x1000, // 附近的人配对成功
	KCT_NEWSYNC_CLOSE = 0x2000, // 通知客户端下线
	KCT_NEWSYNC_ERR_FORBID_LOGI = 0x4000, //通知客户端下线
};

enum enKCTNewSyncStaus
{
	KCT_PRESENCE_WEBIM = 0x1,
	KCT_NEWSYNC_STATUS_SHAKETRANIMG_BIND = 0x2,
	KCT_NEWSYNC_STATUS_SHAKETRANIMG_ACTIVE = 0x4,
	KCT_NEWSYNC_STATUS_SHAKEBOOKMARK_BIND = 0x8,
};


enum enKCTNewSyncKey
{
	KCT_NEWSYNC_KEY_PROFILE = 1,
	KCT_NEWSYNC_KEY_MSG = 2,
	KCT_NEWSYNC_KEY_WXCONTACT = 3,
	KCT_NEWSYNC_KEY_PMCONTACT = 4,
	KCT_NEWSYNC_KEY_QCONTACT = 5,
	//KCT_NEWSYNC_KEY_FRIEND = 6, // 不能用，听说有坑在这里
	KCT_NEWSYNC_KEY_BOTTLECONTACT = 7,
	KCT_NEWSYNC_KEY_ALBUMFEED = 8,
	KCT_NEWSYNC_KEY_SNSTIPS = 9,
	KCT_NEWSYNC_KEY_SUBMSG= 10,
	KCT_NEWSYNC_KEY_LBSMATCH = 11, //只做定义，不做配对同步

	KCT_NEWSYNC_KEY_PUBLIC_BEGIN = 1000,
	KCT_NEWSYNC_KEY_PLUGINKEY_NEWS = 1000,
};

enum enKCTNewSyncScene
{
	// notify 
	KCT_NEWSYNC_SCENE_NOTIFY = 1,
	// timer
	KCT_NEWSYNC_SCENE_TIMER = 2,
	// 进入前台
	KCT_NEWSYNC_SCENE_BG2FG = 3,
	// 进程启动
	KCT_NEWSYNC_SCENE_PROCESSSTART = 4, 
	// init之后第一次newsync
	KCT_NEWSYNC_SCENE_AFTERINIT = 5,
	// continueflag 不为0
	KCT_NEWSYNC_SCENE_CONTINUEFLAG = 6,
	// 其他操作
	KCT_NEWSYNC_SCENE_OTHER = 7,
	// syncwait触发的newsync操作
	KCT_NEWSYNC_SCENE_SYNCWAIT = 8,

    // 第一次syncwait触发的ewsync操作
    KCT_NEWSYNC_SCENE_FIRST_SYNCWAIT = 9,

	// reauth后调用
	KCT_NEWSYNC_SCENE_AFTER_REAUTH = 10,
};

enum enKCTUInfoBitFlag
{
	KCT_UINFOFLAG_USERNAME=0x1,
	KCT_UINFOFLAG_NICKNAME=0x2,
	KCT_UINFOFLAG_BINDUIN=0x4,
	KCT_UINFOFLAG_BINDEMAIL=0x8,
	KCT_UINFOFLAG_BINDMOBILE=0x10,
	KCT_UINFOFLAG_STATUS=0x20,
	KCT_UINFOFLAG_IMG=0x40,
	KCT_UINFOFLAG_PERSONALCARD=0x80,
	KCT_UINFOFLAG_BOTTLEIMG =0x100,
	KCT_UINFOFLAG_HDHEADIMGMD5SUM =0x200,
	KCT_UINFOFLAG_BOTTLEHDHEADIMGMD5SUM =0x400,
	KCT_UINFOFLAG_PLUGINFLAG=0x800,
	//add by yihuang
	KCT_UINFOFLAG_LBS=0x1000,
	KCT_UINFOFLAG_LANGUAGE=0x2000,
	//add by bingchen
	KCT_UINFOFLAG_BIRTHDAY=0x4000,

	KCT_UINFOFLAG_ALL=( KCT_UINFOFLAG_NICKNAME | KCT_UINFOFLAG_BINDUIN | KCT_UINFOFLAG_BINDEMAIL | 
			KCT_UINFOFLAG_BINDMOBILE | KCT_UINFOFLAG_STATUS | KCT_UINFOFLAG_IMG | KCT_UINFOFLAG_PERSONALCARD |
			KCT_UINFOFLAG_BOTTLEIMG |  KCT_UINFOFLAG_HDHEADIMGMD5SUM | KCT_UINFOFLAG_BOTTLEHDHEADIMGMD5SUM | 
			//add by yihuang
			KCT_UINFOFLAG_LBS |
			KCT_UINFOFLAG_LANGUAGE |
			//add by bingchen
			KCT_UINFOFLAG_BIRTHDAY |
			//
			KCT_UINFOFLAG_PLUGINFLAG  ),
};

enum enKCTContactFlag
{
	KCT_CONTACTFLAG_CONTACT=0x01,
	KCT_CONTACTFLAG_CHATCONTACT=0x02,
	KCT_CONTACTFLAG_CHATROOMCONTACT=0x04,
	KCT_CONTACTFLAG_BLACKLISTCONTACT=0x08,
	KCT_CONTACTFLAG_DOMAINCONTACT=0x10,
	KCT_CONTACTFLAG_HIDECONTACT=0x20,
	KCT_CONTACTFLAG_FAVOURCONTACT=0x40,
	KCT_CONTACTFLAG_3RDAPPCONTACT=0x80,
	KCT_CONTACTFLAG_SNSBLACKLISTCONTACT=0x100,
	KCT_CONTACTFLAG_MUTECONTACT=0x200,

	KCT_CONTACTFLAG_ALL=(KCT_CONTACTFLAG_CONTACT | KCT_CONTACTFLAG_CHATCONTACT | KCT_CONTACTFLAG_CHATROOMCONTACT |
			KCT_CONTACTFLAG_BLACKLISTCONTACT | KCT_CONTACTFLAG_DOMAINCONTACT | KCT_CONTACTFLAG_HIDECONTACT |
			KCT_CONTACTFLAG_FAVOURCONTACT | KCT_CONTACTFLAG_3RDAPPCONTACT | KCT_CONTACTFLAG_SNSBLACKLISTCONTACT |
            KCT_CONTACTFLAG_MUTECONTACT),
};

enum enKCTLbsMatchFlag
{
	KCT_LBSMATCHFLAG_CONTACT=0x01,
	KCT_LBSMATCHFLAG_BLACKLISTCONTACT=0x02,

	KCT_LBSMATCHFLAG_ALL=(KCT_LBSMATCHFLAG_CONTACT | KCT_LBSMATCHFLAG_BLACKLISTCONTACT ),
};

enum enKCTLbsMatchType
{
	KCT_LBSMATCHTYPE_MATCH = 1,	// 已经配对成功
	KCT_LBSMATCHTYPE_LIKE = 2,	// 有意思
	KCT_LBSMATCHTYPE_NONE= 3,
};

enum enKCTNotifyStatus
{
	KCT_NOTIFY_CLOSE=0,
	KCT_NOTIFY_OPEN=1,
};

enum enKCTContactImgFlag
{
	KCT_CONTACTIMGFLAG_NOTMODIFY= 1,
	KCT_CONTACTIMGFLAG_MODIFY = 2,
	KCT_CONTACTIMGFLAG_HAS_HEADIMG = 3,
	KCT_CONTACTIMGFLAG_HAS_NO_HEADIMG = 4,
};

enum enKCTBottleContactImgFlag
{
	KCT_BOTTLECONTACTFLAG_HAS_NO_HDIMG = 0,
	KCT_BOTTLECONTACTFLAG_HAS_HDIMG = 1,
};

enum enKCTContinueFlag
{
	KCT_SYNC_END=0,
	KCT_SYNC_CONTINUE=1,
};

enum enKCTInitContinueFlag
{
	KCT_INIT_END=0,
	KCT_INIT_CONTINUE=1,
};

enum enKCTCookieId
{
	KCTC_AUTH_KEY = 0,
	KCTC_BAD_USER = 1, 
	KCTC_BIND_VERIFYINFO = 2,
	KCTC_BIND_CLIENTIP= 3,
	KCTC_AUTH_TIME = 4,
	KCTC_PLUGIN_SHAKETRANIMG_SID = 5,
	KCTC_PLUGIN_SHAKETRANIMG_ACTIVETIME = 6,
	KCTC_PLUGIN_SHAKETRANING_BOOKMARK_SID = 7,
	KCTC_DYE_EXPIRE_TIME = 8, /**< 染色截止时间*/
	KCTC_DYE_LOG_LEVEL = 9,   /**< 染色日志级别 */
	KCTC_USER_IMEI = 10,
	KCTC_BLOCK_OPER = 11, /**block用户某功能**/
	KCTC_CLIENTCHECK_ENABLE = 12, // clientcheck switch
	KCTC_CLIENTCHECK_SUPPORT = 13, // clientcheck support
	KCTC_ACCOUNT_TYPE = 14,
};

enum enBlockOper
{
	KCTBLOCK_TXT_SINGLE = 0x1,     //文本
	KCTBLOCK_TXT_CHATROOM = 0x2,
	KCTBLOCK_IMG_SINGLE = 0x4,     //图片
	KCTBLOCK_IMG_CHATROOM = 0x8,
	KCTBLOCK_VOICE_SINGLE = 0x10,  //语音
	KCTBLOCK_VOICE_CHATROOM = 0x20, 
	KCTBLOCK_VIDEO_SINGLE = 0x40,  //视频
	KCTBLOCK_VIDEO_CHATROOM = 0x80, 
	KCTBLOCK_APPMSG_SINGLE = 0x100, //appmsg
	KCTBLOCK_APPMSG_CHATROOM = 0x200,
	KCTBLOCK_EMOJI_SINGLE = 0x400, //emoji
	KCTBLOCK_EMOJI_CHATROOM = 0x800,
	KCTBLOCK_SNS_POST = 0x1000, //发图
	KCTBLOCK_SNS_COMMENT = 0x2000, //评论
	KCTBLOCK_MASSSEND = 0x4000, //群发
};

enum enKCTVoiceEndFlag
{
	//还在上传状态
	KCT_VOICE_UPLOADING = 0,
	//语音上传结束
	KCT_VOICE_UPLOADEND = 1
};

enum enKCTVoiceCancelFlag
{
	//还在上传状态
	KCT_VOICE_UPLOAD_CONTINUE = 0,
	//语音上传取消
	KCT_VOICE_UPLOAD_CANCEL = 1
};

enum enKCTVoiceForwardFlag
{
	//自己发的
	KCT_VOICE_UPLOAD_ORIGINAL = 0,
	//转发别人的
	KCT_VOICE_UPLOAD_FORWARD = 1
};

//remove by adrianli
//enum enKCTVoiceRemindEndFlag
//{
//	KCT_VOICEREMIND_UPLOAD_NULL = 0,
//	KCT_VOICEREMIND_UPLOAD_ING = 1,
//	KCT_VOICEREMIND_UPLOAD_END = 2,
//    KCT_VOICEREMIND_UPLOAD_CANCEL = 3,
//};

enum enKCTVoiceRecognizeEndFlag
{
	KCT_VOICE_REC_UPLOADING = 0,
	KCT_VOICE_REC_UPLOADEND = 1,
	KCT_VOICE_REC_UPLOADCANCEL = 2,
};

enum enKCTVoiceStatus
{
	KCT_VOICE_STATUS_UNKNOWN = 0,
	KCT_VOICE_STATUS_PIECE= 1,
	KCT_VOICE_STATUS_FULL = 2,
};

enum enSendCardType
{
	KCT_SENDCARD_BLOG= 0x01, 
	KCT_SENDCARD_SIGN= 0x02, 
	KCT_SENDCARD_QZONE= 0x04, 
	KCT_SENDCARDFRIEND= 0x08, 
	KCT_SENDCARD_SINABLOG= 0x10, 
	KCT_SENDCARD_MODIFYHEADIMG= 0x20, 
	KCT_SENDCARD_READER = 0x40, 
	KCT_SENDCARD_FACEBOOK = 0x80,
};

enum enBindOpMobileOpCode
{
	KCT_BINDMOBILE_READY = 1,
	KCT_BINDMOBILE_CHECK = 2,
	KCT_UNBINDMOBILE = 3,
	KCT_BINDMOBILE_CHECKED = 4,
	KCT_BINDMOBILE_REG_READY = 5,
	KCT_BINDMOBILE_REG_CHECK = 6,
	KCT_BINDMOBILE_REG_CHECKED = 7,
	KCT_BINDMOBILE_LOGIN_READY = 8,
	KCT_BINDMOBILE_LOGIN_CHECK = 9,
	KCT_BINDMOBILE_VERIFY_READY = 10,
	KCT_BINDMOBILE_VERIFY_CHECK = 11,
};



enum enImportFriendsOpCode
{
	KCT_IMPORT_AUTOADD = 1,
	KCT_IMPORT_RECOMMEND = 2,
};

enum enImportFriendsSource
{
	KCT_IMPORT_FACEBOOK_FRIENDS = 1,
};

enum enUpdateMContactOpCode
{
	KCT_UPDATEMCONTACT_ADD = 1,
	KCT_UPDATEMCONTACT_DEL= 2,
};

enum enVerifyUserOpcode
{
	KCT_VERIFYUSER_ADDCONTACT = 1,
	KCT_VERIFYUSER_SENDREQUEST = 2,
	KCT_VERIFYUSER_VERIFYOK = 3,
	KCT_VERIFYUSER_VERIFYREJECT = 4,
	KCT_VERIFYUSER_SENDERREPLY = 5,
	KCT_VERIFYUSER_RECVERREPLY = 6,
};

enum enVerifyChatRoomMemberOpcode
{
	KCT_VERIFYMEMBER_SENDREQUEST = 1,
	KCT_VERIFYMEMBER_VERIFYOK = 2,
	KCT_VERIFYMEMBER_VERIFYREJECT = 3,
};

enum enInviteChatRoomMemberOpcode
{
	KCT_INVITEMEMBER_SENDREQUEST = 1,
	KCT_INVITEMEMBER_INVITEOK = 2,
	KCT_INVITEMEMBER_INVITEREJECT = 3,
};


enum enBuiltinIPType
{
	KCT_BUILTINIPTYPE_WIFI =  1 << 0,
	KCT_BUILTINIPTYPE_EDGE =  1 << 1,
	KCT_BUILTINIPTYPE_YIDONG = 1 << 2,
	KCT_BUILTINIPTYPE_LIANTONG = 1 << 3,
	KCT_BUILTINIPTYPE_DIANXIN = 1<< 4
};

enum enGroupOpType
{
	KCT_OP_GROUP = 0,
	KCT_OP_FRIEND = 1,
	KCT_OP_ALL_GROUP = 2,
	KCT_OP_ALL_FRIEND = 3,
};

enum enFriendStatus
{
	KCT_STATUS_OFF = 0,
	KCT_STATUS_ON = 1,
	KCT_STATUS_WEXIN_FRIEND= 2,
};

enum enBindOPCode
{
	KCT_BQOC_BIND = 1,
	KCT_BQOC_GETVERIFYIMG = 2,
	KCT_BQOC_GENKEY = 3,
};

enum enShakeTranImgRet
{
	KCT_SHAKE_TRAN_IMG_OK = 0,
	KCT_SHAKE_TRAN_IMG_ERR_SYS = -1,
	KCT_SHAKE_TRAN_IMG_ERR_ARG = -2,
	KCT_SHAKE_TRAN_IMG_ERR_TICKET = -200,
	KCT_SHAKE_TRAN_IMG_ERR_TOKEN = -201,
	KCT_SHAKE_TRAN_IMG_ERR_SID = -202,

};

enum enShakeTranImgNotifyBit
{
	KCT_SHAKE_TRAN_IMG_NOTIFY_SHAKE = 0x1,
	KCT_SHAKE_TRAN_IMG_NOTIFY_BIND = 0x2,
	KCT_SHAKE_TRAN_IMG_NOTIFY_ACTIVE = 0x4,

};

enum enShakeTranImgOpType
{
	KCT_SHAKE_TRAN_IMG_UNBIND = 1,
	KCT_SHAKE_TRAN_IMG_INACTIVE = 2,

};

enum enShakeTranImgScene
{
	KCT_SHAKE_TRAN_IMG_SCENE_NORMAL = 0,
	KCT_SHAKE_TRAN_IMG_SCENE_SHARE = 1,
	KCT_SHAKE_TRAN_IMG_SCENE_BOOKMARK = 2,

};

enum enShareCardOpCode
{
	KCT_SHARE_CARD_ADD = 1,
	KCT_SHARE_CARD_CHECK = 2,
};

enum enShareCardRetCode
{
	KCT_SHARE_CARD_OK = 0,
	KCT_SHARE_CARD_LOGIC_ERR = 1,
};

enum enSnsTagOptionOpCode
{
	KCT_SNS_TAG_ADD = 1,
	KCT_SNS_TAG_MOD = 2,
	KCT_SNS_TAG_DEL = 3,
};

enum enSnsTagMemOptionOpCode
{
	KCT_SNS_TAG_MEM_ADD = 1,
	KCT_SNS_TAG_MEM_DEL = 2,
	KCT_SNS_TAG_MEM_MOD = 3,
};

enum enSnsTagListOpCode
{
	KCT_SNS_TAG_LIST_GET = 1,
	KCT_SNS_TAG_LIST_RECOMMEND = 2,
};

enum enSnsTagDefaultId
{
	KCT_SNS_TAG_ID_FRIEND = 0X1,//朋友
	KCT_SNS_TAG_ID_COLLEAGUE = 0x2,//同事
	KCT_SNS_TAG_ID_CLASSMATE = 0x3,//同学
	KCT_SNS_TAG_ID_OUTSIDERS = 0x4,//圈外人
	KCT_SNS_TAG_ID_BLACKLIST = 0x5,//黑名单
	KCT_SNS_TAG_ID_OTHERS = 0x6,//自定义 >= 0x6
};

enum enResetPawType
{
	KCT_RESETPWD_MOBILE = 0,
	KCT_RESETPWD_EMAIL = 1,
};

enum enResetErr
{
	KCT_RESETPWD_SYSERR = -1,
	KCT_RESETPWD_OK = 0,
	KCT_RESETPWD_USERNAMEINVALID = 1,
	KCT_RESETPWD_TIMEOUT = 2,
	KCT_RESETPWD_HASRESETED = 3,
	KCT_RESETPWD_KEYNOTFOUND = 4,
	KCT_RESETPWD_TYPEERR = 5,
	KCT_RESETPWD_PWDERR = 6,
	KCT_RESETPWD_FREQERR = 7,
	KCT_RESETPWD_MOBILEINVALID = 8,
	KCT_RESETPWD_PWDINVALID = 9
};

enum enLbsOpCode
{
	KCT_LBS_FIND = 1,
	KCT_LBS_REMOVE = 2,
	KCT_LBS_FINDGG = 3,
	KCT_LBS_FINDMM = 4,
};

enum enLbsMatchOpCode
{
	KCT_LBSMatch_Match = 1,
	KCT_LBSMatch_REMOVE = 2,
};


enum enLbsState
{
	KCT_LBS_ONLINE = 0x1,
};

enum enDirectSendType
{
	KCT_ALBUMLBS_PUSHTYPE = 0x4,
	KCT_SNSLBS_PUSHTYPE = 0x5,
	KCT_ChatStatus = 111, // 发送或取消聊天状态
};

enum enLbsGPSSource
{
	KCT_SOURCE_HARDWARE = 0,
	KCT_SOURCE_NET = 1,
};
enum enVideoNetworkEnv
{
	KCT_VIDEO_NETWORK_WIFI = 1,	
	KCT_VIDEO_NETWORK_OTHER = 2,
};
enum enVideoCameraType
{
	KCT_VIDEO_CAMERA_FRONT = 1,
	KCT_VIDEO_CAMERA_REAR = 2,
};
enum enVideoFuncFlag
{
	KCT_VIDEO_FUNCFLAG_SELF = 1,
	KCT_VIDEO_FUNCFLAG_EXPORT = 2,
};

enum enUpdateOpCode
{
	KCT_UPDATE_ALERT_YES = 0x1,
	KCT_UPDATE_ALERT_NO = 0x2,
	KCT_UPDATE_NEW_CLICK = 0x4,
	KCT_UPDATE_UPDATE_CLICK = 0x8,
	KCT_UPDATE_CHECK_UPDATE = 0x10,
};

enum enGroupCardOpCode
{
	KCT_GROUPCARD_ADD = 1,
	KCT_GROUPCARD_UPDATENICK = 2,
};

enum enBottleType
{
	KCT_BOTTLE_TYPE_NORMAL = 0
};

enum enBottleErr
{
	KCT_BOTTLE_ERR_UNKNOWNTYPE = 15,
	KCT_BOTTLE_COUNT_ERR = 16,
	KCT_BOTTLE_NOTEXIT = 17,
	KCT_BOTTLE_UINNOTMATCH = 18,
	KCT_BOTTLE_PICKCOUNTINVALID = 19
};

//bottle type
enum enBottleDataType
{
	KCT_BOTTLE_TEXT = 1,
	KCT_BOTTLE_IMG = 2,
	KCT_BOTTLE_VOICE = 3,
	KCT_BOTTLE_VIDEO = 4,
};

// to weibo msg type
enum enToWeiboType
{
	KCT_TOWEIBO_TYPE_TEXT = 1,
	KCT_TOWEIBO_TYPE_IMG = 2,
	KCT_TOWEIBO_TYPE_VOICE = 3,
	KCT_TOWEIBO_TYPE_VIDEO = 4,
};

// hd head img type
enum enHDHeadImgType
{
	KCT_HD_HEADIMG_IM = 1,
	KCT_HD_HEADIMG_BOTTLE = 2,
	KCT_HD_HEADIMG_SUBUSER = 3,
};

enum enKVStatKey
{   
	//收取耗时
	KCT_GETDATA_TIMECOST = 1, 
	//wifi下 无连接的次数 
	KCT_DISCONNECT_WIFI = 2, 
	//非wifi下 无连接的次数 
	KCT_DISCONNECT_NOTWIFI = 3,
	KCT_IPXX = 4,
	//3¡è¨¢??¨®¡ä¨®?¦Ì ¡À3¡é¨º?¡À??¡¥?a1 ??¨®¨°?¡ä?3¡é¨º? ¦Ì?¡ä?¨ºy
	KCT_LINKCHANGE_COUNT = 5, 
	//
	KCTSYNC_OPER = 6,
	//
	KCT_ANDROID_REQ_TIMECOST = 7,
	//
	KCT_IPHONE_CRASH_INFO = 8,
	//背景使用 
	KCT_BACKGROUND_USED = 9,
	// 点击好友推荐卡片的场景次数统计 
	KCT_CLICK_FRIEND_SCENE = 10,
	// 塞班请求耗时
	KCT_SYMBIAN_REQ_TIMECOST = 11,

	KCT_SEND_PIC_2_QZONE = 12,

	KCT_CRASHREPORT_ONCE = 13,

	KCT_CRASHREPORT_MORE = 14,
	//分享地理位置通过google查看详细位置
	KCT_SHARELOCATION_DETAIL_GOOGLE= 15,
	//分享地理位置通过其他查看详细位置
	KCT_SHARELOCATION_DETAIL_OTHER= 16,
	//新年彩蛋统计
	KCT_NEWYEAR_EGG_SEND = 17, 

	KCT_NEWYEAR_EGG_RECV = 18, 

	//微信是否切到前台
	KCT_SHOW = 19,

	KCT_SYMBIAN_REQ_ERR = 20,

	KCT_DEVICE_PLATFORM = 21,

	// 用户主动反馈（目前只有 sybiam 用到）
	KCT_USER_FEEDBACK = 22,

	KCT_MEDIA_TRANSFER = 23, /**< 图片视频支持转发 （转发次数、人数)*/
	KCT_SHOW_PWD_PLAIN_TXT = 24, /**< 输入密码时可以选择显示明文 （选择显示明文的人数、次数 */
	KCT_ADJUST_FONT_SIZE = 25, /**< 在设置中添加调整字体大小的选项 （每个字体号的选择人数）*/
	KCT_MOBILE_ADD_FRIEND = 26, /**< 好友列表、手机通讯录好友列表里快捷添加好友按钮 （点击此按钮的人数) */
	KCT_APPMSG_SWITCH = 27,    /* 由进入第3方APP发送消息 */
	// sns
	KCT_SNS_CHECK_BIGPIC = 28, //sns查看大图
	KCT_SNS_CHECK_SELFPAGE = 29, //sns查看自己相册
	KCT_MSG_CLICK = 30, //发起离线，每次点击离线消息按钮发送一个

	//facebook
	KCT_FACEBOOK_CONNECT = 31, //点击facebook Connect的用户数据
	KCT_FACEBOOK_LINK = 32, //点击facebook link的用户数据
	KCT_FACEBOOK_INVITE = 33, //点击facebook邀请的次数

	// symbian专用
	KCT_APP_START = 34, // 程序启动上报
	KCT_APP_AIDREPORT = 35,	// 用户上报应急操作

	KCT_APP_UPDATE_STATE = 36, // 程序升级状态（成功/失败）
	KCT_APP_EMERGENCY_TRIMER = 37, // 程序应急启动

	//larger than 10000, user new attrlog for report
	KCT_GETLBSIMGFROMCDN = 10073,
	KCT_GETSNSIMGFROMCDN = 10074,
	KCT_GETSNSHDIMGFROMCDN = 10075,
	KCT_MAILCLICKCOUNT = 10071,
	KCT_EMBEDMUSICPLAYCOUNT = 10090,
	KCT_LANGUAGE_DOWNLOAD_TIMECOST = 10095,
	KCT_MAINFRAMEQRCODE = 10111,
	KCT_MAINFRAMEWEBIM = 10110, 
	KCT_MAINFRAMEEAR =	10109, 
	KCT_MAINFRAMECREATECHAT = 10108,
	KCT_COPYPHONENUMBER = 10115, 
	KCT_ADDEXISTCONTACTPHONENUMBER = 10114,
	KCT_ADDNEWCONTACTPHONENUMBER = 10113, 
	KCT_CALLPHONENUMBERINMESSAGE = 10112,  
	KCT_APPINSTALL = 10165,
	KCT_VOIPTENMINUTESLOGICREPORT = 10166,
	KCT_CHATCREATECHATROOMCLICK = 10170,
	KCT_CHATROOMADDCONTACTCLICK = 10169,
	KCT_CONTACTLISTCREATECHATCLICK = 10168, 

	KCT_RECEIVERINCITATIONCOLOREGG = 10182,
	KCT_SENDERINCITATIONCOLOREGG = 10181,
};  

enum enAppMsgSouce
{
	KCT_APPMSG_IM = 1,
	KCT_APPMSG_3RD = 2,
	KCT_APPMSG_FORWARD = 3,
};

enum enShakeRet
{
	KCT_SHAKE_DELETE = 2,
	KCT_SHAKE_WAIT = 1,
	KCT_SHAKE_OK = 0,
	KCT_SHAKE_ERR = -1,
	KCT_SHAKE_TIME_OUT = -2,
};

enum enShakeOpCode
{
	KCT_SHAKE_MATCH = 0,
	KCT_SHAKE_MATCH_DELETE = 1,
};

enum enPluginInitFlag
{
	//邮箱
	KCT_PlUGIN_MAIL= 0x1,
	//私信
	KCT_PLUGIN_PM = 0x2,
	//好友推荐助手
	KCT_PLUGIN_FM= 0x4,
	//微博发图助手
	KCT_PLUGIN_WEIBO= 0x8,
	//语音记事本
	KCT_PLUGIN_MEDIANOTE= 0x10,
	//离线消息
	KCT_PLUGIN_QMSG= 0x20,
	//漂流瓶
	KCT_PLUGIN_BOTTLE= 0x40,
	//同步助手
	KCT_PLUGIN_QSYNC= 0x80,
	//撸一撸
	KCT_PLUGIN_SHAKE= 0x100,
	//lbs
	KCT_PLUGIN_LBS= 0x200,
	//
	KCT_PLUGIN_BOTTLE_CHART= 0x400,
	// Gmail
	KCT_PLUGIN_GMAIL= 0x800,
	// 查看好友
	KCT_PLUGIN_CHECKF= 0x1000,
	// FaceBook卸载标志
	KCT_PLUGIN_FACEBOOK = 0x2000,
	// 每日新闻插件
	KCT_PLUGIN_READERAPP = 0x4000,
	// 个人相册
	KCT_PLUGIN_FEEDSAPP = 0x8000,
	//群发
	KCT_PlUGIN_MASSSEND = 0x10000,
	// 美食
	KCT_PlUGIN_MEISHI = 0x20000,
	// 微博精选 
	KCT_PlUGIN_BLOGAPP = 0x40000,
	// newsapp插件 
	KCT_PLUGIN_NEWSAPP = 0x80000,
	//voip
	KCT_PLUGIN_VOIP = 0x100000,
    // voice input
    KCT_PLUGIN_VOICEINPUT = 0x200000,    
	//voip
	KCT_PLUGIN_VOICEVOIP = 0x400000,
};

enum enPluginFlag
{
	//邮箱卸载标志 
	KCT_MAIL_UNINSTALL = 0x1,
	//私信卸载标志
	KCT_PM_UNINSTALL = 0x2,
	//好友推荐助手卸载标志
	KCT_FM_UNINSTALL = 0x4,
	//微博发图助手卸载标志
	KCT_WEIBO_UNINSTALL = 0x8,
	//ÓïÒô¼ÇÊÂ±¾Ð¶ÔØêÖ?
	KCT_MEDIANOTE_UNINSTALL = 0x10,
	//离线消息卸载标志
	KCT_QMSG_UNINSTALL = 0x20,
	//漂流瓶卸载标志
	KCT_BOTTLE_UNINSTALL = 0x40,
	//同步助手卸载标志
	KCT_QSYNC_UNISTALL = 0x80,
	//撸一撸卸载标志
	KCT_SHAKE_UNISTALL = 0x100,
	//lbs卸载标志 
	KCT_LBS_UNISTALL = 0x200,
	//
	KCT_BOTTLE_CHART_INSTALL = 0x400,
	// Gmail卸载标志
	KCT_GMAIL_UNINSTALL = 0x800,
	// 查看好友卸载标志
	KCT_CHECKF_UNINSTALL= 0x1000,
	// FaceBook卸载标志
	KCT_FACEBOOK_UNINSTALL= 0x2000,
	// Reader卸载标志
	KCT_READERAPP_UNINSTALL= 0x4000,
	// 个人相册
	KCT_FEEDSAPP_UNINSTALL = 0x8000,
	// 群发
	KCT_MASSSEND_UNINSTALL = 0x10000,
	// 美食
	KCT_MEISHI_UNINSTALL = 0x20000,
	// 微博精选推选 
	KCT_BLOG_UNINSTALL = 0x40000,
	// newsapp插件
	KCT_NEWSAPP_UNINSTALL = 0x80000,
	// voip
	KCT_VOIP_UNINSTALL = 0x100000,
    // voice input
	KCT_VOICEINPUT_UNINSTALL = 0x200000,
	// voice for voip
	KCT_VOICEVOIP_UNINSTALL = 0x400000,

};

// 举报的场景定义
enum enExposeScene
{
	KCT_EXPOSE_LBS_SEARCH = 1,		// 主动LBS搜到后举报
	KCT_EXPOSE_LBS_RCVGREET = 2,		// 收到LBS的打招呼后举报
	KCT_EXPOSE_SHAKE_SEARCH = 3,		// 主动摇一摇后举报
	KCT_EXPOSE_SHAKE_RCVGREET = 4,	// 收到摇一摇的打招呼后举报
	KCT_EXPOSE_BOTTLE_PICK = 5,		// 收到漂流瓶后举报
	KCT_EXPOSE_BOTTLE_RCVGREET = 6,	// 收到漂流瓶来的打招呼后举报
	KCT_EXPOSE_QRCODE_RCVGREET = 7,	// 收到二维码打招呼后举报

	//因为之前的客户端有bug，会上传8-30之间的脏数据，所以从31开始

	KCT_EXPOSE_ALBUM_PHOTO = 31,	//  举报个人空间的照片 
	KCT_EXPOSE_ALBUM_SIGN = 32,	// 举报个人空间的签名 

	KCT_EXPOSE_SNS = 33, //举报个人动态
};

// 认证用户的类型定义
enum enVUserCategory
{
	KCT_VUSER_BUSINESS = 0x1,  // 商家
	KCT_VUSER_CELEBRITY = 0x2, //名人
};

enum enPublishAssistant
{
	KCT_PUBLISH_WEIBO	= 0x1,
	KCT_PUBLISH_SHUOSHUO = 0x2,
};

enum enKCTVoipRetCode
{
	KCT_VOIP_RET_SESSION_USER_BUSY = 211,
	KCT_VOIP_RET_SESSION_TIME_OUT = 212,
	KCT_VOIP_RET_SESSION_ROOMID_CHECK_FAIL = 213,
	KCT_VOIP_RET_SESSION_NOT_EXIST = 214,
	KCT_VOIP_RET_ROOMINDEX_MEMBER_NOT_IN = 221,
	KCT_VOIP_RET_ROOMINDEX_ROOM_NOT_EXIST = 222,

	KCT_VOIP_RET_SVR_LOGIC_ERROR = 231,
	KCT_VOIP_RET_REDIS_ERROR = 232,
	KCT_VOIP_RET_CHECKCONTACT_ERROR = 233,

	KCT_VOIP_RET_ARG_ERR = -2,

	KCT_VOIP_RET_NET_NOTSUPPORT_ERR = 234,

	KCT_VOIP_RET_USER_NOTSUPPORTED = 235,

	KCT_VOIP_RET_USER_IN_BLACKLIST= 236,
	KCT_VOIP_RET_USER_PLUGINCLOSE = 237,
};

enum enKCTVoipType
{
	KCT_VOIP_INVITE_VIDEO = 0,
	KCT_VOIP_INVITE_VOICE = 1,
};

enum enKCTVoipInviteCallType
{
	KCT_VOIP_INVITE_CALL_TYPE_INVITE = 0,
	KCT_VOIP_INVITE_CALL_TYPE_REINVITE = 1,
};

enum enKCTVoipAnswerType
{
	KCT_VOIP_ANSWER_ACCEPT = 1,
	KCT_VOIP_ANSWER_REJECT = 2,
};

enum enKCTVoipInviteType
{
	KCT_VOIP_INVITE_INVITE = 1,
	KCT_VOIP_INVITE_CANCELINVITE = 2,
	KCT_VIOP_INVITE_REMIDE = 3,
	KCT_VIOP_INVITE_SHUT_DOWM = 4,
};


enum enKCTVoipSyncUserStatus
{
	KCT_VOIP_SYNC_STATUS_STATUS_NEEDSYNC = 0,
	KCT_VOIP_SYNC_STATUS_ACCEPT = 1,
	KCT_VOIP_SYNC_STATUS_REJECT = 2,
	KCT_VOIP_SYNC_STATUS_NORESP = 3,
	KCT_VOIP_SYNC_STATUS_SHUTDOWN = 4,
	KCT_VOIP_SYNC_STATUS_ROOMCREATER= 5,
	KCT_VOIP_SYNC_STATUS_ACKED = 6,
};

enum enKCTVoipSyncCmd
{
	KCT_VOIP_SYNCCMD_STATUS = 1,
	KCT_VOIP_SYNCCMD_RELAYDATA = 2,
	KCT_VOIP_SYNCKEY_CONNECTING_STATUS = 3,
};

enum enKCTVoipSelector
{
	KCT_VOIP_SYNC_SELECTOR_STATUS = 0x1,
	KCT_VOIP_SYNC_SELECTOR_RELAYDATA = 0x2,
	KCT_VOIP_SYNC_SELECTOR_CONNECTING_STATUS = 0x4,
};

enum enKCTVoipPushType
{
	KCT_VOIP_PUSHTYPE_INVITE = 1,
	KCT_VOIP_PUSHTYPE_CLIENT_DEFINE = 100,
};

enum enKCTVoipRelayDataType
{
	KCT_VOIP_RELAY_DATA_NORMAL = 1,
	KCT_VOIP_RELAY_DATA_PEERID = 2,
	KCT_VOIP_RELAY_DATA_CAPINFO = 3,
	KCT_VOIP_RELAY_SENDING_TYPE = 4,
	KCT_VOIP_RELAY_CONNECT_DATA = 5,
};

enum enKCTVoipSendingType
{
	KCT_VOIP_SENDINGTYPE_RELAY_ONLY = 1,
	KCT_VOIP_SENDINGTYPE_AUTO_SELECTION = 2,
	KCT_VOIP_SENDINGTYPE_DC_FIRST = 4,
	KCT_VOIP_SENDINGTYPE_DC_ONLY = 8,
	KCT_VOIP_SENDINGTYPE_RELAY_FIRST = 16

};

enum enKCTVoipSyncKeyType
{
	KCT_VOIP_SYNCKEY_STATUS = 1,
	KCT_VOIP_SYNCKEY_RELAYDATA = 2
};

enum enKCTVoipNetType
{
	KCT_VOIP_NETTYPE_NULL = 0,
	KCT_VOIP_NETTYPE_2G = 1,
	KCT_VOIP_NETTYPE_NOTWIFI = 2,
	KCT_VOIP_NETTYPE_3G = 3,
	KCT_VOIP_NETTYPE_WIFI = 4,
};

enum enKCTVoipDeviceInfo
{
	KCT_VOIP_NEED_SPAN = 0x1,
	KCT_VOIP_CPU_UNUSUAL = 0x2,
};

enum enKCTTalkMicActionType
{
	KCT_TALKMIC_ACTION_SEIZE = 1,
	KCT_TALKMIC_ACTION_PUTAWAY= 2,
};

enum enKCTTalkSysMsgType
{
	KCT_TALKSYSMSG_ENTERROOM = 1,
	KCT_TALKSYSMSG_EXITROOM = 2,
	KCT_TALKSYSMSG_CREATEROOM = 3,
	KCT_TALKSYSMSG_ADDMEMBER = 4,
	KCT_TALKSYSMSG_DELMEMBER = 5,
	KCT_TALKSYSMSG_INVITE = 6,
};

enum enKCTTalkRetCode
{
	KCT_TALK_RET_TALKINFO_USER_BUSY = 311,
	KCT_TALK_RET_TALKINFO_TIME_OUT = 312,
	KCT_TALK_RET_TALKINFO_ROOMID_CHECK_FAIL = 313,
	KCT_TALK_RET_TALKINFO_NOT_EXIST = 314,
	KCT_TALK_RET_ROOMINDEX_MEMBER_NOT_IN = 321,
	KCT_TALK_RET_ROOMINDEX_ROOM_NOT_EXIST = 322,

	KCT_TALK_RET_SVR_LOGIC_ERROR = 331,
	KCT_TALK_RET_REDIS_ERROR = 332,
	KCT_TALK_RET_CHECKCONTACT_ERROR = 333,

	KCT_TALK_RET_ARG_ERR = -2,

	KCT_TALK_RET_NET_NOTSUPPORT_ERR = 334,

	KCT_TALK_RET_USER_NOTSUPPORTED = 335,

	KCT_TALK_RET_USER_IN_BLACKLIST= 336,
	KCT_TALK_RET_USER_PLUGINCLOSE = 337,

	KCT_TALK_RET_TALKMIC_SEIZE_FAIL = 338,
	KCT_TALK_RET_TALKMIC_PUTAWAY_NONEED = 339,
};

enum enKCTTalkSyncUserStatus
{
	KCT_TALK_SYNC_STATUS_STATUS_NEEDSYNC = 0,
	KCT_TALK_SYNC_STATUS_ACCEPT = 1,
	KCT_TALK_SYNC_STATUS_REJECT = 2,
	KCT_TALK_SYNC_STATUS_NORESP = 3,
	KCT_TALK_SYNC_STATUS_SHUTDOWN = 4,
	KCT_TALK_SYNC_STATUS_ROOMCREATER= 5,
	KCT_TALK_SYNC_STATUS_ACKED = 6,
};

enum enKCTComposeSendType
{
	KCT_COMPOSE_SEND_TYPE_REPLY = 1,
	KCT_COMPOSE_SEND_TYPE_FORWARD = 2,
};

enum enKCTEmojiType
{
	KCT_EMOJI_TYPE_PNG = 1,
	KCT_EMOJI_TYPE_GIF = 2,
	KCT_EMOJI_TYPE_JPG = 3,
	KCT_APPEMOJI_TYPE_GIF = 4,
	KCT_APPEMOJI_TYPE_JPG = 5,
};

enum enKCTEmojiGameType
{
	KCT_EMOJI_NOTGAME = 0,
	KCT_EMOJI_JSB = 1,
	KCT_EMOJI_DICE = 2,

};

enum enKCTEmojiGameContent
{
	KCT_EMOJIGame_CONTENT_J = 1,
	KCT_EMOJIGame_CONTENT_S = 2,
	KCT_EMOJIGame_CONTENT_B = 3,
	KCT_EMOJIGame_CONTENT_1 = 4,
	KCT_EMOJIGame_CONTENT_2 = 5,
	KCT_EMOJIGame_CONTENT_3 = 6,
	KCT_EMOJIGame_CONTENT_4 = 7,
	KCT_EMOJIGame_CONTENT_5 = 8,
	KCT_EMOJIGame_CONTENT_6 = 9,
};

enum enMsgEmojiFlag
{
	KCT_MSG_EMOJI = 0x1,
	KCT_MSG_EMOJI_EMOJI = 0x2,
	KCT_MSG_EMOJI_ART = 0x4,
};

enum emMMPackageType
{
	KCT_PACKAGE_THEME = 0,
	KCT_PACKAGE_SESSION_BACKGROUND = 1,
	KCT_PACKAGE_EMOJIART = 2,
	KCT_PACKAGE_RESOURCE = 3,
	KCT_PACKAGE_ALBUM = 4,
	KCT_PACKAGE_STRINGS = 5,
	KCT_PACKAGE_EASTEREGG = 6,
	KCT_PACKAGE_CONFIGLIST = 7,
	KCT_PACKAGE_REGIONCODE = 8,
	KCT_PACKAGE_PATTERN = 9,
	KCT_PACKAGE_FESTIVAL = 10,
};

enum enKCTUserattrFlag
{
	KCT_USERATTR_WEIBO_SHOW	 = 0x1,
	KCT_USERATTR_WEIBO_VERIFY = 0x2,
	KCT_USERATTR_WEIBO_ICON = 0x4,
};

enum emMMFaceBookInfo
{
	KCT_FACEBOOK_UNBIND = 0,
	KCT_FACEBOOK_BIND = 1,
	KCT_FACEBOOK_UPDATE_TOKEN = 2,
};

enum emMMGetFriend
{
	KCT_MOBILE_FRIEND = 0,
	KCT_FACEBOOK_FRIEND = 1,
};

enum emMMModAlbumBitFlag
{
	KCT_ALBUMINFOFLAG_ALBUMSTYLE =0x1,
	KCT_ALBUMINFOFLAG_ALBUMFLAG =0x2,
	KCT_ALBUMINFOFLAG_ALBUMBGIMGID =0x4,
	KCT_ALBUMINFO_ALL = ( KCT_ALBUMINFOFLAG_ALBUMSTYLE | KCT_ALBUMINFOFLAG_ALBUMFLAG | KCT_ALBUMINFOFLAG_ALBUMBGIMGID ),
};

enum enKCTAlbumType
{
	KCT_ALBUM_PHOTO = 1, //照片
	KCT_ALBUM_SIGN = 2, //签名
	KCT_ALBUM_BGIMG_UPLOAD = 4, //背景图片
	KCT_ALBUM_BGIMG_INTL_UPLOAD = 5, //推送到别人timeline的背景图片
};

enum emMMUserAttrVerifyFlag
{
	KCT_USERATTRVERIFYFALG_BIZ = 0x1, // 小商家
	KCT_USERATTRVERIFYFALG_FAMOUS = 0x2,
	KCT_USERATTRVERIFYFALG_BIZ_BIG = 0x4, // 大商家
	KCT_USERATTRVERIFYFALG_BIZ_BRAND= 0x8, // 品牌商家
	KCT_USERATTRVERIFYFALG_VERIFY = 0x10, // 认证
};

enum enAlbumPhotoType
{
	KCT_ALBUM_PHOTO_SMALL = 1, //小图
	KCT_ALBUM_PHOTO_MIDDLE = 2, //中图
	KCT_ALBUM_PHOTO_BIG = 3, //大图
	KCT_ALBUM_BGIMG = 4, //背景图片
	KCT_ALBUM_BGIMG_INTL = 5, //推送到别人timeline的背景图片
};

enum enAlbumCommentType
{
	KCT_ALBUM_COMMENT_LIKE = 1, //标红心
	KCT_ALBUM_COMMENT_WORD = 2, //评论
	KCT_ALBUM_COMMENT_REPLY = 3, //回复评论
	KCT_ALBUM_COMMENT_WITH = 4, //提到的人
	KCT_ALBUM_COMMENT_LIKE_BGIMG = 5, //赞背景
};

enum enAlbumLikeFlag
{
	KCT_ALBUM_NOT_LIKE = 0, //未点赞
	KCT_ALBUM_HAVE_LIKE = 1, //已赞
};

enum enAlbumDelFlag
{
	KCT_ALBUM_NOT_DEL = 0, //未删除
	KCT_ALBUM_HAVE_DEL = 1, //已删除
};

//个人相册timeline拉取
enum enAlbumGetTimelineType
{
	KCT_ALBUM_GETTIMELIME_NEW = 1, //往下拉，获取最新
	KCT_ALBUM_GETTIMELINE_OLD = 2, //往上拉，获取历史
};

enum enKCTUserAttrAlbumFlag
{
	KCT_USERATTRALBUMFLAG_HASIMG = 0x1, //个人空间有照片
	KCT_USERATTRALBUMFLAG_PRIVACY_OPEN = 0x2, //废弃的
	KCT_USERATTRALBUMFLAG_FOR_STRANGER_NO = 0x4, //不允许陌生人看个人相册
	KCT_USERATTRALBUMFLAG_FOR_STRANGER_TENDAY = 0x8, //允许陌生人看最近10天的相片 
	KCT_USERATTRALBUMFLAG_FOR_STRANGER_ONEMONTH = 0x10, //允许陌生人看最近一个月的相片
	KCT_USERATTRALBUMFLAG_FOR_STRANGER_THREEMONTH = 0x20, //允许陌生人看最近三个月的相片
	KCT_USERATTRALBUMFLAG_FOR_STRANGER_ONEYEAR = 0x40, //允许陌生人看一年的相片
	KCT_USERATTRALBUMFLAG_FOR_STRANGER_ALL = 0x80, //允许陌生人看全部相片
	KCT_USERATTRALBUMFLAG_FOR_LBS = 0x100, //相册LBS打开
};

enum enKCTAlbumRetCode
{
	KCT_ALBUM_RET_HAVE_LIKE = 201, //以及赞过了
	KCT_ALBUM_RET_PHOTO_HAS_DELETE = 202, //照片已经删除了
	KCT_ALBUM_RET_LIKE_SELF = 203, //给自己赞
	KCT_ALBUM_RET_NOT_FOR_STRANGER = 204, //不给陌生人看
	KCT_ALBUM_RET_SPAM = 205, //中了反垃圾
	KCT_ALBUM_RET_BAN = 206, //被举报了
	KCT_ALBUM_RET_COMMENT_NO_PERMISSION = 207, //留言或者回复留言没有权限(一般是对方拉黑，或者把你删除好友了)
};

enum enGetAlbumPhotoListType
{
	KCT_GETALBUM_PHOTO = 1,
	KCT_GETALBUM_SIGN = 2,
	KCT_GETALBUM_PHOTO_AND_SIGN_MIX = 3,
	KCT_GETALBUM_PHOTO_AND_SIGN_NOT_MIX = 4,
	KCT_GET_BGIMGLIST = 5,
};

enum enMsgImgCompressType
{
	KCT_MSGIMG_WITH_COMPRESS = 0,
	KCT_MSGIMG_WITHOUT_COMPRESS = 1,
};

enum enSearchQRCodeType
{
	KCT_SEARCH_QRCODE_USER = 0,
	KCT_SEARCH_QRCODE_DIANSHANG = 1,
	KCT_SEARCH_QRCODE_URL = 2,
	KCT_SEARCH_QRCODE_UNKNOW = 3,
};

//push mail
enum
{
	PUSHMAIL_REPLY_INIT=0,
	PUSHMAIL_REPLY_OPENOK=1,
	PUSHMAIL_REPLY_CLOSESENDMSG=2,
	PUSHMAIL_REPLY_OPENSENDMSG=3,
	PUSHMAIL_REPLY_AUTOOPENOK=4,
	PUSHMAIL_REPLY_AUTOOPENFAILED=5,
	PUSHMAIL_REPLY_OTHER=6,
};

enum
{
	PUSHMAIL_STAT_MTYPE=21,
	PUSHMAIL_STAT_OPEN=1,
	PUSHMAIL_STAT_CLOSE=2,
	PUSHMAIL_STAT_RECV=3,
	PUSHMAIL_STAT_READ=4,
	PUSHMAIL_STAT_OPENAUTO=0,
	PUSHMAIL_STAT_OPENMANUAL=1,
};

enum enAlbumAttrFlag
{
	KCT_ABLUM_PHOTOATTR_ISPRIVATED = 0x1, // 是否私密照片
};

enum enAlbumPhotoPrivated
{
	KCT_ALBUM_PHOTO_NOT_PRIVATED = 0,
	KCT_ALBUM_PHOTO_PRIVATED = 1,
};

enum enAlbumSyncFlag
{
	KCT_ALBUM_SYNC2_TXWEIBO = 0x1, //同步到腾讯微薄
};

enum enAlbumOpLogCmd
{
	MOD_ALBUM_PHOTOATTR = 1, //修改相片附属信息
	ACCEPT_WITH_PHOTO = 2, //接受别人推过来的相片
};

//sns相关定义

//sns多媒体数据类型

//sns object排版类型

enum enKCTSnsObjectType
{
	KCTSNS_OBJECT_UNKNOWN = 0, //未知类型
	KCTSNS_OBJECT_PHOTO = 1, 
	KCTSNS_OBJECT_TEXT = 2, 
	KCTSNS_OBJECT_FEED = 3, 
	KCTSNS_OBJECT_MUSIC = 4,
	KCTSNS_OBJECT_VIDEO = 5,
	KCTSNS_OBJECT_LOCATION = 6,
	KCTSNS_OBJECT_BACKGROUND = 7,
	KCTSNS_OBJECT_WXSIGN = 8, //微信签名
};

//sns媒体类型
enum enKCTSnsMediaType
{
	KCTSNS_DATA_TEXT = 1, //文字
	KCTSNS_DATA_PHOTO = 2, //图片
	KCTSNS_DATA_VOICE = 3, //语音
	KCTSNS_DATA_VIDEO = 4, //视频
	KCTSNS_DATA_MUSIC = 5, //音乐
};

//一些个人控制标志位
enum enKCTSnsFlagDef
{
	KCTSNS_FLAG_HAVE_OBJECT = 0x1, //个人主页是否又内容
	//KCTSNS_FLAG_PRIVACY_CLOSE = 0x2, //隐私控制，不允许陌生人查看个人主页
	//KCTSNS_FLAG_PRIVACY_OPEN = 0x4, //隐私控制，允许陌生人查看个人主页所有内容
	//KCTSNS_FLAG_PRIVACY_TEN_OBJ = 0x8, //隐私控制，允许陌生人查看个人主页10个Object
    KCTSNS_FLAG_BLACKLIST_MIGRATE = 0x10, //是否迁移回避的人到新版黑名单
};

enum enKCTSnsFlagExDef
{
	KCTSNS_FLAG_PRIVACY_CLOSE = 0x1, //隐私控制，不允许陌生人查看个人主页
};

//每个object的一些标志位
enum enKCTSnsObjectExtFlag
{
	KCTSNS_OBJECT_EXTFLAG_BIDIRFRIEND = 0x1, //读取者这个object的发表者是否是双向好友关系
};

//评论类型
enum enKCTSnsCommentType
{
	KCTSNS_COMMENT_LIKE = 1, //留红心
	KCTSNS_COMMENT_TEXT = 2, //文字评论
	KCTSNS_COMMENT_MESSAGE = 3, //留言
	KCTSNS_COMMENT_WITH = 4, //占坑
	KCTSNS_COMMENT_STRANGER_LIKE = 5, //陌生人红心
	KCTSNS_COMMENT_FORWORD = 6, //占坑
};

//media url类型
enum enKCTSnsUrlType
{
	KCTSNS_URL_OTHER = 0, //其他url
	KCTSNS_URL_CDN = 1, //cdn
};

//通知类型
enum enKCTSnsMentionType
{
	//1, 有人给你标红心 使用KCTSNS_COMMENT_LIKE
	//2, 有人给你文字评论了 使用KCTSNS_COMMENT_TEXT
	//3, 有人给你留言了 使用KCTSNS_COMMENT_MESSAGE
	KCTSNS_MENTION_WITH = 4, //有人发照片提到你了
	// 5 use KCTSNS_COMMENT_STRANGER_LIKE//陌生人红心
	KCTSNS_MENTION_FORWORD = 6, //forword object
};

//object operation type
enum enKCTSnsObjectOpType
{
	KCTSNS_OBJECTOP_DEL = 1, //删除object或者media
	KCTSNS_OBJECTOP_SET_PRIVACY = 2, //设置object或者media为私密
	KCTSNS_OBJECTOP_SET_OPEN = 3, //设置object或者media为公开
	KCTSNS_OBJECTOP_DELETE_COMMENT = 4, //删除评论
	KCTSNS_OBJECTOP_CANCEL_LIKE = 5, //取消评论
};

//返回值
enum enKCTSnsRetCode
{
	KCTSNS_RET_SPAM = 201, //中了反垃圾
	KCTSNS_RET_BAN = 202, //被举报，被ban了
	KCTSNS_RET_PRIVACY = 203, //由于设置隐私，更多内容不给查看
	KCTSNS_RET_COMMENT_HAVE_LIKE = 204, //以及留过了红心，不能再留
	KCTSNS_RET_COMMENT_NOT_ALLOW = 205, //黑名单不允许评论
	KCTSNS_RET_CLIENTID_EXIST = 206, //clientid已经存在
	KCTSNS_RET_ISALL = 207, //已经是全部object了
	KCTSNS_RET_COMMENT_PRIVACY = 208, //私密照片不允许评论
    KCTSNS_RET_COMMENT_ID_ERROR = 209,
};

//同步到第三方的标志位
enum enKCTSnsSync3rdFlag
{
	KCTSNS_SYNC3RD_TXWEIBO = 0x1, //同步到腾讯微薄
	KCTSNS_SYNC3RD_FACEBOOK = 0x2, // sync to facebook
	KCTSNS_SYNC3RD_QZONE = 0x4, //同步到qzone
	KCTSNS_SYNC3RD_TWITTER = 0x8, //同步到twitter
};

enum enKCTSnsPrivacyFlag
{
	KCTSNS_OBJECT_OPEN = 0, //公开
	KCTSNS_OBJECT_PRIVACY = 1, //私密
	KCTSNS_OBJECT_FOR_STRANGER = 2, //只许陌生人可见
};



//just for oss
enum enKCTSnsCheckSelfPageScene
{
	KCTSNS_CHECKSELFPAGE_TIMELINE = 1, //从timeline进入查看homepage
	KCTSNS_CHECKSELFPAGE_SETTING = 2, //从设置页进入查看homepage
};

enum enSetBgImgType
{
	KCTSNS_SETBGIMG_SELFPIC = 1, //自己上传图片设置背景图
	KCTSNS_SETBGIMG_MASTERPIC = 2, //设置大师背景图 
};

enum enSnsUploadPhotoFrom
{
	KCTSNS_UPLOAD_PHOTO_FROM_TAKE = 1, //拍照
	KCTSNS_UPLOAD_PHOTO_FROM_EXIST = 2, //选择已有照片
};

enum enSnsSpecialTag
{
	KCTSNS_STAR_FANS_GROUP = 100000,
};

//sns --> end

enum enSnsPostSource
{
	KCT_SNSPOST_NORMAL_SHARE = 1, //常规分享
	KCT_SNSPOST_WEIBO_RECOMMAND = 2, //微薄推荐
	KCT_SNSPOST_NEWS = 3, //新闻阅读
	KCT_SNSPOST_BUSINESS_ADDR = 4, //企业联系人订阅
	KCT_SNSPOST_THIRDAPP = 5, //第三方app
	KCT_SNSPOST_SHAKEIMG = 6, //摇到的原图
	KCT_SNSPOST_SHAKEPAGE = 7, //摇到的网页
};

enum enUploadPhotoSource
{
	KCT_UPLOAD_PHOTO_CAMERA_FRONT = 1, //前置摄像头
	KCT_UPLOAD_PHOTO_CAMERA_BEHIND = 2, //后置摄像头
	KCT_UPLOAD_PHOTO_CHOOSE = 3, //选择
	KCT_UPLOAD_PHOTO_FORWARD = 4, //转发
	KCT_UPLOAD_PHOTO_COPY = 5, //复制
	KCT_UPLOAD_PHOTO_FACEBOOKALBUM = 6, 
};

enum enUploadPhotoNetType
{
	KCT_UPLOAD_PHOTO_NET_WIFI = 1, //wifi
	KCT_UPLOAD_PHOTO_NET_OTHER = 2, //其他
};

enum enKCTAppInfoIconType
{
	KCTAPPINFO_ICONTYPE_SD = 1,
	KCTAPPINFO_ICONTYPE_HD = 2,
	KCTAPPINFO_ICONTYPE_MDPI = 3,
	KCTAPPINFO_ICONTYPE_HDPI =4,
	KCTAPPINFO_ICONTYPE_LDPI = 5
};

//getA8key opcode
enum enKCTGetA8KeyOpCode
{
	KCTGETA8KEY_OPENAPI = 1,
	KCTGETA8KEY_REDIRECT = 2,
	KCTGETA8KEY_QZONE = 3,
};

//getA8key scene
enum enKCTGetA8KeyScene
{
	KCTGETA8KEY_SCENE_UNKNOW = 0,
	KCTGETA8KEY_SCENE_MSG = 1,
	KCTGETA8KEY_SCENE_TIMELINE = 2,
	KCTGETA8KEY_SCENE_PROFILE = 3,
	KCTGETA8KEY_SCENE_QRCODE = 4,
	KCTGETA8KEY_SCENE_QZONE = 5,
	KCTGETA8KEY_SCENE_OAUTH = 6,
    KCTGETA8KEY_SCENE_OPEN = 7,
    KCTGETA8KEY_SCENE_PLUGIN = 8,
};


//scan qrcode action
enum enKCTScanQrcodeActionCode
{
	KCTSCAN_QRCODE_A8KEY = 0,
	KCTSCAN_QRCODE_TEXT = 1, 
	KCTSCAN_QRCODE_WEBVIEW = 2,
	KCTSCAN_QRCODE_APP = 3,
	KCTSCAN_QRCODE_PROFILE = 4,
	KCTSCAN_QRCODE_PLUGIN = 5,
	KCTSCAN_QRCODE_SPECIAL_WEBVIEW = 6,
	KCTSCAN_QRCODE_WEBVIEW_NO_NOTICE = 7,
	KCTSCAN_QRCODE_VCARD = 8,
    KCTSCAN_QRCODE_JUMP = 9,
};

//OpCody for webwx logout action
enum enKCTLogoutWebIm
{
	KCTWEBIM_BANNER_LOGOUT = 1,
	KCTWEBIM_MOBILE_LOGOUT = 2,
};

//auth key all in one - key type
enum enKCTAuthKeyAllInOneType
{
	KCTAIO_SKEY = 1,
	KCTAIO_SKEY2 = 2,
	KCTAIO_A2 = 3,
	KCTAIO_A8 = 4,
	KCTAIO_PASSKEY = 5,
	KCTAIO_BIZPASSKEY = 6,
};

enum enKCTBizMsgStatus
{
	KCTBiz_MSGSTATUS_SENDING=1,
	KCTBiz_MSGSTATUS_SENT=2,
	KCTBiz_MSGSTATUS_DELIVERED=3,
	KCTBiz_MSGSTATUS_READ=4,
	KCTBiz_MSGSTATUS_SENDFAIL=5,
	KCTBiz_MSGSTATUS_CHECKFAIL=6,
    
    /*
     *  ÒÔÏÂÎªÀ©Õ¹×´Ì¬£¬mmbizindex Êý¾Ý´æ´¢Ê¹ÓÃ£¬
     *  Íâ½çÄÃµ½µÄÈÔÎ´ KCTBiz_MSGSTATUS_SENDING 1
     */
	KCTBiz_MSGSTATUS_SENDING_SAVE=101,// ±£´æÔÚ mmbizindex
	KCTBiz_MSGSTATUS_SENDING_ANTISPAM=102,//Ìá½»·´À¬»øÉóºË
	KCTBiz_MSGSTATUS_SENDING_ANTISPAm_CALLBACK=103,//·´À¬»ø»Øµ÷ mmbizbroker
	KCTBiz_MSGSTATUS_SENDING_PRE_TBORG=104,// Ìá½» tborg Ç°Ô¤´¦Àí
	KCTBiz_MSGSTATUS_SENDING_TBORG=105,// ÒÑÌá½» tborg
    KCTBiz_MSGSTATUS_SENDING_CALLBAK=106,// tborg »Øµ÷ÖÐ
};

// media type for UploadMedia
enum enKCTMeidaType
{
	KCT_MEDIA_TYPE_IMAGE = 1,
	KCT_MEDIA_TYPE_VIDEO = 2,
	KCT_MEDIA_TYPE_AUDIO = 3,
	KCT_MEDIA_TYPE_ATTACHMENT =4,
	KCT_MEDIA_TYPE_PATTERN_AUDIO = 5, // 旧的模式认别需要的audio类型.
	KCT_MEDIA_TYPE_PATTERN_AUDIO_SINGLE = 6, // 模式认别需要的audio类型.个人
    KCT_MEDIA_TYPE_PATTERN_AUDIO_CHATROOM =  7, //模式认别需要的audio类型.群聊
    KCT_MEDIA_TYPE_PATTERN_AUDIO_APP =  8, //模式认别需要的audio类型.公众号

};

enum enKCTBottleFlag
{
	KCTBOTTLE_FLAG_MATCH_AGE = 1,
	KCTBOTTLE_FLAG_MATCH_CITY = 2,
	KCTBOTTLE_FLAG_NOT_ZH = 4,
};

enum enKCTBottleFlag2
{
	KCTBOTTLE_FLAG2_ZH_CN = 1,
	KCTBOTTLE_FLAG2_ZH_TW = 2,
	KCTBOTTLE_FLAG2_EN = 4,
};

//OPEN API
enum enKCTOpenAppType
{
	KCTOPEN_CLIENTAPP = 0x01,
	KCTOPEN_MESSAGEAPP = 0x02,
	KCTOPEN_TIMELINEAPP = 0x04,
	KCTOPEN_LOGINAPP = 0x08,
	KCTOPEN_BIZAPP = 0x10,
	KCTOPEN_KFAPP = 0x20,
};

// statreport
enum enKCTStatReportFlag
{
	ReportBan = 0,  // 禁止上报
	ReportAll = 1, //全部上报
	ReportNetSenceAndNet = 2, // 只上报 网络事件 和 netsence

	ReportOnlyExt = 4, // 只上报 extinfo
};

enum enKCTStatReportActionID
{
	PushProcStart = 10001, //前台进程启动
	PushProcStop = 10002,

	WorkerProcStart = 10003, //后台进程启动
	WorkerProcStop = 10004,

	Connect = 10101,	// connnect 事件
	DisConnect = 10102,

	NetSenceSendImg = 10401, 
};

enum enKCTStatReportNetType
{
	KCT_STAT_WiFi = 1,
	KCT_STAT_CNWap = 2,
	KCT_STAT_CNNet = 3,
	KCT_STAT_Net3G = 4,
};

enum enKCTStatReportIPType
{
	enDNSFlag = 0x1,
	enLongConnectFlag = 0x10,
};

enum enKCTStatReportIfSuc
{
	enOK = 0,
};

enum enKCTBRAND_SUBSCRIPTION {
	KCTBRAND_SUBSCRIPTION_BLOCK_MESSAGE_NOTIFY = 0x1, //不接收新消息提醒
	KCTBRAND_SUBSCRIPTION_HIDE_FROM_PROFILE = 0x2,  // 不在我的profile中显示
};


//push声音ooplog
enum emMMVoiceSoundBitFlag
{
	KCT_VOICESOUND_SOUND =0x1,
	KCT_VOICESOUND_VOIPSOUND =0x2,
	KCT_VOICESOUND_ALL = ( KCT_VOICESOUND_VOIPSOUND | KCT_VOICESOUND_SOUND ),
};

enum enVerifyPswdType
{
       KCT_VERIFY_PSWD_WX = 1,
       KCT_VERIFY_PSWD = 2,
       KCT_VERIFY_PSWD_GET3GIMG = 3,
       KCT_VERIFY_PSWD_EMAIL = 4,
};

enum enBindType
{
       KCT_BINDTYPE = 1,
       KCT_BINDTYPE_MOBILE = 2,
       KCT_BINDTYPE_FACEBOOK = 3,
       KCT_BINDTYPE_EMAIL = 4,
};

// 设置密码的场景值
enum enQueryHasPasswdScene
{
	KCT_QUERYHASPASSWD_SET_SCENE = 1,		// 用户主动点设置密码场景
	KCT_QUERYHASPASSWD_LOGOUT_SCENE = 2,		//退出登陆场景
	KCT_QUERY_EMAIL_PWD_SCENE = 3 , //查询是否有email密码
};

enum enEmailOpCode
{
     KCT_EMAIL_BIND = 1,  //绑定邮箱
     KCT_EMAIL_UNBIND = 2,  //解绑邮箱
	 KCT_EMAIL_BIND_WITH_SET_MAIN = 4,
};

enum enKCTStatReportLogType
{
	KCT_CLIENTPERFORMANCE = 0, //客户端性能
	KCT_KVSTAT = 1, //kvstat业务统计
	KCT_STACKREPORT = 2,  //crash上报
	KCT_USERACTION = 3,  //点击流
	KCT_STATREPORT = 4, //客户端网络性能
};


enum enVoiceAddrSearchType
{
	KCT_SEARCH_CONTACT = 1,
	KCT_SEARCH_MSG = 2,
};

enum enBakChatDataType
{
	KCT_BAKCHAT_MSG_DATA = 1,//消息列表类型
	KCT_BAKCHAT_MEDIA_DATA = 2,//大媒体数据类型
};

enum enBakChatEncryptType
{
	KCT_BAKCHAT_ENCRYPT_NONE = 0,//不加密码
	KCT_BAKCHAT_ENCRYPT_AES  = 1,//aes加密 ，具体由客户端定义
};

//BakChat MediaType
enum enBakChatMediaType
{
	KCTBAK_NONE = 0,
	KCTBAK_THUMB_IMG = 1,
	KCTBAK_IMG = 2,
	KCTBAK_HD_IMG = 3,
	KCTBAK_EMOJI_THUMB = 4,
	KCTBAK_EMOJI = 5,
	KCTBAK_APPTHUMB = 6,
	KCTBAK_APPDATA = 7,
	KCTBAK_APPIMG = 8,
	KCTBAK_VOICE = 9,
	KCTBAK_VIDEO = 10,
	KCTBAK_VIDEO_THUMB = 11,
};


enum enOpVoiceRemindType
{
    KCT_VOICE_REMIND_CANCEL = 1, //取消语音
};

enum enVoiceFormat
{
    KCT_VOICE_FORMAT_UNKNOWN = -1, 
    KCT_VOICE_FORMAT_AMR = 0,   // AMR语音格式
    KCT_VOICE_FORMAT_SPEEX = 1, // SPEEX语音格式
    KCT_VOICE_FORMAT_MP3 = 2, 
    KCT_VOICE_FORMAT_WAVE = 3, 
};

enum enChatRoomMemberFlag
{
    KCT_CHATROOMFLAG_DISPALYNAME = 0x1, //显示群名片
};

enum enMainAcctType
{
	KCT_MAIN_ACCT_NONE = 0, //无可设置为主帐号的类型
	KCT_MAIN_ACCT = 1, //可设置为主帐号
	KCT_MAIN_ACCT_EMAIL = 2 ,//Email可设置为主帐号
};

enum enMainAcctState
{
	KCT_MAIN_ACCT_STATE_SETABLE = 0, //可以设置为主帐号
	KCT_MAIN_ACCT_STATE_UNSET = 1 , //未设置
	KCT_MAIN_ACCT_STATE_NEED_VERIFY = 2 ,//需要验证
	KCT_MAIN_ACCT_STATE_NNED_PWD = 4, //需要密码
};

enum enBindSafeMobileOpCodeType
{
	KCT_SAFE_MOBILE_OP_BIND = 1, //绑定安全手机
	KCT_SAFE_MOBILE_OP_UNBIND = 2, //解绑安全手机
};

/**
* Auth Ticket 验证的状态
*/
enum enAuthTicketStateType
{
	AUTH_TICKET_STATE_NONE = 0x0, //没有任何验证
	AUTH_TICKET_STATE = 0x1, //验证成功
	AUTH_TICKET_STATE_MOBILE  =  0x2, //验证手机成功
	AUTH_TICKET_STATE_WX  = 0x4, //验证WX密码成功
	AUTH_TICKET_STATE_FACEBOOK = 0x8, //验证Facebook成功
	AUTH_TICKET_STATE_EMAIL = 0x10, //验证Email成功
	AUTH_TICKET_STATE_SAFE_DEVICE = 0x3,  //验证设备锁成功
	AUTH_TICKET_STATE_ANY = 0x1f, //验证任意的一个密码成功
	AUTH_TICKET_STATE_TIMEOUT = 0xffff, //说明已经过期了，当获取ticket没有时，返回此ticket
};

enum enSafeDeviceStateType
{
	KCT_SAFE_DEVICE_STATE_UNSAFE = 0,
	KCT_SAFE_DEVICE_STATE_SAFE = 1,
	KCT_SAFE_DEVICE_STATE_ADD = 2,
};

enum enRegForceType
{
	KCT_REG_UNFORCE = 0,//非强制注册
	KCT_REG_FORCE = 1, //强制注册，不管这个号是否存在都创建新号
};


// 商家与普通用户的交互模式
enum enKCTBizInteractiveMode
{
    KCT_BIZ_INTERACTIVE_MODE_COMM = 0, // 普通消息模式
    KCT_BIZ_INTERACTIVE_MODE_QUESTION = 1, // 提问回答模式
    KCT_BIZ_INTERACTIVE_MODE_SELFMENU = 2, // 自定义菜单模式
};

// 商家自定义菜单中的菜单类型
enum enKCTBizSelfMenuType
{
    KCT_BIZ_SELFMENU_TYPE_SUBMENU = 0, // 弹出子菜单
    KCT_BIZ_SELFMENU_TYPE_CLICK = 1,   // 点击触发事件
    KCT_BIZ_SELFMENU_TYPE_VIEWURL = 2, // 查看url
    KCT_BIZ_SELFMENU_TYPE_MSGEXCHANGE = 3, // 消息交流
};

// 放在用户属性的ExternalInfo的json字段，用于表明该公众号是否能接收模式识别语音
#define RECEIVE_SPEEX_VOICE_FIELD "CanReceiveSpeexVoice" 

enum enCardMainType
{
    KCT_CARD_TYPE_ALL = 0,
    KCT_CARD_TYPE_CARD = 1,    //卡
    KCT_CARD_TYPE_COUPON = 2,  //券
    KCT_CARD_TYPE_TICKET = 3,  //票
};

enum enCardCardSubType
{
    KCT_CARD_CARD_SUBTYPE_ALL = 0,
    KCT_CARD_CARD_SUBTYPE_VIPCARD = 1,        //会员卡
    KCT_CARD_CARD_SUBTYPE_RECHARGECARD = 2,   //充值卡
};

enum enCardCouponSubType
{
    KCT_CARD_COUPON_ALL = 0,
    KCT_CARD_COUPON_SUBTYPE_VOUCHER = 1,      //代金券
    KCT_CARD_COUPON_SUBTYPE_COUPON = 2,       //优惠券
    KCT_CARD_COUPON_SUBTYPE_GIFTVOUCHER = 3,  //礼品券
};

enum enCardTicketSubType
{
    KCT_CARD_TICKET_SUBTYPE_ALL = 0,
    KCT_CARD_TICKET_SUBTYPE_TICKET = 1,  //普通票
};

enum enCardImageType
{
    KCT_CARD_IMAGE_TYPE_QRCODE = 1,
    KCT_CARD_IMAGE_TYPE_BARCODE = 2,
};

enum enCardImageSizeType
{
    KCT_CARD_IMAGE_SIZE_TYPE_NORMAL = 1,
    KCT_CARD_IMAGE_SIZE_TYPE_HD = 2,
};

#define KCT_USERATTR_EXTERNALINFO_BIZMENU "KCTBizMenu" // 用户属性ExternalInfo中json数据的商家菜单的key
#define KCT_USERATTR_EXTERNALINFO_INTERACTIVE_MODE "InteractiveMode" // 用户属性ExternalInfo中交互模式Name

// 消息协议msgSource中用于内容区分的头部串
#define KCT_MSGSOURCE_HEADERSTR_BIZMENU "#bizmenu#" //  针对商家自定义菜单的消息回复

// 商家按钮在空Key传入时自定设置Key时的特定头部串
#define KCTBIZ_NULL_BUTTONKEY_HEADER "nullbuttonkey_"

// click command 的类型定义
enum enClickCommandType
{
    KCT_CLICKCOMMAND_TYPE_BIZMENU_UNRECOGNIZED = 0, // 客户端不识别的菜单类型(都可以通过这个类型上报上来)
    KCT_CLICKCOMMAND_TYPE_BIZMENU_CLICKNOTIFY  = 1, //  商家菜单之点击通知类型
};


enum enUpdateSubUserAttrType
{
    KCT_UPDATE_SUBUSER_ATTR_NICKNAME = 0, //更新昵称
};

enum enSubUserType
{
    KCT_SUBUSER_TYPE_ALL = 0,
    KCT_SUBUSER_TYPE_CS = 1, //Custom Service
};

enum enModSnsBlackListType
{
    MODSNSBLACKLIST_ADD = 1,
    MODSNSBLACKLIST_DEL = 2,
};

enum enCDNImgEncryVer
{
	CNIMGEncry_AES_ECB_128 = 1,
};

enum enJSAPIPermissionBitSet
{
    KCT_JSAPI_PERMISSION_BITSET_LOG = 0x1,
    KCT_JSAPI_PERMISSION_BITSET_IMAGE_PREVIEW = 0x2,
    KCT_JSAPI_PERMISSION_BITSET_PROFILE = 0x4,
    KCT_JSAPI_PERMISSION_BITSET_SHARE_WEIBO = 0x8,
    KCT_JSAPI_PERMISSION_BITSET_SHARE_TIMELINE = 0x10,
    KCT_JSAPI_PERMISSION_BITSET_ADD_CONTACT = 0x20,
    KCT_JSAPI_PERMISSION_BITSET_SEND_APP_MSG = 0x40,
    KCT_JSAPI_PERMISSION_BITSET_SCAN_QRCODE = 0x80,
    KCT_JSAPI_PERMISSION_BITSET_ADD_EMOTICON = 0x100,
    KCT_JSAPI_PERMISSION_BITSET_HAS_EMOTICON = 0x200,
    KCT_JSAPI_PERMISSION_BITSET_CANCEL_ADD_EMOTION = 0x400,
    KCT_JSAPI_PERMISSION_BITSET_HASH_CHANGE = 0x800,
    KCT_JSAPI_PERMISSION_BITSET_HIDE_TOOLBAR = 0x1000,
    KCT_JSAPI_PERMISSION_BITSET_SHOW_TOOLBAR = 0x2000,
    KCT_JSAPI_PERMISSION_BITSET_HIDE_OPTION_MENU = 0x4000,
    KCT_JSAPI_PERMISSION_BITSET_SHOW_OPTION_MENU = 0x8000,
    KCT_JSAPI_PERMISSION_BITSET_GET_NEWWORK_TYPE = 0x10000,
    KCT_JSAPI_PERMISSION_BITSET_GET_CLOSE_WINDOW = 0x20000,

    KCT_JSAPI_PERMISSION_BITSET_FONT_MENU = 0x40000,
    KCT_JSAPI_PERMISSION_BITSET_PROFILE_MENU = 0x80000,
    KCT_JSAPI_PERMISSION_BITSET_ADD_CONTACT_MENU = 0x100000,
    KCT_JSAPI_PERMISSION_BITSET_SEND_APP_MSG_MENU = 0x200000,
    KCT_JSAPI_PERMISSION_BITSET_SHARE_WEIBO_MENU = 0x400000,
    KCT_JSAPI_PERMISSION_BITSET_SHARE_TIMELINE_MENU = 0x800000,
    
    KCT_JSAPI_PERMISSION_BITSET_DOWNLOAD_IN_WEBVIEW = 0x1000000,
    KCT_JSAPI_PERMISSION_BITSET_GET_INSTALL_STATE = 0x2000000,
};

enum enGeneralControlBitSet
{
    KCT_GENERAL_CONTROL_BITSET_SHOW_INPUT_ALERT_TIPS = 0x1,
};

enum enSnsControlFlagType
{
    SNS_CONTROLFLAG_CLOSE_SUMMARY = 0x1,
    SNS_CONTROLFLAG_CLOSE_SOURCE = 0x2,
};
enum enKCTClientCheckCmd
{
	KCT_CLIENTCHECK_CDM_CRC32 = 1    //crc32 
};

enum enLbsRoomOpCode
{
	KCT_JOIN_LBSROOM = 1,
	KCT_LEAVE_LBSROOM = 2,
};

enum enSafeAuthDeviceFlag
{
    KCT_SAFEAUTH_DEVICE_FLAG_NONE = 0, // none
};

enum enEquipmentType
{
    KCT_EQUIPMENT_TYPE_PHOTO_FRAME = 1,
};

enum enEquipmentFlag
{
    KCT_EQUIPMENT_FLAG_EQUIPMENT = 0x1,
};

enum enCDNImgType
{
	KCT_CDNIMG_BIG = 1,
	KCT_CDNIMG_MID = 2,
};

///////////////////////IM//////////////////////////////////////////////////

enum enIMStatus
{
	IM_STATUS_OPEN=0x1,
	IM_STATUS_EMAILVERIFY=0x2,				//邮箱已经通过验证
	IM_STATUS_MOBILEVERIFY=0x4,				//手机已经通过验证
	IM_STATUS_PUSH_OPEN = 0x8,				//用户打开推送开关
	IM_STATUS_USERNAME = 0x10,				//用户已经修改过用户名
	IM_STATUS_VERIFY_USER =0x20,				//打开好友验证的开关
	IM_STATUS_NICKNAME_LIMITED = 0x40,		//用户不能修改昵称
	IM_STATUS_BIRTHDAY_LIMITED = 0x80,		//用户不能修改生日
	IM_STATUS_PUSH_SECRECY = 0x800,			//推送消息隐私设置
	//...
};

enum enIMRegType
{
	IM_USERATTR_REG_UNKNOW = 0,
	IM_USERATTR_REG_GMAIL = 1,
	IM_USERATTR_REG_MOBILE = 2,
	IM_USERATTR_REG_EMAIL = 3,
	IM_USERATTR_REG_FACEBOOK = 5,
	IM_USERATTR_REG_OFFICIAL = 10,			//官方账号
};

enum enIMPushSetOpt
{
	IMPUSH_REG_TOKEN = 0,
	IMPUSH_UNREG_TOKEN = 1,
	IMPUSH_REG_REGID = 2,
	IMPUSH_UNREG_REGID = 3,
	IMPUSH_OPEN = 4,
	IMPUSH_CLOSE = 5,
};

///////////////////////////////////////////////////////////////////////////
enum enIMHeadImgType
{
	IM_HEADIMG_USER = 1,
	IM_HEADIMG_CHATROOM = 2,
};

///////////////////////IM//////////////////////////////////////////////////
enum enIMContactType
{	
	IM_CONTACT_IM = 0,				// MINGLE????	
	IM_CONTACT_FACEBOOK = 1,			//facebook
	IM_CONTACT_GMAIL = 2,				//gmail
	IM_CONTACT_MOBILE = 3,				//mobile
	IM_CONTACT_OFFCIAL = 4,			// 官号
};

enum enIMMobileOptCode
{
	IM_MOBILE_OPT_REG = 1,			//手机注册
	IM_MOBILE_OPT_AUTH = 2,			//手机验证码登陆(修改密码)
	IM_MOBILE_OPT_BIND = 3,			//手机绑定(更换)
	IM_MOBILE_OPT_UNBIND = 4,		//手机解除绑定
	IM_MOBILE_OPT_REGCHECK = 5,		//手机验证码注册校验
	IM_MOBILE_OPT_AUTHCHECK = 6,		//手机验证码登陆校验
	IM_MOBILE_OPT_BINDCHECK = 7,		//手机验证码绑定校验
	IM_MOBILE_OPT_UNBINDCHECK = 8,	//手机验证码解绑校验
};

enum enIMEmailOptCode
{
	IM_EMAIL_OPT_BIND = 1,			//邮箱绑定
	IM_EMAIL_OPT_UNBIND = 2,			//邮箱解除绑定
	IM_EMAIL_OPT_SENDVERIFY = 3,		//发送确认邮件
	IM_EMAIL_OPT_FINDPWD = 4,
};

enum enIMPlugFriendOptCode
{
	IM_PLUG_FRIEND_ADD = 1,				//联系人增加
	IM_PLUG_FRIEND_DEL = 2,				//联系人删除
};

enum enIMPlugFriendType
{
	IM_PLUG_FRIEND_MOBILE = 7,		//手机联系人
	IM_PLUG_FRIEND_EMAIL = 8,		// email好友
	IM_PLUG_FRIEND_FB = 9,			// facebook好友
	IM_PLUG_FRIEND_GMAIL = 10,		// gmail联系人
};

enum enUserStatus//bingchen 个人属性
{
	IM_USERSTATUS_DEACTIVE = 0,//没有设置个人基本信息
	IM_USERSTATUS_ACTIVE,//已经设置了个人基本信息
};

enum enChatRoomType
{
	IM_CHATROOMTYPE_MAKEFRIEND_MEETING = 0, // 交友聚会
	IM_CHATROOMTYPE_GAME,		 	// 游戏
	IM_CHATROOMTYPE_FOOD,			// 美食
	IM_CHATROOMTYPE_RECREATION,	// 娱乐、媒体
	IM_CHATROOMTYPE_HOBBY,			// 兴趣爱好
	IM_CHATROOMTYPE_SCHOOL,		// 学校、大学
	IM_CHATROOMTYPE_LIFE,			// 生活休闲
	IM_CHATROOMTYPE_PRODUCT,		//品牌、产品
	IM_CHATROOMTYPE_IT,			// 网络、科技
	IM_CHATROOMTYPE_SPORTS,		// 体育、健身
	IM_CHATROOMTYPE_LOVE,			// 爱情、约会
	IM_CHATROOMTYPE_NEWS,			// 新闻、政治
	IM_CHATROOMTYPE_OTHER,			// 其他
};

enum enChatRoomVerifyType
{
	IM_CHATROOMVERIFY_NOTNEED = 0,	// 不需要验证
	IM_CHATROOMVERIFY_NEED = 1,	// 需要验证
};

enum enFriendFlag
{
	IM_FRIENDFLAG_NONE = 0,	// 非好友
	IM_FRIENDFLAG_FRIEND = 1,  // 当前是好友状态
};

///////////////////////IM//////////////////////////////////////////////////

enum enKCTArtFlag
{
    KCT_ARTFLAG_READING = 0x1, //阅读
    KCT_ARTFLAG_WRITING = 0x2, //写作
    KCT_ARTFLAG_MUSIC = 0x4, //唱歌
    KCT_ARTFLAG_PLAYINGINSTRUMENT = 0x8, //演奏乐器
    KCT_ARTFLAG_PHOTOGRAPHY = 0x10,
    KCT_ARTFLAG_PAINTING = 0x20,
    KCT_ARTFLAG_POTTERY = 0x40,
};

enum enKCTSportsFlag
{
	KCT_SPORTSFLAG_SWIMMING = 0x1,
	KCT_SPORTSFLAG_JOGGING = 0x2,
	KCT_SPORTSFLAG_GYM = 0x4,
	KCT_SPORTSFLAG_TEAMSPORTS = 0x8,
	KCT_SPORTSFLAG_YOGA = 0x10,
	KCT_SPORTSFLAG_HIKING = 0x20,
	KCT_SPORTSFLAG_FISHING = 0x40,
	KCT_SPORTSFLAG_Racquet = 0x80,
};

enum enKCTSocialActivitiesFlag
{
	KCT_SOCIALACTIVITIESFLAG_BARS = 0x1,
	KCT_SOCIALACTIVITIESFLAG_DANCING = 0x2,
	KCT_SOCIALACTIVITIESFLAG_VOLENTEERING = 0x4,
	KCT_SOCIALACTIVITIESFLAG_MOVIES = 0x8,
	KCT_SOCIALACTIVITIESFLAG_KARAOKE = 0x10,
	KCT_SOCIALACTIVITIESFLAG_SHOPPING = 0x20,
	KCT_SOCIALACTIVITIESFLAG_COFFEE = 0x40,
};

enum enKCTTechnologyFlag
{
	KCT_TECHNOLOGYFLAG_CARS = 0x1,
	KCT_TECHNOLOGYFLAG_GAMES = 0x2,
	KCT_TECHNOLOGYFLAG_INTERNET = 0x4,
};

enum enKCTLifestyleFlag
{
	KCT_LIFESTYLEFLAG_COOKING = 0x1,
	KCT_LIFESTYLEFLAG_KNITTING = 0x2,
	KCT_LIFESTYLEFLAG_GARDENING = 0x4,
	KCT_LIFESTYLEFLAG_PETS = 0x8,
	KCT_LIFESTYLEFLAG_TRAVELLING = 0x10,
	KCT_LIFESTYLEFLAG_SLEEPING = 0x20,
	KCT_LIFESTYLEFLAG_FOOD = 0x40,
};

enum enKCTIntentionFlag
{
	KCT_INTENTION_FLAG_FRIENDSHIP = 0x1,
	KCT_INTENTION_FLAG_RELATIONSHIP = 0x2,
	KCT_INTENTION_FLAG_CASUALDATING = 0x4,
};

enum enLBSVisibleState
{
	KCT_LBSVISIBLESTATE_VISIBLE = 0,
	KCT_LBSVISIBLESTATE_INVISIBLE = 1,
};

enum enLBSDistanceUnit
{
	KCT_LBSDISTANCEUNIT_KILOMETER = 0,
	KCT_LBSDISTANCEUNIT_MILE = 1,
};

enum enIMProfileReportReason
{
	IM_PROFILEREPORT_PORN = 0,
	IM_PROFILEREPORT_SPAM = 1,
	IM_PROFILEREPORT_STOLEN = 2,
	IM_PROFILEREPORT_FAKE = 3,
	IM_PROFILEREPORT_OFFENSIVE = 4,
	IM_PROFILEREPORT_MAX = 5,
};

enum enIMGroupReportReason
{
	IM_GROUPREPORT_PORN = 0,
	IM_GROUPREPORT_SPAM = 1,
	IM_GROUPREPORT_MISLEAD = 2,
	IM_GROUPREPORT_FAKE = 3,
	IM_GROUPREPORT_OFFENSIVE = 4,
	IM_GROUPREPORT_MAX = 5,
};


#define KCT_DATA_SYSCMD_NEWXML_SUBTYPE_EQUIPMENTMSG "equipmentmsg"

//支持主帐号的版本
#define IS_SUPPORT_MAIN_ACCT_CLIENT "IsSupportMainAcctClient"

//支持设备锁的版本
#define IS_SUPPORT_SAFE_DEVICE_CLIENT "IsSupportSafeDeviceClient"

#define KF_USERNAME_PREFIX "mykf_" //客服号前缀

#define EQUIPMENT_USERNAME_PREFIX "eqid_"

// 多语言定义
// 简体中文
#define	KCT_LANG_CN "zh_CN"
//  繁体中文
#define KCT_LANG_TW "zh_TW"
// 英文
#define KCT_LANG_EN "en"
// 泰文
#define KCT_LANG_TH "th"

#define KCT_LANG_VI "vi"

#define KCT_LANG_ID "id"

#define KCT_LANG_PT "pt"

#define KCT_LANG_ES "es"

#define KCT_LANG_RU "ru"

#define KCT_LANG_AR "ar"

#define KCT_LANG_PL "pl"

#define KCT_LANG_HI "hi"

#define KCT_LANG_JA "ja"

#define KCT_LANG_IT "it"

#define KCT_LANG_KO "ko"

#define KCT_LANG_MS "ms"

#define KCT_LANG_DE "de_DE"



// 系统tips中带的content最大字数
#define MAX_TIPS_LENGTH 15

// 群组聊天的最大人数，白名单里的可以支持20个
#define MAX_MEMBER_COUNT_SP  20

// 群组聊天的最大人数
#define MAX_MEMBER_COUNT  10

//用户主动邀请好友时推荐的好友个数
#define INVITEFRIEND_COUNT	30

//msscache 配置文件路径
#define KCTSTORE_CLI	"/opt/im/etc/client/kctstore_cli.conf"

//uploadmsgimg时最大上传的图片buf大小
#define UPLOADMSG_IMG_LEN 64*1024

//getmsgimg时最大下发的图片buf大小
#define GETMSG_IMG_LEN 64*1024

//sync时最大下发的包大小
#define SYNC_LEN 64*1024

//getupdatepack时最大下发的包大小
#define GETUPDATEPACK_LEN 16*1024

// 缩略图规格
#define MAX_WIDTH 100
#define MAX_HEIGTH 100	

#define MAX_COMPRESS_WIDTH 640
#define MAX_COMPRESS_HEIGTH 960
#define MAX_COMPRESS_QUALITY 35

//密码算法
#define NO_ENCRYPT 						0
#define RSA_ENCRYPT_WITH_PUBLICKEY 		1
#define RSA_DECRYPT_WITH_PRIVATEKEY 	2
#define DES_ENCRYPT_WITH_PUBLICKEY		3
#define DES_DECRYPT_WITH_PRIVATEKEY		4
#define AES_DECRYPT_WITH_PRIVATEKEY     5
#define AES_DECRYPT_WITHOUT_ENCRYPT		6
#define RSA_DECRYPT_WITHOUT_ENCRYPT		7

#define RSA_PUBLIC_KEYE "010001"

#define RSA_PUBLIC_KEYN "DFE56EEE6506E5F9796B4F12C3A48121B84E548E9999D834E2C037E3CD276E9C4A2B1758C582A67F6D12895CE5525DDE51D0B92D32B8BE7B2C85827729C3571DCC14B581877BC634BCC7F9DA3825C97A25B341A64295098303C4B584EC579ECCA7C8B96782F65D650039EE7A0772C195DBEFC4488BDFB0B9A58C5C058E3AB04D"

//新加密算法
#define AES_RANDOM_ENCRYPT_KEY_LEN 16

#define AES_A8KEY_WRAPPER_PUBLICKEY "a@8^k!e%y"

// ServerId Encrypt key
#define SERVERID_ENCRYPT_KEY "abcdefgh"


#define KCT_AUTHBYFACEBOOK_USERNAME "facebook@wechat_auth"

#define IM_AUTHBYFACEBOOK_USERNAME "facebook@mingle_auth"
#define IM_AUTHBYGMAIL_USERNAME "gmail@mingle_auth"

#define BIZ_PASSKEY_ENCRYPT_KEY_PREFIX "b!i^z"

//验证码专区
#define KCT_VERIFYIMG_FORMAT "jpg"
#define KCT_VERIFY_SWITCH	0
#define KCT_EMAIL_REG		1	


// 统计专区
#define REG_MTYPE_REG		1
#define REG_MTYPE_UNREG		2
#define REG_MTYPE_VERIFY	3
#define REG_STYPE_MAIL	1
#define REG_STYPE_NOTMAIL	2
#define REG_STYPE		3
#define REG_STYPE_MOBILE 	4
#define REG_STYPE_FACEBOOK	5	

#define OP_MTYPE_SEARCH		1

#define OP_MTYPE_RECOMMEND	2
#define OP_RECOMMEND_STYPE_ISSUED	1
#define OP_RECOMMEND_STYPE_RECEIVE	2
#define OP_RECOMMEND_STYPE_OPEN		3
#define OP_RECOMMEND_STYPE_REPLAY	4

#define OP_MTYPE_CONTACT	3
#define OP_CONTACT_STYPE_ADD		1
#define OP_CONTACT_STYPE_DELETE		2

#define OP_MTYPE_CONTEXT	4
#define OP_CONTEXT_STYPE_SEND		1
#define OP_CONTEXT_STYPE_ISSUED		2
#define OP_CONTEXT_STYPE_READ	    3	
#define OP_CONTEXT_STYPE_DELETE		4

#define OP_MTYPE_IMG		5
#define OP_IMG_STYPE_SEND			1
#define OP_IMG_STYPE_ISSUED			2
#define OP_IMG_STYPE_READ			3
#define OP_IMG_STYPE_DELETE			4

#define OP_MTYPE_CGI_TRAFFIC                    31

/* 用户流量跟踪 */
#define OP_MTYPE_TRAFFIC_TAIL_CGI               1
#define OP_MTYPE_TRAFFIC_TAIL_GETMSGIMG         2
#define OP_MTYPE_TRAFFIC_TAIL_UPLOADMSGIMG      3
#define OP_MTYPE_TRAFFIC_TAIL_GETUPDATEPACK     4
#define OP_MTYPE_TRAFFIC_TAIL_DOWNLOADVOICE     5
#define OP_MTYPE_TRAFFIC_TAIL_DOWNLOADVOICE_END     6
#define OP_MTYPE_TRAFFIC_TAIL_UPLOADVOICE       7

#define OP_MTYPE_TRAFFIC_TAIL_SYNC_RECVMSG       8
#define OP_MTYPE_TRAFFIC_TAIL_SENDMSG       9


#define VOICE_XML_TEMP "<msg><voicemsg endflag=\"%d\" length=\"%u\" voicelength=\"%u\" clientmsgid=\"%s\" fromusername=\"%s\" downcount=\"%u\" cancelflag=\"%u\" forwardflag=\"%u\" /></msg>"

#define BIZ_VOICE_XML_TEMP "<msg><voicemsg endflag=\"%d\" length=\"%u\" voicelength=\"%u\" clientmsgid=\"%s\" fromusername=\"%s\" downcount=\"%u\" cancelflag=\"%u\" masterbufid=\"%llu\" slavebufid=\"%llu\" /></msg>"

#define VOICE_XML_TEMP_KCTid "<msg><voicemsg endflag=\"%d\" length=\"%u\" voicelength=\"%u\" clientmsgid=\"%s\" fromusername=\"%s\" downcount=\"%u\" nickname=\"%s\" cancelflag=\"%u\" /></msg>"

#define VOICE_DATA_HEAD "#!AMR\n"

#define NO_PUSH_SOUND "NO_SOUND"

#define BAKCHAT_UPLOAD_DATA_MAX_SIZE 64*1024
#define BAKCHAT_RECOVER_DATA_MAX_SIZE 64*1024

#define KCTIDC_SHANGHAI_NAME "shanghai"
#define KCTIDC_SHENZHEN_NAME "shenzhen"
#define KCTIDC_HK_NAME "hk"

#define KCT_AUTOAUTH_TICKET_LEN 8
#endif


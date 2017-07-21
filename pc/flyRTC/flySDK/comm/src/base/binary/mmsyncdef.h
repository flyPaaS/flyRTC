#ifndef  MMSYNC_DEFINE_H__
#define  MMSYNC_DEFINE_H__


enum
{
	MMPKG_BESTSIZE=65536,
};

/////////////////////////// 错误码定义 /////////////////////////////
enum
{
	// Every thing is ok
	MM_OK = 0,
	// 服务器错误
	MM_ERR_SYS = -1,
	// 参数错误
	MM_ERR_ARG = -2,
	// 密码错误
	MM_ERR_PASSWORD = 400,
	// 不存在该用户
	MM_ERR_NOUSER,
	// 权限不够
	MM_ERR_ACCESS_DENIED,
	// 需要输入验证码
	MM_ERR_NEED_VERIFY,
	// 用户名已经存在
	MM_ERR_USEREXIST,
	// 邮箱已经被绑定
	MM_ERR_EMAILEXIST,
	// 邮箱不存在
	IGG_ERR_EMAIL_NOTEXIST,
	// 发送贴图失败，详细错误信息参照list ret
	IGG_ERR_EMOJI_SEND,
	// 用户不存在高清头像
	MM_ERR_NO_HDHEADIMG,
	// 该手机已经被其他人绑定
	MM_ERR_MOBILE_BINDED,
	// 该手机不是你的绑定手机
	MM_ERR_MOBILE_UNBINDED,
	// 你已经绑定了这个手机
	MM_ERR_USER_BIND_MOBILE,
	// 验证码不存在
	MM_ERR_VERIFYCODE_NOTEXIST,
	// 验证码不正确
	MM_ERR_VERIFYCODE_UNMATCH,
	// 验证码已经过期
	MM_ERR_VERIFYCODE_TIMEOUT,
	// 手机注册的用户，不能解绑 
	MM_ERR_UNBIND_REGBYMOBILE,
	// 手机号码格式错误
	MM_ERR_MOBILE_FORMAT,
	// 该手机号码不存在
	IGG_ERR_MOBILE_NOT_EXIST,
	// 用户名已经修改过了，不允许再次修改
	IGG_ERR_USERNAME_HADMOD,
	// 验证票根不存在
	IGG_ERR_TICKET_NOTEXIST,
	// 验证票根不正确
	IGG_ERR_TICKET_NOTMATCH,
	// 发送文本消息失败，具体错误见list errcode
	IGG_ERR_SENDMSG,
	// 不能给自己发消息
	IGG_ERR_MSG_SEND_SELF,
	// 你在对方的黑名单中
	MM_ERR_BLACKLIST,
	// 需要验证
	MM_ERR_NEED_VERIFY_USER,
	// 你不是这个群的成员
	MM_ERR_NOTCHATROOMCONTACT,
	// 不在配对列表中
	IGG_ERR_NOTFIND_LBSMATCH,
	// 自己是隐身的，不能示好
	MM_ERR_LBSMATCH_INVISIBLE,
	// 达到每日示好次数上限
	MM_ERR_LBSMATCH_COUNT,
	// 已经示好过了
	IGG_ERR_HASLIKE,
	// 已经配对成功，不能再示好
	IGG_ERR_HASMATCH,
	// 昵称已改过，现在是只读
	IGG_ERR_READONLY,
	// 群组成员数达到上限
	MM_ERR_MEMBER_TOOMUCH,
	// 群聊踢人，不是群主返回
	MM_ERR_IS_NOT_OWNER,
};

// 邀请群成员加入时的返回值
enum enMMMemberStatus
{
	MM_MEMBER_OK = 0,
	// 用户不村在
	MM_MEMBER_NOUSER = 1,
	// 用户名无效
	MM_MEMBER_USERNAMEINVALID = 2,
	// 在对方黑名单中
	MM_MEMBER_BLACKLIST = 3,
	//==== 以下暂时没用到====
	MM_MEMBER_NEEDVERIFYUSER = 4,
	MM_MEMBER_UNSUPPORT_TALK = 5,
};

//===================以下是服务端备用的，客户端暂时不需要理会=====================
enum
{
	MM_ERR_EMAILNOTVERIFY = -9,

	// username reserved
	MM_ERR_USERRESERVED= -10,

	// nickname reserved
	MM_ERR_NICKRESERVED= -11,

	// Uin already exist
	MM_ERR_UINEXIST= -12,

	// sessionkey not exist, need auth 
	MM_ERR_SESSIONTIMEOUT= -13,

	// username invalid
	MM_ERR_USERNAMEINVALID= -14,

	// nickname invalid
	MM_ERR_NICKNAMEINVALID= -15,

	// client need critical update
	MM_ERR_CRITICALUPDATE = -16,

	// client need recommend update
	MM_ERR_RECOMMENDEDUPDATE = -17,

	// do not have update info
	MM_ERR_NOUPDATEINFO = -18,

	// do not open private msg
	MM_ERR_NOTOPENPRIVATEMSG = -19,

	// do not have this microblog contact
	MM_ERR_NOTMICROBLOGCONTACT = -20,

	MM_ERR_SPAM = -24,

	// DOMAIN EMAIL has beed verifyed
	MM_ERR_DOMAINVERIFIED=-25,

	// DOMAIN verified EMAIL reach limit
	MM_ERR_DOMAINMAXLIMITED=-26,

	// DOMAIN verified EMAIL reach limit
	MM_ERR_DOMAINDISABLE=-27,

	// email bad format
	MM_ERR_BADEMAIL= -28,

	// oidb timeout
	MM_ERR_OIDBTIMEOUT = -29,

	// can not auth, need reg
	MM_ERR_NEEDREG = -30,

	// can not auth, need reg
	MM_ERR_NEEDSECONDPWD= -31,

	// freq limited
	MM_ERR_FREQ_LIMITED = -34,

	// bind | unbind | bindchecked opmode error
	MM_ERR_INVALID_BIND_OPMODE = -37,

	// upload m contact opmode error
	MM_ERR_INVALID_UPLOADMCONTACT_OPMODE = -38,

	MM_ERR_MOBILE_NULL = -39,
	// bind check 's req 's mobile != bind read 's mobile
	MM_ERR_UNMATCH_MOBILE = -40,

	//uploadmcontact , req's mobile not user's bind mobile
	MM_ERR_USER_MOBILE_UNMATCH = -42,

	//batchgetcontactprofile err mode
	MM_ERR_BATCHGETCONTACTPROFILE_MODE = -45,

	//not exist in user's qq contact
	MM_ERR_NOTQQCONTACT = -46, 

	//ticket 
	MM_ERR_TICKET_UNMATCH = -47, 

	//ticket not found
	MM_ERR_TICKET_NOTFOUND = -48,

	//need input qq pwd
	MM_ERR_NEED_QQPWD = -49,

	//binduin binded by other
	MM_ERR_BINDUIN_BINDED = -50,

	// some user is not existed in user's contact for groupcard 
	MM_ERR_INVALID_GROUPCARD_CONTACT = -52,

	// the user has no hd headimg
	MM_ERR_HAS_NO_HEADIMG = -53,

	// invalid hd headimg request, invalid totallen
	MM_ERR_INVALID_HDHEADIMG_REQ_TOTAL_LEN = -54,

	// can't get bottle count
	MM_ERR_NO_BOTTLECOUNT = -56,

	//send verify code by mobile failed
	MM_ERR_SEND_VERIFYCODE = -57,

	//pick bottle no bottle
	MM_ERR_PICKBOTTLE_NOBOTTLE = -58,

	//unsupport country for bind or reg
	MM_ERR_UNSUPPORT_COUNTRY = -59,

	// gmail pwd wrong
	MM_ERR_GMAIL_PWD = -60,

	// gmail online limite
	MM_ERR_GMAIL_ONLINELIMITE = -61,

	// gmail need web login
	MM_ERR_GMAIL_WEBLOGIN = -62,

	// gmail imap not open
	MM_ERR_GMAIL_IMAP = -63, 

	// mail parse fail
	MM_ERR_PARSE_MAIL = -64, 

	// facebook绑定 已绑定返回该值
	MM_ERR_HAVE_BIND_FACEBOOK = -67,

	// facebook accesstoken不合法
	MM_ERR_INVALID_TOKEN = -68,

	// 大商家不给登录
	/* mp.weixin.qq.com account, login forbidden */
	MM_ERR_BIGBIZ_AUTH = -69,

	/* facebook getmfriend 未准备好(AsyncMq还在拉数据) */
	MM_ERR_GETMFRIEND_NOT_READY = -70,

	/* MassSend to many users in ToList */
	MM_ERR_TOLIST_LIMITED = -71,

	//QQ靓号过期
	MM_ERR_NICEQQ_EXPIRED = -72,

	//wx密码登录失败，尝试qq密码登录，此错误码客户端无需理会
	MM_ERR_TRYQQPWD = -73,

	//错误手机号，但建议用户修正手机号码。放在BaseResponse的ErrMsg
	MM_ERR_MOBILE_NEEDADJUST = -74,

	// 不在灰度版本的白名单内，不允许操作
	MM_ERR_ALPHA_FORBIDDEN = -75,

	//uuid已经被绑定
	MM_ERR_UUID_BINDED = -76,

	//session key 不正确
	MM_ERR_WRONG_SESSION_KEY = -77,

	//手机格式错误 用户区分 -41 的返回 客户端上来的手机号已经格式错误了 还是 走oversea svr失败的时候格式错误
	MM_ERR_SVR_MOBILE_FORMAT = -78,

	//新用户无法申请灰度白名单
	MM_ERR_NEW_USER = -79,

	//微博推荐新格式消息转换失败，就不下发这条消息
	MM_ERR_WEIBO_PUSH_TRANS = -80,

        //没有绑定QQ号码
        MM_ERR_NOTBINDQQ = -81,

        //只剩下一种绑定关系，不能解绑
        MM_ERR_ONE_BINDTYPE_LEFT = -82,

        //已经解绑，不能重复解绑
        MM_ERR_HAS_UNBINDED = -83,

        //已经绑定，不能再绑
        MM_ERR_HAS_BINDED = -84,

        //已经被别人绑定
        MM_ERR_BINDED_BY_OTHER = -85,

        //绑定的email不能和绑定QQ的qmail一样
        MM_ERR_BIND_EMAIL_SAME_AS_QMAIL = -86,

        //官号粉丝达到人数限制
        MM_ERR_BIZ_FANS_LIMITED = - 87,

	// cancel by user: shake tran img
	MM_ERR_SHAKE_TRAN_IMG_CANCEL = -90,

	// there is nothing upload from plugin: shake tran img
	MM_ERR_SHAKE_TRAN_IMG_NOTFOUND = -91,
	
	// continue : shake tran img
	MM_ERR_SHAKE_TRAN_IMG_CONTINUE = -92,
	
	// ERR: shake tran img
	MM_ERR_SHAKE_TRAN_IMG_OTHER = -93,

	// has auth another place
	MM_ERR_AUTH_ANOTHERPLACE = -100,

	MM_ERR_NO_RETRY = -101, //client don't retry opration more , and show hints from tBaseResponse.tErrMsg while was relied by svr

	MM_ERR_CERT_EXPIRED = -102,// client certification is expired. need to get certification again

	//答题次数用完
	MM_ERR_ANSWER_COUNT = -150,

	//提问次数用完
	MM_ERR_QUESTION_COUNT = -151,
	
	//没有问题
	MM_ERR_NO_QUESTION = -152,

	//关系不对劲
	MM_ERR_QA_RELATION = -153,

	//号码被冻结
	MM_ERR_ACCOUNT_BAN = -200,

	//QQ号码被封
	MM_ERR_QQ_BAN = -201,

	//解绑手机号，需要QQ密码验证
	MM_ERR_UNBIND_MOBILE_NEED_QQPWD = -202,

	MM_ERR_NODATA = -203,

	//帐号相关
	MM_ERR_OTHER_MAIN_ACCT =  -204, //需要其它主帐号登陆

	MM_ERR_QQ_OK_NEED_MOBILE = -205, //QQ验证OK，需要验证手机

	//解绑有关
	MM_ERR_UNBIND_MAIN_ACCT = -206, //解绑号为主帐号

	MM_ERR_TRYEMAILPWD = -207, //重试username+email密码

	//设置EMAIL为主帐号，当Email需要验证后主帐号生效
	MM_ERR_SET_EMAIL_MAIN_ACCT_NEED_VERIFY = -208,
	MM_ERR_MAIN_ACCT_HAS_SET = -209, //主帐号已经被设置了

	MM_ERR_WX_PWD_EXPIRED = -210, //wx密码过期，不再有效

	MM_ERR_NOT_MEET_REGION = -211 , //上传的不是需要的目标区数据

	/* big chatroom */
	MM_ERR_BIGCHATROOM_NOQUOTA = -250,
	MM_ERR_BIGCHATROOM_NONEED = -251,

        // This and all future requests should be directed to the given IDC Host and Builtin IP
        MM_ERR_IDC_REDIRECT = -301,
        
	//通过好友验证时查不到两者发起验证的记录
	MM_ERR_USER_NOT_VERIFYUSER = -302,

    	MM_ERR_NEED_BIND_QRCODE = -350,

	MM_ERR_PROXY_SHM_FULL = -400,

	//img has read to much
	MM_ERR_IMG_READ = -1005,

	MM_ERR_LBSDATANOTFOUND = -2000,

	// 由于被举报多次而屏蔽使用LBS
	MM_ERR_LBSBANBYEXPOSE = -2001,

	// 由于被举报多次而屏蔽使用漂流瓶
	MM_ERR_BOTTLEBANBYEXPOSE = -2002,

	// 由于被举报多次而屏蔽使用摇一摇
	MM_ERR_SHAKEBANBYEXPOSE = -2003,

	// 由于被举报多次而屏蔽使用二维码打招呼
	MM_ERR_QRCODEVERIFY_BANBYEXPOSE = -2004
};

#define MMCONFIG_FILE_PATH "/home/qspace/etc/mmconfig.conf"
#define MMUPDATECONFIG_FILE_PATH "/home/qspace/etc/mmupdateconfig.conf"
#define MMPACKAGECONFIG_FILE_PATH "/home/qspace/etc/mmpackageconfig.conf"
#define MMSHAKEIMG_FILE_PATH "/home/qspace/etc/mmshakeimglogic.conf"
#define MMSEARCH_QRCODE_CONFIG_FILE_PATH "/home/qspace/etc/mmsearchqrcodeconfig.conf"
#define MMQRCODE_SCAN_CONFIG_FILE_PATH "/home/qspace/etc/mmqrcodescanf.conf"
#define MMMATCH_URL_CONFIG_FILE_PATH "/home/qspace/etc/mmmatchurl.conf"

//FuncCtrl 配置文件路径
#define MMFUNCCTRL_FILE_PATH "/home/qspace/etc/client/mmfuncctrl.conf"

#define MM_INVALID_TIME -1

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
};


/* 用于push的CMD */
enum enPushCMD
{
	PUSH_CMD_TXT_MSG = 104,		// 普通消息
	PUSH_CMD_PRIVATE_MSG = 105, // 私信
	PUSH_CMD_CHATROOM_MSG = 106, // 群聊消息
	PUSH_CMD_MAIL = 107, 		// pushmail
	PUSH_CMD_QQOFF_MSG = 108, 	// qq离线相关
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
	MM_RETCODE = 1, // Errcode from the stop step
	MM_RESP = 2,
};

/// 操作功能号
enum emMMFunc
{
	MMFunc_Auth = 101,			// 验证
	MMFunc_Reg= 102,    // 注册
	MMFunc_Sync= 103,    // 同步
	MMFunc_SendMsg= 104, // 发送消息
	MMFunc_IphoneReg = 105, // Iphone注册token
	MMFunc_SearchContact = 106,   // 搜索Contact
	MMFunc_GetVerifyImg = 107,   // 获取验证码
	MMFunc_SendVerifyEmail = 108,  // 验证注册邮箱
	MMFunc_DownloadMsgImg = 109, // 取消息中的大图片
	MMFunc_UploadMsgImg = 110,  // 客户端上传消息中的大图片
	MMFunc_ChatStatus = 111, // 发送或取消聊天状态
	MMFunc_IphoneUnReg = 112, // Iphone注销token
	MMFunc_GetUpdateInfo = 113, // 获取版本升级信息
	MMFunc_GetUpdatePack = 114, // 获取版本文件
	MMFunc_GetInviteFriend = 115, // 获取邀请好友
	MMFunc_SendInviteEmail = 116, // 发送邀请信
	MMFunc_SendPrivateMsg = 117, // 发送微博私信
	MMFunc_UploadPrivateMsgImg = 118, // 发送微博图片
	MMFunc_CreateChatRoom = 119, // 创建聊天群组
	MMFunc_AddChatRoomMember = 120, // 新增聊天群成员
	MMFunc_Init = 121, // Init
	MMFunc_SendDomainEmail= 122, // Init
	MMFunc_BatchGetHeadImg = 123, //批量拉去头像
	MMFunc_SearchFriend = 124, // 搜索好友
	MMFunc_GetUserName = 125, //通过NickName生成UserName
	MMFunc_NewReg = 126, // 新注册协议
	MMFunc_UploadVoice = 127,
	MMFunc_DownloadVoice = 128,
	MMFunc_SwithPushMail = 129,
	MMFunc_UploadWeiboImg = 130,  // 往微博发送照片
	MMFunc_SendCard = 131,  // 往微博发自己的名片 
	MMFunc_MobileOpt = 132, // 手机操作
	MMFunc_UploadMContact = 133, //上传手机联系人
	MMFunc_ModifyHeadImg= 134, //修改用户头像
	MMFunc_NewGetInviteFriend= 135, //拉取全部好友的cgi
	MMFunc_SyncFriend=136, //看看谁在用
	MMFunc_VerifyUser=137, //好友验证
	MMFunc_NewSync=138, // newsync
	MMFunc_NewInit=139, // newsync
	MMFunc_BatchGetContactProfile=140,
	MMFunc_GetPSMImg=141,  //获取PushSystemMsg里面的图片
	MMFunc_GetMFriend=142,  //获取手机好友消息
	MMFunc_GetQQGroup=143,  //获取QQ好友
	MMFunc_BindQQ=144,  //绑定QQ号
	MMFunc_BindMobileForReg=145, //手机号注册 验证
	MMFunc_ShareFriendCard=146, //分享好友名片
	MMFunc_GetResetPwdUrl=147, //下发重置密码url到用户手机
	MMFunc_LbsFind=148, //LBS查找周围联系人
	MMFunc_UploadVideo=149,
	MMFunc_DownloadVideo=150,
	MMFunc_AddGroupCard=151, //添加群名片
	MMFunc_GetBottleCount=152, //获取瓶子数
	MMFunc_SendFeedback =153, //发送用户反馈
	MMFunc_ThrowBottle=154, //扔瓶子
	MMFunc_PickBottle=155, //捡瓶子
	MMFunc_OpenBottle=156, //开瓶子
	MMFunc_UploadHDHeadImg=157, // 上传高清头像
	MMFunc_GetHDHeadImg=158, // 获取高清头像
	MMFunc_GetPackageList=159, // 获取主题列表
	MMFunc_DownloadPackage=160, // 获取主题
	MMFunc_ShakeReport=161,// 上报 Shake 信息
	MMFunc_ShakeGet=162,// 获取 Shake 结果
	MMFunc_ShakeMatch=163,// 请求 Shake 匹配
	MMFunc_Echo=164, // 拨测专用CGI
	MMFunc_ShakeImg=165,// 请求 shake 背景图片
	MMFunc_Expose=166,// 举报
	MMFunc_GetVUserInfo=167, //获取认证用户信息
	MMFunc_GetQRCode=168, //获取用户二维码
	MMFunc_GmailOper=169, //gmail帐号绑定与解绑
	MMFunc_VoipInvite=170, //Voip邀请
	MMFunc_VoipCancelInvite=171, //Voip取消邀请
	MMFunc_VoipAnswer=172, //Voip应答
	MMFunc_VoipShutDown=173, //Voip挂断
	MMFunc_VoipSync=174, //Voip Sync
	MMFunc_UploadEmoji =175, //发送自定义表情
	MMFunc_DownloadEmoji =176, //下载自定义表情
	MMFunc_GeneralSet = 177, //通用设置, 满足阻塞的需求，避免总是增加cgi 
	MMFunc_VoipHeartBeat = 178, //Voip HeartBeat 
	MMFunc_DelChatRoomMember = 179, //群主踢人
	MMFunc_GetMailOAuthUrl = 180, // 获取邮件OAuth的URL
	MMFunc_CollectChatRoom = 181, //群名片转化成群
	MMFunc_GetContact = 182, //get contact
	MMFunc_FaceBookAuth = 183, //facebook绑定/解绑CGI
	MMFunc_GetAlbumPhotoListFP = 184, //拉取第一页个人相册photoinfo
	MMFunc_BatchGetAlbumPhoto = 185, //批量拉取相册缩略图
	MMFunc_BatchGetAlbumPhotoAttr = 186, //批量拉取相册图片附属信息
	MMFunc_UploadAlbumPhoto = 187, //上传个人相册图片
	MMFunc_DownloadAlbumPhoto = 188, //上传个人相册图片
	MMFunc_DelAlbumPhoto = 189, //删除个人相册图片
	MMFunc_AlbumPhotoComment = 190, //标红心
	MMFunc_GetTLPhotoListFP = 191, //拉取timeline第一页
	MMFunc_VoipStat = 192, //voip通话质量数据上报
	MMFunc_MassSend = 193,
	MMFunc_AlbumSync = 194, //个人相册Sync
	MMFunc_SearchQRCode = 195, //二维码搜索
	MMFunc_GetAlbumPhotoListNP = 196, //拉取下一页个人相册photoinfo
	MMFunc_GetTLPhotoListNP = 197, //拉取下一页timeline photoinfo
	MMFunc_GetLatestTLPhoto = 198, //拉取最新的timelinet photoid
	MMFunc_GetPhotoCommentList = 199, //获取一个照片的评论 
	MMFunc_AlbumLbs = 200, //给相册获取他们的lbs信息
	MMFunc_WinphoneReg = 201, //winphonereg 配置长连
	MMFunc_WinphoneUnReg = 202, //winphoneunreg 配置长连
	MMFunc_AlbumOperation = 203, //相册照片的一些操作
	MMFunc_ShakeSync = 204, // 摇一摇传图绑定
	MMFunc_GetWeiBoURL = 205, // 获取微博链接，带登陆态
	MMFunc_VoiceAddr = 206, //语音通讯录
	MMFunc_MMSnsUpload = 207, //sns上传buffer
	MMFunc_MMSnsDownload = 208, //sns下载buffer
	MMFunc_MMSnsPost = 209, //sns发布事件
	MMFunc_MMSnsObjectDetail = 210, //sns事件详情页
	MMFunc_MMSnsTimeLine = 211, //sns获取timeline
	MMFunc_MMSnsUserPage = 212, //sns个人页面
	MMFunc_MMSnsComment = 213, //sns评论
	MMFunc_MMSnsSync = 214, //sns消息系统同步
	MMFunc_GetLocation = 215, //get location
	MMFunc_MMSnsLbs = 216, //sns地理位置信息
	MMFunc_SetPassWd = 217, //手机注册设置密码
	MMFunc_MMSnsObjectOp = 218, //sns事件修改操作
	MMFunc_VerifyPassWd = 219, //验证密码
	MMFunc_UploadAppAttach = 220, // upload app attach
	MMFunc_DownloadAppAttach = 221, // download app attach
	MMFunc_SendAppMsg = 222, // send app msg 
	MMFunc_ImportFriends = 230,	 // add or recommend friends after reg by facebook
	MMFunc_GetAppInfo = 231,  //get app info, by iver
	MMFunc_GetRecommendAppList = 232,  //get recommend app list, by iver
	MMFunc_GetA8Key = 233,  //a2 key to a8 key
	MMFunc_MMUploadMedia = 240, // upload media 
	MMFunc_MMHandleImgMsg = 241, // handle img msg after uploading media
	MMFunc_FreqBlock = 242, // 频率拦截用
	MMFunc_GetQuestion = 243, //公众平?ㄎ蚀? 获取问题
	MMFunc_GetQACount = 244, //公众平台问答 获取提问or问题剩余次数
	MMFunc_UserInfoSet = 245, //bingchen 2014.2.20 用户信息设置
	MMFunc_ReAuth = 246,
	MMFunc_NewSyncCheck = 247,
	MMFunc_QuitChatRoom = 248, //???????
	MMFunc_SearchChatRoom = 249, //???
	// add here

	MMFunc_StatReport = 250,
	MMFunc_StatusNotify = 251, //web和手机会话同步
	MMFunc_PropertySurvey = 252,
	MMFunc_UnBindQQ = 253, //解除绑定QQ号
	MMFunc_CheckUnBind = 254, //判断能否解绑
	MMFunc_QueryHasPasswd = 255, //判断是否需要设置密码
    	MMFunc_EmailOpt = 256, //Email操作
	MMFunc_CreateGroup = 257,//?????
	MMFunc_AddGroupMember = 258, //?????
	MMFunc_DelGroupMember = 259, //?????
	MMFunc_QuitGroup = 260, //?????
	MMFunc_CollectGroup = 261, //???????????

    // add here

	MMFunc_LogOutWebWx = 281,
	MMFunc_LogOut = 282,
	MMFunc_WebWxLoginConfirm = 283,

	// add here

	MMFunc_MMSnsTagOption = 290, //sns tag 修改操作
	MMFunc_MMSnsTagMemberOption = 291,//sns tag member 修改操作
	MMFunc_MMSnsTagList = 292,// sns list 获取、推荐操作
	MMFunc_MMSnsTagMemMutilSet = 293,// 将用户设置到许多分组

	// add here

	MMFunc_UnbindMobileByQQ = 300,
	MMFunc_SnsGetCity = 301,
	MMFunc_GetProfile = 302,
	MMFunc_VoipGetRoomInfo = 303,
	MMFunc_SetPushSound = 304,
	MMFunc_VoipAck = 305,
	MMFunc_VoipInviteRemind = 306,
	MMFunc_VoipGetDeviceInfo = 307,
	MMFunc_ReportStrategyCtrl = 308,
	MMFunc_ReportClntPerf = 309,
	MMFunc_ReportKV = 310,
	MMFunc_ReportAction = 311,
	MMFunc_ReportCgiAccess = 312,
	MMFunc_ReportCrash = 313,
	MMFunc_BlackBerryReg = 314, //BlackBerryreg 配置长连
	MMFunc_BlackBerryUnReg = 315, //BlackBerryunreg 配置长连
	MMFunc_ShakeTranImgReport = 316, 
	MMFunc_ShakeTranImgGet = 317, 
	MMFunc_BatchGetShakeTranImg = 318, 
	MMFunc_ShakeTranImgUnBind = 319, 
	MMFunc_VoipStatReport = 320, 
	MMFunc_BakChatUploadHead = 321,
	MMFunc_BakChatUploadEnd = 322,
	MMFunc_BakChatUploadMsg =323,
	MMFunc_BakChatUploadMedia =324,
	MMFunc_BakChatRecoverGetList  = 325,
	MMFunc_BakChatRecoverHead  = 326,
	MMFunc_BakChatRecoverData  = 327,
	MMFunc_BakChatDelete = 328,
    MMFunc_UploadVoiceRecognize = 329,
	MMFunc_VoiceAddrReport = 330,
    MMFunc_OpVoiceReminder = 331,

	MMFunc_EnterTalkRoom = 332,
	MMFunc_ExitTalkRoom = 333,
	MMFunc_TalkMicAction = 334,
	MMFunc_TalkNoop = 335,
	MMFunc_GetTalkRoomMember = 336,

	MMFunc_BakChatUploadDB = 337,
	MMFunc_BakChatRecoverDB = 338,
	
	MMFunc_GrantBigChatRoom = 339,
    MMFunc_SendQRCodeByEmail = 340,

	MMFunc_BindSafeMobile = 341,
	MMFunc_AddSafeDevice = 342,
	MMFunc_GetSetableMainAcct = 343,
	MMFunc_SetMainAcct = 344,
	MMFunc_SetEmailPwd = 345,

    MMFunc_CreateTalkRoom = 346,
    MMFunc_AddTalkRoomMember = 347,
    MMFunc_DelTalkRoomMember = 348,

    MMFunc_UploadInputVoice = 349,

    MMFunc_CreateSubUser = 350, //客服号相关
    MMFunc_UpdateSubUser= 351,
    MMFunc_UnbindSubUser= 352,
    MMFunc_GetSubUserList = 353,
    
    MMFunc_GetCardList = 354,
    MMFunc_GetCardInfo = 355,
    MMFunc_InsertCard = 356,
    MMFunc_DeleteCard = 357,
    MMFunc_GetCardImg = 358,

    MMFunc_ClickCommand = 359,

	MMFunc_MMSubmsgSync = 360,

	MMFunc_MMUpdateSafeDevice = 361,
	MMFunc_MMDelSafeDevice = 362,

    MMFunc_MMGetBrandList = 363,

	MMFunc_TalkInvite = 364,

    MMFunc_TransferCard = 365,
    MMFunc_ParseCard = 366,

    MMFunc_ShakeMusic = 367,

	//附近的生活
	MMFunc_FindNBS = 368,
	MMFunc_SearchNBS = 369,
	MMFunc_GetNBSDetail = 370,
	MMFunc_GetShowcaseInfo = 371,
	MMFunc_GetObjectDetail = 372,

	MMFunc_TalkStatReport = 373,

    MMFunc_RegEquipment = 374,
    MMFunc_ResetEquipment = 375,

	//附近的群 
	MMFunc_JoinLbsRoom = 376,
	MMFunc_GetRoomMember = 377,

    MMFunc_AppComment = 378, // AppStore 用户推荐评分  
	
    MMFunc_GetCDNDns = 379, // 获取上传图片到cdn的 ip和key
    MMFunc_NewAuth = 380,			//新验证
    MMFunc_GetCert = 381, //获取证书
    MMFunc_NewSetEmailPwd = 382,
    MMFunc_NewSetPassWd = 383,
    MMFunc_NewVerifyPassWd = 384,
    MMFunc_LbsMatch = 385, // 附近的人配对
    MMFunc_SyncLbsMatch = 386, // 同步配对列表

	MMFunc_UploadCoverImg = 387,
	MMFunc_GetCoverImg = 388,

	MMFunc_PushSet = 389,

	MMFunc_UploadPlugFriends = 390,		// 上传第三方联系人
	
	MMFunc_LbsChatRoomFind = 391, //附近的群搜索
	MMFunc_VerifyChatRoomMember=392, //群成员添加验证

	//////////////////////////////////////////////////////////////////
	MMFunc_ModUserName = 393, //修改用户名
	MMFunc_ModPwd =394, //修改密码
	MMFunc_ModPwdByMobile =394, //通过手机修改密码

	MMFunc_InviteChatRoomMember=395, //邀请群成员
	MMFunc_CloseChatRoom=396, //解散群

	MMFunc_ReportProfile = 397, //举报用户
	MMFunc_ReportGroup = 398, //举报群
	
	MMFunc_NotExist
};

enum enGeneralSetType
{
	MM_GENERAL_SETTYPE_ALIAS = 1, //别名柚? 
};


enum enNewRegMode
{
	MM_REGMODE_QQ = 0,
	MM_REGMODE_MOBILE = 1,
	MM_REGMODE_QQ2 = 2,
	MM_REGMODE_MOBILE_NOPWD = 4,
    MM_REGMODE_EQUIPMENT = 5,
};

enum enGmailStatus
{
	MM_GMAIL_OK = 0,

	MM_GMAIL_PWD = 1,

	MM_GMAIL_ONLINELIMITE = 2,

	MM_GMAIL_WEBLOGIN = 3,

	MM_GMAIL_IMAP = 4, 
};

enum enGmailOper
{
	MM_GMAIL_BIND = 1,
	MM_GMAIL_UNBIND = 2,
};

enum enGmailSwitch
{
	MM_GMAIL_SWITCH_OPEN = 1,
	MM_GMAIL_SWITCH_CLOSE = 2,
};

enum enSceneStatus
{
	MM_SCENE_MANUALAUTH = 1,
	MM_SCENE_AUTOAUTH = 2,
	MM_SCENE_NORMALINIT = 3,
	MM_SCENE_UPDATEINIT = 4,
	MM_SCENE_SENDMSG = 5,
	MM_SCENE_RETRYSENDMSG = 6,
	MM_SCENE_REGINIT = 7,
	MM_SCENE_MANUALUSERNAME = 8,
	MM_SCENE_SYSTEMUSERNAME = 9,
	MM_SCENE_FACEBOOK = 10,
	MM_SCENE_AUTOAUTH_CHANGEUUID = 11, //客户端更改了uuid的自动登陆。ios6时可能出现这种情况 by jfd
	MM_SCENE_AUTOAUTH_UPGRADE = 12, //客户端升级后第一次自动登陆 by jfd
	MM_SCENE_WEB_WEIXIN = 13, //微信网页版使用场景
	MM_SCENE_BROKER = 14, //mmbroker
	MM_SCENE_REAUTH = 15,
};

enum enNewRegReturnFlag
{
	MM_NEWREG_RETURNFLAG_NEEDQFRAME = 0x1,
};

enum enRegType
{
	MM_USERATTR_REG_QQ = 1,
	MM_USERATTR_REG_MOBILE = 2,
	MM_USERATTR_REG_EMAIL = 3,
	MM_USERATTR_REG_BIZ = 4,
	MM_USERATTR_REG_FACEBOOK = 5,
    MM_USERATTR_REG_SUBUSER = 6,
    MM_USERATTR_REG_EQUIPMENT = 7,
};

enum enAddContactScene 
{
	MM_ADDSCENE_SEARCH_QQ = 1,       // 通过搜索QQ昵称
	MM_ADDSCENE_SEARCH_EMAIL = 2,    // 通过搜索QQMail昵称
	MM_ADDSCENE_SEARCH_WEIXIN = 3,   // 通过搜索微信号
	MM_ADDSCENE_PF_QQ = 4,           // 通过可能认识的QQ好友 
	MM_ADDSCENE_PF_EMAIL = 5,        // 通过可能认识的QQMail好友
	MM_ADDSCENE_PF_CONTACT = 6,      // 通过把我加到通讯录的人  
	MM_ADDSCENE_PF_WEIXIN = 7,       // 通过可能认识的微信好友(二度关系)
	MM_ADDSCENE_PF_GROUP = 8,        // 通过可能认识的群好友
	MM_ADDSCENE_PF_UNKNOWN = 9,        // “可能认识的好友”（无法区分来源）
	MM_ADDSCENE_PF_MOBILE = 10,        // 手机通讯录
	MM_ADDSCENE_PF_MOBILE_EMAIL = 11,        // 邮箱匹配
	MM_ADDSCENE_VIEW_QQ = 12,		//查看QQ好友
	MM_ADDSCENE_VIEW_MOBILE = 13,	//查看手机通讯录
	MM_ADDSCENE_CHATROOM = 14,	//群动作
	MM_ADDSCENE_SEARCH_PHONE = 15,	//搜索手机
	MM_ADDSCENE_CORP_EMAIL = 16,	//工作邮箱
	MM_ADDSCENE_SEND_CARD = 17,	//发名片
	MM_ADDSCENE_LBS = 18,	//LBS加好友
	MM_ADDSCENE_PF_MOBILE_REVERSE = 21,   // 手机通讯录反向好友
	MM_ADDSCENE_PF_SHAKE_PHONE_PAIR = 22,   // 撸sever加好友单对单 
	MM_ADDSCENE_PF_SHAKE_PHONE_GROUP = 23,   // 撸sever加好友群加 
	MM_ADDSCENE_PF_SHAKE_PHONE_OPPSEX= 24,   // 撸sever加好友1000米外异性
	MM_ADDSCENE_BOTTLE = 25, // 漂流瓶打招呼加好友
	MM_ADDSCENE_SHAKE_SCENE1 = 26, //shake, 3s, <= 1km
	MM_ADDSCENE_SHAKE_SCENE2 = 27, //shake, <= 1km
	MM_ADDSCENE_SHAKE_SCENE3 = 28, //shake <= 10km
	MM_ADDSCENE_SHAKE_SCENE4 = 29, //shake, > 10km
	MM_ADDSCENE_QRCode = 30,//二维码	
	MM_ADDSCENE_FACEBOOK = 31, //facebook插件
	MM_ADDSCENE_SNS = 32, //通过朋友圈推荐添加
	MM_ADDSCENE_WEB = 33, // from web
	MM_ADDSCENE_BRAND_QA = 34, //公众账号问答
    MM_ADDSCENE_FUZZY_SEARCH = 35, //通过搜昵称（模糊搜索）
    MM_ADDSCENE_LOGO_WALL = 36, //通过点击logo墙
    MM_ADDSCENE_TIMELINE_BIZ = 37, // 在朋友圈中，点击公众账号名的方式查看关注了公众账号。
    MM_ADDSCENE_PROMOTE_MSG = 38, //在会话中，点击公众账号下发消息所带的公众账号名查看关注了公众账号。
    MM_ADDSCENE_SEARCH_BRAND = 39, //通过“查找微信公众号”入口关注的公众号。
	MM_ADDSCENE_BIZ_CONFERENCE = 40, //添加开启了好友验证的会议公众号
	MM_ADDSCENE_PROMOTE_BIZCARD = 41, //表示“公众平台下发的名片”而加到的好友。
	MM_ADDSCENE_WEB_PROFILE_URL = 42, 
	MM_ADDSCENE_WEB_OP_MENU = 43, 
	MM_ADDSCENE_LBSROOM = 44,
	MM_ADDSCENE_GOOGLE_PLUS = 45, // 通过 google+ 推荐
};

enum enFriendType
{
	MM_FRIENDTYPE_QQ = 0,
	MM_FRIENDTYPE_BLOG = 1,
	MM_FRIENDTYPE_EMAIL = 2,
	MM_FRIENDTYPE_WEIXIN = 3,
	MM_FRIENDTYPE_SMS = 4,
	MM_FRIENDTYPE_FACEBOOK = 5,
};

enum enChatStatus
{
	MM_CHAT_TYPING = 1,
	MM_CHAT_CANCELTYPING = 2,
	MM_VOICE_TYPING = 3,
	MM_VOICE_CANCELTYPING = 4,
};

enum enMMStatus
{
	MM_STATUS_OPEN=0x1,
	MM_STATUS_EMAILVERIFY=0x2,
	MM_STATUS_MOBILEVERIFY=0x4,
	MM_STATUS_QQ_SEARCH_CLOSE=0x08,
	MM_STATUS_QQ_PROMOTE_CLOSE=0x10,
	MM_STATUS_VERIFY_USER =0x20,
	MM_STATUS_QMSG=0x40,
	MM_STATUS_QQ_PROMOTE_TOME_CLOSE=0x80,
	MM_STATUS_PROMOTE_TOME_CLOSE=0x100,
	MM_STATUS_MOBILE_SEARCH_CLOSE=0x200,
	MM_STATUS_ADD_CONTACT_CLOSE=0x400,
	MM_STATUS_APNS_TIPS_CLOSE= 0x800,
	MM_STATUS_FLOATBOTTLE_OPEN= 0x1000,
	MM_STATUS_WEIXIN_ONLINE_OPEN = 0x2000,
	MM_STATUS_MEDIANOTE_OPEN = 0x4000,
	MM_STATUS_BOTTLE_CHART_OPEN = 0x8000,
	MM_STATUS_BOTTLE_CHART_STARTED = 0x10000,
	MM_STATUS_UPLOADMCONTACT_CLOSE = 0x20000,
	// [0x1, 0x10000] 标志位放在 mmaccount ，再增加新的标志位，请加到 mmuserattr
};


// status can't not be change by moduserinfo
enum enMMProfileFlag
{
	MM_NEED_SET_PASSWD = 0x1,

};

enum enPluginSwitch
{
	MM_STATUS_GMAIL_OPEN = 0x1, // gmail开关
	MM_STATUS_TXWEIBO_OPEN = 0x2, // 微博url开关
	MM_STATUS_RECFBFRIEND_OPEN = 0x4, // facebook好友推荐开关
	MM_STATUS_READAPP_PUSH_OPEN = 0x8, // readerapp 推送开关 
	MM_STATUS_READAPP_TXNEWS_OPEN = 0x10, // readerapp 腾讯新闻推送开关 
	MM_STATUS_READAPP_WB_OPEN = 0x20, // 微博精选推送开关 
	MM_STATUS_TXWEIBO_ICON_OPEN = 0x40, // 是否显示腾讯微博图标开关
	MM_STATUS_MASSSEND_SHOW_IN_CHATLIST_OPEN = 0x80, // 群发主界面显示开关
	MM_STATUS_MEISHI_CARD_OPEN = 0x100, // QQ美食特惠插件开关
	MM_STATUS_USERNAME_SEARCH_CLOSE = 0x200, // search contact 中微信号开关 
	MM_STATUS_NEWSAPP_TXNEWS_CLOSE = 0x400, // newsapp 腾讯新闻推送开关, 注意这个位置上后表示关闭 
	MM_STATUS_PRIVATEMSG_RECV_CLOSE = 0x800, // 私信推送开关，注意这个位置上后表示关闭
	MM_STATUS_PUSHMAIL_NOTIFY_CLOSE = 0x1000, // pushmail 推送开关， 注意这个位置上后表示关闭
	MM_STATUS_WEBONLINE_PUSH_OPEN = 0x2000,  // webwx在线，是否打开PUSH
    MM_STATUS_SAFEDEVICE_OPEN = 0x4000, // for safedevice function switch
};

enum enGrayscaleFlag
{
	MM_GRAYSCALE_CDNIMG = 0x1, // 支持从cdn传图
};

enum enGrayscaleStatus
{
	MM_GRAYSCALE_OPEN = 0x1,
	MM_GRAYSCALE_CLOSE = 0x0,
};

enum enWebWxPluginSwitch
{
	MM_STATUS_TONE_OPEN = 0x1,
    MM_STATUS_TABLE_WARN_OPEN = 0x2,
};

enum enPluginBitStatus
{
	MM_PLUGIN_BIT_STATUS_SWITCH = 0x1,
	MM_PLUGIN_BIT_STATUS_FLAG = 0x2,

	MM_PLUGIN_BIT_STATUS_NONE = 0x80,
};

enum enPluginMsgID
{
	MM_PLUGIN_MSG_ID_NEWS = 1000,
	MM_PLUGIN_MSG_ID_PUSHSYSMSG = 1001,
    MM_PLUGIN_MSG_ID_MASSSEND = 1002,
};

enum enMMSyncMsg
{
	MM_MSG_STATUS=0x01,
	MM_MSG_CONTENT=0x02,
};

enum enMMImgStatus
{
	MM_IMGSTATUS_NOIMG=1,
	MM_IMGSTATUS_HASIMG=2,
};

enum enMMMsgStatus
{
	MM_MSGSTATUS_SENDING=1,
	MM_MSGSTATUS_SENT=2,
	MM_MSGSTATUS_DELIVERED=3,
	MM_MSGSTATUS_READ=4,
	MM_MSGSTATUS_SENDFAIL=5,
};

enum enMMScene
{
	MM_SCENE_SYNCALL=1,
	MM_SCENE_SYNCMSG=2,
};

enum enMMDataType
{
	MM_DATA_TEXT=1, // 文本类型
	//MM_DATA_HTML=2, // 未使用,已废弃
	MM_DATA_IMG=3, // 图片类型
	MM_DATA_PRIVATEMSG_TEXT=11, // 私信文本
	//MM_DATA_PRIVATEMSG_HTML=12, // 未使用,已废弃
	MM_DATA_PRIVATEMSG_IMG=13, // 私信图片
	MM_DATA_CHATROOMMSG_TEXT=21, // 被客户端误用，兼容 
	//MM_DATA_CHATROOMMSG_HTML=22, // 未使用,已废弃
	MM_DATA_CHATROOMMSG_IMG=23,  // 被客户端误用，兼容
	//MM_DATA_EMAILMSG_TEXT=31,    // 未使用,已废弃
	//MM_DATA_EMAILMSG_HTML=32,    // 未使用,已废弃
	//MM_DATA_EMAILMSG_IMG=33,     // 未使用,已废弃
	MM_DATA_VOICEMSG=34,  // 语音类型
	MM_DATA_PUSHMAIL=35, // pushmail类型
	MM_DATA_QMSG=36, // QQ离线消息文本
	MM_DATA_VERIFYMSG=37, // 好友验证类型
	MM_DATA_PUSHSYSTEMMSG=38, // 广告消息类型
	MM_DATA_QQLIXIANMSG_IMG=39, // QQ离线消息图片
	MM_DATA_POSSIBLEFRIEND_MSG=40, // 好友推荐类型
	MM_DATA_PUSHSOFTWARE=41, // 精品软件推荐类型
	MM_DATA_SHARECARD=42, // 名片分享类型
	MM_DATA_VIDEO=43, // 视频类型
	MM_DATA_VIDEO_IPHONE_EXPORT=44, // 转发视频类型
	MM_DATA_GMAIL_PUSHMAIL=45, // Gmail pushmail类型
	MM_DATA_EMPTY=46,//客户端要求，占坑
	MM_DATA_EMOJI=47, // 自定义表情类型
	MM_DATA_LOCATION=48, // 位置消息类型
	MM_DATA_APPMSG=49, // AppMsg
	MM_DATA_VOIP_INVITE=50, //voipinvite
	MM_DATA_STATUSNOTIFY=51, //web和手机状态同步消息
	MM_DATA_WEIBOPUSH=52, // Weibo Push( MMReader )
	MM_DATA_WEBWXVOIPNOTIFY =53, // webwx voip notify
    MM_DATA_CHATROOMVERIFYMSG=54, //申请加入群验证消息
	MM_DATA_BRAND_QA_ASK=55, //公众平台问答 提问
	MM_DATA_TALK_SYSMSG=56, //对讲模式的系统消息
	MM_DATA_BRAND_QA_MSG=57, //公众平台问答 普通消息
	MM_DATA_OPEN_SUBSCRIBE=58, //开放平台 订阅消息
	MM_DATA_OPEN_REPORT=59, //开放平台 报告消息
	MM_DATA_OPEN_LINK=60, //开放平台 报告消息
	MM_DATA_DEL_CONTACT = 61, // 删除好友消息
	MM_DATA_VERIFY_CHATMEMBERMSG=62, // 群成员验证类型
	MM_DATA_INVITE_CHATMEMBERMSG=63, // 邀请群成员类型
	MM_DATA_LBSMATCH_SUCCESS=64, // 配对成功
	MM_DATA_CHATROOMINFO_UPDATE=65, // 群资料修改成功
	MM_DATA_CHATROOM_BEKICKOUT=66, // 被移出群
	MM_DATA_CHATROOM_CLOSE=67, //群解散
	MM_DATA_BIND_PROMTP=68, // 新加入的第三方用户推荐给老用户
	MM_DATA_SYSCMD_IPXX=9998, // 用于查询问题通知客户端上传日志
	MM_DATA_SYSNOTICE=9999, // 系统通知类型
	MM_DATA_SYS=10000, // 系统消息，出现在会话中间
    MM_DATA_SYSCMD_XML = 10001 , //系统命令XML消息，客户端只处理，不显示
    MM_DATA_SYSCMD_NEWXML = 10002 , //系统命令XML消息，客户端只处理，不显示
};

enum enMMStatusNotifyType
{
	MM_STATUSNOTIFY_MARKMSGREAD = 1,
	MM_STATUSNOTIFY_ENTERSESSION = 2,
	MM_STATUSNOTIFY_CHATLISTREQ = 3,
	MM_STATUSNOTIFY_CHATLISTRESP = 4,
	MM_STATUSNOTIFY_QUITSESSION = 5,
	MM_STATUSNOTIFY_CONFIRM = 6,
};

enum emMMEquipmentStatusNotifyType
{
    MM_EQUIPMENT_STATUSNOTIFY_SET_EQUIPMENT_BEGIN = 1,
    MM_EQUIPMENT_STATUSNOTIFY_SET_EQUIPMENT_SUCCESS = 2,
    MM_EQUIPMENT_STATUSNOTIFY_SET_EQUIPMENT_FAIL = 3,
};

enum enMMAppMsgInnerType
{
	MM_APP_TEXT = 1,
	MM_APP_IMG = 2,
	MM_APP_MUSIC = 3,
	MM_APP_VIDEO = 4,
	MM_APP_URL = 5,
	MM_APP_FILE = 6,
	MM_APP_OPEN = 7,
	MM_APP_EMOJI = 8,
	MM_APP_VOICE_REMIND = 9,
};

enum enMMAppMsgShowType
{
	MM_APPMSG_SHOW_DEFAULT = 0,
	MM_APPMSG_SHOW_READER = 1,
	MM_APPMSG_SHAKETRANIMG_RESULT = 2, 
    MM_APPMSG_VOICEREMIND_CONFIRM = 3,
    MM_APPMSG_VOICEREMIND_REMIND = 4,
    MM_APPMSG_VOICEREMIND_SYS = 5, // voice + text
	MM_APPMSG_SHOW_SELF_PIC_TEXT = 6, // 自定义的图文消息
};

enum enMMAppMsgItemShowType
{
	MM_APPMSG_ITEM_SHOW_BIG_PIC_TEXT = 0,   // 大图展示的图文消息Item
	MM_APPMSG_ITEM_SHOW_SMALL_PIC_TEXT = 1, // 小图展示的图文消息Item
	MM_APPMSG_ITEM_SHOW_ALL_TEXT = 2,       // 全文本展示的消息Item, 仅仅显示摘要
	MM_APPMSG_ITEM_SHOW_TABLE_TEXT = 3,     // 表格展示的消息Item,每个显示标题和摘要
};

enum enMMAppMsgScene
{
	MM_APP_FROMMOBILE = 0,
	MM_APP_FROMWEB = 1,
};

enum enMMBizDataType
{
	MM_BIZ_DATA_TEXT=1,
	MM_BIZ_DATA_IMG=2,
	MM_BIZ_DATA_VOICE=3,
	MM_BIZ_DATA_VIDEO=4,
	MM_BIZ_DATA_APPMSG=10,
	MM_BIZ_DATA_SHARECARD=42,
};

enum enMMChatRoomNotifyStatus
{
	MM_CHATROOMNOTIFY_OPEN = 0x1,
	MM_CHATROOMNOTIFY_CLOSE = 0x0,
};

enum enMMContactType
{
	MM_CONTACT_WEIXIN=0,
	MM_CONTACT_QQMICROBLOG=1,
	MM_CONTACT_CHATROOM=2,
	MM_CONTACT_EMAIL=3,
	MM_CONTACT_QQ=4,
	MM_CONTACT_BOTTLE=5,
	MM_CONTACT_MASSSEND=6,
};

enum enMMMicroBlogType
{
	MM_MICROBLOG_QQ=1,
};

enum enMMSexType
{
	MM_SEX_UNKNOWN=0,
	MM_SEX_MALE=1,
	MM_SEX_FEMALE=2,
};

enum enMMPersonalCardFlag
{
	MM_PCFLAG_NONE = 0,
	MM_PCFLAG_SET = 1,
};

enum enMMDelFlagVal
{
	MM_DELFLAG_EXIST=1,
	MM_DELFLAG_DEL=2,
};

enum enMMDomainEmailStatus
{
	MM_DOMAINEMAIL_NOTVERIFY=1,
	MM_DOMAINEMAIL_VERIFIED=2,
};

enum enMMFunctionSwitchType
{
	MM_FUNCTIONSWITCH_PUSHMAIL=1,
	MM_FUNCTIONSWITCH_QQ_SEARCH_CLOSE=2,
	MM_FUNCTIONSWITCH_QQ_PROMOTE_CLOSE=3,
	MM_FUNCTIONSWITCH_VERIFY_USER=4,
	MM_FUNCTIONSWITCH_QQMSG=5,
	MM_FUNCTIONSWITCH_QQ_PROMOTE_TOME_CLOSE=6,
	MM_FUNCTIONSWITCH_PROMOTE_TOME_CLOSE=7,
	MM_FUNCTIONSWITCH_MOBILE_SEARCH_CLOSE=8,
	MM_FUNCTIONSWITCH_ADD_CONTACT_CLOSE=9,
	MM_FUNCTIONSWITCH_APNS_TIPS_CLOSE= 10,
	MM_FUNCTIONSWITCH_BOTTLE_OPEN=11,		
	MM_FUNCTIONSWITCH_WEIXIN_ONLINE_OPEN=12,		
	MM_FUNCTIONSWITCH_MEDIANOTE_OPEN=13,		
	MM_FUNCTIONSWITCH_BOTTLE_CHART_OPEN =14,
	MM_FUNCTIONSWITCH_GMAIL_OPEN =15,
	MM_FUNCTIONSWITCH_TXWEIBO_OPEN =16,
	MM_FUNCTIONSWITCH_UPLOADMCONTACT_CLOSE =17,
	MM_FUNCTIONSWITCH_RECFBFRIEND_OPEN =18,
	MM_FUNCTIONSWITCH_READER_OPEN =19,
	MM_FUNCTIONSWITCH_READER_TXNEWS_OPEN =20,
	MM_FUNCTIONSWITCH_READER_WB_OPEN =21,
	MM_FUNCTIONSWITCH_TXWEIBO_ICON_OPEN =22,
	MM_FUNCTIONSWITCH_MEISHI_CARD_OPEN =23,
	MM_FUNCTIONSWITCH_ALBUM_NOT_FOR_STRANGER =24,
	MM_FUNCTIONSWITCH_USERNAME_SEARCH_CLOSE=25,
	MM_FUNCTIONSWITCH_NEWSAPP_TXNEWS_CLOSE =26,
	MM_FUNCTIONSWITCH_WEBONLINE_PUSH_OPEN = 27,
    MM_FUNCTIONSWITCH_SAFEDEVICE_OPEN = 28, //safedevice
};

enum enMMFunctionSwitchStatus
{
	MM_FUNCTIONSWITCH_NOSUCHFUNCTION=0,
	MM_FUNCTIONSWITCH_OPEN=1,
	MM_FUNCTIONSWITCH_CLOSE=2,
};

enum enMMWebWxFunctionSwitchType
{
	MM_WEBWXFUNCTIONSWITCH_TONE_OPEN = 1,
    MM_WEBWXFUNCTIONSWITCH_TABLE_WARN_OPEN = 2,
};

enum enMMebWxFunctionSwitchStatus
{
	MM_WEBWXFUNCTIONSWITCH_NOSUCHFUNCTION=0,
	MM_WEBWXFUNCTIONSWITCH_OPEN=1,
	MM_WEBWXFUNCTIONSWITCH_CLOSE=2,
};

enum enMMSyncCmd
{
	MM_SYNCCMD_MODUINFO=1,
	MM_SYNCCMD_MODCONTACT=2,
	MM_SYNCCMD_MODCONTACTIMG=3,
	MM_SYNCCMD_DELCONTACT=4,
	MM_SYNCCMD_ADDMSG=5,
	MM_SYNCCMD_MODMSGSTATUS=6,
	MM_SYNCCMD_DELCHATCONTACT=7,
	MM_SYNCCMD_DELCONTACTMSG=8,
	MM_SYNCCMD_DELMSG=9,
	MM_SYNCCMD_REPORT=10,
	MM_SYNCCMD_OPENQQMICROBLOG=11, 
	MM_SYNCCMD_CLOSEMICROBLOG=12,
	MM_SYNCCMD_MODMICROBLOG=13,
	MM_SYNCCMD_MODNOTIFYSTATUS=14,
	MM_SYNCCMD_MODCHATROOMMEMBER=15,
	MM_SYNCCMD_QUITCHATROOM=16,
	MM_SYNCCMD_MODCONTACT_DOMAINEMAIL=17,
	MM_SYNCCMD_MODUSER_DOMAINEMAIL=18,
	MM_SYNCCMD_DELUSER_DOMAINEMAIL=19,
	MM_SYNCCMD_MODCHATROOMNOTIFY=20,
	MM_SYNCCMD_POSSIBLEFRIEND=21,
	MM_SYNCCMD_INVITEFRIENDOPEN=22,
	MM_SYNCCMD_FUNCTIONSWITCH=23,
	MM_SYNCCMD_MODQCONTACT=24,
	MM_SYNCCMD_MODTCONTACT=25,
	MM_SYNCCMD_PSMSTAT=26,
	MM_SYNCCMD_MODCHATROOMTOPIC=27,
	MM_SYNCCMD_OFFICLAMSG=28,
	MM_SYNCCMD_MEDIANOTE=29,
	MM_SYNCCMD_UPDATESTAT=30,
	MM_SYNCCMD_MODDISTURBSETTING=31,
	MM_SYNCCMD_DELETEBOTTLE=32,
	MM_SYNCCMD_MODBOTTLECONTACT=33,
	MM_SYNCCMD_DELBOTTLECONTECT=34,
	MM_SYNCCMD_MODUSERIMG=35,
	MM_SYNCCMD_KVSTAT=36,
	MM_SYNCCMD_THEME=37,
	MM_SYNCCMD_GMAILSWITCH=38,
	MM_SYNCCMD_PLUGININSTALL=39,
	MM_ALBUM_SYNCCMD_PHOTOID = 40,
	MM_ALBUM_SYNCCMD_COMMENTTIPS = 41,
	MM_SYNCCMD_MODALBUMINFO = 42,
	MM_SYNCCMD_MODTXNEWS_CATEGORY = 43,
	MM_SYNCCMD_USERINFOEXT = 44,

	MM_SNS_SYNCCMD_OBJECT = 45,
	MM_SNS_SYNCCMD_ACTION = 46,

	MM_SYNCCMD_BRAND_SETTING = 47,

    MM_SYNCCMD_MODCHATROOMMEMBERDISPLAYNAME = 48,
    MM_SYNCCMD_MODCHATROOMMEMBERFLAG = 49,

	MM_SYNCCMD_WEBWXFUNCTIONSWITCH = 50,
	MM_SYNCCMD_MODSNSUSERINFO = 51,
    MM_SYNCCMD_MODSNSBLACKLIST = 52,
	MM_SYNCCMD_NEWDELMSG=53,
	MM_SYNCCMD_SYNCLBSMATCH=54,//附近的人配对列表

	MM_SYNCCMD_MODGROUPTOPIC=55,
	
	// 出错时使用的虚拟cmd
	MM_SYNCCMD_ERROR = 9999 
};

// 腾讯新闻分类ID
enum enMMTXNewsCategory
{
	MM_CATEGORY_NEWS = 0x1,  // 新闻
	MM_CATEGORY_SPORTS = 0x2, // 体育
	MM_CATEGORY_ENT = 0x4, // 娱乐
	MM_CATEGORY_FINANCE = 0x8, // 财经 
	MM_CATEGORY_TECH = 0x10, // 科技 
	MM_CATEGORY_GAMES = 0x20, // 游戏 
	MM_CATEGORY_LADY = 0x40, // 女性 
	MM_CATEGORY_DIGI = 0x80, // 数码  
	MM_CATEGORY_MIL = 0x100, // 军事 
	MM_CATEGORY_STOCK = 0x200, // 证券 
	MM_CATEGORY_HOUSE = 0x400, // 房产 
	MM_CATEGORY_AUTO = 0x800, // 汽车 
	MM_CATEGORY_USSTOCK = 0x1000, // 美股
	MM_CATEGORY_HKSTOCK = 0x2000, // 港股 
};

enum enMMNewSyncSelector
{
	MM_NEWSYNC_PROFILE = 0x1,
	MM_NEWSYNC_MSG = 0x2,
	MM_NEWSYNC_WXCONTACT = 0x4,
	MM_NEWSYNC_PMCONTACT = 0x8,
	MM_NEWSYNC_QQCONTACT = 0x10,
	MM_NEWSYNC_FRIEND = 0x20,
	MM_NEWSYNC_BOTTLECONTACT = 0x40,
	MM_NEWSYNC_ALBUMSYNCKEY = 0x80,
	MM_NEWSYNC_SNSSYNCKEY = 0x100,
	MM_NEWSYNC_VOIPSNYC = 0x200,
	MM_NEWSYNC_VOIPINVITE = 0x400,
    MM_NEWSYNC_SUBMSGSYNCKEY = 0x800,//客服信息同步
    MM_NEWSYNC_LBSMATCH = 0x1000, // 附近的人配对成功
};

enum enMMNewSyncStaus
{
	MM_PRESENCE_WEBWX = 0x1,
	MM_NEWSYNC_STATUS_SHAKETRANIMG_BIND = 0x2,
	MM_NEWSYNC_STATUS_SHAKETRANIMG_ACTIVE = 0x4,
	MM_NEWSYNC_STATUS_SHAKEBOOKMARK_BIND = 0x8,
};


enum enMMNewSyncKey
{
	MM_NEWSYNC_KEY_PROFILE = 1,
	MM_NEWSYNC_KEY_MSG = 2,
	MM_NEWSYNC_KEY_WXCONTACT = 3,
	MM_NEWSYNC_KEY_PMCONTACT = 4,
	MM_NEWSYNC_KEY_QCONTACT = 5,
	//MM_NEWSYNC_KEY_FRIEND = 6, // 不能用，听说有坑在这里
	MM_NEWSYNC_KEY_BOTTLECONTACT = 7,
	MM_NEWSYNC_KEY_ALBUMFEED = 8,
	MM_NEWSYNC_KEY_SNSTIPS = 9,
	MM_NEWSYNC_KEY_SUBMSG= 10,
	MM_NEWSYNC_KEY_LBSMATCH = 11, // 只做定义，不做配对同步

	MM_NEWSYNC_KEY_PUBLIC_BEGIN = 1000,
	MM_NEWSYNC_KEY_PLUGINKEY_NEWS = 1000,
};

enum enMMNewSyncScene
{
	// notify 
	MM_NEWSYNC_SCENE_NOTIFY = 1,
	// timer
	MM_NEWSYNC_SCENE_TIMER = 2,
	// 进入前台
	MM_NEWSYNC_SCENE_BG2FG = 3,
	// 进程启动
	MM_NEWSYNC_SCENE_PROCESSSTART = 4, 
	// init之后第一次newsync
	MM_NEWSYNC_SCENE_AFTERINIT = 5,
	// continueflag 不为0
	MM_NEWSYNC_SCENE_CONTINUEFLAG = 6,
	// 其他操作
	MM_NEWSYNC_SCENE_OTHER = 7,
	// syncwait触发的newsync操作
	MM_NEWSYNC_SCENE_SYNCWAIT = 8,

    // 第一次syncwait触发的newsync操作
    MM_NEWSYNC_SCENE_FIRST_SYNCWAIT = 9,
};

enum enMMUInfoBitFlag
{
	MM_UINFOFLAG_USERNAME=0x1,
	MM_UINFOFLAG_NICKNAME=0x2,
	MM_UINFOFLAG_BINDUIN=0x4,
	MM_UINFOFLAG_BINDEMAIL=0x8,
	MM_UINFOFLAG_BINDMOBILE=0x10,
	MM_UINFOFLAG_STATUS=0x20,
	MM_UINFOFLAG_IMG=0x40,
	MM_UINFOFLAG_PERSONALCARD=0x80,
	MM_UINFOFLAG_BOTTLEIMG =0x100,
	MM_UINFOFLAG_HDHEADIMGMD5SUM =0x200,
	MM_UINFOFLAG_BOTTLEHDHEADIMGMD5SUM =0x400,
	MM_UINFOFLAG_PLUGINFLAG=0x800,
	//add by yihuang
	MM_UINFOFLAG_LBS=0x1000,
	MM_UINFOFLAG_LANGUAGE=0x2000,
	//

	MM_UINFOFLAG_ALL=( MM_UINFOFLAG_NICKNAME | MM_UINFOFLAG_BINDUIN | MM_UINFOFLAG_BINDEMAIL | 
			MM_UINFOFLAG_BINDMOBILE | MM_UINFOFLAG_STATUS | MM_UINFOFLAG_IMG | MM_UINFOFLAG_PERSONALCARD |
			MM_UINFOFLAG_BOTTLEIMG |  MM_UINFOFLAG_HDHEADIMGMD5SUM | MM_UINFOFLAG_BOTTLEHDHEADIMGMD5SUM | 
			//add by yihuang
			MM_UINFOFLAG_LBS |
			MM_UINFOFLAG_LANGUAGE |
			//
			MM_UINFOFLAG_PLUGINFLAG  ),
};

enum enMMContactFlag
{
	MM_CONTACTFLAG_CONTACT=0x01,
	MM_CONTACTFLAG_CHATCONTACT=0x02,
	MM_CONTACTFLAG_CHATROOMCONTACT=0x04,
	MM_CONTACTFLAG_BLACKLISTCONTACT=0x08,
	MM_CONTACTFLAG_DOMAINCONTACT=0x10,
	MM_CONTACTFLAG_HIDECONTACT=0x20,
	MM_CONTACTFLAG_FAVOURCONTACT=0x40,
	MM_CONTACTFLAG_3RDAPPCONTACT=0x80,
	MM_CONTACTFLAG_SNSBLACKLISTCONTACT=0x100,
	MM_CONTACTFLAG_MUTECONTACT=0x200,

	MM_CONTACTFLAG_ALL=(MM_CONTACTFLAG_CONTACT | MM_CONTACTFLAG_CHATCONTACT | MM_CONTACTFLAG_CHATROOMCONTACT |
			MM_CONTACTFLAG_BLACKLISTCONTACT | MM_CONTACTFLAG_DOMAINCONTACT | MM_CONTACTFLAG_HIDECONTACT |
			MM_CONTACTFLAG_FAVOURCONTACT | MM_CONTACTFLAG_3RDAPPCONTACT | MM_CONTACTFLAG_SNSBLACKLISTCONTACT |
            MM_CONTACTFLAG_MUTECONTACT),
};

enum enMMLbsMatchType
{
	MM_LBSMATCHTYPE_MATCH = 1,	// 已经配对成功
	MM_LBSMATCHTYPE_LIKE = 2,	// 有意思
	MM_LBSMATCHTYPE_NONE= 3,
};

enum enMMNotifyStatus
{
	MM_NOTIFY_CLOSE=0,
	MM_NOTIFY_OPEN=1,
};

enum enMMContactImgFlag
{
	MM_CONTACTIMGFLAG_NOTMODIFY= 1,
	MM_CONTACTIMGFLAG_MODIFY = 2,
	MM_CONTACTIMGFLAG_HAS_HEADIMG = 3,
	MM_CONTACTIMGFLAG_HAS_NO_HEADIMG = 4,
};

enum enMMBottleContactImgFlag
{
	MM_BOTTLECONTACTFLAG_HAS_NO_HDIMG = 0,
	MM_BOTTLECONTACTFLAG_HAS_HDIMG = 1,
};

enum enMMContinueFlag
{
	MM_SYNC_END=0,
	MM_SYNC_CONTINUE=1,
};

enum enMMInitContinueFlag
{
	MM_INIT_END=0,
	MM_INIT_CONTINUE=1,
};

enum enMMCookieId
{
	MMC_AUTH_KEY = 0,
	MMC_BAD_USER = 1, 
	MMC_BIND_VERIFYINFO = 2,
	MMC_BIND_CLIENTIP= 3,
	MMC_AUTH_TIME = 4,
	MMC_PLUGIN_SHAKETRANIMG_SID = 5,
	MMC_PLUGIN_SHAKETRANIMG_ACTIVETIME = 6,
	MMC_PLUGIN_SHAKETRANING_BOOKMARK_SID = 7,
	MMC_DYE_EXPIRE_TIME = 8, /**< 染色截止时间 */
	MMC_DYE_LOG_LEVEL = 9,   /**< 染色日志级别 */
	MMC_USER_IMEI = 10,
	MMC_BLOCK_OPER = 11, /**block用户某功能**/
	MMC_CLIENTCHECK_ENABLE = 12, // clientcheck switch
	MMC_CLIENTCHECK_SUPPORT = 13, // clientcheck support
	MMC_ACCOUNT_TYPE = 14,
};

enum enBlockOper
{
	MMBLOCK_TXT_SINGLE = 0x1,     //文本
	MMBLOCK_TXT_CHATROOM = 0x2,
	MMBLOCK_IMG_SINGLE = 0x4,     //图片
	MMBLOCK_IMG_CHATROOM = 0x8,
	MMBLOCK_VOICE_SINGLE = 0x10,  //语音
	MMBLOCK_VOICE_CHATROOM = 0x20, 
	MMBLOCK_VIDEO_SINGLE = 0x40,  //视频
	MMBLOCK_VIDEO_CHATROOM = 0x80, 
	MMBLOCK_APPMSG_SINGLE = 0x100, //appmsg
	MMBLOCK_APPMSG_CHATROOM = 0x200,
	MMBLOCK_EMOJI_SINGLE = 0x400, //emoji
	MMBLOCK_EMOJI_CHATROOM = 0x800,
	MMBLOCK_SNS_POST = 0x1000, //发图
	MMBLOCK_SNS_COMMENT = 0x2000, //评论
	MMBLOCK_MASSSEND = 0x4000, //群发
};

enum enMMVoiceEndFlag
{
	//还在上传状态
	MM_VOICE_UPLOADING = 0,
	//语音上传结束
	MM_VOICE_UPLOADEND = 1
};

enum enMMVoiceCancelFlag
{
	//还在上传状态
	MM_VOICE_UPLOAD_CONTINUE = 0,
	//语音上传取消
	MM_VOICE_UPLOAD_CANCEL = 1
};

enum enMMVoiceForwardFlag
{
	//自己发的
	MM_VOICE_UPLOAD_ORIGINAL = 0,
	//转发别人的
	MM_VOICE_UPLOAD_FORWARD = 1
};

//remove by adrianli
//enum enMMVoiceRemindEndFlag
//{
//	MM_VOICEREMIND_UPLOAD_NULL = 0,
//	MM_VOICEREMIND_UPLOAD_ING = 1,
//	MM_VOICEREMIND_UPLOAD_END = 2,
//    MM_VOICEREMIND_UPLOAD_CANCEL = 3,
//};

enum enMMVoiceRecognizeEndFlag
{
	MM_VOICE_REC_UPLOADING = 0,
	MM_VOICE_REC_UPLOADEND = 1,
	MM_VOICE_REC_UPLOADCANCEL = 2,
};

enum enMMVoiceStatus
{
	MM_VOICE_STATUS_UNKNOWN = 0,
	MM_VOICE_STATUS_PIECE= 1,
	MM_VOICE_STATUS_FULL = 2,
};

enum enSendCardType
{
	MM_SENDCARD_QQBLOG= 0x01, 
	MM_SENDCARD_QQSIGN= 0x02, 
	MM_SENDCARD_QZONE= 0x04, 
	MM_SENDCARD_QQFRIEND= 0x08, 
	MM_SENDCARD_SINABLOG= 0x10, 
	MM_SENDCARD_MODIFYHEADIMG= 0x20, 
	MM_SENDCARD_READER = 0x40, 
	MM_SENDCARD_FACEBOOK = 0x80,
};

enum enBindOpMobileOpCode
{
	MM_BINDMOBILE_READY = 1,
	MM_BINDMOBILE_CHECK = 2,
	MM_UNBINDMOBILE = 3,
	MM_BINDMOBILE_CHECKED = 4,
	MM_BINDMOBILE_REG_READY = 5,
	MM_BINDMOBILE_REG_CHECK = 6,
	MM_BINDMOBILE_REG_CHECKED = 7,
	MM_BINDMOBILE_LOGIN_READY = 8,
	MM_BINDMOBILE_LOGIN_CHECK = 9,
	MM_BINDMOBILE_VERIFY_READY = 10,
	MM_BINDMOBILE_VERIFY_CHECK = 11,
};



enum enImportFriendsOpCode
{
	MM_IMPORT_AUTOADD = 1,
	MM_IMPORT_RECOMMEND = 2,
};

enum enImportFriendsSource
{
	MM_IMPORT_FACEBOOK_FRIENDS = 1,
};

enum enUpdateMContactOpCode
{
	MM_UPDATEMCONTACT_ADD = 1,
	MM_UPDATEMCONTACT_DEL= 2,
};

enum enVerifyUserOpcode
{
	MM_VERIFYUSER_ADDCONTACT = 1,
	MM_VERIFYUSER_SENDREQUEST = 2,
	MM_VERIFYUSER_VERIFYOK = 3,
	MM_VERIFYUSER_VERIFYREJECT = 4,
	MM_VERIFYUSER_SENDERREPLY = 5,
	MM_VERIFYUSER_RECVERREPLY = 6,
};

enum enVerifyChatRoomMemberOpcode
{
	MM_VERIFYMEMBER_SENDREQUEST = 1,
	MM_VERIFYMEMBER_VERIFYOK = 2,
	MM_VERIFYMEMBER_VERIFYREJECT = 3,
};

enum enInviteChatRoomMemberOpcode
{
	MM_INVITEMEMBER_SENDREQUEST = 1,
	MM_INVITEMEMBER_INVITEOK = 2,
	MM_INVITEMEMBER_INVITEREJECT = 3,
};


enum enBuiltinIPType
{
	MM_BUILTINIPTYPE_WIFI =  1 << 0,
	MM_BUILTINIPTYPE_EDGE =  1 << 1,
	MM_BUILTINIPTYPE_YIDONG = 1 << 2,
	MM_BUILTINIPTYPE_LIANTONG = 1 << 3,
	MM_BUILTINIPTYPE_DIANXIN = 1<< 4
};

enum enQQGroupOpType
{
	MM_OP_GROUP = 0,
	MM_OP_FRIEND = 1,
	MM_OP_ALL_GROUP = 2,
	MM_OP_ALL_FRIEND = 3,
};

enum enQQFriendStatus
{
	MM_STATUS_WEIXIN_OFF = 0,
	MM_STATUS_WEIXIN_ON = 1,
	MM_STATUS_WEXIN_FRIEND= 2,
};

enum enBindQQOPCode
{
	MM_BQOC_BINDQQ = 1,
	MM_BQOC_GETVERIFYIMG = 2,
	MM_BQOC_GENKEY = 3,
};

enum enShakeTranImgRet
{
	MM_SHAKE_TRAN_IMG_OK = 0,
	MM_SHAKE_TRAN_IMG_ERR_SYS = -1,
	MM_SHAKE_TRAN_IMG_ERR_ARG = -2,
	MM_SHAKE_TRAN_IMG_ERR_TICKET = -200,
	MM_SHAKE_TRAN_IMG_ERR_TOKEN = -201,
	MM_SHAKE_TRAN_IMG_ERR_SID = -202,

};

enum enShakeTranImgNotifyBit
{
	MM_SHAKE_TRAN_IMG_NOTIFY_SHAKE = 0x1,
	MM_SHAKE_TRAN_IMG_NOTIFY_BIND = 0x2,
	MM_SHAKE_TRAN_IMG_NOTIFY_ACTIVE = 0x4,

};

enum enShakeTranImgOpType
{
	MM_SHAKE_TRAN_IMG_UNBIND = 1,
	MM_SHAKE_TRAN_IMG_INACTIVE = 2,

};

enum enShakeTranImgScene
{
	MM_SHAKE_TRAN_IMG_SCENE_NORMAL = 0,
	MM_SHAKE_TRAN_IMG_SCENE_SHARE = 1,
	MM_SHAKE_TRAN_IMG_SCENE_BOOKMARK = 2,

};

enum enShareCardOpCode
{
	MM_SHARE_CARD_ADD = 1,
	MM_SHARE_CARD_CHECK = 2,
};

enum enShareCardRetCode
{
	MM_SHARE_CARD_OK = 0,
	MM_SHARE_CARD_LOGIC_ERR = 1,
};

enum enSnsTagOptionOpCode
{
	MM_SNS_TAG_ADD = 1,
	MM_SNS_TAG_MOD = 2,
	MM_SNS_TAG_DEL = 3,
};

enum enSnsTagMemOptionOpCode
{
	MM_SNS_TAG_MEM_ADD = 1,
	MM_SNS_TAG_MEM_DEL = 2,
	MM_SNS_TAG_MEM_MOD = 3,
};

enum enSnsTagListOpCode
{
	MM_SNS_TAG_LIST_GET = 1,
	MM_SNS_TAG_LIST_RECOMMEND = 2,
};

enum enSnsTagDefaultId
{
	MM_SNS_TAG_ID_FRIEND = 0X1,//朋友
	MM_SNS_TAG_ID_COLLEAGUE = 0x2,//同事
	MM_SNS_TAG_ID_CLASSMATE = 0x3,//同学
	MM_SNS_TAG_ID_OUTSIDERS = 0x4,//圈外人
	MM_SNS_TAG_ID_BLACKLIST = 0x5,//黑名单
	MM_SNS_TAG_ID_OTHERS = 0x6,//自定义 >= 0x6
};

enum enResetPawType
{
	MM_RESETPWD_MOBILE = 0,
	MM_RESETPWD_EMAIL = 1,
};

enum enResetErr
{
	MM_RESETPWD_SYSERR = -1,
	MM_RESETPWD_OK = 0,
	MM_RESETPWD_USERNAMEINVALID = 1,
	MM_RESETPWD_TIMEOUT = 2,
	MM_RESETPWD_HASRESETED = 3,
	MM_RESETPWD_KEYNOTFOUND = 4,
	MM_RESETPWD_TYPEERR = 5,
	MM_RESETPWD_PWDERR = 6,
	MM_RESETPWD_FREQERR = 7,
	MM_RESETPWD_MOBILEINVALID = 8,
	MM_RESETPWD_PWDINVALID = 9
};

enum enLbsOpCode
{
	MM_LBS_FIND = 1,
	MM_LBS_REMOVE = 2,
	MM_LBS_FINDGG = 3,
	MM_LBS_FINDMM = 4,
};

enum enLbsMatchOpCode
{
	MM_LBSMatch_Match = 1,
	MM_LBSMatch_REMOVE = 2,
};


enum enLbsState
{
	MM_LBS_ONLINE = 0x1,
};

enum enDirectSendType
{
	MM_ALBUMLBS_PUSHTYPE = 0x4,
	MM_SNSLBS_PUSHTYPE = 0x5,
	MM_ChatStatus = 111, // 发送或取消聊天状态
};

enum enLbsGPSSource
{
	MM_SOURCE_HARDWARE = 0,
	MM_SOURCE_NET = 1,
};
enum enVideoNetworkEnv
{
	MM_VIDEO_NETWORK_WIFI = 1,	
	MM_VIDEO_NETWORK_OTHER = 2,
};
enum enVideoCameraType
{
	MM_VIDEO_CAMERA_FRONT = 1,
	MM_VIDEO_CAMERA_REAR = 2,
};
enum enVideoFuncFlag
{
	MM_VIDEO_FUNCFLAG_SELF = 1,
	MM_VIDEO_FUNCFLAG_EXPORT = 2,
};

enum enUpdateOpCode
{
	MM_UPDATE_ALERT_YES = 0x1,
	MM_UPDATE_ALERT_NO = 0x2,
	MM_UPDATE_NEW_CLICK = 0x4,
	MM_UPDATE_UPDATE_CLICK = 0x8,
	MM_UPDATE_CHECK_UPDATE = 0x10,
};

enum enGroupCardOpCode
{
	MM_GROUPCARD_ADD = 1,
	MM_GROUPCARD_UPDATENICK = 2,
};

enum enBottleType
{
	MM_BOTTLE_TYPE_NORMAL = 0
};

enum enBottleErr
{
	MM_BOTTLE_ERR_UNKNOWNTYPE = 15,
	MM_BOTTLE_COUNT_ERR = 16,
	MM_BOTTLE_NOTEXIT = 17,
	MM_BOTTLE_UINNOTMATCH = 18,
	MM_BOTTLE_PICKCOUNTINVALID = 19
};

//bottle type
enum enBottleDataType
{
	MM_BOTTLE_TEXT = 1,
	MM_BOTTLE_IMG = 2,
	MM_BOTTLE_VOICE = 3,
	MM_BOTTLE_VIDEO = 4,
};

// to weibo msg type
enum enToWeiboType
{
	MM_TOWEIBO_TYPE_TEXT = 1,
	MM_TOWEIBO_TYPE_IMG = 2,
	MM_TOWEIBO_TYPE_VOICE = 3,
	MM_TOWEIBO_TYPE_VIDEO = 4,
};

// hd head img type
enum enHDHeadImgType
{
	MM_HD_HEADIMG_WEIXIN = 1,
	MM_HD_HEADIMG_BOTTLE = 2,
	MM_HD_HEADIMG_SUBUSER = 3,
};

enum enKVStatKey
{   
	//收取耗时
	MM_GETDATA_TIMECOST = 1, 
	//wifi下 无连接的次数 
	MM_DISCONNECT_WIFI = 2, 
	//非wifi下 无连接的次数 
	MM_DISCONNECT_NOTWIFI = 3,
	MM_IPXX = 4,
	//3¤á??ó′ó?μ ±3￠ê?±??ˉ?a1 ??óò?′?3￠ê? μ?′?êy
	MM_LINKCHANGE_COUNT = 5, 
	//
	MM_QQSYNC_OPER = 6,
	//
	MM_ANDROID_REQ_TIMECOST = 7,
	//
	MM_IPHONE_CRASH_INFO = 8,
	// 背景使用 
	MM_BACKGROUND_USED = 9,
	// 点击好友推荐卡片的场景次数统计 
	MM_CLICK_FRIEND_SCENE = 10,
	// 塞班请求耗时
	MM_SYMBIAN_REQ_TIMECOST = 11,

	MM_SEND_PIC_2_QZONE = 12,

	MM_CRASHREPORT_ONCE = 13,

	MM_CRASHREPORT_MORE = 14,
	//分享地理位置通过google查看详细位置
	MM_SHARELOCATION_DETAIL_GOOGLE= 15,
	//分享地理位置通过其他查看详细位置
	MM_SHARELOCATION_DETAIL_OTHER= 16,
	//新年彩蛋统计
	MM_NEWYEAR_EGG_SEND = 17, 

	MM_NEWYEAR_EGG_RECV = 18, 

	//微信是否切到前台
	MM_WEIXIN_SHOW = 19,

	MM_SYMBIAN_REQ_ERR = 20,

	MM_DEVICE_PLATFORM = 21,

	// 用户主动反馈（目前只有 sybiam 用到）
	MM_USER_FEEDBACK = 22,

	MM_MEDIA_TRANSFER = 23, /**< 图片视频支持转发 （转发次数、人数） */
	MM_SHOW_PWD_PLAIN_TXT = 24, /**< 输入密码时可以选择显示明文 （选择显示明文的人数、次数） */
	MM_ADJUST_FONT_SIZE = 25, /**< 在设置中添加调整字体大小的选项 （每个字体号的选择人数） */
	MM_QQ_MOBILE_ADD_FRIEND = 26, /**< QQ好友列表、手机通讯录好友列表里快捷添加好友按钮 （点击此按钮的人数） */
	MM_APPMSG_SWITCH = 27,    /* 由微信进入第3方APP发送消息 */
	// sns
	MM_SNS_CHECK_BIGPIC = 28, //sns查看大图
	MM_SNS_CHECK_SELFPAGE = 29, //sns查看自己相册
	MM_QQMSG_CLICK = 30, //发起QQ离线，每次点击QQ离线消息按钮发送一个

	//facebook
	MM_FACEBOOK_CONNECT = 31, //点击facebook Connect的用户数据
	MM_FACEBOOK_LINK = 32, //点击facebook link的用户数据
	MM_FACEBOOK_INVITE = 33, //点击facebook邀请的次数

	// symbian专用
	MM_APP_START = 34, // 程序启动上报
	MM_APP_AIDREPORT = 35,	// 用户上报应急操作

	MM_APP_UPDATE_STATE = 36, // 程序升级状态（成功/失败）
	MM_APP_EMERGENCY_TRIGGER = 37, // 程序应急启动

	MM_APPMSG2WEIXIN = 38, //第3方appmsg发送到微信的消息
	//larger than 10000, user new attrlog for report
	MM_GETLBSIMGFROMCDN = 10073,
	MM_GETSNSIMGFROMCDN = 10074,
	MM_GETSNSHDIMGFROMCDN = 10075,
	MM_QQMAILCLICKCOUNT = 10071,
	MM_EMBEDMUSICPLAYCOUNT = 10090,
	MM_LANGUAGE_DOWNLOAD_TIMECOST = 10095,
	MM_MAINFRAMEQRCODE = 10111,
	MM_MAINFRAMEWEBWX = 10110, 
	MM_MAINFRAMEEAR =	10109, 
	MM_MAINFRAMECREATECHAT = 10108,
	MM_COPYPHONENUMBER = 10115, 
	MM_ADDEXISTCONTACTPHONENUMBER = 10114,
	MM_ADDNEWCONTACTPHONENUMBER = 10113, 
	MM_CALLPHONENUMBERINMESSAGE = 10112,  
	MM_APPINSTALL = 10165,
	MM_VOIPTENMINUTESLOGICREPORT = 10166,
	MM_CHATCREATECHATROOMCLICK = 10170,
	MM_CHATROOMADDCONTACTCLICK = 10169,
	MM_CONTACTLISTCREATECHATCLICK = 10168, 

	MM_RECEIVERINCITATIONCOLOREGG = 10182,
	MM_SENDERINCITATIONCOLOREGG = 10181,
};  

enum enAppMsgSouce
{
	MM_APPMSG_WEIXIN = 1,
	MM_APPMSG_3RD = 2,
	MM_APPMSG_FORWARD = 3,
};

enum enShakeRet
{
	MM_SHAKE_DELETE = 2,
	MM_SHAKE_WAIT = 1,
	MM_SHAKE_OK = 0,
	MM_SHAKE_ERR = -1,
	MM_SHAKE_TIME_OUT = -2,
};

enum enShakeOpCode
{
	MM_SHAKE_MATCH = 0,
	MM_SHAKE_MATCH_DELETE = 1,
};

enum enPluginInitFlag
{
	//QQ邮箱
	MM_PlUGIN_QQMAIL= 0x1,
	//私信
	MM_PLUGIN_PM = 0x2,
	//好友推荐助手
	MM_PLUGIN_FM= 0x4,
	//微博发图助手
	MM_PLUGIN_WEIBO= 0x8,
	//语音记事本
	MM_PLUGIN_MEDIANOTE= 0x10,
	//QQ离线消息
	MM_PLUGIN_QMSG= 0x20,
	//漂流瓶
	MM_PLUGIN_BOTTLE= 0x40,
	//QQ同步助手
	MM_PLUGIN_QSYNC= 0x80,
	//撸一撸
	MM_PLUGIN_SHAKE= 0x100,
	//lbs
	MM_PLUGIN_LBS= 0x200,
	//
	MM_PLUGIN_BOTTLE_CHART= 0x400,
	// Gmail
	MM_PLUGIN_GMAIL= 0x800,
	// 查看QQ好友
	MM_PLUGIN_CHECKQQF= 0x1000,
	// FaceBook卸载标志
	MM_PLUGIN_FACEBOOK = 0x2000,
	// 每日新闻插件
	MM_PLUGIN_READERAPP = 0x4000,
	// 个人相册
	MM_PLUGIN_FEEDSAPP = 0x8000,
	// 群发
	MM_PlUGIN_MASSSEND = 0x10000,
	// 美食
	MM_PlUGIN_MEISHI = 0x20000,
	// 微博精选 
	MM_PlUGIN_BLOGAPP = 0x40000,
	// newsapp插件 
	MM_PLUGIN_NEWSAPP = 0x80000,
	//voip
	MM_PLUGIN_VOIP = 0x100000,
    // voice input
    MM_PLUGIN_VOICEINPUT = 0x200000,    
	//voip
	MM_PLUGIN_VOICEVOIP = 0x400000,
};

enum enPluginFlag
{
	//QQ邮箱卸载标志 
	MM_QQMAIL_UNINSTALL = 0x1,
	//私信卸载标志
	MM_PM_UNINSTALL = 0x2,
	//好友推荐助手卸载标志
	MM_FM_UNINSTALL = 0x4,
	//微博发图助手卸载标志
	MM_WEIBO_UNINSTALL = 0x8,
	//语音记事本卸载曛?
	MM_MEDIANOTE_UNINSTALL = 0x10,
	//QQ离线消息卸载标志
	MM_QMSG_UNINSTALL = 0x20,
	//漂流瓶卸载标志
	MM_BOTTLE_UNINSTALL = 0x40,
	//QQ同步助手卸载标志
	MM_QSYNC_UNISTALL = 0x80,
	//撸一撸卸载标志
	MM_SHAKE_UNISTALL = 0x100,
	//lbs卸载标志 
	MM_LBS_UNISTALL = 0x200,
	//
	MM_BOTTLE_CHART_INSTALL = 0x400,
	// Gmail卸载标志
	MM_GMAIL_UNINSTALL = 0x800,
	// 查看QQ好友卸载标志
	MM_CHECKQQF_UNINSTALL= 0x1000,
	// FaceBook卸载标志
	MM_FACEBOOK_UNINSTALL= 0x2000,
	// Reader卸载标志
	MM_READERAPP_UNINSTALL= 0x4000,
	// 个人相册
	MM_FEEDSAPP_UNINSTALL = 0x8000,
	// 群发
	MM_MASSSEND_UNINSTALL = 0x10000,
	// 美食
	MM_MEISHI_UNINSTALL = 0x20000,
	// 微博精选推选 
	MM_BLOG_UNINSTALL = 0x40000,
	// newsapp插件 
	MM_NEWSAPP_UNINSTALL = 0x80000,
	// voip
	MM_VOIP_UNINSTALL = 0x100000,
    // voice input
	MM_VOICEINPUT_UNINSTALL = 0x200000,
	// voice for voip
	MM_VOICEVOIP_UNINSTALL = 0x400000,

};

// 举报的场景定义
enum enExposeScene
{
	MM_EXPOSE_LBS_SEARCH = 1,		// 主动LBS搜到后举报
	MM_EXPOSE_LBS_RCVGREET = 2,		// 收到LBS的打招呼后举报
	MM_EXPOSE_SHAKE_SEARCH = 3,		// 主动摇一摇后举报
	MM_EXPOSE_SHAKE_RCVGREET = 4,	// 收到摇一摇的打招呼后举报
	MM_EXPOSE_BOTTLE_PICK = 5,		// 收到漂流瓶后举报
	MM_EXPOSE_BOTTLE_RCVGREET = 6,	// 收到漂流瓶来的打招呼后举报
	MM_EXPOSE_QRCODE_RCVGREET = 7,	// 收到二维码打招呼后举报

	//因为之前的客户端有bug，会上传8-30之间的脏数据，所以从31开始

	MM_EXPOSE_ALBUM_PHOTO = 31,	//  举报个人空间的照片 
	MM_EXPOSE_ALBUM_SIGN = 32,	// 举报个人空间的签名 

	MM_EXPOSE_SNS = 33, //举报个人动态
};

// 认证用户的类型定义
enum enVUserCategory
{
	MM_VUSER_BUSINESS = 0x1,  // 商家
	MM_VUSER_CELEBRITY = 0x2, // 名人
};

enum enPublishAssistant
{
	MM_PUBLISH_WEIBO	= 0x1,
	MM_PUBLISH_SHUOSHUO = 0x2,
};

enum enMMVoipRetCode
{
	MM_VOIP_RET_SESSION_USER_BUSY = 211,
	MM_VOIP_RET_SESSION_TIME_OUT = 212,
	MM_VOIP_RET_SESSION_ROOMID_CHECK_FAIL = 213,
	MM_VOIP_RET_SESSION_NOT_EXIST = 214,
	MM_VOIP_RET_ROOMINDEX_MEMBER_NOT_IN = 221,
	MM_VOIP_RET_ROOMINDEX_ROOM_NOT_EXIST = 222,

	MM_VOIP_RET_SVR_LOGIC_ERROR = 231,
	MM_VOIP_RET_REDIS_ERROR = 232,
	MM_VOIP_RET_CHECKCONTACT_ERROR = 233,

	MM_VOIP_RET_ARG_ERR = -2,

	MM_VOIP_RET_NET_NOTSUPPORT_ERR = 234,

	MM_VOIP_RET_USER_NOTSUPPORTED = 235,

	MM_VOIP_RET_USER_IN_BLACKLIST= 236,
	MM_VOIP_RET_USER_PLUGINCLOSE = 237,
};

enum enMMVoipType
{
	MM_VOIP_INVITE_VIDEO = 0,
	MM_VOIP_INVITE_VOICE = 1,
};

enum enMMVoipInviteCallType
{
	MM_VOIP_INVITE_CALL_TYPE_INVITE = 0,
	MM_VOIP_INVITE_CALL_TYPE_REINVITE = 1,
};

enum enMMVoipAnswerType
{
	MM_VOIP_ANSWER_ACCEPT = 1,
	MM_VOIP_ANSWER_REJECT = 2,
};

enum enMMVoipInviteType
{
	MM_VOIP_INVITE_INVITE = 1,
	MM_VOIP_INVITE_CANCELINVITE = 2,
	MM_VIOP_INVITE_REMIDE = 3,
	MM_VIOP_INVITE_SHUT_DOWM = 4,
};


enum enMMVoipSyncUserStatus
{
	MM_VOIP_SYNC_STATUS_STATUS_NEEDSYNC = 0,
	MM_VOIP_SYNC_STATUS_ACCEPT = 1,
	MM_VOIP_SYNC_STATUS_REJECT = 2,
	MM_VOIP_SYNC_STATUS_NORESP = 3,
	MM_VOIP_SYNC_STATUS_SHUTDOWN = 4,
	MM_VOIP_SYNC_STATUS_ROOMCREATER= 5,
	MM_VOIP_SYNC_STATUS_ACKED = 6,
};

enum enMMVoipSyncCmd
{
	MM_VOIP_SYNCCMD_STATUS = 1,
	MM_VOIP_SYNCCMD_RELAYDATA = 2,
	MM_VOIP_SYNCKEY_CONNECTING_STATUS = 3,
};

enum enMMVoipSelector
{
	MM_VOIP_SYNC_SELECTOR_STATUS = 0x1,
	MM_VOIP_SYNC_SELECTOR_RELAYDATA = 0x2,
	MM_VOIP_SYNC_SELECTOR_CONNECTING_STATUS = 0x4,
};

enum enMMVoipPushType
{
	MM_VOIP_PUSHTYPE_INVITE = 1,
	MM_VOIP_PUSHTYPE_CLIENT_DEFINE = 100,
};

enum enMMVoipRelayDataType
{
	MM_VOIP_RELAY_DATA_NORMAL = 1,
	MM_VOIP_RELAY_DATA_PEERID = 2,
	MM_VOIP_RELAY_DATA_CAPINFO = 3,
	MM_VOIP_RELAY_SENDING_TYPE = 4,
	MM_VOIP_RELAY_CONNECT_DATA = 5,
};

enum enMMVoipSendingType
{
	MM_VOIP_SENDINGTYPE_RELAY_ONLY = 1,
	MM_VOIP_SENDINGTYPE_AUTO_SELECTION = 2,
	MM_VOIP_SENDINGTYPE_DC_FIRST = 4,
	MM_VOIP_SENDINGTYPE_DC_ONLY = 8,
	MM_VOIP_SENDINGTYPE_RELAY_FIRST = 16

};

enum enMMVoipSyncKeyType
{
	MM_VOIP_SYNCKEY_STATUS = 1,
	MM_VOIP_SYNCKEY_RELAYDATA = 2
};

enum enMMVoipNetType
{
	MM_VOIP_NETTYPE_NULL = 0,
	MM_VOIP_NETTYPE_2G = 1,
	MM_VOIP_NETTYPE_NOTWIFI = 2,
	MM_VOIP_NETTYPE_3G = 3,
	MM_VOIP_NETTYPE_WIFI = 4,
};

enum enMMVoipDeviceInfo
{
	MM_VOIP_NEED_SPAN = 0x1,
	MM_VOIP_CPU_UNUSUAL = 0x2,
};

enum enMMTalkMicActionType
{
	MM_TALKMIC_ACTION_SEIZE = 1,
	MM_TALKMIC_ACTION_PUTAWAY= 2,
};

enum enMMTalkSysMsgType
{
	MM_TALKSYSMSG_ENTERROOM = 1,
	MM_TALKSYSMSG_EXITROOM = 2,
	MM_TALKSYSMSG_CREATEROOM = 3,
	MM_TALKSYSMSG_ADDMEMBER = 4,
	MM_TALKSYSMSG_DELMEMBER = 5,
	MM_TALKSYSMSG_INVITE = 6,
};

enum enMMTalkRetCode
{
	MM_TALK_RET_TALKINFO_USER_BUSY = 311,
	MM_TALK_RET_TALKINFO_TIME_OUT = 312,
	MM_TALK_RET_TALKINFO_ROOMID_CHECK_FAIL = 313,
	MM_TALK_RET_TALKINFO_NOT_EXIST = 314,
	MM_TALK_RET_ROOMINDEX_MEMBER_NOT_IN = 321,
	MM_TALK_RET_ROOMINDEX_ROOM_NOT_EXIST = 322,

	MM_TALK_RET_SVR_LOGIC_ERROR = 331,
	MM_TALK_RET_REDIS_ERROR = 332,
	MM_TALK_RET_CHECKCONTACT_ERROR = 333,

	MM_TALK_RET_ARG_ERR = -2,

	MM_TALK_RET_NET_NOTSUPPORT_ERR = 334,

	MM_TALK_RET_USER_NOTSUPPORTED = 335,

	MM_TALK_RET_USER_IN_BLACKLIST= 336,
	MM_TALK_RET_USER_PLUGINCLOSE = 337,

	MM_TALK_RET_TALKMIC_SEIZE_FAIL = 338,
	MM_TALK_RET_TALKMIC_PUTAWAY_NONEED = 339,
};

enum enMMTalkSyncUserStatus
{
	MM_TALK_SYNC_STATUS_STATUS_NEEDSYNC = 0,
	MM_TALK_SYNC_STATUS_ACCEPT = 1,
	MM_TALK_SYNC_STATUS_REJECT = 2,
	MM_TALK_SYNC_STATUS_NORESP = 3,
	MM_TALK_SYNC_STATUS_SHUTDOWN = 4,
	MM_TALK_SYNC_STATUS_ROOMCREATER= 5,
	MM_TALK_SYNC_STATUS_ACKED = 6,
};

enum enMMComposeSendType
{
	MM_COMPOSE_SEND_TYPE_REPLY = 1,
	MM_COMPOSE_SEND_TYPE_FORWARD = 2,
};

enum enMMEmojiType
{
	MM_EMOJI_TYPE_PNG = 1,
	MM_EMOJI_TYPE_GIF = 2,
	MM_EMOJI_TYPE_JPG = 3,
	MM_APPEMOJI_TYPE_GIF = 4,
	MM_APPEMOJI_TYPE_JPG = 5,
};

enum enMMEmojiGameType
{
	MM_EMOJI_NOTGAME = 0,
	MM_EMOJI_JSB = 1,
	MM_EMOJI_DICE = 2,

};

enum enMMEmojiGameContent
{
	MM_EMOJIGame_CONTENT_J = 1,
	MM_EMOJIGame_CONTENT_S = 2,
	MM_EMOJIGame_CONTENT_B = 3,
	MM_EMOJIGame_CONTENT_1 = 4,
	MM_EMOJIGame_CONTENT_2 = 5,
	MM_EMOJIGame_CONTENT_3 = 6,
	MM_EMOJIGame_CONTENT_4 = 7,
	MM_EMOJIGame_CONTENT_5 = 8,
	MM_EMOJIGame_CONTENT_6 = 9,
};

enum enMsgEmojiFlag
{
	MM_MSG_EMOJI_QQ = 0x1,
	MM_MSG_EMOJI_EMOJI = 0x2,
	MM_MSG_EMOJI_ART = 0x4,
};

enum emMMPackageType
{
	MM_PACKAGE_THEME = 0,
	MM_PACKAGE_SESSION_BACKGROUND = 1,
	MM_PACKAGE_EMOJIART = 2,
	MM_PACKAGE_RESOURCE = 3,
	MM_PACKAGE_ALBUM = 4,
	MM_PACKAGE_MMSTRINGS = 5,
	MM_PACKAGE_EASTEREGG = 6,
	MM_PACKAGE_CONFIGLIST = 7,
	MM_PACKAGE_REGIONCODE = 8,
	MM_PACKAGE_PATTERN = 9,
	MM_PACKAGE_FESTIVAL = 10,
};

enum enMMUserattrFlag
{
	MM_USERATTR_WEIBO_SHOW	 = 0x1,
	MM_USERATTR_WEIBO_VERIFY = 0x2,
	MM_USERATTR_WEIBO_ICON = 0x4,
};

enum emMMFaceBookInfo
{
	MM_FACEBOOK_UNBIND = 0,
	MM_FACEBOOK_BIND = 1,
	MM_FACEBOOK_UPDATE_TOKEN = 2,
};

enum emMMGetFriend
{
	MM_MOBILE_FRIEND = 0,
	MM_FACEBOOK_FRIEND = 1,
};

enum emMMModAlbumBitFlag
{
	MM_ALBUMINFOFLAG_ALBUMSTYLE =0x1,
	MM_ALBUMINFOFLAG_ALBUMFLAG =0x2,
	MM_ALBUMINFOFLAG_ALBUMBGIMGID =0x4,
	MM_ALBUMINFO_ALL = ( MM_ALBUMINFOFLAG_ALBUMSTYLE | MM_ALBUMINFOFLAG_ALBUMFLAG | MM_ALBUMINFOFLAG_ALBUMBGIMGID ),
};

enum enMMAlbumType
{
	MM_ALBUM_PHOTO = 1, //照片
	MM_ALBUM_SIGN = 2, //签名
	MM_ALBUM_BGIMG_UPLOAD = 4, //背景图片
	MM_ALBUM_BGIMG_INTL_UPLOAD = 5, //推送到别人timeline的背景图片
};

enum emMMUserAttrVerifyFlag
{
	MM_USERATTRVERIFYFALG_BIZ = 0x1, // 小商家
	MM_USERATTRVERIFYFALG_FAMOUS = 0x2,
	MM_USERATTRVERIFYFALG_BIZ_BIG = 0x4, // 大商家
	MM_USERATTRVERIFYFALG_BIZ_BRAND= 0x8, // 品牌商家
	MM_USERATTRVERIFYFALG_VERIFY = 0x10, // 认证
};

enum enAlbumPhotoType
{
	MM_ALBUM_PHOTO_SMALL = 1, //小图 
	MM_ALBUM_PHOTO_MIDDLE = 2, //中图
	MM_ALBUM_PHOTO_BIG = 3, //大图
	MM_ALBUM_BGIMG = 4, //背景图片
	MM_ALBUM_BGIMG_INTL = 5, //推送到别人timeline的背景图片
};

enum enAlbumCommentType
{
	MM_ALBUM_COMMENT_LIKE = 1, //标红心
	MM_ALBUM_COMMENT_WORD = 2, //评论
	MM_ALBUM_COMMENT_REPLY = 3, //回复评论
	MM_ALBUM_COMMENT_WITH = 4, //提到的人
	MM_ALBUM_COMMENT_LIKE_BGIMG = 5, //赞背景
};

enum enAlbumLikeFlag
{
	MM_ALBUM_NOT_LIKE = 0, //未点赞
	MM_ALBUM_HAVE_LIKE = 1, //已赞
};

enum enAlbumDelFlag
{
	MM_ALBUM_NOT_DEL = 0, //未删除
	MM_ALBUM_HAVE_DEL = 1, //已删除
};

//个人相册timeline拉取
enum enAlbumGetTimelineType
{
	MM_ALBUM_GETTIMELIME_NEW = 1, //往下拉，获取最新
	MM_ALBUM_GETTIMELINE_OLD = 2, //往上拉，获取历史
};

enum enMMUserAttrAlbumFlag
{
	MM_USERATTRALBUMFLAG_HASIMG = 0x1, //个人空间有照片
	MM_USERATTRALBUMFLAG_PRIVACY_OPEN = 0x2, //废弃的
	MM_USERATTRALBUMFLAG_FOR_STRANGER_NO = 0x4, //不允许陌生人看个人相册
	MM_USERATTRALBUMFLAG_FOR_STRANGER_TENDAY = 0x8, //允许陌生人看最近10天的相片 
	MM_USERATTRALBUMFLAG_FOR_STRANGER_ONEMONTH = 0x10, //允许陌生人看最近一个月的相片
	MM_USERATTRALBUMFLAG_FOR_STRANGER_THREEMONTH = 0x20, //允许陌生人看最近三个月的相片
	MM_USERATTRALBUMFLAG_FOR_STRANGER_ONEYEAR = 0x40, //允许陌生人看一年的相片
	MM_USERATTRALBUMFLAG_FOR_STRANGER_ALL = 0x80, //允许陌生人看全部相片
	MM_USERATTRALBUMFLAG_FOR_LBS = 0x100, //相册LBS打开
};

enum enMMAlbumRetCode
{
	MM_ALBUM_RET_HAVE_LIKE = 201, //以及赞过了
	MM_ALBUM_RET_PHOTO_HAS_DELETE = 202, //照片已经删除了
	MM_ALBUM_RET_LIKE_SELF = 203, //给自己赞
	MM_ALBUM_RET_NOT_FOR_STRANGER = 204, //不给陌生人看
	MM_ALBUM_RET_SPAM = 205, //中了反垃圾
	MM_ALBUM_RET_BAN = 206, //被举报了
	MM_ALBUM_RET_COMMENT_NO_PERMISSION = 207, //留言或者回复留言没有权限(一般是对方拉黑，或者把你删除好友了)
};

enum enGetAlbumPhotoListType
{
	MM_GETALBUM_PHOTO = 1,
	MM_GETALBUM_SIGN = 2,
	MM_GETALBUM_PHOTO_AND_SIGN_MIX = 3,
	MM_GETALBUM_PHOTO_AND_SIGN_NOT_MIX = 4,
	MM_GET_BGIMGLIST = 5,
};

enum enMsgImgCompressType
{
	MM_MSGIMG_WITH_COMPRESS = 0,
	MM_MSGIMG_WITHOUT_COMPRESS = 1,
};

enum enSearchQRCodeType
{
	MM_SEARCH_QRCODE_USER = 0,
	MM_SEARCH_QRCODE_DIANSHANG = 1,
	MM_SEARCH_QRCODE_URL = 2,
	MM_SEARCH_QRCODE_UNKNOW = 3,
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
	MM_ABLUM_PHOTOATTR_ISPRIVATED = 0x1, // 是否私密照片
};

enum enAlbumPhotoPrivated
{
	MM_ALBUM_PHOTO_NOT_PRIVATED = 0,
	MM_ALBUM_PHOTO_PRIVATED = 1,
};

enum enAlbumSyncFlag
{
	MM_ALBUM_SYNC2_TXWEIBO = 0x1, //同步到腾讯微薄
};

enum enAlbumOpLogCmd
{
	MOD_ALBUM_PHOTOATTR = 1, //修改相片附属信息
	ACCEPT_WITH_PHOTO = 2, //接受别人推过来的相片
};

//sns相关定义

//sns多媒体数据类型

//sns object排版类型
enum enMMSnsObjectType
{
	MMSNS_OBJECT_UNKNOWN = 0, //未知类型
	MMSNS_OBJECT_PHOTO = 1, 
	MMSNS_OBJECT_TEXT = 2, 
	MMSNS_OBJECT_FEED = 3, 
	MMSNS_OBJECT_MUSIC = 4,
	MMSNS_OBJECT_VIDEO = 5,
	MMSNS_OBJECT_LOCATION = 6,
	MMSNS_OBJECT_BACKGROUND = 7,
	MMSNS_OBJECT_WXSIGN = 8, //微信签名
};

//sns媒体类型
enum enMMSnsMediaType
{
	MMSNS_DATA_TEXT = 1, //文字
	MMSNS_DATA_PHOTO = 2, //图片
	MMSNS_DATA_VOICE = 3, //语音
	MMSNS_DATA_VIDEO = 4, //视频
	MMSNS_DATA_MUSIC = 5, //音乐
};

//一些个人控制标志位
enum enMMSnsFlagDef
{
	MMSNS_FLAG_HAVE_OBJECT = 0x1, //个人主页是否又内容
	//MMSNS_FLAG_PRIVACY_CLOSE = 0x2, //隐私控制，不允许陌生人查看个人主页
	//MMSNS_FLAG_PRIVACY_OPEN = 0x4, //隐私控制，允许陌生人查看个人主页所有内容
	//MMSNS_FLAG_PRIVACY_TEN_OBJ = 0x8, //隐私控制，允许陌生人查看个人主页10个Object
    MMSNS_FLAG_BLACKLIST_MIGRATE = 0x10, //是否迁移回避的人到新版黑名单 
};

enum enMMSnsFlagExDef
{
	MMSNS_FLAG_PRIVACY_CLOSE = 0x1, //隐私控制，不允许陌生人查看个人主页
};

//每个object的一些标志位
enum enMMSnsObjectExtFlag
{
	MMSNS_OBJECT_EXTFLAG_BIDIRFRIEND = 0x1, //读取者这个object的发表者是否是双向好友关系
};

//评论类型
enum enMMSnsCommentType
{
	MMSNS_COMMENT_LIKE = 1, //留红心
	MMSNS_COMMENT_TEXT = 2, //文字评论
	MMSNS_COMMENT_MESSAGE = 3, //留言
	MMSNS_COMMENT_WITH = 4, //占坑
	MMSNS_COMMENT_STRANGER_LIKE = 5, //陌生人红心
	MMSNS_COMMENT_FORWORD = 6, //占坑
};

//media url类型
enum enMMSnsUrlType
{
	MMSNS_URL_OTHER = 0, //其他url
	MMSNS_URL_CDN = 1, //cdn
	MMSNS_URL_WEIXIN = 2, //存于weixin服务器，走snsdownload
};

//通知类型
enum enMMSnsMentionType
{
	//1, 有人给你标红心 使用MMSNS_COMMENT_LIKE
	//2, 有人给你文字评论了 使用MMSNS_COMMENT_TEXT
	//3, 有人给你留言了 使用MMSNS_COMMENT_MESSAGE
	MMSNS_MENTION_WITH = 4, //有人发照片提到你了
	// 5 use MMSNS_COMMENT_STRANGER_LIKE//陌生人红心
	MMSNS_MENTION_FORWORD = 6, //forword object
};

//object operation type
enum enMMSnsObjectOpType
{
	MMSNS_OBJECTOP_DEL = 1, //删除object或者media
	MMSNS_OBJECTOP_SET_PRIVACY = 2, //设置object或者media为私密
	MMSNS_OBJECTOP_SET_OPEN = 3, //设置object或者media为公开
	MMSNS_OBJECTOP_DELETE_COMMENT = 4, //删除评论
	MMSNS_OBJECTOP_CANCEL_LIKE = 5, //取消评论
};

//返回值
enum enMMSnsRetCode
{
	MMSNS_RET_SPAM = 201, //中了反垃圾
	MMSNS_RET_BAN = 202, //被举报，被ban了
	MMSNS_RET_PRIVACY = 203, //由于设置隐私，更多内容不给查看
	MMSNS_RET_COMMENT_HAVE_LIKE = 204, //以及留过了红心，不能再留
	MMSNS_RET_COMMENT_NOT_ALLOW = 205, //黑名单不允许评论
	MMSNS_RET_CLIENTID_EXIST = 206, //clientid已经存在
	MMSNS_RET_ISALL = 207, //已经是全部object了
	MMSNS_RET_COMMENT_PRIVACY = 208, //私密照片不允许评论
    MMSNS_RET_COMMENT_ID_ERROR = 209,
};

//同步到第三方的标志位
enum enMMSnsSync3rdFlag
{
	MMSNS_SYNC3RD_TXWEIBO = 0x1, //同步到腾讯微薄
	MMSNS_SYNC3RD_FACEBOOK = 0x2, // sync to facebook
	MMSNS_SYNC3RD_QZONE = 0x4, //同步到qzone
	MMSNS_SYNC3RD_TWITTER = 0x8, //同步到twitter
};

enum enMMSnsPrivacyFlag
{
	MMSNS_OBJECT_OPEN = 0, //公开
	MMSNS_OBJECT_PRIVACY = 1, //私密
	MMSNS_OBJECT_FOR_STRANGER = 2, //只许陌生人可见
};



//just for oss
enum enMMSnsCheckSelfPageScene
{
	MMSNS_CHECKSELFPAGE_TIMELINE = 1, //从timeline进入查看homepage
	MMSNS_CHECKSELFPAGE_SETTING = 2, //从设置页进入查看homepage
};

enum enSetBgImgType
{
	MMSNS_SETBGIMG_SELFPIC = 1, //自己上传图片设置背景图
	MMSNS_SETBGIMG_MASTERPIC = 2, //设置大师背景图 
};

enum enSnsUploadPhotoFrom
{
	MMSNS_UPLOAD_PHOTO_FROM_TAKE = 1, //拍照
	MMSNS_UPLOAD_PHOTO_FROM_EXIST = 2, //选择已有照片
};

enum enSnsSpecialTag
{
	MMSNS_STAR_FANS_GROUP = 100000,
};

//sns --> end

enum enSnsPostSource
{
	MM_SNSPOST_NORMAL_SHARE = 1, //常规分享
	MM_SNSPOST_WEIBO_RECOMMAND = 2, //微薄推荐
	MM_SNSPOST_NEWS = 3, //新闻阅读
	MM_SNSPOST_BUSINESS_ADDR = 4, //企业联系人订阅
	MM_SNSPOST_THIRDAPP = 5, //第三方app
	MM_SNSPOST_SHAKEIMG = 6, //摇到的原图
	MM_SNSPOST_SHAKEPAGE = 7, //摇到的网页
};

enum enUploadPhotoSource
{
	MM_UPLOAD_PHOTO_CAMERA_FRONT = 1, //前置摄像头
	MM_UPLOAD_PHOTO_CAMERA_BEHIND = 2, //后置摄像头
	MM_UPLOAD_PHOTO_CHOOSE = 3, //选择
	MM_UPLOAD_PHOTO_FORWARD = 4, //转发
	MM_UPLOAD_PHOTO_COPY = 5, //复制
	MM_UPLOAD_PHOTO_FACEBOOKALBUM = 6, 
};

enum enUploadPhotoNetType
{
	MM_UPLOAD_PHOTO_NET_WIFI = 1, //wifi
	MM_UPLOAD_PHOTO_NET_OTHER = 2, //其他
};

enum enMMAppInfoIconType
{
	MMAPPINFO_ICONTYPE_SD = 1,
	MMAPPINFO_ICONTYPE_HD = 2,
	MMAPPINFO_ICONTYPE_MDPI = 3,
	MMAPPINFO_ICONTYPE_HDPI =4,
	MMAPPINFO_ICONTYPE_LDPI = 5
};

//getA8key opcode
enum enMMGetA8KeyOpCode
{
	MMGETA8KEY_OPENAPI = 1,
	MMGETA8KEY_REDIRECT = 2,
	MMGETA8KEY_QZONE = 3,
};

//getA8key scene
enum enMMGetA8KeyScene
{
	MMGETA8KEY_SCENE_UNKNOW = 0,
	MMGETA8KEY_SCENE_MSG = 1,
	MMGETA8KEY_SCENE_TIMELINE = 2,
	MMGETA8KEY_SCENE_PROFILE = 3,
	MMGETA8KEY_SCENE_QRCODE = 4,
	MMGETA8KEY_SCENE_QZONE = 5,
	MMGETA8KEY_SCENE_OAUTH = 6,
    MMGETA8KEY_SCENE_OPEN = 7,
    MMGETA8KEY_SCENE_PLUGIN = 8,
};


//scan qrcode action
enum enMMScanQrcodeActionCode
{
	MMSCAN_QRCODE_A8KEY = 0,
	MMSCAN_QRCODE_TEXT = 1, 
	MMSCAN_QRCODE_WEBVIEW = 2,
	MMSCAN_QRCODE_APP = 3,
	MMSCAN_QRCODE_PROFILE = 4,
	MMSCAN_QRCODE_PLUGIN = 5,
	MMSCAN_QRCODE_SPECIAL_WEBVIEW = 6,
	MMSCAN_QRCODE_WEBVIEW_NO_NOTICE = 7,
	MMSCAN_QRCODE_VCARD = 8,
    MMSCAN_QRCODE_JUMP = 9,
};

//OpCody for webwx logout action
enum enMMLogoutWebWx
{
	MMWEBWX_BANNER_LOGOUT = 1,
	MMWEBWX_MOBILE_LOGOUT = 2,
};

//auth key all in one - key type
enum enMMAuthKeyAllInOneType
{
	MMAIO_SKEY = 1,
	MMAIO_MMSKEY = 2,
	MMAIO_A2 = 3,
	MMAIO_A8 = 4,
	MMAIO_PASSKEY = 5,
	MMAIO_BIZPASSKEY = 6,
};

enum enMMBizMsgStatus
{
	MMBiz_MSGSTATUS_SENDING=1,
	MMBiz_MSGSTATUS_SENT=2,
	MMBiz_MSGSTATUS_DELIVERED=3,
	MMBiz_MSGSTATUS_READ=4,
	MMBiz_MSGSTATUS_SENDFAIL=5,
	MMBiz_MSGSTATUS_CHECKFAIL=6,
    
    /*
     *  以下为扩展状态，mmbizindex 数据存储使用，
     *  外界拿到的仍未 MMBiz_MSGSTATUS_SENDING 1
     */
	MMBiz_MSGSTATUS_SENDING_SAVE=101,// 保存在 mmbizindex
	MMBiz_MSGSTATUS_SENDING_ANTISPAM=102,//提交反垃圾审核
	MMBiz_MSGSTATUS_SENDING_ANTISPAm_CALLBACK=103,//反垃圾回调 mmbizbroker
	MMBiz_MSGSTATUS_SENDING_PRE_TBORG=104,// 提交 tborg 前预处理
	MMBiz_MSGSTATUS_SENDING_TBORG=105,// 已提交 tborg
    MMBiz_MSGSTATUS_SENDING_CALLBAK=106,// tborg 回调中
};

// media type for UploadMedia
enum enMMMeidaType
{
	MM_MEDIA_TYPE_IMAGE = 1,
	MM_MEDIA_TYPE_VIDEO = 2,
	MM_MEDIA_TYPE_AUDIO = 3,
	MM_MEDIA_TYPE_ATTACHMENT =4,
	MM_MEDIA_TYPE_PATTERN_AUDIO = 5, // 旧的模式认别需要的audio类型.
	MM_MEDIA_TYPE_PATTERN_AUDIO_SINGLE = 6, // 模式认别需要的audio类型.个人
    MM_MEDIA_TYPE_PATTERN_AUDIO_CHATROOM =  7, // 模式认别需要的audio类型.群聊
    MM_MEDIA_TYPE_PATTERN_AUDIO_APP =  8, // 模式认别需要的audio类型.公众号

};

enum enMMBottleFlag
{
	MMBOTTLE_FLAG_MATCH_AGE = 1,
	MMBOTTLE_FLAG_MATCH_CITY = 2,
	MMBOTTLE_FLAG_NOT_ZH = 4,
};

enum enMMBottleFlag2
{
	MMBOTTLE_FLAG2_ZH_CN = 1,
	MMBOTTLE_FLAG2_ZH_TW = 2,
	MMBOTTLE_FLAG2_EN = 4,
};

//OPEN API
enum enMMOpenAppType
{
	MMOPEN_CLIENTAPP = 0x01,
	MMOPEN_MESSAGEAPP = 0x02,
	MMOPEN_TIMELINEAPP = 0x04,
	MMOPEN_LOGINAPP = 0x08,
	MMOPEN_BIZAPP = 0x10,
	MMOPEN_KFAPP = 0x20,
};

// statreport
enum enMMStatReportFlag
{
	ReportBan = 0,  // 禁止上报
	ReportAll = 1, // 全部上报
	ReportNetSenceAndNet = 2, // 只上报 网络事件 和 netsence

	ReportOnlyExt = 4, // 只上报 extinfo
};

enum enMMStatReportActionID
{
	PushProcStart = 10001, // 前台进程启动
	PushProcStop = 10002,

	WorkerProcStart = 10003, //后台进程启动
	WorkerProcStop = 10004,

	Connect = 10101,	// connnect 事件
	DisConnect = 10102,

	NetSenceSendImg = 10401, 
};

enum enMMStatReportNetType
{
	MM_STAT_WiFi = 1,
	MM_STAT_CNWap = 2,
	MM_STAT_CNNet = 3,
	MM_STAT_Net3G = 4,
};

enum enMMStatReportIPType
{
	enDNSFlag = 0x1,
	enLongConnectFlag = 0x10,
};

enum enMMStatReportIfSuc
{
	enOK = 0,
};

enum enMMBRAND_SUBSCRIPTION {
	MMBRAND_SUBSCRIPTION_BLOCK_MESSAGE_NOTIFY = 0x1, // 不接收新消息提醒
	MMBRAND_SUBSCRIPTION_HIDE_FROM_PROFILE = 0x2,  // 不在我的profile中显示
};


//push声音oplog
enum emMMVoiceSoundBitFlag
{
	MM_VOICESOUND_SOUND =0x1,
	MM_VOICESOUND_VOIPSOUND =0x2,
	MM_VOICESOUND_ALL = ( MM_VOICESOUND_VOIPSOUND | MM_VOICESOUND_SOUND ),
};

enum enVerifyPswdType
{
       MM_VERIFY_PSWD_WX = 1,
       MM_VERIFY_PSWD_QQ = 2,
       MM_VERIFY_PSWD_GET3GIMG = 3,
       MM_VERIFY_PSWD_EMAIL = 4,
};

enum enBindType
{
       MM_BINDTYPE_QQ = 1,
       MM_BINDTYPE_MOBILE = 2,
       MM_BINDTYPE_FACEBOOK = 3,
       MM_BINDTYPE_EMAIL = 4,
};

// 设置密码的场景值
enum enQueryHasPasswdScene
{
	MM_QUERYHASPASSWD_SET_SCENE = 1,		// 用户主动点设置密码场景
	MM_QUERYHASPASSWD_LOGOUT_SCENE = 2,		// 退出登陆场景
	MM_QUERY_EMAIL_PWD_SCENE = 3 , //查询是否有email密码
};

enum enEmailOpCode
{
     MM_EMAIL_BIND = 1,  //绑定邮箱
     MM_EMAIL_UNBIND = 2,  //解绑邮箱
	 MM_EMAIL_BIND_WITH_SET_MAIN = 4,
};

enum enMMStatReportLogType
{
	MM_CLIENTPERFORMANCE = 0, //客户端性能
	MM_KVSTAT = 1, //kvstat业务统计
	MM_STACKREPORT = 2,  //crash上报
	MM_USERACTION = 3,  //点击流
	MM_STATREPORT = 4, //客户端网络性能
};


enum enVoiceAddrSearchType
{
	MM_SEARCH_CONTACT = 1,
	MM_SEARCH_MSG = 2,
};

enum enBakChatDataType
{
	MM_BAKCHAT_MSG_DATA = 1,//消息列表类型
	MM_BAKCHAT_MEDIA_DATA = 2,//大媒体数据类型
};

enum enBakChatEncryptType
{
	MM_BAKCHAT_ENCRYPT_NONE = 0,//不加密码
	MM_BAKCHAT_ENCRYPT_AES  = 1,//aes加密 ，具体由客户端定义
};

//BakChat MediaType
enum enBakChatMediaType
{
	MMBAK_NONE = 0,
	MMBAK_THUMB_IMG = 1,
	MMBAK_IMG = 2,
	MMBAK_HD_IMG = 3,
	MMBAK_EMOJI_THUMB = 4,
	MMBAK_EMOJI = 5,
	MMBAK_APPTHUMB = 6,
	MMBAK_APPDATA = 7,
	MMBAK_APPIMG = 8,
	MMBAK_VOICE = 9,
	MMBAK_VIDEO = 10,
	MMBAK_VIDEO_THUMB = 11,
};


enum enOpVoiceRemindType
{
    MM_VOICE_REMIND_CANCEL = 1, //取消语音
};

enum enVoiceFormat
{
    MM_VOICE_FORMAT_UNKNOWN = -1, 
    MM_VOICE_FORMAT_AMR = 0,   // AMR语音格式
    MM_VOICE_FORMAT_SPEEX = 1, // SPEEX语音格式
    MM_VOICE_FORMAT_MP3 = 2, 
    MM_VOICE_FORMAT_WAVE = 3, 
};

enum enChatRoomMemberFlag
{
    MM_CHATROOMFLAG_DISPALYNAME = 0x1, //显示群名片
};

enum enMainAcctType
{
	MM_MAIN_ACCT_NONE = 0, //无可设置为主帐号的类型
	MM_MAIN_ACCT_QQ = 1, //QQ可设置为主帐号
	MM_MAIN_ACCT_EMAIL = 2 ,//Email可设置为主帐号
};

enum enMainAcctState
{
	MM_MAIN_ACCT_STATE_SETABLE = 0, //可以设置为主帐号
	MM_MAIN_ACCT_STATE_UNSET = 1 , //未设置
	MM_MAIN_ACCT_STATE_NEED_VERIFY = 2 ,//需要验证
	MM_MAIN_ACCT_STATE_NNED_PWD = 4, //需要密码
};

enum enBindSafeMobileOpCodeType
{
	MM_SAFE_MOBILE_OP_BIND = 1, //绑定安全手机
	MM_SAFE_MOBILE_OP_UNBIND = 2, //解绑安全手机
};

/**
* Auth Ticket 验证的状态
*/
enum enAuthTicketStateType
{
	AUTH_TICKET_STATE_NONE = 0x0, //没有任何验证
	AUTH_TICKET_STATE_QQ = 0x1, //验证QQ成功
	AUTH_TICKET_STATE_MOBILE  =  0x2, //验证手机成功
	AUTH_TICKET_STATE_WX  = 0x4, //验证WX密码成功
	AUTH_TICKET_STATE_FACEBOOK = 0x8, //验证Facebook成功
	AUTH_TICKET_STATE_EMAIL = 0x10, //验证Email成功
	AUTH_TICKET_STATE_SAFE_DEVICE = 0x3,  //验证设备锁成功（验证手机与QQ都成功)
	AUTH_TICKET_STATE_ANY = 0x1f, //验证任意的一个密码成功
	AUTH_TICKET_STATE_TIMEOUT = 0xffff, //说明已经过期了，当获取ticket没有时，返回此ticket
};

enum enSafeDeviceStateType
{
	MM_SAFE_DEVICE_STATE_UNSAFE = 0,
	MM_SAFE_DEVICE_STATE_SAFE = 1,
	MM_SAFE_DEVICE_STATE_ADD = 2,
};

enum enRegForceType
{
	MM_REG_UNFORCE = 0,//非强制注册
	MM_REG_FORCE = 1, //强制注册，不管这个号是否存在都创建新号
};


// 商家与普通用户的交互模式
enum enMMBizInteractiveMode
{
    MM_BIZ_INTERACTIVE_MODE_COMM = 0, // 普通消息模式
    MM_BIZ_INTERACTIVE_MODE_QUESTION = 1, // 提问回答模式
    MM_BIZ_INTERACTIVE_MODE_SELFMENU = 2, // 自定义菜单模式
};

// 商家自定义菜单中的菜单类型
enum enMMBizSelfMenuType
{
    MM_BIZ_SELFMENU_TYPE_SUBMENU = 0, // 弹出子菜单
    MM_BIZ_SELFMENU_TYPE_CLICK = 1,   // 点击触发事件
    MM_BIZ_SELFMENU_TYPE_VIEWURL = 2, // 查看url
    MM_BIZ_SELFMENU_TYPE_MSGEXCHANGE = 3, // 消息交流
};

// 放在用户属性的ExternalInfo的json字段，用于表明该公众号是否能接收模式识别语音
#define RECEIVE_SPEEX_VOICE_FIELD "CanReceiveSpeexVoice" 

enum enCardMainType
{
    MM_CARD_TYPE_ALL = 0,
    MM_CARD_TYPE_CARD = 1,    //卡
    MM_CARD_TYPE_COUPON = 2,  //券
    MM_CARD_TYPE_TICKET = 3,  //票
};

enum enCardCardSubType
{
    MM_CARD_CARD_SUBTYPE_ALL = 0,
    MM_CARD_CARD_SUBTYPE_VIPCARD = 1,        //会员卡
    MM_CARD_CARD_SUBTYPE_RECHARGECARD = 2,   //充值卡
};

enum enCardCouponSubType
{
    MM_CARD_COUPON_ALL = 0,
    MM_CARD_COUPON_SUBTYPE_VOUCHER = 1,      //代金券
    MM_CARD_COUPON_SUBTYPE_COUPON = 2,       //优惠券
    MM_CARD_COUPON_SUBTYPE_GIFTVOUCHER = 3,  //礼品券
};

enum enCardTicketSubType
{
    MM_CARD_TICKET_SUBTYPE_ALL = 0,
    MM_CARD_TICKET_SUBTYPE_TICKET = 1,  //普通票
};

enum enCardImageType
{
    MM_CARD_IMAGE_TYPE_QRCODE = 1,
    MM_CARD_IMAGE_TYPE_BARCODE = 2,
};

enum enCardImageSizeType
{
    MM_CARD_IMAGE_SIZE_TYPE_NORMAL = 1,
    MM_CARD_IMAGE_SIZE_TYPE_HD = 2,
};

#define MM_USERATTR_EXTERNALINFO_BIZMENU "MMBizMenu" // 用户属性ExternalInfo中json数据的商家菜单的key
#define MM_USERATTR_EXTERNALINFO_INTERACTIVE_MODE "InteractiveMode" // 用户属性ExternalInfo中交互模式Name

// 消息协议msgSource中用于内容区分的头部串
#define MM_MSGSOURCE_HEADERSTR_BIZMENU "#bizmenu#" // 针对商家自定义菜单的消息回复

// 商家按钮在空Key传入时自定设置Key时的特定头部串
#define MMBIZ_NULL_BUTTONKEY_HEADER "nullbuttonkey_"

// click command 的类型定义
enum enClickCommandType
{
    MM_CLICKCOMMAND_TYPE_BIZMENU_UNRECOGNIZED = 0, // 客户端不识别的菜单类型(都可以通过这个类型上报上来)
    MM_CLICKCOMMAND_TYPE_BIZMENU_CLICKNOTIFY  = 1, // 商家菜单之点击通知类型
};


enum enUpdateSubUserAttrType
{
    MM_UPDATE_SUBUSER_ATTR_NICKNAME = 0, //更新昵称
};

enum enSubUserType
{
    MM_SUBUSER_TYPE_ALL = 0,
    MM_SUBUSER_TYPE_CS = 1, //Custom Service
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
    MM_JSAPI_PERMISSION_BITSET_LOG = 0x1,
    MM_JSAPI_PERMISSION_BITSET_IMAGE_PREVIEW = 0x2,
    MM_JSAPI_PERMISSION_BITSET_PROFILE = 0x4,
    MM_JSAPI_PERMISSION_BITSET_SHARE_WEIBO = 0x8,
    MM_JSAPI_PERMISSION_BITSET_SHARE_TIMELINE = 0x10,
    MM_JSAPI_PERMISSION_BITSET_ADD_CONTACT = 0x20,
    MM_JSAPI_PERMISSION_BITSET_SEND_APP_MSG = 0x40,
    MM_JSAPI_PERMISSION_BITSET_SCAN_QRCODE = 0x80,
    MM_JSAPI_PERMISSION_BITSET_ADD_EMOTICON = 0x100,
    MM_JSAPI_PERMISSION_BITSET_HAS_EMOTICON = 0x200,
    MM_JSAPI_PERMISSION_BITSET_CANCEL_ADD_EMOTION = 0x400,
    MM_JSAPI_PERMISSION_BITSET_HASH_CHANGE = 0x800,
    MM_JSAPI_PERMISSION_BITSET_HIDE_TOOLBAR = 0x1000,
    MM_JSAPI_PERMISSION_BITSET_SHOW_TOOLBAR = 0x2000,
    MM_JSAPI_PERMISSION_BITSET_HIDE_OPTION_MENU = 0x4000,
    MM_JSAPI_PERMISSION_BITSET_SHOW_OPTION_MENU = 0x8000,
    MM_JSAPI_PERMISSION_BITSET_GET_NEWWORK_TYPE = 0x10000,
    MM_JSAPI_PERMISSION_BITSET_GET_CLOSE_WINDOW = 0x20000,

    MM_JSAPI_PERMISSION_BITSET_FONT_MENU = 0x40000,
    MM_JSAPI_PERMISSION_BITSET_PROFILE_MENU = 0x80000,
    MM_JSAPI_PERMISSION_BITSET_ADD_CONTACT_MENU = 0x100000,
    MM_JSAPI_PERMISSION_BITSET_SEND_APP_MSG_MENU = 0x200000,
    MM_JSAPI_PERMISSION_BITSET_SHARE_WEIBO_MENU = 0x400000,
    MM_JSAPI_PERMISSION_BITSET_SHARE_TIMELINE_MENU = 0x800000,
    
    MM_JSAPI_PERMISSION_BITSET_DOWNLOAD_IN_WEBVIEW = 0x1000000,
    MM_JSAPI_PERMISSION_BITSET_GET_INSTALL_STATE = 0x2000000,
};

enum enGeneralControlBitSet
{
    MM_GENERAL_CONTROL_BITSET_SHOW_INPUT_ALERT_TIPS = 0x1,
};

enum enSnsControlFlagType
{
    SNS_CONTROLFLAG_CLOSE_SUMMARY = 0x1,
    SNS_CONTROLFLAG_CLOSE_SOURCE = 0x2,
};
enum enMMClientCheckCmd
{
	MM_CLIENTCHECK_CDM_CRC32 = 1    //crc32 
};

enum enLbsRoomOpCode
{
	MM_JOIN_LBSROOM = 1,
	MM_LEAVE_LBSROOM = 2,
};

enum enSafeAuthDeviceFlag
{
    MM_SAFEAUTH_DEVICE_FLAG_NONE = 0, // none
};

enum enEquipmentType
{
    MM_EQUIPMENT_TYPE_PHOTO_FRAME = 1,
};

enum enEquipmentFlag
{
    MM_EQUIPMENT_FLAG_EQUIPMENT = 0x1,
};

enum enCDNImgType
{
	MM_CDNIMG_BIG = 1,
	MM_CDNIMG_MID = 2,
};

///////////////////////IGG//////////////////////////////////////////////////

enum enIGGStatus
{
	IGG_STATUS_OPEN=0x1,
	IGG_STATUS_EMAILVERIFY=0x2,				//邮箱已经通过验证
	IGG_STATUS_MOBILEVERIFY=0x4,				//手机已经通过验证
	IGG_STATUS_PUSH_OPEN = 0x8,				//用户打开推送开关
	IGG_STATUS_USERNAME = 0x10,				//用户已经修改过用户名
	IGG_STATUS_VERIFY_USER =0x20,				//打开好友验证的开关
	IGG_STATUS_PUSH_SECRECY = 0x800,			//推送消息隐私设置
	//...
};

enum enIGGRegType
{
	IGG_USERATTR_REG_UNKNOW = 0,
	IGG_USERATTR_REG_GMAIL = 1,
	IGG_USERATTR_REG_MOBILE = 2,
	IGG_USERATTR_REG_EMAIL = 3,
	IGG_USERATTR_REG_FACEBOOK = 5,
};

enum enIGGPushSetOpt
{
	IGGPUSH_REG_TOKEN = 0,
	IGGPUSH_UNREG_TOKEN = 1,
	IGGPUSH_REG_REGID = 2,
	IGGPUSH_UNREG_REGID = 3,
	IGGPUSH_OPEN = 4,
	IGGPUSH_CLOSE = 5,
};

///////////////////////////////////////////////////////////////////////////
enum enIGGHeadImgType
{
	IGG_HEADIMG_USER = 1,
	IGG_HEADIMG_CHATROOM = 2,
};

///////////////////////IGG//////////////////////////////////////////////////
enum enIGGContactType
{	
	IGG_CONTACT_IGG = 0,				// MINGLE????	
	IGG_CONTACT_FACEBOOK = 1,			//facebook
	IGG_CONTACT_GMAIL = 2,				//gmail
	IGG_CONTACT_MOBILE = 3,			//mobile
};

enum enIGGMobileOptCode
{
	IGG_MOBILE_OPT_REG = 1,			//手机注册
	IGG_MOBILE_OPT_AUTH = 2,			//手机验证码登陆(修改密码)
	IGG_MOBILE_OPT_BIND = 3,			//手机绑定(更换)
	IGG_MOBILE_OPT_UNBIND = 4,		//手机解除绑定
	IGG_MOBILE_OPT_REGCHECK = 5,		//手机验证码注册校验
	IGG_MOBILE_OPT_AUTHCHECK = 6,		//手机验证码登陆校验
	IGG_MOBILE_OPT_BINDCHECK = 7,		//手机验证码绑定校验
	IGG_MOBILE_OPT_UNBINDCHECK = 8,	//手机验证码解绑校验
};

enum enIGGEmailOptCode
{
	IGG_EMAIL_OPT_BIND = 1,			//邮箱绑定
	IGG_EMAIL_OPT_UNBIND = 2,			//邮箱解除绑定
	IGG_EMAIL_OPT_SENDVERIFY = 3,		//发送确认邮件
	IGG_EMAIL_OPT_VERIFY = 4,
};

enum enIGGPlugFriendOptCode
{
	IGG_PLUG_FRIEND_ADD = 1,				//联系人增加
	IGG_PLUG_FRIEND_DEL = 2,				//联系人删除
};

enum enIGGPlugFriendType
{
	IGG_PLUG_FRIEND_MOBILE = 7,		// 手机联系人
	IGG_PLUG_FRIEND_EMAIL = 8,		// email联系人
	IGG_PLUG_FRIEND_FB = 9,			// facebook好友
	IGG_PLUG_FRIEND_GMAIL = 10,		// gmail联系人
};

enum enUserStatus//bingchen 个人属性
{
	IGG_USERSTATUS_DEACTIVE = 0,//没有设置个人基本信息
	IGG_USERSTATUS_ACTIVE,//已经设置了个人基本信息
};

enum enChatRoomType
{
	IGG_CHATROOMTYPE_MAKEFRIEND_MEETING = 0, // 交友聚会
	IGG_CHATROOMTYPE_GAME,		 	// 游戏
	IGG_CHATROOMTYPE_RECREATION,	// 娱乐
	IGG_CHATROOMTYPE_HOBBY,			// 兴趣爱好
	IGG_CHATROOMTYPE_SCHOOL,		// 学校、大学
	IGG_CHATROOMTYPE_LIFE,			// 生活休闲
	IGG_CHATROOMTYPE_BUSINESS,		// 行业交流
	IGG_CHATROOMTYPE_PRODUCT,		// 品牌、产品
	IGG_CHATROOMTYPE_OTHER,			// 其他
};

enum enChatRoomVerifyType
{
	IGG_CHATROOMVERIFY_NOTNEED = 0,	// 不需要验证
	IGG_CHATROOMVERIFY_NEED = 1,	// 需要验证
};

///////////////////////IGG//////////////////////////////////////////////////

enum enMMArtFlag
{
    MM_ARTFLAG_READING = 0x1, //阅读
    MM_ARTFLAG_WRITING = 0x2, //写作
    MM_ARTFLAG_MUSIC = 0x4, //唱歌
    MM_ARTFLAG_PLAYINGINSTRUMENT = 0x8, //演奏乐器
    MM_ARTFLAG_PHOTOGRAPHY = 0x10,
    MM_ARTFLAG_PAINTING = 0x20,
    MM_ARTFLAG_POTTERY = 0x40,
};

enum enMMSportsFlag
{
	MM_SPORTSFLAG_SWIMMING = 0x1,
	MM_SPORTSFLAG_JOGGING = 0x2,
	MM_SPORTSFLAG_GYM = 0x4,
	MM_SPORTSFLAG_TEAMSPORTS = 0x8,
	MM_SPORTSFLAG_YOGA = 0x10,
	MM_SPORTSFLAG_HIKING = 0x20,
	MM_SPORTSFLAG_FISHING = 0x40,
	MM_SPORTSFLAG_Racquet = 0x80,
};

enum enMMSocialActivitiesFlag
{
	MM_SOCIALACTIVITIESFLAG_BARS = 0x1,
	MM_SOCIALACTIVITIESFLAG_DANCING = 0x2,
	MM_SOCIALACTIVITIESFLAG_VOLENTEERING = 0x4,
	MM_SOCIALACTIVITIESFLAG_MOVIES = 0x8,
	MM_SOCIALACTIVITIESFLAG_KARAOKE = 0x10,
	MM_SOCIALACTIVITIESFLAG_SHOPPING = 0x20,
	MM_SOCIALACTIVITIESFLAG_COFFEE = 0x40,
};

enum enMMTechnologyFlag
{
	MM_TECHNOLOGYFLAG_CARS = 0x1,
	MM_TECHNOLOGYFLAG_GAMES = 0x2,
	MM_TECHNOLOGYFLAG_INTERNET = 0x4,
};

enum enMMLifestyleFlag
{
	MM_LIFESTYLEFLAG_COOKING = 0x1,
	MM_LIFESTYLEFLAG_KNITTING = 0x2,
	MM_LIFESTYLEFLAG_GARDENING = 0x4,
	MM_LIFESTYLEFLAG_PETS = 0x8,
	MM_LIFESTYLEFLAG_TRAVELLING = 0x10,
	MM_LIFESTYLEFLAG_SLEEPING = 0x20,
	MM_LIFESTYLEFLAG_FOOD = 0x40,
};

enum enMMIntentionFlag
{
	MM_INTENTION_FLAG_FRIENDSHIP = 0x1,
	MM_INTENTION_FLAG_RELATIONSHIP = 0x2,
	MM_INTENTION_FLAG_CASUALDATING = 0x4,
};

enum enLBSVisibleState
{
	MM_LBSVISIBLESTATE_VISIBLE = 0,
	MM_LBSVISIBLESTATE_INVISIBLE = 1,
};

enum enLBSDistanceUnit
{
	MM_LBSDISTANCEUNIT_KILOMETER = 0,
	MM_LBSDISTANCEUNIT_MILE = 1,
};

enum enIGGProfileReportReason
{
	IGG_PROFILEREPORT_PORN = 0,
	IGG_PROFILEREPORT_SPAM = 1,
	IGG_PROFILEREPORT_STOLEN = 2,
	IGG_PROFILEREPORT_FAKE = 3,
	IGG_PROFILEREPORT_OFFENSIVE = 4,
	IGG_PROFILEREPORT_MAX = 5,
};

enum enIGGGroupReportReason
{
	IGG_GROUPREPORT_PORN = 0,
	IGG_GROUPREPORT_SPAM = 1,
	IGG_GROUPREPORT_MISLEAD = 2,
	IGG_GROUPREPORT_FAKE = 3,
	IGG_GROUPREPORT_OFFENSIVE = 4,
	IGG_GROUPREPORT_MAX = 5,
};


#define MM_DATA_SYSCMD_NEWXML_SUBTYPE_EQUIPMENTMSG "equipmentmsg"

//支持主帐号的版本
#define IS_SUPPORT_MAIN_ACCT_CLIENT "IsSupportMainAcctClient"

//支持设备锁的版本
#define IS_SUPPORT_SAFE_DEVICE_CLIENT "IsSupportSafeDeviceClient"

#define KF_USERNAME_PREFIX "mykf_" //客服号前缀

#define EQUIPMENT_USERNAME_PREFIX "eqid_"

// 多语言定义
// 简体中文
#define	MM_LANG_CN "zh_CN"
// 繁体中文
#define MM_LANG_TW "zh_TW"
// 英文
#define MM_LANG_EN "en"
// 泰文
#define MM_LANG_TH "th"

#define MM_LANG_VI "vi"

#define MM_LANG_ID "id"

#define MM_LANG_PT "pt"

#define MM_LANG_ES "es"

#define MM_LANG_RU "ru"

#define MM_LANG_AR "ar"

#define MM_LANG_PL "pl"

#define MM_LANG_HI "hi"

#define MM_LANG_JA "ja"

#define MM_LANG_IT "it"

#define MM_LANG_KO "ko"

#define MM_LANG_MS "ms"

#define MM_LANG_DE "de_DE"



// 系统tips中带的content最大字数
#define MAX_TIPS_LENGTH 15

// 群组聊天的最大人数，白名单里的可以支持20个
#define MAX_MEMBER_COUNT_SP  20

// 群组聊天的最大人数
#define MAX_MEMBER_COUNT  10

//用户主动邀请好友时推荐的好友个数
#define INVITEFRIEND_COUNT	30

//msscache 配置文件路径
#define MMSTORE_CLI	"/home/qspace/etc/client/mmstore_cli.conf"

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

#define RSA_PUBLIC_KEYE "010001"

#define RSA_PUBLIC_KEYN "DFE56EEE6506E5F9796B4F12C3A48121B84E548E9999D834E2C037E3CD276E9C4A2B1758C582A67F6D12895CE5525DDE51D0B92D32B8BE7B2C85827729C3571DCC14B581877BC634BCC7F9DA3825C97A25B341A64295098303C4B584EC579ECCA7C8B96782F65D650039EE7A0772C195DBEFC4488BDFB0B9A58C5C058E3AB04D"

//新加密算法
#define AES_RANDOM_ENCRYPT_KEY_LEN 16

#define AES_A8KEY_WRAPPER_PUBLICKEY "a@8^k!e%y"

// ServerId Encrypt key
#define SERVERID_ENCRYPT_KEY "abcdefgh"

// GetHeadImg UserName Encrypt key
#define USERNAME_ENCRYPT_KEY "weixin"

#define MM_AUTHBYFACEBOOK_USERNAME "facebook@wechat_auth"

#define IGG_AUTHBYFACEBOOK_USERNAME "facebook@mingle_auth"
#define IGG_AUTHBYGMAIL_USERNAME "gmail@mingle_auth"

#define BIZ_PASSKEY_ENCRYPT_KEY_PREFIX "b!i^z"

//验证码专区
#define MM_VERIFYIMG_FORMAT "jpg"
#define MM_VERIFY_SWITCH	0
#define MM_EMAIL_REG		1	

// 特殊命令专区
#define MM_SPECIAL_CMD_LIGHTON_WEIXIN_ICON "ktzx"  // 点亮QQ头像上的微信小图标
#define MM_SPECIAL_CMD_LIGHTOFF_WEIXIN_ICON "gbzx" // 熄灭QQ头像上的微信小图标
#define MM_SPECAIL_CMD_START_LINE_CONTROL "kqej"  //使用线控功能
#define MM_SPECAIL_CMD_STOP_LINE_CONTROL "gbej"  //停用线控功能

// 统计专区
#define REG_MTYPE_REG		1
#define REG_MTYPE_UNREG		2
#define REG_MTYPE_VERIFY	3
#define REG_STYPE_QQMAIL	1
#define REG_STYPE_NOTQQMAIL	2
#define REG_STYPE_QQ		3
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

//////////////////  QQ邮箱 APP 化的错误码定义
// 1. 通用：
// note: -3， -4 的错误码统一可以在客户端提示 用户未激活QQ邮箱。
#define MBErrNoError 0        //  正常
#define MBErrNetwork -999     //  网络错误
#define MBErrSystem -1        //  系统错误
#define MBErrParams -2        //   参数错误
#define MBErrInactive -3      //   用户未激活
#define MBErrAccessDenied -4  //  请求拒绝，可能为cgi名字写错，也可能是用户未激活QQ邮箱
#define MBErrNotPass -5       //  skey错误，验证失败
// 2. ReadMail
#define MBErrMailid -6   //  错误的mailid
#define MBErrDeletedMail -7   //  邮件已经被删除
/* 3. ComposeSend
 ** -6：邮件被拦截（反垃圾或是收件人主动拒收，或是收件人未开通拒收）
 **  -101：收件人总数超过限制
 **  -102：邮件大小超过限制
 **  -103：邮件发送失败
 **  -104： 频率限制
 **  -105： 垃圾邮件
 **/
#define MBErrRefused -6
#define MBErrTooManyRcpts -101
#define MBErrTooManyData -102
#define MBErrSendMailFail -103
#define MBErrFreqErr -104
#define MBErrJunkMail -105

#define VOICE_XML_TEMP "<msg><voicemsg endflag=\"%d\" length=\"%u\" voicelength=\"%u\" clientmsgid=\"%s\" fromusername=\"%s\" downcount=\"%u\" cancelflag=\"%u\" forwardflag=\"%u\" /></msg>"

#define BIZ_VOICE_XML_TEMP "<msg><voicemsg endflag=\"%d\" length=\"%u\" voicelength=\"%u\" clientmsgid=\"%s\" fromusername=\"%s\" downcount=\"%u\" cancelflag=\"%u\" masterbufid=\"%llu\" slavebufid=\"%llu\" /></msg>"

#define VOICE_XML_TEMP_QQ "<msg><voicemsg endflag=\"%d\" length=\"%u\" voicelength=\"%u\" clientmsgid=\"%s\" fromusername=\"%s\" downcount=\"%u\" qqnickname=\"%s\" cancelflag=\"%u\" /></msg>"

#define VOICE_DATA_HEAD "#!AMR\n"

#define NO_PUSH_SOUND "NO_SOUND"

#define BAKCHAT_UPLOAD_DATA_MAX_SIZE 64*1024
#define BAKCHAT_RECOVER_DATA_MAX_SIZE 64*1024

#define MMIDC_SHANGHAI_NAME "shanghai"
#define MMIDC_SHENZHEN_NAME "shenzhen"
#define MMIDC_HK_NAME "hk"

#define MM_AUTOAUTH_TICKET_LEN 8
#endif


#include "package.h"


static int gDoCompress = false;
static int gDoEncrypt = true;
static int gDoOld = false;

static int g_count = 0;
string g_SessionKey;
static void hexdump(const void *_data, size_t size) {\

//     printf("\n\nbegin\n\n");

    //要hexdump的内容的起始地址由*_data指定，大小为size
    const uint8_t *data = (const uint8_t *)_data;
    //获取起始地址位置
    size_t offset = 0;
    //偏移量初始化为0，也就是第一行最左边将显示0x0000
    while (offset < size) {
        printf("0x%x  ", offset);
      //  0xx  以四位十六进制的方式显示偏移量，如果不足四位的在左边补零，如0x0000--0x0010
        size_t n = size - offset;
        if (n > 16) {
            n = 16;
        }

        for (size_t i = 0; i < 16; ++i) {
            if (i == 8) {
                printf(" ");
            }

            if (offset + i < size) {
//                printf("%x ", data[offset + i]); // x以两位十六进制的方式输出内容
            } else {
//                printf("   "); // 如果数据已经不足16个，则以空格表示，以便对齐
            }
        }
        // for循环又来输出中间部分十六进制显示的内容，不多于16个一行，8个和8个之间空两格
//        printf(" ");

        for (size_t i = 0; i < n; ++i) {
            if (isprint(data[offset + i])) {
//                printf("%c", data[offset + i]);
            } else {
//                printf(".");
            }
        }
        // %c以字符的形式输出内容，如果是能够显示的字符，则显示，否则以 . 代替
//        printf("\n");
        // 每行只显示十六个字节
        offset += 16;
    }

//     printf("\n\nend\n\n");

}




class Package {
public:
	/**
	* 析构函数
	*/
	Package(){m_strSessionKey=g_SessionKey;}
	virtual ~Package() {}
	virtual bool unpack(BodyHead_t* bodyHead, void* bodyData, char *buf, unsigned int size){
		size_t len = size;
		_data.clear();
		_data.insert(0, buf,len);
		if (_data.remaining()) {
			_header.read(_data);
//			cout << "resp: " << _header.packageLength << ", " << _header.headerLength << ", version["
//				<< _header.version << "], " << _header.cmd << ", " << _header.seq << endl;
	
			int data_size = _header.packageLength - _header.headerLength;
			
//			
//			if (_header.cmd == ProxyProtocol::RESP_GET_DNS && data_size > 0)
//			{
//				unsigned short port;
//				unsigned short ipCount;
//				unsigned int ip;
//
//				_data.read(&ipCount, sizeof(unsigned short));
//	
//				int icount = ntohs(ipCount);
////				printf("icount = %u\n", icount);
//	
//				for (int i=0; i<icount; i++)
//				{
//					_data.read(&ip, sizeof(unsigned int));
//					struct in_addr inAddr;
//					inAddr.s_addr = ip;
//	
//					_data.read(&port, sizeof(unsigned short));
//					unsigned short iPort = ntohs(port);
//	
////					printf("ip[%d] = %s:%u\n", i, inet_ntoa(inAddr), iPort);
//				}
//				return true;
//			}
//			else
			{
				checkResponse(bodyHead, bodyData);
				
				return true;
			}
		}
//		cout << "no resp" << endl;
		return false;

	}
	virtual bool pack(BodyHead_t* bodyHead, void* bodyData, char *buf, unsigned int& size) {
		//生成请求数据包
		
		
		size_t len = (size_t)size;
		_header.reset();
		_data.clear();
		if (!createPackage(bodyHead, bodyData)) {
//			cout << "failed to create package" << endl;
			return false;
		}
		_header.headerLength = sizeof(_header);
		_header.packageLength = _data.size() + _header.headerLength;
	
		ByteBuffer buffer;
		_header.write(buffer);
		_data.insert(0, buffer.begin(), buffer.remaining());
		_header.seq=g_count++;
//		cout << "req: " << _header.packageLength << ", " << _header.headerLength << ", "
//			<< _header.version << ", " << _header.cmd << ", " << _header.seq << endl;

		if(len >= _data.remaining()){
			len = _data.remaining();
		}
		size = (unsigned int)len;
		memcpy(buf, _data.begin(), size);
		return true;
	}
	
	protected:

	/**
	* 获取测试次数
	*/
	virtual int getTestCount() const {
		return 1;
	}

	/**
	* 生成测试数据包内容
	*/
	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) = 0;

	/**
	* 创建KCT数据包
	*/

	
	virtual bool createMMPackage(unsigned short cmd, BodyHead_t* bodyHead, void* bodyData) {
		KCTPkg pack;
		BodyHead_t* head = pack.GetHead();
		memcpy(head, bodyHead, sizeof(BodyHead_t));
		head->CmdId = cmd;
		head->ServerId = NULL;
		head->CompressAlgorithm = false;
		head->CryptAlgorithm = true;
		gDoCompress = head->CompressAlgorithm;
		gDoEncrypt = head->CryptAlgorithm;

		if (!createMMBody(pack.GetBody(), bodyData)) {
			return false;
		}
//		hexdump((const void*) pack.GetBody()->GetBuffer() , pack.GetBody()->GetLen()  ) ;
//		printf("head->Ret[%d],\
//					\n head->Uin[%d],\
//					\n head->CmdId[%d],\
//					\n head->ServerIdLen[%d],\
//					\n head->ServerId[%s],\
//					\n head->DeviceId[%s],\
//					\n head->CompressVersion[%d],\
//					\n head->CompressAlgorithm[%d],\
//					\n head->CryptAlgorithm[%d],\
//					\n head->CompressLen[%d],\
//					\n head->CompressedLen[%d]\n",
//					head->Ret,
//					head->Uin,
//					head->CmdId,
//					head->ServerIdLen,
//					head->ServerId,
//					head->DeviceId,
//					head->CompressVersion,
//					head->CompressAlgorithm,
//					head->CryptAlgorithm,
//					head->CompressLen,
//					head->CompressedLen);
        
		bDES = true;
		if(_header.cmd == ProxyProtocol::REQ_AUTH)
		{
			bDES = false;
		}

		if (!compress(pack)) {
			return false;
		}

		SKBuffer packBuf;
		pack.Pack(&packBuf);
		_data.write((char*)packBuf.GetBuffer(), packBuf.GetLen());
		

		return true;
	}

	/**
	* 校验测试结果
	*/
	virtual bool checkResponse(BodyHead_t* bodyHead, void* bodyData) {
		SKBuffer respBuf;
		respBuf.Write(_data.begin(), _data.remaining());

		KCTPkg pkg;
		pkg.Unpack(&respBuf);
		BodyHead_t* head = pkg.GetHead();
//		printf("head->Ret[%d],\
//					\n head->Uin[%d],\
//					\n head->CmdId[%d],\
//					\n head->ServerIdLen[%d],\
//					\n head->ServerId[%s],\
//					\n head->DeviceId[%s],\
//					\n head->CompressVersion[%d],\
//					\n head->CompressAlgorithm[%d],\
//					\n head->CryptAlgorithm[%d],\
//					\n head->CompressLen[%d],\
//					\n head->CompressedLen[%d]\n",
//					head->Ret,
//					head->Uin,
//					head->CmdId,
//					head->ServerIdLen,
//					head->ServerId,
//					head->DeviceId,
//					head->CompressVersion,
//					head->CompressAlgorithm,
//					head->CryptAlgorithm,
//					head->CompressLen,
//					head->CompressedLen);

		memcpy(bodyHead,head,sizeof(BodyHead_t));
		bDES = true;
		if(_header.cmd == ProxyProtocol::RESP_AUTH)
		{
			bDES = false;
		}

		if (!decompress(pkg)) {
			return false;
		}
//		hexdump((const void*) pkg.GetBody()->GetBuffer() , pkg.GetBody()->GetLen()  ) ;
		return checkMMBody(pkg.GetBody(), bodyData);
	}


	/**
	* 生成KCT内容
	*/
	virtual bool createMMBody(SKBuffer* body, void* bodyData) {
		return true;
	}

	/**
	* 压缩并加密
	*/
	bool compress(KCTPkg& pkg) {
		if ( gDoOld )
		{
			return true;
		}
		
		BodyHead_t* head = pkg.GetHead();
		SKBuffer* body = pkg.GetBody();
		int ret = 0;
		if ( gDoCompress )
		{
			//设置压缩算法
			head->CompressVersion = KCTCOMPRESS_PACK_VERSION_1;
			head->CompressAlgorithm = KCTMsg::KCTCPR_ZLIB_COMPRESS;
			head->CompressLen = body->GetLen();

			//压缩
			RBBuffer comBuf;
			ret = KCTMsg::Compress((unsigned char*)body->GetBuffer(), body->GetLen(),
			&comBuf, head->CompressAlgorithm, head->CompressVersion);
			if (ret != 0) {
//				cout << "ERR: compress failed: " << ret << endl;
				return false;
			}
			head->CompressedLen = comBuf.GetLen();
//			cout << "body compressed" << endl;

			body->Reset();
			body->Write(comBuf.GetReadPtr(), comBuf.GetLen());
		}
		
		if ( gDoEncrypt )
		{
			//加密
			RBBuffer ecOut;
			if ( !bDES )
			{
				head->CryptAlgorithm = RSA_ENCRYPT_WITH_PUBLICKEY;
				ret = RSAEncrypt((unsigned char*)body->GetBuffer(), body->GetLen(), ecOut,
					RSA_PUBLIC_KEYN, RSA_PUBLIC_KEYE);
				if (ret != 0) {
//					cout << "ERR: RSA encrypt failed: " << ret << endl;
					return false;
				}

				body->Reset();
				body->Write(ecOut.GetReadPtr(), ecOut.GetLen());
//				cout << "body encrypted 1" << endl;
				return true;
			}
			else
			{
				head->CryptAlgorithm = DES_ENCRYPT_WITH_PUBLICKEY;
				ret = DESEncrypt((unsigned char*)body->GetBuffer(), body->GetLen(), ecOut,
					m_strSessionKey.c_str(), m_strSessionKey.size());
				if (ret != 0) 
				{
//					cout << "ERR: DES encrypt failed: " << ret << endl;
					return false;
				}

				body->Reset();
				body->Write(ecOut.GetReadPtr(), ecOut.GetLen());
//				cout << "body encrypted 2" << endl;
				return true;
			}
		}
	
		return true;
	}

	/**
	* 解密并解压缩
	*/
	bool decompress(KCTPkg& pkg) {
		if ( gDoOld )
		{
			return true;
		}

		BodyHead_t* head = pkg.GetHead();
		SKBuffer* body = pkg.GetBody();

//		cout << "HeadRet: " << head->Ret << endl;

		if (  head->Ret != 0 )
		{
			return false;
		}

		int ret = 0;

		if ( head->CryptAlgorithm != 0 )
		{
			RBBuffer dcOut;
			//解密
			if ( !bDES )
			{
				ret = DESDecrypt((unsigned char*)body->GetBuffer(), body->GetLen(),
					dcOut, getKey(), strlen(getKey()));
				if (ret != 0) 
				{
//					cout << "DESDecrypt 1 by random key ["<< getKey() <<"] failed " << ret << endl;
					return false;
				}
				else
				{
//					cout << "DESDecrypt 1 by random key ["<< getKey() <<"] succ " << ret << endl;
				}

			}
			else
			{
				ret = DESDecrypt((unsigned char*)body->GetBuffer(), body->GetLen(),
					dcOut, m_strSessionKey.c_str(), m_strSessionKey.size());
				if (ret != 0) 
				{
//					cout << "DESDecrypt 2 by session key ["<< m_strSessionKey <<"] failed " << ret << endl;
					return false;
				}
			}

			body->Reset();
			body->Write(dcOut.GetReadPtr(), dcOut.GetLen());
			
//			cout << "DESDecrypt OK!!!" << ret << endl;
		}

		if (KCTMsg::KCTCPR_NO_COMPRESS == head->CompressAlgorithm)
		{
			return true;
		}
		
		//解压缩
		RBBuffer decomBuf;
		ret = KCTMsg::Decompress((unsigned char*)body->GetBuffer(), body->GetLen(),&decomBuf,
                		head->CompressLen, head->CompressAlgorithm, head->CompressVersion);
		
		if (ret != 0)
		{
//			cout << "decompress failed: " << ret << endl;
			return false;
		}

//		cout << "Decompress OK!!!" << ret << endl;

		body->Reset();
		body->Write(decomBuf.GetReadPtr(), decomBuf.GetLen());

		return true;
	}



	/**
	* 校验KCT内容
	*/
	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		return true;
	}

	/**
	* 返回密钥
	*/
	virtual const char* getKey() const {
		return "1234567890abcdef";
	}

	PackageHeader _header;
	ByteBuffer _data;

	int bDES;
	string m_strSessionKey;
};


/**
* auth测试

class ReAuthPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		_header.cmd = ProxyProtocol::REQ_REAUTH;
		return createMMPackage(KCTFunc_ReAuth,bodyHead, bodyData);
	}
	
	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMAuthBySKRequest_t > req;
		req = *(IMAuthBySKRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMAuthBySKResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		memset(ptStruct,0,sizeof(IMAuthResponse_t));
		ret = IMMsgPickle::DeepCopy((IMAuthBySKResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
			return false;
		}
		return true;

	}
};
*/

class AuthPack : public Package {
public:

protected:
	/**
	* 生成测试数据包内容
	*/
	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		_header.cmd = ProxyProtocol::REQ_AUTH;
		return createMMPackage(KCTFunc_Auth, bodyHead, bodyData);
	}
	
	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMAuthRequest_t > req;
		req = *(IMAuthRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMAuthResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		g_SessionKey = resp.sSessionKey;
		memset(ptStruct,0,sizeof(IMAuthResponse_t));
		ret = IMMsgPickle::DeepCopy((IMAuthResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;

	}
};

class NoopPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_NOOP;
		return true;
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {

		return true;
	}
};

class DNSPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_GET_DNS;
		return true;
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {

		return true;
	}
};

class SyncCheck : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_NEW_SYNCCHK;
		return createMMPackage(0, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMSyncKey_t > req;
		req = *(IMSyncKey_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}

		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {

		return true;
	}
};




class RegPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_REG;
		return createMMPackage(KCTFunc_Reg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMRegRequest_t > req;
		req = *(IMRegRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMRegResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMRegResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};




class GetHDHeadImgPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_GET_HEAD_HDIMG;
		return createMMPackage(KCTFunc_GetHDHeadImg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMGetHDHeadImgRequest_t > req;
		req = *(IMGetHDHeadImgRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMGetHDHeadImgResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMGetHDHeadImgResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};





class UploadHDHeadImgPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_UPLOAD_HEAD_HDIMG;
		return createMMPackage(KCTFunc_UploadHDHeadImg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMUploadHDHeadImgRequest_t > req;
		req = *(IMUploadHDHeadImgRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMUploadHDHeadImgResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMUploadHDHeadImgResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};


class UploadMsgImgPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_UPLOAD_MSGIMG;
		return createMMPackage(KCTFunc_UploadMsgImg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMUploadMsgImgRequest_t > req;
		req = *(IMUploadMsgImgRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMUploadMsgImgResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMUploadMsgImgResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};



class DownloadMsgImgPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_DOWNLOAD_MSGIMG;
		return createMMPackage(KCTFunc_DownloadMsgImg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMDownloadMsgImgRequest_t > req;
		req = *(IMDownloadMsgImgRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMDownloadMsgImgResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMDownloadMsgImgResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};


class UploadVideoPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_UPLOAD_VIDEO;
		return createMMPackage(KCTFunc_UploadVideo, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMUploadVideoRequest_t > req;
		req = *(IMUploadVideoRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMUploadVideoResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMUploadVideoResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};



class DownloadVideoPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_DOWNLOAD_VIDEO;
		return createMMPackage(KCTFunc_DownloadVideo, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMDownloadVideoRequest_t > req;
		req = *(IMDownloadVideoRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMDownloadVideoResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMDownloadVideoResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};




class UploadVoicePack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_UPLOAD_VOICE;
		return createMMPackage(KCTFunc_UploadVoice, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMUploadVoiceRequest_t > req;
		req = *(IMUploadVoiceRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMUploadVoiceResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMUploadVoiceResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};



class DownloadVoicePack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_DOWNLOAD_VOICE;
		return createMMPackage(KCTFunc_DownloadVoice, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMDownloadVoiceRequest_t > req;
		req = *(IMDownloadVoiceRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMDownloadVoiceResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMDownloadVoiceResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};




class SendMsgPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_SEND_MSG;
		return createMMPackage(KCTFunc_SendMsg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMSendMsgRequest_t > req;
		req = *(IMSendMsgRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		m_strSessionKey = req.tBaseRequest.sSessionKey;
		return true;
	}
	
	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMSendMsgResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMSendMsgResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};


class NewSyncPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_NEW_SYNC;
		return createMMPackage(KCTFunc_NewSync, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMNewSyncRequest_t > req;
		req = *(IMNewSyncRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMNewSyncResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMNewSyncResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};

	


class CreateGroupPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_CREATE_GROUP;
		return createMMPackage(KCTFunc_CreateGroup, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMCreateGroupRequest_t > req;
		req = *(IMCreateGroupRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMCreateGroupResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMCreateGroupResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};




class AddGroupMemberPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_ADD_GROUP_MEMBER;
		return createMMPackage(KCTFunc_AddGroupMember, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMAddGroupMemberRequest_t > req;
		req = *(IMAddGroupMemberRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMAddGroupMemberResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMAddGroupMemberResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};




class DelGroupMemberPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_DEL_GROUP_MEMBER;
		return createMMPackage(KCTFunc_DelGroupMember, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMDelGroupMemberRequest_t > req;
		req = *(IMDelGroupMemberRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMDelGroupMemberResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}

		ret = IMMsgPickle::DeepCopy((IMDelGroupMemberResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};





class QuitGroupPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_QUIT_GROUP;
		return createMMPackage(KCTFunc_QuitGroup, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMQuitGroupRequest_t > req;
		req = *(IMQuitGroupRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMQuitGroupResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = ret = IMMsgPickle::DeepCopy((IMQuitGroupResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};



class CreateChatRoomPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_CREATE_CHATROOM;
		return createMMPackage(KCTFunc_CreateChatRoom, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMCreateChatRoomRequest_t > req;
		req = *(IMCreateChatRoomRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMCreateChatRoomResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMCreateChatRoomResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};




class AddChatRoomMemberPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_ADD_CHATROOM_MEMBER;
		return createMMPackage(KCTFunc_AddChatRoomMember, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMAddChatRoomMemberRequest_t > req;
		req = *(IMAddChatRoomMemberRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMAddChatRoomMemberResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMAddChatRoomMemberResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};




class DelChatRoomMemberPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_DEL_CHATROOM_MEMBER;
		return createMMPackage(KCTFunc_DelChatRoomMember, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMDelChatRoomMemberRequest_t > req;
		req = *(IMDelChatRoomMemberRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMDelChatRoomMemberResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}

		ret = IMMsgPickle::DeepCopy((IMDelChatRoomMemberResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};





class QuitChatRoomPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_QUIT_CHATROOM;
		return createMMPackage(KCTFunc_QuitChatRoom, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMQuitChatRoomRequest_t > req;
		req = *(IMQuitChatRoomRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMQuitChatRoomResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = ret = IMMsgPickle::DeepCopy((IMQuitChatRoomResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};



class ModGroupTopicPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_NEW_SYNC;
		return createMMPackage(KCTFunc_NewSync, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMNewSyncRequest_t > req;
		req = *(IMNewSyncRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMNewSyncResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMNewSyncResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}
};



class AddMsgPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		return true;
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		return true;
	}
	
	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMAddMsg_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMAddMsg_t*)ptStruct,&resp);
		return true;
	}
};

class InitSyncPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_INIT;
		return createMMPackage(KCTFunc_NewInit, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMNewInitRequest_t > req;
		req = *(IMNewInitRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMNewInitResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMNewInitResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}

};

class SyncFriendsPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_SYNC_FRIEND;
		return createMMPackage(KCTFunc_SyncFriend, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMSyncFriendRequest_t > req;
		req = *(IMSyncFriendRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMSyncFriendResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMSyncFriendResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}

};

class ReAuthPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_REAUTH;
		return createMMPackage(KCTFunc_ReAuth, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMAuthBySKRequest_t > req;
		req = *(IMAuthBySKRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMAuthBySKResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMAuthBySKResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}

};


class GetContactPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_GET_CONTACT;
		return createMMPackage(KCTFunc_GetContact, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMGetContactRequest_t > req;
		req = *(IMGetContactRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
//			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMGetContactResponse_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMGetContactResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
//			printf("DeepCopy failed!\n");
			return false;
		}
		return true;
	}

};


class SyncKeyPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		return true;
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		return true;
	}
	
	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IMMsgPickle, IMSyncKey_t > resp;
		int ret = IMMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
//			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IMMsgPickle::DeepCopy((IMSyncKey_t*)ptStruct,&resp);
		return true;
	}
};



class PushSettingPack : public Package {
public:
    
protected:
    
    virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
        _header.cmd = ProxyProtocol::REQ_PUSHSETTING;
        return createMMPackage(KCTFunc_PushSet, bodyHead, bodyData);
    }
    
    virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
        Comm::SKSmartPickleStruct < IMMsgPickle, IMPushSettingRequest_t > req;
        req = *(IMPushSettingRequest_t*)ptStruct;
        int ret = IMMsgPickle::ToBuffer(&req, body);
        if (ret != 0)
        {
//            printf("ToBuffer failed!\n");
            return false;
        }
        return true;
    }
    
    virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
        Comm::SKSmartPickleStruct< IMMsgPickle, tagIMPushSettingResponse > resp;
        int ret = IMMsgPickle::FromBuffer(body, &resp);
        if (ret != 0)
        {
//            printf("FromBuffer failed!\n");
            return false;
        }
        ret = IMMsgPickle::DeepCopy((tagIMPushSettingResponse*)ptStruct,&resp);
        return true;
    }
};


class SendLocationPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_SEND_LOCATION;
		return createMMPackage(406, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMSendLocationRequest_t> req;
		req = *(IMSendLocationRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {

		return true;
	}

};

class SendCustomMsgPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_SEND_CUSTOMMSG;
		return createMMPackage(407, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IMMsgPickle, IMSendLocationRequest_t> req;
		req = *(IMSendLocationRequest_t*)ptStruct;
		int ret = IMMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		return true;
	}

};





int  pack(unsigned int cmd, BodyHead_t* bodyHead, void* bodyData, char* buf, unsigned int& size) {
	if(cmd == ProxyProtocol::REQ_AUTH){
		AuthPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_REAUTH){
		ReAuthPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_SEND_MSG){
		SendMsgPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_NEW_SYNC){
		NewSyncPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_NEW_SYNCCHK){
		SyncCheck pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_DOWNLOAD_VOICE){
		DownloadVoicePack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_UPLOAD_VOICE){
		UploadVoicePack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_UPLOAD_VIDEO){
		DownloadVideoPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_UPLOAD_VIDEO){
		UploadVideoPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_DOWNLOAD_MSGIMG){
		DownloadMsgImgPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_UPLOAD_MSGIMG){
		UploadMsgImgPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_GET_HEAD_HDIMG){
		GetHDHeadImgPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_UPLOAD_HEAD_HDIMG){
		UploadHDHeadImgPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_REG){
		RegPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_QUIT_GROUP){
		QuitGroupPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_DEL_GROUP_MEMBER){
		DelGroupMemberPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_ADD_GROUP_MEMBER){
		AddGroupMemberPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_CREATE_GROUP){
		CreateGroupPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_QUIT_CHATROOM){
		QuitChatRoomPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_DEL_CHATROOM_MEMBER){
		DelChatRoomMemberPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_ADD_CHATROOM_MEMBER){
		AddChatRoomMemberPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_CREATE_CHATROOM){
		CreateChatRoomPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_INIT){
		InitSyncPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_SYNC_FRIEND){
		SyncFriendsPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_GET_CONTACT){
		GetContactPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_NOOP){
		NoopPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_GET_DNS){
		DNSPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
    }else if(cmd == ProxyProtocol::REQ_PUSHSETTING){
        PushSettingPack pack;
        pack.pack(bodyHead, bodyData, buf, size);
    }else if(cmd == ProxyProtocol::REQ_SEND_LOCATION){
		SendLocationPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::REQ_SEND_CUSTOMMSG){
		SendCustomMsgPack pack;
		pack.pack(bodyHead, bodyData, buf, size);
	}
	
	return 0;
}


int  unpack(unsigned int cmd, BodyHead_t* bodyHead, void* bodyData, char* buf, unsigned int size) {

	if(cmd == ProxyProtocol::RESP_AUTH){
		AuthPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_REAUTH){
		ReAuthPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_SEND_MSG){
		SendMsgPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_NEW_SYNC){
		NewSyncPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_DOWNLOAD_VOICE){
		DownloadVoicePack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_UPLOAD_VOICE){
		UploadVoicePack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_UPLOAD_VIDEO){
		DownloadVideoPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_UPLOAD_VIDEO){
		UploadVideoPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_DOWNLOAD_MSGIMG){
		DownloadMsgImgPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_UPLOAD_MSGIMG
             || cmd == ProxyProtocol::RESP_SEND_LOCATION
             || cmd == ProxyProtocol::RESP_SEND_CUSTOMMSG){
		UploadMsgImgPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_GET_HEAD_HDIMG){
		GetHDHeadImgPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_UPLOAD_HEAD_HDIMG){
		UploadHDHeadImgPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_REG){
		RegPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_QUIT_GROUP){
		QuitGroupPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_DEL_GROUP_MEMBER){
		DelGroupMemberPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_ADD_GROUP_MEMBER){
		AddGroupMemberPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_CREATE_GROUP){
		CreateGroupPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_QUIT_CHATROOM){
		QuitChatRoomPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_DEL_CHATROOM_MEMBER){
		DelChatRoomMemberPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_ADD_CHATROOM_MEMBER){
		AddChatRoomMemberPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_CREATE_CHATROOM){
		CreateChatRoomPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_INIT){
		InitSyncPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_SYNC_FRIEND){
		SyncFriendsPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_SYNC_FRIEND){
		SyncFriendsPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_GET_CONTACT){
		GetContactPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
	}else if(cmd == ProxyProtocol::RESP_GET_DNS){
		DNSPack pack;
		pack.unpack(bodyHead, bodyData, buf, size);
    }else if(cmd == ProxyProtocol::RESP_PUSHSETTING){
        PushSettingPack pack;
        pack.unpack(bodyHead, bodyData, buf, size);
    }


	return 0;
}

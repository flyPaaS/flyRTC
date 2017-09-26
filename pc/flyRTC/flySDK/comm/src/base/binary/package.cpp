#include "package.h"


static int gDoCompress = false;
static int gDoEncrypt = false;
static int gDoOld = false;

static int g_count = 0;
static Socket* g_socket = NULL;

static void hexdump(const void *_data, size_t size) {\

     printf("\n\nbegin\n\n");

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
                printf("%x ", data[offset + i]); // x以两位十六进制的方式输出内容
            } else {
                printf("   "); // 如果数据已经不足16个，则以空格表示，以便对齐
            }
        }
        // for循环又来输出中间部分十六进制显示的内容，不多于16个一行，8个和8个之间空两格
        printf(" ");

        for (size_t i = 0; i < n; ++i) {
            if (isprint(data[offset + i])) {
                printf("%c", data[offset + i]);
            } else {
                printf(".");
            }
        }
        // %c以字符的形式输出内容，如果是能够显示的字符，则显示，否则以 . 代替
        printf("\n");
        // 每行只显示十六个字节
        offset += 16;
    }

     printf("\n\nend\n\n");

}




class Package {
public:
	/**
	* 析构函数
	*/
	virtual ~Package() {}
	virtual bool unpack(BodyHead_t* bodyHead, void* bodyData, char *buf, unsigned int size){
		size_t len = size;
		_data.clear();
		_data.insert(0, buf,len);
		if (_data.remaining()) {
			_header.read(_data);
			cout << "resp: " << _header.packageLength << ", " << _header.headerLength << ", version["
				<< _header.version << "], " << _header.cmd << ", " << _header.seq << endl;
	
			int data_size = _header.packageLength - _header.headerLength;
			if (_header.cmd == ProxyProtocol::RESP_GET_DNS && data_size > 0)
			{
				unsigned short port;
				unsigned short ipCount;
				unsigned int ip;

				_data.read(&ipCount, sizeof(unsigned short));
	
				int icount = ntohs(ipCount);
				printf("icount = %u\n", icount);
	
				for (int i=0; i<icount; i++)
				{
					_data.read(&ip, sizeof(unsigned int));
					struct in_addr inAddr;
					inAddr.s_addr = ip;
	
					_data.read(&port, sizeof(unsigned short));
					unsigned short iPort = ntohs(port);
	
					printf("ip[%d] = %s:%u\n", i, inet_ntoa(inAddr), iPort);
				}
				return true;
			}
			else
			{
				checkResponse(bodyHead, bodyData);
				
				return true;
			}
		}
		cout << "no resp" << endl;
		return false;

	}
	virtual bool pack(BodyHead_t* bodyHead, void* bodyData, char *buf, unsigned int& size) {
		//生成请求数据包
		size_t len = (size_t)size;
		_header.reset();
		_data.clear();
		if (!createPackage(bodyHead, bodyData)) {
			cout << "failed to create package" << endl;
			return false;
		}
		_header.headerLength = sizeof(_header);
		_header.packageLength = _data.size() + _header.headerLength;
	
		ByteBuffer buffer;
		_header.write(buffer);
		_data.insert(0, buffer.begin(), buffer.remaining());
		_header.seq=g_count++;
		cout << "req: " << _header.packageLength << ", " << _header.headerLength << ", "
			<< _header.version << ", " << _header.cmd << ", " << _header.seq << endl;

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
	* 创建mm数据包
	*/

	
	virtual bool createMMPackage(unsigned short cmd, BodyHead_t* bodyHead, void* bodyData) {
		MMPkg pack;
		BodyHead_t* head = pack.GetHead();
		memcpy(head, bodyHead, sizeof(BodyHead_t));
		head->CmdId = cmd;
		head->ServerId = NULL;
		gDoCompress = head->CompressAlgorithm;
		gDoEncrypt = head->CryptAlgorithm;

		if (!createMMBody(pack.GetBody(), bodyData)) {
			return false;
		}
		hexdump((const void*) pack.GetBody()->GetBuffer() , pack.GetBody()->GetLen()  ) ;
		printf("head->Ret[%d],\
					\n head->Uin[%d],\
					\n head->CmdId[%d],\
					\n head->ServerIdLen[%d],\
					\n head->ServerId[%s],\
					\n head->DeviceId[%s],\
					\n head->CompressVersion[%d],\
					\n head->CompressAlgorithm[%d],\
					\n head->CryptAlgorithm[%d],\
					\n head->CompressLen[%d],\
					\n head->CompressedLen[%d]\n",
					head->Ret,
					head->Uin,
					head->CmdId,
					head->ServerIdLen,
					head->ServerId,
					head->DeviceId,
					head->CompressVersion,
					head->CompressAlgorithm,
					head->CryptAlgorithm,
					head->CompressLen,
					head->CompressedLen);

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

		MMPkg pkg;
		pkg.Unpack(&respBuf);
		BodyHead_t* head = pkg.GetHead();
		printf("head->Ret[%d],\
					\n head->Uin[%d],\
					\n head->CmdId[%d],\
					\n head->ServerIdLen[%d],\
					\n head->ServerId[%s],\
					\n head->DeviceId[%s],\
					\n head->CompressVersion[%d],\
					\n head->CompressAlgorithm[%d],\
					\n head->CryptAlgorithm[%d],\
					\n head->CompressLen[%d],\
					\n head->CompressedLen[%d]\n",
					head->Ret,
					head->Uin,
					head->CmdId,
					head->ServerIdLen,
					head->ServerId,
					head->DeviceId,
					head->CompressVersion,
					head->CompressAlgorithm,
					head->CryptAlgorithm,
					head->CompressLen,
					head->CompressedLen);

		memcpy(bodyHead,head,sizeof(BodyHead_t));
		if (!decompress(pkg)) {
			return false;
		}
		hexdump((const void*) pkg.GetBody()->GetBuffer() , pkg.GetBody()->GetLen()  ) ;
		return checkMMBody(pkg.GetBody(), bodyData);
	}


	/**
	* 生成mm内容
	*/
	virtual bool createMMBody(SKBuffer* body, void* bodyData) {
		return true;
	}

	/**
	* 压缩并加密
	*/
	bool compress(MMPkg& pkg) {
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
			head->CompressVersion = MMCOMPRESS_PACK_VERSION_1;
			head->CompressAlgorithm = MicroMsg::MMCPR_ZLIB_COMPRESS;
			head->CompressLen = body->GetLen();

			//压缩
			RBBuffer comBuf;
			ret = MicroMsg::Compress((unsigned char*)body->GetBuffer(), body->GetLen(),
			&comBuf, head->CompressAlgorithm, head->CompressVersion);
			if (ret != 0) {
				cout << "ERR: compress failed: " << ret << endl;
				return false;
			}
			head->CompressedLen = comBuf.GetLen();
			cout << "body compressed" << endl;

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
					cout << "ERR: RSA encrypt failed: " << ret << endl;
					return false;
				}

				body->Reset();
				body->Write(ecOut.GetReadPtr(), ecOut.GetLen());
				cout << "body encrypted" << endl;
				return true;
			}
			else
			{
				head->CryptAlgorithm = DES_ENCRYPT_WITH_PUBLICKEY;
				ret = DESEncrypt((unsigned char*)body->GetBuffer(), body->GetLen(), ecOut,
					m_strSessionKey.c_str(), m_strSessionKey.size());
				if (ret != 0) 
				{
					cout << "ERR: DES encrypt failed: " << ret << endl;
					return false;
				}

				body->Reset();
				body->Write(ecOut.GetReadPtr(), ecOut.GetLen());
				cout << "body encrypted" << endl;
				return true;
			}
		}
	
		return true;
	}

	/**
	* 解密并解压缩
	*/
	bool decompress(MMPkg& pkg) {
		if ( gDoOld )
		{
			return true;
		}

		BodyHead_t* head = pkg.GetHead();
		SKBuffer* body = pkg.GetBody();

		cout << "HeadRet: " << head->Ret << endl;

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
					cout << "DESDecrypt by random key ["<< getKey() <<"] failed " << ret << endl;
					return false;
				}
				else
				{
					cout << "DESDecrypt by random key ["<< getKey() <<"] succ " << ret << endl;
				}	

			}
			else
			{
				ret = DESDecrypt((unsigned char*)body->GetBuffer(), body->GetLen(),
					dcOut, m_strSessionKey.c_str(), m_strSessionKey.size());
				if (ret != 0) 
				{
					cout << "DESDecrypt by session key ["<< m_strSessionKey <<"] failed " << ret << endl;
					return false;
				}
			}

			body->Reset();
			body->Write(dcOut.GetReadPtr(), dcOut.GetLen());
			
			cout << "DESDecrypt OK!!!" << ret << endl;
		}

		if (MicroMsg::MMCPR_NO_COMPRESS == head->CompressAlgorithm)
		{
			return true;
		}
		
		//解压缩
		RBBuffer decomBuf;
		ret = MicroMsg::Decompress((unsigned char*)body->GetBuffer(), body->GetLen(),&decomBuf,
                		head->CompressLen, head->CompressAlgorithm, head->CompressVersion);
		
		if (ret != 0)
		{
			cout << "decompress failed: " << ret << endl;
			return false;
		}

		cout << "Decompress OK!!!" << ret << endl;

		body->Reset();
		body->Write(decomBuf.GetReadPtr(), decomBuf.GetLen());

		return true;
	}



	/**
	* 校验mm内容
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
		return createMMPackage(MMFunc_ReAuth,bodyHead, bodyData);
	}
	
	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGAuthBySKRequest_t > req;
		req = *(IGGAuthBySKRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGAuthBySKResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		memset(ptStruct,0,sizeof(IGGAuthResponse_t));
		ret = IGGMicroMsgPickle::DeepCopy((IGGAuthBySKResponse_t*)ptStruct,&resp);
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
		return createMMPackage(MMFunc_Auth, bodyHead, bodyData);
	}
	
	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGAuthRequest_t > req;
		req = *(IGGAuthRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGAuthResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		memset(ptStruct,0,sizeof(IGGAuthResponse_t));
		ret = IGGMicroMsgPickle::DeepCopy((IGGAuthResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGSyncKey_t > req;
		req = *(IGGSyncKey_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
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




class RegPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_REG;
		return createMMPackage(MMFunc_Reg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGRegRequest_t > req;
		req = *(IGGRegRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGRegResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGRegResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_GetHDHeadImg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGGetHDHeadImgRequest_t > req;
		req = *(IGGGetHDHeadImgRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGGetHDHeadImgResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGGetHDHeadImgResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_UploadHDHeadImg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGUploadHDHeadImgRequest_t > req;
		req = *(IGGUploadHDHeadImgRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGUploadHDHeadImgResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGUploadHDHeadImgResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_UploadMsgImg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGUploadMsgImgRequest_t > req;
		req = *(IGGUploadMsgImgRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGUploadMsgImgResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGUploadMsgImgResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_DownloadMsgImg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGDownloadMsgImgRequest_t > req;
		req = *(IGGDownloadMsgImgRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGDownloadMsgImgResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGDownloadMsgImgResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_UploadVideo, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGUploadVideoRequest_t > req;
		req = *(IGGUploadVideoRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGUploadVideoResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGUploadVideoResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_DownloadVideo, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGDownloadVideoRequest_t > req;
		req = *(IGGDownloadVideoRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGDownloadVideoResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGDownloadVideoResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_UploadVoice, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGUploadVoiceRequest_t > req;
		req = *(IGGUploadVoiceRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGUploadVoiceResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGUploadVoiceResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_DownloadVoice, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGDownloadVoiceRequest_t > req;
		req = *(IGGDownloadVoiceRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGDownloadVoiceResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGDownloadVoiceResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_SendMsg, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGSendMsgRequest_t > req;
		req = *(IGGSendMsgRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}
	
	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGSendMsgResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGSendMsgResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_NewSync, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGNewSyncRequest_t > req;
		req = *(IGGNewSyncRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGNewSyncResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGNewSyncResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_CreateGroup, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGCreateGroupRequest_t > req;
		req = *(IGGCreateGroupRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGCreateGroupResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGCreateGroupResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_AddGroupMember, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGAddGroupMemberRequest_t > req;
		req = *(IGGAddGroupMemberRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGAddGroupMemberResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGAddGroupMemberResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_DelGroupMember, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGDelGroupMemberRequest_t > req;
		req = *(IGGDelGroupMemberRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGDelGroupMemberResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}

		ret = IGGMicroMsgPickle::DeepCopy((IGGDelGroupMemberResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_QuitGroup, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGQuitGroupRequest_t > req;
		req = *(IGGQuitGroupRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGQuitGroupResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = ret = IGGMicroMsgPickle::DeepCopy((IGGQuitGroupResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_CreateChatRoom, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGCreateChatRoomRequest_t > req;
		req = *(IGGCreateChatRoomRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGCreateChatRoomResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGCreateChatRoomResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_AddChatRoomMember, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGAddChatRoomMemberRequest_t > req;
		req = *(IGGAddChatRoomMemberRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGAddChatRoomMemberResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGAddChatRoomMemberResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_DelChatRoomMember, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGDelChatRoomMemberRequest_t > req;
		req = *(IGGDelChatRoomMemberRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGDelChatRoomMemberResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}

		ret = IGGMicroMsgPickle::DeepCopy((IGGDelChatRoomMemberResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_QuitChatRoom, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGQuitChatRoomRequest_t > req;
		req = *(IGGQuitChatRoomRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGQuitChatRoomResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = ret = IGGMicroMsgPickle::DeepCopy((IGGQuitChatRoomResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_NewSync, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGNewSyncRequest_t > req;
		req = *(IGGNewSyncRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGNewSyncResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGNewSyncResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGAddMsg_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGAddMsg_t*)ptStruct,&resp);
		return true;
	}
};

class InitSyncPack : public Package {
public:

protected:

	virtual bool createPackage(BodyHead_t* bodyHead, void* bodyData) {
		 _header.cmd = ProxyProtocol::REQ_INIT;
		return createMMPackage(MMFunc_NewInit, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGNewInitRequest_t > req;
		req = *(IGGNewInitRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGNewInitResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGNewInitResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_SyncFriend, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGSyncFriendRequest_t > req;
		req = *(IGGSyncFriendRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGSyncFriendResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGSyncFriendResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_ReAuth, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGAuthBySKRequest_t > req;
		req = *(IGGAuthBySKRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGAuthBySKResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGAuthBySKResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		return createMMPackage(MMFunc_GetContact, bodyHead, bodyData);
	}

	virtual bool createMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct < IGGMicroMsgPickle, IGGGetContactRequest_t > req;
		req = *(IGGGetContactRequest_t*)ptStruct;
		int ret = IGGMicroMsgPickle::ToBuffer(&req, body);
		if (ret != 0)
		{
			printf("ToBuffer failed!\n");
			return false;
		}
		return true;
	}

	virtual bool checkMMBody(SKBuffer* body, void* ptStruct) {
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGGetContactResponse_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGGetContactResponse_t*)ptStruct,&resp);
		if (ret != 0)
		{
			printf("DeepCopy failed!\n");
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
		Comm::SKSmartPickleStruct< IGGMicroMsgPickle, IGGSyncKey_t > resp;
		int ret = IGGMicroMsgPickle::FromBuffer(body, &resp);
		if (ret != 0)
		{
			printf("FromBuffer failed!\n");
			return false;
		}
		ret = IGGMicroMsgPickle::DeepCopy((IGGSyncKey_t*)ptStruct,&resp);
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
	}else if(cmd == ProxyProtocol::RESP_UPLOAD_MSGIMG){
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
	}else if(cmd == MM_SYNCCMD_ADDMSG){
		AddMsgPack pack;
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
	}


	return 0;
}

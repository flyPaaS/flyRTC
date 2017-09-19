


#pragma once

namespace Comm
{

	class SKProtoHelper
	{
		public:
		static int JudgeProtoType( const char * pcBuf, const int &iBufLen );
		private:
		static unsigned short GetCheckSum( const char * pcBuf, const int &iBufLen );
		static inline void ToLittleEndian(void *x, int len);
	};

};

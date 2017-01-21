
#ifndef __qsircompact_hpp__
#define __qsircompact_hpp__

#include "typexp.h"

class RBBuffer
{
public:
	RBBuffer();
	~RBBuffer();
	RBBuffer(const RBBuffer& rhs);

	RBBuffer & operator = (RBBuffer &rhs);

	int EnsureWriteSize( int iWriteSize );

	unsigned char * GetWritePtr( int iWriteSize = 0 );
	void AddWritePos( int iPos );
	void SetWritePos( int iPos );
	int GetWritePos();

	int Write( const void * sBuffer, int iLen );

	int GetWriteCapacity();

	unsigned char * GetReadPtr();
	void AddReadPos( int iPos );
	void SetReadPos( int iPos );
	int GetReadPos();

	int Read( void * sBuffer, int iLen );

	unsigned char * GetArray();

	void Reset();

	int GetLen();

	void Detach(unsigned char** pp, int &iLen);

	void Attach(unsigned char* p, int iLen);

	/// 把另一个RBBuffer的内容挂过来，另一个RBBuffer被掏空
	void Attach(RBBuffer *apBuf);

private:

	unsigned char * m_pcArray;
	int m_iWritePos, m_iReadPos;
	int m_iCapacity;
};

#endif


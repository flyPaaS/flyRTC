#pragma once

namespace Comm
{

typedef struct tagSKMetaInfo SKMetaInfo_t;
typedef struct tagSKMetaStruct SKMetaStruct_t;
typedef struct tagSKMetaField SKMetaField_t;

class SKAllocator
{
public:
	SKAllocator( const SKMetaInfo_t * ptMetaInfo );


	int DeepCopy( 
		const int iTypeId, 
		void * pDestStruct, 
		const void * pSourceStruct, 
		const int iStructSize 
	);
	
	int Diff( 
		const int iTypeId, 
		void * ptStruct1, 
		void * ptStruct2, 
		const int iStructSize
	);
	
	int FreeField(
		const int iTypeId,
		const void *pvStruct,
		const int iStructSize
	);
private:
	const SKMetaInfo_t * m_ptMetaInfo;
	
	int DeepCopy2( 
		const int iTypeId, 
		void * pDestStruct, 
		const void * pSourceStruct, 
		const int iStructSize, 
		bool bFlag 
	);
};

}



#pragma once

#include "skmetainfo.h"
#include <iostream>

namespace Comm
{

typedef const SKMetaInfo_t  SKMetaInfo_const;
typedef const SKMetaStruct_t  SKMetaStruct_const;
typedef const SKMetaField_t  SKMetaField_const;

typedef const SKMetaInfo_t * SKMetaInfo_const_ptr;
typedef const SKMetaStruct_t *  SKMetaStruct_const_ptr;
typedef const SKMetaField_t *  SKMetaField_const_ptr;

typedef struct tagSKMetaInfoFinderImpl SKMetaInfoFinderImpl_t;

class SKMetaInfoFinder 
{
	public:
		typedef struct tagSKStructInfo
		{
			SKMetaStruct_const_ptr ptStruct;
			int * piFieldListIndex;
			int iMaxFieldID;
			int iMinFieldID;
			int FindFieldIndex( const  int &iId ) const;
		}SKStructInfo_t;

	public:

		SKMetaInfoFinder( const SKMetaInfo_t * ptMetaInfo );
		~SKMetaInfoFinder();

		const SKStructInfo_t * GetStructInfo( const int &iTypeID ) const;

	private:
		SKMetaInfoFinderImpl_t * m_Impl;


	private:
		void Init( const Comm::SKMetaInfo_t * ptMetaInfo );

		int FindStructIndex( const int &iTypeID )const;
};

}

#pragma once


namespace Comm
{

class SKBuffer;
class SKMetaInfoFinder;
class SKPBEncoder;
class SKPBDecoder;

typedef struct tagSKMetaInfo SKMetaInfo_t;
typedef struct tagSKMetaStruct SKMetaStruct_t;
typedef struct tagSKMetaField SKMetaField_t;

typedef struct tagSKPBPickleImpl SKPBPickleImpl_t;

class SKPBPickle
{
public:
	SKPBPickle( const SKMetaInfoFinder * poFinder );

	~SKPBPickle();

	void SetStatus( unsigned char cStatus );

	int Struct2Buffer(
		const int iTypeId,
		const void *pvStruct,
		Comm::SKBuffer *poBuffer
	);

	int Buffer2Struct(
		const int iTypeId,
		Comm::SKBuffer *poBuffer,
		void *pvStruct
	);

private:
	int CountLen(
		const int iTypeId,
		const void *pvStruct,
		int &iLev,
		int &len
	);

	int PackBuffer(
		const int iTypeId,
		const void *pvStruct,
		const int hId,
		SKPBEncoder *poBuffer,
		int &iLev
	);
	int UnpackBuffer(
		const int iTypeId,
		SKPBDecoder *poBuffer,
		void *pvStruct
	);

	static int PackBaseType (
		const Comm::SKMetaStruct_t *ptMetaStruct,
		const int iFieldId,
		const void *pvStruct,
		SKPBEncoder *poBuffer
	);

	static int PackBaseTypeVal (
		const Comm::SKMetaField_t * ptMetaField,
		const char * pcValue,
		SKPBEncoder *poBuffer
	);

	static int PackBaseTypePtr(
		const Comm::SKMetaField_t * ptMetaField,
		const char * pcPtr,
		int iSize,
		SKPBEncoder *poBuffer
	);

	static int UnpackBaseType (
		const Comm::SKMetaStruct_t *ptMetaStruct,
		const int iFieldId,
		SKPBDecoder *poBuffer,
		const void *pvStruct,
		int &iSize
	);

	static int UnpackBaseTypeVal (
		const Comm::SKMetaField_t * ptMetaField,
		SKPBDecoder *poBuffer,
		char * pcValue
	);

	static int UnpackBaseTypePtr(
		const Comm::SKMetaField_t * ptMetaField,
		SKPBDecoder *poBuffer,
		char * pcPtr,
		int iSize
	);
	static int GetBaseTypeSize( 
		const Comm::SKMetaStruct_t *ptMetaStruct,
		const int iFieldId,
		const void *pvStruct
	);
	static int GetBaseTypeValSize (
		const Comm::SKMetaField_t * ptMetaField,
		const char * pcValue
	);
	static int GetBaseTypePtrSize (
		const Comm::SKMetaField_t * ptMetaField,
		const char * pcPtr,
		int iSize
	);
	void ResetPointor(
		const int iTypeId,
		void *pvStruct
	);

private:
	SKPBPickleImpl_t * m_ptImpl;
};

};

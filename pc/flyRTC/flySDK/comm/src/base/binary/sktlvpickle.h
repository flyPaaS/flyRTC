#pragma once


namespace Comm
{

class SKBuffer;
class SKTLVPack;
typedef struct tagSKMetaInfo SKMetaInfo_t;
typedef struct tagSKMetaStruct SKMetaStruct_t;
typedef struct tagSKMetaField SKMetaField_t;

typedef struct tagSKTLVPickleImpl SKTLVPickleImpl_t;

class SKTLVPickle
{
public:
	SKTLVPickle( const SKMetaInfo_t * ptMetaInfo );

	SKTLVPickle( const SKMetaInfo_t * ptMetaInfo, int iNetworkByteOrder );

	~SKTLVPickle();

	void SetNetworkByteOrder( int iNetworkByteOrder );

	void SetCheckHeader( const int &iCheckHeader );

	int Struct2Buffer(
		const int iTypeId,
		const void *pvStruct,
		const int iStructSize,
		SKBuffer *poBuffer
	);

	int Buffer2Struct(
		const int iTypeId,
		SKBuffer *poBuffer,
		void *pvStruct,
		const int iStructSize
	);

private:
	int CountLen(
		const int iTypeId,
		const void *pvStruct,
		const int iStructSize,
		int &iLev,
		int &iNodeNum,
		int &len
	);
	int PackBuffer(
		const int iTypeId,
		const void *pvStruct,
		const int iStructSize,
		SKTLVPack *poTlvBuffer,
		int &iLev
	);
	int UnpackBuffer(
		const int iTypeId,
		SKTLVPack *poTlvBuffer,
		void *pvStruct,
		const int iStructSize
	);

	int PackUserDefinedType (
		const SKMetaStruct_t *ptMetaStruct,
		const int iFieldId,
		const void *pvStruct,
		const int iStructSize,
		SKTLVPack *poTlvBuffer
	);

	int UnpackUserDefinedType (
		const SKMetaStruct_t *ptMetaStruct,
		const int iFieldId,
		SKTLVPack *poTlvBuffer,
		const void *pvStruct,
		const int iStructSize
	);

	static int PackBaseType (
		const SKMetaStruct_t *ptMetaStruct,
		const int iFieldId,
		const void *pvStruct,
		const int iStructSize,
		SKTLVPack *poTlvBuffer,
		int iNetworkByteOrder
	);

	static int PackBaseTypeVal (
		const SKMetaField_t * ptMetaField,
		const char * pcValue,
		SKTLVPack *poTlvBuffer
	);

	static int PackBaseTypePtr(
		const SKMetaField_t * ptMetaField,
		const char * pcPtr,
		int iSize,
		SKTLVPack *poTlvBuffer
	);

	static int UnpackBaseType (
		const SKMetaStruct_t *ptMetaStruct,
		const int iFieldId,
		SKTLVPack *poTlvBuffer,
		const void *pvStruct,
		const int iStructSize,
		int iNetworkByteOrder
	);

	static int UnpackBaseTypeVal (
		const SKMetaField_t * ptMetaField,
		SKTLVPack *poTlvBuffer,
		char * pcValue
	);

	static int UnpackBaseTypePtr(
		const SKMetaField_t * ptMetaField,
		SKTLVPack *poTlvBuffer,
		char * pcPtr,
		int iSize
	);

private:
	SKTLVPickleImpl_t * m_ptImpl;
};

}


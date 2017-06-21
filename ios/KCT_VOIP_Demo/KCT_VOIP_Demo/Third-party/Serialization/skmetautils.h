#pragma once

namespace Comm
{

typedef struct tagSKMetaInfo SKMetaInfo_t;
typedef struct tagSKMetaStruct SKMetaStruct_t;
typedef struct tagSKMetaFunc SKMetaFunc_t;
typedef struct tagSKMetaField SKMetaField_t;

class SKBuffer;

class SKMetaUtils
{
public:
	static void Dump( const SKMetaInfo_t * ptMetaInfo );

	static void Dump2Xml( const SKMetaInfo_t * ptMetaInfo, SKBuffer * poResult );

	static const SKMetaFunc_t * FindFunc(
			const SKMetaInfo_t *ptMetaInfo,
			const int iCmdId ) ;

	static const SKMetaFunc_t * FindFunc(
			const SKMetaInfo_t *ptMetaInfo,
			const char * sFuncName );

	static const SKMetaStruct_t * FindStruct(
			const SKMetaInfo_t *ptMetaInfo,
			const int iTypeId ) ;

	static const SKMetaStruct_t * FindStruct(
			const SKMetaInfo_t *ptMetaInfo,
			const char *sStructName ) ;

	static int GetReferCount(
			const void * pvStruct,
			const SKMetaStruct_t * ptMetaStruct,
			const SKMetaField_t * ptField );

	static int GetFieldSize (
		const SKMetaStruct_t *ptMetaStruct,
		const int iFieldId,
		const void *pvStruct,
		const int iStructSize
	);

	static int GetItemCnt (
		const SKMetaStruct_t *ptMetaStruct,
		const int iFieldId,
		const void *pvStruct,
		const int iStructSize
	);

	static int FreeField(
		const SKMetaInfo_t *ptMetaInfo,
		const int iTypeId,
		const void *pvStruct,
		const int iStructSize
	);

	static void DumpField(const SKMetaField_t *ptMetaField);

	static void DumpStruct(const SKMetaStruct_t *ptMetaStruct);

private:
	SKMetaUtils();
	~SKMetaUtils();
};

}


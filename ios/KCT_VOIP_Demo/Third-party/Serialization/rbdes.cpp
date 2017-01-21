#include "rbdes.h"
#include <string.h>

//密钥置换PC1列表
static const char PC1_TABLE[56] =
    { 57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4 };

// 压缩置换PC2列表
static const char PC2_TABLE[48] =
    { 14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10, 23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2, 41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32 };

// 密钥置换中每轮移动的位数
static const char LOOP_TABLE[16] =
    { 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1 };

// S-盒代替（substitution box）// S盒1
static const char S_BOX1[4][16] =
{ {14, 4, 13, 1 , 2, 15, 11, 8 , 3, 10, 6, 12, 5, 9, 0, 7}, {0, 15, 7, 4 , 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8}, {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0}, {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 } };

// S盒2
static const char S_BOX2[4][16] =
{   {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 }, {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 }, {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 }, {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 } };

// S盒3
static const char S_BOX3[4][16] =
{   {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 }, {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 }, {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 }, {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12} };

// S盒4
static const char S_BOX4[4][16] =
{   {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 }, {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 }, { 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4}, {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14} };

// S盒5
static const char S_BOX5[4][16] =
{   {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 }, {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 }, {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14}, {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3} };

// S盒6
static const char S_BOX6[4][16] =
{   {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11}, {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 }, {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6}, {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13} };

// S盒7
static const char S_BOX7[4][16] =
{   {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1}, {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6}, {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2}, {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}};

// S盒8
static const char S_BOX8[4][16] =
{   {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7}, {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2}, {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8}, {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 } };

// 扩展置换（expansion operation）E列表
static const char E_TABLE[48] =
{ 32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1 };

// P-盒置换（straight permutation ）或称为直接置换
static const char P_TABLE[32] =
{ 16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10, 2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25
};

// 初始置换（initial permutation）IP列表
const char IP_TABLE[64] =
{ 58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8, 57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3, 61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7 };

// 末置换（final permutation）IP^-1列表
const char IPF_TABLE[64] =
{ 40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27, 34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25 };

//void InitVars(char *M, char *K, char **KL, char **KR, char **Li, char **Ri)
//{
//	*KL=&K[0];
//	*KR=&K[28];
//	*Li=&M[0];
//	*Ri=&M[32];
//}
#define InitVars(M, K, KL, KR, Li, Ri) \
        *KL=&K[0]; \
        *KR=&K[28]; \
        *Li=&M[0]; \
        *Ri=&M[32]

inline void XOR2(char Out[8], const char In1[8], const char In2[8])
{
	int i;
	for(i = 0; i < 8; i++)
	{
		Out[i] = In1[i] ^ In2[i];
	}
}

inline void ByteToBit(char *Out, const char *In, int bits)
{ 
	int i;
  	for(i=0; i<bits; ++i)
     		Out[i] = (In[i>>3]>>(i&7)) & 1; 
}

inline void BitToByte(char *Out, const char *In, int bits)
{ 
	int i;
	memset(Out, 0, bits>>3); 
    for(i=0; i<bits; ++i) 
    Out[i>>3] |= In[i]<<(i&7);
} 

inline void CYCLELEFT(char *In, char *T, int len, int loop)
{ 
    memcpy(T, In, loop); 
    memcpy(In, In+loop, len-loop); 
    memcpy(In+len-loop, T, loop); 
} 

inline void TRANSFORM(char *Out, char *In, const char *Table, int len, char *T)
{
	int i;
 	for(i=0; i<len; ++i) 
   	T[i] = In[ Table[i]-1 ]; 
   	memcpy(Out, T, len); 
}

//little endian
#define BYTE_TO_INT(Out, In) \
        *(Out)      = (((In) & 0x1) \
                    | (((In) & 0x2) << 7) \
                    | (((In) & 0x4) << 14) \
                    | (((In) & 0x8) << 21))

#define BYTE_TO_INT2(Out, In) \
        *(Out)      = (((In)        & 0x1) << 24) \
                    | ((((In) >> 1) & 0x1) << 16) \
                    | ((((In) >> 2) & 0x1) << 8 ) \
                    | ((((In) >> 3) & 0x1)); \
        *(Out+1)    = ((((In) >> 4) & 0x1) << 24) \
                    | ((((In) >> 5) & 0x1) << 16) \
                    | ((((In) >> 6) & 0x1) << 8 ) \
                    | ((((In) >> 7) & 0x1));

#define BYTE_TO_INT16(Out, In) \
        BYTE_TO_INT2((Out), *(In)); \
        BYTE_TO_INT2((Out)+2, *((In)+1)); \
        BYTE_TO_INT2((Out)+4, *((In)+2)); \
        BYTE_TO_INT2((Out)+6, *((In)+3)); \
        BYTE_TO_INT2((Out)+8, *((In)+4)); \
        BYTE_TO_INT2((Out)+10, *((In)+5)); \
        BYTE_TO_INT2((Out)+12, *((In)+6)); \
        BYTE_TO_INT2((Out)+14, *((In)+7))

#define S_BBOXF_ROUND(Out, In, S) \
        BYTE_TO_INT((Out), (S)[(*(In)<<1) | *((In)+5)][(*((In)+1)<<3) | (*((In)+2)<<2) | (*((In)+3)<<1) | *((In)+4)])

#define S_BOXF(Out, In) \
        S_BBOXF_ROUND((Out), (In), S_BOX1); \
        S_BBOXF_ROUND((Out)+1, ((In) + 6), S_BOX2); \
        S_BBOXF_ROUND((Out)+2, ((In) + 12), S_BOX3); \
        S_BBOXF_ROUND((Out)+3, ((In) + 18), S_BOX4); \
        S_BBOXF_ROUND((Out)+4, ((In) + 24), S_BOX5); \
        S_BBOXF_ROUND((Out)+5, ((In) + 30), S_BOX6); \
        S_BBOXF_ROUND((Out)+6, ((In) + 36), S_BOX7); \
        S_BBOXF_ROUND((Out)+7, ((In) + 42), S_BOX8)

#define CP_INT2(a, b) \
        *(a) = *(b); \
        *((a) + 1) = *((b) + 1)

#define CP_INT8(a, b) \
        *(a) = *(b); \
        *((a) + 1) = *((b) + 1); \
        *((a) + 2) = *((b) + 2); \
        *((a) + 3) = *((b) + 3); \
        *((a) + 4) = *((b) + 4); \
        *((a) + 5) = *((b) + 5); \
        *((a) + 6) = *((b) + 6); \
        *((a) + 7) = *((b) + 7)

#define CP_INT12(a, b) \
        CP_INT8(a, b); \
        *((a) + 8) = *((b) + 8); \
        *((a) + 9) = *((b) + 9); \
        *((a) + 10) = *((b) + 10); \
        *((a) + 11) = *((b) + 11)

#define XOR_INT2(a, b) \
        *(a) ^= *(b); \
        *((a) + 1) ^= *((b) + 1);

#define XOR_INT8(a, b) \
        *(a) ^= *(b); \
        *((a) + 1) ^= *((b) + 1); \
        *((a) + 2) ^= *((b) + 2); \
        *((a) + 3) ^= *((b) + 3); \
        *((a) + 4) ^= *((b) + 4); \
        *((a) + 5) ^= *((b) + 5); \
        *((a) + 6) ^= *((b) + 6); \
        *((a) + 7) ^= *((b) + 7)

#define XOR_INT12(a, b) \
        XOR_INT8(a, b); \
        *((a) + 8) ^= *((b) + 8); \
        *((a) + 9) ^= *((b) + 9); \
        *((a) + 10) ^= *((b) + 10); \
        *((a) + 11) ^= *((b) + 11)

#define XOR2_INT2(a, b, c) \
        *(a) = *(b) ^ *(c); \
        *((a) + 1) = *((b) + 1) ^ *((c) + 1)

//little endian
#define TRANSFORM_INT(Out, In, Table, S) \
        *((Out)+((S)>>2)) = (((In)[ *(Table+S)-1 ]) \
                          | ((In)[ *(Table+S+1)-1 ] << 8) \
                          | ((In)[ *(Table+S+2)-1 ] << 16) \
                          | ((In)[ *(Table+S+3)-1 ] << 24))

#define TRANSFORM_INT2(Out, In, Table, S) \
        TRANSFORM_INT(Out, In, Table, S); \
        TRANSFORM_INT(Out, In, Table, S + 4)

#define TRANSFORM_INT4(Out, In, Table, S) \
        TRANSFORM_INT2(Out, In, Table, S); \
        TRANSFORM_INT2(Out, In, Table, S + 8)

#define TRANSFORM_INT8(Out, In, Table) \
        TRANSFORM_INT4(Out, In, Table, 0); \
        TRANSFORM_INT4(Out, In, Table, 16)

#define TRANSFORM_INT12(Out, In, Table) \
        TRANSFORM_INT8(Out, In, Table); \
        TRANSFORM_INT4(Out, In, Table, 32)

#define TRANSFORM_INT14(Out, In, Table) \
        TRANSFORM_INT12(Out, In, Table); \
        TRANSFORM_INT2(Out, In, Table, 48)

#define TRANSFORM_INT16(Out, In, Table) \
        TRANSFORM_INT14(Out, In, Table); \
        TRANSFORM_INT2(Out, In, Table, 56)

#define TRANSFORM48(Out, In, Table, T) \
    TRANSFORM_INT12((unsigned int*)(T), (In), (Table)); \
    CP_INT12((unsigned int*)(Out), (unsigned int*)(T))

#define TRANSFORM32(Out, In, Table, T) \
    TRANSFORM_INT8((unsigned int*)(T), (In), (Table)); \
    CP_INT8((unsigned int*)(Out), (unsigned int*)(T))

void Set_SubKey(SUBKEY_P pskey, const char Key[8], char *K, char *KL, char *KR, char *T)
{
    int i;
    ByteToBit(K, Key, 64);
    TRANSFORM(K, K, PC1_TABLE, 56, T);
    for(i=0; i<16; ++i)
    {
        CYCLELEFT(KL, T, 28, LOOP_TABLE[i]);
        CYCLELEFT(KR, T, 28, LOOP_TABLE[i]);
        TRANSFORM48((*pskey)[i], K, PC2_TABLE, T);
    }
}

void SETKEY(SUBKEY_P SubKey, char *deskey, const char* Key, int len, char *K, char *KL, char *KR, char *T)
{
    memset(deskey, 0, 24);
    memcpy(deskey, Key, len>24?24:len);
    Set_SubKey(&SubKey[0], &deskey[0], K, KL, KR, T);
}

inline void F_FUNCTION(char In[32], const char Ki[48], char *MR, char *T)
{
    TRANSFORM48((MR), (In), E_TABLE, (T));
    XOR_INT12((unsigned int*)(MR), (unsigned int*)(Ki));
    S_BOXF((unsigned int*)(In), (MR));
    TRANSFORM32((In), (In), P_TABLE, (T));
}

void DES(char Out[8], const char In[8], const SUBKEY_P pskey, char Type, char *M, char *MR, char *tmp, char *Li, char *Ri, char *T)
{
	int i;
	ByteToBit(M, In, 64);
  	TRANSFORM(M, M, IP_TABLE, 64, T); 

    unsigned int* LiI = (unsigned int*)Li;
    unsigned int* RiI = (unsigned int*)Ri;
    unsigned int* tmpI = (unsigned int*)tmp;

  	if( Type == ENCRYPTION ) 
  	{ 
    	for(i=0; i<16; ++i)
    	{ 
    	    CP_INT8(tmpI, RiI);
       		F_FUNCTION(Ri, (*pskey)[i], MR, T); 
       		XOR_INT8(RiI, LiI);
       		CP_INT8(LiI, tmpI);
    	} 
  	} 
  	else 
  	{ 
     	for(i=15; i>=0; --i) 
     	{ 
     	    CP_INT8(tmpI, LiI);
       		F_FUNCTION(Li, (*pskey)[i], MR, T); 
       		XOR_INT8(LiI, RiI);
       		CP_INT8(RiI, tmpI);
     	} 
   	} 

   	TRANSFORM(M, M, IPF_TABLE, 64, T); 
   	BitToByte(Out, M, 64); 
}

char Using_DES(
		char *Out, const char *In, 
		long textlen, int keylen,
		const char *Key, char Type) 
{
	char InitVec[9];
	return Using_DES(Out, In, textlen, keylen, Key, Type, InitVec);
}

char Using_DES(
		char *Out, const char *In, 
		long textlen, int keylen,
		const char *Key, char Type, char InitVec[9]) 
{
	// 子密钥 
	char SubKey[1][16][48];
	char T[256], deskey[16];
	char K[64], *KL, *KR;
	char MR[48];
	char M[64], tmp[32], *Li, *Ri;
	//char InitVec[9];
	char Block[8];

	long i, j;

 	if( !( Out && In && Key && (textlen=(textlen+7)&0xfffffff8) ) )
      		return 0; 


//    unsigned int* OutI = (unsigned int*)Out;
//    unsigned int* InitVecI = (unsigned int*)InitVec;
//    unsigned int* InI = (unsigned int*)In;

	InitVars(M, K, &KL, &KR, &Li, &Ri);
	SETKEY(SubKey, deskey, Key, keylen, K, KL, KR, T); 

	if(Type == ENCRYPTION)
	{
//		srandom(time(NULL));
//		sprintf(InitVec, "%08x", random());
		DES(Out, InitVec, &SubKey[0], Type, M, MR, tmp, Li, Ri, T);
		memcpy(Block, Out, 8);
		Out += 8;
 		for(i=0,j=textlen>>3; i<j; ++i,Out+=8,In+=8)
		{
 		    XOR_INT2((unsigned int*)Block, (unsigned int*)In);
          	DES(Out, Block, &SubKey[0], Type, M, MR, tmp, Li, Ri, T); // 1次DES
			CP_INT2((unsigned int*)Block, (unsigned int*)Out);
		}
	} 
	else
	{
		memcpy(InitVec, In, 8);
		In += 8;
 		for(i=1,j=textlen>>3; i<j; ++i,Out+=8,In+=8) 
		{
			DES(Block, In, &SubKey[0], Type, M, MR, tmp, Li, Ri, T); // 1次DES
			XOR2(Out, Block, InitVec);
			memcpy(InitVec, In, 8);
		}
	}
 	return 1; 
} 





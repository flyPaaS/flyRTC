#include "oldrbdes.h"

#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

namespace olddes {

void InitVars(char *M, char *K, char **KL, char **KR, char **Li, char **Ri)
{
	*KL=&K[0];
	*KR=&K[28];
	*Li=&M[0];
	*Ri=&M[32]; 
}

void XOR2(char Out[8], const char In1[8], const char In2[8])
{
	int i;
	for(i = 0; i < 8; i++)
	{
		Out[i] = In1[i] ^ In2[i];
	}
}

void XOR(char *InA, const char *InB, int len) 
{ 
	int i;
   	for(i=0; i<len; ++i) InA[i] ^= InB[i];
}

void ByteToBit(char *Out, const char *In, int bits) 
{ 
	int i;
  	for(i=0; i<bits; ++i)
     		Out[i] = (In[i>>3]>>(i&7)) & 1; 
}

void BitToByte(char *Out, const char *In, int bits) 
{ 
	int i;
	memset(Out, 0, bits>>3); 
    for(i=0; i<bits; ++i) 
    Out[i>>3] |= In[i]<<(i&7);
} 

void CYCLELEFT(char *In, char *T, int len, int loop) 
{ 
    memcpy(T, In, loop); 
    memcpy(In, In+loop, len-loop); 
    memcpy(In+len-loop, T, loop); 
} 

void TRANSFORM(char *Out, char *In, const char *Table, int len, char *T) 
{
	int i;
 	for(i=0; i<len; ++i) 
   	T[i] = In[ Table[i]-1 ]; 
   	memcpy(Out, T, len); 
} 

void Set_SubKey(SUBKEY_P pskey, const char Key[8], char *K, char *KL, char *KR, char *T) 
{ 
	//ÃÜÔ¿ÖÃ»»PC1ÁÐ±í 
	const char PC1_TABLE[56] = 
	{ 57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4 };

	// Ñ¹ËõÖÃ»»PC2ÁÐ±í 
	const char PC2_TABLE[48] =
	{ 14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10, 23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2, 41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32 };

	// ÃÜÔ¿ÖÃ»»ÖÐÃ¿ÂÖÒÆ¶¯µÄÎ»Êý 
	const char LOOP_TABLE[16] = 
	{ 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1 };

	int i;
    ByteToBit(K, Key, 64); 
    TRANSFORM(K, K, PC1_TABLE, 56, T); 
    for(i=0; i<16; ++i) 
    { 
        CYCLELEFT(KL, T, 28, LOOP_TABLE[i]); 
        CYCLELEFT(KR, T, 28, LOOP_TABLE[i]); 
        TRANSFORM((*pskey)[i], K, PC2_TABLE, 48, T); 
    } 
} 

void SETKEY(SUBKEY_P SubKey, char *deskey, const char* Key, int len, char *K, char *KL, char *KR, char *T) 
{ 
   	memset(deskey, 0, 24);
   	memcpy(deskey, Key, len>24?24:len);
   	Set_SubKey(&SubKey[0], &deskey[0], K, KL, KR, T); 
}


void S_BOXF(char Out[32], const char In[48]) 
{ 
	// S-ºÐ´úÌæ£¨substitution box£© 
	const char S_BOX[8][4][16] = 
	{ 
	// SºÐ1
	{ {14, 4, 13, 1 , 2, 15, 11, 8 , 3, 10, 6, 12, 5, 9, 0, 7}, {0, 15, 7, 4 , 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8}, {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0}, {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 } }, 
	// SºÐ2
	{   {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 }, {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 }, {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 }, {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 } }, 
	// SºÐ3
	{   {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 }, {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 }, {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 }, {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12} },  
	// SºÐ4 
	{   {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 }, {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 }, { 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4}, {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14} },  
	// SºÐ5
	{   {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 }, {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 }, {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14}, {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3} }, 
	// SºÐ6 
	{   {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11}, {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 }, {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6}, {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13} },  
	// SºÐ7
	{   {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1}, {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6}, {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2}, {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},  
	// SºÐ8 
	{   {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7}, {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2}, {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8}, {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 } }
	}; 
	

	unsigned char i, j, k;
    for(i=0; i<8; ++i,In+=6,Out+=4) 
    { 
      	j = (In[0]<<1) + In[5]; 
      	k = (In[1]<<3) + (In[2]<<2) + (In[3]<<1) + In[4];
      	ByteToBit(Out, &S_BOX[i][j][k], 4);
    } 
} 

void F_FUNCTION(char In[32], const char Ki[48], char *MR, char *T) 
{
	// À©Õ¹ÖÃ»»£¨expansion operation£©EÁÐ±í 
	const char E_TABLE[48] = 
	{ 32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1 };

// P-ºÐÖÃ»»£¨straight permutation £©»ò³ÆÎªÖ±½ÓÖÃ»»
	const char P_TABLE[32] = 
	{ 16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10, 2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25
	};

   	TRANSFORM(MR, In, E_TABLE, 48, T); 
   	XOR(MR, Ki, 48); 
   	S_BOXF(In, MR); 
   	TRANSFORM(In, In, P_TABLE, 32, T); 
} 

void DES(char Out[8], const char In[8], const SUBKEY_P pskey, char Type, char *M, char *MR, char *tmp, char *Li, char *Ri, char *T)
{
	// ³õÊ¼ÖÃ»»£¨initial permutation£©IPÁÐ±í 
	const char IP_TABLE[64] = 
	{ 58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8, 57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3, 61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7 };

	// Ä©ÖÃ»»£¨final permutation£©IP^-1ÁÐ±í 
	const char IPF_TABLE[64] = 
	{ 40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27, 34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25 };

	int i;
	ByteToBit(M, In, 64);
  	TRANSFORM(M, M, IP_TABLE, 64, T); 
  	if( Type == ENCRYPTION ) 
  	{ 
    	for(i=0; i<16; ++i)
    	{ 
       		memcpy(tmp, Ri, 32); 
       		F_FUNCTION(Ri, (*pskey)[i], MR, T); 
       		XOR(Ri, Li, 32); 
       		memcpy(Li, tmp, 32); 
    	} 
  	} 
  	else 
  	{ 
     	for(i=15; i>=0; --i) 
     	{ 
       		memcpy(tmp, Li, 32); 
       		F_FUNCTION(Li, (*pskey)[i], MR, T); 
       		XOR(Li, Ri, 32); 
       		memcpy(Ri, tmp, 32); 
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
	// ×ÓÃÜÔ¿ 
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
			XOR2(Block, Block, In);
          	DES(Out, Block, &SubKey[0], Type, M, MR, tmp, Li, Ri, T); // 1´ÎDES 
			memcpy(Block, Out, 8);
		} 			
	} 
	else
	{
		memcpy(InitVec, In, 8);
		In += 8;
 		for(i=1,j=textlen>>3; i<j; ++i,Out+=8,In+=8) 
		{
			DES(Block, In, &SubKey[0], Type, M, MR, tmp, Li, Ri, T); // 1´ÎDES
			XOR2(Out, Block, InitVec);
			memcpy(InitVec, In, 8);
		}
	}
 	return 1; 
} 




}


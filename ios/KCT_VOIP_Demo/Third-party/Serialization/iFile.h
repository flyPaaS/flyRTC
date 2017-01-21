/**
@file
@brief 文件操作帮助类函数
  $Id$
@author $Author$
@date $Date$
@version $Revision$
@note Editor: tab=4
@note Platform: Linux
*/

#pragma once

#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define CP_MAX_FILE_NAME_LEN		260
#define CP_FIND_HANDLE_NULL			-1

// find type
#define CP_FIND_TYPE_FILE			0
#define CP_FIND_TYPE_FOLDER			1

#define CP_TEMP_FILE_PREFIX			".tmp"

#define CP_COPY_BUFFER_SIZE			1024000	// 1 MB

// open file flag
#define OPEN_FILE_FLAG_APPEND		O_APPEND
#define OPEN_FILE_FLAG_CREATE		O_CREAT
#define OPEN_FILE_FLAG_WRONLY		O_WRONLY
#define OPEN_FILE_FLAG_RDONLY		O_RDONLY
#define OPEN_FILE_FLAG_TRUNC		O_TRUNC
#define OPEN_FILE_FLAG_READ_WRITE	O_RDWR

#define SIZE_64B  64
#define SIZE_SECTION            SIZE_64B
#define SIZE_KEY                SIZE_64B

#define CP_MAX(a, b) ((a > b) ? a : b)
#define CP_MIN(a, b) ((a < b) ? a : b)

#define CP_MALLOC(size)			malloc(size)
#define CP_REALLOC(ptr, size)	realloc(ptr, size)
#define CP_FREE(ptr)			free(ptr)
#define CP_CLOSE(handle)		close(handle)
#define CP_FCLOSE(ptr)			fclose(ptr)
#define CP_DELETE(obj)			delete obj
#define CP_NEW					new
#define CP_FOPEN(name, mode)	fopen(name, mode)
#define CP_OPEN(name, flag)		open(name, flag, 0666)

/** 
	@brief delete an object in safe way
	@param obj: object pointer.
*/
#define CP_SAFE_DELETE(obj)		\
{								\
	if(obj != NULL)				\
	{							\
		CP_DELETE(obj);			\
		obj = NULL;				\
	}							\
}

/** 
	@brief free a memory block in safe way
	@param p: memory block pointer.
*/
#define CP_SAFE_FREE(p)			\
{								\
	if(p != NULL)				\
	{							\
		CP_FREE(p);				\
		p = NULL;				\
	}							\
}

/** 
	@brief close file in safe way
	@param pfile: file handle
*/
#define CP_SAFE_FCLOSE(pfile)	\
{								\
	if(pfile != NULL)			\
	{							\
		CP_FCLOSE(pfile);		\
		pfile = NULL;			\
	}							\
}

#define CP_MEMCPY(dest, src, len)	\
{									\
	if(len > 0)						\
	{								\
		memcpy(dest, src, len);		\
	}								\
}

#define CP_DUP_NSTR(dest, src, len)     \
{                                       \
dest = (char *)CP_MALLOC(len + 1);  \
memcpy(dest, src, len);             \
dest[len] = 0;                      \
}

namespace Comm{

/**	
	@brief check if the file exist.
	@param sFileName: File name.	
	@return 0=OK, 1=FAIL.
*/
int FileExist(const char *sFileName);

/**	
	@brief Get file size.
	@param sFileName: File name.	
	@return File size, in byte. -1=FAIL.
*/
int GetFileSize(const char *sFileName);

/**	
	@brief Load file into buffer.
	@note The function will append 0 at the end of the buffer.
	@param sFileName: File name.
	@param sBuffer: To store file data.
	@param nBufferSize: Buffer size.
	@param nSize: To store the size of the data read from file.
	@param nOffset: The offset of the file where start to read.
	@return 0=OK. 1=FAIL.
*/
int LoadFromFile(const char *sFileName, char *sBuffer, 
			const int nBufferSize, int *nSize, const int nOffset = 0);

/**	
	@brief Write data into file.
	@param sFileName: File name.
	@param sBuffer: The data buffer.
	@param nSize: To size of the buffer.
	@return 0=OK. 1=FAIL.
*/
int WriteToFile(const char *sFileName, const char *sBuffer, const int nSize);

/**	
	@brief Append data into file.
	@param sFileName: File name.
	@param sBuffer: The data buffer.
	@param nSize: To size of the buffer.
	@return 0=OK. 1=FAIL.
*/
int AppendToFile(const char *sFileName, const char *sBuffer, const int nSize);

/**	
	@brief Extend file size, open in binary mode.
	@param sFileName: File name.
	@param nExtendSize: The extend size, in byte.
	@return 0=OK. 1=FAIL.
*/
int ExtendFile(const char *sFileName, const int nExtendSize);

/**	
	@brief Copy file with fopen serial io functions.
	@param pDest: destination file handle
	@param pSrc: source file handle
	@param nCopySize: copy size, in byte
	@param nBufferSize: copy buffer size, in byte.
	@return 0=OK. 1=FAIL.
*/
int CopyFile(FILE *pDest, FILE *pSrc, 
					  const int nCopySize, const int nBufferSize);

/**	
	@brief Copy file with fopen serial io functions.
	@param pDest: destination file handle
	@param pSrc: source file handle
	@param nCopySize: copy size, in byte
	@param sBuffer: the copy buffer
	@param nBufferSize: copy buffer size, in byte.
	@return 0=OK. 1=FAIL.
*/
int CopyFile(FILE *pDest, FILE *pSrc, 
			const int nCopySize, char *sBuffer, const int nBufferSize);

/**	
	@brief Copy file with low io functions.
	@param nDestHandle: destination file handle
	@param nSrcHandle: source file handle
	@param nCopySize: copy size, in byte
	@param nBufferSize: copy buffer size, in byte.
	@return 0=OK. 1=FAIL.
*/
int CopyFile(int nDestHandle, int nSrcHandle, 
					  const int nCopySize, const int nBufferSize);

/**	
	@brief Copy file with low io functions.
	@param nDestHandle: destination file handle
	@param nSrcHandle: source file handle
	@param nCopySize: copy size, in byte
	@param sBuffer: the copy buffer
	@param nBufferSize: copy buffer size, in byte.
	@return 0=OK. 1=FAIL.
*/
int CopyFile(int nDestHandle, int nSrcHandle, 
			const int nCopySize, char *sBuffer, const int nBufferSize);

/**	
	@brief Copy file with low io functions.
	@param sDestFile: destination file name
	@param sSrcFile: source file name
	@param nBufferSize: copy buffer size, in byte.
	@return 0=OK. 1=FAIL.
*/
int CopyFile(const char *sDestFile, const char *sSrcFile, const int nBufferSize);

/**	
	@brief Get file timestamps.
	@param sFileName: file name
	@param nCreateTime: to store create time
	@param nAccessTime: to store access time
	@param nWriteTime: to store write time
	@return 0=OK. 1=FAIL.
*/
int GetFileTime(const char *sFileName, 
					int *nCreateTime, int *nAccessTime, int *nWriteTime);

/**	
	@brief Make new directory.
	@note before call this function, the parent directory must exist.
	@param sDir: directory name, full path or relative path
	@return 0=OK. 1=FAIL.
*/
int MakeDir(const char *sDir);

/**
	to store file and directory finding information
*/
typedef struct
{
	/** type, defined in macro CP_FIND_TYPE_xxx */
	char			cType;
	/** is hiden, 0/1 */
	char			cIsHiden;
	/** create time */
	int				nCreateTime;
	/** access time */
	int				nAccessTime;
	/** last write time */
	int				nWriteTime;
	/** size */
	unsigned int	nSize;
	/** name */
	char			sName[CP_MAX_FILE_NAME_LEN];
}	FindInfo;

/** 
	@brief folder finding class.
*/
class FindFolder
{
public:

/** 
	@brief Initiate
*/
	FindFolder();

/** 
	@brief Release resource
*/
	~FindFolder();

/** 
	@brief Begin to find
	@param sFolderName: folder name
*/
	void BeginFind(const char *sFolderName);

/** 
	@brief End findind and close finding cursor
*/
	void EndFind();

/** 
	@brief Get next folder or file
	@param nFindHandle: find handle, assign nFindHandle=CP_FIND_HANDLE_NULL to get the first
	@param pFindInfo: to store file information
	@return: find handle, CP_FIND_HANDLE_NULL means end
*/
	int GetNext(int nFindHandle, FindInfo *pFindInfo);

private:
	// CP_FIND_HANDLE_NULL means NULL
	int			m_nFindHandle;
	char		*m_sFolderName;
	DIR			*m_pDIR;
};

/** 
	@brief Get temporary file name
	@note On windows, nMaxLen must >= MAX_PATH
	@param sPrefix: temporary file name prefix
	@param sFileName: to store temporary file name
	@param nMaxLen: the max length of the temporary file name
	@return 0=OK, 1=FAIL
*/
int GetTempFileName(const char *sPrefix, char *sFileName, const int nMaxLen);

/** 
	@brief extract file name from the path(support GBK path)
	@note support '\' and '/'
	@param sPath: path name
	@param sFileName: File name.
	@param nMaxLen: size of sFileName
*/
void GetFileNameFromPath(const char *sPath, char *sFileName, const int nMaxLen);

/** 
	@brief extract sub folder name from the path
	@note support '\' and '/'
	@param sPath: path name
	@param sFileName: File name.
	@param nMaxLen: size of sFileName
*/
void GetSubFolderFromPath(const char *sPath, char *sFileName, const int nMaxLen);

/** 
	@brief extract parent folder name from the path
	@note support '\' and '/'
	@param sPath: path name
	@param sFileName: File name.
	@param nMaxLen: size of sFileName
*/
void GetParentFromPath(const char *sPath, char *sFileName, const int nMaxLen);

/** 
	@brief extract extension name from file name
	@param sFileName: file name
	@param sExt: to store extension name
	@param nSize: sExt buffer size
*/
void GetExtension(const char *sFileName, char *sExt, const int nSize);

/** 
	@brief parepare folder for the file. create sub-folders neccessary to the file
	@note support '\' and '/'
	@param sFilePath: file path name
	@return 0=OK, 1=FAIL
*/
int PrepareFolder(const char *sFilePath);


/** 
	@brief open file by calling open() function
	@param sFileName: file name
	@param nFlag: open flag, defined in macro OPEN_FILE_FLAG_xxx
	@return file handle, <=0 means FAIL
*/
int OpenFile(const char *sFileName, int nFlag);

/** 
	@brief remove all files and folders under specific folder
	@param sFolder: folder name
	@return 0=OK, 1=FAIL
*/
int RemoveFolder(const char *sFolder);

/** 
	@brief move file
	@param sOld: old file name
	@param sNew: new file name
	@return 0=OK, 1=FAIL
*/
int MoveFile(const char *sOld, const char *sNew);

/** 
	@brief move dir
	@param sOld: old dir name
	@param sNew: new dir name
	@return 0=OK, 1=FAIL
*/
int MoveDir(const char *sOld, const char *sNew);

/**	
	@brief Copy dir with low io functions.
	@param sDestDir: destination dir name
	@param sSrcDir: source dir name
	@param nBufferSize: copy buffer size, in byte.
	@return 0=OK. 1=FAIL.
*/
int CopyDir(const char *sDestDir, const char *sSrcDir, const int nBufferSize);

char * gsGetSetting ( char *asSetting,
  const char *asFile, const char *asSection, const char *asKey ) ;
int giSaveSetting ( const char *asFile,
  const char *asSection, const char *asKey, const char *asValue ) ;
 
/**
 	@brief write to fd（原sysarch的函数，原实现有timeout逻辑，但目前的磁盘基本已经没有读写问题，因此已去掉该逻辑）
*/
ssize_t Write(int fd, const void *buf, size_t count);

/**
 	@brief read from fd（原sysarch的函数，原实现有timeout逻辑，但目前的磁盘基本已经没有读写问题，因此已去掉该逻辑）
 */
ssize_t Read(int fd, void *buf, size_t count);
 
 }


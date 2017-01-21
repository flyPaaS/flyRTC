#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>

#include "iFile.h"
#include "iLog.h"

namespace Comm{

const int DISK_IO_TIMEOUT = 5;
	
static int _Strcpy(char *sDest, const char *sSrc, const int nDestLen)
     {
         int     n = 0;
     
         for(n=0; n<nDestLen-1 && sSrc[n]!=0; ++n)
         {
             sDest[n] = sSrc[n]; 
         }       
     sDest[n] = 0;
         
         return(n);
     }

int GetFileSize(const char *sFileName)
{
	struct stat sbuf;

    if(stat(sFileName, &sbuf) != 0 )
	{
		return(-1);
    }

    return(sbuf.st_size);
}

int LoadFromFile(const char *sFileName, char *sBuffer, 
			const int nBufferSize, int *nSize, const int nOffset)
{
	int		nHandle, nReturn = 0;

	*nSize = GetFileSize(sFileName);
	if(*nSize <= 0)
		return(1);

	nHandle = CP_OPEN(sFileName, OPEN_FILE_FLAG_RDONLY );
	if(nHandle <= 0)
		return(1);

	if(nOffset > 0)
	{
		if(lseek(nHandle, nOffset, SEEK_SET) != nOffset)
		{
			CP_CLOSE(nHandle);
			return(1);
		}
		*nSize = *nSize - nOffset;
	}

	// make sure the buffer size does not larger the buffer size
	*nSize = CP_MIN(*nSize, nBufferSize);
	
	if(read(nHandle, sBuffer, *nSize) != *nSize)
	{
		nReturn = 1;
	}
	CP_CLOSE(nHandle);
	return(nReturn);
}

int WriteToFile(const char *sFileName, const char *sBuffer, const int nSize)
{
	int		nHandle, nReturn = 0;

		nHandle = CP_OPEN(sFileName, 
					OPEN_FILE_FLAG_CREATE | OPEN_FILE_FLAG_WRONLY | OPEN_FILE_FLAG_TRUNC  );
	if(nHandle <= 0)
		return(1);

	if(nSize > 0)
	{
		if(write(nHandle, sBuffer, nSize) != nSize)
		{
			nReturn = 1;
		}
	}
	CP_CLOSE(nHandle);
	return(nReturn);
}

int AppendToFile(const char *sFileName, const char *sBuffer, const int nSize)
{
	int		nHandle, nReturn = 0;
	nHandle = CP_OPEN(sFileName, OPEN_FILE_FLAG_APPEND | OPEN_FILE_FLAG_WRONLY  );

	if(nHandle <= 0)
		return(1);

	if(write(nHandle, sBuffer, nSize) != nSize)
	{
		nReturn = 1;
	}
	CP_CLOSE(nHandle);
	return(nReturn);
}

int ExtendFile(const char *sFileName, const int nExtendSize)
{
	FILE		*pFile;
	int			nRemainSize, nWriteSize;
	char*	    theMemBlock[1024];

	pFile = CP_FOPEN(sFileName, "ab+");
	if(pFile == NULL)
		return(1);
	
	memset(theMemBlock, 0, 1024);
	nRemainSize = nExtendSize;
	while(nRemainSize > 0)
	{
		nWriteSize = CP_MIN(nRemainSize, 1024);
		fwrite(theMemBlock, nWriteSize, 1, pFile);
		nRemainSize = nRemainSize - nWriteSize;
	}
	CP_FCLOSE(pFile);
	return(0);
}

int CopyFile(FILE *pDest, FILE *pSrc, const int nCopySize, const int nBufferSize)
{
	char*		theMemBlock = NULL;
	int			nRemainSize, nRealCopySize;
	char		*sBuffer;

	theMemBlock = (char*)malloc(nBufferSize);
	if(theMemBlock == NULL)
		return(1);
	
	sBuffer = theMemBlock;
	nRemainSize = nCopySize;

	while(nRemainSize > 0)
	{
		nRealCopySize = CP_MIN(nRemainSize, nBufferSize);
		if(fread(sBuffer, nRealCopySize, 1, pSrc) != 1)
		{
			free(theMemBlock);
			return(1);
		}
		if(fwrite(sBuffer, nRealCopySize, 1, pDest) != 1)
		{
			free(theMemBlock);
			return(1);
		}
		nRemainSize = nRemainSize - nRealCopySize;
	}
	free(theMemBlock);
	return(0);
}

int CopyFile(FILE *pDest, FILE *pSrc, 
				const int nCopySize, char *sBuffer, const int nBufferSize)
{
	int			nRemainSize, nRealCopySize;

	nRemainSize = nCopySize;
	while(nRemainSize > 0)
	{
		nRealCopySize = CP_MIN(nRemainSize, nBufferSize);
		if(fread(sBuffer, nRealCopySize, 1, pSrc) != 1)
		{
			return(1);
		}
		if(fwrite(sBuffer, nRealCopySize, 1, pDest) != 1)
		{
			return(1);
		}
		nRemainSize = nRemainSize - nRealCopySize;
	}

	return(0);
}

int CopyFile(int nDestHandle, int nSrcHandle, 
					  const int nCopySize, const int nBufferSize)
{
	char*		theMemBlock = NULL;
	int			nRemainSize, nRealCopySize;
	char		*sBuffer;

	theMemBlock = (char*)malloc(nBufferSize);
	if(theMemBlock == NULL)
		return(1);
	sBuffer = theMemBlock;
	nRemainSize = nCopySize;

	while(nRemainSize > 0)
	{
		nRealCopySize = CP_MIN(nRemainSize, nBufferSize);
		if(read(nSrcHandle, sBuffer, nRealCopySize) != nRealCopySize)
		{
			free(theMemBlock);
			return(1);
		}
		if(write(nDestHandle, sBuffer, nRealCopySize) != nRealCopySize)
		{
			free(theMemBlock);
			return(1);
		}
		nRemainSize = nRemainSize - nRealCopySize;
	}
	free(theMemBlock);
	return(0);
}

int CopyFile(int nDestHandle, int nSrcHandle, 
			const int nCopySize, char *sBuffer, const int nBufferSize)
{
	int			nRemainSize, nRealCopySize;

	nRemainSize = nCopySize;
	while(nRemainSize > 0)
	{
		nRealCopySize = CP_MIN(nRemainSize, nBufferSize);
		if(read(nSrcHandle, sBuffer, nRealCopySize) != nRealCopySize)
		{
			return(1);
		}
		if(write(nDestHandle, sBuffer, nRealCopySize) != nRealCopySize)
		{
			return(1);
		}
		nRemainSize = nRemainSize - nRealCopySize;
	}

	return(0);
}

int GetFileTime(const char *sFileName, 
				int *nCreateTime, int *nAccessTime, int *nWriteTime)
{
	struct stat sbuf;

    if(stat(sFileName, &sbuf) != 0 )
	{
		return(-1);
    }
	*nCreateTime = sbuf.st_ctime;
	*nAccessTime = sbuf.st_atime;
	*nWriteTime = sbuf.st_mtime;

	return(0);
}

int MakeDir(const char *sDir)
{
	if(access(sDir, F_OK) != 0)
	{
		if(mkdir(sDir, 0766) != 0)
		{
			return(1);
		}
	}

	return(0);
}


// begin of class cpFindFolder

FindFolder::FindFolder()
{
	m_nFindHandle = CP_FIND_HANDLE_NULL;
	m_sFolderName = NULL;
}

FindFolder::~FindFolder()
{
	EndFind();
}

void FindFolder::BeginFind(const char *sFolderName)
{
	// try to end first
	EndFind();

	// spare space to store "/*.*" on Window platform
	m_sFolderName = (char *)CP_MALLOC(strlen(sFolderName) + 4 + 1);
	strcpy(m_sFolderName, sFolderName);
}

void FindFolder::EndFind()
{
	CP_SAFE_FREE(m_sFolderName);

	if(m_pDIR != NULL)
	{
		closedir(m_pDIR);
		m_pDIR = NULL;
	}
	m_nFindHandle = CP_FIND_HANDLE_NULL;
}

int FindFolder::GetNext(int nFindHandle, FindInfo *pFindInfo)
{
	int		nNextHandle = CP_FIND_HANDLE_NULL;

	memset(pFindInfo, 0, sizeof(FindInfo));

	struct dirent	*pNextFile;
	char			*sFileName;
	struct stat		sbuf;

	sFileName = (char *)CP_MALLOC(CP_MAX_FILE_NAME_LEN);
	if(m_nFindHandle == CP_FIND_HANDLE_NULL)
	{
		m_pDIR = opendir(m_sFolderName);
		if(m_pDIR == NULL)
		{
			CP_SAFE_FREE(sFileName);
			return(CP_FIND_HANDLE_NULL);
		}
		// filter "." and ".." special subdirectories
		pNextFile = readdir(m_pDIR);
		if(pNextFile == NULL)
		{
			CP_SAFE_FREE(sFileName);
			return(CP_FIND_HANDLE_NULL);
		}
		pNextFile = readdir(m_pDIR);
		if(pNextFile == NULL)
		{
			CP_SAFE_FREE(sFileName);
			return(CP_FIND_HANDLE_NULL);
		}

		pNextFile = readdir(m_pDIR);
		if(pNextFile == NULL)
		{
			CP_SAFE_FREE(sFileName);
			return(CP_FIND_HANDLE_NULL);
		}
		m_nFindHandle = 1;
	}
	else
	{
		pNextFile = readdir(m_pDIR);
		if(pNextFile == NULL)
		{
			CP_SAFE_FREE(sFileName);
			return(CP_FIND_HANDLE_NULL);
		}
		m_nFindHandle = 1;
	}
	nNextHandle = m_nFindHandle;

	sprintf(sFileName, "%s/%s", m_sFolderName, pNextFile->d_name);
	if(lstat(sFileName, &sbuf) != 0 )
	{
		CP_SAFE_FREE(sFileName);
		return(-1);
    }

	if(S_ISLNK(sbuf.st_mode))
	{
		pFindInfo->cType = CP_FIND_TYPE_FILE;
	}
	else if(S_ISDIR(sbuf.st_mode))
	{
		pFindInfo->cType = CP_FIND_TYPE_FOLDER;
	}
	else
	{
		pFindInfo->cType = CP_FIND_TYPE_FILE;
	}
	pFindInfo->cIsHiden = 0;

	pFindInfo->nCreateTime = sbuf.st_ctime;
	pFindInfo->nAccessTime = sbuf.st_atime;
	pFindInfo->nWriteTime = sbuf.st_mtime;

	pFindInfo->nSize = sbuf.st_size;
	_Strcpy(pFindInfo->sName, pNextFile->d_name, sizeof(pFindInfo->sName));

	CP_SAFE_FREE(sFileName);

	return(nNextHandle);
}

// end of class cpFindFolder


int GetTempFileName(const char *sPrefix, char *sFileName, const int nMaxLen)
{
	//char	*sPtr;

	/*
	sPtr = tempnam("/tmp", sPrefix);
	_Strcpy(sFileName, sPtr, nMaxLen);
	CP_SAFE_FREE(sPtr);
	*/

	int preflen = strlen(sPrefix);
	int buflen = 5 + preflen + 6;  // "/tmp/" + sPrefixa + XXXXXX
	char buf[buflen+1];
	strncpy(buf,"/tmp/",5);
	strncpy(buf+5,sPrefix,preflen);
	strncpy(buf+5+preflen,"XXXXXX",6);
	buf[buflen] = '\0';
	int fd = mkstemp(buf);
	close(fd);
	unlink(buf);
	_Strcpy(sFileName, buf, nMaxLen);

	return(0);
}

void GetFileNameFromPath(const char *sPath, char *sFileName, const int nMaxLen)
{
	int		n, nLen;

	nLen = strlen(sPath);
	if(nLen <= 0)
	{
		goto NotFound;
	}

	// it is a folder, not file
	if(sPath[nLen-1] == '\\' ||  sPath[nLen-1] == '/')
	{
		goto NotFound;
	}

	for(n=nLen-1; n>=0; --n)
	{
		if(sPath[n] == '\\' || sPath[n] == '/')
		{
			if (n>=1 && isascii(sPath[n-1]))   //若GBK字符的第二字节刚好是\或/，这种情况不理。	
			{
				nLen = nLen - n;
				if(nMaxLen < nLen)
				{
					return;
				}
				_Strcpy(sFileName, &(sPath[n+1]), nLen);
				return;
			}
		}
	}

	if(nMaxLen < nLen)
	{
		return;
	}
	_Strcpy(sFileName, sPath, nLen);

NotFound:

	if(nMaxLen < 1)
	{
		return;
	}
	strcpy(sFileName, "");
}

void GetSubFolderFromPath(const char *sPath, char *sFileName, const int nMaxLen)
{
	int		n, nLen;

	nLen = strlen(sPath);
	if(nLen <= 0)
	{
		goto NotFound;
	}

	/* skip last / or \ */
	if(sPath[nLen-1] == '\\' ||  sPath[nLen-1] == '/')
	{
		nLen--;
	}

	for(n=nLen-1; n>=0; --n)
	{
		if(sPath[n] == '\\' || sPath[n] == '/')
		{
			nLen = nLen - n;
			if(nMaxLen < nLen)
			{
				return;
			}
			_Strcpy(sFileName, &(sPath[n+1]), nLen);
			return;
		}
	}

NotFound:

	if(nMaxLen < 1)
	{
		return;
	}
	strcpy(sFileName, "");
}

void GetParentFromPath(const char *sPath, char *sFileName, const int nMaxLen)
{
	int		n, nLen;

	nLen = strlen(sPath);
	if(nLen <= 0)
	{
		goto NotFound;
	}

	/* skip last / or \ */
	if(sPath[nLen-1] == '\\' ||  sPath[nLen-1] == '/')
	{
		nLen--;
	}

	for(n=nLen-1; n>=0; --n)
	{
		if(sPath[n] == '\\' || sPath[n] == '/')
		{
			if(nMaxLen < n+1)
			{
				return;
			}
			_Strcpy(sFileName, sPath, n+1);
			return;
		}
	}

NotFound:

	if(nMaxLen < 1)
	{
		return;
	}
	strcpy(sFileName, "");
}

int FileExist(const char *sFileName)
{
	if(access(sFileName, R_OK) == 0)
	{
		return(0);
	}
	else
	{
		return(1);
	}
}

int PrepareFolder(const char *sFilePath)
{
	const char	*sPtr;
	char		*sFolder = NULL;

	if(access(sFilePath, R_OK) == 0)
	{
		return(0);
	}

	sPtr = sFilePath;

	while(*sPtr != 0)
	{
		if(*sPtr == '\\' || *sPtr == '/')
		{
			// test the folder
			CP_SAFE_FREE(sFolder);
			// skip '/' or '\'
			CP_DUP_NSTR(sFolder, sFilePath, sPtr - sFilePath);
			// skip empty dir
			if(strlen(sFolder) > 0)
			{
				if(access(sFolder, R_OK) != 0)
				{
					// try to create
					if(mkdir(sFolder, 0766) != 0)
					{
                        CP_SAFE_FREE(sFolder);
						return(1);
					}
				}
			}
		}
		sPtr++;
	}

	CP_SAFE_FREE(sFolder);

	if(access(sFilePath, R_OK) != 0)
	{
		// try to create
		if(mkdir(sFilePath, 0766) != 0)
		{
			return(1);
		}
	}
	
	return(0);
}

int OpenFile(const char *sFileName, int nFlag)
{
	int			nHandle;
	char        theMemBlock[CP_MAX_FILE_NAME_LEN];

	// make sure the folder exist
	GetParentFromPath(sFileName, theMemBlock, CP_MAX_FILE_NAME_LEN);
	if(strlen(theMemBlock) > 0)
	{
		if(PrepareFolder(theMemBlock) != 0)
		{
			return(0);
		}
	}

	nHandle = CP_OPEN(sFileName, nFlag);
	return(nHandle);
}

int RemoveFolder(const char *sFolder)
{
	FindFolder	theFindFolder;
	int				nHandle = CP_FIND_HANDLE_NULL, nReturn = 0;
	FindInfo		theFindInfo;
	char			*sFileName = NULL;

	sFileName = (char *)CP_MALLOC(CP_MAX_FILE_NAME_LEN);
	theFindFolder.BeginFind(sFolder);
	while((nHandle = theFindFolder.GetNext(nHandle, &theFindInfo)) != CP_FIND_HANDLE_NULL)
	{
		if(theFindInfo.cType == CP_FIND_TYPE_FOLDER)
		{
			sprintf(sFileName, "%s/%s", sFolder, theFindInfo.sName);
			if(RemoveFolder(sFileName) != 0)
			{
				nReturn = 1;
				break;
			}
		}
		else
		{
			sprintf(sFileName, "%s/%s", sFolder, theFindInfo.sName);
			if(remove(sFileName) != 0)
			{
				nReturn = 1;
				break;
			}
		}
	}
	theFindFolder.EndFind();

	CP_SAFE_FREE(sFileName);

	if(nReturn == 0)
	{
		if(rmdir(sFolder) != 0)
		{
			nReturn = 1;
		}
	}
	return(nReturn);
}

int CopyFile(const char *sDestFile, const char *sSrcFile, const int nBufferSize)
{
	int		nFileSize, nDestHandle, nSrcHandle, nReturn = 0;

	nFileSize = GetFileSize(sSrcFile);
	if(nFileSize < 0)
		return(1);

	nSrcHandle = CP_OPEN(sSrcFile, OPEN_FILE_FLAG_RDONLY  );
	if(nSrcHandle <= 0)
	{
		return(1);
	}
	nDestHandle = CP_OPEN(sDestFile, 
		OPEN_FILE_FLAG_CREATE | OPEN_FILE_FLAG_TRUNC | OPEN_FILE_FLAG_WRONLY );
	if(nDestHandle <= 0)
	{
		CP_CLOSE(nSrcHandle);
		return(1);
	}
	if(nFileSize > 0)
	{
		nReturn = CopyFile(nDestHandle, nSrcHandle, nFileSize, nBufferSize);
	}

	CP_CLOSE(nSrcHandle);
	CP_CLOSE(nDestHandle);
	return(nReturn);
}

int MoveFile(const char *sOld, const char *sNew)
{
	if(rename(sOld, sNew) != 0)
	{
		if(errno == EXDEV)
		{
			// copy file
			if(CopyFile(sNew, sOld, CP_COPY_BUFFER_SIZE) != 0)
			{
				return(1);
			}
			remove(sOld);
		}
		else
		{
			return(1);
		}
	}
	return(0);
}

int MoveDir(const char *sOld, const char *sNew)
{
	if(rename(sOld, sNew) != 0)
	{
		if(errno == EXDEV)
		{
			// copy file
			if(CopyDir(sNew, sOld, CP_COPY_BUFFER_SIZE) != 0)
			{
				return(1);
			}
			RemoveFolder(sOld);
		}
		else
		{
			return(1);
		}
	}
	return(0);
}

int CopyDir(const char *sDestDir, const char *sSrcDir, const int nBufferSize)
{
	FindFolder	theFindFolder;
	int				nHandle = CP_FIND_HANDLE_NULL, nReturn = 0;
	FindInfo		theFindInfo;
	char			*sOldFile = NULL, *sNewFile = NULL;

	// create dest dir
	if(MakeDir(sDestDir) != 0)
	{
		return(1);
	}

	sOldFile = (char *)CP_MALLOC(CP_MAX_FILE_NAME_LEN);
	sNewFile = (char *)CP_MALLOC(CP_MAX_FILE_NAME_LEN);
	theFindFolder.BeginFind(sSrcDir);
	while((nHandle = theFindFolder.GetNext(nHandle, &theFindInfo)) != CP_FIND_HANDLE_NULL)
	{
		if(theFindInfo.cType == CP_FIND_TYPE_FILE)
		{
			sprintf(sOldFile, "%s/%s", sSrcDir, theFindInfo.sName);
			sprintf(sNewFile, "%s/%s", sDestDir, theFindInfo.sName);
			if(CopyFile(sNewFile, sOldFile, nBufferSize) != 0)
			{
				nReturn = 1;
				break;
			}
		}
		else if(theFindInfo.cType == CP_FIND_TYPE_FOLDER)
		{
			sprintf(sOldFile, "%s/%s", sSrcDir, theFindInfo.sName);
			sprintf(sNewFile, "%s/%s", sDestDir, theFindInfo.sName);
			if(CopyDir(sNewFile, sOldFile, nBufferSize) != 0)
			{
				nReturn = 1;
				break;
			}
		}
	}
	theFindFolder.EndFind();

	CP_SAFE_FREE(sOldFile);
	CP_SAFE_FREE(sNewFile);

	return(nReturn);
}

void GetExtension(const char *sFileName, char *sExt, const int nSize)
{
	int		nLen, n;

	nLen = strlen(sFileName);
	if(nLen <= 0)
	{
		sExt[0] = 0;
		return;
	}
	for(n=nLen - 1; n>=1; --n)
	{
		if(sFileName[n] == '.')
			break;
	}
	_Strcpy(sExt, &(sFileName[n+1]), nSize);
}

/***************************************************************************/
/*  Func Name  : gsGetSetting                                              */
/*  Description: Get settings from the specified file                      */
/*  Parameters : asSetting,char*,buffer to store the queried result        */
/*               asFile,char*,name with path of the configuration file     */
/*               asSection,char*,section name containing the requested key */
/*               asKey,char*,key name                                      */
/*  Return Val : asSetting                                                 */
/*  Update Log :                                                           */
/*  Date     Author  Description                                           */
/*  ---------------------------------------------------------------------- */
/*  98.01.20 Jeikul  for a multithread env., the buffer should be allocated*/
/*                   outside the gsGetSetting. It's supposed that the      */
/*                   buffer is large enough.                               */
/*  98.02.23 Jeikul  First release                                         */
/*  98.08.26 jeikul  Support /_*...*_/ inline memo                         */
/*                                                                         */
/***************************************************************************/

char * gsGetSetting ( char *asSetting,
  const char *asFile, const char *asSection, const char *asKey )
{
  char *buf, *p, *d, *s, lsSection [SIZE_SECTION] ;
  const char *str ;
  int liFileLen, liStrLen, i ;
  FILE * fp ;

  *asSetting = '\0' ; /* The default value */
  if ( *asSection == '\0' || *asKey == '\0' ) return asSetting ;
  snprintf ( lsSection, sizeof (lsSection), "[%s]", asSection ) ;
  /* Open the configuration file. */
  fp = fopen ( asFile, "rb" ) ;
  if ( fp == NULL ) {
    return asSetting ; /* Cannot open file */
  }
  /* Get the file length */
  fseek ( fp, 0L, SEEK_END ) ;
  liFileLen = ftell ( fp ) ;
  fseek ( fp, 0L, SEEK_SET ) ;
  /* Allocate a buffer to hold the whole file */
  buf = (char *)malloc ( liFileLen ) ;
  if ( buf == NULL ) {
    assert ( fclose ( fp ) == 0 ) ;
    return asSetting ;
  }
  i = fread ( buf, 1, liFileLen, fp ) ;
  assert ( i == liFileLen ) ;
  assert ( fclose ( fp ) == 0 ) ; /* Close the file */
  /* Search for the requested string */
  for ( str = lsSection, liStrLen = strlen(str), p = buf; p < buf + liFileLen - liStrLen; p ++ ) {
    if ( memcmp ( p, str, liStrLen ) == 0 ) { /* Found */
      p += liStrLen ; /* Point to the next char of lsSection or asKey */
      if ( str == lsSection ) {
        str = asKey ; /* Section found, then the Key */
        liStrLen = strlen(str) ;
      } else {
        while ( *p == '=' || *p == ' ' || *p == '\t' ) p ++ ; /* Skip blank characters */
        for ( i=0; *p!='\n' && *p!='\r' && p!=buf+liFileLen && i<256-1; p++, i++ ) 
          * (asSetting + i) = *p ; /* copy the string */
        * (asSetting + i) = '\0' ; /* add the terminate character */
        break ;
      }
    } 
  }
  free ( buf ) ; /* Don't forget to free the memory */
  for ( d = asSetting, s = d; ; s ++ ) {
    if ( *s == '\\' ) { /* Regonize '\n', '\r', \011' characters */
      s ++ ;
      if ( *s == 'n' ) {
        *d = '\n' /*, d ++, *d = '\r'*/ ;
      } else {
        if ( ! isdigit ( *s ) )
        *d = (*s - '0') * 100 + (*(s+1) - '0') * 10 + (*(s+2) - '0') ;
        s += 2 ;
      }  
    } else 
      *d = * s ;
    d ++ ;
    if ( *s == ';' || *s == '/' && *(s+1) == '*' ) *s = '\0' ; /* Skip the memo */
    if ( *s == '\0' ) break ;
  }
  assert ( fclose ( fp ) == 0 ) ;
  return asSetting ;
}

/***************************************************************************/
/*  Func Name  : giSaveSetting                                             */
/*  Description: Save settings value to the configuration file             */
/*  Parameters : asFile,char*,name with path of the configuration file     */
/*               asSection,char*,section name containing the requested key */
/*               asKey,char*,key name                                      */
/*               asValue,char*,buffer containing the setting value         */
/*  Return Val : 0,success                                                 */
/*               -1, wrong arguments.                                      */
/*               -2, cannot open file for reading.                         */
/*               -3, not enough memory                                     */
/*               -4, key not found                                         */
/*               -5, cannot open file for writing.                         */
/*  Update Log :                                                           */
/*  Date     Author  Description                                           */
/*  ---------------------------------------------------------------------- */
/*  98.02.23 Jeikul  First release                                         */
/*                                                                         */
/***************************************************************************/
int giSaveSetting (
  const char *asFile, const char *asSection, const char *asKey, const char *asValue )
{
  char *buf, *p, *pSection, *pVal, lsSection [SIZE_SECTION], ssSetting[256] ;
  const char *str ;
  int liFileLen, liStrLen, i ;
  FILE * fp ;

  if ( !asFile || !asSection || !asKey || !asValue ) return -1 ;
  if ( *asSection == '\0' || *asKey == '\0' ) return -1 ;
  snprintf ( lsSection, sizeof (lsSection), "[%s]", asSection ) ;
  /* Open the configuration file. */
  fp = fopen ( asFile, "rb" ) ;
  if ( fp == NULL ) {
    return -2 ; /* Cannot open file */
  }
  /* Get the file length */
  fseek ( fp, 0L, SEEK_END ) ;
  liFileLen = ftell ( fp ) ;
  fseek ( fp, 0L, SEEK_SET ) ;
  /* Allocate a buffer to hold the whole file */
  buf = (char *)malloc ( liFileLen + 256) ;
  if ( buf == NULL ) {
    assert ( fclose ( fp ) == 0 ) ;
    return -3 ;
  }
  * buf = '\0' ;
  i = fread ( buf, 1, liFileLen, fp ) ;
  assert ( i == liFileLen ) ;
  assert ( fclose ( fp ) == 0 ) ; /* Close the file */
  /* Search for the requested string */
  pSection = NULL ;
  for ( str = asSection, liStrLen = strlen(str), p = buf; p != buf + liFileLen; p ++ ) {
    if ( memcmp ( p, str, liStrLen ) == 0 ) { /* Found */
      p += liStrLen ; /* Point to the next char of asSection or asKey */
      if ( str == asSection ) {
        pSection = p ;
        str = asKey ; /* Section found, then the Key */
        liStrLen = strlen(str) ;
      } else {
        while ( *p == '=' || *p == ' ' || *p == '\t' ) p ++ ; /* Skip blank characters */
        pVal = p ;
        for ( i=0; *pVal!='\n' && *pVal!='\r' && pVal!=buf+liFileLen && i<256-1; pVal++,i++ ) 
          * (ssSetting + i) = *pVal ; /* copy the string */
        * (ssSetting + i) = '\0' ; /* add the terminate character */
        break ;
      }
    }
  }
  if ( p == buf + liFileLen ) { /* The Entry Not Found */
#ifdef AUTO_ADD_KEY
    for ( liInsPos = liFileLen; liInsPos > 0; ) { /* Reserved the EOF flag */
      ch = *(buf + liInsPos - 1) ;
      if ( !isprint(ch) && ch != '\r' && ch != '\n' ) {
        liInsPos -- ;
      }
    }
    if ( pSection == NULL ) { /* The Section not found */
      strcat ( lsSection, "\r\n" ) ;
      memmove ( buf + liInsPos + strlen(lsSection), buf + liInsPos, liFileLen - liInsPos ) ;
      memmove ( buf + liInsPos, lsSection, strlen(lsSection) ) ;
      liFileLen += strlen(lsSection) ;
      pSection = buf + liInsPos + strlen(lsSection) ;
    }
    while ( IsSpace (*pSection) ) pSection ++ ;
#else
    return -4 ;
#endif
  }
  memmove ( p + strlen(asValue), p + strlen(ssSetting), liFileLen - (p - buf) ) ;
  memmove ( p, asValue, strlen(asValue) ) ;
  fp = fopen ( asFile, "wb+" ) ;
  if ( fp == NULL ) {
    return -5 ;
  }
  fwrite ( buf, 1, liFileLen - strlen(ssSetting) + strlen(asValue), fp ) ;
  assert ( fclose ( fp ) == 0 ) ;
  return 0 ;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
	size_t iSize = 0;
	time_t tEnd;
	int iErrno = 0;
	ssize_t iRet = 0;
	for (tEnd = time ( NULL ) + DISK_IO_TIMEOUT; iSize < count && tEnd > time ( NULL ); ) 
	{
		iRet = write(fd, (char *) buf + iSize, count - iSize) ;
		if (iRet > 0)
		{
			iSize += iRet;
		}
		else if (errno != 0)
		{
			iErrno = errno;
			LogErr("Comm::Write ERROR: fd=%i,buf%c=NULL,count=%i,ret=%i,errno=%d,errstr=%s",
					fd, buf ? '!' : '=', count, iRet, errno, strerror(errno));
			sleep(1);
		}
		else
		{
			break;
		}
	}
    return iSize;
}

ssize_t Read(int fd, void *buf, size_t count)
{
	size_t iSize = 0;
	time_t tEnd;
	int iErrno = 0;
	ssize_t iRet = 0;
	for (tEnd = time ( NULL ) + DISK_IO_TIMEOUT; iSize < count && tEnd > time ( NULL ); ) 
	{
		iRet = read(fd, (char *) buf + iSize, count - iSize) ;
		if (iRet > 0)
		{
			iSize += iRet;
		}
		else if (errno != 0)
		{
			iErrno = errno;
			LogErr("Comm::Write ERROR: fd=%i,buf%c=NULL,count=%i,ret=%i,errno=%d,errstr=%s",
					fd, buf ? '!' : '=', count, iRet, errno, strerror(errno));
			sleep(1);
		}
		else
		{
			break;
		}
	}
    return iSize;
}

}

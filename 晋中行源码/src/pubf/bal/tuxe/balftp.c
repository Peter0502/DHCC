#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <libgen.h>

#include "atmi.h"
#include "balftp.h"

int  bferrno;

int 	balftp_fileinfo(T_BFR*);
int 	balftp_getfile(T_BFR*, T_BFR**, long*);
int 	balftp_putfile(T_BFR*);
void 	balftp_ntoh(T_BFHD*);
void 	balftp_hton(T_BFHD*);
int 	balftp_common_checkreq(T_BFR*, long);
int 	balftp_get_target_file(T_BFR*);
int 	balftp_fileinfo_checkreq(T_BFR*, long);
int 	balftp_getfile_checkreq(T_BFR*, long);
int 	balftp_putfile_checkreq(T_BFR*, long);
int 	balftp_geterr(int);
int     balftp_check_readperm(char*);
int     balftp_check_writeperm(char*);
void 	balftp_trimline(char* szLine);

/*static char*  m_cfgfile;*/
static char   m_cfgfile[MAXPATHLEN + 1];
static char   m_filename[MAXPATHLEN + 1];

int tpsvrinit(int argc, char** argv)
{
	tpopen();
	if( NULL == (strcpy(m_cfgfile , getenv("BALFTPCFG"))))

	{
		userlog("%d:ERROR: Environment variable BALFTPCFG undefined!",__LINE__);
		return -1;
	}

	userlog("balftp init!---%s",m_cfgfile);

	return 0;
}

void tpsvrdone( void )
{
	tpclose();
	userlog("BALFTP exit.");
}

void BALFTP(TPSVCINFO* ptpinfo)
{
	T_BFR*	pBFR;
	T_BFR*  pBFRRpl;
	long    lRplLen;

	/** PERFORMANCE **/
/*
	struct timeval BeginTime, EndTime;
	void *Tzp;
	gettimeofday(&BeginTime,Tzp);
*/
	pBFR = (T_BFR*)ptpinfo->data;

	if( -1 == balftp_common_checkreq(pBFR, ptpinfo->len ) )
	{
		userlog("%d:ERROR: Invliad request", __LINE__);
		tpreturn(TPFAIL, bferrno, NULL, 0, 0L);
	}
	if( -1 == balftp_get_target_file(pBFR) )
	{
		userlog("%d:ERROR: Invliad request", __LINE__);
		
		tpreturn(TPFAIL, EBALFTP_INVALID_FILE, NULL, 0, 0L);
	}

	switch (pBFR->head.mesgtype)
	{
	case BALFTP_FILEINFO:
		if( -1 == balftp_fileinfo_checkreq(pBFR, ptpinfo->len ) )
		{
			userlog("%d:ERROR: Invliad request", __LINE__);
			
			tpreturn(TPFAIL, bferrno, NULL, 0, 0L);
		}

		if( -1 == balftp_fileinfo(pBFR) )
		{
			
			tpreturn(TPFAIL, bferrno, NULL, 0, 0L);
		}
		
		tpreturn(TPSUCCESS, 0, (char *)pBFR, sizeof(pBFR->head), 0L);

		break;
	case BALFTP_GETFILE:
		if( -1 == balftp_getfile_checkreq(pBFR, ptpinfo->len ) )
		{
			userlog("%d:ERROR: Invliad request", __LINE__);
			
			tpreturn(TPFAIL, bferrno, NULL, 0, 0L);
		}

		if( -1 == balftp_fileinfo(pBFR) )
		{
			
			tpreturn(TPFAIL, bferrno, NULL, 0, 0L);
		}

		if( -1 == balftp_getfile(pBFR, &pBFRRpl, &lRplLen) )
		{
			
			tpreturn(TPFAIL, bferrno, NULL, 0, 0L);
		}
		
		tpreturn(TPSUCCESS, 0, (char *)pBFRRpl, lRplLen, 0L);

		break;
	case BALFTP_PUTFILE:
		if( -1 == balftp_putfile_checkreq(pBFR, ptpinfo->len ) )
		{
			userlog("%d:ERROR: Invliad request", __LINE__);
			
			tpreturn(TPFAIL, bferrno, NULL, 0, 0L);
		}

		if( -1 == balftp_putfile(pBFR) )
		{
			
			tpreturn(TPFAIL, bferrno, NULL, 0, 0L);
		}
		
		tpreturn(TPSUCCESS, 0, (char *)pBFR, sizeof(pBFR->head), 0L);

		break;
	default:
		
		tpreturn(TPFAIL, EBALFTP_UNKNOWN_REQUEST, NULL, 0, 0L);
	}
}

int balftp_fileinfo(T_BFR* pBFR)
{
	struct stat  tFileStat;
	
	/**userlog("%d:m_filename: [%s]", __LINE__, m_filename);**//**add by hxc 091126**/
	if( -1 == stat( m_filename, &tFileStat ) )
	{
		bferrno = balftp_geterr(errno);
		userlog("%d:ERROR(%d): stat - %s", __LINE__, errno, strerror(errno));
		return -1;
	}

	if( ( tFileStat.st_mode & S_IFMT ) != S_IFREG )
	{
		userlog("%d:ERROR: st_mode = [%lX]", __LINE__, tFileStat.st_mode);
		bferrno = EBALFTP_INVALID_FILE;
		return -1;
	}

	pBFR->head.filesize  = tFileStat.st_size;
	pBFR->head.modtime   = tFileStat.st_mtime;
	/****Modified by SSH,2006.8.22,一次传完整个文件****/
	pBFR->head.datasize  = tFileStat.st_size;
	pBFR->head.offset  = 0;
	/**************************************************/
	

	balftp_hton(&pBFR->head);
	return 0;
}

int balftp_getfile(T_BFR* pBFR, T_BFR** ppBFRRpl, long* plRplLen)
{
	struct stat  tFileStat;
	FILE*        fp;

	/** check whether file is exist and not modified since last access **/
	if( -1 == stat( m_filename, &tFileStat ) )
	{
		userlog("%d:ERROR(%d): stat - %s", __LINE__, errno, strerror(errno));
		bferrno = balftp_geterr(errno);
		return -1;
	}

	if( ( tFileStat.st_mode & S_IFMT ) != S_IFREG )
	{
		bferrno = EBALFTP_INVALID_FILE;
		return -1;
	}
	if (pBFR->head.offset == 0){
		pBFR->head.filesize = tFileStat.st_size;
		pBFR->head.modtime = tFileStat.st_mtime;
		if (pBFR->head.filesize <= pBFR->head.datasize)
			pBFR->head.datasize =  pBFR->head.filesize;
	}
	if(tFileStat.st_size  != pBFR->head.filesize ||
	   tFileStat.st_mtime != pBFR->head.modtime  )
	{
		userlog("%d:ERROR: file changed", __LINE__);
		bferrno = EBALFTP_FILE_CHANGED;
		return -1;
	}

	if( pBFR->head.offset + pBFR->head.datasize > pBFR->head.filesize )
	{
		userlog("%d:ERROR: invalid request", __LINE__);
		bferrno = EBALFTP_INVALID_REQUEST;
		return -1;
	}

	/** open file  **/
	if( NULL == ( fp = fopen(m_filename, "r") ) )
	{
		userlog("%d:ERROR(%d): fopen - %s", errno, strerror(errno), __LINE__);
		bferrno = EBALFTP_FILEOPEN;
		return -1;
	}

	/** allocate reply buf **/
	*plRplLen = sizeof(T_BFHD) + pBFR->head.datasize;
	if( NULL == ( *ppBFRRpl = (T_BFR*)tpalloc("CARRAY", NULL, *plRplLen)))
	{
		userlog("%d:ERROR(%d): tpalloc - %s", 
				tperrno, 
				tpstrerror(tperrno),
				__LINE__);
		bferrno = EBALFTP_ALLOC_ERROR;
		fclose(fp);
		return -1;
	}

	memcpy(&(*ppBFRRpl)->head, &pBFR->head, sizeof(pBFR->head));

	/** seek to offset **/
	if( -1 == fseek(fp, pBFR->head.offset, SEEK_SET) )
	{
		userlog("%d:ERROR(%d): fseek - %s", __LINE__, errno, strerror(errno));
		bferrno = EBALFTP_FILESEEK;
		fclose(fp);
		tpfree((char*)(*ppBFRRpl));
		return -1;
	}

	/** read file **/
	(*ppBFRRpl)->head.datasize = fread( (*ppBFRRpl)->data, 
										1, 
										pBFR->head.datasize, 
										fp);
	if( -1 == (*ppBFRRpl)->head.datasize )
	{
		userlog("%d:ERROR(%d): fread - %s", __LINE__, errno, strerror(errno));
		bferrno = EBALFTP_FILEREAD;
		fclose(fp);
		tpfree((char*)(*ppBFRRpl));
		return -1;
	}

	/** close file **/
	fclose(fp);

	balftp_hton(&(*ppBFRRpl)->head);

	return 0;
}

int balftp_putfile(T_BFR* pBFR)
{
	struct stat  tFileStat;
	FILE*        fp;

	/** check whether file is exist and not modified since last access if 
		offset is not equal zero **/

	if( -1 == stat( m_filename, &tFileStat ) )
	{
		if( pBFR->head.offset != 0  || errno != ENOENT )
		{
			userlog("%d:ERROR(%d): stat - %s", __LINE__, 
					errno, 
					strerror(errno));
			bferrno = balftp_geterr(errno);
			return -1;
		}

		/** new file  **/
		if( NULL == ( fp = fopen(m_filename, "w+") ) )
		{
			userlog("%d:ERROR(%d): fopen - %s", __LINE__, 
					errno, 
					strerror(errno));
			bferrno = EBALFTP_FILEOPEN;
			return -1;
		}
	}
	else /** file exist ***/
	{
		if( ( tFileStat.st_mode & S_IFMT ) != S_IFREG )
		{
			bferrno = EBALFTP_INVALID_FILE;
			return -1;
		}

		if ( pBFR->head.offset == 0 ) /** first writing **/
		{
			if( BFLAG_NORMAL == pBFR->head.flag )
			{
				userlog("%d:ERROR: file exist!", __LINE__);
				bferrno = EBALFTP_FILE_EXIST;
				return -1;
			}
			else  if( BFLAG_OVERWRITE == pBFR->head.flag )
			{
				if( NULL == ( fp = fopen(m_filename, "w+") ) )
				{
					userlog("%d:ERROR(%d): fopen - %s", __LINE__, 
				         	errno, 
					        strerror(errno));
					bferrno = EBALFTP_FILEOPEN;
					return -1;
				}
			}
			else
			{
				userlog("%d:ERROR: invalid flag setting", __LINE__);
				bferrno = EBALFTP_INVALID_REQUEST;
				return -1;
			}
		}
		else /* continue writing */
		{
			if (tFileStat.st_size  != pBFR->head.offset)
			{
				userlog("ERROR: file changed");
				bferrno = EBALFTP_FILE_CHANGED;
				return -1;
			}

			if( NULL == ( fp = fopen(m_filename, "a+") ) )
			{
				userlog("%d:ERROR(%d): fopen - %s", __LINE__, 
					    errno, 
					    strerror(errno));
				bferrno = EBALFTP_FILEOPEN;
				return -1;
			}
		}
	}

	/***
	if( pBFR->head.offset + pBFR->head.datasize > pBFR->head.filesize )
	{
		userlog("%d:ERROR: invalid request", __LINE__);
		bferrno = EBALFTP_INVALID_REQUEST;
		return -1;
	}
	****/

	/** seek to offset **/
	/**
	if( -1 == fseek(fp, pBFR->head.offset, SEEK_SET) )
	{
		userlog("%d:ERROR(%d): fseek - %s", __LINE__, errno, strerror(errno));
		bferrno = EBALFTP_FILESEEK;
		fclose(fp);
		return -1;
	}
	***/

	/** write file **/
	pBFR->head.datasize = fwrite( pBFR->data, 1, pBFR->head.datasize, fp );
	if( -1 == pBFR->head.datasize )
	{
		userlog("%d:ERROR(%d): fread - %s", __LINE__, errno, strerror(errno));
		bferrno = EBALFTP_FILEREAD;
		fclose(fp);
		return -1;
	}
	userlog("%s,%d,Write file[%s][%d]\n",__FILE__,__LINE__,m_filename,pBFR->head.datasize);

	/** close file **/
	fclose(fp);

	balftp_hton(&pBFR->head);
	return 0;
}

void balftp_ntoh(T_BFHD* pBFHD)
{
	pBFHD->filesize = (long)ntohl((unsigned long)pBFHD->filesize);
	pBFHD->modtime  = (long)ntohl((unsigned long)pBFHD->modtime );
	pBFHD->offset   = (long)ntohl((unsigned long)pBFHD->offset  );
	pBFHD->datasize = (long)ntohl((unsigned long)pBFHD->datasize);
}

void balftp_hton(T_BFHD* pBFHD)
{
	pBFHD->filesize = (long)htonl((unsigned long)pBFHD->filesize);
	pBFHD->modtime  = (long)htonl((unsigned long)pBFHD->modtime );
	pBFHD->offset   = (long)htonl((unsigned long)pBFHD->offset  );
	pBFHD->datasize = (long)htonl((unsigned long)pBFHD->datasize);
}

int balftp_common_checkreq(T_BFR* pBFR, long len)
{
	userlog("%s,%d,Write file[%d][%d]\n",__FILE__,__LINE__,len,sizeof(T_BFHD));
	userlog("%s,%d,Write file[%s][%d]\n",__FILE__,__LINE__,m_filename,pBFR->head.datasize);
	if( len < sizeof(T_BFHD) )
	{
		bferrno = EBALFTP_INVALID_REQUEST;
		return -1;
	}

	balftp_ntoh(&pBFR->head);


	return 0;
}

int balftp_get_target_file(T_BFR* pBFR)
{
	char filebase[MAXPATHLEN + 1];
	char filedir[MAXPATHLEN + 1];
	char  tmpname[MAXPATHLEN + 1];
	char  token[128];
	int   i,j,k;


	memset(tmpname, '\0', MAXPATHLEN + 1);
	strcpy(tmpname,getenv("FILDIR"));
	strcat(tmpname,"/");
	
	k = strlen(tmpname);
/*
	for( i = 0, k = 0; i < strlen(pBFR->head.name); i++)
*/
	for( i = 0 ; i < strlen(pBFR->head.name); i++)
	{
		if( '$' == pBFR->head.name[i] )
		{
			j = 0;
			i++;
			while( (isalpha(pBFR->head.name[i]) || '_' == pBFR->head.name[i] ) 
				   && j < 128)
			{
				token[j++] = pBFR->head.name[i++];
			}
			token[j] = '\0';
			i--;

			strcat(tmpname, getenv(token));
			k = strlen(tmpname);
		}
		else
		{
			tmpname[k++] = pBFR->head.name[i];
		}
	}

	for( i = strlen(tmpname) - 1;  i >= 0 && tmpname[i] != '/'; i-- );

	if( i < 0 )
	{
		strcpy(m_filename, tmpname);
	}
	else
	{
		memcpy(filedir, tmpname, i );
		filedir[i] = '\0';
		strcpy(filebase, &tmpname[i + 1]);

		if(  NULL == realpath(filedir, m_filename) )
		{
			userlog("%d:ERROR(%d): realpath(%s) - %s", __LINE__, 
					errno, 
					filedir,
					strerror(errno));
			return -1;
		}


		strcat(m_filename, "/");
		strcat(m_filename, filebase);
	}

	return 0;
}

int balftp_fileinfo_checkreq(T_BFR* pBFR, long len)
{
	if( len > sizeof(T_BFHD) )
	{
		userlog("%d:ERROR: size error!", __LINE__);
		bferrno = EBALFTP_INVALID_REQUEST;
		return -1;
	}

	if( -1 == balftp_check_readperm(m_filename) && 
		-1 == balftp_check_writeperm(m_filename) )
	{
		userlog("%d:ERROR: permission denied!", __LINE__);
		bferrno = EBALFTP_PERM;
		return -1;
	}

	return 0;
}

int balftp_getfile_checkreq(T_BFR* pBFR, long len)
{
	if( len > sizeof(T_BFHD) )
	{
		userlog("%d:ERROR: size error[%d][%d]!", __LINE__,len,sizeof(T_BFHD));
		bferrno = EBALFTP_INVALID_REQUEST;
		return -1;
	}
/*
	if( -1 == balftp_check_readperm(m_filename))
	{
		userlog("%d:ERROR: permission denied!", __LINE__);
		bferrno = EBALFTP_PERM;
		return -1;
	}
*/
	return 0;
}

int balftp_putfile_checkreq(T_BFR* pBFR, long len)
{
	if( len != sizeof(T_BFHD) + pBFR->head.datasize )
	{
		userlog("%d:ERROR: size error! - datasize[%ld] , mesglen[%ld]", 
				__LINE__, 
				pBFR->head.datasize,
				len);
		bferrno = EBALFTP_INVALID_REQUEST;
		return -1;
	}
/*
	if( -1 == balftp_check_writeperm(m_filename) )
	{
		userlog("%d:ERROR: permission denied!", __LINE__);
		bferrno = EBALFTP_PERM;
		return -1;
	}
*/
	return 0;
}

int balftp_geterr(int syserr)
{
	switch(syserr)
	{
	case EACCES:
		return EBALFTP_NOTACCESS;
	case ENOENT:
	case ENOTDIR:
		return EBALFTP_NOTEXIST;
	default:
		return EBALFTP_SYSTEM;
	}
}

int balftp_check_readperm(char* filename)
{
	char line[BAL_LINE_MAX + 1];
	char readpath[MAXPATHLEN + 1];
	char rrpath[MAXPATHLEN + 1];

	FILE *fp;

	if( NULL == (fp = fopen(m_cfgfile, "r")))
	{
		userlog("%d:ERROR(%d): fopen - %s", __LINE__, errno, strerror(errno));
		return -1;
	}

	while( 0 == balftp_get_readpath(fp, readpath, MAXPATHLEN + 1) )
	{
		if( -1 == balftp_getrfname(readpath, rrpath, sizeof(rrpath)) )
		{
			continue;
		}

		if( strlen(m_filename)  < strlen(rrpath) )
		{
			continue;
		}

		if( 0 != memcmp(rrpath, m_filename, strlen(rrpath) ) )
		{
			continue;
		}

		if( m_filename[strlen(rrpath)] != '/' &&
			m_filename[strlen(rrpath)] != '\0' )
		{
			continue;
		}

		fclose(fp);
		return 0;
	}

	fclose(fp);
	return -1;
}

int balftp_check_writeperm(char* filename)
{
	char line[BAL_LINE_MAX + 1];
	char writepath[MAXPATHLEN + 1];
	char rwpath[MAXPATHLEN + 1];

	FILE *fp;

	if( NULL == (fp = fopen(m_cfgfile, "r")))
	{
		userlog("%d:ERROR(%d): fopen - %s", __LINE__, errno, strerror(errno));
		return -1;
	}

	while( 0 == balftp_get_writepath(fp, writepath, MAXPATHLEN + 1) )
	{
		if( -1 == balftp_getrfname(writepath, rwpath, sizeof(rwpath)) )
		{
			continue;
		}

		if( strlen(m_filename)  < strlen(rwpath) )
		{
			continue;
		}

		if( 0 != memcmp(rwpath, m_filename, strlen(rwpath) ) )
		{
			continue;
		}

		if( m_filename[strlen(rwpath)] != '/' &&
			m_filename[strlen(rwpath)] != '\0' )
		{
			continue;
		}

		fclose(fp);
		return 0;
	}

	fclose(fp);
	return -1;
}

int balftp_getrfname(char* fname, char* rfname, int rf_len)
{
	char  tmpname[MAXPATHLEN + 1];
	char  token[128];
	int   i,j,k;

	memset(tmpname, '\0', MAXPATHLEN + 1);
	for( i = 0, k = 0; i < strlen(fname); i++)
	{
		if( '$' == fname[i] )
		{
			j = 0;
			i++;
			while( (isalpha(fname[i]) || '_' == fname[i] ) && j < 128)
			{
				token[j++] = fname[i++];
			}
			token[j] = '\0';
			i--;

			strcat(tmpname, getenv(token));
			k = strlen(tmpname);
		}
		else
		{
			tmpname[k++] = fname[i];
		}
	}

	if(  NULL == realpath(tmpname, rfname) )
	{
		userlog("%d:ERROR(%d): realpath(%s) - %s", __LINE__, 
				errno, 
				tmpname,
				strerror(errno));
		return -1;
	}

	return 0;
}

int balftp_get_readpath(FILE* fp, char* read_path, int len)
{
	char saLine[MAXPATHLEN + 1];

	while(fgets(saLine, MAXPATHLEN, fp) != NULL )
	{
		balftp_trimline(saLine);

		/****** Ignor comment ****/
		if( saLine[0] == '#' )
		{
			continue;
		}
		
		if( memcmp("READ:", saLine, 5) == 0 )
		{
			strcpy(read_path, &saLine[5]);
			balftp_trimline(read_path);
			return 0;
		}
	}

	return  -1;
}

int balftp_get_writepath(FILE* fp, char* write_path, int len)
{
	char saLine[MAXPATHLEN + 1];

	while(fgets(saLine, MAXPATHLEN, fp) != NULL )
	{
		balftp_trimline(saLine);

		/****** Ignor comment ****/
		if( saLine[0] == '#' )
		{
			continue;
		}
		
		if( memcmp("WRITE:", saLine, 6) == 0 )
		{
			strcpy(write_path, &saLine[6]);
			balftp_trimline(write_path);
			return 0;
		}
	}

	return  -1;
}

void balftp_trimline(char* szLine)
{
	int i;
	int offset;

	for( i = strlen(szLine) - 1; 
         i >= 0	&&	( szLine[i] == '\n' ||	szLine[i] == '\t' || 
		 szLine[i] == ' ' );
		 i-- )
	{
		szLine[i] = '\0';
	}

	for( offset  = 0; 
	     offset < strlen(szLine) && (szLine[offset] == ' ' || 
                                     szLine[offset] == '\t');
		 offset++ );

	for( i = 0; i <= strlen(szLine) - offset; i++)
	{
		szLine[i] = szLine[i + offset];
	}
}


#include "bal.h"
#include <stdlib.h>
#include <malloc.h>

void 			bal_gettime( char*);
void 			bal_gen_audit(T_BTR*, int);
unsigned long 	bal_get_hostip();
unsigned long 	bal_get_svcdigit();
long 			bal_get_responseid(char*);
int 			bal_callback_rpl_ind(T_BTR*, int);
int 			bal_is_callback_rpl_ind();
void 			bal_dumpmesg(char*, char*, int);
char* 			balstrerror(int);
char* 			balchstrerror(int);
void			bal_return_error( TPSVCINFO* ptpinfo );
void 			bal_dump_mallocinfo (char *);
int 			bal_get_req_version(T_BTR*);

void bal_gettime( char* pTime )
{
   struct timeb nowb;
   struct tm    *now;

   ftime( &nowb );
   now = localtime( &nowb.time );
   sprintf( pTime, "%02d%02d%02d%03hd", now->tm_hour, now->tm_min,
                                        now->tm_sec, nowb.millitm );
}

/***************************************************************************\
 *
 *  FUNCTION:		unsigned long bal_get_hostip()
 *  DESCRIPTION:	return the host's IP, just more than one exists, just
 *					return the first IP.
 *  INPUT:
 *  OUTPUT:
 *  RETURN:			ip, success.
 *					-1, error.
\***************************************************************************/
unsigned long bal_get_hostip()
{
	struct utsname host_utsname;
	struct hostent *host_ent;
	
	if( -1 == uname(&host_utsname))
	{
		printf("ERROR(%d): uname - %s\n",
		        errno,
		        strerror(errno));
		return -1;
	}
	
	host_ent = gethostbyname(host_utsname.nodename);
	if( NULL == host_ent )
	{
		printf("ERROR(%d): gethostbyname - %s\n",
		        errno,
		        strerror(errno));
		return -1;
	}
	
	return *(unsigned long*)host_ent->h_addr;
}

/***************************************************************************\
 *
 *  FUNCTION:		int bal_get_svcdigit
 *  DESCRIPTION:	According to IP addr of host and process id, generate 
 *					the unique id of process.
 *  INPUT:
 *  OUTPUT:
 *  RETURN:			svcdigit, success;
 *					-1, error.
\***************************************************************************/
unsigned long bal_get_svcdigit()
{
	unsigned short int  proc_id;
	unsigned short int  host_addr;
	unsigned long svcdigit;
	unsigned long host_ip;

	if( - 1 == ( proc_id = (unsigned short int)getpid() ) )
	{
		printf("ERROR(%d): getpid - %s\n",
		        errno,
		        strerror(errno));
		return 0;
	}
	if( -1 == (host_ip = bal_get_hostip() ) )
	{
		printf("ERROR: bal_get_hostip() failed!\n");
		return 0;
	}
	
	svcdigit = proc_id | ( host_ip & 0X0000FFFF ) << 16;
	
	return svcdigit;
}

/***************************************************************************\
 *
 *  FUNCTION:		long bal_get_responseid(char* szCBSName);
 *  DESCRIPTION:	get response_id from cbsname;
 *  INPUT:
 *  OUTPUT:
 *  RETURN:			0, success;
 *					-1, error.
\***************************************************************************/
long bal_get_responseid(char* cbsname)
{
	char tmpname[CSLCBSName + 1];
	long response_id = 0;
	
	memset(tmpname, '\0', CSLCBSName + 1);
	memcpy(tmpname,
	       &cbsname[strlen(POLLCALLSVC_PREFIX)],
	       CSLCBSName - strlen(POLLCALLSVC_PREFIX));
	return strtol(tmpname, NULL, 16);
}

int bal_is_callback_rpl_ind()
{
#ifdef FOR_APCTL

	if( it_txcom.response_id != 0 ){
		printf("it_txcom.response_id [%ld]",it_txcom.response_id);
		return 1;
	}
	else
		return 0;
#endif

}

void bal_dumpmesg(char* prompt, char* text, int len)
{
    FILE* fp;
    int  i;
    int  j;

	vtcp_log("============= Open MESG.LOG ===============");
    fp = fopen("MESG.LOG", "a");
    if( NULL == fp)
	{
		vtcp_log("============= Open MESG.LOG ERROR ===============");
		return;
	}

    fprintf(fp, "===================================\n");
	fprintf(fp, "%s\n", prompt);
    fprintf(fp, "mesglen = [%d]\n", len);
    for( i = 0; i < len; i+= 16)
    {
        for( j = 0; j < 16; j++)
        {
            if( i + j < len )
            {
                fprintf(fp, "%02X ", (unsigned char)text[i+j]);
            }
            else
            {
                fprintf(fp, "   ");
            }

            if( j == 16 / 2 - 1 )
            {
                fprintf(fp, " ");
            }

        }

        for( j = 0; j < 16 && i + j < len; j++)
        {
            if( isprint(text[i + j]) )
            {
                fprintf(fp, "%c", text[i+j]);
            }
            else
            {
                fprintf(fp,".");
            }
        }

        fprintf(fp, "\n");
    }

	fclose(fp);
}

void bal_dump_mallocinfo (char *str)
{
#ifdef BAL_DEBUG
	struct mallinfo	mInfo = mallinfo();

	printf("==> mallinfo(%s)\n", str);

	printf("arena      :  %d\n", mInfo.arena);
	printf("ordblks    :  %d\n", mInfo.ordblks);
	printf("smblks     :  %d\n", mInfo.smblks);
	printf("hblks      :  %d\n", mInfo.hblks);
	printf("hblkhd     :  %d\n", mInfo.hblkhd);
	printf("usmblks    :  %d\n", mInfo.usmblks);
	printf("fsmblks    :  %d\n", mInfo.fsmblks);
	printf("uordblks   :  %d\n", mInfo.uordblks);
	printf("fordblks   :  %d\n", mInfo.fordblks);
	printf("keepcost   :  %d\n", mInfo.keepcost);
#endif
}

char* balchstrerror(int error_no)
{
	static char text[256];

	switch(error_no)
	{
	case EBALSETLOCKWAIT:
		strcpy(text, "错误：设置数据库锁等待时间失败,请联系中心管理员!");
		break;
	case EBALTUXERR:
		strcpy(text, "错误：联机事务处理平台错误,请联系中心管理员!");
		break;
	case EBALREQGLOBAL:
		strcpy(text, "错误：读取全局信息失败,请联系中心管理员!");
		break;
	case EBALTXBEGIN:
		strcpy(text, "错误：无法开始事务,请联系中心管理员!");
		break;
	case EBALTXCOMMIT:
		strcpy(text, "错误：无法提交事务,请联系中心管理员!");
		break;
	case EBALTXABORT:
		strcpy(text, "错误：无法中止事务,请联系中心管理员!");
		break;
	case EBALCALLAUDIT:
		strcpy(text, "错误：记审计日志失败,请联系中心管理员!");
		break;
	case EBALCALLBACKRPLY:
		strcpy(text, "错误：回调应答失败,请联系中心管理员!");
		break;
	case EBALGETCLKEY:
		strcpy(text, "错误：无法得到终端密钥,请联系中心管理员!");
		break;
	case EBALMACERROR:
		strcpy(text, "错误：消息验证失败,请联系中心管理员!");
		break;
	default:
		strcpy(text, "错误：未知错误,请联系中心管理员!");
		break;
	}

	return text;
}

char* balstrerror(int error_no)
{
	static char text[256];

	switch(error_no)
	{
	case EBALSETLOCKWAIT:
		strcpy(text, "ERROR: EBALSETLOCKWAIT - set lock wait time failed!");
		break;
	case EBALTUXERR:
		strcpy(text, "ERROR: EBALTUXERR - TUXEDO error!");
		break;
	case EBALREQGLOBAL:
		strcpy(text, "ERROR: EBALREQGLOBAL- get global info error!");
		break;
	case EBALTXBEGIN:
		strcpy(text, "ERROR: EBALTXBEGIN- begin transaction failed!");
		break;
	case EBALTXCOMMIT:
		strcpy(text, "ERROR: EBALTXCOMMIT - commit transaction failed!");
		break;
	case EBALTXABORT:
		strcpy(text, "ERROR: EBALTXABORT - abort transaction failed!");
		break;
	case EBALCALLAUDIT:
		strcpy(text, "ERROR: EBALCALLAUDIT - call bal_audit failed!");
		break;
	case EBALCALLBACKRPLY:
		strcpy(text, "ERROR: EBALCALLBACKRPLY - callback rpl_ind failed!");
		break;
	case EBALGETCLKEY:
		strcpy(text, "ERROR: EBALGETCLKEY - get client key failed!");
		break;
	case EBALMACERROR:
		strcpy(text, "ERROR: EBALMACERROR - MAC error!");
		break;
	default:
		strcpy(text, "");
		break;
	}

	return text;
}

void bal_return_error( TPSVCINFO* ptpinfo )
{
#ifdef FOR_APCTL
    int   	sendlen;

	sendlen = sizeof(T_BHDR) + 46;

	if (!memcmp(RtCode,"0000",4)){
		memcpy(ptpinfo->data+46,"1",1);
		sprintf(RtCode,"%04d",balerrno);
		memcpy(ptpinfo->data+60,RtCode,4);
		ptpinfo->len=64;
	}
	else{
		bal_abort_tx(&g_balinfo); 
		memcpy(ptpinfo->data+46,"1",1);
		memcpy(ptpinfo->data+60,RtCode,4);
		ptpinfo->len=64;
	}

	return;
#endif
}

int bal_get_req_version(T_BTR* pBTR)
{
	return BAL31_VERSION;
}

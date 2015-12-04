#include "find_debug.h" 
/*************************************************
* 文 件 名: pubf_base_file
* 功能描述：1、包括模块pub_base_GetFileLen计算文件行数；
*  			2、包括模块pub_base_GenDwnFilName生成下传文件名(批量);
*        	3、包括模块pub_base_AllDwnFilName生成下传全路经文件名(批量);
*        	4、pub_base_cut_str在字符串中按照指定分割符取得指定的域;
*        	5、pub_base_readfield 把字符串中按照指定分割符取到指定的字符串;
* 作    者:    
* 完成日期：2003年12月16日
* 修改记录：    
* 1. 日    期:
*    修 改 人:
*    修改内容:
*************************************************/

#include "stdio.h"
#include <varargs.h>
#include <sys/stat.h>
#include <sys/types.h>

#define EXTERN
#include "public.h"

#define loc_t sys_loc_t

/**********************************************************************
* 函数名：    pub_base_GetFileLen
* 函数功能：  计算文件行数
* 作者/时间：    
* 
* 参数：
*     输入：文件名infile
*           
*     输出：-1：交易失败
*              0：交易成功，文件行数为0
*              n：交易成功，文件行数n
*
* 返 回 值: >=0:成功 <0:失败
*           
* 修改历史：
*
********************************************************************/
int pub_base_GetFileLen(char *infile)
{
    int ret;
    char tmpfile[100],comm[150],str0[150],num[8];
    FILE *fp_line;

    sprintf(tmpfile,"%s/fileline.con",getenv("FILDIR"));
    sprintf(comm,"wc -l %s 1>%s",infile,tmpfile);

    ret=system(comm);
    if ( ret )
    {
        sprintf(acErrMsg,"执行[%s]出错!",comm);WRITEMSG
        return -1;
    }
    fp_line=fopen(tmpfile,"r");
    if ( fp_line==NULL )
    {
        sprintf(acErrMsg,"打开[%s]出错!",tmpfile);WRITEMSG
        return -1;
    }
    MEMSET_DEBUG(str0,0x00,sizeof(str0));
    fgets(str0,100,fp_line);
    strncpy(num,str0,sizeof(num)-1);
    num[sizeof(num)-1]=NULL;

    return (atol(num));
}

/**********************************************************************
* 函数名：    pub_base_GenDwnFilName
* 函数功能：  生成下传文件名(批量)
* 作者/时间：    
* 
* 参数：
*     输入：文件名filname
*           
*     输出：无
* 
* 返 回 值: 0: 检查成功 1: 不成功
*    
* 修改历史：
*
********************************************************************/

int pub_base_GenDwnFilName( char * filname )
{
    char brno[10];
    char ttyno[5];
    register int i=0;

    get_zd_data(DC_TX_BR_NO,brno); /* 机构编码 */

    strcpy(filname,brno);

    get_zd_data(DC_TTY,ttyno); /* 终端号 */

    strcat(filname,ttyno);

    pub_base_strpack ( filname );

    return ( 0 );
}
int pub_base_AllDwnFilName( char * allname )
{
    char filname[20];
	FILE *fp;

	pub_base_GenDwnFilName( filname );

    sprintf( allname,"%s/%s",getenv("FILDIR"),filname );
	if( Gl_file_first==1 )
	{
		fp=fopen( allname,"w" );
		fclose(fp);
		Gl_file_first=0;
	}

    return ( 0 );
}
/*
*        sstr 原串
*        dstr 目标输出串
*        num  列数
*        mode 分割符列表
*/
pub_base_cut_str( char *sstr,char *dstr,char *mode,int num )
{
	int i=0,len,j=0,n;
	char tmpstr[1025];
	int flag;
	int ret=0;
	
	len=strlen(sstr);
	if( sstr[len-1]=='\n' ) 
	{
		sstr[len-1]='\0';
		len--;
	}

	MEMSET_DEBUG( tmpstr,0,sizeof(tmpstr) );
	j=0; 
	flag=0; 
	n=0;

	for( i=0;i<=len;i++ )
	{
		if( strchr(mode,sstr[i]) || sstr[i]=='\0' )
		{
			tmpstr[j]='\0';
			/*if( flag==0 ) */
			{
				n++;
				if( n==num )
				{
					flag=2;
					break;
				}
			}
			MEMSET_DEBUG( tmpstr,0,sizeof(tmpstr) );
			j=0; 
			flag=1;
		}
		else
		{
			tmpstr[j]=sstr[i];
			j++;
			flag=0;
		}
	}

	if( flag!=2 )
		ret=100;

	tmpstr[j]='\0';
		strcpy( dstr,tmpstr );

	return ret;
}
/**********************************************************************
* 函 数 名：  pub_base_prom_inf
* 函数功能：  写网点提示信息文件
*         
* 作    者：  lance
* 完成时间：  2003年06月16日
*
* 参    数： 
*     输  入：brno      char      机构号    
*      	      sts       char	  日期状态( 0-取当前日期 ， 1-取当前日期+1 ) 
*             fmtstr          
*             va_alist      
*     输  出: 
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
pub_base_prom_inf( brno , sts , fmtstr , va_alist)
char *brno;
char *sts;
char *fmtstr;
va_dcl
{

	FILE *    fp;
	char    tmpstr[100];
	char    date_tmp[100];
	char    log_tmp[400];
	char    tmpname[100];
	char	comm[100];
	char	add_str[101];
	long	log_date;
	long	num=0;
	int ret=0;
	
	struct stat lfsts;

	va_list ap;
	va_start(ap);
	vsprintf ( log_tmp , fmtstr , ap );
	va_end(ap);
	

	/* 取日期时间 */
	if( sts[0]=='0' )
	{
		log_date = g_pub_tx.tx_date ;
	}else if( sts[0]=='1' ){
		ret = pub_base_deadlineD( g_pub_tx.tx_date , 1 , &log_date );
		if ( ret ) return 1;
	}else{
		sprintf(acErrMsg,"传人的日期类型错误,请检查![%s]",sts);
		WRITEMSG
		return 1;
	}		
	
	/* 取提示信息存放目录 */
	if ( getenv("MAILDIR")!= NULL  )/* LOG目录 */
	{
		strcpy( tmpstr, getenv("MAILDIR") );
		sprintf( comm , "mkdir %s 1>/dev/null 2>/dev/null",tmpstr );
		system( comm );
		
		MEMSET_DEBUG(tmpstr,0x00,sizeof(tmpstr));		
		MEMSET_DEBUG(comm,0x00,sizeof(comm));
			
		sprintf( tmpstr,"%s/%ld/",getenv("MAILDIR"),log_date );
		sprintf( comm , "mkdir %s 1>/dev/null 2>/dev/null",tmpstr );
		system( comm );		
	}else{
		strcpy(tmpstr,"/tmp");
	}
		
	strcat( tmpstr, "/" );	

	if ( log_date )
	{
		strcat( tmpstr, brno );	/* 文件名称 */
		sprintf( date_tmp, "%ld", log_date );
		strcat( tmpstr, date_tmp );		
		/*********
		strcat( tmpstr, ".log");
		*********/		
	}else{
		strcat(tmpstr,"eod_inf.log");
	}
		
	fp = fopen(tmpstr, "a"); /* 写文件 */
	
/*	sprintf(acErrMsg,"FILENAME,请检查![%s]",tmpstr);
	WRITEMSG
*/

	fprintf(fp,"%s|" , log_tmp );
	fprintf(fp, "\n");
        fclose(fp);
        return 0;
}	
/***************************************************************
5、pub_base_readfield 把字符串中按照指定分割符取到指定的字符串;
入口参数： tmp_buf 传入的字符串指针
           flag 分隔符

出口参数： bscr 下个分割符前的内容
		   tmp_buf 下个分割符后面的内容

注意：这个函数可以传出取到的分隔符中间内容，
也会改变传入的字符串指针的位置，指向下一个分隔符后第一位
***************************************************************/
int pub_base_readfield(char *bsrc, char *tmp_buf, char flag)
{
	int i = 0, j = 0;
	char temp_buff[1025];

	MEMSET_DEBUG(temp_buff, 0x0, sizeof(temp_buff));
	strcpy(temp_buff, tmp_buf);

	for( ; ; )
	{
		if ( temp_buff[j] == flag || temp_buff[j] == 0x0a || temp_buff[j] == 0)
		{
			break;
		}

		if (temp_buff[j] != ' ')
		{
			bsrc[i++] = temp_buff[j];
		}
		j++;
	}
	bsrc[i] = 0;
	if (temp_buff[j] == flag)
		strcpy(tmp_buf, &temp_buff[++j]);

	sprintf(acErrMsg, "--------------> [%s]", bsrc);
	WRITEMSG

	return 0;
}

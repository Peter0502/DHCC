/*************************************************************
* 文 件 名: rpt002.c
* 功能描述：日报
*
* 作    者: rob
* 完成日期: 2003年4月13日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "gl_mst_c.h"
#include "com_item_c.h"
#include "com_branch_c.h"


int line=0;
struct com_branch_c rggjgm;
struct gl_mst_c rzz,ozz,hzz;
struct com_item_c rkm,okm;
char vjgbm[10];
char vzqqx[2];
long vgsrq;
long vxtrq;
char vrkcs[21];
char jgtj[100];
char sentence[1024];
int page_lines[100];

char idno1[20];
char idno2[20];
char ui_brno1[2];

FILE *fp;
int cpzs=0;
int vsjly=0;
int prtzero=0;  /*余额、发生额为零打否 1打 0不*/

int bbt;


int page=0;	
int get_ratio_rjb( int bli,int bll, char str[100], int rpt_code );
int vxh;
char code[21];
long syrq1,syrq2;
long byrq1,byrq2;

rpt075()
{
	int ret = 0;
	struct com_sys_parm_c com_sys_parm_c;

    char brno[6];
	char fullname[100];
	int rmmode=0,opncnt=0;
	int vd=0,vm=0,vy=0;
	int i=0,k,h,n;
	char tmpstr[10];
	long xmrq,ymrq,jmrq,bnmrq,nmrq;
	int gqbz=0;

	memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();

    /* 取前一天日期 */
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	if (ret != 0)
	{
		sprintf(acErrMsg,"取系统参数表错误!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	strcpy( vjgbm,"61000" );

	vsjly=0;
    g_pub_tx.tx_date = com_sys_parm_c.lst_date;
	vxtrq=com_sys_parm_c.sys_date;
	vgsrq=com_sys_parm_c.lst_date;

	byrq2=vgsrq/100;
	syrq2=vgsrq/100-1;

	sprintf(acErrMsg,"输入日期[%d] 归属日期[%d]\n",vxtrq,vgsrq);
	WRITEMSG

	strcpy( idno1,"xjszybb" );
	strcpy( idno2,"BMxjszybb" );
	
	pub_rpt_rmfile( vjgbm,idno2,0 );

	if( pub_m_xjszybb( g_pub_tx.tx_date/100 ) )
			goto ErrExit;

	if( Make_xjybb( brno ) )
		goto ErrExit;


OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成xjybb成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成xjybb失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}


int Make_xjybb( )
{
	
	int first=0;
	int flag = 0;
	double tmpdbl;
	int ret;
	char new_cur_no[3],old_cur_no[3];

	if( Make_yeb_sub("KK") ) goto ErrExit;

GoodExit:
	return 0;
ErrExit:
	return 1;
}
int Make_yeb_sub( char vrec[3] )
{
	char vpfm[21];
	int rpt_code;
	int opncnt=0;
	
	strcpy( vpfm,idno1 );


	pub_rpt_openfile( &fp,vjgbm,idno2 );

	rpt_code=0002;


	if( vrec[0]=='H' && vrec[1]=='Y' )
		fprintf( fp,"\n\f\n" );
	else
	{
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,rpt_code,vrec,&opncnt,
				get_ratio_rjb) ) 
			goto ErrExit;
	}

	fclose(fp);
GoodExit:
	return 0;
ErrExit:
	return 1;
}
int get_ratio_rjb( bli,bll,str ,rpt_code)
 int bli,bll,rpt_code;
 char str[100];
{
	char vhm[101];
	char vstr[101];
	char fmt[20];
	int i=0;
	int ret;
	double vje=0.0,vje1=0.0;

	memset( str,0,sizeof(str) );

	switch( bli )
	{
		case 'S':
			memset( str,' ',bll );
			sprintf( fmt,"%%-%ds",bll );
			sprintf( str,fmt,jgtj );
			break;
		case 'T':
			ret = Com_branch_Sel(g_pub_tx.reply,&rggjgm,"br_no='%s'",
					vjgbm);
 			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg,"SELECT COM_BRANCH ERROR !! [%d]",ret);
				WRITEMSG
				sprintf(acErrMsg,"br_no=[%s]",vjgbm);
				WRITEMSG
				goto ErrExit;
			}
			else if (ret == 100)
			{
				strcpy(rggjgm.br_name,"");
			}

			memset( str,' ',bll );
			ldchar( rggjgm.br_name,strlen(rggjgm.br_name),rggjgm.br_name );
			i=strlen(rggjgm.br_name);
			strncpy( str,rggjgm.br_name,i );
			if( strncmp(vjgbm,"00000",5) )
			{
			strncpy( str+i,"(",1 );
			i++;
			strncpy( str+i,vjgbm,5 );
			i=i+5;
			strncpy( str+i,")",1 );
			}
			break;
		case 'X':
			sprintf( fmt,"%%0%dd",bll );
			sprintf( str,fmt,vgsrq/10000 );
			break;
		case 'W':
			sprintf( fmt,"%%0%dd",bll );
			sprintf( str,fmt,vgsrq%10000/100 );
			break;
		case 'V':
			sprintf( fmt,"%%0%dd",bll );
			sprintf( str,fmt,vgsrq%100 );
			break;
		case 'Z':
			sprintf( fmt,"%%0%dd",bll );
			sprintf( str,fmt,vgsrq );
			break;
		case 'U':
			sprintf( fmt,"%%0%dd",bll );
			sprintf( str,fmt,page );
			break;

		case 'A':
			if( strlen(rzz.acc_hrt)>=3 )
			{
				sprintf( fmt,"%%-%ds",bll );
				ldchar( rzz.acc_hrt,strlen(rzz.acc_hrt),rzz.acc_hrt );
				sprintf( str,fmt,rzz.acc_hrt );
			}
			else
			{
				memset( str,' ',bll );
			}
			break;
		case 'C':
			if( rzz.ldd_bal==0.00 )
				memset( str,' ',bll );
			else
			{
				sprintf( vstr,"%.2lf",rzz.ldd_bal );
				pub_rpt_flttomon( vstr,vhm );
				sprintf( fmt,"%%%ds",bll );
				sprintf( str,fmt,vhm );
			}
			break;
		case 'D':
			if( rzz.lcd_bal==0.00 )
				memset( str,' ',bll );
			else
			{
				sprintf( vstr,"%.2lf",rzz.lcd_bal );
				pub_rpt_flttomon( vstr,vhm );
				sprintf( fmt,"%%%ds",bll );
				sprintf( str,fmt,vhm );
			}
			break;
		case 'E':
			if( rzz.rdd_amt==0.00 )
				memset( str,' ',bll );
			else
			{
				sprintf( vstr,"%.2lf",rzz.rdd_amt );
				pub_rpt_flttomon( vstr,vhm );
				sprintf( fmt,"%%%ds",bll );
				sprintf( str,fmt,vhm );
			}
			break;
		case 'F':
			if( rzz.rcd_amt==0.00 )
				memset( str,' ',bll );
			else
			{
				sprintf( vstr,"%.2lf",rzz.rcd_amt );
				pub_rpt_flttomon( vstr,vhm );
				sprintf( fmt,"%%%ds",bll );
				sprintf( str,fmt,vhm );
			}
			break;
		case 'G':
			if( rzz.dr_bal==0.00 )
				memset( str,' ',bll );
			else
			{
				sprintf( vstr,"%.2lf",rzz.dr_bal );
				pub_rpt_flttomon( vstr,vhm );
				sprintf( fmt,"%%%ds",bll );
				sprintf( str,fmt,vhm );
			}
			break;
		case 'H':
			if( rzz.cr_bal==0.00 )
				memset( str,' ',bll );
			else
			{
				sprintf( vstr,"%.2lf",rzz.cr_bal );
				pub_rpt_flttomon( vstr,vhm );
				sprintf( fmt,"%%%ds",bll );
				sprintf( str,fmt,vhm );
			}
			break;
		case 'a':
			vxh++;
			get_code();
			sql_sum_double ( "target_statistic","amt",&vje,"target_no='%s' and tx_date=%d ", code,syrq2 );
			if( vje==0.00 )
				memset( str,' ',bll );
			else
			{
				sprintf( vstr,"%.0lf",pub_base_PubDround_1(vje/1000000)*100 );
				sprintf( fmt,"%%%ds",bll );
				sprintf( str,fmt,vstr );
			}
			break;
		case 'b':
			get_code();
			sql_sum_double ( "target_statistic","amt",&vje,"target_no='%s' and tx_date=%d ", code,byrq2 );

vtcp_log("VVVJE[%d] [%.2lf] [%s][%d]",vxh,vje,code,byrq2);
			if( vje==0.00 )
				memset( str,' ',bll );
			else
			{
				sprintf( vstr,"%.0lf",pub_base_PubDround_1(vje/1000000)*100 );
				sprintf( fmt,"%%%ds",bll );
				sprintf( str,fmt,vstr );
vtcp_log("VVV [%s]",str);
			}
			break;

		default :
			memset( str,' ',bll );
			break;
	}

GoodExit:
	return 0;
ErrExit:
	return 1;
}
int get_code( )
{
			switch ( vxh )
			{
				case 1:
					strcpy( code,"121110000" );
					break;
				case 2:
					strcpy( code,"121120000" );
					break;
				case 3:
					strcpy( code,"121130000" );
					break;
				case 4:
					strcpy( code,"121140000" );
					break;

				case 5:
					strcpy( code,"121150000" );
					break;
				case 6:
					strcpy( code,"121160000" );
					break;
				case 7:
					strcpy( code,"121170000" );
					break;
				case 8:
					strcpy( code,"121180000" );
					break;

				case 9:
					strcpy( code,"121190000" );
					break;
				case 10:
					strcpy( code,"1211A0000" );
					break;
				case 11:
					strcpy( code,"1211A1000" );
					break;
				case 12:
					strcpy( code,"121100000" );
					break;

				case 13:
					strcpy( code,"121200000" );
					break;
				case 14:
					strcpy( code,"121300000" );
					break;
				case 15:
					strcpy( code,"121400000" );
					break;
				case 16:
					strcpy( code,"121500000" );
					break;

				case 17:
					strcpy( code,"121600000" );
					break;
				case 18:
					strcpy( code,"121700000" );
					break;
				case 19:
					strcpy( code,"121000000" );
					break;

				case 20:
					strcpy( code,"122110000" );
					break;
				case 21:
					strcpy( code,"122111000" );
					break;
				case 22:
					strcpy( code,"122112000" );
					break;
				case 23:
					strcpy( code,"122113000" );
					break;
				case 24:
					strcpy( code,"122114000" );
					break;

				case 25:
					strcpy( code,"12212Q000" );
					break;
				case 26:
					strcpy( code,"122120000" );
					break;
				case 27:
					strcpy( code,"1221B0000" );
					break;
				case 28:
					strcpy( code,"122130000" );
					break;

				case 29:
					strcpy( code,"122140000" );
					break;
				case 30:
					strcpy( code,"122150000" );
					break;
				case 31:
					strcpy( code,"122160000" );
					break;
				case 32:
					strcpy( code,"122170000" );
					break;

				case 33:
					strcpy( code,"122180000" );
					break;
				case 34:
					strcpy( code,"122190000" );
					break;
				case 35:
					strcpy( code,"1221A0000" );
					break;
				case 36:
					strcpy( code,"1221A1000" );
					break;

				case 37:
					strcpy( code,"122100000" );
					break;
				case 38:
					strcpy( code,"122200000" );
					break;
				case 39:
					strcpy( code,"122300000" );
					break;
				case 40:
					strcpy( code,"122400000" );
					break;

				case 41:
					strcpy( code,"122500000" );
					break;
				case 42:
					strcpy( code,"122600000" );
					break;
				case 43:
					strcpy( code,"122700000" );
					break;
				case 44:
					strcpy( code,"122000000" );
					break;

				case 45:
					strcpy( code,"123100000" );
					break;
				case 46:
					strcpy( code,"123200000" );
					break;
				case 47:
					strcpy( code,"123H00000" );
					break;
				case 48:
					strcpy( code,"000000000" );
					break;
				default:
					strcpy( code,"" );
					break;
			}
}

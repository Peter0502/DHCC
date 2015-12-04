/*************************************************************
* �� �� ��: rpt090.c
* ������������Աҵ����ͳ�Ʊ���
*
* ��    ��: dadary
* �������: 2006��12��4��
*
* ע����ʽ�ļ�ΪRPT090.tbl
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "com_branch_c.h"
#include "trace_log_rz_c.h"
#include "com_tel_c.h"

static int page,total;
static struct com_sys_parm_c sComSysParm;
static struct trace_log_rz_c s_trace_log_rz;
static struct com_tel_c	     s_com_tel;


char    pre_tel[5];
long	pre_trace_no = 0;
long	pre_tx_date = 0;


int    i_tx_num = 0;/**ĳһ����Ա���׵ı���**/


int get_ratio_mbfesd(int bli,int bll,char str[100],int prt_code,char mygsm[3]);
rpt090()
{
	int line, opncnt;
	int hy_line;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	
	
	char cWherelist[512];
	
	
	
	FILE *fp; 
	int    iRecordNum = 0;/***���������ļ�¼������,�������¼����������Ҳʱ���������һҳ�ϴ�ӡ���������**/
	int    iCycNum    = 0;/***ѭ���ߵĴ���**/
	
	int 	ret = 0;
	
	
	int	i_beg_day	=	0;/*�µĵ�һ��**/
	int	i_end_day	=	0;/*�µ����һ��Ҳ�����ۺ�ҵ��ı���***/
	
	int	i_index_day	=	0;/*forѭ����day****/
	
	
	
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );


	long   beg_date = 0;/**��ѯ�Ŀ�ʼ����**/
	long   end_date = 0;/**��ѯ�Ľ�������**/



	memset(pre_tel,0x00,sizeof(pre_tel));

	memset(cWherelist,0x00,sizeof(cWherelist));


	


	vtcp_log("[%s][%d]��ʼ��ӡ��Աҵ����!\n",__FILE__,__LINE__);
	
    	g_reply_int = sql_begin(); /*������*/
    	if( g_reply_int != 0 )
    	{
    	    sprintf( acErrMsg, "BEGIN WORK error!!!" );
    	    WRITEMSG
    	    goto ErrExit;
    	}
    	
    	/**------- ��ʼ��ȫ�ֱ��� --------**/
    	pub_base_sysinit();
    
	strcpy( fname, "RPT090" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"ɾ���������ҵ���嵥����!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	g_reply_int = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "sys_date=%ld" , g_pub_tx.tx_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}

	/*ȷ����ʼ����*****/
	beg_date = sComSysParm.lst_date/100 *100 +1;
	end_date = sComSysParm.lst_date;
	
	i_beg_day	= 1;
	i_end_day	=sComSysParm.lst_date%100;


	vtcp_log("[%s][%d]���潫��ӡ����%l]--[%l]ʱ����չ�Ա��ҵ����!\n",__FILE__,__LINE__,beg_date,end_date);



	strcpy(vpfm, "RPT090");
	
	
	/*ret = Com_tel_Dec_Sel(g_pub_tx.reply,"csts!='4' and substr(tel,3,2)!='99' and br_no in(select br_no from com_branch where br_type not in('0','6') and wrk_sts!='*') order by br_no,tel"); lwb 20150525  ���ӷ���  */
	ret = Com_tel_Dec_Sel(g_pub_tx.reply,"csts!='4' and substr(tel,3,2)!='99' and br_no in(select br_no from com_branch where br_type not in('0','6','7') and wrk_sts!='*') order by br_no,tel");
	if(ret)
	{
		vtcp_log("[%s][%d]��ѯ��Ա�Ŵ���!\n",__FILE__,__LINE__);
		goto OkExit;
	}
	while(1)
	{
		ret = Com_tel_Fet_Sel(&s_com_tel,g_pub_tx.reply);
		
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]��com_tel���ȡ��¼����!ret====[%d]\n",__FILE__,__LINE__,ret);
			
			goto OkExit;
		}
		
		if(ret==100)/***���������*****/
		{
			vtcp_log("[%s][%d]���������еĻ���!\n",__FILE__,__LINE__);
			break;
		}
		
		
		i_tx_num = 0;	/**��Ա�仯���ı����Աҵ������***/
		
		
		for(i_index_day=i_beg_day;i_index_day<=i_end_day;i_index_day++)
		{
			memset(cWherelist,0x00,sizeof(cWherelist));
			sprintf(cWherelist," ( select distinct(trace_no)  from trace_log_rz where tel='%s' and tx_date= %ld  )",s_com_tel.tel,sComSysParm.lst_date/100*100+i_index_day);
			i_tx_num = i_tx_num + sql_count(cWherelist,"1=1");
			
			vtcp_log("[%s][%d]��Ա[%s] ��%d]��  ҵ����=[%d]��\n",__FILE__,__LINE__,s_com_tel.tel,i_index_day,i_tx_num);
		}
		
		vtcp_log("[%s][%d]��Ա[%s] ��%d]��  ��ҵ����=[%d]��\n",__FILE__,__LINE__,s_com_tel.tel,sComSysParm.lst_date%10000/100,i_tx_num);
	
		/*�����ű�,��һ���ļ�*/
		if( strcmp( s_com_tel.br_no, pre_br_no ) )
		{
			/*���ǵ�һ������,���ϸ������Ľ�β*/
			if( strlen( pre_br_no ) )
			{
				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","TT",&opncnt,get_ratio_mbfesd);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				fclose( fp );
			}

			g_reply_int = pub_rpt_openfile( &fp, s_com_tel.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		
			hy_line = 0;
			page = 1;

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_mbfesd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( pre_br_no, s_com_tel.br_no );
		}
		
		
		
		g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "KK", &opncnt, get_ratio_mbfesd );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		
		i_tx_num = 0;
		
		/****Ҫ�ǹ�Ա�ű仯�ˣ�����һ���µļ�¼***/
		/**
		if( strcmp(s_com_tel.tel,pre_tel) && strlen(pre_tel)!=0)
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "KK", &opncnt, get_ratio_mbfesd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			
			i_tx_num = 0;
			
			memcpy(pre_tel,s_com_tel.tel,sizeof(s_com_tel.tel)-1);
		}
		****/
		
	} 
	
	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
	
	Trace_log_rz_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɴ�������嵥�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"���ɴ��δ��������嵥ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_mbfesd( bli,bll,str,rpt_code,mygsm )
int bli,bll;
char str[100];
int  rpt_code;
char mygsm[3];
{
	char vstr[101]; 
	char fmt[10]; 
	char cDate[9];
	char cTx_Desc[17];

	memset(cTx_Desc,0x00,sizeof(cTx_Desc));
	memset(vstr,	0x00,sizeof(vstr));
	memset(cDate,	0x00,sizeof(cDate));
	memset(fmt,	0x00,sizeof(fmt));

	struct com_branch_c sComBranch;
	memset(&sComBranch,0x00,sizeof(sComBranch));
	
	
	vtcp_log("[%s][%d]mygsm===[%s],bli=[%c],bll=[%d]\n",__FILE__,__LINE__,mygsm,(char)bli,bll);
	
	switch( bli ) 
	{
		case 'B':
			vtcp_log("[%s][%d]��ʼ��ӡ--��������!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'",s_com_tel.br_no );
			if( g_reply_int )
			{
				sprintf(acErrMsg, "SELECT com_branch error![%d]", g_reply_int );
				WRITEMSG
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s��%c%s��", sComBranch.br_name, ' ',sComBranch.br_no );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'E': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!sComSysParm.lst_date=[%ld]\n",__FILE__,__LINE__,sComSysParm.lst_date);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'F': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!sComSysParm.lst_date[%ld]\n",__FILE__,__LINE__,sComSysParm.lst_date);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'G': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��![%ld]\n",__FILE__,__LINE__,sComSysParm.lst_date);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break;
		case 'U':
			vtcp_log("[%s][%d]��ʼ��ӡ--��Ա��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str,fmt,s_com_tel.tel);
			 
			 break;
		case 'V':
			vtcp_log("[%s][%d]��ʼ��ӡ--��Ա����!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str,fmt,s_com_tel.name);
			 break;
		case 'W': 
			vtcp_log("[%s][%d]��ʼ��ӡ--ҵ����!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str,fmt,i_tx_num);
			/*strcpy( str, cDate ); */
			break;
		case 'X': 
			vtcp_log("[%s][%d]��ʼ��ӡ--����!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str,fmt,sComSysParm.lst_date/100);
			/*strcpy( str, cDate ); */
			break;
		
		default : 
			memset( str,' ',bll ); 
			
	}
	vtcp_log("[%s][%d]501501501-------[%s][%d],mygsm===[%s]\n",__FILE__,__LINE__,str,bll,mygsm);
	vtcp_log("[%s][%d]ֵ[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}



 

/*************************************************************
* �� �� ��: rpt508.c
* ������������ӡ������ɸ�ʽ
*
* ��    ��: dadary
* �������: 2006��10��18��
*
* ע����ʽ�ļ�Ϊrpt508.tbl
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
#include "hv_fzf_c.h"

static int page,total;
static struct hv_fzf_c	hv_fzf;
static struct com_sys_parm_c sComSysParm;
static struct tx_def_c sTxDef;
static int hy_line = 0;

int get_ratio_mbfesd(int bli,int bll,char str[100]);
char *cGetHvCmnoDes(char *cmtno,char *desc);
char *cGetHvzfStat(char stat,char *desc);
void cDeal_str(char *str,int lenth);
char * cDeal_content(char *buf,char ch,char *str);
void  myfun(char *source,char *target);
/***ע����:�����Ǵ�ӡһ������,ÿһҳ��4����*****/
rpt508()
{
	
	int line, opncnt;
	 hy_line = 0;    /**��¼һ�������Ĳ�ѯ�ı��ĸ���**/
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	int    iRecordNum = 0;/***���������ļ�¼������,�������¼����������Ҳʱ���������һҳ�ϴ�ӡ���������**/
	int    iCycNum    = 0;/***ѭ���ߵĴ���**/

	total			  = 0;/***ĳ�Ի��������ɸ�ʽ�ܱ���**/
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );


	vtcp_log("[%s][%d]��ʼ��ӡ������ɸ�ʽ!\n",__FILE__,__LINE__);
    g_reply_int = sql_begin(); /*������*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
    
	strcpy( fname, "RPT508" );
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

	strcpy(vpfm, "rpt508");
	vtcp_log("[%s][%d]vpfm==[%s]\n",__FILE__,__LINE__,vpfm);

	g_reply_int = Hv_fzf_Dec_Sel(g_pub_tx.reply, \
		"wt_date=%ld   and  cmtno ='303'  and otype ='1' ORDER BY br_no, cmtno,orderno,hv_fzf_sts  ",sComSysParm.lst_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE Hv_fzf  CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	} 

	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	while(1) 
	{ 
		vtcp_log("[%s][%d]iCycNum=[%d]\n",__FILE__,__LINE__,iCycNum);
		g_reply_int = Hv_fzf_Fet_Sel( &hv_fzf, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH hv_fzf  CUR error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int == 100  )
		{
			if(iCycNum>0)
			{
				fclose(fp);
				break;
			}
			else
			{
				vtcp_log("[%s][%d]û���ҵ���¼!\n",__FILE__,__LINE__);
				goto OkExit;	
			}
		}
		
		/*�����ű�,��һ���ļ�*/
		if(strlen(hv_fzf.br_no)==0) memcpy(hv_fzf.br_no,QS_BR_NO,5);
		if( strcmp( hv_fzf.br_no, pre_br_no ) )
		{
			if(strlen(pre_br_no)) /**������뵽��������,���Ҳ��ǵ�һ�ν���,��Ҫ�ر�ǰ����ĸ��ļ�**/
				fclose(fp);
				
			hy_line  = 0 ; /**��������ű仯��,��������¼�ֶ���0****/
			vtcp_log("[%s][%d]fname==[%s]\n",__FILE__,__LINE__,fname);
			g_reply_int = pub_rpt_openfile( &fp, hv_fzf.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			
                         
			total = sql_count( "hv_fzf", \
				"br_no='%s' and wt_date=%ld and otype='2'   and cmtno='303' and 1=1 ", hv_fzf.br_no,hv_fzf.wt_date );

			strcpy( pre_br_no, hv_fzf.br_no );
		}

		hy_line ++;	/**���ĸ�����Ҫ����**/
		iCycNum ++; /**��¼whileѭ���Ĵ���**/
		
		/*��ʼ��ӡ��ѯ�ı�******/
		g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		/****������ʹ�ӡ���˱��ϵ�������****/
		
	
		/** if(hy_line>0 &&hy_line%6 ==0 ) �Ѿ���ӡ����������,��ҳ del by martin 20070519***/
		if(hy_line>0 &&hy_line%4 ==0)/**�Ѿ���ӡ����������,��ҳ***/
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbfesd);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		}

	} 
	
	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
	Hv_fzf_Clo_Sel();

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

int get_ratio_mbfesd( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	char cDate[9];
	char cTx_Desc[17];
	char cTel[7];
	
	memset(vstr,	0x00,	sizeof(vstr));
	memset(fmt,	0x00,	sizeof(fmt));
	memset(cDate,	0x00,	sizeof(cDate));
	memset(cTel,	0x00,	sizeof(cTel));
	memset(cTx_Desc,0x00,	sizeof(cTx_Desc));

	struct com_branch_c sComBranch;
	memset(&sComBranch,0x00,sizeof(sComBranch));
	memset(str,	0x00,	sizeof(str));
	
	
	switch( bli ) 
	{
		case 'C':
			vtcp_log("[%s][%d]��ʼ��ӡ--��������!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", hv_fzf.br_no );
			if( g_reply_int )
			{
				sprintf(acErrMsg, "SELECT com_branch error![%d]", g_reply_int );
				WRITEMSG
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s��%s��", sComBranch.br_name, sComBranch.br_no );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'A': 
			vtcp_log("[%s][%d]��ʼ��ӡ--���!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str,fmt,hy_line);
			break;
		case 'B': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��ѯ�к�!\n",__FILE__,__LINE__);
			memcpy(str,hv_fzf.or_br_no,bll);
			break;
		case 'D':
			vtcp_log("[%s][%d]��ʼ��ӡ--�����к�!\n",__FILE__,__LINE__);
			memcpy(str,hv_fzf.ac_br_no,bll);
			 break;
		case 'E': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'F': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'G': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'H':
			vtcp_log("[%s][%d]��ӡί������!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, hv_fzf.wt_date ); 
			break;
		case 'I':
			vtcp_log("[%s][%d]��ʼ��ӡ--�ܱ���!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, total  ); 
			 break;
		case 'J': 
			vtcp_log("[%s][%d]��ʼ��ӡ--�������!\n",__FILE__,__LINE__);
			memcpy(str,hv_fzf.orderno,bll);
			break;
		case 'O':
			vtcp_log("[%s][%d]��ʼ��ӡ--��ѯ����1!\n",__FILE__,__LINE__);
			memset(vstr,0x00,sizeof(vstr));
			cDeal_content(hv_fzf.req_content,'1',vstr);
			vtcp_log("funk!vstr==[%s]\n",vstr);
			memcpy(str,vstr,bll);
			 break;
		case 'P':
			vtcp_log("[%s][%d]��ʼ��ӡ--��ѯ����2!\n",__FILE__,__LINE__);
			memset(vstr,0x00,sizeof(vstr));
		        cDeal_content(hv_fzf.req_content,'2',vstr);
			memcpy(str,vstr,bll);
			 break;
		case 'Q':
			vtcp_log("[%s][%d]��ʼ��ӡ--��ѯ����3!\n",__FILE__,__LINE__);
			memset(vstr,0x00,sizeof(vstr));
		        cDeal_content(hv_fzf.req_content,'3',vstr);
		        vtcp_log("[%s][%d]��ѯ���ݵĵ�������vstr==[%s]\n",__FILE__,__LINE__,vstr);
			memcpy(str,vstr,bll);
			vtcp_log("[%s][%d]��ѯ���ݵĵ�������==[%s]\n",__FILE__,__LINE__,str);
			 break;
		case 'U':
			vtcp_log("[%s][%d]��ʼ��ӡ--������Դ!\n",__FILE__,__LINE__);
			if(hv_fzf.lw_ind[0]=='1')
				memcpy(str,"���з���",bll);
			else if(hv_fzf.lw_ind[0]=='2')
				memcpy(str,"���з���",bll);
			break;
		default : 
			memset( str,' ',bll ); 
			
	}
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]ֵ[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}



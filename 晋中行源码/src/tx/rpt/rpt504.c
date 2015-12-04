/*************************************************************
* �� �� ��: rpt504.c
* ������������ӡ����Ʊ
*
* ��    ��: dadary
* �������: 2006��10��18��
*
* ע����ʽ�ļ�Ϊrpt504.tbl
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
#include "hv_poinfo_c.h"

static int page,total;
static struct hv_poinfo_c	hv_poinfo;
static struct com_sys_parm_c sComSysParm;
static struct tx_def_c sTxDef;

int get_ratio_mbfesd(int bli,int bll,char str[100]);
/***ע����:�����Ǵ�ӡһ������,ÿһҳ��������*****/
rpt504()
{
	int line, opncnt;
	int hy_line;    /**��¼һ�������Ĳ�ѯ�ı��ĸ���**/
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	int    iRecordNum = 0;/***���������ļ�¼������,�������¼����������Ҳʱ���������һҳ�ϴ�ӡ���������**/
	int    iCycNum    = 0;/***ѭ���ߵĴ���**/
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );


	vtcp_log("[%s][%d]��ʼ��ӡ����ѯ!\n",__FILE__,__LINE__);
    g_reply_int = sql_begin(); /*������*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
    
	strcpy( fname, "RPT504" );
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

	strcpy(vpfm, "rpt504");

	g_reply_int = Hv_poinfo_Dec_Sel(g_pub_tx.reply, \
   			"l_tx_date=%ld   ORDER BY br_no, po_no,po_sts ",sComSysParm.lst_date );

	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE Hv_fzf  CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	} 

	while(1) 
	{ 
		g_reply_int = Hv_poinfo_Fet_Sel( &hv_poinfo, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH hv_poinfo  CUR error![%d]", g_reply_int );
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
		if( strcmp( hv_poinfo.br_no, pre_br_no ) )
		{
			if(strlen(pre_br_no)) /**������뵽��������,���Ҳ��ǵ�һ�ν���,��Ҫ�ر�ǰ����ĸ��ļ�**/
				fclose(fp);
				
			hy_line  = 0 ; /**��������ű仯��,��������¼�ֶ���0****/
			g_reply_int = pub_rpt_openfile( &fp, hv_poinfo.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcpy( pre_br_no, hv_poinfo.br_no );
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
		
	
		
		if(hy_line>0 &&hy_line%3 ==0)/**�Ѿ���ӡ����������,��ҳ***/
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbfesd);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			fprintf(fp,"\f");
		}

	} 
	
	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
	Hv_poinfo_Clo_Sel();

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

	memset(cTx_Desc,0x00,sizeof(cTx_Desc));

	struct com_branch_c sComBranch;
	
	switch( bli ) 
	{
		case 'C':
			vtcp_log("[%s][%d]��ʼ��ӡ--��������!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", hv_poinfo.br_no );
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
		case 'L':
			vtcp_log("[%s][%d]��ʼ��ӡ--ǩ���к�!\n",__FILE__,__LINE__);
			sprintf(str,"%8d",hv_poinfo.sign_date);
			 break;
		case 'U':
			vtcp_log("[%s][%d]��ʼ��ӡ--ǩ���к�!\n",__FILE__,__LINE__);
		         memcpy(str,hv_poinfo.pay_br_no,bll);
			 break;
		case 'R':
			vtcp_log("[%s][%d]��ʼ��ӡ--��Ʊ����!\n",__FILE__,__LINE__);
			memcpy(str,hv_poinfo.po_no,bll);
			break;
	        case 'I':
			vtcp_log("[%s][%d]��ʼ��ӡ--��Ʊ����!\n",__FILE__,__LINE__);
			if(hv_poinfo.po_ind[0]=='0')
				memcpy(str,"��ת�û�Ʊ",10);
			if(hv_poinfo.po_ind[0]=='1')
				memcpy(str,"����ת�û�Ʊ",10);
			if(hv_poinfo.po_ind[0]=='1')
				memcpy(str,"�ֽ��Ʊ",8);
			break;
		case 'S':
			vtcp_log("[%s][%d]��ʼ��ӡ--�����ʺ�!\n",__FILE__,__LINE__);
			
			memcpy(str,hv_poinfo.pay_ac_no,bll);
			
			break;
		case 'T':
			vtcp_log("[%s][%d]��ʼ��ӡ--��������!\n",__FILE__,__LINE__);
			
			memcpy(str,hv_poinfo.pay_name,bll);
			cDeal_str(str, bll);
			break;
		case 'Q': 
			vtcp_log("[%s][%d]��ʼ��ӡ--Ʊ����!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, hv_poinfo.po_bal );/*�ô����ŵĽ��*/
			strcpy( str, vstr ); 
			break; 
		case 'P': 
			vtcp_log("[%s][%d]��ʼ��ӡ--ʵ�ʽ�����!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, hv_poinfo.cash_amt );/*�ô����ŵĽ��*/
			strcpy( str, vstr ); 
			break; 
		case 'O': 
			vtcp_log("[%s][%d]��ʼ��ӡ--������!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, hv_poinfo.rem_amt );/*�ô����ŵĽ��*/
			strcpy( str, vstr ); 
			break; 
		case 'V':
			vtcp_log("[%s][%d]��ʼ��ӡ--�����к�!\n",__FILE__,__LINE__);
			memcpy(str,hv_poinfo.cash_br_no,bll);
			break;
		case 'X':
			vtcp_log("[%s][%d]��ʼ��ӡ--�տ��ʺ�!\n",__FILE__,__LINE__);
			memcpy(str,hv_poinfo.cash_ac_no,bll);
			cDeal_str(str,bll);
			break;
		case 'W':
			vtcp_log("[%s][%d]��ʼ��ӡ--�տ�������!\n",__FILE__,__LINE__);
			memcpy(str,hv_poinfo.cash_name,bll);
			cDeal_str(str,bll);
			break;
		case 'M':
			vtcp_log("[%s][%d]��ʼ��ӡ--��Ʊ״̬!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetHpStat(hv_poinfo.po_sts[0],cTx_Desc);
			
			memcpy(str,cTx_Desc,bll);
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
		case 'D': 
			vtcp_log("[%s][%d]��ʼ��ӡ--ҳ��!\n",__FILE__,__LINE__);
			sprintf( str, "%02ld", page ); 
			break; 
		case 'Z': 
			vtcp_log("[%s][%d]��ʼ��ӡ--�ܼ�ҳ��!\n",__FILE__,__LINE__);
			sprintf( str, "%02ld", total ); 
			break; 
		case 'K': 
			vtcp_log("[%s][%d]��ʼ��ӡ--�Ҹ�����!\n",__FILE__,__LINE__);
			cGet_br_name(hv_poinfo.cash_br_no,vstr);
			vtcp_log("[%s][%d]vstr==[%s]\n",__FILE__,__LINE__,vstr);
			cDeal_str(vstr, bll);
			memcpy(str,vstr,bll);
			break; 
		case 'Y': 
			vtcp_log("[%s][%d]��ʼ��ӡ--ǩ������!\n",__FILE__,__LINE__);
			cGet_br_name(hv_poinfo.pay_br_no,vstr);
			vtcp_log("[%s][%d]vstr==[%s]\n",__FILE__,__LINE__,vstr);
			cDeal_str(vstr, bll);
			memcpy(str,vstr,bll);
			break; 
		case 'J':
			vtcp_log("[%s][%d]��ʼ��ӡ--������Դ!\n",__FILE__,__LINE__);
			if(hv_poinfo.bank_type[0]=='0')
				memcpy(str,"���л�Ʊ",bll);
			else if(hv_poinfo.bank_type[0]=='1')
				memcpy(str,"���л�Ʊ",bll);
			break;
		default : 
			memset( str,' ',bll ); 
			
	}
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]ֵ[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}



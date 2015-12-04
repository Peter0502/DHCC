/*************************************************************
* �� �� ��: rpt666.c
* ����������֧���ʽ�ͷ���
* ��    ��: 
* �������: 2006��12��05��
* ע����ʽ�ļ�Ϊrpt555.tbl
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define  EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "com_branch_c.h"
#include "gl_sub_c.h"

static struct gl_sub_c gl_sub;
static int page,total;
static struct com_sys_parm_c sComSysParm;
static struct com_branch_c sCom_branch;
static struct tx_def_c sTxDef;

int get_ratio_mbfesd(int bli,int bll,char str[100]);
void cDeal_str(char *str,int lenth);
char * cDeal_content(char *buf,char ch,char *str);
void  myfun(char *source,char *target);
double tmp_amt1=0.00,tmp_amt2=0.00,tmp_amt3=0.00,tmp_amt4=0.00;
        double tmp_amt5=0.00,tmp_amt63=0.00,tmp_amt7=0.00,tmp_amt8=0.00;
        double tmp_amt9=0.00,tmp_amt10=0.00,tmp_amt11=0.00,tmp_amt12=0.00;
        int ret=0,ret1=0,ret2=0,ret3=0,ret4=0,ret5=0,ret6=0;
        int ret7=0,ret8=0,ret9=0,ret10=0,ret11=0,ret12=0;
/***ע����:�����Ǵ�ӡһ������,ÿһҳ��������*****/
rpt666()
{
	int  line, opncnt;
	int  hy_line;    /**��¼һ�������Ĳ�ѯ�ı��ĸ���**/
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	int  iRecordNum = 0; /**  ���������ļ�¼������,�������¼����������Ҳʱ���������һҳ�ϴ�ӡ���������**/
	int  iCycNum    = 0;  /** ѭ���ߵĴ���**/
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );
	
 vtcp_log("[%s][%d]��ʼ��ӡ֧���ʽ�ͷ��!\n",__FILE__,__LINE__);
    g_reply_int = sql_begin(); /*������*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
    
	strcpy( fname, "RPT666" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"ɾ��֧���ʽ�ͷ������!!");
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

	strcpy(vpfm, "rpt666");
	
	
	ret = Com_branch_Dec_Sel(g_pub_tx.reply,"1=1 order by br_no");
	if(ret)
	{
		vtcp_log("[%s][%d]����com_branch����!\n",__FILE__,__LINE__);
		sprintf(acErrMsg, "DECLARE COM_BRANCH ERROR !! [%d]", ret);
                WRITEMSG
                goto ErrExit;
	}
	while(1)
	{
		memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
                ret = Com_branch_Fet_Sel(&sCom_branch, g_pub_tx.reply);
                vtcp_log("wyb[%s]",sCom_branch.br_no);
                if (ret == 100)
                   {
                      break;
                   }
                else if ( ret )
                        {
                           sprintf(acErrMsg, "FETCH COM_BRANCH ERROR !! [%d]", ret);
                           WRITEMSG
                           goto ErrExit;
                         }
          pub_base_strpack(sCom_branch.br_no);
          tmp_amt1=0.00;
          ret1 = sql_sum_double("gl_sub","cr_bal",&tmp_amt1,"up_acc_hrt in ('20100','20200','20500','21100','21500','21700','22100','24300','25100') and br_no='%s' order by br_no",sCom_branch.br_no);
         vtcp_log("wyb[%f]",tmp_amt1);
	if (ret1)
	{
		sprintf(acErrMsg, "ͳ�Ʋ�ѯ������!! [%d]", ret1);
		WRITEMSG
		goto ErrExit;
	}

		
		/*�����ű�,��һ���ļ�*/
		if( strcmp( sCom_branch.br_no, pre_br_no ) )
		{
			if(strlen(pre_br_no)) /**������뵽��������,���Ҳ��ǵ�һ�ν���,��Ҫ�ر�ǰ����ĸ��ļ�**/
				fclose(fp);
				
			hy_line  = 0 ; /**��������ű仯��,��������¼�ֶ���0****/
			g_reply_int = pub_rpt_openfile( &fp, sCom_branch.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcpy( pre_br_no, sCom_branch.br_no );
		

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
		}
	}
}
	     Com_branch_Clo_Sel();
	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
	Hv_fzf_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����֧���ʽ�ͷ���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"����֧���ʽ�ͷ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_mbfesd( bli,bll,str )
int bli,bll;
char str[100];
{ char amp[40];
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
memset(str,	0x00,	sizeof(str));
	
	
	switch( bli ) 
	{
		case 'C':
				pub_base_strpack( sCom_branch.br_no );
			memcpy(str,sCom_branch.br_no,bll);
			vtcp_log("wyb1983");
			break;
		case 'A': 
			vtcp_log("wyb121[%2f]",tmp_amt1);
		
			sprintf(str,"%2f",tmp_amt1 );/*�ô����ŵĽ��*/
		
			pub_rpt_flttomon(amp,amp);
			vtcp_log("wyb1313[%f]",str);
			break; 
	
		default : 
			memset( str,' ',bll ); 
			
	}
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]ֵ[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}


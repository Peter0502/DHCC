/*************************************************************
* �� �� ��: rpt064.c
* �������������д���������ִ�Ʊ��ӡ
*
* ��    ��: dadary
* �������: 2006��11��27��
*
* ע����ʽ�ļ�ΪRPT063.tbl
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define TABLE_RECORD 3
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "trace_log_bk_c.h"
#include "com_branch_c.h"
#include "ln_mst_hst_c.h"
#include "com_sys_parm_c.h"
#include "ln_parm_c.h"
#include "com_item_c.h"
#include "ln_mst_c.h"
#include "dc_acc_rel_c.h"
#include "trace_log_bk_c.h"
#include "dc_log_bk_c.h"
#include "trace_log_c.h"
#include "dc_log_c.h"
#include "tx_def_c.h"


struct trace_log_bk_c 	s_trace_log_bk;
struct dc_log_bk_c	s_dc_log_bk;
static struct com_sys_parm_c sComSysParm;
struct tx_def_c		s_tx_def;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
char cTx_name[21];

static char acc_hrt[8];
static char form_acc_hrt[8];

int get_ratio_mbfesd(int bli,int bll,char str[100]);
char *cGetHvCmnoDes(char *cmtno,char *desc);
char *cGetHvzfStat(char stat,char *desc);
void cDeal_str(char *str,int lenth);
char * cDeal_content(char *buf,char ch,char *str);
void  myfun(char *source,char *target);
char * cTurnMoneyUpper(char *money);
int pub_ln_get_form_acc_hrt( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , char *data_code , char *acc_hrt );

/****����һ������,���и��������������Ĵ�Ʊ�������Ľ�����****/
static char cTx_code[100][5]={"G088","G083"};

/****�������tx_code,�жϸ�tx_code�Ƿ�cTx_code������***
****tx_code�����з��� >1,�������з���0              ***
****�����ı����tx_code�ǲ���cTx_code�е�           ***/

int iExsit(char *tx_code)
{
	int ret = 0;
	int rtn = 1;
	int index = 0;
	for(index =0;index<100;index++)
	{
		if(memcmp(tx_code,cTx_code[index],4))
		{
			rtn = 0 ;
			ret = ret +rtn;
		}
		else
		{
			rtn = 1;
			ret = ret +rtn;
			break;
		}
	}
	return ret;
}
char cAcc_hrt[11][61];/***������Ŀ��**/
char dAcc_hrt[11][61];/***�跽��Ŀ��**/
void v_init_acc_hrt()
{
	int index = 0;
	for(index = 0;index<11;index++)
	{
		memset(cAcc_hrt[index],0x00,61);
		memset(dAcc_hrt[index],0x00,61);
	}
}

char cDc_code[61];
/***ע����:�����Ǵ�ӡһ������,ÿһҳ��4����*****/
rpt064()
{
	int flag=0;
	int ttlnum;
	int iRecordNum = 0 ; /**��־���ñ���ˮ�з�¼�ĸ���***/
	
	int iCr_num = 1;
	int iDr_num = 1;
	
	char amt_tmp[22];
	memset(amt_tmp,0x00,sizeof(amt_tmp));


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	long old_trace_no=0;
	char acc_hrt_tmp[8];
	char form_acc_hrt_tmp[8];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_trace_log_bk , 0x00 , sizeof(struct trace_log_bk_c));
	memset(acc_hrt,0x00,8);
	memset(fname,0x00,sizeof(fname));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(tmp_str,0x00,sizeof(tmp_str));
	memset(old_br_no,0x00,sizeof(old_br_no));
	memset(cTx_name,	0x00,	sizeof(cTx_name));
	
	
	memset(cDc_code,0x00,sizeof(cDc_code));
	memset(acc_hrt_tmp,0,sizeof(acc_hrt_tmp));
	memcpy(acc_hrt,"0000000",7);
	memset(form_acc_hrt_tmp,0,sizeof(form_acc_hrt_tmp));
	memcpy(form_acc_hrt,"0000000",7);
	
	
	strcpy( old_br_no , "*****" );
		
   
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
	ret  = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "1=1" );
	if( ret  ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", ret  ); 
		WRITEMSG 
		goto ErrExit; 
	}
	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	strcpy( filename , "RPT064");
	strcpy(vpfm, "RPT064");
	
	/**��ɲ�ѯ**/

	sprintf(wherelist," tx_date=%ld  and 1=1  ORDER BY tx_tx_br_no, trace_no, amt,trace_cnt",sComSysParm.lst_date) ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Dc_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	if(ret)
	{
		vtcp_log("[%s][%d]����Dc_log_bkʧ��!\n",__FILE__,__LINE__);
		goto ErrExit;
		ttlnum = 0;
	}
	
	/***��ʼ������***/
	v_init_acc_hrt();
	while(1)
	{
		ret=Dc_log_bk_Fet_Sel( &s_dc_log_bk, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"��ȡ��Ʒ�¼�쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		iRecordNum ++;
		
		/****ͨ��dc_log����ˮ������ȡ����trace_log�еļ�¼��ȡ��������*****/
		if(iRecordNum==1)/**û�б�Ҫÿ�ζ������������***/
		{
			ret = Trace_log_bk_Sel(g_pub_tx.reply,&s_trace_log_bk," tx_date=%ld and trace_no=%ld  and rownum=1",
					s_dc_log_bk.tx_date,s_dc_log_bk.trace_no);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]���ݿ�����쳣!ret=[%d]\n",__FILE__,__LINE__,ret);
				/*return ret;*/
			}
			else if (ret==100)
			{
				vtcp_log("[%s][%d]û���ҵ���Ӧ�ļ�¼!\n",__FILE__,__LINE__);
				iRecordNum = 0;
				continue;
			}
			ret = Tx_def_Sel(g_pub_tx.reply,&s_tx_def,"tx_code='%s' ",s_trace_log_bk.tx_code);
			if(ret)
			{
				vtcp_log("[%s][%d]��ѯ�������ƴ���!ret==[%d]\n",__FILE__,__LINE__,ret);
				/*return ret;*/
			}
		}
		
		/* �����任ʱ */
		vtcp_log("!!!!!!!!!�����Ƿ񻻻�����[%s][%s]\n",s_dc_log_bk.tx_tx_br_no , old_br_no);
		pub_base_strpack( s_dc_log_bk.tx_tx_br_no );
		if( strcmp( s_dc_log_bk.tx_tx_br_no , old_br_no ) )
		{
			vtcp_log("!!!!!!!!!!!!!��������!\n");
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/**���ǵ�һ������ ��������һ�������ر��ļ�***/
				fclose(fp);
			}
			
			strcpy( old_br_no , s_dc_log_bk.tx_tx_br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( s_dc_log_bk.tx_tx_br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , s_dc_log_bk.tx_tx_br_no , filename );
			ERR_DEAL
			
		}
		
		
		
		/*�еı����Ѿ������������г���,��û�б�Ҫ������������***/
		ret = iExsit(s_trace_log_bk.tx_code);
		if(ret) /**˵�������tx_code������Ҫ���ĵĴ�Ʊ***/
		{
			vtcp_log("[%s][%d],[%s]�ý����Ѿ�����Ҫ��ӡ��!\n",__FILE__,__LINE__,s_trace_log_bk.tx_code);
			continue;
		}
		
		
		/***��һ�θ���ֵ***/
		if(ttlnum==0)
			old_trace_no= s_dc_log_bk.trace_no;
		if( s_dc_log_bk.trace_no!=old_trace_no )	/*��ˮ�Ÿı�*/
		{
			ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","TT",&opncnt,get_ratio_mbfesd);
			ERR_DEAL
			v_init_acc_hrt();
			iRecordNum=1;
			iCr_num = 1;
			iDr_num = 1;
				
		}
		old_trace_no= s_dc_log_bk.trace_no;
		
		vtcp_log("[%s][%d]����һ����ˮ���еĵ�[%d]��!\n",__FILE__,__LINE__,iRecordNum);
		memset(cDc_code,0x00,sizeof(cDc_code));
		if(s_dc_log_bk.dc_ind[0]=='1')	/***��****/
		{
		    vtcp_log("[%s][%d]��ȡ���˽跽��Ŀ===[%s]\n",__FILE__,__LINE__,s_dc_log_bk.acc_hrt);
                    memset(amt_tmp,0x00,sizeof(amt_tmp));
                    sprintf( cDc_code,"%12s","��Ŀ���衳:");
                    strcat(cDc_code,s_dc_log_bk.acc_hrt);
                    strcat(cDc_code,"    ");
                    sprintf(amt_tmp, "%.2lf" , s_dc_log_bk.amt );
                    pub_rpt_flttomon( amt_tmp,amt_tmp );
                    strcat(cDc_code,amt_tmp);
                    vtcp_log("[%s][%d]���Ŀ=[%s]\n",__FILE__,__LINE__,cDc_code);

		}
		else if(s_dc_log_bk.dc_ind[0]=='2')/***��***/
		{
		    vtcp_log("[%s][%d]��ȡ���˴�����Ŀ===[%s]\n",__FILE__,__LINE__,s_dc_log_bk.acc_hrt);
                    memset(amt_tmp,0x00,sizeof(amt_tmp));
                    sprintf( cDc_code,"%12s","��Ŀ������:");
                    strcat(cDc_code,s_dc_log_bk.acc_hrt);
                    strcat(cDc_code,"    ");
                    sprintf(amt_tmp, "%.2lf" , s_dc_log_bk.amt );
                    pub_rpt_flttomon( amt_tmp,amt_tmp );
                    strcat(cDc_code,amt_tmp);
                    vtcp_log("[%s][%d]����Ŀ=[%s]\n",__FILE__,__LINE__,cDc_code);
		}
	
		
		if(iRecordNum==1)
		{
    		/* ��ӡ��Ϣ */
			ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
			ERR_DEAL
			line_file++;
		}
		ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","KK",&opncnt,get_ratio_mbfesd);
			ERR_DEAL
		
	
		if( line_file>TABLE_RECORD )	
		{
			line_file=0;
			page++;
			/*ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","TT",&opncnt,get_ratio_mbfesd);
			ERR_DEAL*/
			fprintf(fp,"\f");
		}
	
		ttlnum++;
	}		
	Dc_log_bk_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɴ������ˮ�嵥(����ת����)�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɴ������ˮ�嵥(����ת����)ʧ��!!!![%s]",g_pub_tx.reply);
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
		case 'A': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��������!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%-%ds", bll );
			sprintf(str,fmt,s_tx_def.tx_name);
			break;
		case 'B':
			vtcp_log("[%s][%d]��ʼ��ӡ--��������!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			ret = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", s_dc_log_bk.tx_tx_br_no );
			if( ret )
			{
				sprintf(acErrMsg, "SELECT com_branch error![%d]", ret );
				WRITEMSG
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s", sComBranch.br_name);
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'C': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��ˮ��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%0dld", bll);
			sprintf( str, fmt, s_dc_log_bk.trace_no ); 
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
		case 'T':
			vtcp_log("[%s][%d]��ӡ����Ա!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.tel ); 
			break;
		case 'S':
			vtcp_log("[%s][%d]��ӡ��ˮ״̬!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%ds", bll);
			if(s_dc_log_bk.sts[0]=='1')/**����**/
				sprintf(vstr,"%4s","����");
			else if(s_dc_log_bk.sts[0]=='0')
				sprintf(vstr,"%4s","����");
			sprintf( str, fmt, vstr ); 
			break;
		case 'X':
			vtcp_log("[%s][%d]��ʼ��ӡ�跽��Ŀ!\n",__FILE__,__LINE__);
			
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, cDc_code ); 
			
			break;
		case 'Y':
			vtcp_log("[%s][%d]��ʼ��ӡ������Ŀ!\n",__FILE__,__LINE__);
			break;
		default : 
			memset( str,' ',bll ); 
			
	}
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]ֵ[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}

/****���������ת��Ϊ��д****/
/***�ͽ����Ľ�����ַ���ʽ��,��12345.67,12345.00 ,12345***/
char * cTurnMoneyUpper(char *money)
{
    char cBuf[100];
    char cZPart[30];
    char cXPart[10];
    int  i_num = 0;
    int  i_unit= 0;
    char *_prt = NULL;
    int i =0;

    char unit[13][5] = {"��","Բ","ʰ","��","Ǫ","��","ʰ","��","Ǫ","��","ʰ��","����"};
    char numb[11][3] = {"","Ҽ","��","��","��","��","½","��","��","��","ʰ"};

    memset(cBuf,0x00,sizeof(cBuf));
    memset(cZPart,0x00,sizeof(cZPart));
    memset(cXPart,0x00,sizeof(cXPart));

    pub_base_strpack( money );
    printf("money===[%s]\n",money);

    _prt = strtok(money,".");
    printf("_prt==[%s]\n",_prt);
    if(_prt!=NULL)
    {
    	memcpy(cZPart,_prt,strlen(_prt));
    	memcpy(cXPart,money+strlen(_prt)+1,2);
    }
    else if(_prt==0x00) /***���û��.�Ļ�,˵������00*****/
    {
        memcpy(cZPart,money,strlen(money));
        memcpy(cXPart,"00",2);
    }

    printf("��������==[%s],С������=[%s]\n",cZPart,cXPart);

    /****��ʼ������������****/
    i = 0;
    printf("[%c] cZPart[i]===\n",cZPart[i]);
    while(cZPart[i]!=0x00)
    {
        i_num = cZPart[i]-'0';
        printf("cZPart[i]==[%c] i_num===[%d]\n",cZPart[i],i_num);
        if(i_num!=0 ||(strlen(cZPart)-i)==5)
        {
        	strcat(cBuf,numb[i_num]);
			strcat(cBuf,unit[strlen(cZPart)-i]);
		}
        i ++;
    }
    /*******��ʼ����С������***/
    if(!memcmp(cXPart,"00",2) && cBuf[0]!=0x00)
        strcat(cBuf,"��");
    else if(cBuf[0]==0x00 &&!memcmp(cXPart,"00",2))
    	memcpy(cBuf,"��Բ��",6);
    else
    {
        strcat(cBuf,"��");
        i_num = cXPart[0]-'0';
        if(i_num==0)
            	strcat(cBuf,"��");
        else
       		strcat(cBuf,numb[i_num]);
       		
       		
        i_num = cXPart[1]-'0';
        if(i_num==0)
            	strcat(cBuf,"��");
        else
        	strcat(cBuf,numb[i_num]);
    }

    printf("money==[%s]-->[%s]======\n",money,cBuf);

    return cBuf;


}
/**********************************************************************
* �� �� ����  pub_ln_get_acc_hrt
* �������ܣ�  �������ݱ���ȡ�����Ŀ��
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��07��29��
*
* ��    ����
*     ��  �룺   p_ln_mst            �������ļ��ṹ
*		 p_ln_parm 	     ��������ṹ
*                data_code           ���ݱ���
*      
*     ��  ��:    acc_hrt	     char(8)	 ��Ŀ��
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int pub_ln_get_form_acc_hrt( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , char *data_code , char *acc_hrt )
{
	int ret =0;
	char ln_dc_code[5];
	
	struct dc_acc_rel_c	dc_acc_rel_tmp;
	struct com_item_c com_item_tmp;
	memset( &com_item_tmp  , 0x00, sizeof(struct com_item_c));
	memset( &dc_acc_rel_tmp , 0x00 , sizeof( struct dc_acc_rel_c ) );
	
	if( p_ln_mst.ac_sts[0]=='2' )/* ����90���� */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.dc_code);	
	}
	if( p_ln_mst.ac_sts[0]=='5' )/* ����90���� */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.over_dc_code);	
	}
	if(p_ln_mst.ac_sts[0]=='3')/*����  */
	{
		TRACE
		strcpy(ln_dc_code,p_ln_parm.over90_dc_code);
		
	}
	if( p_ln_mst.ac_sts[0]=='4' )/*����  */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.sla_dc_code);	
	}
	
	ret = Dc_acc_rel_Sel(g_pub_tx.reply , &dc_acc_rel_tmp , "dc_code='%s' and data_code='%s' ",ln_dc_code, data_code );
	if ( ret )
	{
		sprintf(acErrMsg,"��ȡ��ƴ�����ձ����!!RespCode=[%ld][%s]",
			ret,ln_dc_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S050" );
		return 1;
	}
	ret = Com_item_Sel(g_pub_tx.reply,&com_item_tmp,"acc_hrt='%s'",dc_acc_rel_tmp.acc_hrt);
	if(ret)
	{
		sprintf(acErrMsg,"��ȡ��ƴ�����ձ����!!RespCode=[%ld][%s]",
			ret,ln_dc_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S050" );
		return 1;
	}
	strcpy( acc_hrt , com_item_tmp.acc_hrt);
	
	return 0;
}	

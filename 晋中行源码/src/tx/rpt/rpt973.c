/*************************************************************
* �� �� ��: rpt973.c
* �����������Թ���Ϣ�ͻ��ص�
*
* ��    ��: rob
* �������: 2003��09��16��
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
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "intst_hst_c.h"
#include "dd_parm_c.h"
#include "dc_acc_rel_c.h"

struct com_sys_parm_c	s_com_sys_parm;
struct intst_hst_c	s_intst_hst;
struct intst_hst_c	o_intst_hst;      /* add begin by Yanqq 20110620  �ֶμ�Ϣ�����ӡ*/
char cr_acc_hrt[8];
char dr_acc_hrt[8];

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;


int get_ratio_cxxhqd(int bli,int bll,char str[100]);

rpt973()
{
	int flag=0;
	int ttlnum;
	long tmp_date;
        /* add begin by Yanqq 20110620  �ֶμ�Ϣ�����ӡ*/
        int dMon=0;
        int dYear=0;
        int dDay=0;
        long dBeg_date=0;
        /*add end by Yanqq 20110620 */
	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[100];
	char tmp_str[60];
	char old_br_no[6];
	long intst_date=0;
	
	memset( wherelist,0,sizeof(wherelist) );
        memset(&o_intst_hst,0,sizeof(o_intst_hst));   /** add begin by Yanqq 20110620  �ֶμ�Ϣ�����ӡ*/
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	strcpy( old_br_no , "*****" );
   
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
/*
	strcpy( filename , "BJdgjxhd" );	
	strcpy(vpfm, "dgjxhd" );
*/	
	strcpy( filename , "RPT973");
	strcpy(vpfm, "RPT973");
        /* add begin by Yanqq 20110620  �ֶμ�Ϣ�����ӡ*/
        dMon=g_pub_tx.tx_date/100%100;
        dYear=g_pub_tx.tx_date/10000;
        dDay=g_pub_tx.tx_date%100;
        if(dMon%3>0)
        {
            vtcp_log("%s,%d,��ǰ�·ݷǽ�Ϣ�·ݲ�����Ϣ�嵥!",__FILE__,__LINE__);
            goto OkExit;
        }
        if(dMon/3==1)
        {
            dBeg_date=(dYear-1)*10000+12*100+21;
        }else if(dMon/3==2)
        {
            dBeg_date=dYear*10000+3*100+21;
        }else if(dMon/3==3)
        {
            dBeg_date=dYear*10000+6*100+21;
        }else{
            dBeg_date=dYear*10000+9*100+21;
        }
        /** ����ֶμ�Ϣ�ĳ����� 20110620
	ret = Intst_hst_Dec_Sel(g_pub_tx.reply,"ac_ind='0' and pt_ind='1' and intst_date=%ld order by br_no,prdt_no,ac_id",g_pub_tx.tx_date ); **/
        ret = Intst_hst_Dec_Sel(g_pub_tx.reply,"ac_ind='0' and pt_ind='1' and intst_date>%ld  and intst_date<=%ld order by br_no,prdt_no,ac_id,intst_date",dBeg_date,g_pub_tx.tx_date );
        /*add end by Yanqq 20110620 */
	ERR_DEAL
	ttlnum=0;
	while(1)
	{
            memset(&s_intst_hst,0,sizeof(s_intst_hst));
		ret = Intst_hst_Fet_Sel( &s_intst_hst , g_pub_tx.reply );
		if( ret==100 ) 
		{
			fclose(fp);
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"��ȡ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* �����任ʱ */
		pub_base_strpack(s_intst_hst.br_no );
		if( strcmp( s_intst_hst.br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				ret = print_tail();
				fclose(fp);
			}
			strcpy( old_br_no , s_intst_hst.br_no );
			page=1;
			line_file=0;
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile(s_intst_hst.br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_intst_hst.br_no , filename );
			ERR_DEAL
		}
		/****��ȡ������Ŀ��***/
		memset(cr_acc_hrt,0x00,sizeof(cr_acc_hrt));
		vtcp_log("[%s][%d]��ʼ��ȡ������Ŀ��!\n",__FILE__,__LINE__);
		cGet_cr_acc_hrt(s_intst_hst.prdt_no,"0152",cr_acc_hrt);
		vtcp_log("[%s][%d]��ȡ������Ŀ��=====[%s]!\n",__FILE__,__LINE__,cr_acc_hrt);
		/****��ȡ�跽��Ŀ��***/
		memset(dr_acc_hrt,0x00,sizeof(dr_acc_hrt));
		vtcp_log("[%s][%d]��ʼ��ȡ�跽��Ŀ��!\n",__FILE__,__LINE__);
		cGet_cr_acc_hrt(s_intst_hst.prdt_no,"0153",dr_acc_hrt);
		vtcp_log("[%s][%d]��ȡ�跽��Ŀ��=====[%s]!\n",__FILE__,__LINE__,dr_acc_hrt);
    	/* ��ӡ��Ϣ */
                /* add begin by Yanqq 20110620 ����ac_idû�б仯�Ľ����ϴε�intst_date **/
                if(o_intst_hst.ac_id == s_intst_hst.ac_id)
                {
                    s_intst_hst.ic_date=o_intst_hst.intst_date;
                }
                memcpy(&o_intst_hst,&s_intst_hst,sizeof(o_intst_hst));
                memset(&g_dd_mst,0,sizeof(g_dd_mst));
                ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d ",s_intst_hst.ac_id,s_intst_hst.ac_seqn);
                if( ret )
                {
                    sprintf(acErrMsg, "ȡ��������! [%d][%d][%d]", s_intst_hst.ac_id,s_intst_hst.ac_seqn,ret);
                    WRITEMSG
                    goto ErrExit;
                }
                /*add end by Yanqq 20110620 */
                /* add begin by Yanqq 20110620 �ж�����������Ϣ�򲻳���Ϣ�嵥 **/
                if(g_dd_mst.ac_sts[0]=='*')
                {
                    continue;
                }
                /*add end by Yanqq 20110620 */
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>=2 )	/* ���ڻ�ҳ */
		{
			fprintf(fp,"\f");
			page++;
			line_file = 0;
			
		}
		ttlnum++;
	}		
	Intst_hst_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɶԹ���Ϣ�ͻ��ص��ɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɶԹ���Ϣ�ͻ��ص�ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_cxxhqd);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ӡ��β */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ӡ���� */
int print_body( )
{
	char vrec[3];

	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ֵ���� */
int get_ratio_cxxhqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	int  season;/*����**/
	int i=0; 
	long tmpdate=0;
	char tmpname[51];
	char tmpac_no[51];
	struct com_branch_c sCom_branch;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&s_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	
	switch( bli ) 
	{ 
		case 'A': /* �������� */
			pub_base_deadlineD(g_pub_tx.tx_date,-1,&tmpdate);
			sprintf( t_str , "%4d��%2d��%2d��" , tmpdate/10000 , tmpdate%10000/100 , tmpdate%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_intst_hst.br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",s_intst_hst.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_intst_hst.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'E': /* ��     �� */
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_id=%ld and ac_seqn=1 ",s_intst_hst.ac_id);
			if( ret==100 )
			{
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_id=%ld and ac_seqn=9999 ",s_intst_hst.ac_id);
			}
			if( ret )
			{
				sprintf(acErrMsg, "ȡ�˻�����! [%d][%d]", s_intst_hst.ac_id,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_mdm_ac_rel.ac_no); 
			break; 
		case 'L': /*����*/
			sprintf( amt_tmp, "%.5lf" , s_intst_hst.rate ); 	
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'M': /*����*/
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d ",s_intst_hst.ac_id,s_intst_hst.ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg, "ȡ��������! [%d][%d][%d]", s_intst_hst.ac_id,s_intst_hst.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , g_dd_mst.name); 
			break; 
		case 'N': /*��Ϣ����*/
			
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.bic_acm ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
				
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'J': /* ��Ϣ */
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.fct_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /*�ڼ�����*/
			pub_base_deadlineD(g_pub_tx.tx_date,-1,&tmpdate);
			season = (int )tmpdate%10000/100 /4 +1;
			vtcp_log("[%s][%d]���ǵ�%d����!\n",__FILE__,__LINE__,season);
			sprintf(str,"%1d",season);
			break;
		case 'Q': /*��Ӧ�Ĵ�����ƿ�Ŀ��**/
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , cr_acc_hrt ); 
			break; 
		case 'R': /*��Ӧ�Ľ跽��ƿ�Ŀ�� martin 2008/12/26 14:48:28**/
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , dr_acc_hrt ); 
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

/***���ݲ�Ʒ��� ��dc_acc_rel �л�ȡ������Ŀ��**/
/***���� ��Ʒ��� prdt_no,���ݱ���data_code,���ؿ�Ŀ��acc_hrt***/
int cGet_cr_acc_hrt(char *c_prdt_no,char *data_code,char *acc_hrt)
{
	struct dd_parm_c 	s_dd_parm;
	struct dc_acc_rel_c 	s_dc_acc_rel;
	int ret = 0;
	vtcp_log("[%s][%d]��Ʒ���=[%s],���ݱ���=[%s]\n",__FILE__,__LINE__,c_prdt_no,data_code);
	
	ret = Dd_parm_Sel(g_pub_tx.reply,&s_dd_parm,"prdt_no='%.4s' ",c_prdt_no);
	if(ret && ret!=100)
	{
		vtcp_log("[%s][%d]���Ҵ����������ݿ����!ret=[%d]\n",__FILE__,__LINE__,ret);
		return (-1);
	}
	else if(ret==100)
	{
		vtcp_log("[%s][%d]û�������Ʒ��!\n",__FILE__,__LINE__);
		return 1;
	}
	ret = Dc_acc_rel_Sel(g_pub_tx.reply,&s_dc_acc_rel,"dc_code='%.4s' and data_code='%.4s' ",s_dd_parm.dc_code,data_code);
		if(ret && ret!=100)
	{
		vtcp_log("[%s][%d]���һ�ƴ����Ӧ��Ŀ�����ݿ����!ret=[%d]\n",__FILE__,__LINE__,ret);
		return (-1);
	}
	else if(ret==100)
	{
		vtcp_log("[%s][%d]�Ҳ�����Ӧ�Ŀ�Ŀ��!\n",__FILE__,__LINE__);
		return 1;
	}
	
	memcpy(acc_hrt,s_dc_acc_rel.acc_hrt,sizeof(s_dc_acc_rel.acc_hrt)-1);
	vtcp_log("[%s][%d]��ȡ�Ŀ�Ŀ��Ϊ[%s]\n",__FILE__,__LINE__,acc_hrt);
	
	return 0;
	
}

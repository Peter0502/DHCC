/*************************************************************
* �� �� ��: rpt295.c
* ��������: ���ڼ�����Ϣ�嵥
*
* ��    ��: Jarod
* �������: 2007��07��24��
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
#include "com_branch_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "com_sys_parm_c.h"
#include "td_list_c.h"

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
struct td_list_c sTd_list;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct td_mst_c sTd_mst;
	struct com_sys_parm_c sCom_sys_parm;
long hs_cnt=0;
double amt=0;

int get_ratio_tdtdb(int bli,int bll,char str[100]);

rpt295()
{
	int flag=0;
	int ttlnum;
	char fname[100];
	char filename[100];
	char wherelist[1024];

	char tmp_str[60];
	char old_br_no[6];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &sTd_list , 0x00 , sizeof(struct td_list_c));
	
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
	
	strcpy( filename , "RPT295");
	strcpy(vpfm, "RPT295");
	
			ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm," 1=1");	
			if(ret)
			{
				sprintf(acErrMsg,"ȡϵͳ���ڳ���!![%d]",ret);		
				WRITEMSG
				goto ErrExit;
			}
	ret=Td_list_Dec_Sel( g_pub_tx.reply,"tx_date=%ld order by opn_br_no,ac_id,flag",sCom_sys_parm.sys_date );
	ERR_DEAL
		ttlnum=0;
	while(1)
	{
	memset(&sTd_mst,0x0,sizeof(struct td_mst_c));
	memset(&sMdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset( &sTd_list , 0x00 , sizeof(struct td_list_c));
		ret=Td_list_Fet_Sel( &sTd_list, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
	vtcp_log("��ӡҳβ1:[%s]--[%d]",__FILE__,__LINE__);
				ret=print_sum();
				ERR_DEAL
				amt=0.00;
				hs_cnt=0;
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* �����任ʱ */
		pub_base_strpack(sTd_list.opn_br_no );
		if( strcmp( sTd_list.opn_br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
	vtcp_log("��ӡҳβ2:[%s]--[%d]",__FILE__,__LINE__);
				ret=print_sum();
				ERR_DEAL
				amt=0.00;
				hs_cnt=0;
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , sTd_list.opn_br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( sTd_list.opn_br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , sTd_list.opn_br_no , filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
    		/* ��ӡ��Ϣ */
		ret = print_body();
		ERR_DEAL
		line_file++;

		if(sTd_list.flag[0]=='0')
			amt+=sTd_list.td_lx;
		else
			amt+=sTd_list.dd_lx;
		hs_cnt++;
	
		if( line_file>50 )	/* ����50�л�ҳ */
		{
			line_file=0;
			page++;
			/* ��ӡҳβ */
			ret = print_tail();
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}
	Td_list_Clo_Sel();
	if(sCom_sys_parm.sys_date%10000==1231)
	{
	ret=sql_execute("delete from td_list where tx_date!=%ld",sCom_sys_parm.sys_date);
			ERR_DEAL
	}	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ͨ��ͨ����ˮ�嵥(������)�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"����ͨ��ͨ����ˮ�嵥(������)ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_tdtdb);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ӡ���� */
int print_head( )
{
	char vrec[3];
	
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
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

/*��ӡ�ϼƽ��*/
int print_sum()
{
	char vrec[3];
	
	strcpy(vrec,"ZZ");
	ret=Make_yeb_sub(vrec);
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

	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ֵ���� */
int get_ratio_tdtdb( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	int i=0; 
	struct com_branch_c sCom_branch;
	int len=0;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));

	switch( bli ) 
	{
		case 'A': /* �������� */
			ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm," 1=1");	
			if(ret)
			{
				sprintf(acErrMsg,"ȡϵͳ���ڳ���!![%d]",ret);		
				WRITEMSG
				goto ErrExit;
			}
			sprintf( t_str , "%4d��%2d��%2d��" , sCom_sys_parm.lst_date/10000,sCom_sys_parm.lst_date%10000/100 , sCom_sys_parm.lst_date%100 );			
			sprintf(fmt,"%%-%ds",bll);
 			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , sTd_list.opn_br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		sTd_list.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , sTd_list.opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /*�ʺ�*/
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld ",sTd_list.ac_id);	
	ERR_DEAL
			pub_base_strpack(sMdm_ac_rel.ac_no); 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt,sMdm_ac_rel.ac_no ); 
			break; 
		case 'F': /*���� */ 
	ret=Td_mst_Sel(g_pub_tx.reply,&sTd_mst,"ac_id=%ld and ac_seqn=%d",sTd_list.ac_id,sTd_list.ac_seqn);
	ERR_DEAL
			pub_base_strpack(sTd_mst.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, sTd_mst.name);
			break;
		case 'G': /* ״̬*/
			if(sTd_mst.ac_sts[0]=='1')
				sprintf(t_str,"����");
			else if(sTd_mst.ac_sts[0]=='*')
				sprintf(t_str,"����");
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'H':  /*��������*/
			if(sTd_list.flag[0]=='0')
				sprintf( t_str , "%ld" , sTd_list.td_day); 
			else
				sprintf( t_str , "%ld" , sTd_list.dd_day); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'I': /*��Ϣ��� */
			if(sTd_list.flag[0]=='0')
				sprintf( amt_tmp, "%.2lf" ,sTd_list.td_amt); 
			else
				sprintf( amt_tmp, "%.2lf" ,sTd_list.dd_amt); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				
				sprintf(fmt,"%%%ds",bll); 		
				sprintf( str, fmt, amt_tmp ); 
			break;
		case 'J': /*����*/
			if(sTd_list.flag[0]=='0')
				sprintf( amt_tmp, "%.2lf" ,sTd_list.td_rate); 
			else
				sprintf( amt_tmp, "%.2lf" ,sTd_list.dd_rate); 
			pub_rpt_flttomon(amt_tmp,amt_tmp);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'K': /*��Ʒ����*/
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,sTd_mst.prdt_no);
			break;
		case 'L': /*������*/
			if(sTd_list.flag[0]=='0')
				sprintf( amt_tmp, "%.2lf" ,sTd_list.td_lx); 
			else
				sprintf( amt_tmp, "%.2lf" ,sTd_list.dd_lx); 
			pub_rpt_flttomon(amt_tmp,amt_tmp);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'M': /*�����ϼ�*/
			sprintf(amt_tmp,"%ld",hs_cnt);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'N': /*������ϼ�*/
			sprintf( amt_tmp, "%.2lf" ,amt); 
			pub_rpt_flttomon(amt_tmp,amt_tmp);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str,fmt,amt_tmp);
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

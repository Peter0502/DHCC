/*************************************************************
* �� �� ��: rpt929.c
* ������������ӡ��������ҵ�������д������  (��  ��)
*
* ��    ��: Jarod
* �������: 2006��11��08��
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
#include "mo_merge_ac_c.h"
#include "dd_mst_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "mdm_ac_rel_c.h"

struct dd_mst_c gDd_mst;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
long g_count=0;
char old_ac_no[25];
static double sum=0.0;

int get_ratio_tdtdb(int bli,int bll,char str[200]);

rpt929()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];

	memset(&sum,0x00,sizeof(double));	
	memset( &gDd_mst,0x00,sizeof(struct dd_mst_c));	
	memset( wherelist,0,sizeof(wherelist) );
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
	
	strcpy( filename , "RPT929");
	strcpy(vpfm, "RPT929");
	
	/**��ɲ�ѯ**/
	memset(wherelist,'\0',sizeof(wherelist));
	strcat(wherelist," bal != 0.0 and  ac_type<>'5'  order by opn_br_no,prdt_no ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret= Dd_mst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Dd_mst_Fet_Sel( &gDd_mst, g_pub_tx.reply );
		memset(old_ac_no,'\0',sizeof(old_ac_no));		
		sum+=gDd_mst.bal;
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));	
	vtcp_log("��ӡҳβ1:[%s]--[%d]",__FILE__,__LINE__);
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"�����ڴ�����ļ�������!![%d]",ret);
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* �����任ʱ */
		pub_base_strpack( gDd_mst.opn_br_no );
		if( strcmp( gDd_mst.opn_br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));	
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , gDd_mst.opn_br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( gDd_mst.opn_br_no, filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , gDd_mst.opn_br_no, filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
    		/* ��ӡ��Ϣ */
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>50 )	/* ����50�л�ҳ */
		{
			line_file=0;
			page++;
			ret=print_ZZ(); /*��ӡÿҳ��С��*/
			ERR_DEAL
			memset(&sum,0x00,sizeof(double));	
			/* ��ӡҳβ */
			ret = print_tail();
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
		g_count++;
	}
	Dd_mst_Clo_Sel();

	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����¾��ʺŶ��ձ�ɹ�!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
printf("\n������[%d]����¼.\n",g_count);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�����¾��ʺŶ��ձ�ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_tdtdb);
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

int print_ZZ()
{
	char vrec[3];
	strcpy(vrec,"ZZ");
	ret=Make_yeb_sub(vrec);
	ERR_DEAL
OkExit:
	return 0;
ErrExit:
	return 1;
}

/* ��ֵ���� */
int get_ratio_tdtdb( bli,bll,str )
int bli,bll;
char str[200];
{
	char fmt[10]; 
	struct com_branch_c sCom_branch;
	struct com_sys_parm_c sCom_sys_parm;
	struct mo_merge_ac_c sMo_merge_ac;
	int ret1=0,ret2=0;
	struct mdm_ac_rel_c sMdm_ac_rel;
	char str_mon[30];

    memset(str_mon,'\0',sizeof(str_mon));
	memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sMo_merge_ac,0x00,sizeof(struct mo_merge_ac_c));	
	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': /* �������� */
			ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ����ϵͳ��������!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf( t_str , "%4d��%2d��%2d��" , sCom_sys_parm.sys_date/10000 , sCom_sys_parm.sys_date%10000/100 , sCom_sys_parm.sys_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",gDd_mst.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name ,gDd_mst.opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break;
		case 'M': /*���ʺ�*/
			ret1=Mo_merge_ac_Sel(g_pub_tx.reply,&sMo_merge_ac,"ac_id=%d",gDd_mst.ac_id);
			if(ret1 == 100)
			{
				ret2=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%d",gDd_mst.ac_id);
				if(ret2)
				{
					sprintf(acErrMsg,"��ѯ�������ʻ���ϵ�����!![%d]",ret2);
					WRITEMSG
					goto ErrExit;
				}
				sprintf( t_str , "%s" , sMdm_ac_rel.ac_no); 
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str, fmt, t_str);
				break;
			}
			else if(ret1)
			{
				sprintf(acErrMsg,"��ѯ�¾��ʺŶ��ձ����!!!~~~[%d]",ret1);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(old_ac_no,sMo_merge_ac.old_ac_no);
			sprintf( t_str , "%s" , sMo_merge_ac.ac_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'I': /*���ʺ�*/
			sprintf( t_str , "%s" ,old_ac_no);
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'N': /*���*/
			sprintf(t_str,"%.2lf",gDd_mst.bal);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,str_mon);
			break;	
		case 'L':  /*��      �� */
			pub_base_strpack(gDd_mst.name); 
			sprintf( t_str , "%s" , gDd_mst.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'O': /*ÿҳС��*/
			sprintf(t_str,"%.2lf",sum);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,str_mon);
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

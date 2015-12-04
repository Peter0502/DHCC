/*************************************************************
* �� �� ��: rpt930.c
* ������������ӡ���д�������
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
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "mdm_ac_rel_c.h"
#include "ln_mst_c.h"
#include "ln_parm_c.h"
#include "dc_acc_rel_c.h"								/*���ӡ���Ŀ�š���zhangpeng 20131126*/
#include "com_item_c.h"									/*���ӡ���Ŀ�š���zhangpeng 20131126*/

struct ln_mst_c gLn_mst;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
long g_count=0;
static double sum;
static double sumval;
int get_ratio_tdtdb(int bli,int bll,char str[200]);

rpt930()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[100];
	char prdt_no[5];			/**��Ʒ���**/
	char tmp_str[60];
	char old_br_no[6];

	memset(&sum,0x00,sizeof(sum));
	memset(&sumval,0x00,sizeof(sumval));
	memset(&gLn_mst,0x00,sizeof(struct ln_mst_c));
	memset( wherelist,0,sizeof(wherelist) );
	strcpy(prdt_no,"***");
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
	
	strcpy( filename , "RPT930");
	strcpy(vpfm, "RPT930");
	
	/**��ɲ�ѯ**/
	memset(wherelist,'\0',sizeof(wherelist));
	strcat(wherelist," bal != 0.0 or in_lo_intst!=0.00 or out_lo_intst!=0.00 or cmpd_lo_intst!=0.00 order by opn_br_no,prdt_no,ac_id ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret= Ln_mst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Ln_mst_Fet_Sel( &gLn_mst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(sum));
				memset(&sumval,0x00,sizeof(sumval));
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
		pub_base_strpack( gLn_mst.opn_br_no );
		if( strcmp( gLn_mst.opn_br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(sum));
				memset(&sumval,0x00,sizeof(sumval));
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			strcpy(prdt_no,"***");/**��ʼ����ƷС��**/
			strcpy( old_br_no , gLn_mst.opn_br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( gLn_mst.opn_br_no, filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , gLn_mst.opn_br_no, filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
    		/* ��ӡ��Ϣ */
		vtcp_log("%s,%d,��ǰ��Ʒ�ţ�%s,prdt_no->%s",__FILE__,__LINE__,gLn_mst.prdt_no,prdt_no);    			
    if(strcmp( gLn_mst.prdt_no , prdt_no ))
    {
    	if(strcmp(prdt_no,"***"))
    		{
    			vtcp_log("%s,%d,��Ʒ��Ϊ��%s,С�ƣ�%f",__FILE__,__LINE__,gLn_mst.prdt_no,sumval);
    			print_XX();/**��ӡ��ƷС��**/
    			sumval = 0.0;
    		}else
    		{
    			sumval = 0.0;
    		}
    	strcpy(prdt_no,gLn_mst.prdt_no);
    }
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>50 )	/* ����50�л�ҳ */
		{
			line_file=0;
			page++;
			print_XX();/**��ӡ��ƷС��**/
    	sumval = 0.0;
			ret=print_ZZ();
			ERR_DEAL
			memset(&sum,0x00,sizeof(sum));
			memset(&sumval,0x00,sizeof(sumval));
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
	Ln_mst_Clo_Sel();

	
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
	
	if( page != 1 )
		{
			fprintf ( fp , "\f" );
		}
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

/*��ӡС��*/
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
/*��ӡ��ƷС��*/
int print_XX()
{
	char vrec[3];
	strcpy(vrec,"XX");
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
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dc_acc_rel_c s_dc_acc_rel;				/*���ӡ���Ŀ�š���zhangpeng 20131126*/
	struct com_item_c s_com_item;						/*���ӡ���Ŀ�š���zhangpeng 20131126*/
	double lixi;
	struct ln_parm_c sLn_parm;
	char str_mon[30];

	memset(&lixi,0x00,sizeof(lixi));
	memset(str_mon,0x00,sizeof(str_mon));
	memset(&sLn_parm,0x00,sizeof(struct ln_parm_c));
	memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&s_dc_acc_rel, 0x0, sizeof(struct dc_acc_rel_c));	/*���ӡ���Ŀ�š���zhangpeng 20131126*/
	memset(&s_com_item, 0x0, sizeof(struct com_item_c));			/*���ӡ���Ŀ�š���zhangpeng 20131126*/
	
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
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",gLn_mst.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name ,gLn_mst.opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break;
		case 'M': /*�ʺ�*/
		       /*ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%d",gLn_mst.ac_id);*/
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%d order by ac_no",gLn_mst.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ�������ʻ���ϵ�����!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf( t_str , "%s" , sMdm_ac_rel.ac_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'N': /*���*/
			sum+=gLn_mst.bal;
			sumval+=gLn_mst.bal;
			sprintf(t_str,"%.2lf",gLn_mst.bal);
			pub_rpt_flttomon(t_str,str_mon);
			/****
			sprintf(fmt,"%%-%ds",bll);
			****/
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,str_mon);
			break;	
		case 'L':  /*��      �� */
			pub_base_strpack(gLn_mst.name); 
			sprintf( t_str , "%s" , gLn_mst.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
		case 'O': /*����*/
			sprintf(t_str,"%.6lf",gLn_mst.rate);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,str_mon);
			break;
		case 'F': /*��Ϣ��*/
			sprintf(t_str,"%d",gLn_mst.opn_date);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,t_str);
			break;
		case 'I': /*������*/
			sprintf(t_str,"%d",gLn_mst.mtr_date);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,t_str);
			break;
			
		/**ǷϢ���ע�͵���zhangpeng 20131126 start**
		case 'J': //ǷϢ���
			lixi=gLn_mst.in_lo_intst+gLn_mst.out_lo_intst+gLn_mst.cmpd_lo_intst;
			sprintf(t_str,"%.2lf",lixi);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,str_mon);
			break;	
		*ǷϢ���ע�͵���zhangpeng 20131126 end***/
		
		/**��ǷϢ�������Ϊ����ǷϢ�ͱ���ǷϢ��zhangpeng 20131126 start***/	
		case 'J': /*����ǷϢ*/
			sprintf(t_str,"%.2lf",gLn_mst.in_lo_intst);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,str_mon);
			break;
		case 'Q': /*����ǷϢ*/
			sprintf(t_str,"%.2lf",gLn_mst.out_lo_intst);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,str_mon);
			break;
		/**��ǷϢ�������Ϊ����ǷϢ�ͱ���ǷϢ��zhangpeng 20131126 start***/
			
		/**��Ʒ����ע�͵���zhangpeng 20131126 start**
		case 'K': //��Ʒ����
			ret=Ln_parm_Sel(g_pub_tx.reply,&sLn_parm,"prdt_no='%s'",gLn_mst.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ�����Ʒ���������!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(t_str,"%s",sLn_parm.title);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,t_str);
			break;
		*��Ʒ����ע�͵���zhangpeng 20131126 end***/
		
		/**��Ʒ���Ƹ�Ϊ��Ŀ�š�zhangpeng 20131126 start***/	
		case 'K': /*��Ŀ��*/
			ret=Ln_parm_Sel( g_pub_tx.reply ,&sLn_parm,"prdt_no = '%s'",gLn_mst.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"��ѯ�����Ʒ���������!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			/* �޸Ŀ�Ŀ���ѵ�����Ϣ��Ŀ����� */
			ret=Dc_acc_rel_Sel( g_pub_tx.reply ,&s_dc_acc_rel,"dc_code = '%s' and data_code='0152'",sLn_parm.dc_code);
			if( ret )
			{
				sprintf(acErrMsg,"��ѯ��ƴ����Ӧ��Ŀ�����!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			ret=Com_item_Sel( g_pub_tx.reply ,&s_com_item,"acc_hrt = '%s'",s_dc_acc_rel.acc_hrt);
			if( ret )
			{
				sprintf(acErrMsg,"��ѯ������Ŀ�����!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, s_com_item.acc_no ); 
			memset( &sLn_parm, 0x00 , sizeof( struct ln_parm_c ));
			memset( &s_dc_acc_rel, 0x00 , sizeof( struct dc_acc_rel_c ));
			memset( &s_com_item, 0x00 , sizeof( struct com_item_c ));
			break;
		/**��Ʒ���Ƹ�Ϊ��Ŀ�š�zhangpeng 20131126 end***/
		
		case 'P': /*ÿҳС��*/
			sprintf(t_str,"%.2lf",sum);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt," %%-%ds",bll);
			sprintf(str,fmt,str_mon);
			break;
		case 'X': /*��ƷС��*/
			sprintf(t_str,"%.2lf",sumval);
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

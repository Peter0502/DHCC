/*************************************************************
* �� �� ��: aclist.c
* ������������ӡ�¾��ʺŶ��ձ�
*
* ��    ��: Jarod
* �������: 2006��11��04��
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
#include "cif_basic_inf_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
#include "mo_merge_ac_c.h"
#include "com_sys_parm_c.h"
#include "in_parm_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"
#include "prdt_ac_id_c.h"

struct mo_merge_ac_c gMo_merge_ac;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

int get_ratio_tdtdb(int bli,int bll,char str[100]);

aclist()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset(&gMo_merge_ac,0x00,sizeof(struct mo_merge_ac_c));	
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
	
	strcpy( filename , "ACLIST");
	strcpy(vpfm, "aclist");
	
	/* ����:���׻���!=�������� */
	strcpy( tmpstr," tx_br_no != opn_br_no and ");
	strcat( wherelist,tmpstr );	
	/**��ɲ�ѯ**/
	
	strcat(wherelist," 1=1  ORDER BY br_no,ac_no ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret= Mo_merge_ac_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Mo_merge_ac_Fet_Sel( &gMo_merge_ac, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
	vtcp_log("��ӡҳβ1:[%s]--[%d]",__FILE__,__LINE__);
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"���¾��ʺŶ��ձ������!![%d]",ret);
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* �����任ʱ */
		pub_base_strpack( gMo_merge_ac.br_no );
		if( strcmp( gMo_merge_ac.br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
	vtcp_log("��ӡҳβ2:[%s]--[%d]",__FILE__,__LINE__);
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , gMo_merge_ac.br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( gMo_merge_ac.br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , gMo_merge_ac.br_no , filename );
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
			/* ��ӡҳβ */
			ret = print_tail();
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}
	Mo_merge_ac_Clo_Sel();

	/*  fclose(fp);  */
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����¾��ʺŶ��ձ�ɹ�!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
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
	struct cif_basic_inf_c  sCif_basic_inf;
	struct in_mst_c sIn_mst;
	int len=0;
	struct com_sys_parm_c sCom_sys_parm;
	struct dd_parm_c sDd_parm;
	struct td_parm_c sTd_parm;
	struct in_parm_c sIn_parm;
	struct prdt_ac_id_c sPrdt_ac_id;

	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&sIn_parm,0x00,sizeof(struct in_parm_c));	
	memset(&sTd_parm,0x00,sizeof(struct td_parm_c));
	memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&sIn_mst,0x0,sizeof(struct in_mst_c));
	memset(&sCif_basic_inf,0x0,sizeof(struct cif_basic_inf_c));
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
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		gMo_merge_ac.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name ,gMo_merge_ac.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break;
		case 'M': /*��Ʒ����*/
			ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%d",gMo_merge_ac.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ����!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			switch(sPrdt_ac_id.ac_id_type[0])
			{
				case '1':
					ret=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sPrdt_ac_id.prdt_no);
					if(ret)
					{
						sprintf(acErrMsg,"��ѯ���ڲ�Ʒ���������!![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
					pub_base_strpack(sDd_parm.title);
					sprintf(t_str,"%s",sDd_parm.title);
					sprintf(fmt,"%%-%ds",bll);
					sprintf(str,fmt,t_str);
					break;
				case '2':
					ret=Td_parm_Sel(g_pub_tx.reply,&sTd_parm,"prdt_no='%s'",sPrdt_ac_id.prdt_no);
					if(ret)
					{
						sprintf(acErrMsg,"��ѯ���ڲ�Ʒ���������!![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
					pub_base_strpack(sTd_parm.title);
					sprintf(t_str,"%s",sTd_parm.title);
					sprintf(fmt,"%%-%ds",bll);
					sprintf(str,fmt,t_str);
					break;
				case '9':
					ret=In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no='%s'",sPrdt_ac_id.prdt_no);
					if(ret)
					{
						sprintf(acErrMsg,"�ڲ���Ʒ���������!![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
					pub_base_strpack(sIn_parm.prdt_mo);
					sprintf(t_str,"%s",sIn_parm.prdt_mo);
					sprintf(fmt,"%%-%ds",bll);
					sprintf(str,fmt,t_str);
					break;
				default:
					memset( str,' ',bll ); 
					break;
			}
			break;
		case 'I': /*�� �� �� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, gMo_merge_ac.ac_no);
			break;
		case 'N': /*�»�����*/
			sprintf(fmt,"%%-5s");
			sprintf(str,fmt,gMo_merge_ac.br_no);
			break;
		case 'J': /*���ʺ�*/
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,gMo_merge_ac.old_ac_no);
			break;
		case 'K': /*�ɻ�����*/
			sprintf(fmt,"%%-7s");
			sprintf(str,fmt,gMo_merge_ac.old_br_no);
			break;
		case 'L':  /*��      �� */
			ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%d",gMo_merge_ac.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ����!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			if(gMo_merge_ac.ac_no[0]=='9')
			{
				ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%d",gMo_merge_ac.ac_id);
				if(ret && ret != 100)
				{
					sprintf(acErrMsg,"��ѯ�ڲ�̨�����ļ�������haha!![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}	
			pub_base_strpack(sIn_mst.name); 
			sprintf( t_str , "%s" , sIn_mst.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
			}
			else
			{	
				ret=Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf,"cif_no=%d",sPrdt_ac_id.cif_no);
				if(ret)
				{
					sprintf(acErrMsg,"��ѯ�ͻ�������Ϣ����� !![%d]",ret);
					WRITEMSG	
					goto ErrExit;
				}
			pub_base_strpack(sCif_basic_inf.name); 
			sprintf( t_str , "%s" , sCif_basic_inf.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
			}
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}

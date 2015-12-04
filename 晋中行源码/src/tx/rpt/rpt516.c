/*************************************************************
* �� �� ��: rpt516.c
* ����������С��֧���������ܾ��嵥
*
* ��    ��: xyy
* �������: 2006-10-17 17:59
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
#define   EXTERN
#include "public.h"
#include "lv_wbctl_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"

static struct lv_wbctl_c lv_wbctl;
static struct com_sys_parm_c com_sys_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

int get_ratio_lvwbjj(int bli,int bll,char str[100]);
extern char *cGetLvPkgDes(char *stat,char *desc);
extern char *cGetLvCmnoDes(char *cmtno,char *desc);

rpt516()
{
	int ttlnum;
	int cnt=0;
	long qry_date = 0;
	
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[300];

	char old_br_no[6];
	memset( vpfm     ,0,sizeof(vpfm));
	memset( t_str    ,0,sizeof(t_str));
	memset( filename ,0,sizeof(filename));
	memset( tmpstr   ,0,sizeof(tmpstr));
	memset( wherelist,0,sizeof(wherelist));
	memset( &lv_wbctl , 0x00 , sizeof(struct lv_wbctl_c));
	memset( &com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	
	strcpy( old_br_no , "*****" );
	/*���㽻������*/
	
	g_reply_int = Com_sys_parm_Sel( g_pub_tx.reply , &com_sys_parm , "sys_date=%ld" , g_pub_tx.tx_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}
  qry_date = com_sys_parm.lst_date;
   
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	
	strcpy( filename , "RPT516" );	
	strcpy(vpfm, "rpt516" );

	sprintf( tmpstr," in_date ='%ld' and packstat='01' " , qry_date);
	strcat( wherelist,tmpstr );	

	strcat(wherelist," order by br_no,packid,pkgno");
	vtcp_log( "[%s][%d]WHERE = [%s]",__FILE__,__LINE__,wherelist);
	
	ret=Lv_wbctl_Dec_Sel( g_pub_tx.reply, wherelist );

	ttlnum=0;
	
	while(1)
	{
		memset( &lv_wbctl , 0x00 , sizeof(struct lv_wbctl_c));
		ret=Lv_wbctl_Fet_Sel( &lv_wbctl, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"��ȡlv_wbctl�ܾ���ϸ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
					
		/* �����任ʱ */
		pub_base_strpack( lv_wbctl.br_no );
		if( strcmp( lv_wbctl.br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , lv_wbctl.br_no );
			vtcp_log("[%s][%d]br_no=[%s]",__FILE__,__LINE__,lv_wbctl.br_no);
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( lv_wbctl.br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , lv_wbctl.br_no , filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
    		/* ͳ����Ϣ */
    cnt = sql_count("lv_wbctl"," in_date ='%ld' and br_no='%s' and packstat ='01' ",qry_date,old_br_no);
    vtcp_log("[%s][%d]������Ϊ[%s]���ܹ�[%d]�ʵ�ǰ��ӡ��[%d]��",__FILE__,__LINE__,old_br_no,cnt,line_file);
    		/* ��ӡ��Ϣ */		
		ret = print_body();
		ERR_DEAL
		vtcp_log("[%s][%d]line_file=[%d]",__FILE__,__LINE__,line_file);
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
	Lv_wbctl_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ѯlv_wbctl�ܾ���ϸ�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��ѯlv_wbctl�ܾ���ϸʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	vtcp_log("[%s][%d]Make_yeb_sub begin",__FILE__,__LINE__);
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lvwbjj);
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
int print_body( )
{
	char vrec[3];
  vtcp_log("[%s][%d]print_body begin",__FILE__,__LINE__);
	strcpy( vrec,"BB" );
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


/* ��ֵ���� */
int get_ratio_lvwbjj( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char amt_tmp[21]; 
	char cTx_Desc[17];
	char fmt[10]; 
	int i=0; 
	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(cTx_Desc,0x00,	sizeof(cTx_Desc));
	
	switch( bli ) 
	{ 
		case 'A': /* �������� */
			sprintf( t_str , "%4d��%2d��%2d��" , lv_wbctl.in_date/10000 , lv_wbctl.in_date%10000/100 ,lv_wbctl.in_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin in_date=[%s]",__FILE__,__LINE__,str);
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , lv_wbctl.br_no ); 
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin br_no=[%s]",__FILE__,__LINE__,str);
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		  lv_wbctl.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , lv_wbctl.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin br_name=[%s]",__FILE__,__LINE__,str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin page=[%d]",__FILE__,__LINE__,str);
			break; 
		case 'E': /* PKGNO 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_wbctl.pkgno);
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin pkgno=[%s]",__FILE__,__LINE__,str);
			break;
			**/
			vtcp_log("[%s][%d]��ʼ��ӡ--����״̬!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetLvCmnoDes(lv_wbctl.pkgno,cTx_Desc);
			cDeal_str(cTx_Desc, bll);
			memcpy(str,cTx_Desc,bll);
			break;
		case 'F': /* ����� */						
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_wbctl.packid); 
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin packid=[%s]",__FILE__,__LINE__,str);
			break;
		case 'G': /* ������ */
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, lv_wbctl.pack_date);
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin pack_date=[%ld]",__FILE__,__LINE__,str);
			break;	
		case 'H': /* ���������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_wbctl.pay_qs_no);
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin pay_qs_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'I': /* ���������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_wbctl.cash_qs_no);
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin cash_qs_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'J': /* �ܽ�� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , lv_wbctl.totamt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin totamt=[%.2lf]",__FILE__,__LINE__,str);
			break;
		case 'K': /* �ܱ��� */
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, lv_wbctl.dtlcnt);
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin dtlcnt=[%ld]",__FILE__,__LINE__,str);
			break;		
		case 'L': /* �ɹ��ܽ�� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , lv_wbctl.dtlsucamt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin dtlsucamt=[%.2lf]",__FILE__,__LINE__,str);
			break;
		case 'M': /* �ɹ��ܱ��� */
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, lv_wbctl.dtlsuccnt);
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin dtlsuccnt=[%ld]",__FILE__,__LINE__,str);
			break;	
		case 'N': /* ������״̬ */
			/**
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_wbctl.packstat);
			vtcp_log("[%s][%d]get_ratio_lvwbjj begin packstat=[%s]",__FILE__,__LINE__,str);
			break;
			**/
			vtcp_log("[%s][%d]��ʼ��ӡ--����״̬!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetLvPkgDes(lv_wbctl.packstat,cTx_Desc);
			cDeal_str(cTx_Desc, bll);
			memcpy(str,cTx_Desc,bll);
			break;
		case 'O': /* ��ϵͳ��״̬ 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_wbctl.stat);
			vtcp_log("[%s][%d]get_ratio_lvpkgwb begin pack_date=[%s]",__FILE__,__LINE__,str);
			break;	
			**/
			vtcp_log("[%s][%d]��ʼ��ӡ--����״̬!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetLvPkg2Des(lv_wbctl.stat,cTx_Desc);
			cDeal_str(cTx_Desc, bll);
			memcpy(str,cTx_Desc,bll);
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

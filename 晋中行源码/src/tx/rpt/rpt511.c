/*************************************************************
* �� �� ��: rpt511.c
* ����������С��֧���������嵥
*
* ��    ��: xyy
* �������: 2006-10-16 20:10
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
#include "lv_pkgreg_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"

static struct lv_pkgreg_c lv_pkgreg;
static struct com_sys_parm_c com_sys_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

int get_ratio_lvpkglz(int bli,int bll,char str[100]);
extern char *cGetLvCmnoDes(char *cmtno,char *desc);
extern char *cGetLvzfStat(char stat,char *desc);

rpt511()
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
	memset( &lv_pkgreg , 0x00 , sizeof(struct lv_pkgreg_c));
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
	
	strcpy( filename , "RPT511" );	
	strcpy(vpfm, "rpt511" );

	sprintf( tmpstr," in_date ='%ld' and lw_ind='2'  " , qry_date);
	strcat( wherelist,tmpstr );	

	strcat(wherelist," order by br_no,orderno");
	vtcp_log( "[%s][%d]WHERE = [%s]",__FILE__,__LINE__,wherelist);
	
	ret=Lv_pkgreg_Dec_Sel( g_pub_tx.reply, wherelist );

	ttlnum=0;
	
	while(1)
	{
		memset( &lv_pkgreg , 0x00 , sizeof(struct lv_pkgreg_c));
		vtcp_log("[%s][%d] line_file=[%d] ret=[%d]",__FILE__,__LINE__,line_file,ret);
		ret=Lv_pkgreg_Fet_Sel( &lv_pkgreg, g_pub_tx.reply );
		vtcp_log("[%s][%d] line_file=[%d] ret=[%d]",__FILE__,__LINE__,line_file,ret);
		if( ret==100 )
		{
			if( !ttlnum )
			{
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
				fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);
				fclose(fp);
			}
			fclose(fp);
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"��ȡС��������ϸ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
	  vtcp_log("[%s][%d] here line_file ",__FILE__,__LINE__,line_file);
		/* �����任ʱ */
		pub_base_strpack( lv_pkgreg.br_no );
		if( strcmp( lv_pkgreg.br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , lv_pkgreg.br_no );
			vtcp_log("[%s][%d]br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.br_no);
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( lv_pkgreg.br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , lv_pkgreg.br_no , filename );
			ERR_DEAL
			 fprintf(fp,"%c%c",0x1b,0x40);
                         fprintf(fp,"%c%c%c%c%c\n",0x1b,0x40,0x1b,0x33,0x18);
                         fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
    		/* ͳ����Ϣ **/
    cnt = sql_count("lv_pkgreg"," in_date='%ld' and br_no ='%s' and lw_ind='2' ",qry_date,old_br_no);
    vtcp_log("[%s][%d]������Ϊ[%s]���ܹ�[%d]�ʵ�ǰ��ӡ��[%d]��",__FILE__,__LINE__,old_br_no,cnt,line_file);
    
    		/* ��ӡ��Ϣ */		
		ret = print_body();
		ERR_DEAL
		vtcp_log("[%s][%d]line_file=[%d]",__FILE__,__LINE__,line_file);
		line_file++;
		vtcp_log("[%s][%d]line_file=[%d]",__FILE__,__LINE__,line_file);
		/* if( line_file>8 ) del by martin 070517 ����8�л�ҳ */
		if( line_file>7 )	/* ����7�л�ҳ */
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
		vtcp_log("[%s][%d]line_file=[%d]",__FILE__,__LINE__,line_file);
		ttlnum++;
		vtcp_log("[%s][%d]line_file=[%d]",__FILE__,__LINE__,line_file);
	}
	Lv_pkgreg_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ѯС��֧������������ϸ�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��ѯС��֧������������ϸʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	vtcp_log("[%s][%d]Make_yeb_sub begin",__FILE__,__LINE__);
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lvpkglz);
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
	if (page != 1)
	{
		fprintf(fp, "\f");
	}
	strcpy( vrec,"TT" );
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
	strcpy( vrec,"HH" );
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
	/*fprintf(fp,"\f");*/
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ֵ���� */
int get_ratio_lvpkglz( bli,bll,str )
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
			sprintf( t_str , "%4d��%2d��%2d��" , lv_pkgreg.in_date/10000 , lv_pkgreg.in_date%10000/100 ,lv_pkgreg.in_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin in_date=[%s]",__FILE__,__LINE__,str);
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , lv_pkgreg.br_no ); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin br_no=[%s]",__FILE__,__LINE__,str);
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		  lv_pkgreg.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , lv_pkgreg.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin br_name=[%s]",__FILE__,__LINE__,str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin page=[%d]",__FILE__,__LINE__,str);
			break; 
		case 'E': /* PKGNO */
			/**
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.pkgno);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin pkgno=[%s]",__FILE__,__LINE__,str);
			break;
			****/
			vtcp_log("[%s][%d]��ʼ��ӡ--����״̬!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetLvCmnoDes(lv_pkgreg.pkgno,cTx_Desc);
			cDeal_str(cTx_Desc, bll);
			memcpy(str,cTx_Desc,bll);
			break;
		case 'F': /* ������� */						
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.orderno); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin orderno=[%s]",__FILE__,__LINE__,str);
			break;
		case 'G': /* �������к� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.or_br_no);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin or_br_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'H': /* �������ʺ� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.pay_ac_no);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin pay_ac_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'I': /* ���������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.pay_name);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin pay_name=[%s]",__FILE__,__LINE__,str);
			break;
		case 'J': /* �������к� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.ac_br_no); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin ac_br_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'K': /* ���׽�� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , lv_pkgreg.tx_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin tx_amt=[%.2lf]",__FILE__,__LINE__,str);
			break;	
		case 'L': /* �տ����ʺ� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.cash_ac_no);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin cash_ac_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'M': /* �տ������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.cash_name);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin cash_name=[%s]",__FILE__,__LINE__,str);
			break;
		case 'N': /* ����� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.packid);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin packid=[%s]",__FILE__,__LINE__,str);
			break;
		case 'O': /* ������ */
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, lv_pkgreg.pack_date);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin pack_date=[%ld]",__FILE__,__LINE__,str);
			break;
		case 'P': /* ��¼״̬ 
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, lv_pkgreg.lv_sts);
			vtcp_log("[%s][%d]get_ratio_lvpkgjj begin pack_date=[%ld]",__FILE__,__LINE__,str);
			break;
			**/
			vtcp_log("[%s][%d]��ʼ��ӡ--����״̬!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			vtcp_log("[%s][%d]lv_sts===[%c]",__FILE__,__LINE__,lv_pkgreg.lv_sts[0]);
			cGetLvzfStat(lv_pkgreg.lv_sts[0],cTx_Desc);
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

/*************************************************
* �� �� ��:  sp5380.c
* ���������� ͨ��ͨ�������ѳ���
*
* ��    ��:  martin
* ������ڣ� 
*
* �޸ļ�¼��
* ��   ��:2007.11.16
* �� �� ��:
* �޸�����:�Ǳ�������������������ʻ���Ϣ���ܲ�ѯ
*************************************************/
#include <stdio.h>
#include <math.h>
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "lv_pkgreg_c.h"
#include "hv_orno_brno_c.h"

int sp5380()
{
	struct lv_pkgreg_c f_lv_pkgreg;
	struct lv_pkgreg_c vlv_pkgreg;
	struct hv_orno_brno_c f_hv_orno_brno;
	
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		long WtDate=0;
		double TmpAmt=0.00;
		FILE *fp;
	char opn_br_no[6];
	char Flag[2];
	char LvSts[5];
	char ChrgInd[5];
	char Orderno[9];
	
	memset( Flag     ,'\0',sizeof(Flag) );
	memset( LvSts    ,'\0',sizeof(LvSts) );
	memset( ChrgInd  ,'\0',sizeof(ChrgInd) );
	memset( Orderno  ,'\0',sizeof(Orderno) );
	memset( &g_pub_tx,  0 ,sizeof(g_pub_tx) );
  memset( wherelist,  0 ,sizeof(wherelist) );
	memset( opn_br_no,'\0',sizeof(opn_br_no) );

	pub_base_sysinit();

	get_zd_data("0030",opn_br_no);
	get_zd_data("0670",Flag);

	/**��ɲ�ѯ����**/
	ret=Hv_orno_brno_Sel(g_pub_tx.reply,&f_hv_orno_brno," br_no='%s' ",opn_br_no );
	TRACE
	
	if( strlen(opn_br_no) )
	{
		if(memcmp(opn_br_no,QS_BR_NO, 5) == 0||memcmp(opn_br_no, KJ_BR_NO, 5) == 0)
		{
		}
		else
		{
			sprintf( tmpstr," or_br_no='%s' and",f_hv_orno_brno.or_br_no );
			strcat( wherelist,tmpstr );
		}
	}
 if( Flag[0]=='0' )
 {
	/**��ɲ�ѯ**/
	strcat(wherelist," lw_ind='1' and pkgno in ('003','004')  and lv_sts in ('5','C') and tx_chrg_ind in ('1','2') ORDER BY wt_date ") ;

	ret=Lv_pkgreg_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Lv_pkgreg_Fet_Sel( &f_lv_pkgreg, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			/**��ʾ�б���**/
			fprintf( fp,"~ί������|Pkg���|�������|����״̬|�������ʺ�|�տ����ʺ�|�����ѱ�־|$�����ѽ��|\n" );
		}
		
	if( f_lv_pkgreg.lv_sts[0] =='5' )
		{
				strcpy( LvSts , "�ܾ�" );		
		}else{
				strcpy( LvSts , "����" );
	  }
	if( f_lv_pkgreg.tx_chrg_ind[0] =='1' )
		{
				strcpy( ChrgInd , "�ֽ�" );		
		}else{
				strcpy( ChrgInd , "ת��" );
	  }
	  
 
	fprintf( fp, "%ld|%s|%s|%s|%s|%s|%s|%.2lf|\n", \ 
		 			f_lv_pkgreg.wt_date,f_lv_pkgreg.pkgno,f_lv_pkgreg.orderno,LvSts,\
		 			f_lv_pkgreg.pay_ac_no,f_lv_pkgreg.cash_ac_no,ChrgInd,f_lv_pkgreg.chrg_amt); 
	 ttlnum++;
	}

	ret=Lv_pkgreg_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}
 }else
 if(Flag[0]=='1')
 {
 	
 	get_zd_data("0280",Orderno);
 	get_zd_long("0440",&WtDate);
 	
 	if( strlen(opn_br_no) )
	{
		if(memcmp(opn_br_no,QS_BR_NO, 5) == 0||memcmp(opn_br_no, KJ_BR_NO, 5) == 0)
		{
			sprintf(acErrMsg,"�û����������˽���!opn_br_no=[%s]",opn_br_no);
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				goto ErrExit;
		}
 	}
 	if( strlen(Orderno) )
 	{
			sprintf( tmpstr," orderno=%s and",Orderno );
			strcat( wherelist,tmpstr );
	}
	if( WtDate!=0 )
 	{
			sprintf( tmpstr," wt_date=%ld and",WtDate );
			strcat( wherelist,tmpstr );
	}
	
 	strcat(wherelist," lw_ind='1' and pkgno in ('003','004')  and lv_sts in ('5','C') and tx_chrg_ind in ('1','2','B','C') ") ;

	ret=Lv_pkgreg_Dec_Upd( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ret=Lv_pkgreg_Fet_Upd(&f_lv_pkgreg,g_pub_tx.reply);
	if(ret)
		{
			vtcp_log("[%s][%d]��ѯ���ݿ����[%d]",__FILE__,__LINE__,ret);
			set_zd_data("0130","��ѯ���ݴ���");
			Lv_pkgreg_Clo_Upd();
			goto ErrExit;
		}
	/** ״̬��� **/
	if( f_lv_pkgreg.tx_chrg_ind[0]=='B' || f_lv_pkgreg.tx_chrg_ind[0]=='C')
		{
			vtcp_log("[%s][%d]�˱��ѳ���tx_chrg_ind==[%s]",__FILE__,__LINE__,f_lv_pkgreg.tx_chrg_ind);
			strcpy( g_pub_tx.reply,"LS69" );
			Lv_pkgreg_Clo_Upd();
			goto ErrExit;
		}
	
	vtcp_log("[%s][%d]�������ݿ�ǰf_lv_pkgreg.tx_chrg_ind==[%s]",__FILE__,__LINE__,f_lv_pkgreg.tx_chrg_ind);
	/** �����ѳ��� **/
	if( f_lv_pkgreg.tx_chrg_ind[0]=='1')
		{
			f_lv_pkgreg.tx_chrg_ind[0]='B';
		}else
		{
			f_lv_pkgreg.tx_chrg_ind[0]='C';
		}
	vtcp_log("[%s][%d]�������ݿ��f_lv_pkgreg.tx_chrg_ind==[%s]",__FILE__,__LINE__,f_lv_pkgreg.tx_chrg_ind);
	
 	ret=Lv_pkgreg_Upd_Upd(f_lv_pkgreg,g_pub_tx.reply);
 	if(ret)
 		{
			vtcp_log("[%s][%d]�������ݿ����[%d]",__FILE__,__LINE__,ret);
			set_zd_data("0130","�������ݿ����");
			Lv_pkgreg_Clo_Upd();
			goto ErrExit;
		}
 	Lv_pkgreg_Clo_Upd();
 	
 	pub_base_old_acno(f_lv_pkgreg.pay_ac_no);
	pub_base_strpack(f_lv_pkgreg.pay_ac_no);
	pub_base_old_acno(f_lv_pkgreg.cash_ac_no);
	pub_base_strpack(f_lv_pkgreg.cash_ac_no);
	if( f_lv_pkgreg.chrg_amt>0.005 )
	 {
	   TmpAmt=-f_lv_pkgreg.chrg_amt;
	   vtcp_log("[%s][%d]�����ѳ�����TmpAmt==[%.2lf]",__FILE__,__LINE__,TmpAmt);
   }
 	
 	/*** �������ݴ��� ***/
 	if( f_lv_pkgreg.tx_chrg_ind[0]=='B')
		{
			set_zd_double("0400",TmpAmt);
			set_zd_data("0670","1");/*** �ֽ� ***/
			goto CashExit;
		}else
	if( f_lv_pkgreg.tx_chrg_ind[0]=='C')
		{
			if( memcmp(f_lv_pkgreg.pkgno,"003",3)==0 )
				{
					set_zd_data("0300",f_lv_pkgreg.pay_ac_no);
				}
				else
			if( memcmp(f_lv_pkgreg.pkgno,"004",3)==0 )
				{
					set_zd_data("0300",f_lv_pkgreg.cash_ac_no);
				}
			set_zd_double("0400",TmpAmt);
			set_zd_data("0670","2");/*** ת�� ***/
			goto TrsfExit;
		}else
		{
			vtcp_log("[%s][%d]���������f_lv_pkgreg.tx_chrg_ind==[%s]",__FILE__,__LINE__,f_lv_pkgreg.tx_chrg_ind);
			goto ErrExit;
		}
 }

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
CashExit:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before Cash return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
TrsfExit:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before Transfers return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
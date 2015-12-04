/*************************************************
* �� �� ��:  sp8507.c
* ���������� �Թ��˻���Ϣ��ѯ(֧�������Ƹ�ͨ��)
*
* ��    ��:  Han Xichao
* ������ڣ� 2011-07-19
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "dd_mst_c.h"

int sp8507()
{
	struct dd_mst_c f_dd_mst;
	struct dd_mst_c z_dd_mst;
	char f_acno1[25];
	char cChnl[5];
	long date_zw;
	int ret = 0;
	double sum_dr_amt = 0.00;
	double sum_cr_amt = 0.00; 

	memset( &g_pub_tx,0x00,sizeof(g_pub_tx) );
	memset(cChnl,0x00,sizeof(cChnl));
	memset(&f_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&z_dd_mst,0x00,sizeof(struct dd_mst_c));

	pub_base_sysinit();
	get_zd_data("0380",f_acno1 );
    	pub_base_old_acno(f_acno1);
	get_zd_long("0430",&date_zw );
	vtcp_log("[%s],[%d],date_zw=[%d]",__FILE__,__LINE__,date_zw);

	pub_base_strpack( f_acno1 );
	
	if ( pub_base_acno_acid(&f_dd_mst.ac_id,f_acno1) )
	{
		goto ErrExit;
	}
	vtcp_log("[%s],[%d],f_acno1=[%s],ac_id=[%ld]",__FILE__,__LINE__,f_acno1,f_dd_mst.ac_id);

	memset(&z_dd_mst,0x00,sizeof(z_dd_mst));
		
	ret=Dd_mst_Sel(g_pub_tx.reply,&z_dd_mst,"ac_id=%ld",f_dd_mst.ac_id);
	ERR_DEAL
	vtcp_log("[%s],[%d],�м�����˻���Ϣ��ѯ!ac_id=[%ld]",__FILE__,__LINE__,f_dd_mst.ac_id);
	set_zd_double("0390",z_dd_mst.bal);
	set_zd_double("0400",z_dd_mst.ys_bal);
	set_zd_data("0720",z_dd_mst.ac_sts);
	set_zd_data("0730",z_dd_mst.hold_sts);

	/**�Ƹ�ͨ�Թ��˻�Ϊ�����˻�,���մ����˻�ͳ��**/
	sum_dr_amt=0.00;
	sum_cr_amt=0.00;
	/**ͳ�Ƶ��ս跽������**/
	ret=sql_sum_double("dd_mst_hst","tx_amt",&sum_dr_amt,"ac_id = %ld and add_ind='0' and tx_date=%ld",f_dd_mst.ac_id,g_pub_tx.tx_date);
	/**ͳ�Ƶ��մ���������**/
	ret=sql_sum_double("dd_mst_hst","tx_amt",&sum_cr_amt,"ac_id = %ld and add_ind='1' and tx_date=%ld",f_dd_mst.ac_id,g_pub_tx.tx_date);
	set_zd_double("0410",sum_dr_amt);	/**���ս跽������**/
	set_zd_double("0420",sum_cr_amt);	/**���մ���������**/
	vtcp_log("%s,%d ��ѯ���!�跽sum_dr_amt=[%.2f],����sum_cr_amt=[%.2f]",__FILE__,__LINE__,sum_dr_amt,sum_cr_amt);
	/***��ѯ�м�����˻����***�ߴ˷�֧**/
	
	set_zd_data( DC_FILE_SND,"0" );
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

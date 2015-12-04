/*************************************************
* �� �� ��:  spB905.c
* ���������� ͬ�ǽ��׸���
*
* ��    ��:  liuxuan 
* ������ڣ� 2006-8-19
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
#include "in_mst_hst_c.h"
#include "accountreg_c.h"
	struct in_mst_hst_c f_in_mst_hst;
	struct in_mst_hst_c f_in_mst_hsta;
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char titstr[1024];
		char tmpstr[512];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		FILE *fp;
struct accountreg_c accountreg;
long trace_no;
char tel[5];

int spB905()
{
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&f_in_mst_hst,0,sizeof(f_in_mst_hst));
	memset(&f_in_mst_hsta,0,sizeof(f_in_mst_hsta));
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset(&accountreg,'\0',sizeof(accountreg));	

	pub_base_sysinit();

	get_zd_data("0910",f_in_mst_hst.tx_br_no );	/* ���׻��� */
	get_zd_long("0290",&f_in_mst_hst.tx_date );
	get_zd_long("0880",&trace_no );	/* ��ˮ�� */
	get_zd_data("0070",tel );	/* ������ */

	ret=get_ac_hst();
	if( ret ) goto ErrExit;

GoodExit:
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
get_ac_hst()
{
	char tmp_no[64],sstat[5];

	/**��ɲ�ѯ����**/

	memset( wherelist,0,sizeof(wherelist)); 
	if( f_in_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date=%ld and",f_in_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( trace_no)
	{
		sprintf( tmpstr," trace_no='%ld' ",trace_no);
		strcat( wherelist,tmpstr );
	}

	/**��ɲ�ѯ**/
	vtcp_log("��ѯ���� wherelist =[%s]",wherelist);
	memset(&accountreg,'\0',sizeof(accountreg));
	ret=Accountreg_Sel( g_pub_tx.reply,&accountreg,wherelist);
	ERR_DEAL

	if(accountreg.stat[0]!='2'){
		vtcp_log("ͬ�ǵǼǲ����״̬����ȷ[%s]",accountreg.stat);
		strcpy( g_pub_tx.reply,"D122" );
		goto ErrExit;
	}
	accountreg.stat[0]='3';
	strcpy(accountreg.chk,tel);

	ret=Accountreg_Upd_Upd( accountreg,g_pub_tx.reply);
	ERR_DEAL

	return 0;
ErrExit:
	return -1;
}

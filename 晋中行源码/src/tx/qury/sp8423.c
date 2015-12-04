/******************************************************************************************
* 文 件 名:  sp8423.c
* 功能描述：ATM 柜员钱箱查询
*
* 作    者:  peter
* 完成日期：
*
* 修改记录：
* 日   期:   20130507
* 修 改 人:  wudawei
* 修改内容:  返回券别的数量,和库存现金/根据电子银行部要求，查询AMT柜员的钱箱 此交易由sp8401.c修改而来
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('8423', 'ATM钱箱查询', '10000000000000000000100000000000000000010000000000000000000000010000010000000000000000000010000000000000000000000000000000000000', '0', '4', '0');

insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('8423', 0, '8423', '#$');

insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('8423', 'ATM钱箱查询', 'sp8423', '0', '0');

******************************************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"
#include "com_tel_c.h"

int sp8423()
{
	struct cash_mst_c vcash_mst;
	struct com_tel_c vcom_tel;
	char filename [100];
	char cBr_no [6];
	char cTel [5];
	char cCur_no [3];
	char cChflag [2]; /****打印标志****/
	double dTotamt = 0.00;
	int ret=0;
	FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( &vcash_mst,0,sizeof(struct cash_mst_c) );
	memset( &vcom_tel,0,sizeof(struct com_tel_c) );
	memset( cBr_no,0,sizeof(cBr_no));
	memset( cTel,0,sizeof(cTel));
	memset( cCur_no,0,sizeof(cCur_no));
	memset( cChflag,0,sizeof(cChflag));

	pub_base_sysinit();

	get_zd_data("0910",cBr_no );
	get_zd_data("0640",cTel );
	get_zd_data("0210",cCur_no );
	get_zd_double("0400",&dTotamt );

	ret = Com_tel_Sel(g_pub_tx.reply,&vcom_tel,"tel = %s and lvl=9",cTel);
	if(ret != 0)
	{
		sprintf(acErrMsg,"输入柜员号错误!此交易只能查询ATM柜员的钱箱 Tel= [%s]\n",cTel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S009" );
		goto ErrExit;
	}

	ret = Cash_mst_Sel(g_pub_tx.reply,&vcash_mst,"br_no = %s and tel = %s",cBr_no,cTel);
	if(ret != 0)
	{
		sprintf(acErrMsg,"柜员钱箱异常! Br_no = [%s] Tel= [%s]\n",cBr_no,cTel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"M035" );
		goto ErrExit;
	}
	if(vcash_mst.sts[0] != '0')
	{
		sprintf(acErrMsg,"柜员钱箱不可用! sts= [%s]\n",vcash_mst.sts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D189" );
		goto ErrExit;
	}


	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"a");
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	pub_base_strpack( vcash_mst.br_no );
	pub_base_strpack( vcash_mst.tel );
	pub_base_strpack( vcash_mst.cur_no );
	pub_base_strpack( vcash_mst.ac_no );
	if(vcash_mst.bal<-2147000000) vcash_mst.bal=0.0;
	if(vcash_mst.ys_bal<-2147000000) vcash_mst.ys_bal=0.0;
	if(vcash_mst.t10000<-2147000000) vcash_mst.t10000=0;
	if(vcash_mst.t5000<-2147000000) vcash_mst.t5000=0;
	if(vcash_mst.t2000<-2147000000) vcash_mst.t2000=0;
	if(vcash_mst.t1000<-2147000000) vcash_mst.t1000=0;
	if(vcash_mst.t500<-2147000000) vcash_mst.t500=0;
	if(vcash_mst.t200<-2147000000) vcash_mst.t200=0;
	if(vcash_mst.t100<-2147000000) vcash_mst.t100=0;
	if(vcash_mst.t50<-2147000000) vcash_mst.t50=0;
	if(vcash_mst.t20<-2147000000) vcash_mst.t20=0;
	if(vcash_mst.t10<-2147000000) vcash_mst.t10=0;
	if(vcash_mst.t5<-2147000000) vcash_mst.t5=0;
	if(vcash_mst.t2<-2147000000) vcash_mst.t2=0;
	if(vcash_mst.t1<-2147000000) vcash_mst.t1=0;
	memcpy(cChflag,"1",sizeof(cChflag)-1);
	fprintf( fp, "QBSL%s|%s|%s|%s|%.2lf|%.2lf|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|\n",vcash_mst.br_no,vcash_mst.tel,vcash_mst.cur_no,vcash_mst.ac_no,vcash_mst.bal,vcash_mst.ys_bal,vcash_mst.t10000,vcash_mst.t5000,vcash_mst.t2000,vcash_mst.t1000,vcash_mst.t500,vcash_mst.t200,vcash_mst.t100,vcash_mst.t50,vcash_mst.t20,vcash_mst.t10,vcash_mst.t5,vcash_mst.t2,vcash_mst.t1);
	
	vtcp_log("[%s][%d] chflag=[%s]\n",__FILE__,__LINE__,cChflag);
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	set_zd_data("0700",cChflag); /****返回打印标志****/
	{
		char tmp[52];
		get_zd_data("0700",tmp);
		vtcp_log("FD70 [%s][%d] tmp=[%s]\n",__FILE__,__LINE__,tmp);
	}

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

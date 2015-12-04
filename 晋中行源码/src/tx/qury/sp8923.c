/**************************************************************
* 文 件 名: sp8923.c
* 功能描述：可用资金管理状态表查询
* 数据库表：zjgl_mst
*
* 作    者: jack
* 完成日期：2004年3月28日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error %d " , ret); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "zjgl_mst_c.h"

sp8923()
{
	int ret = 0;
	char flag[2];        
	char wherelist[128];    /***查询条件***/

	char cOpn_br_no[6];   /***开户机构***/
	char cCur_no   [3];   /***币种***/
	char cAcc_no   [8];   /***科目号***/
	char tmpstr[128];
	char cBr_no[6];
	char cFlag[2];
	struct zjgl_mst_c zjgl_mst;  /*可用资金管理状态表*/
  int  ttlnum=0;
  FILE *fp=NULL;
  char filename[100];
  char cStat[7];
  char cCurName[11];
  char cIntsttype[20];
	double tmp_lmt=0.00; 
  memset(cIntsttype,0x0,sizeof(cIntsttype));
  memset(cCurName,0x0,sizeof(cCurName));
  memset(cStat,0x0,sizeof(cStat));
  memset(filename,0x0,sizeof(filename));
	memset(wherelist,0,sizeof(wherelist));
  memset(tmpstr,0x0,sizeof(tmpstr));
	memset(&zjgl_mst,0x0,sizeof(zjgl_mst));
  memset(cFlag,0x0,sizeof(cFlag));
	memset(cOpn_br_no,0x0,sizeof(cOpn_br_no));
	memset(cCur_no,0x0,sizeof(cCur_no));
	memset(cAcc_no,0x0,sizeof(cAcc_no));
	if(pub_base_sysinit())
  {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
  }
	/*-------- 取输入变量  ---------*/

	get_zd_data("0910",cOpn_br_no);
	get_zd_data("0210",cCur_no);
	get_zd_data("0650",cAcc_no);
	get_zd_data("0670",cFlag);
	get_zd_data("0030",cBr_no);
	pub_base_strpack_all(cBr_no);
if(cFlag[0]!='1')			/* 前台固定写的1 */
{
		memset(&zjgl_mst,0x0,sizeof(zjgl_mst));
		ret=Zjgl_mst_Sel( g_pub_tx.reply,&zjgl_mst,"opn_br_no='%s' and cur_no='%s' and acc_no='%s'",cOpn_br_no,cCur_no,cAcc_no);
		ERR_DEAL
		set_zd_data("0250",zjgl_mst.name);
		set_zd_long("0460",zjgl_mst.opn_date);
		set_zd_long("0450",zjgl_mst.lim_beg_date);
		set_zd_long("0440",zjgl_mst.lim_end_date);
		set_zd_data("0220",zjgl_mst.lim_term);
		tmp_lmt=0.00;
		if(zjgl_mst.lim_amt < 0.001)
			tmp_lmt=-zjgl_mst.lim_amt;
		else
			tmp_lmt=zjgl_mst.lim_amt;	
			
		set_zd_double("0400",tmp_lmt);
		set_zd_data("0530",zjgl_mst.intst_knd);
		set_zd_double("0840",zjgl_mst.rate);
		set_zd_double("0850",zjgl_mst.over_rate);
}
else
{
	pub_base_strpack_all(cOpn_br_no);
	if(strlen(cOpn_br_no))
	{
	 
	 sprintf(tmpstr,"opn_br_no='%s' and ",cOpn_br_no);
	 strcat(wherelist,tmpstr);
	}
	pub_base_strpack_all(cCur_no);
	if(strlen(cCur_no))
	{
	 sprintf(tmpstr,"cur_no='%s' and ",cCur_no);
	 strcat(wherelist,tmpstr);
	}
	pub_base_strpack_all(cAcc_no);
	if(strlen(cAcc_no))
	{
		sprintf(tmpstr,"acc_no='%s' and ",cAcc_no);
	 	strcat(wherelist,tmpstr);
	}
	if(memcmp(cBr_no,QS_BR_NO, 5)!=0)
	{
		sprintf(tmpstr,"opn_br_no='%s' and ",cBr_no);
	 	strcat(wherelist,tmpstr);	
	}
	strcat(wherelist,"1=1 order by opn_br_no,acc_no");
	vtcp_log( "WHERE [%s]",wherelist);
	ret=Zjgl_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
	ttlnum=0;
	while(1)
	{
		memset(&zjgl_mst,0x00,sizeof(zjgl_mst));
		memset(cStat,0x00,sizeof(cStat));
		memset(cCurName,0x00,sizeof(cCurName));
    memset(cIntsttype,0x00,sizeof(cIntsttype));
		ret=Zjgl_mst_Fet_Sel(&zjgl_mst, g_pub_tx.reply );
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
			fprintf( fp,"~机构|科目号|名称|借方余额|贷方余额|昨日借方余额|昨日贷方余额|帐户状态|开户日期|起息日期|计息类型|利率|逾期利率|交易限额|累计交易额|限额周期|\n" );
		}
		if(zjgl_mst.sts[0]=='1')
		{
			memcpy(cStat,"正常",4);
		}
		else
		{
			memcpy(cStat,"不正常",6);
		}
		pub_base_dic_show_str(cCurName,"cur_name",zjgl_mst.cur_no);
		pub_base_dic_show_str(cIntsttype,"intst_knd",zjgl_mst.intst_knd);

	if(0 == strcmp("4070203", zjgl_mst.acc_no)){
		double _407amt=0;
		if(ret = _4070203_avbal(&zjgl_mst, &_407amt))
		goto ErrExit;
			vtcp_log("%d@%s  _407amt=[%.2lf]\n", __LINE__, __FILE__, _407amt);
		zjgl_mst.ys_cr_bal=zjgl_mst.cr_bal;
		zjgl_mst.ys_dr_bal=zjgl_mst.dr_bal;
		zjgl_mst.dr_bal+=_407amt;
	}
		fprintf( fp, "%.5s|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%s|%ld|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%s|\n",zjgl_mst.opn_br_no,zjgl_mst.acc_no,zjgl_mst.name,zjgl_mst.dr_bal,zjgl_mst.cr_bal,zjgl_mst.ys_dr_bal,zjgl_mst.ys_cr_bal,cStat,zjgl_mst.opn_date,zjgl_mst.ic_date,cIntsttype,zjgl_mst.rate,zjgl_mst.over_rate,zjgl_mst.lim_amt,zjgl_mst.lim_bal,zjgl_mst.lim_term);	
		ttlnum++;
	}
	ret=Zjgl_mst_Clo_Sel( );
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


int _4070203_avbal(struct zjgl_mst_c *i_zjgl, double *avl){

	int ret=0;

	double ddep_dbal = 0, ddep_cbal = 0, dcln_dbal = 0, dcln_cbal = 0, dsy_dbal = 0, dsy_cbal = 0, dqy_dbal = 0, dqy_cbal = 0;		
	double  dzbj1_bal = 0, dzbj6_bal=0;

   /** 存款借方合计 **/       /*20150917 增加203科目*/
   ret = sql_sum_double( "dc_log","amt",&ddep_dbal,\
   "dc_ind = '1' and tx_opn_br_no='%s' and cur_no='%s' and sts='0' and substr(acc_hrt,1,3) in ('201','202','203','205','211','215','217','221','222','223',\
   '231','232','233','234','235','236','241','242','243','244','251','252','255','258','260',\
   '261','262','263','264','265','266','267','268','272','280')", \
   i_zjgl->opn_br_no,i_zjgl->cur_no);
   ERR_DEAL

	vtcp_log("%d@%s  ddep_dbal[%.2lf]\n", __LINE__, __FILE__, ddep_dbal);

   /** 存款贷方合计 **/       /*20150917 增加203科目*/
   ret = sql_sum_double( "dc_log","amt",&ddep_cbal,\
   "dc_ind = '2' and tx_opn_br_no='%s' and cur_no='%s' and sts='0' and substr(acc_hrt, 1, 3) in ('201','202','203','205','211','215','217','221','222','223',\
   '231','232','233','234','235','236','241','242','243','244','251','252','255','258','260',\
   '261','262','263','264','265','266','267','268','272','280')", \
   i_zjgl->opn_br_no,i_zjgl->cur_no);
   ERR_DEAL
   
	vtcp_log("%d@%s  ddep_cbal[%.2lf]\n", __LINE__, __FILE__, ddep_cbal);
   /** 贷款合计 **/
   ret = sql_sum_double( "dc_log","amt",&dcln_dbal,\
   " dc_ind = '1' and tx_opn_br_no='%s' and cur_no='%s' and sts='0' and substr(acc_hrt, 1, 3) in ('101','115','120','121','122','123','124','125','126','127','128','129',\
   '130','131','132','133','134','135','136','137','138','139','140','141','142','143',\
   '145','149','151','152','153','154','161','162','163','190')",
    i_zjgl->opn_br_no,i_zjgl->cur_no);
   ERR_DEAL
	vtcp_log("%d@%s  (++ dcln_dbal[%.2lf] ++)\n", __LINE__, __FILE__, dcln_dbal);
   
   /** 贷款合计 **/
   ret = sql_sum_double( "dc_log","amt",&dcln_cbal,\
   " dc_ind = '2' and tx_opn_br_no='%s' and cur_no='%s' and sts='0' and substr(acc_hrt, 1, 3) in ('101','115','120','121','122','123','124','125','126','127','128','129',\
   '130','131','132','133','134','135','136','137','138','139','140','141','142','143',\
   '145','149','151','152','153','154','161','162','163','190')",
    i_zjgl->opn_br_no,i_zjgl->cur_no);
   ERR_DEAL
	vtcp_log("%d@%s  (++ dcln_cbal[%.2lf] ++)\n", __LINE__, __FILE__, dcln_cbal);

   /** 损益类合计 **/
   ret = sql_sum_double( "dc_log","amt",&dsy_dbal,\
   "dc_ind = '1' and  tx_opn_br_no='%s' and cur_no='%s' and sts='0' and substr(acc_hrt,1,1)='5'",\
		i_zjgl->opn_br_no,i_zjgl->cur_no);
   ERR_DEAL
	vtcp_log("$$$SY$$$ ++ %d@%s  dsy_dbal[%.2lf]\n", __LINE__, __FILE__, dsy_dbal);
   
   /** 损益类合计 **/
   ret = sql_sum_double( "dc_log","amt",&dsy_cbal,\
   "dc_ind = '2' and  tx_opn_br_no='%s' and cur_no='%s' and sts='0' and substr(acc_hrt,1,1)='5'",\
    	i_zjgl->opn_br_no,i_zjgl->cur_no);
   ERR_DEAL
   
	vtcp_log("$$$SY$$$ ++ %d@%s  dsy_cbal[%.2lf]\n", __LINE__, __FILE__, dsy_cbal);
   /** 所有者权益合计 **/
   ret = sql_sum_double( "dc_log","amt",&dqy_dbal,\
   "dc_ind = '1' and  tx_opn_br_no='%s' and cur_no='%s' and sts='0' and substr(acc_hrt,1,1)='3'" ,\
		i_zjgl->opn_br_no,i_zjgl->cur_no);
   ERR_DEAL   	

	vtcp_log("%d@%s  dqy_dbal[%.2lf]\n", __LINE__, __FILE__, dqy_dbal);
   /** 所有者权益合计 **/
   ret = sql_sum_double( "dc_log","amt",&dqy_cbal,\
   "dc_ind = '2' and  tx_opn_br_no='%s' and cur_no='%s' and sts='0' and substr(acc_hrt,1,1)='3'",\
    	i_zjgl->opn_br_no,i_zjgl->cur_no);
   ERR_DEAL   	


	vtcp_log("%d@%s  dqy_cbal[%.2lf]\n", __LINE__, __FILE__, dqy_cbal);
   /** 所有者权益合计 **/

/* 这些先不加了，太复杂计算起来
   ret = sql_sum_double( "ass_mst","bal",&dzbj1_bal,\
   "ic_date = %ld and br_no='%s' and sts<>'*' and sign='1' ", g_pub_tx.tx_date, i_zjgl->opn_br_no);
   ERR_DEAL
	vtcp_log("%d@%s  dzbj1_bal[%.2lf]\n", __LINE__, __FILE__, dzbj1_bal);

   ret = sql_sum_double( "ass_mst","bal",&dzbj6_bal,\
   "ic_date = %ld and br_no='%s' and sts<>'*' and sign='2' ", g_pub_tx.tx_date, i_zjgl->opn_br_no);
   ERR_DEAL
	vtcp_log("%d@%s  dzbj6_bal[%.2lf]\n", __LINE__, __FILE__, dzbj6_bal);
*/
	   /** 4070203 **/
    *avl = (ddep_cbal-ddep_dbal)-(dcln_dbal-dcln_cbal)+(dsy_cbal-dsy_dbal) +(dqy_cbal - dqy_dbal) +dzbj1_bal;
  /*                资产                -负债                +损益                 +所有者     */ 

	vtcp_log("%d@%s  ddep_cbal[%.2lf]\n", __LINE__, __FILE__, ddep_cbal);
    vtcp_log("LINE %d  FILE %s >> *avl[%.2lf] = (ddep_cbal[%.2lf]-ddep_dbal[%.2lf])-(dcln_dbal[%.2lf]-dcln_cbal[%.2lf])+(dsy_cbal[%.2lf]-dsy_dbal[%.2lf]) +(dqy_cbal[%.2lf] - dqy_dbal[%.2lf]) +dzbj1_bal[%.2lf] -dzbj6_bal[%.2lf]\n", __LINE__, __FILE__, *avl, ddep_cbal, ddep_dbal, dcln_dbal, dcln_cbal, dsy_cbal, dsy_dbal, dqy_cbal, dqy_dbal, dzbj1_bal, dzbj6_bal);
	ErrExit:
		strcpy( g_pub_tx.reply,"E099" );
		return ret;	
}

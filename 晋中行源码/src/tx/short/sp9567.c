/*****************************************************
* 文 件 名:  sp9567.c
* 功能描述： 返回当前记录中没有被回执的记录
* 作    者:  Liuyue 
* 完成日期： 2006年9月21日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
* 注    意: 
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
int sp9567()
{
	int ret = 0;
	LV_FD123_AREA fd123;
	struct lv_pkgreg_c   lv_pkgreg;
	char cPkgno[4];
	char cBrno[BRNO_LEN+1];
	char cBuff[2001];
	char cFilename[101];
	int  iPkgno = 0;



	memset(&fd123		,0x00	,sizeof(fd123));
	memset(&lv_pkgreg	,0x00	,sizeof(lv_pkgreg));
	memset(cPkgno		,0x00	,sizeof(cPkgno)-1);
	memset(cBrno		,0x00	,sizeof(cBrno));
	memset(cBuff		,0x00	,sizeof(cBuff));
	memset(cFilename	,0x00	,sizeof(cFilename));
	
	
	memcpy(cBrno   ,lvca.brno, sizeof(cBrno)-1);
	zip_space(cBrno);
	/*
	get_zd_data("0510",cPkgno);
	*/
	get_zd_data("0030",cBrno);
    get_fd_data("1230",(char*)&fd123);

	memcpy(cPkgno,fd123.pkgno,sizeof(fd123.pkgno));

	
	iPkgno = apatoi(cPkgno,sizeof(cPkgno)-1);
	
	sprintf(cPkgno,"%03d",iPkgno);

	vtcp_log("[%s][%d],cPKgno===[%s]\n",__FILE__,__LINE__,cPkgno);

	
	if (!memcmp(cPkgno,PKGNO_SSJJHZ,sizeof(cPkgno)-1))
	{
		vtcp_log("[%s][%d]这里是实时贷记业务cPkgno=[%s]",__FILE__,__LINE__,cPkgno);
		ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply,"pkgno = '%s' and lw_ind = '%c' and br_no = '%s' and  lv_sts = '%c' ORDER BY WT_DATE,ORDERNO ", PKGNO_SSDJ, "2", cBrno, 'I');

	}


	else if (!memcmp(cPkgno,PKGNO_SSDJHZ,sizeof(cPkgno)-1))
	{

		vtcp_log("[%s][%d]这里是实时借记业务cPkgno=[%s]",__FILE__,__LINE__,cPkgno);
		ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and lw_ind = '2' and br_no = '%s' and lv_sts = '%c' ORDER BY WT_DATE,ORDERNO ", PKGNO_SSJJ, cBrno, 'I');

	}


	else if (!memcmp(cPkgno,PKGNO_PTJJHZ,sizeof(cPkgno)-1))
	{

		vtcp_log("[%s][%d]这里是普通借记业务cPkgno=[%s]",__FILE__,__LINE__,cPkgno);

		ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and lw_ind = '2' and  br_no = '%s' and lv_sts = 'H' and orderno>='%.8s'  ORDER BY WT_DATE,ORDERNO ", PKGNO_PTJJ, cBrno ,fd123.orderno);

	}


	else if (!memcmp(cPkgno,PKGNO_DQJJHZ,sizeof(cPkgno)-1))
	{

		vtcp_log("[%s][%d]这里是定期借记业务cPkgno=[%s]",__FILE__,__LINE__,cPkgno);
		ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply,  "pkgno = '%s' and lw_ind = '2' and br_no = '%s'  and lv_sts = 'H' and orderno >='%.8s'  ORDER BY WT_DATE,ORDERNO  ", PKGNO_DQJJ, cBrno,fd123.orderno );

	}


	vtcp_log("[%s][%d]开始取记录!\n",__FILE__,__LINE__);


	ret = Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);	/*在这里只取第一条就可以了*/
	vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
	if(ret&&ret!=100)
	{
		vtcp_log("[%s][%d]数据库操作异常!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	else if(ret&&ret==100)
	{
		vtcp_log("[%s][%d]说明了已经不存在等待回执的记录了!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"9999");
		goto ErrExit1;
	}

	vtcp_log("[%s][%d]记录存在,取其第一条记录!\n",__FILE__,__LINE__);

	vtcp_log("[%s][%d]开始准备输出数据!\n",__FILE__,__LINE__);

	/**PKGNO**/
	memcpy(fd123.pkgno      ,lv_pkgreg.pkgno,sizeof(fd123.pkgno));
	/**付款人帐号**/
	memcpy(fd123.pay_ac_no,lv_pkgreg.pay_ac_no,sizeof(fd123.pay_ac_no));
	vtcp_log("[%s][%d]lv_pkgreg.pay_ac_no=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_ac_no);
	/**付款人姓名**/
	memcpy(fd123.pay_name,lv_pkgreg.pay_name,sizeof(fd123.pay_name));
	vtcp_log("[%s][%d]lv_pkgreg.pay_name=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_name);
	/**付款人地址**/
	memcpy(fd123.pay_addr,lv_pkgreg.pay_addr,sizeof(fd123.pay_addr));
	/**付款人开户行**/
	memcpy(fd123.pay_opn_br_no,lv_pkgreg.pay_opn_br_no, sizeof(fd123.pay_opn_br_no));
	/**收款人帐号**/
	memcpy(fd123.cash_ac_no ,lv_pkgreg.cash_ac_no, sizeof(fd123.cash_ac_no));
	vtcp_log("[%s][%d]lv_pkgreg.cash_ac_no=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_ac_no);
	/**收款人名称**/
	memcpy(fd123.cash_name,lv_pkgreg.cash_name,sizeof(fd123.cash_name));
	vtcp_log("[%s][%d]lv_pkgreg.cash_name=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_name);
	/**收款人地址**/
	memcpy(fd123.cash_addr,lv_pkgreg.cash_addr,sizeof(fd123.cash_addr));
	  /**收款人开户行**/
	memcpy(fd123.cash_opn_br_no,lv_pkgreg.cash_opn_br_no, sizeof(fd123.cash_opn_br_no));
	/**发起行号 **/
	memcpy(fd123.or_br_no,lv_pkgreg.or_br_no,sizeof(fd123.or_br_no));
	/**付款清算行行号**/
	memcpy(fd123.pay_qs_no,lv_pkgreg.pay_qs_no,sizeof(fd123.pay_qs_no));
	/**接受清算行号**/
	memcpy(fd123.cash_qs_no,lv_pkgreg.cash_qs_no, sizeof(fd123.cash_qs_no));
	/**接受行号**/
	memcpy(fd123.ac_br_no   ,lv_pkgreg.ac_br_no,sizeof(fd123.ac_br_no));
	/**委托日期**/
	sprintf(fd123.wt_date,"%08d",lv_pkgreg.wt_date);
	/**交易序号**/
	memcpy(fd123.orderno    ,lv_pkgreg.orderno,sizeof(fd123.orderno));
	/**业务类型编号**/
	memcpy(fd123.txnum      ,lv_pkgreg.txnum,sizeof(fd123.txnum));
	/**业务种类**/
	memcpy(fd123.yw_type    ,lv_pkgreg.lv_yw_ind,sizeof(fd123.yw_type));
	memcpy(fd123.yw_type    ,lv_pkgreg.lv_yw_ind,sizeof(fd123.yw_type));
	/**应答状态**/
	memcpy(fd123.rcp_sts    ,lv_pkgreg.rcpstat,sizeof(fd123.rcp_sts));
	/**附言**/
	memcpy(fd123.brf        ,lv_pkgreg.lv_brf,sizeof(fd123.brf));
	/**原交易序号**/
	memcpy(fd123.o_orderno,lv_pkgreg.o_orderno,sizeof(fd123.o_orderno));
	/**原交易金额**/
	dot_dbl2str(lv_pkgreg.tx_amt,sizeof(fd123.tx_amt) ,2,fd123.tx_amt);
	/**包序号**/
	memcpy(fd123.pack_id    ,lv_pkgreg.packid,sizeof(fd123.pack_id));
	/**业务类型编号**/
	apitoa(lv_pkgreg.pack_date,sizeof(fd123.pack_date),fd123.pack_date);
	/**支付交易状态**/
	fd123.lv_sts[0] = lv_pkgreg.lv_sts[0];


	vtcp_log("[%s][%d]取数据完毕!程序准备结束!\n",__FILE__,__LINE__);


	Lv_pkgreg_Clo_Sel();


	 if(memcmp(lv_pkgreg.txnum,TXNUM_QTJJ,5)!=0 && memcmp(lv_pkgreg.txnum,TXNUM_DQJJ,5)!=0)
	 {
		FILE *fpw;
		iHvGetAddit(fd123.wt_date,fd123.wt_date,lv_pkgreg.addid,"000",cBuff,sizeof(cBuff)-1);
		vtcp_log("%s,%d 得到的附加域=[%s]",__FILE__,__LINE__,cBuff);

		ret=pub_base_AllDwnFilName(cFilename);
		if(ret)
		{
			 vtcp_log("%s,%d 生成下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
		}
		
		fpw=fopen(cFilename,"w");
		if(fpw==NULL)
		{
			vtcp_log("%s,%d 打开下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
		}
	
		if(memcmp(lv_pkgreg.txnum,TXNUM_GKJJ,sizeof(lv_pkgreg.txnum)-1)==0)
		{
			GKDJ_ADD_AREA lv_gkdj;
			memset(&lv_gkdj, 0 , sizeof(lv_gkdj));
			memcpy((char *)&lv_gkdj,cBuff, sizeof(lv_gkdj));
			vtcp_log("%s,%d 写入附加域文件=[%s]",__FL__,(char *)&lv_gkdj);

			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_gkdj,sizeof(lv_gkdj));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		if(memcmp(lv_pkgreg.txnum,TXNUM_ZJZF,sizeof(lv_pkgreg.txnum)-1)==0 || memcmp(lv_pkgreg.txnum,TXNUM_SQZF,sizeof(lv_pkgreg.txnum)-1)==0 )
		{
				ZJZF_ADD_AREA lv_zjzf;
				memset(&lv_zjzf, 0 , sizeof(lv_zjzf));
				memcpy((char *)&lv_zjzf,cBuff, sizeof(lv_zjzf));
				vtcp_log("%s,%d 写入附加域文件=[%s]",__FL__,(char *)&lv_zjzf);
				memset(cBuff, 0 , sizeof(cBuff));
				memcpy(cBuff,(char *)&lv_zjzf,sizeof(lv_zjzf));
				fprintf(fpw,"%s",cBuff);
				set_zd_data( DC_FILE_SND,"2" );
			}
			if(memcmp(lv_pkgreg.txnum,TXNUM_GZJJ,sizeof(lv_pkgreg.txnum)-1)==0)
			{
				GZDJ_ADD_AREA lv_gzdj;
				memset(&lv_gzdj, 0 , sizeof(lv_gzdj));
				memcpy((char *)&lv_gzdj,cBuff, sizeof(lv_gzdj));

			        vtcp_log("%s,%d 写入附加域文件=[%s]",__FL__,(char *)&lv_gzdj);
				memset(cBuff, 0 , sizeof(cBuff));
				memcpy(cBuff,(char *)&lv_gzdj,sizeof(lv_gzdj));
				fprintf(fpw,"%s",cBuff);
                        	set_zd_data( DC_FILE_SND,"2" );
                	}
			if(memcmp(lv_pkgreg.txnum,TXNUM_PLKS,sizeof(lv_pkgreg.txnum)-1)==0)
			{
				SSKS_ADD_AREA lv_ssks;
				memset(&lv_ssks, 0 , sizeof(lv_ssks));
				memcpy((char *)&lv_ssks,cBuff, sizeof(lv_ssks));
				vtcp_log("%s,%d 写入附加域文件=[%s]",__FL__,(char *)&lv_ssks);
				memset(cBuff, 0 , sizeof(cBuff));
				memcpy(cBuff,(char *)&lv_ssks,sizeof(lv_ssks));
				fprintf(fpw,"%s",cBuff);
				set_zd_data( DC_FILE_SND,"2" );
			}
			if(memcmp(lv_pkgreg.txnum,TXNUM_ZPJL,sizeof(lv_pkgreg.txnum)-1)==0)
			{
				ZPJL_ADD_AREA lv_zpjl;
				memset(&lv_zpjl, 0 , sizeof(lv_zpjl));
				memcpy((char *)&lv_zpjl,cBuff, sizeof(lv_zpjl));

				vtcp_log("%s,%d 写入附加域文件=[%s]",__FL__,(char *)&lv_zpjl);
				memset(cBuff, 0 , sizeof(cBuff));
				memcpy(cBuff,(char *)&lv_zpjl,sizeof(lv_zpjl));
                        	fprintf(fpw,"%s",cBuff);
                        	set_zd_data( DC_FILE_SND,"2" );
                	}
			fclose(fpw);
		}

		pub_base_full_space((char*)&fd123,sizeof(fd123));
	 	set_zd_data("1230",(char*)&fd123);
		vtcp_log("fd123=[%s]",(char*)&fd123);
		
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
ErrExit1:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}



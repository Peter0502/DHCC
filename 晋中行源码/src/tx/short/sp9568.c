/*****************************************************
* 文 件 名:  sp9568.c
* 功能描述： 小额支付交易基本信息
* 作    者:  xyy 
* 完成日期： 2006-9-14 17:58 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "public.h"
#include "lvnet.h"
#include "lv_pub.h"
#include "lv_pkgreg_c.h"
#include "lv_pldfr_c.h" /*add by chenggx 2011-6-14*/
#include "lv_pldsr_c.h" /*add by chenggx 2011-6-14*/
int sp9568()
{
	int ret=0,i;
	double dTmp1=0.0;
	struct lv_pkgreg_c  lv_pkgreg;
	struct lv_pldfr_c  lv_pldfr;
	struct lv_pldsr_c  lv_pldsr;
	char cKinbr[BRNO_LEN+1];
	char cBuff[2001];
	char cDate[9];
	char cFilename[1000];
	memset(cFilename, 0 , sizeof(cFilename));
	LV_FD123_AREA fd123;
	memset(cBuff, 0 , sizeof(cBuff));
	memset(cDate,0,sizeof(cDate));
	memset(&fd123, ' ',sizeof(fd123));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
	memset(&lv_pldfr,0,sizeof(lv_pldfr));
	memset(&lv_pldsr,0,sizeof(lv_pldsr));
	memset(cKinbr , 0 , sizeof(cKinbr));
	vtcp_log("sp9568 开始！[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();

	get_zd_data("0030",cKinbr);
	get_fd_data("1230",(char*)&fd123);
	vtcp_log("[%s,%d]1230=[%d]",__FILE__,__LINE__,sizeof(fd123));
	vtcp_log("[%s,%d]1230-----=[%s]",__FILE__,__LINE__,(char*)&fd123);

	/******获取三个索引的值*******/
	vtcp_log("[%s][%d]fd123.wt_date===[%.8s]\n",__FILE__,__LINE__,fd123.wt_date);
	vtcp_log("[%s][%d]fd123.brfe===[%s]\n",__FILE__,__LINE__,fd123.brf);
	lv_pkgreg.wt_date = apatoi(fd123.wt_date,8);
	memcpy(lv_pkgreg.orderno,fd123.orderno,sizeof(lv_pkgreg.orderno)-1);
	memcpy(lv_pkgreg.or_br_no,fd123.or_br_no,sizeof(lv_pkgreg.or_br_no)-1);
        /***add by NEWBP2***/
        lv_pkgreg.lw_ind[0] = fd123.lw_ind[0];
        /***end by NEWBP2***/
	vtcp_log("+++++lv_pkgreg.or_br_no==[%s]!+++++",lv_pkgreg.or_br_no);
	zip_space(lv_pkgreg.or_br_no);
	if(strlen(lv_pkgreg.or_br_no)==0)
	{
		GetOr_br_noByBr_no(cKinbr, lv_pkgreg.or_br_no );
	}
	
	vtcp_log("[%s][%d]根据这里锁定小额支付表:wt_date=[%ld],or_br_no=[%s],orderno=[%s]"\
	,__FILE__,__LINE__,lv_pkgreg.wt_date,lv_pkgreg.or_br_no,lv_pkgreg.orderno);
	
	vtcp_log("sp9568 开始！[%s][%d]",__FILE__,__LINE__);

	ret = Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, "orderno='%s' and or_br_no='%s' \
	and wt_date=%ld and lw_ind='%c' ", lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date,lv_pkgreg.lw_ind[0]); /**modify by NEWBP2**/
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此小额支付交易!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "9999" );
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询小额支付交易基本信息出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
  vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
  memcpy(fd123.or_br_no        ,lv_pkgreg.or_br_no        ,sizeof(lv_pkgreg.or_br_no   )-1);
	vtcp_log("[%s][%d]fd123.or_br_no=[%s] ",__FL__,fd123.or_br_no );
	memcpy(fd123.pkgno       ,lv_pkgreg.pkgno       ,sizeof(lv_pkgreg.pkgno  )-1);   
	vtcp_log("[%s][%d]fd123.pkgno=[%s] ",__FL__,fd123.pkgno );
	memcpy(fd123.pay_qs_no       ,lv_pkgreg.pay_qs_no       ,sizeof(lv_pkgreg.pay_qs_no  )-1);   
	vtcp_log("[%s][%d]fd123.pay_qs_no=[%s] ",__FL__,fd123.pay_qs_no );
	memcpy(fd123.pay_ac_no       ,lv_pkgreg.pay_ac_no       ,sizeof(lv_pkgreg.pay_ac_no  )-1);   
	vtcp_log("[%s][%d]fd123.pay_ac_no=[%s] ",__FL__,fd123.pay_ac_no );
	memcpy(fd123.pay_name        ,lv_pkgreg.pay_name        ,sizeof(lv_pkgreg.pay_name   )-1);
	vtcp_log("[%s][%d]fd123.pay_name=[%s] ",__FL__,fd123.pay_name );
	memcpy(fd123.pay_addr        ,lv_pkgreg.pay_addr        ,sizeof(lv_pkgreg.pay_addr   )-1);
	vtcp_log("[%s][%d]fd123.pay_addr=[%s] ",__FL__,fd123.pay_addr );
	memcpy(fd123.ac_br_no        ,lv_pkgreg.ac_br_no        ,sizeof(lv_pkgreg.ac_br_no   )-1);
	vtcp_log("[%s][%d]fd123.ac_br_no=[%s] ",__FL__,fd123.ac_br_no );
	memcpy(fd123.cash_ac_no      ,lv_pkgreg.cash_ac_no      ,sizeof(lv_pkgreg.cash_ac_no  )-1);
	vtcp_log("[%s][%d]fd123.cash_ac_no=[%s] ",__FL__,fd123.cash_ac_no );
	memcpy(fd123.cash_qs_no      ,lv_pkgreg.cash_qs_no      ,sizeof(lv_pkgreg.cash_qs_no  )-1);
	vtcp_log("[%s][%d]fd123.cash_qs_no=[%s] ",__FL__,fd123.cash_qs_no );
	memcpy(fd123.cash_name       ,lv_pkgreg.cash_name       ,sizeof(lv_pkgreg.cash_name   )-1);
	vtcp_log("[%s][%d]fd123.cash_name=[%s] ",__FL__,fd123.cash_name );
	memcpy(fd123.cash_addr       ,lv_pkgreg.cash_addr       ,sizeof(lv_pkgreg.cash_addr   )-1);
	vtcp_log("[%s][%d]fd123.cash_addr=[%s] ",__FL__,fd123.cash_addr );                 
	memcpy(fd123.orderno         ,lv_pkgreg.orderno         ,sizeof(lv_pkgreg.orderno     )-1);
	vtcp_log("[%s][%d]fd123.orderno=[%s] ",__FL__,fd123.orderno );                    
	memcpy(fd123.o_orderno       ,lv_pkgreg.o_orderno       ,sizeof(lv_pkgreg.o_orderno   )-1);
	vtcp_log("[%s][%d]fd123.o_orderno=[%s] ",__FL__,fd123.o_orderno );
	memcpy(fd123.pay_opn_br_no   ,lv_pkgreg.pay_opn_br_no   ,sizeof(lv_pkgreg.pay_opn_br_no)-1);
	vtcp_log("[%s][%d]fd123.pay_opn_br_no=[%s] ",__FL__,fd123.pay_opn_br_no );
	memcpy(fd123.cash_opn_br_no  ,lv_pkgreg.cash_opn_br_no ,sizeof(lv_pkgreg.cash_opn_br_no)-1);
	vtcp_log("[%s][%d]fd123.cash_opn_br_no=[%s] ",__FL__,fd123.cash_opn_br_no );
	memcpy(fd123.txnum           ,lv_pkgreg.txnum           ,sizeof(lv_pkgreg.txnum       )-1);
	vtcp_log("[%s][%d]fd123.txnum=[%s] ",__FL__,fd123.txnum );
	memcpy(fd123.yw_type           ,lv_pkgreg.lv_yw_ind     ,sizeof(lv_pkgreg.lv_yw_ind   )-1);
	vtcp_log("[%s][%d]fd123.yw_type=[%s] ",__FL__,fd123.yw_type );
	memcpy(fd123.brf           ,lv_pkgreg.lv_brf     ,sizeof(lv_pkgreg.lv_brf       )-1);
	vtcp_log("[%s][%d]fd123.brf=[%s] ",__FL__,fd123.brf );
  memcpy(fd123.rcp_sts	,lv_pkgreg.rcpstat,sizeof(fd123.rcp_sts));/**应答状态**/
  memcpy(fd123.ret_code	,lv_pkgreg.retcode,sizeof(fd123.ret_code));/**应答状态**/
  memcpy(fd123.pack_id,lv_pkgreg.packid,sizeof(fd123.pack_id));/**原包号**/
  memcpy(fd123.o_or_br_no,lv_pkgreg.o_or_br_no,sizeof(fd123.o_or_br_no));
  vtcp_log("[%s][%d]fd123.o_or_br_no=[%s] ",__FL__,fd123.o_or_br_no );
  memcpy(fd123.o_ac_br_no,lv_pkgreg.o_ac_br_no,sizeof(fd123.o_ac_br_no));
  vtcp_log("[%s][%d]fd123.o_ac_br_no=[%s] ",__FL__,fd123.o_ac_br_no );
  apitoa(lv_pkgreg.pack_date,sizeof(fd123.pack_date),fd123.pack_date);
  apitoa(lv_pkgreg.o_wt_date,sizeof(fd123.o_wt_date),fd123.o_wt_date);/**原包日期**/
  vtcp_log("[%s][%d]lv_pkgreg.o_pack_date=[%d] ",__FL__,lv_pkgreg.o_pack_date);
  vtcp_log("[%s][%d]fd123.pack_id=[%s] ",__FL__,fd123.pack_id );
  vtcp_log("[%s][%d]fd123.rcp_sts=[%s] ",__FL__,fd123.rcp_sts );
	fd123.chrg_ind[0]  = lv_pkgreg.tx_chrg_ind[0];
	fd123.lw_ind[0] = lv_pkgreg.lw_ind[0];
	fd123.lv_sts[0] = lv_pkgreg.lv_sts[0];
	vtcp_log("[%s][%d]fd123.chrg_ind=[%c] ",__FL__,fd123.chrg_ind[0] );	
	vtcp_log("[%s][%d]fd123.lw_ind=[%c] ",__FL__,fd123.lw_ind[0] );	
	vtcp_log("[%s][%d]fd123.lv_sts=[%c] ",__FL__,fd123.lv_sts[0] );	
	vtcp_log("[%s][%d]fd123.lv_pkgreg.tx_amt*100=[%f]",__FILE__,__LINE__,lv_pkgreg.tx_amt*100);
	/* rem by LiuHuafeng 20060915
	dot_dbl2str(lv_pkgreg.tx_amt*100,sizeof(fd123.tx_amt) ,0,fd123.tx_amt);
	*******************************/
	{
	dTmp1=0.0;
	dTmp1=lv_pkgreg.tx_amt*100.0;
	dbl2str(&dTmp1,0,sizeof(fd123.tx_amt),0,fd123.tx_amt);
	}
	vtcp_log("[%s][%d]fd123.tx_amt=[%s]",__FILE__,__LINE__,fd123.tx_amt);
	vtcp_log("[%s][%d]lv_pkgreg.tx_amt=[%f]",__FILE__,__LINE__,lv_pkgreg.tx_amt);
	vtcp_log("[%s][%d]fd123.chrg_ind=[%c] ",__FL__,fd123.chrg_ind[0] );
	apitoa(lv_pkgreg.o_wt_date, sizeof(fd123.o_wt_date),fd123.o_wt_date);
	apitoa(lv_pkgreg.wt_date,sizeof(fd123.wt_date),fd123.wt_date);
	vtcp_log("[%s][%d]fd123.o_wt_date=[%s] ",__FL__,fd123.o_wt_date );
	vtcp_log("[%s,%d]1230=[%d]",__FILE__,__LINE__,sizeof(fd123));
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	vtcp_log("[%s,%d]1230=[%d]",__FILE__,__LINE__,sizeof(fd123));
	fd123.buff[sizeof(fd123)-1]='\0';
	
	/*beg add by chenggx 2011-6-14 20:25:55*/
	vtcp_log("[%s][%d]pkgno=[%s],txnum=[%s]",__FILE__,__LINE__,lv_pkgreg.pkgno,lv_pkgreg.txnum);
	vtcp_log("[%s][%d]PKGNO_TYXX=[%s],TXNUM_DF=[%s],TXNUM_DS=[%s]",__FILE__,__LINE__,PKGNO_TYXX,TXNUM_DF,TXNUM_DS);
	if(memcmp(lv_pkgreg.pkgno,PKGNO_TYXX,3)==0 && memcmp(lv_pkgreg.txnum,TXNUM_DF,5)==0){/*代付*/
		vtcp_log("[%s][%d] 代付 我看到了 哈哈哈 ^_^O(∩_∩)O哈哈~",__FILE__,__LINE__);
		ret = Lv_pldfr_Sel(g_pub_tx.reply,&lv_pldfr, " packid='%s' and orderno='%s' and wt_date=%ld and tx_mx_no='00000000' ",lv_pkgreg.packid,lv_pkgreg.orderno,lv_pkgreg.wt_date);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"查询批量代付表失败!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "9999" );
			goto	ErrExit;
		}
		memcpy(fd123.pay_ac_no       ,lv_pldfr.pay_ac_no       ,strlen(lv_pldfr.pay_ac_no  ));   
		vtcp_log("[%s][%d]fd123.pay_ac_no=[%s] ",__FL__,fd123.pay_ac_no );
		memcpy(fd123.pay_name        ,lv_pldfr.pay_name        ,strlen(lv_pldfr.pay_name   ));
		vtcp_log("[%s][%d]fd123.pay_name=[%s] ",__FL__,fd123.pay_name );
		dTmp1=0.0;
		dTmp1=lv_pldfr.totamt*100.0;
		dbl2str(&dTmp1,0,sizeof(fd123.tx_amt),0,fd123.tx_amt);
	}else if(memcmp(lv_pkgreg.pkgno,PKGNO_TYXX,3)==0 && memcmp(lv_pkgreg.txnum,TXNUM_DS,5)==0){/*代收*/
		vtcp_log("[%s][%d] 代收 我看到了 哈哈哈 ^_^O(∩_∩)O哈哈~",__FILE__,__LINE__);
		ret = Lv_pldsr_Sel(g_pub_tx.reply,&lv_pldsr, " packid='%s' and orderno='%s' and wt_date=%ld and tx_mx_no='00000000' ",lv_pkgreg.packid,lv_pkgreg.orderno,lv_pkgreg.wt_date);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"查询批量代收表失败!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "9999" );
			goto	ErrExit;
		}
		memcpy(fd123.cash_ac_no      ,lv_pldsr.cash_ac_no      ,strlen(lv_pldsr.cash_ac_no  ));
		vtcp_log("[%s][%d]fd123.cash_ac_no=[%s] ",__FL__,fd123.cash_ac_no );
		memcpy(fd123.cash_name       ,lv_pldsr.cash_name       ,strlen(lv_pldsr.cash_name   ));
		vtcp_log("[%s][%d]fd123.cash_name=[%s] ",__FL__,fd123.cash_name );
		dTmp1=0.0;
		dTmp1=lv_pldsr.totamt*100.0;
		dbl2str(&dTmp1,0,sizeof(fd123.tx_amt),0,fd123.tx_amt);
	}
	/*end add by chenggx 2011-6-14 20:25:55*/
	
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("[%s,%d]1230=[%d]",__FILE__,__LINE__,sizeof(fd123));
	vtcp_log("&fd123==[%s]\n",&fd123);
	
	if(memcmp(lv_pkgreg.txnum,TXNUM_QTJJ,5)!=0 && memcmp(lv_pkgreg.txnum,TXNUM_DQJJ,5)!=0 && \
     memcmp(lv_pkgreg.txnum,TXNUM_DQDJ,5)!=0 && memcmp(lv_pkgreg.txnum,TXNUM_DJTH,5)!=0 && \
     memcmp(lv_pkgreg.txnum,TXNUM_QTDJ,5)!=0 && memcmp(lv_pkgreg.txnum,TXNUM_JS  ,5)!=0 && \
     memcmp(lv_pkgreg.txnum,TXNUM_HD  ,5)!=0 )
   {
		FILE *fpw;
		PAY_IN_AREA payin;
		memset(&payin,'\0',sizeof(payin));
		vtcp_log("%s,%d 得到的附加域=[%s]",__FILE__,__LINE__,cBuff);
		memcpy(payin.T_addid,lv_pkgreg.addid,sizeof(lv_pkgreg.addid)-1);
		apitoa(lv_pkgreg.wt_date,sizeof(payin.F_wtday),payin.F_wtday);
		iLvGetAddit(&payin,"000",cBuff,0);
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

		if(!memcmp(lv_pkgreg.txnum,TXNUM_WT,sizeof(lv_pkgreg.txnum)-1))
		{
			WTSK_ADD_AREA lv_wtsk;
			memset(&lv_wtsk, 0 , sizeof(lv_wtsk));
			memcpy((char *)&lv_wtsk,cBuff, sizeof(lv_wtsk));
			vtcp_log("%s,%d 写入附加域文件=[%s]",__FL__,(char *)&lv_wtsk);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_wtsk,sizeof(lv_wtsk));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		if(!memcmp(lv_pkgreg.txnum,TXNUM_TSCF,sizeof(lv_pkgreg.txnum)-1))
		{
			LVTSCF_ADD_AREA lv_tscf;
			memset(&lv_tscf, 0 , sizeof(lv_tscf));
			memcpy((char *)&lv_tscf,cBuff, sizeof(lv_tscf));
			vtcp_log("%s,%d 写入附加域文件=[%s]",__FL__,(char *)&lv_tscf);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_tscf,sizeof(lv_tscf));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		if(memcmp(lv_pkgreg.txnum,TXNUM_GKDJ,sizeof(lv_pkgreg.txnum)-1)==0 || \
	     memcmp(lv_pkgreg.txnum,TXNUM_GKJJ,sizeof(lv_pkgreg.txnum)-1)==0)
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
    if(memcmp(lv_pkgreg.txnum,TXNUM_GZDJ,sizeof(lv_pkgreg.txnum)-1)==0 || \
	     memcmp(lv_pkgreg.txnum,TXNUM_GZJJ,sizeof(lv_pkgreg.txnum)-1)==0)
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
		if(memcmp(lv_pkgreg.txnum,TXNUM_ZJZF,sizeof(lv_pkgreg.txnum)-1)==0 || \
	     memcmp(lv_pkgreg.txnum,TXNUM_SQZF,sizeof(lv_pkgreg.txnum)-1)==0 || \
	     memcmp(lv_pkgreg.txnum,TXNUM_ZJTH,sizeof(lv_pkgreg.txnum)-1)==0 || \
	     memcmp(lv_pkgreg.txnum,TXNUM_SQTH,sizeof(lv_pkgreg.txnum)-1)==0)
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
		if(memcmp(lv_pkgreg.txnum,TXNUM_TC  ,sizeof(lv_pkgreg.txnum)-1)==0 || \
	     memcmp(lv_pkgreg.txnum,TXNUM_TD  ,sizeof(lv_pkgreg.txnum)-1)==0 || \
	     memcmp(lv_pkgreg.txnum,TXNUM_DGTD,sizeof(lv_pkgreg.txnum)-1)==0)
		{
			TCTD_ADD_AREA lv_tctd;
			memset(&lv_tctd, 0 , sizeof(lv_tctd));
			memcpy((char *)&lv_tctd,cBuff, sizeof(lv_tctd));
			vtcp_log("%s,%d 写入附加域文件=[%s]",__FL__,(char *)&lv_tctd);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_tctd,sizeof(lv_tctd));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		if(memcmp(lv_pkgreg.txnum,TXNUM_SSKS,sizeof(lv_pkgreg.txnum)-1)==0 || \
	     memcmp(lv_pkgreg.txnum,TXNUM_PLKS,sizeof(lv_pkgreg.txnum)-1)==0)
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
		if(memcmp(lv_pkgreg.txnum,TXNUM_FSSR,sizeof(lv_pkgreg.txnum)-1)==0)
		{
			FSSR_ADD_AREA lv_fssr;
			memset(&lv_fssr, 0 , sizeof(lv_fssr));
			memcpy((char *)&lv_fssr,cBuff, sizeof(lv_fssr));
			vtcp_log("%s,%d 写入附加域文件=[%s]",__FL__,(char *)&lv_fssr);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_fssr,sizeof(lv_fssr));
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
		if(memcmp(lv_pkgreg.txnum,TXNUM_JF,sizeof(lv_pkgreg.txnum)-1)==0)
		{
			JF_ADD_AREA lv_jf;
			memset(&lv_jf, 0 , sizeof(lv_jf));
			memcpy((char *)&lv_jf,cBuff, sizeof(lv_jf));
			vtcp_log("%s,%d 写入附加域文件=[%s]",__FL__,(char *)&lv_jf);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_jf,sizeof(lv_jf));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		fclose(fpw);
		
		}
		
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
}

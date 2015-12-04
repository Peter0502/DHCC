/***************************************************************
* 文 件 名:     spJ020.c
* 功能描述：    大额退回应答		
*
* 作    者:   LiuYue
* 完成日期：  2006-8-26  17:27
*
* 修改记录：  改动流程
* 日    期:   20060922
* 修 改 人:   liuyue
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hv_define.h"
#include "hvpack.h"
#include "find_debug.h"

spJ020()
{
	int ret = 0;
	HV_NOTPAY_AREA hv_notpayin;
        struct	hv_zf_c        hv_zf;
	struct  hv_fzf_c       hv_fzf;
	HV_FD123_AREA          fd123;
	THVPACKAGE   pack314;

	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&hv_zf,	      0x00 ,  sizeof(hv_zf));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(&fd123,	      0x00 ,  sizeof(fd123));
  	memset(&pack314, 0 , sizeof(pack314));

	/*********************
	 * 1 .初始化全局变量 *
   	 *********************/
	pub_base_sysinit();

	/*********************
     	* 2. 获取8583域内容  *
     	**********************/
	ret=iHvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
	print_notpay(&hv_notpayin,'F');
	vtcp_log("[%s][%d!!!!!!!!!\n",__FILE__,__LINE__);

	
	if(!memcmp(hv_notpayin.F_or_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_notpayin.F_or_br_no)))
	{
	vtcp_log("[%s][%d!!!!!!!!!\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"接受行号和发起行号不能一致!\n");
		WRITEMSG
		vtcp_log("[%s][%d]orbrno=[%.12s] acbrno=[%.12s]\n",__FILE__,__LINE__,hv_notpayin.F_or_br_no,hv_notpayin.F_ac_br_no);
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	/**********************
         * 4 . 数据库操作     *
	 **********************/

 	
	MEMCPY_DEBUG(hv_fzf.cmtno,hv_notpayin.F_o_cmtno,sizeof(hv_fzf.cmtno)-1);
	MEMCPY_DEBUG(hv_fzf.orderno,hv_notpayin.F_orderno,sizeof(hv_fzf.orderno));
	hv_fzf.wt_date = apatoi(hv_notpayin.F_o_fzf_date,sizeof(hv_notpayin.F_o_fzf_date));
	MEMCPY_DEBUG(hv_fzf.or_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_fzf.or_br_no)-1);
	

	vtcp_log("[%s][%d]开始查找非支付交易的记录!\n",__FILE__,__LINE__); 

        ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, "cmtno='%s' and orderno='%s' and or_br_no='%s' and wt_date=%ld ", hv_fzf.cmtno,hv_fzf.orderno,hv_fzf.or_br_no,hv_fzf.wt_date);
        if(ret)
        {
                sprintf(acErrMsg,"操作数据库错误!ret=[%d]\n",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"D103");
                goto ErrExit;
        }

        vtcp_log("[%s][%d]",__FILE__,__LINE__);

        ret = Hv_fzf_Fet_Upd(&hv_fzf,g_pub_tx.reply);
        if ( ret==100 )
        {
		vtcp_log("[%s][%d]原非支付记录不存在!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P019");
		goto ErrExit;
	}

        ret = iGetHvOrderno(hv_notpayin.F_orderno);
        if(ret)
         {
                sprintf(acErrMsg,"生成新的交易序号错误!ret=[%d]\n",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P019");
                goto ErrExit;
          }

	hv_fzf.res_date = apatoi(hv_notpayin.F_wt_date,sizeof(hv_notpayin.F_wt_date));
	MEMCPY_DEBUG(hv_fzf.res_br_no,hv_notpayin.F_or_br_no,sizeof(hv_fzf.res_br_no)-1);
	MEMCPY_DEBUG(hv_fzf.res_content,hv_notpayin.F_content,sizeof(hv_notpayin.F_content));
	MEMCPY_DEBUG(hv_fzf.res_orderno,hv_notpayin.F_orderno,sizeof(hv_fzf.res_orderno)-1);
	hv_fzf.hv_fzf_sts[0] = RECEIVE_BACK_ACCT;
	hv_fzf.res_sts[0]= hv_notpayin.F_respcode[0];


	ret = Hv_fzf_Upd_Upd(hv_fzf,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"更新原非支付交易记录错误!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;	
	}
	vtcp_log("fzf......o_cmtno=[%3s],o_ordenro=[%8s],o_or_br_no=[%12s],wt_date=[%lf]",hv_fzf.o_cmtno,hv_fzf.o_orderno,hv_fzf.o_or_br_no,hv_fzf.o_tx_date);
	vtcp_log("查询非支付记录成功! [%s][%d]",__FILE__,__LINE__);
        Hv_fzf_Clo_Sel();


	/**********插入历史明细表***********/
	ret = iHvfFrontToTable(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数F->T错误!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P015");
		goto ErrExit;
        }
	ret = dg_hv_fzfhtr(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"插入非支付明细表错误!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P011");
		goto ErrExit;
	}

	/*******************
         * 5    写流水     *
         *******************/	
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}

	/********************
	 * 6.准备返回数据   *
	 ********************/

	MEMCPY_DEBUG(fd123.orderno,hv_notpayin.F_orderno,sizeof(fd123.orderno));
	pub_base_full_space((char *)&fd123,sizeof(fd123));
        set_zd_data("1230",(char *)&fd123);





	/********************
	 * 7. 准备发送报文  *
	 ********************/

	/**********读取表里面的内容到T字段中****/
	vtcp_log("hv_fzf:or_br_no=[%s],cmtno=[%s],wt_date=[%ld],orderno=[%s]\n",hv_fzf.or_br_no,hv_fzf.cmtno,hv_fzf.wt_date,hv_fzf.orderno);
	MEMCPY_DEBUG(hv_notpayin.F_or_br_no,hv_fzf.or_br_no,sizeof(hv_fzf.or_br_no)-1);
	sprintf(hv_notpayin.F_wt_date,"%8d", hv_fzf.wt_date);
	MEMCPY_DEBUG(hv_notpayin.F_orderno, hv_fzf.orderno,sizeof(hv_fzf.orderno)-1);
	MEMCPY_DEBUG(hv_notpayin.F_cmtno,    hv_fzf.cmtno,sizeof(hv_fzf.cmtno)-1);

	ret = iHvTable2Notpayin(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数iHvTable2Notpayin 错误!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	iGetHvRefno(hv_notpayin.T_or_br_no,hv_notpayin.T_refno);
	vtcp_log("[%s][%d]开始拼包发送了!\n",__FILE__,__LINE__);
  	/****Add by SSH,2006.8.28,直接发送****/
	pub_com_CMT314_T2TAG(&hv_notpayin,&pack314);
	ret=hv_gen_pack(&pack314);
	if(ret<0){
		sprintf(acErrMsg,"组织大额支付报文错误!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack314,hv_notpayin.F_tx_br_no,hv_notpayin.F_tel\
			,hv_notpayin.T_trace_no,hv_notpayin.T_orderno)<0){
		sprintf(acErrMsg,"插入发送登记簿失败!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack314)){
		sprintf(acErrMsg,"发送314报文到前置机失败!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]发送成功!\n",__FILE__,__LINE__);
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

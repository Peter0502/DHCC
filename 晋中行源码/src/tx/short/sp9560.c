/*****************************************************
* 文 件 名:  sp9560.c
* 功能描述： 根据汇票号查询汇票基本信息
* 作    者:  ChenMing
* 完成日期： 2006年8月8日 
* 修改记录： 
* 日    期:
* 修 改 人:  liuyue
* 修改内容:  增加了返回内容  也增加了查询条件
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "hv_brno_orno_c.h"
#include "draw_loan_hst_c.h"

int sp9560()
{
	int ret=0;
	struct hv_poinfo_c hv_poinfo;    
	struct hv_brno_orno_c hv_brno_orno;
	struct draw_loan_hst_c m_draw_loan_hst;
	char cFilename[1000];

	HV_FD123_AREA fd123;

	memset(&m_draw_loan_hst,0x00,sizeof(m_draw_loan_hst));
	memset(&hv_poinfo,'\0',sizeof(hv_poinfo));
	memset(&hv_brno_orno,0x00,sizeof(hv_brno_orno));
	memset(&fd123,0x00,sizeof(fd123));
	memset(cFilename, 0 , sizeof(cFilename));
	vtcp_log("sp9560 开始！[%s][%d]",__FILE__,__LINE__);

	pub_base_sysinit();

	/****获取FD123域中的内容*****/
	get_fd_data("1230",(char *)&fd123);

	vtcp_log("[%s][%d]从前台获取到签发行和汇票号码!\n",__FILE__,__LINE__);	

	memcpy(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));
	memcpy(hv_poinfo.pay_br_no,fd123.pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	zip_space(hv_poinfo.pay_br_no);
	/*****这里实在想不出什么办法了,因为52域是整数类型的,即使不给值也会自动
	给上一个0,没有办法就用2来代替本应该的0吧********/
	vtcp_log("hv_poinfo.len==[%d]\n",strlen(hv_poinfo.pay_br_no));

	if (hv_poinfo.pay_br_no[0]==0x00)/****如果签发行为空,说明是本行****/
	{
		vtcp_log("[%s][%d]根据交易机构号查找交易行号!\n",__FILE__,__LINE__);
		vtcp_log("++++++g_pub_tx.tx_br_no==[%s]!+++++\n",g_pub_tx.tx_br_no);
		ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'  ",g_pub_tx.tx_br_no);
		if (ret)
		{
			sprintf(acErrMsg,"根据机构号找交易机构错误!\n");
			WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		memcpy(hv_poinfo.pay_br_no,hv_brno_orno.or_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	}
	vtcp_log("hv_poinfo.po_no=[%s]  [%s][%d]",hv_poinfo.po_no,__FILE__,__LINE__);
	vtcp_log("hv_poinfo.pay_br_no[%s]  [%s][%d]",hv_poinfo.pay_br_no,__FILE__,__LINE__);

	if ( strlen(hv_poinfo.po_no) == 0 ) 
	{
		sprintf(acErrMsg,"汇票号错误[%d]",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"P003");
		goto ErrExit;
	}
	if ( strlen(hv_poinfo.pay_br_no) == 0 ) 
	{
		sprintf(acErrMsg,"请输入行号\n");
		WRITEMSG
			strcpy(g_pub_tx.reply,"P003");
		goto ErrExit;
	}
	ret = Hv_poinfo_Sel(g_pub_tx.reply,&hv_poinfo, "po_no='%s' and pay_br_no='%s' ",hv_poinfo.po_no ,hv_poinfo.pay_br_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此汇票号");
		WRITEMSG
		/* strcpy( g_pub_tx.reply, "MBFE" ); */
			goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询汇票基本信息出错[%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	
	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
	set_zd_data("0030",hv_poinfo.br_no);       		/*机构号码*/
	set_zd_data("0670",hv_poinfo.bank_type);   		/*行别标志*/
	set_zd_data("0680",hv_poinfo.po_sts);   		/*汇票状态*/
	set_zd_data("0720",hv_poinfo.deal_mode);		/**转出类型**/
	set_zd_long("0450",hv_poinfo.reg_date); 		/*登记日期*/
	set_zd_long("0460",hv_poinfo.sign_date);   		/*签发日期*/
	set_zd_double("0390",hv_poinfo.po_bal);      	/*出票金额*/
	set_zd_data("0920",hv_poinfo.op_tel);      	/*录入柜员*/

	fd123.reqtype[0]=hv_poinfo.deal_mode[0]; 
	memcpy(fd123.or_br_no,hv_poinfo.or_br_no,sizeof(fd123.or_br_no)); 
	/*发起行行号*/ 
	memcpy(fd123.pay_qs_no,hv_poinfo.pay_qs_no,sizeof(fd123.pay_qs_no)); 
	/*付款清算行***/ 
	memcpy(fd123.pay_opn_br_no,hv_poinfo.pay_br_no,sizeof(fd123.pay_opn_br_no)); /***签发行*/ 
	memcpy(fd123.po_no,hv_poinfo.po_no,sizeof(fd123.po_no)); /***票据号码******/ 
	fd123.po_type[0] = hv_poinfo.po_ind[0]; /***汇票种类***/ 
	if (!memcmp(hv_poinfo.cur_id,"RMB",3)) set_zd_data("0210","01"); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /***币种***/ 
	memcpy(fd123.pay_ac_no,hv_poinfo.pay_ac_no,sizeof(fd123.pay_ac_no)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*付款帐号***/ 
	memcpy(fd123.pay_name,hv_poinfo.pay_name,sizeof(fd123.pay_name)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*付款人*/ 
	memcpy(fd123.pay_addr,hv_poinfo.pay_addr,sizeof(fd123.pay_addr)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*付款地址*/ 
	memcpy(fd123.cash_ac_no,hv_poinfo.cash_ac_no,sizeof(fd123.cash_ac_no)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*收款帐号*/ 
	memcpy(fd123.cash_name ,hv_poinfo.cash_name,sizeof(fd123.cash_name)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*收款人*/ 
	memcpy(fd123.cash_addr,hv_poinfo.cash_addr,sizeof(fd123.cash_addr)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*付款地址*/ 
	memcpy(fd123.cash_opn_br_no,hv_poinfo.cash_br_no,sizeof(fd123.cash_opn_br_no)); 
	vtcp_log(" [%s][%d] cashbrno=[%s]",__FILE__,__LINE__,hv_poinfo.cash_br_no); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /***兑付行号***/ 
	memcpy(fd123.brf,hv_poinfo.brf,sizeof(fd123.brf)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /***摘要****/

	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]fd123==[%s]\n",(char *)&fd123);
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	set_zd_data("1230",(char *)&fd123);/**返回123域**/
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	
	/*add by gong 20110803 晋中商行 因信贷系统更改*/
	
	ret=Draw_loan_hst_Sel(g_pub_tx.reply,&m_draw_loan_hst,"brf1 like 'YH________%s' ",hv_poinfo.po_no);
	if(ret){
		if(ret==100){
			set_zd_data("0830","");
			sprintf(acErrMsg,"没找到啊po_no=[%s]",hv_poinfo.po_no);
			WRITEMSG
		}else{
			sprintf(acErrMsg,"查询信贷系统审批号码出错[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	set_zd_data("0830",m_draw_loan_hst.draw_no);
	/*end by gong 20110803 晋中商行 因信贷系统更改*/
	OkExit:
		strcpy( g_pub_tx.reply, "0000" );
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);	
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	/* begin add by LiuHuafeng 2006-9-6 15:48 */
	{
		FILE *fpw;		
		/* 写入文件传回去 */
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
		if(memcmp(fd123.cmtno,"121",3)==0)
		{
			HV_121_ADD wd_add;
			memset(&wd_add, 0 , sizeof(wd_add));
			memcpy(wd_add.spec_br_no,hv_poinfo.cash_br_no,sizeof(wd_add.spec_br_no));
			vtcp_log("%s,%d cash_br_no=[%s]",__FILE__,__LINE__,hv_poinfo.cash_br_no);
			apitoa(hv_poinfo.reg_date,sizeof(wd_add.pj_date),wd_add.pj_date);
			memcpy(wd_add.pj_num,hv_poinfo.po_no,sizeof(wd_add.pj_num));
			wd_add.hp_type[0]=hv_poinfo.po_ind[0];
			memcpy(wd_add.hp_miya,hv_poinfo.pwd,sizeof(wd_add.hp_miya));
			pub_base_full_space((char*)&wd_add,sizeof(wd_add));
			vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&wd_add);
			fprintf(fpw,"%s",(char*)&wd_add);
			set_zd_data( DC_FILE_SND,"2" );
		}
		
		if(memcmp(fd123.cmtno,"724",3)==0)
		{
			double dPobal=0.0;
			HV_724_ADD wd_add;
			memset(&wd_add, 0 , sizeof(wd_add));
			dPobal=hv_poinfo.po_bal*100;

			apitoa(hv_poinfo.reg_date,sizeof(wd_add.pj_date),wd_add.pj_date);
			memcpy(wd_add.pj_num,hv_poinfo.po_no,sizeof(wd_add.pj_num));
			wd_add.hp_type[0]=hv_poinfo.po_ind[0];
			memcpy(wd_add.df_br_no,hv_poinfo.cash_br_no,sizeof(wd_add.df_br_no));
			dPobal=hv_poinfo.po_bal*100;
			dbl2str(&dPobal,0,sizeof(wd_add.cp_amt),0,wd_add.cp_amt);
			memcpy(wd_add.hp_miya,hv_poinfo.pwd,sizeof(wd_add.hp_miya));
			memcpy(wd_add.pay_opn_br_no,hv_poinfo.or_br_no,sizeof(wd_add.pay_opn_br_no));
			vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&wd_add);
			pub_base_full_space((char*)&wd_add,sizeof(wd_add));
			fprintf(fpw,"%s",(char*)&wd_add);
			set_zd_data( DC_FILE_SND,"2" );
		}
		fclose(fpw);
	}
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


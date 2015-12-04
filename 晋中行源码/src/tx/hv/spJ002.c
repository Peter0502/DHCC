/***************************************************************
* 文 件 名:     spJ002.c
* 功能描述：	大额支付业务复合
*
* 作    者:   LiuHuafeng
* 完成日期：  2006-8-9 9:28
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "svrpub_c.h"
#include <fcntl.h>
#include "find_debug.h"
#include "mdm_ac_rel_c.h"

spJ002()
{
	int ret = 0;
	char cErrmsg[51];
	char cKinbr[BRNO_LEN+1];
	memset(cKinbr, 0 ,sizeof(cKinbr));
	HV_PAYIN_AREA hv_payin;
	char cAc_no[25];
	memset(cAc_no, 0 , sizeof(cAc_no));
	
	/***定义凭证变量add by martin start***/
	char cVtype[4];    /*凭证类型*/
	char cVocnum[17];  /*凭证号码*/
	char cAcno[25];
	struct mdm_ac_rel_c s_mdm_ac_rel;
	memset(cAcno, 0 , sizeof(cAcno));
	memset(&s_mdm_ac_rel, 0 , sizeof(s_mdm_ac_rel));
	/***定义凭证变量add by martin end***/
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(cVtype     ,0, sizeof(cVtype));
	memset(cVocnum    ,0, sizeof(cVocnum));
	memset(&hv_payin, 0 , sizeof(hv_payin));
	memset(cErrmsg, 0 , sizeof(cErrmsg));
	get_zd_data(DC_TX_BR_NO,cKinbr);
	/***得到大额支付业务8583数据***/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	TRACE
	ret=FetchRecToPayin(&hv_payin);
	if(ret)
	{
		WRITEMSG
		goto ErrExit;
	}
	TRACE
	vtcp_log("%s,%dhvpayin===[%s]\n",__FILE__,__LINE__,(char*)&hv_payin);
	ret=iHvCheckInput(&hv_payin,cErrmsg);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		sprintf(acErrMsg,"%s",cErrmsg);
		WRITEMSG
		goto ErrExit;
	}
	ret=iHv_checkstat(hv_payin.F_cmtno,hv_payin.F_yw_type);
	if(ret)
	{
		set_zd_data("0130",cErrmsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d hv_payin.T_refno************=[%s]\n",__FILE__,__LINE__,hv_payin.T_refno);
	iHvResetPayin(&hv_payin);
	/* 准备写入登记簿的数据 */
	ret=iHvFrontToTable(&hv_payin);
	if( ret )
	{
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		vtcp_log("%s %d 检查记录状态错误 ",__FILE__,__LINE__);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d hv_payin.T_refno************=[%s]\n",__FILE__,__LINE__,hv_payin.T_refno);
	/* 写入登记簿 */
	vtcp_log("%s,%d iHvUpdZfRecFromPayin 参数是0\n",__FILE__,__LINE__);
	vtcp_log("%s,%d cmtno =[%.3s][%.3s]",__FILE__,__LINE__,hv_payin.F_cmtno,hv_payin.T_cmtno);
	ret=iHvUpdZfRecFromPayin(&hv_payin,0);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d hv_payin.T_refno************=[%s]\n",__FILE__,__LINE__,hv_payin.T_refno);
	/* 写入业务流水 */
	ret=pub_ins_trace_log();
	if(ret)
	{
		strcpy( g_pub_tx.reply, "HV01");
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	/** 凭证处理开始 20071108 by martin **/
	get_zd_data("0900",cVtype);
  get_zd_data("0590",cVocnum);
  get_zd_data("0590",cVocnum);
  get_zd_data("0330",cAcno);
  vtcp_log("大额业务帐号id!!!cAcno[%s][%s][%d]\n",cAcno,__FILE__,__LINE__);
  vtcp_log("大额业务处理机构g_pub_tx.tx_br_no==[%s]HV_CZ_QSBRNO==[%s][%s][%d]\n",g_pub_tx.tx_br_no,HV_CZ_QSBRNO,__FILE__,__LINE__);
  /*** 按清算要求清算中心凭证要下账 20130114 wudw
  if( memcmp( g_pub_tx.tx_br_no,HV_CZ_QSBRNO,sizeof(g_pub_tx.tx_br_no)-1 ) )
  {
  *****/
  vtcp_log("大额业务处理机构hv_payin.F_cmtno==[%s]cVtype[%s][%s][%d]\n",hv_payin.F_cmtno,cVtype,__FILE__,__LINE__);
	  if ( (memcmp(hv_payin.F_cmtno,"100",3)==0)&&(memcmp(cVtype,"005",3)==0) )
	  {
	  	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no='%s'",cAcno);
	  	if(ret)
	  	{
	  		sprintf(acErrMsg,"帐号查询出错!");
	  		WRITEMSG
	  		goto ErrExit;
	  	}
          	vtcp_log("开始凭证处理 type[%s][%s][%s][%d]\n",cVtype,cVocnum,__FILE__,__LINE__);
	  	ret=pub_com_NoteUse(s_mdm_ac_rel.ac_id,s_mdm_ac_rel.ac_seqn,cVtype,cVocnum,cVocnum,g_pub_tx.tel);
	  	if(ret)
	  	{
	  		sprintf(acErrMsg,"处理凭证出错!");
	  		WRITEMSG
	  		goto ErrExit;
	  	}
	  }
	 /*** 按清算要求清算中心凭证要下账 20130114 wudw
	}
	****end 20130114 ***/
	/** 凭证处理结束 20071108 by martin **/
	/*** 输出变量 ***/
	if(!memcmp(hv_payin.F_cmtno,HV_CJ,3))
	{
			ret=pub_base_GetParm_Str(cKinbr,PARM_HVCJ,cAc_no);
			if(ret)
			{
				sprintf(acErrMsg,"得到本机构大额清算帐号错误!\n");
				WRITEMSG
				vtcp_log("%s,%d 得到本机构大额清算帐号错误\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P073");
				goto ErrExit;
			}
			set_zd_data("0300",cAc_no);
			set_zd_data("0660","1");/*借贷标志 */
			set_zd_data("0680","6");/*是否根据币种得到序号*/
		/* 拆借的时候准备拆借帐户 */
	}
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("112A",g_pub_tx.ac_no);

			ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
			if(ret)
			{
				sprintf(acErrMsg,"得到本机构大额清算帐号错误!\n");
				WRITEMSG
				vtcp_log("%s,%d 得到本机构大额清算帐号错误\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P073");
				goto ErrExit;
			}
			set_zd_data("0310",cAc_no);
			set_zd_data("0990","2");
			
			
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



/**************打印出8583域的内容***********/
int print_8583_area()
{
	char cFilename[150];
	FILE *fp;
	int i_fp;
	long length;
	char fmt[10];
	char cTmplist[1024];
	memset(fmt,0x00,sizeof(fmt));
	
	memset(cFilename,0x00,sizeof(cFilename));
	memset(cTmplist,0x00,sizeof(cTmplist));
	
	
	int recordnum = 0;
	
	int index = 0;
	
	recordnum = sql_count ( "data_dic" , "%s" , "1=1" );
	sql_sum_long( "data_dic","data_leng",&length,"%s","1=1" );
	
	sprintf(cFilename,"%s/%s",(char *) getenv("HOME"),"8583.txt");
	vtcp_log("[%s][%d]文件名字=[%s]\n",__FILE__,__LINE__,cFilename);
	
	vtcp_log("[%s][%d]8585域中一共有[%d]个数据编码!总的8585域的长度=[%ld]\n",__FILE__,__LINE__,recordnum,length);
	
	i_fp = open(cFilename,O_RDWR|O_CREAT|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
	if(i_fp==-1)
	{
		vtcp_log("[%s][%d]打开或者创建文件失败!fp==[%d]\n",__FILE__,__LINE__,i_fp);
		
		return (-1);
	}
	
	close(i_fp);
	
	length = 1;
	fp = fopen(cFilename,"a+");
	if(fp==NULL)
		vtcp_log("[%s][%d]打开文件失败!\n",__FILE__,__LINE__);
	for(index=0;index<recordnum;index++)
	{
		memset(cTmplist,0x00,sizeof(cTmplist));
		memset(fmt,	0x00,sizeof(fmt));
			
		sprintf(fmt,"%%-%ds",Pu_Val[index].length);
		sprintf(cTmplist,fmt,Pu_Val[index].val);
		fprintf(fp,"---[%d]-----[%d]----------------------\n",length,length+Pu_Val[index].length);
		fprintf(fp,"数据编码=[%s]\n",Pu_Val[index].sjbm);
		fprintf(fp,"数据长度=[%d]\n",Pu_Val[index].length);
		fprintf(fp,"数据精度=[%d]\n",Pu_Val[index].prec);
		fprintf(fp,"数据类型=[%d]\n",Pu_Val[index].lx);
		fprintf(fp,"数据值域=[%s]\n\n\n",cTmplist);
		
		length += Pu_Val[index].length;
			
	}
	fclose(fp);
	
	
	
	vtcp_log("[%s][%d]写文件成功!\n",__FILE__,__LINE__);
	return 0;
}

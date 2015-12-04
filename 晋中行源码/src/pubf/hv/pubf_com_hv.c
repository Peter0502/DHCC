#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#define EXTERN
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hvpack.h"
#include "hv_addit_c.h"
#include "hv_orno_brno_c.h"
#include "hv_brno_orno_c.h"
#include "hv_zfhtr_c.h"
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#include "hv_sendlist_c.h"
#include "seqctl_c.h"
#include "hv_define.h"
#include "find_debug.h"

int iHvStr2Dbl(char* cAmt,double* dAmt);
int  del_dot( char *source,char *target,int size);
void vGetRecvTagTxamt(char *tagamt,char *newtxamt);
#define MAX_BUF 1000
int dot_str2dbl(char * str,int size,double *d_value);
int iHvSetAddit(char *inday,char *txday,char *addid,char *name,char *buf,int length);
int iGetHvOrderno(char * orderno);
int iGet_full_hv_no(char *or_br_no, char *sendco,char * qs_br_no);
int iHVSETTAG(THVPACKAGE * pack,char * tag,char * content,int size);
char* hv_getcmtdesc(char *cmtno);
void vCur_dhcc_mbfe(char *fd_cur,char *mbfe_cur);

/* 函数功能:将前台2位的币种转换为后台mbfe的币种 *
 * 函数名字:vCur_dhcc_mbfe                      *
 * 参    数:char *fd_cur,char *mbfe_cur         *
 * 参数描述:8583种的币种，mbfe的币种            *
 * 作    者:LiuHuafeng 2006-8-21 16:21          *
 * 修    改:
*************************************************/
void vCur_dhcc_mbfe(char *fd_cur,char *mbfe_cur)
{
	if(!memcmp(fd_cur,"01",2))
	{
		MEMCPY_DEBUG(mbfe_cur,"RMB",3);
	}
	else
	{
		MEMCPY_DEBUG(mbfe_cur,"RMB",3);
	}
}
/************************************************
 * 函数功能:把8583种的内容转换为hv_payin中      *
 * 函数名字:vCur_dhcc_mbfe                      *
 * 参    数:HV_PAYIN_AREA * phv_payin           *
 * 参数描述:hv_payin结构                        *
 * 作    者:LiuHuafeng 2006-8-21 16:21          *
 * 修    改:                                    *
*************************************************/
int iHvTistoPayin(HV_PAYIN_AREA * phv_payin)
{
	HV_FD123_AREA fd123;
	char cBuff4[60];
	char cOrderno[9];
	char cInd[2];
	char cCur_no[3];
	char cTistemp[9];
	char cTistemp2[9];
	int iRet=0;
	int i=0;
	MEMSET_DEBUG(cCur_no , 0 , sizeof(cCur_no));
	MEMSET_DEBUG(cInd    , 0 , sizeof(cInd));
	MEMSET_DEBUG(cOrderno, 0 , sizeof(cOrderno));
	MEMSET_DEBUG(cTistemp, 0 , sizeof(cTistemp));
	MEMSET_DEBUG(cTistemp2, 0 , sizeof(cTistemp2));
	MEMSET_DEBUG(&fd123  , 0 , sizeof(fd123));
	MEMSET_DEBUG(cBuff4,'\0',sizeof(cBuff4));
	
	get_fd_data("1230",(char*)&fd123);
	vtcp_log("123域=[%s]",(char*)&fd123.cmtno);TRACE 
	vtcp_log("[%s][%d]sizeof(fd123)=[%d]\n",__FILE__,__LINE__,sizeof(fd123));

	/* 交易机构 */ 
	get_fd_data("0030",phv_payin->F_tx_br_no);
	vtcp_log("前台传来的brno=[%.5s]",phv_payin->F_tx_br_no);TRACE 

	/**委托日期       **/
	get_fd_data("0440",phv_payin->F_wt_date);
	vtcp_log("前台传来的wt_date=[%.8s]",phv_payin->F_wt_date);TRACE 
	get_fd_data("0050",phv_payin->F_tx_date);
	/* 币种 */
	get_fd_data("0210",cCur_no);
	vCur_dhcc_mbfe(cCur_no,phv_payin->F_cur_no);
	/* 交易金额 */
	get_fd_data("0390",phv_payin->F_tx_amt);
	get_zd_double("0390",&g_pub_tx.tx_amt1);
	vtcp_log("前台传来的txamt=[%.16s]",phv_payin->F_tx_amt);TRACE 
	/**凭证种类 **/
	get_fd_data("0890",phv_payin->F_note_type);
	vtcp_log("前台传来的notetype=[%.3s]",phv_payin->F_note_type);TRACE 
	/**凭证号码 **/
	get_fd_data("0580",phv_payin->F_note_no);
	vtcp_log("前台传来的note_no=[%.16s]",phv_payin->F_note_no);TRACE 
	/**操作员号**/
	vtcp_log("前台传来的wt_date=[%.8s]",phv_payin->F_wt_date);TRACE 
	vtcp_log("前台传来的txamt=[%.16s]",phv_payin->F_tx_amt);TRACE 
	get_fd_data(DC_TEL,phv_payin->F_tel);
	vtcp_log("前台传来的tel=[%.8s]",phv_payin->F_tel);TRACE 
	/**复合员号**/
	get_fd_data("0080",phv_payin->F_chk);
	vtcp_log("前台传来的chk=[%.8s]",phv_payin->F_chk);TRACE 
	/**授权操作员号**/
	get_fd_data("0091",phv_payin->F_auth_tel);
	vtcp_log("前台传来的auth_tel=[%.8s]",phv_payin->F_auth_tel);TRACE 
	/**授权口令**/
	get_fd_data("0092",phv_payin->F_auth_pwd);
	/*操作代码***/
	MEMCPY_DEBUG(phv_payin->F_opcd,fd123.opcd,sizeof(phv_payin->F_opcd));
	vtcp_log("前台传来的opcd=[%.2s]",phv_payin->F_opcd);TRACE 
	/* 手续费有单独交易处理
	* MEMCPY_DEBUG(phv_payin->F_chrg_amt       ,,sizeof(phv_payin->F_chrg_amt      ));**手续费金额 **
	*/
	/* 这个不需要，因为有专门的原子交易检查密码
	* MEMCPY_DEBUG(phv_payin->F_userpswd   ,fd79,sizeof(phv_payin->F_userpswd  ));**储户密码       **
	* MEMCPY_DEBUG(phv_payin->F_auth_pwd   ,,sizeof(phv_payin->F_auth_pwd      ));**授权操作员密码 **
	*/	
	vtcp_log("[%s][%d]fd123.yw_type =[%.8s]",__FILE__,__LINE__,fd123.yw_type );
	MEMCPY_DEBUG(phv_payin->F_yw_type,fd123.yw_type,sizeof(phv_payin->F_yw_type));
	vtcp_log("[%s][%d]phv_payin->F_yw_type =[%.8s]",__FILE__,__LINE__,phv_payin->F_yw_type );
	vtcp_log("[%s,%d]ywtype===[%s]\n",__FILE__,__LINE__,phv_payin->F_yw_type);
	/*支付交易序号   **/
	MEMCPY_DEBUG(cOrderno ,fd123.orderno ,sizeof(cOrderno)-1);
	vtcp_log("前台传来的wtdate=[%.8s]",phv_payin->F_wt_date);TRACE 
	zip_space(cOrderno);
	vtcp_log("前台传来的orderno=[%.8s]",cOrderno);TRACE  
	if(strlen(cOrderno)!=8 || memcmp(phv_payin->F_opcd,"10",sizeof(phv_payin->F_opcd))==0)
	{
		/**增加网银的发送大额业务处理 20130607 hzh**/
		char cTx_code[5];
		char cSub_code[5];
		memset(cTx_code, 0 , sizeof(cTx_code));
		memset(cSub_code , 0 , sizeof(cSub_code));
		memcpy(cTx_code, g_pub_tx.tx_code, sizeof(cTx_code)-1);
		memcpy(cSub_code,g_pub_tx.sub_tx_code , sizeof(cSub_code)-1);
		vtcp_log("[%s][%d]cTx_code=[%s][%s]\n",__FILE__,__LINE__,cTx_code,cSub_code);
		if(memcmp(cTx_code,"6389",sizeof(cTx_code)-1)==0 &&
		memcmp(cSub_code,"J009",sizeof(cSub_code)-1)==0)
		{
			memcpy(phv_payin->F_opcd,"40",sizeof(phv_payin->F_opcd));
			memcpy(fd123.opcd,"40",sizeof(fd123.opcd));
			g_pub_tx.trace_cnt++;
		}
		else
		{
		iRet=iGetHvOrderno(cOrderno);
		if(iRet)
		{
		sprintf( acErrMsg,"得到支付交易序号错误![%s][%d][%d]",__FILE__,__LINE__,iRet);
		set_zd_data(DC_GET_MSG,"得到支付交易序号错误!");
	    	strcpy(g_pub_tx.reply,"HV01");
		return iRet;
		}
		/**增加网银的发送大额业务处理 20130607 hzh**/
		MEMCPY_DEBUG(fd123.orderno,cOrderno,sizeof(fd123.orderno));
		set_zd_data("1230",(char*)&fd123);
		}
	}
	vtcp_log("前台传来的wtdate=[%.8s]",phv_payin->F_wt_date);TRACE 
	/*往来表示***/
	phv_payin->F_lw_ind[0]=fd123.lw_ind[0];
	vtcp_log("前台传来的来往标志=[%c]",phv_payin->F_lw_ind[0]);
	/* 手续费标志 */
	get_fd_data("0093",phv_payin->F_tx_chrg_ind);
	vtcp_log("前台传来的tx_chrg_ind=[%.1s]",phv_payin->F_tx_chrg_ind);TRACE 
	MEMCPY_DEBUG(phv_payin->F_orderno ,cOrderno ,sizeof(phv_payin->F_orderno ));
	/*cmt编号**/
	MEMCPY_DEBUG(phv_payin->F_cmtno   ,fd123.cmtno   ,sizeof(phv_payin->F_cmtno   ));
	vtcp_log("cmtno前台传来的附加域=[%s][%s]",phv_payin->F_cmtno,fd123.cmtno);TRACE 

	/*发起行行号     **/
	vtcp_log("[%s,%d]交易行号 br_no=[%.12s]",__FILE__,__LINE__,phv_payin->F_tx_br_no);TRACE 
	if(phv_payin->F_lw_ind[0]=='1')
	{
		char cOpnbr[BRNO_LEN+1];
		MEMSET_DEBUG(cOpnbr, 0 , sizeof(cOpnbr));
		get_fd_data("0020",cOpnbr);
		g_reply_int=GetOr_br_noByBr_no(phv_payin->F_tx_br_no,phv_payin->F_or_br_no);
		if(g_reply_int)
		{
			strncpy(g_pub_tx.reply,"P007",4);
			vtcp_log("%s,%d 查找支付系统行号错误",__FILE__,__LINE__);
			return g_reply_int;
		}
		
		g_reply_int=GetOr_br_noByBr_no(cOpnbr,phv_payin->F_pay_opn_br_no);
		if(g_reply_int)
		{
			strncpy(g_pub_tx.reply,"P007",4);
			vtcp_log("%s,%d 查找支付系统行号错误",__FILE__,__LINE__);
			return g_reply_int;
		}  
	}
	else
	{
		MEMCPY_DEBUG(phv_payin->F_or_br_no,fd123.or_br_no,sizeof(phv_payin->F_or_br_no));
		/*付款人开户行**/
		MEMCPY_DEBUG(phv_payin->F_pay_opn_br_no ,fd123.pay_opn_br_no ,
			sizeof(phv_payin->F_pay_opn_br_no ));
	}
	vtcp_log("[%s,%d]支付系统发起行号 or_br_no=[%.12s]",__FILE__,__LINE__,phv_payin->F_or_br_no);TRACE 
	vtcp_log("[%s,%d]付款人开户行号 opn_br_no=[%.12s]",__FILE__,__LINE__,phv_payin->F_pay_opn_br_no);TRACE 
	/*接收行行号     **/
	MEMCPY_DEBUG(phv_payin->F_ac_br_no,fd123.ac_br_no,sizeof(phv_payin->F_ac_br_no));
	vtcp_log("前台传来的ac_br_no=[%.12s]",phv_payin->F_ac_br_no);TRACE 
	/*付款人帐号*/
	MEMCPY_DEBUG(phv_payin->F_pay_ac_no ,fd123.pay_ac_no,
		sizeof(phv_payin->F_pay_ac_no));
	vtcp_log("前台传来的pay_ac_no=[%.12s]",phv_payin->F_pay_ac_no);TRACE 
	/*付款人名称*/
	MEMCPY_DEBUG(phv_payin->F_pay_name  ,fd123.pay_name,
		sizeof(phv_payin->F_pay_name  ));
	/* 检查户名的有效使用范围 */
	vtcp_log("前台传来的pay_name=[%.60s]",phv_payin->F_pay_name);TRACE 
	/*付款人地址*/
	MEMCPY_DEBUG(phv_payin->F_pay_addr  ,fd123.pay_addr,
		sizeof(phv_payin->F_pay_addr  ));
	/*收款人帐号*/
	MEMCPY_DEBUG(phv_payin->F_cash_ac_no,fd123.cash_ac_no,
		sizeof(phv_payin->F_cash_ac_no));
	/*收款人名称*/
	MEMCPY_DEBUG(phv_payin->F_cash_name ,fd123.cash_name ,
		sizeof(phv_payin->F_cash_name ));
	/*收款人地址*/
	MEMCPY_DEBUG(phv_payin->F_cash_addr,fd123.cash_addr,
		sizeof(phv_payin->F_cash_addr));
	vtcp_log("前台传来的cash_addr=[%.60s]",phv_payin->F_cash_addr);TRACE 
	/*附言*/
	MEMCPY_DEBUG(phv_payin->F_brf,fd123.brf,sizeof(phv_payin->F_brf));
	/*优先级*/
	MEMCPY_DEBUG(phv_payin->F_operlevel ,fd123.operlevel ,
		sizeof(phv_payin->F_operlevel ));
	vtcp_log("前台传来的wtdate=[%.8s]",phv_payin->F_wt_date);TRACE 

	/*收款人开户行行号**/
	MEMCPY_DEBUG(phv_payin->F_cash_opn_br_no,fd123.cash_opn_br_no	,
		sizeof(phv_payin->F_cash_opn_br_no));

	vtcp_log("前台传来的wtdate=[%.8s]",phv_payin->F_wt_date);TRACE 
	/* 原交易日期 */
	get_fd_data("0450",phv_payin->F_o_tx_date);
	vtcp_log("前台传来的o_tx_date=[%.8s]",phv_payin->F_o_tx_date);TRACE 
	/* 原委托日期 */
	get_fd_data("0450",phv_payin->F_o_wt_date);
	vtcp_log("前台传来的o_wt_date=[%.8s]",phv_payin->F_o_wt_date);TRACE 
	/**原CMT编号**/
	MEMCPY_DEBUG(phv_payin->F_o_cmtno,fd123.o_cmtno,
		sizeof(phv_payin->F_o_cmtno));
	/**附言***/
	MEMCPY_DEBUG(phv_payin->F_hv_brf,fd123.brf,sizeof(phv_payin->F_hv_brf));
	vtcp_log("前台传来的hv_brf=[%.8s]",phv_payin->F_hv_brf);TRACE 
	/**原支付交易序号**/
	MEMCPY_DEBUG(phv_payin->F_o_orderno,fd123.o_orderno,
		sizeof(phv_payin->F_o_orderno));
	vtcp_log("前台传来的o_orderno=[%.8s]",phv_payin->F_o_orderno);TRACE 
	/**处理号码**/
	MEMCPY_DEBUG(phv_payin->F_resp1,fd123.procode,sizeof(phv_payin->F_resp1));
	vtcp_log("cmtno前台传来的附加域=[%.3s][%.3s]",phv_payin->F_cmtno,fd123.cmtno);TRACE 
	iHvZip_hvpayinF(phv_payin);
	vtcp_log("%s%d",__FILE__,__LINE__);
	return 0;
}

/************************************************
 * 函数功能:把hv_payin中F开头的内容写入到T开头  *
 * 函数名字:iHvFrontToTable                     *
 * 参    数:HV_PAYIN_AREA * phv_payin           *
 * 参数描述:hv_payin结构                        *
 * 作    者:LiuHuafeng 2006-8-21 16:21          *
 * 修    改:                                    *
*************************************************/
int iHvFrontToTable(HV_PAYIN_AREA * phv_payin)
{
	char cFilename[1000];
	FILE * fpr;
	char reply[5];
	int iRet=0;
	char cBuff[2000];
	MEMSET_DEBUG(reply, 0 , sizeof(reply));
	MEMSET_DEBUG(cBuff, 0 , sizeof(cBuff));
	MEMSET_DEBUG(cFilename, 0  , sizeof(cFilename));
	Gl_file_first=0;
	MEMCPY_DEBUG(phv_payin->T_tx_br_no,phv_payin->F_tx_br_no,
		sizeof(phv_payin->T_tx_br_no));/* 交易行行号*/
	MEMCPY_DEBUG(phv_payin->T_tx_date ,phv_payin->F_tx_date ,
		sizeof(phv_payin->T_tx_date ));/* 交易日期*/
	MEMCPY_DEBUG(phv_payin->T_wt_date ,phv_payin->F_wt_date ,
		sizeof(phv_payin->T_wt_date ));/* 委托日期*/
	/* 交易时间*/
	get_fd_data(DC_TIME,phv_payin->T_tx_time);
	vtcp_log("[%s][%d]得到的时间===%s",__FILE__,__LINE__,phv_payin->T_tx_time);
	vtcp_log("[%s][%d]得到的时间===%s",__FILE__,__LINE__,phv_payin->T_tx_time);
	vtcp_log("[%s][%d]得到的时间===%s",__FILE__,__LINE__,phv_payin->T_tx_time);
	vtcp_log("[%s][%d]得到的时间===%s",__FILE__,__LINE__,phv_payin->T_tx_time);
	vtcp_log("[%s][%d]F_tx_br_no===%s",__FILE__,__LINE__,phv_payin->F_tx_br_no);
	vtcp_log("[%s][%d]T_tx_br_no===%s",__FILE__,__LINE__,phv_payin->T_tx_br_no);
	MEMCPY_DEBUG(phv_payin->T_orderno  ,phv_payin->F_orderno  ,
		sizeof(phv_payin->T_orderno  ));/* 人行的交易序号*/
	MEMCPY_DEBUG(phv_payin->T_cur_no   ,phv_payin->F_cur_no   ,
		sizeof(phv_payin->T_cur_no   ));/* 货币符号*/
	MEMCPY_DEBUG(phv_payin->T_note_type,phv_payin->F_note_type,
		sizeof(phv_payin->T_note_type));/* 凭证类型 */
	MEMCPY_DEBUG(phv_payin->T_note_no  ,phv_payin->F_note_no  ,
		sizeof(phv_payin->T_note_no  ));/* 凭证号 */
	vtcp_log("前台传来的note_no=[%.16s]",phv_payin->T_note_no);TRACE 
	MEMCPY_DEBUG(phv_payin->T_tx_amt   ,phv_payin->F_tx_amt   ,
		sizeof(phv_payin->T_tx_amt   ));/* 交易金额 */
	vtcp_log("前台传来的note_no=[%.16s]",phv_payin->T_note_no);TRACE 
	MEMCPY_DEBUG(phv_payin->T_tx_chrg_ind,phv_payin->F_tx_chrg_ind,
		sizeof(phv_payin->T_tx_chrg_ind ));/*手续费标志 0.现金 1.现金加急 2.转帐3.转帐加急 4.不收 */
	/*note 手续费怎么处理没有讨论完，先放放2006-8-7 14:28
	*MEMCPY_DEBUG(phv_payin->T_chrg_amt,phv_payin->F_chrg_amt,
	sizeof(phv_payin->T_chrg_amt         ));  手续费 
	*/

	vtcp_log("前台传来的note_no=[%.16s]",phv_payin->T_note_no);TRACE 
	g_reply_int=GetOr_br_noByBr_no(phv_payin->T_tx_br_no,phv_payin->T_or_br_no);
	if(g_reply_int)
	{
		strncpy(g_pub_tx.reply,"P007",4);
		vtcp_log("%s,%d 查找支付系统行号错误",__FILE__,__LINE__);
		return g_reply_int;
	}

	g_reply_int=iGet_full_hv_no(phv_payin->T_or_br_no,phv_payin->T_sendco,phv_payin->T_pay_qs_no);
	if(g_reply_int)
	{
		strncpy(g_pub_tx.reply,"P007",4);
		vtcp_log("%s,%d 查找支付系统清算行号错误",__FILE__,__LINE__);
		return g_reply_int;
	}
	if(!memcmp(phv_payin->F_cmtno,"121",3))
	{
		MEMCPY_DEBUG(phv_payin->F_ac_br_no,HV_HPCENTER,sizeof(phv_payin->F_ac_br_no));
	}
	vtcp_log("发起清算行号=[%.12s]",phv_payin->T_pay_qs_no);TRACE 
	
	MEMCPY_DEBUG(phv_payin->T_ac_br_no ,phv_payin->F_ac_br_no ,
		sizeof(phv_payin->T_ac_br_no));/* 接收行行号--退汇交易的原发起行 */
	g_reply_int=iGet_full_hv_no(phv_payin->T_ac_br_no,phv_payin->T_receco,phv_payin->T_cash_qs_no);
	if(g_reply_int)
	{
		strncpy(g_pub_tx.reply,"P007",4);
		vtcp_log("%s,%d ac_br_no=[%.12s]",__FILE__,__LINE__,phv_payin->T_ac_br_no);
		vtcp_log("%s,%d 查找支付系统清算行号错误",__FILE__,__LINE__);
		return g_reply_int;
	}
	
	/*iFindQsbrno(phv_payin->T_ac_br_no,phv_payin->T_cash_qs_no); 接收行清算行号 	
	 *iFindQsbrno(phv_payin->T_or_br_no,phv_payin->T_pay_qs_no);  发起清算行行号 
	 *iGetSendco(phv_payin->T_or_br_no,phv_payin->T_sendco);发报中心代码 
	 *iGetSendco(phv_payin->T_ac_br_no,phv_payin->T_receco);收报中心代码 
	 */
  vtcp_log("[%s][%d]F_pay_opn_br_no===[%s]",__FILE__,__LINE__,phv_payin->F_pay_opn_br_no);
	MEMCPY_DEBUG(phv_payin->T_pay_opn_br_no,phv_payin->F_pay_opn_br_no,
		sizeof(phv_payin->T_pay_opn_br_no));/* 付款人开户行行号 */
	vtcp_log("[%s][%d]T_pay_opn_br_no===[%s]",__FILE__,__LINE__,phv_payin->T_pay_opn_br_no);	
	MEMCPY_DEBUG(phv_payin->T_pay_ac_no    ,phv_payin->F_pay_ac_no    ,
		sizeof(phv_payin->T_pay_ac_no    ));/* 付款人帐号 */
	MEMCPY_DEBUG(phv_payin->T_pay_name , phv_payin->F_pay_name ,
		sizeof(phv_payin->T_pay_name));/* 付款人姓名-退汇交易的原收款人户名*/
	MEMCPY_DEBUG(phv_payin->T_pay_addr ,phv_payin->F_pay_addr ,
		sizeof(phv_payin->T_pay_addr     ));/* 付款人地址 */


	MEMCPY_DEBUG(phv_payin->T_cash_opn_br_no,phv_payin->F_cash_opn_br_no,
		sizeof(phv_payin->T_cash_opn_br_no));/* 收款人开户行行号 */
	MEMCPY_DEBUG(phv_payin->T_cash_ac_no,phv_payin->F_cash_ac_no,
		sizeof(phv_payin->T_cash_ac_no    ));/* 收款人帐号       */
	MEMCPY_DEBUG(phv_payin->T_cash_name ,phv_payin->F_cash_name ,
		sizeof(phv_payin->T_cash_name     ));/* 收款人姓名-退汇交易的原付款人户名*/
	MEMCPY_DEBUG(phv_payin->T_cash_addr ,phv_payin->F_cash_addr ,
		sizeof(phv_payin->T_cash_addr     ));/* 收款人地址   */
	vtcp_log("[%s][%d]phv_payin->F_yw_type =[%.8s]",__FILE__,__LINE__,phv_payin->F_yw_type );	
	MEMCPY_DEBUG(phv_payin->T_yw_type   ,phv_payin->F_yw_type   ,
		sizeof(phv_payin->T_yw_type       ));/* 业务种类     */
	vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,phv_payin->T_yw_type);	
	vtcp_log("[%s,%d]ywtype===[%s]\n",__FILE__,__LINE__,phv_payin->T_yw_type);

	MEMCPY_DEBUG(phv_payin->T_cmtno    ,phv_payin->F_cmtno    ,
		sizeof(phv_payin->T_cmtno    ));/* CMT编号      */
	vtcp_log("[%s][%d]phv_payin->F_hv_brf======[%s]",__FILE__,__LINE__,phv_payin->F_hv_brf);	
	MEMCPY_DEBUG(phv_payin->T_hv_brf      ,phv_payin->F_hv_brf      ,
		sizeof(phv_payin->T_hv_brf      ));/* 附言         */
	vtcp_log("[%s][%d]hv_payin->T_brf======[%s]",__FILE__,__LINE__,phv_payin->T_hv_brf);
	MEMCPY_DEBUG(phv_payin->T_operlevel,phv_payin->F_operlevel,
		sizeof(phv_payin->T_operlevel));/* 优先级别0-一般1-紧急2-特急[救灾战备款等支付]*/

	MEMCPY_DEBUG(phv_payin->T_auth_tel ,phv_payin->F_auth_tel ,
		sizeof(phv_payin->T_auth_tel ));/* 授权操作员*/
	/* note 需要加入到hv_define.h中 */
	MEMCPY_DEBUG(phv_payin->T_o_orderno,phv_payin->F_o_orderno,
		sizeof(phv_payin->T_o_orderno));/* 原支付交易序号--退汇交易用的 */
	MEMCPY_DEBUG(phv_payin->T_o_tx_date,phv_payin->F_o_tx_date,
		sizeof(phv_payin->T_o_tx_date));/* 原支付交易日期--退汇交易用的 */
	MEMCPY_DEBUG(phv_payin->T_o_cmtno  ,phv_payin->F_o_cmtno  ,
		sizeof(phv_payin->T_o_cmtno  ));/* 原cmtno--退汇交易用的 */
	MEMCPY_DEBUG(phv_payin->T_resp1    ,phv_payin->F_resp1    ,
		sizeof(phv_payin->T_resp1    ));/* 应答码*/
	MEMCPY_DEBUG(phv_payin->T_o_wt_date,phv_payin->F_o_wt_date,
		sizeof(phv_payin->T_o_wt_date));/*原委托日期*/			
	vtcp_log("%s,%d 原委托日期==[%.8s]",__FILE__,__LINE__,phv_payin->T_o_wt_date);
	vtcp_log("[%s][%d]T_pay_opn_br_no===[%s]",__FILE__,__LINE__,phv_payin->T_pay_opn_br_no);
	MEMCPY_DEBUG(phv_payin->T_hv_brf   ,phv_payin->F_hv_brf   ,
		sizeof(phv_payin->T_hv_brf   ));/* 备注 */
	/*以下 对附加域进行处理 */
	/* add by LiuHuafeng 如果是发送就不再设置附加域 */
	/***change by liuyue 20070122 汇票移存 有时候会不写附加域的密押字段，因此将附加域处理放到spJ008中进行处理**/

	if(memcmp(phv_payin->F_opcd,"40",sizeof(phv_payin->F_opcd)))
	if (memcmp(phv_payin->T_cmtno , HV_HD , sizeof(phv_payin->T_cmtno))!=0 && 
		memcmp(phv_payin->T_cmtno , HV_GKZJHH , sizeof(phv_payin->T_cmtno))!=0  &&
		memcmp(phv_payin->T_cmtno , HV_HPYC , sizeof(phv_payin->T_cmtno))!=0 ) 
	{
		vtcp_log("有附加域cmtno=[%s]",phv_payin->T_cmtno);TRACE 
		/* 不是汇兑和国库资金的情况 */
		Gl_file_first=0;/* add by LiuHuafeng 2006-8-26 14:32 */
		pub_base_AllDwnFilName( cFilename);
		vtcp_log("%s,%d filename=[%s]\n",__FILE__,__LINE__,cFilename);
		fpr=fopen(cFilename,"rb");
		if (fpr==NULL)
		{
			sprintf( acErrMsg,"得到附加域文件错误![%s][%d]",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"得到附加域文件错误");
			WRITEMSG
			return -1;
		}
		fgets(cBuff,sizeof(cBuff),fpr);
		vtcp_log("%s,%d file=[%s]\n",__FILE__,__LINE__,cBuff);
		fclose(fpr);
		iRet=iHvSetAddit(phv_payin->F_tx_date,phv_payin->F_wt_date,phv_payin->T_addid,"000",cBuff,strlen(cBuff));
		if (iRet)
		{
			sprintf( acErrMsg,"写入附加域错误![%s][%d][%d]",__FILE__,__LINE__,iRet);
			set_zd_data(DC_GET_MSG,"写入附加域错误");
			WRITEMSG
			return -1;
		}
		vtcp_log("%s,%d wt_date=%s,addid=[%s]",__FILE__,__LINE__,phv_payin->F_wt_date,phv_payin->T_addid);
	}
	MEMCPY_DEBUG(phv_payin->T_refno , phv_payin->F_refno ,
		sizeof(phv_payin->T_refno ));/* 报文参考号 */
	/* 暂时没有办法处理的代码
	* MEMCPY_DEBUG(phv_payin->T_auth_trace_no , phv_payin->F_auth_trace_no ,
	sizeof(phv_payin->T_auth_trace_no    )); *授权操作员流水号*
	*/
	
	vtcp_log("%s,%d phv_payin->F_refno************=[%s]\n",__FILE__,__LINE__,phv_payin->F_refno);
	vtcp_log("%s,%d phv_payin->T_refno************=[%s]\n",__FILE__,__LINE__,phv_payin->T_refno);
	vtcp_log("%s,%d phv_payin->F_opcd*[%s]\n",__FILE__,__LINE__,phv_payin->F_opcd);
	phv_payin->T_lw_ind[0]=phv_payin->F_lw_ind[0];
	if (memcmp(phv_payin->F_opcd,"10",2) == 0)
	{/* 01录入 */
		phv_payin->T_lw_ind[0]='1'; /* 来往标识 1-往 2-来 */	
		phv_payin->T_hv_sts[0]=SEND_REGIST;
		if(!memcmp(phv_payin->T_cmtno,"121",3)){
			phv_payin->T_hv_sts[0]=SEND_ACCT_CHECK;
		}
		iRet=iGetHvRefno(phv_payin->T_or_br_no,phv_payin->T_refno);/*modify by xyy 2009-7-17 14:42:02*/
		if (iRet)
		{
				vtcp_log("[%s,%d]得到支付交易序号错误=%d",__FILE__,__LINE__,iRet);
				sprintf( acErrMsg,"得到支付交易序号错误![%s][%d][%d]",__FILE__,__LINE__,iRet);
				set_zd_data(DC_GET_MSG,"得到支付交易序号错误");
				WRITEMSG
				return -1;
		}

		MEMCPY_DEBUG(phv_payin->F_refno,phv_payin->T_refno,sizeof(phv_payin->F_refno));
		vtcp_log("%s,%d phv_payin->F_refno=[%s]****hvsts=[%c]\n",__FILE__,__LINE__,phv_payin->F_refno,phv_payin->T_hv_sts[0]);
		vtcp_log("%s,%d phv_payin->T_refno=[%s]****hvsts=[%c]\n",__FILE__,__LINE__,phv_payin->T_refno,phv_payin->T_hv_sts[0]);
		/* 流水号*/
		get_fd_data(DC_TRACE_NO,phv_payin->T_trace_no);
		get_fd_data(DC_TEL,phv_payin->T_tel);
		MEMCPY_DEBUG(g_pub_tx.ac_no,phv_payin->F_pay_ac_no,sizeof(g_pub_tx.ac_no)-1);
		g_pub_tx.ac_no[strlen(g_pub_tx.ac_no)]='\0';
		pub_base_old_acno(g_pub_tx.ac_no);
		pub_base_strpack(g_pub_tx.ac_no);
		iMbfeGetAcid(g_pub_tx.ac_no);
	}
	if (memcmp(phv_payin->F_opcd,"30",2) == 0)
	{/* 02复合 */
		iRet=iHvCheckRecordStat(phv_payin->F_cmtno,phv_payin->T_hv_sts[0],SEND_ACCT_CHECK,phv_payin->T_lw_ind[0],acErrMsg);
		if(iRet)
		{
			sprintf( acErrMsg,"记录状态错误![%s][%d][%d]",__FILE__,__LINE__,iRet);
			set_zd_data(DC_GET_MSG,"记录状态错误");
			WRITEMSG
			return -1;
		}
		get_fd_data(DC_TRACE_NO,phv_payin->T_chk_trace_no);/* 复核操作员流水号*/
		get_fd_data(DC_TEL,phv_payin->T_chk);
		phv_payin->T_hv_sts[0]=SEND_ACCT_CHECK;
	}
	if(memcmp(phv_payin->F_opcd,"40",2) == 0)
	{/* 03发送 */
		iRet=iHvCheckRecordStat(phv_payin->F_cmtno,phv_payin->T_hv_sts[0],SEND_ACCT_WAIT,phv_payin->T_lw_ind[0],acErrMsg);
		if(iRet)
		{
			sprintf( acErrMsg,"记录状态错误![%s][%d][%d]",__FILE__,__LINE__,iRet);
			set_zd_data(DC_GET_MSG,"记录状态错误");
			WRITEMSG
			return -1;
		}
		/* 发送操作员流水号*/
		get_fd_data(DC_TRACE_NO,phv_payin->T_send_tel_trace_no);
		get_fd_data(DC_TEL,phv_payin->T_send_tel);
		/***add 20061009 liuyue** 20061116改一下  liuyue   之前没有加return  try*****/
		 if ( phv_payin->T_hv_sts[0] == SEND_ACCT_WAIT || phv_payin->T_hv_sts[0]!=SEND_ACCT_CHECK)
    		{
        		sprintf(acErrMsg,"记录状态错误!\n");
        		strcpy( g_pub_tx.reply, "P016");
        		vtcp_log("[%s][%d]T_hv_sts=[%c][%c]\n",__FILE__,__LINE__,phv_payin->T_hv_sts[0] , SEND_ACCT_CHECK);
        		set_zd_data(DC_GET_MSG,"记录状态错误");
        		WRITEMSG
				return (-1);
        		
    		}

		phv_payin->T_hv_sts[0]=SEND_ACCT_WAIT;
	}
	if(memcmp(phv_payin->F_opcd,"60",2)==0)
	{/* 04接收 */
		phv_payin->T_beg_sts[0]=phv_payin->T_hv_sts[0];
		phv_payin->T_lw_ind[0]='2'; /* 来往标识 1-往 2-来 */
		get_fd_data("0120",reply);
		if (!memcmp(reply,"0000",4))
		{
			phv_payin->T_hv_sts[0]=RECEIVE_ACCT_QS;
		}
		else
		{
			/* 记账失败了，要先判断接收机构是否存在，存在挂支行，不存在挂清算 */
			phv_payin->T_hv_sts[0]=RECEIVE_CUSTNAME_ERR;
		}
	}
	if (memcmp(phv_payin->F_opcd,"50",2)==0)
	{/* 05应答 */
		phv_payin->T_lw_ind[0]='2'; /* 来往标识 1-往 2-来 */	
		phv_payin->T_hv_sts[0]=SEND_ACCT_QS;
	}
	if(memcmp(phv_payin->F_opcd,"11",2)==0)
	{/* 06查询 */
	}
	if(memcmp(phv_payin->F_opcd,"71",2)==0)
	{/* 07来帐修改 */
		MEMCPY_DEBUG(phv_payin->T_dealerr_date , phv_payin->F_tx_date ,
			sizeof(phv_payin->T_dealerr_date));/* 错帐处理日期*/
		MEMCPY_DEBUG(phv_payin->T_dealerr_tel , phv_payin->F_tel ,
			sizeof(phv_payin->T_dealerr_tel ));/* 错帐处理柜员*/
		phv_payin->T_lw_ind[0]='2'; /* 来往标识 1-往 2-来 */	
		MEMCPY_DEBUG(g_pub_tx.ac_no,phv_payin->F_pay_ac_no,sizeof(g_pub_tx.ac_no)-1);
		g_pub_tx.ac_no[strlen(g_pub_tx.ac_no)]='\0';
		pub_base_old_acno(g_pub_tx.ac_no);
		pub_base_strpack(g_pub_tx.ac_no);
		iMbfeGetAcid(g_pub_tx.ac_no);
	}
	if (memcmp(phv_payin->F_opcd,"80",2)==0)
	{/* 10 拒绝退回支行 */
		MEMCPY_DEBUG(phv_payin->T_resend_date , phv_payin->T_tx_date ,
			sizeof(phv_payin->T_resend_date)); /* 拒绝重新发送日期 */
		phv_payin->T_lw_ind[0]='1'; /* 来往标识 1-往 2-来 */	
	}
	vtcp_log("[%s][%d]T_pay_opn_br_no===[%s]",__FILE__,__LINE__,phv_payin->T_pay_opn_br_no);
	return 0;
}
/************************
*       PART 1       ***
*   往帐报文处理接口 ***
************************/

int pub_com_CMT100_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack100)
{    
	char cTmpBuf[19];	
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	pack100->opeType='0';/*大额*/
	pack100->PRI=hv_payin->T_operlevel[0];				/*优先级别*/
	MEMCPY_DEBUG(pack100->CMTCode,"100",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack100->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	vtcp_log("[%s][%d]hv_payin->T_refno======[%s]",__FILE__,__LINE__,hv_payin->T_refno);
	
	vtcp_log("[%s][%d]pack100->reNo======[%s]",__FILE__,__LINE__,pack100->reNo);
	MEMCPY_DEBUG(pack100->workDate,hv_payin->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack100,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVSETTAG(pack100,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*货币类型*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*交易金额*/
	iHVSETTAG(pack100,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*货币+金额*/
	iHVSETTAG(pack100,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack100,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack100,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVSETTAG(pack100,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVSETTAG(pack100,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVSETTAG(pack100,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*付款人地址*/
	iHVSETTAG(pack100,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack100,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack100,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));/*收款人开户行号*/
	iHVSETTAG(pack100,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVSETTAG(pack100,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVSETTAG(pack100,"59B",hv_payin->T_cash_addr,sizeof(hv_payin->T_cash_addr));	/*收款人地址*/
	iHVSETTAG(pack100,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*业务种类*/
	iHVSETTAG(pack100,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	iHVSETTAG(pack100,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack100,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	vtcp_log("[%s][%d]hv_payin->T_brf======[%s]",__FILE__,__LINE__,hv_payin->T_hv_brf);
	/**
	iHVSETTAG(pack100,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			附言*/
	vtcp_log("[%s][%d]hv_payin->T_brf======[%s]",__FILE__,__LINE__,hv_payin->T_hv_brf);
	iHVSETTAG(pack100,"72A",hv_payin->T_hv_brf,60);			/*附言*/
	
	/******汇兑业务没有附加域***************/	
	return 0; 
}
int pub_com_CMT101_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack101)
{  
	/***************委托收款（划回）支付报文***************************/  
	char cTmpBuf[19];	
	char cAddid[9];
	HV_WTSK_ADD  hv_wtsk;/***附加域****/

	MEMSET_DEBUG(&hv_wtsk,0,sizeof(hv_wtsk));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));

	pack101->opeType='0';/*大额*/					
	pack101->PRI=hv_payin->T_operlevel[0];				/*优先级别*/
	MEMCPY_DEBUG(pack101->CMTCode,"101",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack101->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack101->workDate,hv_payin->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack101,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*货币种类*/	
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*交易金额*/
	iHVSETTAG(pack101,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*货币+金额*/
	iHVSETTAG(pack101,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack101,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack101,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVSETTAG(pack101,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVSETTAG(pack101,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVSETTAG(pack101,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack101,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack101,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));/*收款人开户行号*/
	iHVSETTAG(pack101,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVSETTAG(pack101,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVSETTAG(pack101,"CEJ",hv_payin->T_note_type,sizeof(hv_payin->T_note_type));	/*凭证种类*/
	/** rem by LiuHuafeng 20070130
	iHVSETTAG(pack101,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		*业务种类*
	***************************************************************************/
	iHVSETTAG(pack101,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	iHVSETTAG(pack101,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack101,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVSETTAG(pack101,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	MEMCPY_DEBUG(cAddid,hv_payin->T_addid , sizeof(cAddid)-1);
	zip_space(cAddid);
	if (strlen(cAddid)==8)
	{
		iHvGetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,cAddid,"000",&hv_wtsk,sizeof(hv_wtsk));
		iHVSETTAG(pack101,"30B",hv_wtsk.pj_date,sizeof(hv_wtsk.pj_date));	/*票据日期*/
		iHVSETTAG(pack101,"21A",hv_wtsk.pj_num,sizeof(hv_wtsk.pj_num));		/*票据号码*/
		iHVSETTAG(pack101,"CEJ",hv_wtsk.note_type,sizeof(hv_wtsk.note_type));   /**凭证种类 add by zyl 20110715 **/
	}
	return 0; 
}
int pub_com_CMT102_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack102)
{   
	/*****************托收承付（划回）支付报文*************************/ 
	char cTmpBuf[19];	
	char cAddid[9];

	HV_TSCF_ADD hv_tscf;  /****处理附加域***********/
	MEMSET_DEBUG(&hv_tscf,0,sizeof(hv_tscf));

	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));				

	pack102->opeType='0';/*大额*/
	pack102->PRI=hv_payin->T_operlevel[0];				/*优先级别*/
	MEMCPY_DEBUG(pack102->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack102->CMTCode,"102",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack102->workDate,hv_payin->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack102,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);		 		/*货币类型*/ 
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*交易金额*/
	iHVSETTAG(pack102,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*货币+金额*/
	iHVSETTAG(pack102,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack102,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack102,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVSETTAG(pack102,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVSETTAG(pack102,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVSETTAG(pack102,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack102,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack102,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVSETTAG(pack102,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVSETTAG(pack102,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVSETTAG(pack102,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	iHVSETTAG(pack102,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack102,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVSETTAG(pack102,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));		/*附言*/

	MEMCPY_DEBUG(cAddid,hv_payin->T_addid , sizeof(cAddid)-1);
	zip_space(cAddid);
	if (strlen(cAddid)==8)
	{
		iHvGetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,cAddid,"000",&hv_tscf,sizeof(hv_tscf));
		iHVSETTAG(pack102,"30B",hv_tscf.pj_date,sizeof(hv_tscf.pj_date));		/*票据日期*/
		iHVSETTAG(pack102,"21A",hv_tscf.pj_num,sizeof(hv_tscf.pj_num));			/*票据号码*/
		del_dot(hv_tscf.ptxamt,hv_tscf.ptxamt,15);
		iHVSETTAG(pack102,"33A",hv_tscf.ptxamt,sizeof(hv_tscf.ptxamt));		/*赔偿金额*/
		del_dot(hv_tscf.jtxamt,hv_tscf.jtxamt,15);
		iHVSETTAG(pack102,"33B",hv_tscf.jtxamt,sizeof(hv_tscf.jtxamt));		/*拒付金额*/
	}
	vtcp_log("body==$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$[%s]",pack102->body);
	return 0; 
}
int pub_com_CMT103_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack103)
{   
	/****************国库资金汇划（贷记）支付报文**********************/ 
	char cTmpBuf[19];	
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	pack103->opeType='0';/*大额*/
	pack103->PRI=hv_payin->T_operlevel[0];				/*优先级别*/
	MEMCPY_DEBUG(pack103->CMTCode,"103",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack103->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack103->workDate,hv_payin->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack103,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*货币种类*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*交易金额*/
	iHVSETTAG(pack103,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*货币+金额*/
	iHVSETTAG(pack103,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack103,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack103,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVSETTAG(pack103,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVSETTAG(pack103,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVSETTAG(pack103,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack103,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack103,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVSETTAG(pack103,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVSETTAG(pack103,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin->T_yw_type);
	iHVSETTAG(pack103,"0BF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*业务种类*/
	vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin->T_yw_type);
	iHVSETTAG(pack103,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	iHVSETTAG(pack103,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack103,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVSETTAG(pack103,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/
	vtcp_log("body==$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$[%s]",pack103->body);
	/*****没有附加域********/	
	return 0; 
}
int pub_com_CMT104_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack104)
{   
	/***************定期贷记支付报文***********************************/ 
	char cTmpBuf[19];	
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));
	pack104->opeType='0';/*大额*/
	pack104->PRI=hv_payin->T_operlevel[0];				/*优先级别*/
	MEMCPY_DEBUG(pack104->CMTCode,"104",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack104->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack104->workDate,hv_payin->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack104,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*货币种类*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*交易金额*/
	iHVSETTAG(pack104,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*货币+金额*/
	iHVSETTAG(pack104,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack104,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack104,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVSETTAG(pack104,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVSETTAG(pack104,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVSETTAG(pack104,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack104,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack104,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVSETTAG(pack104,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVSETTAG(pack104,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVSETTAG(pack104,"59B",hv_payin->T_cash_addr,sizeof(hv_payin->T_cash_addr));	/*收款人地址*/
	iHVSETTAG(pack104,"OBF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*业务种类*/
	iHVSETTAG(pack104,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	iHVSETTAG(pack104,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack104,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVSETTAG(pack104,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	/**********没有附加域******************************/	
	return 0; 
}
int pub_com_CMT105_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack105)
{    
	/************银行间同业拆借支付报文********************************/
	char cTmpBuf[19];	
	char cAddid[9];
	HV_TYCJ_ADD hv_tycj;

	MEMSET_DEBUG(&hv_tycj,0,sizeof(hv_tycj));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));

	pack105->opeType='0';/*大额*/
	pack105->PRI=hv_payin->T_operlevel[0];				/*优先级别*/
	MEMCPY_DEBUG(pack105->CMTCode,"105",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack105->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack105->workDate,hv_payin->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack105,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*货币种类*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*交易金额*/
	iHVSETTAG(pack105,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*货币+金额*/
	iHVSETTAG(pack105,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack105,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack105,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack105,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack105,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*业务种类*/
	iHVSETTAG(pack105,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	iHVSETTAG(pack105,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack105,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVSETTAG(pack105,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	MEMCPY_DEBUG(cAddid,hv_payin->T_addid , sizeof(cAddid)-1);
	zip_space(cAddid);
	if (strlen(cAddid)==8)
	{
		iHvGetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,cAddid,"000",&hv_tycj,sizeof(hv_tycj));
		iHVSETTAG(pack105,"CF3",hv_tycj.rate,sizeof(hv_tycj.rate));				/*拆借利率*/
		iHVSETTAG(pack105,"CF4",hv_tycj.ratelimit,sizeof(hv_tycj.ratelimit));		/*拆借期限*/
	}
	/*******************************************************************/
	return 0; 
}
int pub_com_CMT108_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack108)
{   
	/**************退汇支付报文***************************************/ 
	char cTmpBuf[19];	

	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	pack108->opeType='0';/*大额*/
	pack108->PRI=hv_payin->T_operlevel[0];				/*优先级别*/	
	MEMCPY_DEBUG(pack108->CMTCode,"108",3);					/*CMT编号*/		
	MEMCPY_DEBUG(pack108->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack108->workDate,hv_payin->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack108,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*货币类型*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*交易金额*/
	iHVSETTAG(pack108,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack108,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack108,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack108,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack108,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*货币+金额*/
	iHVSETTAG(pack108,"051",hv_payin->T_o_wt_date,sizeof(hv_payin->T_o_wt_date));	/*原委托日期*/
	iHVSETTAG(pack108,"005",hv_payin->T_o_orderno,sizeof(hv_payin->T_o_orderno));	/*原交易序号*/
	iHVSETTAG(pack108,"02B",hv_payin->T_o_cmtno,sizeof(hv_payin->T_o_cmtno));		/*原cmt编号*/
	iHVSETTAG(pack108,"CQ1",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*原付款人帐号*/
	iHVSETTAG(pack108,"CR1",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*原付款人姓名*/
	iHVSETTAG(pack108,"CQ2",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*原收款人帐号*/
	iHVSETTAG(pack108,"CR2",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*原收款人姓名*/
	iHVSETTAG(pack108,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	iHVSETTAG(pack108,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack108,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVSETTAG(pack108,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	/********没有附加域*********/	
	return 0; 
}
int pub_com_CMT109_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack109)
{   
	/**********电子联行专用汇兑报文************************************/ 
	char cTmpBuf[19];

	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	pack109->opeType='0';/*大额*/
	pack109->PRI=hv_payin->T_operlevel[0];				/*优先级别*/
	MEMCPY_DEBUG(pack109->CMTCode,"109",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack109->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack109->workDate,hv_payin->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack109,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*货币种类*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*交易金额*/
	iHVSETTAG(pack109,"32A",cTmpBuf,sizeof(cTmpBuf));					/*货币+金额*/
	iHVSETTAG(pack109,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack109,"CG1",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*发起行号*/
	iHVSETTAG(pack109,"CG2",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVSETTAG(pack109,"CG3",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVSETTAG(pack109,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack109,"CG4",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack109,"CG5",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVSETTAG(pack109,"CG6",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVSETTAG(pack109,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	iHVSETTAG(pack109,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack109,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVSETTAG(pack109,"CG7",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	return 0; 
}
int pub_com_CMT119_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack119)
{    
	/***************通用借记支付报文**********************************/
	char cTmpBuf[19];

	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	pack119->opeType='0';/*大额*/
	pack119->PRI=hv_payin->T_operlevel[0];				/*优先级别*/
	MEMCPY_DEBUG(pack119->CMTCode,"119",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack119->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack119->workDate,hv_payin->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack119,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*货币种类*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*交易金额*/
	iHVSETTAG(pack119,"32A",cTmpBuf,sizeof(cTmpBuf));					/*货币+金额*/
	iHVSETTAG(pack119,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack119,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack119,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVSETTAG(pack119,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVSETTAG(pack119,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVSETTAG(pack119,"59B",hv_payin->T_cash_addr,sizeof(hv_payin->T_cash_addr));	/*收款人地址*/
	iHVSETTAG(pack119,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack119,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack119,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVSETTAG(pack119,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVSETTAG(pack119,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVSETTAG(pack119,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*付款人地址*/
	iHVSETTAG(pack119,"20A",hv_payin->T_note_no,sizeof(hv_payin->T_note_no));		/*凭证号码*/
	iHVSETTAG(pack119,"OBF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*业务种类*/
	iHVSETTAG(pack119,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	iHVSETTAG(pack119,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack119,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVSETTAG(pack119,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	/********没有附加域***************/	
	return 0; 
}
int pub_com_CMT121_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack121)
{   
	/***************银行汇票资金移存报文******************************/ 
	char cTmpBuf[19];	
	HV_121_ADD hv_121;/****附加域*****/

	MEMSET_DEBUG(&hv_121,0,sizeof(hv_121));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));
	pack121->opeType='0';/*大额*/
	pack121->PRI=hv_payin->T_operlevel[0];				/*优先级别*/
	MEMCPY_DEBUG(pack121->CMTCode,"121",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack121->workDate,hv_payin->T_tx_date,8);	/*交易日期*/

	vtcp_log("[%s][%d]开始获取附加于\n",__FILE__,__LINE__);
	vtcp_log("处理附加于:tx_date=[%s],wt_date=[%s],addid=[%s]\n",
		hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid);
	iHvGetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000" ,&hv_121,sizeof(hv_121));
	vtcp_log("%s,%d 汇票附加域==[%s] ",__FILE__,__LINE__,(char*)&hv_121);

	MEMCPY_DEBUG(pack121->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	iHVSETTAG( pack121,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*货币种类*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*交易金额*/
	iHVSETTAG(pack121,"32A",cTmpBuf,sizeof(cTmpBuf));					/*货币+金额*/
	iHVSETTAG(pack121,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack121,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack121,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack121,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack121,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVSETTAG(pack121,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVSETTAG(pack121,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVSETTAG(pack121,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*付款人地址*/
	iHVSETTAG(pack121,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVSETTAG(pack121,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	iHVSETTAG(pack121,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack121,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVSETTAG(pack121,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));		/*附言*/

	/****处理附加域******/	
	{	
	char cSpec_br_no[13];
	MEMSET_DEBUG(cSpec_br_no , 0 , sizeof(cSpec_br_no));
	MEMCPY_DEBUG(cSpec_br_no,hv_121.spec_br_no,sizeof(cSpec_br_no));
	zip_space(cSpec_br_no);
	iHVSETTAG(pack121,"C33",cSpec_br_no,sizeof(cSpec_br_no)-1);	/*现金汇票兑付行*/
	}
	iHVSETTAG(pack121,"30B",hv_121.pj_date,sizeof(hv_121.pj_date));	/*票据日期*/
	vtcp_log("%s,%d 汇票附加域==[%s] ",__FILE__,__LINE__,(char*)&hv_121);
	iHVSETTAG(pack121,"21A",hv_121.pj_num,sizeof(hv_121.pj_num));		/*票据号码*/
	iHVSETTAG(pack121,"D02",hv_121.hp_type,sizeof(hv_121.hp_type));	/*票据类型*/
	iHVSETTAG(pack121,"C10",hv_121.hp_miya,sizeof(hv_121.hp_miya));		/*汇票密押*/


	return 0; 
}
int pub_com_CMT301_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack301)
{    
	/****************查询报文*********************************/
	char cTmpBuf[19];

	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));

	pack301->opeType='2';/*其他*/
	MEMCPY_DEBUG(cTmpBuf,hv_notpay->T_o_cur_no,3);	
	vtcp_log("[%s][%d]cTmpBuf==[%s]\n",__FILE__,__LINE__,cTmpBuf);				/*货币种类*/
	del_dot(hv_notpay->T_o_tx_amt,cTmpBuf+3,15);/***交易金额**/
	vtcp_log("[%s][%d]cTmpBuf==[%s]\n",__FILE__,__LINE__,cTmpBuf);
	MEMCPY_DEBUG(pack301->CMTCode,"301",3);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);						/*CMT编号*/
	MEMCPY_DEBUG(pack301->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	MEMCPY_DEBUG(pack301->workDate,hv_notpay->T_tx_date,8);		/*交易日期*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG( pack301,"CJ9",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*查询日期*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"01C",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*查询行行号*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*发起清算行*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"056",hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));			/*查询书号*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"01D",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*查复行行号*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*接受清算行号*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*原委托日期*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));		/*原发起行行号*/
	iHVSETTAG(pack301,"CC2",hv_notpay->T_o_ac_br_no,sizeof(hv_notpay->T_o_ac_br_no));		/*原接受行号*/
	iHVSETTAG(pack301,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*原交易种类*/
	iHVSETTAG(pack301,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*原支付交易序号*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"CND",cTmpBuf,sizeof(cTmpBuf));						/*原货币符号,金额*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*发报中心代码*/
	iHVSETTAG(pack301,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*收报中心代码*/
	iHVSETTAG(pack301,"053",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));		/*查询内容*/

	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	/***********没有附加域***************/	
	return 0; 
}
int pub_com_CMT302_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack302)
{    
	/****************查复报文*********************************/
	char cTmpBuf[19];
	MEMSET_DEBUG(cTmpBuf,0x00,sizeof(cTmpBuf));

	pack302->opeType='2';/*其他*/
	MEMCPY_DEBUG(cTmpBuf,hv_notpay->T_o_cur_no,3);					/*货币种类*/
	del_dot(hv_notpay->T_o_tx_amt,cTmpBuf+3,15);/***交易金额**/
	MEMCPY_DEBUG(pack302->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack302->CMTCode,"302",3);						/*CMT编号*/
	MEMCPY_DEBUG(pack302->workDate,hv_notpay->T_tx_date,8);		/*交易日期*/
	iHVSETTAG( pack302,"CJ1",hv_notpay->T_res_date,sizeof(hv_notpay->T_res_date));		/*查复日期*/
	iHVSETTAG(pack302,"01D",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*查复行行号*/
	iHVSETTAG(pack302,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*接收清算行行号*/
	iHVSETTAG(pack302,"043",hv_notpay->T_res_orderno,sizeof(hv_notpay->T_res_orderno));			/*查复书号*/
	iHVSETTAG(pack302,"CCB",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*原查询行行号*/
	iHVSETTAG(pack302,"CCB",hv_notpay->T_req_br_no,sizeof(hv_notpay->T_req_br_no));		/*原查询行行号*/
	iHVSETTAG(pack302,"CCC",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*原查询清算行号*/
	iHVSETTAG(pack302,"CP1",hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));		/*原查询书号*/
	iHVSETTAG(pack302,"CJA",hv_notpay->T_req_date,sizeof(hv_notpay->T_req_date));		/*原查询日期*/
	iHVSETTAG(pack302,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*原委托日期*/
	iHVSETTAG(pack302,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));		/*原发起行行号*/
	iHVSETTAG(pack302,"CC2",hv_notpay->T_o_ac_br_no,sizeof(hv_notpay->T_o_ac_br_no));		/*原接受行号*/
	iHVSETTAG(pack302,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*原交易种类*/
	iHVSETTAG(pack302,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*原支付交易序号*/
	iHVSETTAG(pack302,"CND",cTmpBuf,sizeof(cTmpBuf));						/*原货币符号,金额*/
	iHVSETTAG(pack302,"010",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*发报中心代码*/
	iHVSETTAG(pack302,"0B9",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*收报中心代码*/
	iHVSETTAG(pack302,"053",hv_notpay->T_res_content,sizeof(hv_notpay->T_res_content));		/*查复内容*/

    vtcp_log("[%s][%d] hv_notpay->T_req_br_no=[%.12s] ",__FILE__,__LINE__,hv_notpay->T_req_br_no);  
    vtcp_log("[%s][%d] hv_notpay->T_req_date=[%.8s] ",__FILE__,__LINE__,hv_notpay->T_req_date);  
    vtcp_log("[%s][%d] hv_notpay->T_req_orderno=[%.8s] ",__FILE__,__LINE__,hv_notpay->T_req_orderno);  
 
 
 
	/***********没有附加域***************/	
	return 0; 
}
int pub_com_CMT303_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack303)
{
	/*************自由格式报文********************/    

	pack303->opeType='2';/*其他*/
	MEMCPY_DEBUG(pack303->CMTCode,"303",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack303->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack303->workDate,hv_notpay->T_tx_date,8);	/*交易日期*/
	MEMCPY_DEBUG(pack303->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	iHVSETTAG( pack303,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));	/*委托日期*/
	iHVSETTAG(pack303,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));	/*特许参与者*/
	iHVSETTAG(pack303,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));	/*发起清算行*/
	iHVSETTAG(pack303,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));	/*接受行号*/
	iHVSETTAG(pack303,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));	/*接受清算行号*/
	iHVSETTAG(pack303,"053",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));	/*附言*/
	iHVSETTAG(pack303,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));		/*发报中心代码*/
	iHVSETTAG(pack303,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));		/*收报中心代码*/

	/****没有附加域***********/	
	return 0; 
}
int pub_com_CMT311_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack311)
{   
	/******申请撤销报文***********************************************/ 
	char	cSub_content[61];
	char	cSub_orderno[4];

	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));
	MEMSET_DEBUG(cSub_orderno,0,sizeof(cSub_orderno));

	MEMCPY_DEBUG(cSub_content,hv_notpay->T_req_content,60);/***由于是申请类 所以从 req取**/
	vtcp_log("[%s][%d]T_orderno=[%s]",__FILE__,__LINE__,hv_notpay->T_orderno);
	vtcp_log("[%s][%d]T_orderno=[%.3s]",__FILE__,__LINE__,hv_notpay->T_orderno);
	MEMCPY_DEBUG(cSub_orderno,hv_notpay->T_orderno+5,sizeof(cSub_orderno)-1);
	zip_space(cSub_orderno);
	vtcp_log("[%s][%d]cSub_orderno=[%s]",__FILE__,__LINE__,cSub_orderno);
	pack311->opeType='2';/*大额*/
	MEMCPY_DEBUG(pack311->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack311->CMTCode,"311",3);					/*CMT编号*/
	MEMCPY_DEBUG(pack311->workDate,hv_notpay->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack311,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));	/*委托日期*/
	iHVSETTAG(pack311,"C47",cSub_orderno,sizeof(cSub_orderno)-1);				/*申请号*/
	iHVSETTAG(pack311,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));	/*原委托日期*/
	iHVSETTAG(pack311,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));	/*原支付交易发起行*/
	iHVSETTAG(pack311,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));	/*原交易种类*/
	iHVSETTAG(pack311,"046",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));/*原支付交易序号*/
	iHVSETTAG(pack311,"72A",cSub_content,sizeof(cSub_content));					/*附言*/
	/***没有附加域*****/	
	return 0; 
}
int pub_com_CMT312_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack312)
{    
	/****************撤销应答报文*************/
	char	cSub_content[61];
	char	cSub_orderno[4];
	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));
	MEMSET_DEBUG(cSub_orderno,0,sizeof(cSub_orderno));
	MEMCPY_DEBUG(cSub_content,hv_notpay->T_res_content,60);/**应答类交易从res中取***/
	MEMCPY_DEBUG(cSub_orderno,hv_notpay->T_req_orderno,3); /*截取3位ORDERNO作为书号,对于应答类的交易,需要找到原来收到的申请类交易, 找到后,读取该记录中的非支付交易序号 到结构体的字段T_req_orderno中***/
	pack312->opeType='2';/*大额*/

	MEMCPY_DEBUG(pack312->CMTCode,"312",3);						/*CMT编号*/
	MEMCPY_DEBUG(pack312->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack312->workDate,hv_notpay->T_tx_date,8);		/*交易日期*/
	iHVSETTAG( pack312,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*委托日期*/
	iHVSETTAG(pack312,"CCD",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*申请撤销行号*/
	iHVSETTAG(pack312,"CP2",cSub_orderno,sizeof(cSub_orderno));					/*原撤销申请号*/
	iHVSETTAG(pack312,"046",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*撤销支付交易序号*/
	iHVSETTAG(pack312,"04C",hv_notpay->T_respcode,sizeof(hv_notpay->T_respcode));		/*撤销应答*/
	iHVSETTAG(pack312,"72A",cSub_content,sizeof(cSub_content));					/*附言*/
	/****没有附加域*****/	
	return 0; 
}
int pub_com_CMT313_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack313)
{    
	/****************申请退回报文*********************************/
	char cTmpBuf[19];
	char cSub_content[61];/***截取60位字段,60位与255位公用变量的***/
	char cSub_orderno[4];/***截取交易序号作为帐号*****/
	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));
	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));
	MEMSET_DEBUG(cSub_orderno,0,sizeof(cSub_orderno));

	MEMCPY_DEBUG(cSub_orderno,hv_notpay->T_orderno+5,3);/**截取orderno的后3位***/
	MEMCPY_DEBUG(cSub_content,hv_notpay->T_req_content,60);/***截取60位作为附言**/

	pack313->opeType='2';/*大额*/
	MEMCPY_DEBUG(pack313->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(cTmpBuf,hv_notpay->T_o_cur_no,3);				/*货币种类*/
	del_dot(hv_notpay->T_o_tx_amt,cTmpBuf+3,15);/**交易金额**/
	MEMCPY_DEBUG(pack313->CMTCode,"313",3);					/* CMT编号*/
	MEMCPY_DEBUG(pack313->workDate,hv_notpay->T_tx_date,8);	/*交易日期*/
	iHVSETTAG( pack313,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*委托日期*/
	iHVSETTAG(pack313,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*发起行行号*/
	iHVSETTAG(pack313,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*发起清算行*/
	iHVSETTAG(pack313,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*发报中心代码*/
	iHVSETTAG(pack313,"C47",cSub_orderno,sizeof(cSub_orderno));					/*退回申请号*/
	iHVSETTAG(pack313,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*接受行行号*/
	iHVSETTAG(pack313,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*接受清算行号*/
	iHVSETTAG(pack313,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*收报中心代码*/
	iHVSETTAG(pack313,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*原委托日期*/
	iHVSETTAG(pack313,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*原交易种类*/
	iHVSETTAG(pack313,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*原支付交易序号*/
	iHVSETTAG(pack313,"CND",cTmpBuf,sizeof(cTmpBuf));						/*原货币符号,金额*/
	iHVSETTAG(pack313,"72A",cSub_content,sizeof(cSub_content));					/*附言*/

	/***********没有附加域***************/	
	return 0; 
}
int pub_com_CMT314_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack314)
{    
	/****************退回应答报文*********************************/
	char cTmpBuf[19];
	char cSub_content[61];/***截取60位字段,60位与255位公用变量的***/
	char cSub_orderno[4];/***截取交易序号作为帐号*****/

	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));
	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));
	MEMSET_DEBUG(cSub_orderno,0,sizeof(cSub_orderno));

	MEMCPY_DEBUG(cSub_orderno,hv_notpay->T_req_orderno+5,3);/****截取orderno的3位,应答类交易需要从其对应的申请序号中截取字段,说明参照报文312**/
	MEMCPY_DEBUG(cSub_content,hv_notpay->T_res_content,60);/***截取60位作为附言**/

	pack314->opeType='2';/*大额*/
	MEMCPY_DEBUG(cTmpBuf,hv_notpay->T_o_cur_no,3);					/*货币种类*/
	del_dot(hv_notpay->T_o_tx_amt,cTmpBuf+3,15);/**交易金额**/
	MEMCPY_DEBUG(pack314->CMTCode,"314",3);						/*CMT编号*/
	MEMCPY_DEBUG(pack314->workDate,hv_notpay->T_tx_date,8);		/*交易日期*/
	MEMCPY_DEBUG(pack314->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	iHVSETTAG( pack314,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*委托日期*/
	iHVSETTAG(pack314,"CCE",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_or_br_no));		/*退回行行号*/
	iHVSETTAG(pack314,"011",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*发起清算行 我行的写在了cash中*/
	iHVSETTAG(pack314,"010",hv_notpay->T_receco,sizeof(hv_notpay->T_sendco));			/*发报中心代码*/
	iHVSETTAG(pack314,"58A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_ac_br_no));		/*接受行行号*/
	iHVSETTAG(pack314,"012",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*接受清算行号*/
	iHVSETTAG(pack314,"0B9",hv_notpay->T_sendco,sizeof(hv_notpay->T_receco));			/*收报中心代码*/
	iHVSETTAG(pack314,"CJB",hv_notpay->T_req_date,sizeof(hv_notpay->T_req_date));		/*原退回申请日期*/
	iHVSETTAG(pack314,"CP2",cSub_orderno,sizeof(cSub_orderno));					/*原退回申请序号*/
	iHVSETTAG(pack314,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*原委托日期*/
	iHVSETTAG(pack314,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*原交易种类*/
	iHVSETTAG(pack314,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*原支付交易序号*/
	iHVSETTAG(pack314,"CND",cTmpBuf,sizeof(cTmpBuf));						/*原货币符号,金额*/
	iHVSETTAG(pack314,"018",hv_notpay->T_respcode,sizeof(hv_notpay->T_respcode));		/*退回应答*/
	iHVSETTAG(pack314,"72A",cSub_content,sizeof(cSub_content));					/*附言*/

	/***********没有附加域***************/	
	return 0; 
}
int pub_com_CMT721_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack721)
{   
	/********代理兑付城市商业银行银行汇票的商业银行专用**************/ 
	char cAddid[9];
	char cSub_content[61];
	HV_721_ADD hv_721;

	MEMSET_DEBUG(&hv_721,0,sizeof(hv_721));
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));
	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));

	MEMCPY_DEBUG(cSub_content,hv_notpay->T_req_content,60);/**从255的内容中截取60个 字段,因为录入的60位的长度放到了T_req_content字段中**/

	pack721->opeType='2';/*大额*/
	MEMCPY_DEBUG(pack721->CMTCode,"721",3);						/*CMT编号*/
	MEMCPY_DEBUG(pack721->workDate,hv_notpay->T_tx_date,8);		/*交易日期*/
	MEMCPY_DEBUG(pack721->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	vtcp_log("[%s][%d]hv_notpay.T_wt_date=[%.8s]",__FILE__,__LINE__,hv_notpay->T_wt_date);
	iHVSETTAG( pack721,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*委托日期*/
	vtcp_log("[%s][%d]hv_notpay.T_wt_date=[%.8s]",__FILE__,__LINE__,hv_notpay->T_wt_date);
	iHVSETTAG(pack721,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack721,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack721,"72A",cSub_content,sizeof(cSub_content));					/*附言*/
	vtcp_log("body**********==[%s]",pack721->body);
	MEMCPY_DEBUG(cAddid,hv_notpay->T_addid , sizeof(cAddid)-1);
	zip_space(cAddid);
	if (strlen(cAddid)==8)
	{
		iHvGetAddit(hv_notpay->T_tx_date,hv_notpay->T_wt_date,cAddid,"000",&hv_721,sizeof(hv_721));
		iHVSETTAG(pack721,"30B",hv_721.pj_date,sizeof(hv_721.pj_date));		/*票据日期*/
		iHVSETTAG(pack721,"21A",hv_721.pj_num,sizeof(hv_721.pj_num));		/*票据号码*/
		iHVSETTAG(pack721,"D02",hv_721.hp_type,sizeof(hv_721.hp_type));		/*汇票类别*/
		iHVSETTAG(pack721,"C10",hv_721.hp_miya,sizeof(hv_721.hp_miya));		/*汇票密押*/
		del_dot(hv_721.cp_amt,hv_721.cp_amt,15);
		iHVSETTAG(pack721,"33C",hv_721.cp_amt,sizeof(hv_721.cp_amt));		/*出票金额*/
		vtcp_log("%s,%d real_js_amt=[%.15s],change_real_js_amt=[%.15s]",__FILE__,
			__LINE__,hv_721.real_js_amt,hv_721.real_js_amt);
		del_dot(hv_721.real_js_amt,hv_721.real_js_amt,15);
		vtcp_log("%s,%d real_js_amt=[%.15s],change_real_js_amt=[%.15s]",__FILE__,
			__LINE__,hv_721.real_js_amt,hv_721.real_js_amt);
		iHVSETTAG(pack721,"CNV",hv_721.real_js_amt,sizeof(hv_721.real_js_amt));	/*实际结算金额*/
		del_dot(hv_721.dy_amt,hv_721.dy_amt,15);
		iHVSETTAG(pack721,"33D",hv_721.dy_amt,sizeof(hv_721.dy_amt));		/*多余金额*/
		iHVSETTAG(pack721,"59E",hv_721.last_ac_no,sizeof(hv_721.last_ac_no));	/*最后持票人账号*/
		iHVSETTAG(pack721,"59D",hv_721.last_name,sizeof(hv_721.last_name));		/*最后持票人名称*/
		iHVSETTAG(pack721,"30C",hv_721.return_date,sizeof(hv_721.return_date));	/*提示付款日期*/
		iHVSETTAG(pack721,"CC4",hv_721.pay_opn_br_no,sizeof(hv_721.pay_opn_br_no));	/*付款人开户行号*/
		iHVSETTAG(pack721,"CC5",hv_721.cash_opn_br_no,sizeof(hv_721.cash_opn_br_no));/*收款人开户行号*/

	}
	return 0; 
}
int pub_com_CMT724_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack724)
{   
	/********代理兑付城市商业银行银行汇票的商业银行专用**************/ 
	char cAddid[9];
	char cSub_content[61];
	HV_724_ADD hv_724;

	MEMSET_DEBUG(&hv_724,0,sizeof(hv_724));
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));
	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));

	MEMCPY_DEBUG(cSub_content,hv_notpay->T_req_content,60);/**从255的内容中截取60个 字段,因为录入的60位的长度放到了T_req_content字段中**/

	pack724->opeType='2';/*大额*/
	MEMCPY_DEBUG(pack724->CMTCode,"724",3);						/*CMT编号*/
	MEMCPY_DEBUG(pack724->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	vtcp_log("[%s][%d]hv_notpay.T_wt_date=[%.8s]",__FILE__,__LINE__,hv_notpay->T_wt_date);
	MEMCPY_DEBUG(pack724->workDate,hv_notpay->T_tx_date,8);		/*交易日期*/
	iHVSETTAG( pack724,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*委托日期*/
	iHVSETTAG(pack724,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*特许参与者*/
	iHVSETTAG(pack724,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*接受行号*/
	iHVSETTAG(pack724,"72A",cSub_content,sizeof(cSub_content));					/*附言*/

	MEMCPY_DEBUG(cAddid,hv_notpay->T_addid , sizeof(cAddid)-1);
	zip_space(cAddid);
	if (strlen(cAddid)==8)
	{
		/* begin add by LiuHuafeng 20061002 */
		char cRec_br_no[13];
		MEMSET_DEBUG(cRec_br_no, 0 , sizeof(cRec_br_no));
		/* end by LiuHuafeng 20061002 */
		iHvGetAddit(hv_notpay->T_tx_date,hv_notpay->T_wt_date,cAddid,"000",&hv_724,sizeof(hv_724));
		iHVSETTAG(pack724,"30B",hv_724.pj_date,sizeof(hv_724.pj_date));		/*票据日期*/
		iHVSETTAG(pack724,"21A",hv_724.pj_num,sizeof(hv_724.pj_num));		/*票据号码*/
		iHVSETTAG(pack724,"D02",hv_724.hp_type,sizeof(hv_724.hp_type));		/*汇票类别*/
		MEMCPY_DEBUG(cRec_br_no, hv_724.df_br_no,sizeof(cRec_br_no)-1);
		zip_space(cRec_br_no);
		iHVSETTAG(pack724,"D01",cRec_br_no,sizeof(cRec_br_no)-1);		/*原指定兑付行号*/
		del_dot(hv_724.cp_amt,hv_724.cp_amt,15);
		iHVSETTAG(pack724,"33C",hv_724.cp_amt,sizeof(hv_724.cp_amt));		/*出票金额*/
		iHVSETTAG(pack724,"C10",hv_724.hp_miya,sizeof(hv_724.hp_miya));		/*汇票密押*/
		iHVSETTAG(pack724,"CC4",hv_724.pay_opn_br_no,sizeof(hv_724.pay_opn_br_no));	/*付款人开户行号*/
	}
	return 0; 
}

/************************
*       PART 2       ***
*   来帐报文处理接口 ***
************************/

int pub_com_CMT100_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack100)
{    
	char	cTmpBuf[19];
	double  dTxamt     = 0.0;
	MEMSET_DEBUG(cTmpBuf,0x00,sizeof(cTmpBuf));


	MEMCPY_DEBUG(hv_payin->T_cmtno,pack100->CMTCode,3);		/*CMT编号*/
	hv_payin->T_operlevel[0] = pack100->PRI;			/**优先级**/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack100->workDate,8);	/**交易日期**/
	iHVGETTAG(pack100,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack100,"32A",cTmpBuf,sizeof(cTmpBuf));	/*交易金额*/
	vtcp_log("%s,%d tmpbuf===[%s]",__FILE__,__LINE__,cTmpBuf);
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*货币种类*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack100,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack100,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack100,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVGETTAG(pack100,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVGETTAG(pack100,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVGETTAG(pack100,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*付款人地址*/
	iHVGETTAG(pack100,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack100,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack100,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVGETTAG(pack100,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVGETTAG(pack100,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVGETTAG(pack100,"59B",hv_payin->T_cash_addr,sizeof(hv_payin->T_cash_addr));	/*收款人地址*/
	iHVGETTAG(pack100,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*业务种类*/
	iHVGETTAG(pack100,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack100,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack100,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVGETTAG(pack100,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/
	/******汇兑业务没有附加域***************/	
	return 0; 
}
int pub_com_CMT101_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack101)
{  
	/***************委托收款（划回）支付报文***************************/  
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;

	HV_WTSK_ADD  hv_wtsk;/***附加域****/

	MEMSET_DEBUG(&hv_wtsk,0,sizeof(hv_wtsk));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack101->PRI;			/**优先级别**/
	MEMCPY_DEBUG(hv_payin->T_cmtno,pack101->CMTCode,3);		/**CMT编号**/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack101->workDate,8);	/**交易日期**/
	iHVGETTAG(pack101,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/**委托日期**/
	iHVGETTAG(pack101,"32A",cTmpBuf,sizeof(cTmpBuf));					/*交易金额*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/**货币种类**/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack101,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack101,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack101,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVGETTAG(pack101,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVGETTAG(pack101,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVGETTAG(pack101,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack101,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack101,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVGETTAG(pack101,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVGETTAG(pack101,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVGETTAG(pack101,"CEJ",hv_payin->T_note_type,sizeof(hv_payin->T_note_type));	/*凭证种类*/
	iHVGETTAG(pack101,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*业务种类*/
	iHVGETTAG(pack101,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack101,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack101,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVGETTAG(pack101,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	/***添加附加域*****/	
	iHVGETTAG(pack101,"30B",hv_wtsk.pj_date,sizeof(hv_wtsk.pj_date));		/*票据日期*/
	iHVGETTAG(pack101,"21A",hv_wtsk.pj_num,sizeof(hv_wtsk.pj_num));			/*票据号码*/
	/*********iHvSetAddit函数将对hv_payin->T_addid字段赋值***/
	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_wtsk,sizeof(hv_wtsk));

	return 0; 
}

int pub_com_CMT102_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack102)
{   
	/*****************托收承付（划回）支付报文*************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	HV_TSCF_ADD hv_tscf;  /****处理附加域***********/

	MEMSET_DEBUG(&hv_tscf,0,sizeof(hv_tscf));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack102->PRI;			/**优先级**/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"102",3);					/**CMT编号**/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack102->workDate,8);	/**交易日期**/
	iHVGETTAG(pack102,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack102,"32A",cTmpBuf,sizeof(cTmpBuf));					/**货币+金额*/	
	iHVGETTAG(pack102,"32A",cTmpBuf,sizeof(cTmpBuf));					/*交易金额*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/**货币种类**/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/

	iHVGETTAG(pack102,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack102,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack102,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVGETTAG(pack102,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVGETTAG(pack102,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVGETTAG(pack102,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack102,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack102,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVGETTAG(pack102,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVGETTAG(pack102,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVGETTAG(pack102,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack102,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack102,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVGETTAG(pack102,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));		/*附言*/

	/***处理附加域**/	
	iHVGETTAG(pack102,"30B",hv_tscf.pj_date,sizeof(hv_tscf.pj_date));		/*票据日期*/
	iHVGETTAG(pack102,"21A",hv_tscf.pj_num,sizeof(hv_tscf.pj_num));		/*票据号码*/
	iHVGETTAG(pack102,"33A",hv_tscf.ptxamt,sizeof(hv_tscf.ptxamt));		/*赔偿金额*/
	iHVGETTAG(pack102,"33B",hv_tscf.jtxamt,sizeof(hv_tscf.jtxamt));		/*拒付金额*/

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_tscf,sizeof(hv_tscf));

	return 0; 
}
int pub_com_CMT103_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack103)
{   
	/****************国库资金汇划（贷记）支付报文**********************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack103->PRI;			/*优先级*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"103",3);					/*CMT编号*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack103->workDate,8);	/*交易日期*/
	iHVGETTAG(pack103,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack103,"32A",cTmpBuf,sizeof(cTmpBuf));					/*金额 货币*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*货币类型*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack103,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack103,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack103,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVGETTAG(pack103,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVGETTAG(pack103,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVGETTAG(pack103,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack103,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack103,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVGETTAG(pack103,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVGETTAG(pack103,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVGETTAG(pack103,"OBF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*业务种类*/
	iHVGETTAG(pack103,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack103,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack103,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVGETTAG(pack103,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));		/*附言*/

	/*****没有附加域********/	
	return 0; 
}
int pub_com_CMT104_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack104)
{   
	double  dTxamt     = 0.0;
	/***************定期贷记支付报文***********************************/ 
	char cTmpBuf[19];	
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0]=pack104->PRI;				/*优先级*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"104",3);					/*CMT编号*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack104->workDate,8);	/*交易日期*/
	iHVGETTAG(pack104,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack104,"32A",cTmpBuf,sizeof(cTmpBuf));					/*货币 金额*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*货币类型*/	
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack104,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack104,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack104,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVGETTAG(pack104,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVGETTAG(pack104,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVGETTAG(pack104,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack104,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack104,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVGETTAG(pack104,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVGETTAG(pack104,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVGETTAG(pack104,"59B",hv_payin->T_cash_addr,sizeof(hv_payin->T_cash_addr));	/*收款人地址*/
	iHVGETTAG(pack104,"OBF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*业务种类*/
	iHVGETTAG(pack104,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack104,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack104,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVGETTAG(pack104,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	/**********没有附加域******************************/	
	return 0; 
}
int pub_com_CMT105_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack105)
{    
	/************银行间同业拆借支付报文********************************/
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	HV_TYCJ_ADD hv_tycj;

	MEMSET_DEBUG(&hv_tycj,0,sizeof(hv_tycj));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack105->PRI;			/*有限级别*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"105",3);					/*CMT编号*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack105->workDate,8);	/*交易日期*/
	iHVGETTAG( pack105,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack105,"32A",cTmpBuf,sizeof(cTmpBuf));					/*货币+金额*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*货币类型*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack105,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack105,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack105,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack105,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack105,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*业务种类*/
	iHVGETTAG(pack105,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack105,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack105,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVGETTAG(pack105,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	/******** 处理附加域**/	
	{
	char cRate[8];
	char cLimit[6];
	MEMSET_DEBUG(cRate, 0 , sizeof(cRate));
	MEMSET_DEBUG(cLimit, 0 , sizeof(cLimit));
	iHVGETTAG(pack105,"CF3",cRate,sizeof(hv_tycj.rate));				/*拆借利率*/
	iHVGETTAG(pack105,"CF4",cLimit,sizeof(hv_tycj.ratelimit));		/*拆借期限*/
	sprintf(cRate,"%07d",atoi(cRate));
	sprintf(cLimit,"%05d",atoi(cLimit));
	MEMCPY_DEBUG(hv_tycj.rate,cRate,sizeof(hv_tycj.rate));
	MEMCPY_DEBUG(hv_tycj.ratelimit,cLimit,sizeof(hv_tycj.ratelimit));
	}
	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_tycj,sizeof(hv_tycj));
	return 0; 
}
int pub_com_CMT108_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack108)
{   
	/**************退汇支付报文***************************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack108->PRI;			/*优先级*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"108",3);					/*CMT编号*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack108->workDate,8);	/*交易日期*/
	iHVGETTAG( pack108,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack108,"32A",cTmpBuf,sizeof(cTmpBuf));					/*货币+金额*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*货币类型*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack108,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack108,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack108,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack108,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack108,"051",hv_payin->T_o_wt_date,sizeof(hv_payin->T_o_wt_date));	/*原委托日期*/
	iHVGETTAG(pack108,"02B",hv_payin->T_o_cmtno,sizeof(hv_payin->T_o_cmtno));		/*原cmt编号*/
	iHVGETTAG(pack108,"CQ1",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*原付款人帐号*/
	iHVGETTAG(pack108,"CR1",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*原付款人姓名*/
	iHVGETTAG(pack108,"CQ2",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*原收款人帐号*/
	iHVGETTAG(pack108,"CR2",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*原收款人姓名*/
	iHVGETTAG(pack108,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack108,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack108,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVGETTAG(pack108,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/
  iHVGETTAG(pack108,"005",hv_payin->T_o_orderno,sizeof(hv_payin->T_o_orderno));		/*原支付交易序号*/

	/********没有附加域*********/	
	return 0; 
}
int pub_com_CMT122_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack122)
{   
	/***************清算银行汇票资金报文******************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	HV_HP_ADD hv_hp;/****附加域*****/

	MEMSET_DEBUG(&hv_hp,0,sizeof(hv_hp));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack122->PRI;			/*优先级*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"122",3);					/*CMT编号*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack122->workDate,8);	/*交易日期*/	
	iHVGETTAG( pack122,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack122,"32A",cTmpBuf,sizeof(cTmpBuf));					/*货币+金额*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*货币类型*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack122,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack122,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack122,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack122,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack122,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVGETTAG(pack122,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVGETTAG(pack122,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVGETTAG(pack122,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVGETTAG(pack122,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*付款人地址*/
	iHVGETTAG(pack122,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVGETTAG(pack122,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/

  /***add   2006-11-7 17:17  *****/
	iHVGETTAG(pack122,"59E",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_name));		/*最后持票人帐号*/	
	
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack122,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack122,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVGETTAG(pack122,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));		/*附言*/

	/****处理附加域******/	
	iHVGETTAG(pack122,"59E",hv_hp.last_ac_no,sizeof(hv_hp.last_ac_no));		/*最后持票人帐号*/
	iHVGETTAG(pack122,"59D",hv_hp.last_name,sizeof(hv_hp.last_name));		/*最后持票人姓名*/
	iHVGETTAG(pack122,"30B",hv_hp.pj_date,sizeof(hv_hp.pj_date));		/*票据日期*/
	iHVGETTAG(pack122,"21A",hv_hp.pj_num,sizeof(hv_hp.pj_num));			/*票据号码*/
	iHVGETTAG(pack122,"C10",hv_hp.hpmy,sizeof(hv_hp.hpmy));			/*汇票密押*/
	iHVGETTAG(pack122,"33C",hv_hp.cp_amt,sizeof(hv_hp.cp_amt));			/*出票金额*/
	iHVGETTAG(pack122,"33D",hv_hp.dy_amt,sizeof(hv_hp.dy_amt));			/*多余金额*/

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_hp,sizeof(hv_hp));
	return 0; 
}
int pub_com_CMT123_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack123)
{   
	/***************清算银行汇票资金报文******************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;

	HV_123_ADD hv_123;/****附加域*****/

	MEMSET_DEBUG(&hv_123,0,sizeof(hv_123));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack123->PRI;			/*优先级*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"123",3);					/*CMT编号*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack123->workDate,8);	/*交易日期*/	
	iHVGETTAG( pack123,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack123,"32A",cTmpBuf,sizeof(cTmpBuf));					/*货币+金额*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*货币类型*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack123,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack123,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack123,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack123,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack123,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVGETTAG(pack123,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVGETTAG(pack123,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVGETTAG(pack123,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVGETTAG(pack123,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*付款人地址*/
	iHVGETTAG(pack123,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack123,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack123,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVGETTAG(pack123,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	/****处理附加域******/	
	iHVGETTAG(pack123,"30B",hv_123.pj_date,sizeof(hv_123.pj_date));		/*票据日期*/
	iHVGETTAG(pack123,"21A",hv_123.pj_num,sizeof(hv_123.pj_num));			/*票据号码*/
	iHVGETTAG(pack123,"C10",hv_123.hp_miya,sizeof(hv_123.hp_miya));			/*汇票密押*/

  MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));
  dTxamt = 0.0;
	iHVGETTAG(pack123,"33C",cTmpBuf,sizeof(cTmpBuf));						/*出票金额*/
	iHvStr2Dbl(cTmpBuf,&dTxamt);
	dTxamt = dTxamt/100;
	vtcp_log("%s,%d  dTxamt====[%lf] ",__FILE__,__LINE__,dTxamt);	
	sprintf(hv_123.cp_amt,"%016.2f",dTxamt);	                  /*出票金额*/
	vtcp_log("%s,%d  hv_123.cp_amt==[%s]",__FILE__,__LINE__,hv_123.cp_amt);

  MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));
  dTxamt = 0.0;			
	iHVGETTAG(pack123,"CNV",cTmpBuf,sizeof(cTmpBuf));						/*实际结算金额*/
	iHvStr2Dbl(cTmpBuf,&dTxamt);
	dTxamt = dTxamt/100;
	vtcp_log("%s,%d  dTxamt====[%lf] ",__FILE__,__LINE__,dTxamt);	
	sprintf(hv_123.real_js_amt,"%016.2f",dTxamt);	              /*实际结算金额*/
	vtcp_log("%s,%d  hv_123.real_js_amt==[%s]",__FILE__,__LINE__,hv_123.real_js_amt);

	dTxamt   =  0.0;
	iHVGETTAG(pack123,"CC5",hv_123.cash_opn_br_no,sizeof(hv_123.cash_opn_br_no));	/*最后持票人开户行*/
	iHVGETTAG(pack123,"59E",hv_123.last_ac_no,sizeof(hv_123.last_ac_no));		/*最后持票人帐号*/
	iHVGETTAG(pack123,"59D",hv_123.last_name,sizeof(hv_123.last_name));		/*最后持票人姓名*/
	iHVGETTAG(pack123,"30C",hv_123.return_date,sizeof(hv_123.return_date));	/*提示付款日期*/
	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_123,sizeof(hv_123));
	return 0; 
}
int pub_com_CMT124_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack124)
{   
	/***************银行汇票未用退回资金报文************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	HV_124_ADD hv_124;/****附加域*****/

	MEMSET_DEBUG(&hv_124,0,sizeof(hv_124));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack124->PRI;			/*优先级*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"124",3);					/*CMT编号*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack124->workDate,8);	/*交易日期*/	
	iHVGETTAG( pack124,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack124,"32A",cTmpBuf,sizeof(cTmpBuf));					/*货币+金额*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*货币类型*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack124,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack124,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack124,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack124,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack124,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVGETTAG(pack124,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVGETTAG(pack124,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVGETTAG(pack124,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*付款人地址*/
	iHVGETTAG(pack124,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVGETTAG(pack124,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack124,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack124,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*收报中心代码*/
	iHVGETTAG(pack124,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/

	/****处理附加域******/	
	iHVGETTAG(pack124,"C33",hv_124.spec_br_no,sizeof(hv_124.spec_br_no));		/*现金支票汇兑行*/
	iHVGETTAG(pack124,"30B",hv_124.pj_date,sizeof(hv_124.pj_date));			/*票据日期*/
	iHVGETTAG(pack124,"21A",hv_124.pj_num,sizeof(hv_124.pj_num));			/*票据号码*/
	iHVGETTAG(pack124,"C10",hv_124.hp_miya,sizeof(hv_124.hp_miya));				/*汇票密押*/

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_124,sizeof(hv_124));
	return 0; 
}
int pub_com_CMT232_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack232)
{   
	/***************清算银行汇票资金报文******************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	HV_232_ADD hv_232;/****附加域*****/

	MEMSET_DEBUG(&hv_232,0,sizeof(hv_232));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack232->PRI;			/*优先级*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"232",3);					/*CMT编号*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack232->workDate,8);	/*交易日期*/	
	iHVGETTAG( pack232,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack232,"OB6",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack232,"32A",cTmpBuf,sizeof(cTmpBuf));					/*货币+金额*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*货币类型*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack232,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*人行的交易序号*/
	iHVGETTAG(pack232,"CC7",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*被借记行号*/
	iHVGETTAG(pack232,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*付款人开户行号*/
	iHVGETTAG(pack232,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*付款人账号*/
	iHVGETTAG(pack232,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*付款人名称*/
	iHVGETTAG(pack232,"CC8",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*被贷记行行号*/
	iHVGETTAG(pack232,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*收款人开户行号*/
	iHVGETTAG(pack232,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*收款人账号*/
	iHVGETTAG(pack232,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*收款人名称*/
	iHVGETTAG(pack232,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*附言*/
	strncpy(g_pub_tx.brf,hv_payin->T_hv_brf,sizeof(g_pub_tx.brf)-1);

	/****处理附加域******/	
	iHVGETTAG(pack232,"90A",hv_232.tag_90A,sizeof(hv_232.tag_90A));		/*交换、轧差场次/交易批次*/
	iHVGETTAG(pack232,"CEB",hv_232.tag_CEB,sizeof(hv_232.tag_CEB));		/*即时转账业务种类*/
	iHVGETTAG(pack232,"CF2",hv_232.tag_CF2,sizeof(hv_232.tag_CF2));		/*债券结算已匹配指令号 */
	iHVGETTAG(pack232,"CF1",hv_232.tag_CF1,sizeof(hv_232.tag_CF1));		/*债券代码 */
	iHVGETTAG(pack232,"CNN",hv_232.tag_CNN,sizeof(hv_232.tag_CNN));		/*净价金额 */
	dTxamt     = 0.0;
	str2dbl(hv_232.tag_CNN,15,0,&dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_232.tag_CNN,"%015.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack232,"CNP",hv_232.tag_CNP,sizeof(hv_232.tag_CNP));		/*债券利息*/
	dTxamt     = 0.0;
	str2dbl(hv_232.tag_CNP,15,0,&dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_232.tag_CNP,"%015.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack232,"CNM",hv_232.tag_CNM,sizeof(hv_232.tag_CNM));		/*债券面额*/	
	dTxamt     = 0.0;
	str2dbl(hv_232.tag_CNM,15,0,&dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_232.tag_CNM,"%015.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack232,"CNQ",hv_232.tag_CNQ,sizeof(hv_232.tag_CNQ));		/*回购首期结算额 */
	dTxamt     = 0.0;
	str2dbl(hv_232.tag_CNQ,15,0,&dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_232.tag_CNQ,"%015.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack232,"CJ8",hv_232.tag_CJ8,sizeof(hv_232.tag_CJ8));		/*回购到期日*/
	iHVGETTAG(pack232,"CNR",hv_232.tag_CNR,sizeof(hv_232.tag_CNR));		/*回购利息 */
	dTxamt     = 0.0;
	str2dbl(hv_232.tag_CNR,15,0,&dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_232.tag_CNR,"%015.2f",dTxamt);	/*货币金额*/
	iHVGETTAG(pack232,"90C",hv_232.tag_90C,sizeof(hv_232.tag_90C));		/*借贷标识*/
	iHVGETTAG(pack232,"CCF",hv_232.tag_CCF,sizeof(hv_232.tag_CCF));		/*被借记行所在CCPC代码*/
	iHVGETTAG(pack232,"CCG",hv_232.tag_CCG,sizeof(hv_232.tag_CCG));		/*被贷记行所在CCPC代码*/
	/* begin add by LiuHuafeng 2009-5-17 19:52:16 将借贷标志借用operlevl存放 */
	hv_payin->T_operlevel[0]=hv_232.tag_90C[0];
	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_232,sizeof(hv_232));
	return 0; 
}
int pub_com_CMT253_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack253)
{   
	/**************大额清算结果返回报文*********************************/ 
	MEMCPY_DEBUG(hv_notpay->T_cmtno,"253",3);					/*CMT编号*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack253->workDate,8);	/*交易日期*/
	iHVGETTAG( pack253,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack253,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack253,"0BC",hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));		/*支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));
	iHVGETTAG(pack253,"C42",hv_notpay->T_resp1,sizeof(hv_notpay->T_resp1));		/*处理码*/
vtcp_log("%s,%d,hv_notpay->T_orderno   [%s]",__FILE__,__LINE__,hv_notpay->T_orderno );
vtcp_log("%s,%d,hv_notpay->T_or_br_no  [%s]",__FILE__,__LINE__,hv_notpay->T_or_br_no);
vtcp_log("%s,%d,hv_notpay->T_wt_date   [%s]",__FILE__,__LINE__,hv_notpay->T_wt_date );
vtcp_log("%s,%d,hv_notpay->T_resp1     [%s]",__FILE__,__LINE__,hv_notpay->T_resp1   );


	return 0; 
}
int pub_com_CMT910_TAG2T(HV_NOTPAY_AREA * hv_notpayin,THVPACKAGE * pack)
{   
	/***************银行汇票资金移存报文******************************/ 
	iHVGETTAG(pack,"30A",hv_notpayin->T_wt_date,sizeof(hv_notpayin->T_wt_date));/*委托日期*/
	iHVGETTAG(pack,"C42",hv_notpayin->T_resp1  ,sizeof(hv_notpayin->T_resp1));	   /*处理码*/
	iHVGETTAG(pack,"72A",hv_notpayin->T_hv_brf ,sizeof(hv_notpayin->T_hv_brf));  /*附言*/
	MEMCPY_DEBUG(hv_notpayin->T_refno,pack->reNo,sizeof(hv_notpayin->T_refno)); /* 参考号 */
	return 0; 
}

int pub_com_CMT301_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack301)
{    
	/****************查询报文*********************************/
	char cTmpBuf[19];
	double  dTxamt     = 0.0;
	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));

	MEMCPY_DEBUG(hv_notpay->T_cmtno,"301",3);						/*CMT编号*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack301->workDate,8);		/*交易日期*/
	iHVGETTAG(pack301,"CND",cTmpBuf,sizeof(cTmpBuf));						/*原货币符号,金额*/
	MEMCPY_DEBUG(hv_notpay->T_o_cur_no,cTmpBuf,3);					/*货币类型*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	vtcp_log("cTmpBuf==[%s]\n",cTmpBuf);
	vtcp_log("来帐的金额dTxmat=[%f]\n",dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_notpay->T_o_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	vtcp_log("转化后的o_Tx_amt=[%s]\n",hv_notpay->T_o_tx_amt);
	iHVGETTAG(pack301,"CJ9",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));			/*查询日期*/
	iHVGETTAG(pack301,"01C",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*查询行行号*/
	iHVGETTAG(pack301,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*发起清算行*/
	iHVGETTAG(pack301,"056",hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));			/*查询书号*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));
	iHVGETTAG(pack301,"01D",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*查复行行号*/
	iHVGETTAG(pack301,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*接受清算行号*/
	iHVGETTAG(pack301,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*原委托日期*/
	iHVGETTAG(pack301,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));		/*原发起行行号*/
	iHVGETTAG(pack301,"CC2",hv_notpay->T_o_ac_br_no,sizeof(hv_notpay->T_o_ac_br_no));		/*原接受行号*/
	iHVGETTAG(pack301,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*原交易种类*/
	iHVGETTAG(pack301,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*原支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_o_payin_orderno,hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));
	iHVGETTAG(pack301,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*发报中心代码*/
	iHVGETTAG(pack301,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*收报中心代码*/
	iHVGETTAG(pack301,"053",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));		/*查询内容*/

	/***********没有附加域***************/	
	hv_notpay->T_hv_fzf_sts[0] = '9';
	vtcp_log("查询的初始状态为:[%c]\n",hv_notpay->T_hv_fzf_sts[0]);
	MEMCPY_DEBUG(hv_notpay->T_req_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_req_orderno));
	MEMCPY_DEBUG(hv_notpay->T_req_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_req_br_no));
	MEMCPY_DEBUG(hv_notpay->T_req_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_req_date));
	hv_notpay->T_otype[0]='1';/**大额**/
	return 0; 
}
int pub_com_CMT302_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack302)
{    
	/****************查复报文*********************************/
	char cTmpBuf[19];
	double  dTxamt     = 0.0;
	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));

	MEMCPY_DEBUG(hv_notpay->T_cmtno,"302",3);						/*CMT编号*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack302->workDate,8);		/*交易日期*/
	iHVGETTAG(pack302,"CND",cTmpBuf,sizeof(cTmpBuf));						/*原货币符号,金额*/
	MEMCPY_DEBUG(hv_notpay->T_o_cur_no,cTmpBuf,3);					/*货币类型*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_notpay->T_o_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG( pack302,"CJ1",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*查复日期*/
	iHVGETTAG(pack302,"01D",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*查复行行号*/
	iHVGETTAG(pack302,"012",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*查复接受清算行*/
	iHVGETTAG(pack302,"043",hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));			/*查复书号*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));
	iHVGETTAG(pack302,"CCB",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*原查询行行号*/
	iHVGETTAG(pack302,"CCC",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*原查询清算行号*/
	iHVGETTAG(pack302,"CP1",hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));		/*原查询书号*/
	del_dot(hv_notpay->T_req_orderno,hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));
	iHVGETTAG(pack302,"CJA",hv_notpay->T_req_date,sizeof(hv_notpay->T_req_date));		/*原查询日期*/
	iHVGETTAG(pack302,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*原委托日期*/
	iHVGETTAG(pack302,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));		/*原发起行行号*/
	iHVGETTAG(pack302,"CC2",hv_notpay->T_o_ac_br_no,sizeof(hv_notpay->T_o_ac_br_no));		/*原接受行号*/
	iHVGETTAG(pack302,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*原交易种类*/
	iHVGETTAG(pack302,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*原支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_o_payin_orderno,hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));
	iHVGETTAG(pack302,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*发报中心代码*/
	iHVGETTAG(pack302,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*收报中心代码*/
	iHVGETTAG(pack302,"053",hv_notpay->T_res_content,sizeof(hv_notpay->T_res_content));		/*查询内容*/

	/***********没有附加域***************/	
	hv_notpay->T_hv_fzf_sts[0] = '9';
	vtcp_log("查询的初始状态为:[%c]\n",hv_notpay->T_hv_fzf_sts[0]);
	MEMCPY_DEBUG(hv_notpay->T_req_br_no,hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_req_br_no));/***提取原查询行****/
	MEMCPY_DEBUG(hv_notpay->T_res_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_res_orderno));
	MEMCPY_DEBUG(hv_notpay->T_res_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_res_br_no));
	MEMCPY_DEBUG(hv_notpay->T_res_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_res_date));
	return 0; 
}
int pub_com_CMT303_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack303)
{
	/*************自由格式报文********************/

	MEMCPY_DEBUG(hv_notpay->T_cmtno,"303",3);					/*CMT编号*/ 
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack303->workDate,8);	/*交易日期*/
	iHVGETTAG( pack303,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));	/*委托日期*/
	iHVGETTAG(pack303,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));	/*特许参与者*/
	iHVGETTAG(pack303,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));	/*发起清算行*/
	iHVGETTAG(pack303,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));	/*接受行号*/
	iHVGETTAG(pack303,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));	/*接受清算行号*/
	iHVGETTAG(pack303,"053",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));	/*附言*/
	iHVGETTAG(pack303,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));		/*发报中心代码*/
	iHVGETTAG(pack303,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));		/*收报中心代码*/
		
  vtcp_log(" hv_notpay->T_cmtno[%s] ",hv_notpay->T_cmtno);
  vtcp_log(" hv_notpay->T_tx_date[%s] ",hv_notpay->T_tx_date);
  vtcp_log(" hv_notpay->T_wt_date[%s] ",hv_notpay->T_wt_date);
  vtcp_log(" hv_notpay->T_or_br_no[%s] ",hv_notpay->T_or_br_no);
  vtcp_log(" hv_notpay->T_pay_qs_no[%s] ",hv_notpay->T_pay_qs_no);
  vtcp_log(" hv_notpay->T_ac_br_no[%s] ",hv_notpay->T_ac_br_no);
  vtcp_log(" hv_notpay->T_cash_qs_no[%s] ",hv_notpay->T_cash_qs_no);
  vtcp_log(" hv_notpay->T_req_content[%s] ",hv_notpay->T_req_content);
  vtcp_log(" hv_notpay->T_sendco[%s] ",hv_notpay->T_sendco);
  vtcp_log(" hv_notpay->T_receco[%s] ",hv_notpay->T_receco);   
  
	/****没有附加域***********/	
	hv_notpay->T_hv_fzf_sts[0] = '9';
	if(iGetHvFzfOrderno(hv_notpay->T_orderno))
	{
   	    sprintf(acErrMsg,"[%s][%d]生成交易序号错误!\n",__FILE__,__LINE__);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"HV01");
	    return (-1);
		
	}
	MEMCPY_DEBUG(hv_notpay->T_req_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_req_orderno));
	MEMCPY_DEBUG(hv_notpay->T_req_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_req_br_no));
	MEMCPY_DEBUG(hv_notpay->T_req_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_req_date));
	return 0; 
}
int pub_com_CMT312_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack312)
{    
	/****************撤销应答报文*************/
	/***应答交易中的的原申请字段赋值到T_req_orderno中去**/
	MEMCPY_DEBUG(hv_notpay->T_cmtno,"312",3);						/*CMT编号*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack312->workDate,8);		/*交易日期*/
	iHVGETTAG(pack312,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));			/*委托日期*/
	iHVGETTAG(pack312,"CCD",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*申请撤销行号*/
	iHVGETTAG(pack312,"CP2",hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));		/*原撤销申请号*/
    vtcp_log("[%s][%d]原撤销申请序号req_orderno=[%s]\n",__FILE__,__LINE__,hv_notpay->T_req_orderno);
	/***add by liuyue*****/
	del_dot(hv_notpay->T_req_orderno,hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));
	iHVGETTAG(pack312,"046",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*撤销支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_o_payin_orderno,hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));
	iHVGETTAG(pack312,"04C",hv_notpay->T_respcode,sizeof(hv_notpay->T_respcode));		/*撤销应答*/
	iHVGETTAG(pack312,"72A",hv_notpay->T_res_content,sizeof(hv_notpay->T_res_content));		/*附言*/
	/****没有附加域*****/	
	/***add by liuyue 20060908***/
	if(iGetHvFzfOrderno(hv_notpay->T_orderno)) /***生成一个交易序号*****/
	{
   	    sprintf(acErrMsg,"[%s][%d]生成交易序号错误!\n",__FILE__,__LINE__);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"HV01");
	    return (-1);
		
	}
	hv_notpay->T_hv_fzf_sts[0] = '9';
	vtcp_log("查询的初始状态为:[%c]\n",hv_notpay->T_hv_fzf_sts[0]);
	MEMCPY_DEBUG(hv_notpay->T_res_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_res_orderno));
	MEMCPY_DEBUG(hv_notpay->T_res_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_res_br_no));
	MEMCPY_DEBUG(hv_notpay->T_res_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_res_date));
	return 0; 
}
int pub_com_CMT313_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack313)
{    
	/****************申请退回报文*********************************/
	double  dTxamt     = 0.0;
	/***退回申请号 赋值到orderno 字段****/
	char cTmpBuf[19];
	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));

	MEMCPY_DEBUG(hv_notpay->T_cmtno,"313",3);						/*CMT编号*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack313->workDate,8);		/*交易日期*/
	iHVGETTAG(pack313,"CND",cTmpBuf,sizeof(cTmpBuf));						/*原货币符号,金额*/
	MEMCPY_DEBUG(hv_notpay->T_o_cur_no,cTmpBuf,3);					/*货币类型*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	vtcp_log("!!!!!![%d]cTmpbuf==[%s]\n",__LINE__,cTmpBuf);
	vtcp_log("!!!!!![%d]cTmpbuf==[%s]\n",__LINE__,cTmpBuf);
	vtcp_log("!!!!!![%d]cTmpbuf==[%s]\n",__LINE__,cTmpBuf);
	dTxamt = dTxamt/100;
	vtcp_log("!!!!![%f]\n",dTxamt);
	vtcp_log("!!!!![%f]\n",dTxamt);
	sprintf(hv_notpay->T_o_tx_amt,"%16.2f",dTxamt);	/*货币金额*/
	vtcp_log("t_o_tx_amt==[%s][%d]\n",hv_notpay->T_o_tx_amt,sizeof(hv_notpay->T_o_tx_amt));
	vtcp_log("t_o_tx_amt==[%s][%d]\n",hv_notpay->T_o_tx_amt,sizeof(hv_notpay->T_o_tx_amt));
	vtcp_log("t_o_tx_amt==[%s][%d]\n",hv_notpay->T_o_tx_amt,sizeof(hv_notpay->T_o_tx_amt));
	iHVGETTAG( pack313,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack313,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*发起行行号*/
	iHVGETTAG(pack313,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*发起清算行*/
	iHVGETTAG(pack313,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*发报中心代码*/
	iHVGETTAG(pack313,"C47",hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));			/*退回申请号*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));
	iHVGETTAG(pack313,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*接受行行号*/
	iHVGETTAG(pack313,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*接受清算行号*/
	iHVGETTAG(pack313,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*收报中心代码*/
	iHVGETTAG(pack313,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*原委托日期*/
	iHVGETTAG(pack313,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*原交易种类*/
	iHVGETTAG(pack313,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*原支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_o_payin_orderno,hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));
	iHVGETTAG(pack313,"72A",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));		/*附言*/

	/***********没有附加域***************/	
	hv_notpay->T_hv_fzf_sts[0] = '9';
	vtcp_log("查询的初始状态为:[%c]\n",hv_notpay->T_hv_fzf_sts[0]);
	MEMCPY_DEBUG(hv_notpay->T_req_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_req_orderno));
	MEMCPY_DEBUG(hv_notpay->T_req_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_req_br_no));
	MEMCPY_DEBUG(hv_notpay->T_req_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_req_date));
	hv_notpay->T_otype[0]='1';/**大额**/
	/*****由于要根据原非支付交易找支付交易，这里需要对原支付交易的交易行赋值**/
	MEMCPY_DEBUG(hv_notpay->T_o_or_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));
	return 0; 
}
int pub_com_CMT314_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack314)
{    
	/****************退回应答报文*********************************/
	/***Modified by SSH,2006.9.7,有些字段赋值不对****/
	double  dTxamt     = 0.0;
	char cTmpBuf[19];
	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));

	MEMCPY_DEBUG(hv_notpay->T_cmtno,"314",3);						/*CMT编号*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack314->workDate,8);		/*交易日期*/
	iHVGETTAG(pack314,"CND",cTmpBuf,sizeof(cTmpBuf));						/*原货币符号,金额*/
	MEMCPY_DEBUG(hv_notpay->T_o_cur_no,cTmpBuf,3);					/*货币种类*/	
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_notpay->T_o_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
	iHVGETTAG( pack314,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack314,"CCE",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*退回行行号*/
	iHVGETTAG(pack314,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*发起清算行*/
	iHVGETTAG(pack314,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*发报中心代码*/
	iHVGETTAG(pack314,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*接受行行号*/
	iHVGETTAG(pack314,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*接受清算行号*/
	iHVGETTAG(pack314,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*收报中心代码*/
	iHVGETTAG(pack314,"CJB",hv_notpay->T_req_date,sizeof(hv_notpay->T_req_date));		/*原退回申请日期*/
	iHVGETTAG(pack314,"CP2",hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));		/*原退回申请序号*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_req_orderno,hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));
	iHVGETTAG(pack314,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*原委托日期*/
	iHVGETTAG(pack314,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*原交易种类*/
	iHVGETTAG(pack314,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*原支付交易序号*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_o_payin_orderno,hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));
	iHVGETTAG(pack314,"018",hv_notpay->T_respcode,sizeof(hv_notpay->T_respcode));		/*退回应答*/
	iHVGETTAG(pack314,"72A",hv_notpay->T_res_content,sizeof(hv_notpay->T_res_content));		/*附言*/

	/***********没有附加域***************/	
	if(iGetHvFzfOrderno(hv_notpay->T_orderno)) /***生成一个交易序号*****/
	{
   	    sprintf(acErrMsg,"[%s][%d]生成交易序号错误!\n",__FILE__,__LINE__);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"HV01");
	    return (-1);
		
	}
	hv_notpay->T_hv_fzf_sts[0] = '9';
	vtcp_log("查询的初始状态为:[%c]\n",hv_notpay->T_hv_fzf_sts[0]);
	MEMCPY_DEBUG(hv_notpay->T_res_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_res_orderno));
	MEMCPY_DEBUG(hv_notpay->T_res_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_res_br_no));
	MEMCPY_DEBUG(hv_notpay->T_res_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_res_date));
	return 0; 
}
int pub_com_CMT725_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack725)
{   
	/**********银行汇票全额兑付通知报文**********/
	double  dTxamt     = 0.0;
	HV_725_ADD hv_725;
	MEMSET_DEBUG(&hv_725,' ',sizeof(hv_725));
	char cTx_amt[17];

	MEMSET_DEBUG(cTx_amt, 0, sizeof(cTx_amt));
	MEMCPY_DEBUG(hv_notpay->T_cmtno,"725",3);						/*CMT编号*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack725->workDate,8);		/*交易日期*/
	iHVGETTAG( pack725,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*委托日期*/
	iHVGETTAG(pack725,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*特许参与者*/
	iHVGETTAG(pack725,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*收报中心代码*/
	iHVGETTAG(pack725,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*接受行号*/
	iHVGETTAG(pack725,"72A",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));					/*附言*/
  hv_notpay->T_hv_fzf_sts[0] = '9';

  iHVGETTAG(pack725,"21A",hv_notpay->T_hv_brf,sizeof(hv_notpay->T_hv_brf));	/*票据号码*/
  
	if(iGetHvFzfOrderno(hv_notpay->T_orderno)) /***生成一个交易序号*****/
	{
   	  sprintf(acErrMsg,"[%s][%d]生成交易序号错误!\n",__FILE__,__LINE__);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"HV01");
	    return (-1);
		
	}
  vtcp_log("[%s][%d] hv_notpay->T_orderno==[%.8s]  ",__FILE__,__LINE__,hv_notpay->T_orderno);

	/*****这里开始处理附加域****/	
	iHVGETTAG(pack725,"50C",hv_725.pay_ac_no,sizeof(hv_725.pay_ac_no));	/*付款人帐号,汇票申请人帐号*/
	iHVGETTAG(pack725,"50A",hv_725.pay_name,sizeof(hv_725.pay_name));	/*付款人姓名*/
	iHVGETTAG(pack725,"50B",hv_725.pay_addr,sizeof(hv_725.pay_addr));	/*付款人地址*/
	iHVGETTAG(pack725,"30B",hv_725.pj_date,sizeof(hv_725.pj_date));	/*票据日期*/
	iHVGETTAG(pack725,"21A",hv_725.pj_num,sizeof(hv_725.pj_num));	/*票据号码*/
	iHVGETTAG(pack725,"C10",hv_725.hp_miya,sizeof(hv_725.hp_miya));		/*汇票密押*/
	
	iHVGETTAG(pack725,"33C",hv_725.cp_amt,sizeof(hv_725.cp_amt));		/*出票金额*/
	MEMSET_DEBUG(cTx_amt, 0, sizeof(cTx_amt));
	MEMCPY_DEBUG(cTx_amt, hv_725.cp_amt, sizeof(hv_725.cp_amt));
	iHvStr2Dbl(cTx_amt, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_725.cp_amt,"%016.2f",dTxamt);	

	sprintf(hv_notpay->T_o_tx_amt,"%016.2f",dTxamt);	/* Added by Chenming  2006-11-6 23:15 */
		
	iHVGETTAG(pack725,"CNV",hv_725.real_js_amt,sizeof(hv_725.real_js_amt));	/*实际结算金额*/
	dTxamt     = 0.0;
	MEMSET_DEBUG(cTx_amt, 0, sizeof(cTx_amt));
	MEMCPY_DEBUG(cTx_amt, hv_725.real_js_amt, sizeof(hv_725.real_js_amt));
	iHvStr2Dbl(cTx_amt, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_725.real_js_amt,"%016.2f",dTxamt);	
	
	iHVGETTAG(pack725,"CC5",hv_725.cash_opn_br_no,sizeof(hv_725.cash_opn_br_no));/*收款人开户行号*/
	iHVGETTAG(pack725,"59E",hv_725.last_ac_no,sizeof(hv_725.last_ac_no));	/*最后持票人账号*/
	iHVGETTAG(pack725,"59D",hv_725.last_name,sizeof(hv_725.last_name));		/*最后持票人名称*/
	iHVGETTAG(pack725,"30C",hv_725.return_date,sizeof(hv_725.return_date));	/*提示付款日期*/

	iHvSetAddit(hv_notpay->T_tx_date,hv_notpay->T_wt_date,hv_notpay->T_addid,"000",(char*)&hv_725,sizeof(hv_725));

	return 0; 
}

/************************
*       PART 3       ***
*   附加域处理接口   ***
************************/

/***************************************************************************/
/* 函数名称: iHvSetAddit                                                   */
/* 功能描述: 增加附加数据域                                                */
/* 参数:                                                                   */
/*         !!!!   在调用时必须先进行参数的验证，addid不能为NULL    !!!!    */
/*            hv_payin   -支付交易通用输入结构体                           */
/*                hv_payin->T_inday   hv_payin->T_addid                    */
/*            name       -附加数据域的名称                                 */
/*            buf        -附加数据域的内容                                 */
/*            length     -附加数据域的长度           >= 0                  */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     石升辉                                                        */
/* 说明:     附加数据的名称为3个字节长的数字或者字母, 为000表示此附加域要  */
/*           直接拼到报文中做附加域,其他为程序内部处理                     */
/***************************************************************************/

int iHvSetAddit(char *inday,char *txday,char *addid,char *name,char *buf,int length)
{
	int iRc;
	int index = 0;
	struct hv_addit_c hv_addit;
	/*begin add by LiuHuafeng 20060323 */
	char cTxday[9];
	char cAddid[9];
	MEMSET_DEBUG(cTxday, '\0', sizeof(cTxday));
	MEMSET_DEBUG(cAddid, '\0', sizeof(cAddid));
	MEMSET_DEBUG(&hv_addit,'\0',sizeof(hv_addit));	
	MEMCPY_DEBUG(cTxday,txday,sizeof(cTxday)-1);
	zip_space(cTxday);	  	
	if ( strlen(cTxday) == 0 )
	{
		get_fd_data("0050",cTxday);
		/*MEMCPY_DEBUG(cTxday,pcHvGetDay(),sizeof(cTxday)-1);by lhf 20060325 */
	}
	sprintf(acErrMsg,"%s,%d, cTxday=[%s],addid=[%s]len_addid=[%d],len_txday=[%d]",
		__FILE__,__LINE__,cTxday,cAddid,strlen(cAddid),strlen(cTxday));
	WRITEMSG
	hv_addit.tx_date=apatoi(cTxday,8);
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));
	MEMCPY_DEBUG(cAddid,addid,sizeof(cAddid)-1);
	zip_space(cAddid);
	/****add liuyue 20061114 解决字符串中出现空字节的问题***/
	for( index=0;index<length;index++)
		if(buf[index]==0x00)
			buf[index]=' ';
	/**end***********/
	/* MEMCPY_DEBUG(hv_addit.tx_date,cTxday,sizeof(hv_addit.tx_date)-1); */
	
	if ( strlen(cAddid) ==0 )
	{
		iGetHvAddid(hv_addit.addid);
		MEMCPY_DEBUG(hv_addit.tagname,name, sizeof(hv_addit.tagname)-1);
		MEMCPY_DEBUG(hv_addit.tagdata,buf,length);

		iRc = Hv_addit_Ins(hv_addit, g_pub_tx.reply);
		if (iRc != 0)
		{
			sprintf( acErrMsg,"插入附加数据表失败![%s][%d][%d]",__FILE__,__LINE__,iRc);
			set_zd_data(DC_GET_MSG,"插入附加数据表失败");
			WRITEMSG
			return(1);
		}				
		MEMCPY_DEBUG(addid,hv_addit.addid,sizeof(hv_addit.addid)-1);
	}else
	{
	sprintf(acErrMsg,"[%s][%d] addid=[%s]name=[%s]",__FILE__,__LINE__,addid,name);
	WRITEMSG
	sprintf(acErrMsg,"[%s][%d] buf==[%s]",__FILE__,__LINE__,buf);
	WRITEMSG

		iRc = Hv_addit_Sel(g_pub_tx.reply,&hv_addit,"addid='%s' and tagname='%s' ",cAddid,name );
		if (iRc != 0)
		{  /* 不存在则插入 */
			iRc = Hv_addit_Ins(&hv_addit, g_pub_tx.reply);
			if (iRc != 0)
			{
				sprintf( acErrMsg,"插入附加数据表失败![%s][%d][%d]",__FILE__,__LINE__,iRc);
				set_zd_data(DC_GET_MSG,"插入附加数据表失败");
				WRITEMSG
				return(1);
			}		    	
		}
		else
		{    /* 存在则更新 */
			iRc = Hv_addit_Dec_Upd(g_pub_tx.reply,"addid='%s' and tagname='%s' ",cAddid,name);
			if (iRc != 0)
			{
				sprintf( acErrMsg,"DECLARE FOR ERROR![%s][%d][%d]",__FILE__,__LINE__,iRc);
				set_zd_data(DC_GET_MSG,"DECLARE FOR ERROR");
				WRITEMSG
				return(1);
			}        
			iRc = Hv_addit_Fet_Upd(&hv_addit, g_pub_tx.reply);
			if (iRc != 0)
			{
				sprintf( acErrMsg,"fetch FOR ERROR![%s][%d][%d]",__FILE__,__LINE__,iRc);
				set_zd_data(DC_GET_MSG,"fetch FOR ERROR");
				WRITEMSG
					return(1);
			}	
			MEMCPY_DEBUG(hv_addit.tagdata,buf,length);

			iRc = Hv_addit_Upd_Upd(hv_addit, g_pub_tx.reply);
			if (iRc != 0)
			{
				sprintf( acErrMsg,"update FOR ERROR![%s][%d][%d]",__FILE__,__LINE__,iRc);
				set_zd_data(DC_GET_MSG,"update FOR ERROR");
				WRITEMSG
					return(1);
			}
			Hv_addit_Clo_Upd();  
		}	  
		/* if(addid != 0 ) 调用时必须先验证参数，addid不能为NULL或只读内存区域 */
		MEMCPY_DEBUG(addid,hv_addit.addid,sizeof(hv_addit.addid)-1);
	}
  vtcp_log("iHvSetAddit() 附加域已设置 !  ");	
	return(0);
}

/*******1.3.7.	得到附加数据**************/
/***************************************************************************/
/* 函数名称: iHvGetAddit                                                   */
/* 功能描述: 得到附加数据域                                                */
/* 参数:                                                                   */
/*          !!!!   在调用时必须先进行参数的验证，          !!!!            */
/*            hv_payin   -支付交易通用输入结构体                           */
/*                hv_payin->T_inday  hv_payin->F_wt_date hv_payin->T_addid   */
/*            name       -附加数据域的名称                                 */
/*            buf        -附加数据域的内容                                 */
/*            length     -附加数据域的长度           >= 0                  */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     石升辉                                                        */
/* 说明:     附加数据的名称为3个字节长的数字或者字母                       */
/***************************************************************************/
int iHvGetAddit(char *inday,char *wt_date,char *addid,char *name,char *buf,int length)
{
	int iRc;
	struct hv_addit_c hv_addit;
	char cTxday[9];	
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	MEMSET_DEBUG(cTxday,0 , sizeof(cTxday));
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	MEMSET_DEBUG(&hv_addit,'\0',sizeof(hv_addit));
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	MEMCPY_DEBUG(cTxday,inday,sizeof(cTxday)-1);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	zip_space(cTxday);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if (strlen(cTxday)!=8)
		MEMCPY_DEBUG(cTxday,wt_date,sizeof(cTxday)-1);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);

	vtcp_log("[%s][%d]txday=[%.8s]",__FILE__,__LINE__,cTxday);
	vtcp_log("[%s][%d]addid=[%.8s]",__FILE__,__LINE__,addid);
	vtcp_log("[%s][%d]tagname=[%.3s]",__FILE__,__LINE__,name);

	iRc = Hv_addit_Sel(g_pub_tx.reply,&hv_addit,"addid='%.8s' and tagname='%.3s' and tx_date=%d ",addid,name,apatoi(cTxday,8));
	if ( iRc==100 )
	{
		sprintf( acErrMsg,"不存在该附加数据![%s][%d][%d]",__FILE__,__LINE__,iRc);
		set_zd_data(DC_GET_MSG,"不存在该附加数据");
		WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
		return(1);
	}
	else if ( iRc )
	{
		sprintf( acErrMsg,"查找附加数据表出错![%s][%d][%d]",__FILE__,__LINE__,iRc);
		set_zd_data(DC_GET_MSG,"查找附加数据表出错");
		WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
		return(1);
	}	
	if (length<=0)
	{
		strcpy(buf,hv_addit.tagdata);
	}else
	{
		MEMCPY_DEBUG(buf,hv_addit.tagdata,length);
	}
	vtcp_log("%s,%d buf==[%s]",__FILE__,__LINE__,buf);
	return(0);
}     

/****************************************************************************/
/*  OBJECT :  double truncation*/
/*  INPUT  :  ival->  123.4567*/
/*itnclen   ->  2*/
/*  OUTPUT :  ival->  123.45*/
/*  RETURN :  0->  sucessed*/
/*-1->  failed*/
/****************************************************************************/
int dbltrunc( ival, itnclen)
double  *ival;
int     itnclen;
{
	char lstrbuf[ MAX_BUF ];
	char lfmt[ MAX_BUF ];
	int     lofset,len;
	double  temp;
	sprintf(lfmt,"%%.%df",itnclen+1);
	sprintf(lstrbuf,lfmt,*ival);
	len=strlen(lstrbuf);
	for(lofset=0; ( lofset < len) && ( lstrbuf[lofset] != '.') ; lofset++);
	if ( itnclen >= 0 )
		MEMSET_DEBUG(&lstrbuf[lofset+itnclen+1],0x00,MAX_BUF-(lofset+itnclen+2 ));
	else
	{
		MEMSET_DEBUG(&lstrbuf[lofset+itnclen],'0', itnclen * -1 );
		MEMSET_DEBUG(&lstrbuf[lofset],0x00, MAX_BUF -lofset+1 );
	}
	temp=atof(lstrbuf);
	*ival=temp;
	return(0);
}

/****************************************************************************/
/*  OBJECT :  decimal to  string                                            */
/*  INPUT  :  idec      ->  456.123                                         */
/*            idot      ->  3                                               */
/*            ilen      ->  10           :output length                     */
/*            isign     ->  0 : none                                        */
/*                      ->  1 : add sign                                    */
/*  OUTPUT :  obuf      ->  +00123.456                                      */
/*  RETURN :  0         ->  sucessed                                        */
/*            -1        ->  failed                                          */
/****************************************************************************/
dbl2str( idbl,idot,ilen,isign,obuf )
	double	*idbl       ;
short	idot        ;
short	ilen        ;
short	isign       ;
char	*obuf       ;
{
	char	lstrbuf[ MAX_BUF ];
	char	lstrtmp[ MAX_BUF ];
	char	Lstr[ MAX_BUF ];
	char	fmt[10];
	char	*p1;
	double	ldbl;
	/*********** check input data ****************/
	if ( idot > ilen )
		return -1;
	/*********** truncate ************************/
	ldbl=*idbl;
	dbltrunc( &ldbl,idot );
	sprintf(fmt,"%%+#0%d.%df",ilen+2,idot);
	sprintf(lstrbuf,fmt,ldbl);
	strcpy(lstrtmp,&lstrbuf[1]);
	if ( isign != 0 )
	{
		strcpy(Lstr,lstrtmp);
		p1=strtok(Lstr,".");
		if ( strlen(p1) > ( ilen - idot - 1 ) )
			MEMCPY_DEBUG(&lstrbuf[1],&lstrtmp[strlen(p1)-ilen+idot+1],
			ilen-idot);
		else
			strcpy(&lstrbuf[1],p1);

		p1=strtok(NULL,".");
		MEMCPY_DEBUG(&lstrbuf[ilen-idot],p1,idot);
		if ( *idbl > 0 )
			lstrbuf[0]='+';
		else
			lstrbuf[0]='-';
	}
	else
	{
		strcpy(Lstr,lstrtmp);
		p1=strtok(Lstr,".");
		if ( strlen(p1) > ( ilen - idot ) )
			MEMCPY_DEBUG(lstrbuf,&lstrtmp[strlen(p1)-ilen+idot],
			ilen-idot);
		else
			strcpy(lstrbuf,p1);

		p1=strtok(NULL,".");
		MEMCPY_DEBUG(&lstrbuf[ilen-idot],p1,idot);
	}
	MEMCPY_DEBUG(obuf,lstrbuf,ilen);
	return(0);
}
/*************************************************
* 函 数 名:  iGetHvOrderno
* 功能描述： 得到大额支付系统的支付交易序号
*            按照支行排序
* 作    者:  LiuHuafeng
* 完成日期： 2006-8-9 14:57
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
int iGetHvOrderno(char * orderno)
{
	char cSeqno[9];
	char cKinbr[BRNO_LEN+1];
	int ret=0;
	struct seqctl_c hv_seqctl_c;
	MEMSET_DEBUG(cSeqno, 0 , sizeof(cSeqno));
	MEMSET_DEBUG(cKinbr, 0 , sizeof(cKinbr));
	MEMSET_DEBUG(&hv_seqctl_c, 0x00 , sizeof(hv_seqctl_c));
	get_fd_data("0030",cKinbr);
	vtcp_log("[%s][%d] cKinbr=[%s]",__FILE__,__LINE__,cKinbr);
	GetOr_br_noByBr_no(cKinbr,hv_seqctl_c.br_no);
	vtcp_log("[%s][%d] hv_seqctl_c.br_no=[%s]",__FILE__,__LINE__,hv_seqctl_c.br_no);
	vtcp_log("[%s][%d] hv_seqctl_c.ac_ind=[%s]",__FILE__,__LINE__,hv_seqctl_c.ac_ind);
	MEMCPY_DEBUG(hv_seqctl_c.ac_ind,HV_ORDERNO_IND,sizeof(hv_seqctl_c.ac_ind)-1);
	vtcp_log("[%s][%d] hv_seqctl_c.ac_ind=[%s]",__FILE__,__LINE__,hv_seqctl_c.ac_ind);
	vtcp_log("[%s][%d] hv_seqctl_c.br_no=[%s]",__FILE__,__LINE__,hv_seqctl_c.br_no);
	sprintf(acErrMsg,"%s,%d ac_ind=[%s],br_no=[%s]",
		__FILE__,__LINE__,hv_seqctl_c.ac_ind,hv_seqctl_c.br_no);
	WRITEMSG
	vtcp_log("[%s][%d] hv_seqctl_c.br_no=[%s]",__FILE__,__LINE__,hv_seqctl_c.br_no);
	ret=Seqctl_Dec_Upd(g_pub_tx.reply," br_no='%s' and ac_ind='%s'",
		hv_seqctl_c.br_no,hv_seqctl_c.ac_ind);
	if (ret)
	{
		sprintf( acErrMsg,"执行Seqctl_Dec_Upd错![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"执行Seqctl_Dec_Upd错");
		WRITEMSG
		goto ErrExit;
	}
	ret=Seqctl_Fet_Upd(&hv_seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf( acErrMsg,"执行Seqctl_Fet_Upd查询大额支付交易序号表错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"执行Seqctl_Fet_Upd查询大额支付交易序号表错误");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg,"执行seqctl_Fet_Upd错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"执行seqctl_Fet_Upd错误");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(cSeqno,"%08.0f",hv_seqctl_c.seqno);	
	if (hv_seqctl_c.seqno==99999999) hv_seqctl_c.seqno=0;
	vtcp_log("%s,%d 加值前seqno ++++++++ [%f]",__FILE__,__LINE__,hv_seqctl_c.seqno);
	hv_seqctl_c.seqno++;
	vtcp_log("%s,%d 加值后seqno ++++++++ [%f]",__FILE__,__LINE__,hv_seqctl_c.seqno);

	ret=Seqctl_Upd_Upd(hv_seqctl_c,g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf( acErrMsg,"修改积分主表错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"修改积分主表错误");
		WRITEMSG
		goto ErrExit;
	}
	ret=Seqctl_Clo_Upd();
	OkExit:
		MEMCPY_DEBUG(orderno,cSeqno,sizeof(cSeqno)-1);
	sprintf(acErrMsg,"取得支付交易序号=[%s]成功![%s]",cSeqno,g_pub_tx.reply);
	Seqctl_Sel(g_pub_tx.reply,&hv_seqctl_c," br_no='%s' and ac_ind='%s'",hv_seqctl_c.br_no,hv_seqctl_c.ac_ind);
	Seqctl_Debug(&hv_seqctl_c);
	WRITEMSG
		return 0;
	ErrExit:
		sprintf(acErrMsg,"取得支付交易序号失败![%s]",g_pub_tx.reply);
	WRITEMSG
		return 1;
}

/*************************************************
* 函 数 名:  iGetHvFzfOrderno
* 功能描述： 得到大额支付系统的非支付交易序号
*            按照全行排序
* 作    者:  LiuHuafeng
* 完成日期： 2006-8-9 14:57
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
int iGetHvFzfOrderno(char * orderno)
{
	char cSeqno[9];
	char cKinbr[BRNO_LEN+1];
	int ret=0;
	struct seqctl_c hv_seqctl_c;
	MEMSET_DEBUG(cSeqno, 0 , sizeof(cSeqno));
	MEMSET_DEBUG(cKinbr, 0 , sizeof(cKinbr));
	MEMSET_DEBUG(&hv_seqctl_c, 0x00 , sizeof(hv_seqctl_c));
	vtcp_log("[%s][%d] hv_seqctl_c.br_no=[%s]",__FILE__,__LINE__,hv_seqctl_c.br_no);
	ret=Seqctl_Dec_Upd(g_pub_tx.reply," br_no='%s' and ac_ind='%s'",
		MBFE_FZF_BR_NO,MBFE_FZF_IND);
	if (ret)
	{
		sprintf( acErrMsg,"执行Seqctl_Dec_Upd错![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"执行Seqctl_Dec_Upd错");
		WRITEMSG
		goto ErrExit;
	}
	ret=Seqctl_Fet_Upd(&hv_seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf( acErrMsg,"执行Seqctl_Fet_Upd查询大额支付交易序号表错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"执行Seqctl_Fet_Upd查询大额支付交易序号表错误");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg,"执行seqctl_Fet_Upd错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"执行seqctl_Fet_Upd错误");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(cSeqno,"%08.0f",hv_seqctl_c.seqno);	
	if (hv_seqctl_c.seqno==99999999) hv_seqctl_c.seqno=0;
	vtcp_log("%s,%d 加值前seqno ++++++++ [%f]",__FILE__,__LINE__,hv_seqctl_c.seqno);
	hv_seqctl_c.seqno++;
	vtcp_log("%s,%d 加值后seqno ++++++++ [%f]",__FILE__,__LINE__,hv_seqctl_c.seqno);

	ret=Seqctl_Upd_Upd(hv_seqctl_c,g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf( acErrMsg,"修改积分主表错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"修改积分主表错误");
		WRITEMSG
		goto ErrExit;
	}
	ret=Seqctl_Clo_Upd();
	OkExit:
		MEMCPY_DEBUG(orderno,cSeqno,sizeof(cSeqno)-1);
	sprintf(acErrMsg,"取得支付交易序号=[%s]成功![%s]",cSeqno,g_pub_tx.reply);
	Seqctl_Sel(g_pub_tx.reply,&hv_seqctl_c," br_no='%s' and ac_ind='%s'",hv_seqctl_c.br_no,hv_seqctl_c.ac_ind);
	Seqctl_Debug(&hv_seqctl_c);
	WRITEMSG
		return 0;
	ErrExit:
		sprintf(acErrMsg,"取得支付交易序号失败![%s]",g_pub_tx.reply);
	WRITEMSG
		return 1;
}

/*************************************************
* 函 数 名:  iGetHvRefno
* 功能描述： 得到大额支付系统的参考号，全行统一排序
*
* 作    者:  LiuHuafeng
* 完成日期： 2006-8-9 14:57
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
int iGetHvRefno(char * pay_or_br_no,char * refno)
{
	struct seqctl_c hv_seqctl_c;
	char cFullRefno[21];
	char cSeqno[9];
	int ret=0;
	MEMSET_DEBUG(cSeqno, 0 ,sizeof(cSeqno));
	MEMSET_DEBUG(cFullRefno, 0 , sizeof(cFullRefno));
	MEMSET_DEBUG(&hv_seqctl_c, 0 , sizeof(hv_seqctl_c));
	MEMCPY_DEBUG(cFullRefno,pay_or_br_no,12);
	vtcp_log("%s,%d Seqctl_Dec_Upd",__FILE__,__LINE__);
	ret=Seqctl_Dec_Upd(g_pub_tx.reply,"br_no='%s' and ac_ind='%s'",HV_BR_NO,HV_REFNO_IND);
	if (ret)
	{
		sprintf( acErrMsg,"执行Point_point_mst_Dec_Upd错![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"执行Point_point_mst_Dec_Upd错");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d Seqctl_Fet_Upd",__FILE__,__LINE__);
	ret=Seqctl_Fet_Upd(&hv_seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf( acErrMsg,"查询大额支付交易序号表错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"查询大额支付交易序号表错误");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg,"执行seqctl_Fet_Upd错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"执行seqctl_Fet_Upd错误");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(cSeqno,"%08.0f",hv_seqctl_c.seqno);
	strncat(cFullRefno,cSeqno,sizeof(cSeqno)-1);
	if (hv_seqctl_c.seqno==99999999) hv_seqctl_c.seqno=0;
	hv_seqctl_c.seqno++;
	vtcp_log("%s,%d Seqctl_Upd_Upd",__FILE__,__LINE__);
	ret=Seqctl_Upd_Upd(hv_seqctl_c,g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf( acErrMsg,"修改积分主表错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"修改积分主表错误");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d Seqctl_Clo_Upd",__FILE__,__LINE__);
	ret=Seqctl_Clo_Upd();
	OkExit:
		MEMCPY_DEBUG(refno,cFullRefno,sizeof(cFullRefno)-1);
	sprintf(acErrMsg,"取得支付交易序号成功![%s]",g_pub_tx.reply);
	WRITEMSG
		return 0;
	ErrExit:
		sprintf(acErrMsg,"取得支付交易序号失败![%s]",g_pub_tx.reply);
	WRITEMSG
		return 1;
}
/*************************************************
* 函 数 名:  iGetHvFlgno
* 功能描述： 得到大额支付系统的报文标识号，全行统一排序
*
* 作    者:  LiuHuafeng
* 完成日期： 2006-8-9 14:57
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
int iGetHvFlgno(char * refno,char * flgno)
{
	char cRefno[21];
	char cFlgno[9];
	MEMSET_DEBUG(cFlgno, 0 , sizeof(cFlgno));
	MEMSET_DEBUG(cRefno, 0 , sizeof(cRefno));
	MEMCPY_DEBUG(cRefno,refno,sizeof(cRefno)-1);
	vtcp_log("%s,%d refno==[%s]",__FILE__,__LINE__,cRefno);
	if (strlen(cRefno)!=20)
		return -1;
	MEMCPY_DEBUG(cFlgno,cRefno+13,7);
	cFlgno[7]=rand()%10+'0';
	vtcp_log("%s,%d refno==[%s]flag7=[%c]",__FILE__,__LINE__,cRefno,cFlgno[7]);
	MEMCPY_DEBUG(flgno,cFlgno,sizeof(cFlgno)-1);
	return 0;
}

/*************************************************
* 函 数 名:  iGetHvAddid
* 功能描述： 得到大额支付系统的附加域号，全行统一排序
*
* 作    者:  LiuHuafeng
* 完成日期： 2006-8-9 14:57
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
int iGetHvAddid(char * addid)
{
	struct seqctl_c hv_seqctl_c;
	char cFullRefno[21];
	char cSeqno[9];
	int ret=0;
	MEMSET_DEBUG(cSeqno, 0 ,sizeof(cSeqno));
	MEMSET_DEBUG(&hv_seqctl_c, 0 , sizeof(hv_seqctl_c));
	ret=Seqctl_Dec_Upd(g_pub_tx.reply,"br_no='%s' and ac_ind='%s'",
		HV_BR_NO,HV_ADDID_IND);
	if (ret)
	{
		sprintf( acErrMsg,"执行Point_point_mst_Dec_Upd错![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"执行Point_point_mst_Dec_Upd错");
		WRITEMSG
			goto ErrExit;
	}
	ret=Seqctl_Fet_Upd(&hv_seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf( acErrMsg,"查询大额支付附加域序号表错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"查询大额支付附加域序号表错误");
		WRITEMSG
			goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg,"执行seqctl_Fet_Upd错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"执行seqctl_Fet_Upd错误");
		WRITEMSG
			goto ErrExit;
	}
	sprintf(cSeqno,"%08.0f",hv_seqctl_c.seqno);
	if (hv_seqctl_c.seqno==99999999) hv_seqctl_c.seqno=0;
	hv_seqctl_c.seqno++;
	ret=Seqctl_Upd_Upd(hv_seqctl_c,g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf( acErrMsg,"修改积分主表错误![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"修改积分主表错误");
		WRITEMSG
			goto ErrExit;
	}
	ret=Seqctl_Clo_Upd();
	OkExit:
		MEMCPY_DEBUG(addid,cSeqno,sizeof(cSeqno)-1);
	sprintf(acErrMsg,"取得支付交易附加域序号成功![%s]",g_pub_tx.reply);
	WRITEMSG
		return 0;
	ErrExit:
		sprintf(acErrMsg,"取得支付交易附加域序号失败![%s]",g_pub_tx.reply);
	WRITEMSG
		return 1;
}


/*************************************************
* 函 数 名:  iHvTable2Notpayin
* 功能描述： 根据NOTPAY结构体中的索引字段检索到数据表,
*			 将返回结果到notpay的T字段中?
* 作    者:	 dadary 
* 完成日期： 2006-8-9 14:57
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

int iHvTable2Notpayin(HV_NOTPAY_AREA *phv_notpay)
{
	int ret = 0;
	struct hv_fzf_c hv_fzf;/**非支付结构体**/
	MEMSET_DEBUG(&hv_fzf,0x00,sizeof(hv_fzf));

	/*****利用F字段对hv_fzf中对应的字段赋值,最为主键****/
	MEMCPY_DEBUG(hv_fzf.or_br_no,phv_notpay->F_or_br_no,sizeof(phv_notpay->F_or_br_no));
	hv_fzf.wt_date = apatoi(phv_notpay->F_wt_date,8);
	MEMCPY_DEBUG(hv_fzf.orderno,phv_notpay->F_orderno,sizeof(phv_notpay->F_orderno));
	
	MEMCPY_DEBUG(hv_fzf.cmtno,phv_notpay->F_cmtno,sizeof(phv_notpay->F_cmtno));

	sprintf(acErrMsg,"orbrno=[%.12s],wt_date=[%d],orderno=[%s],cmtno=[%s]\n",\
		hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno,
		hv_fzf.cmtno);WRITEMSG
	/****SELECT数据表********/
	ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"or_br_no='%s' and wt_date='%d' and orderno='%s' and cmtno='%s'", \
		 hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno,hv_fzf.cmtno);
	if (ret!=0)
	{
		sprintf( acErrMsg,"Select hv_zf table error!![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"Select hv_zf table error!");
		WRITEMSG
		return 1;
	}
	phv_notpay->T_otype[0]=	hv_fzf.otype[0];
	MEMCPY_DEBUG(phv_notpay->T_or_br_no,	hv_fzf.or_br_no,	sizeof(hv_fzf.or_br_no));
	MEMCPY_DEBUG(phv_notpay->T_orderno,	hv_fzf.orderno,		sizeof(hv_fzf.orderno));
	apitoa(hv_fzf.tx_date,	sizeof(phv_notpay->T_tx_date),phv_notpay->T_tx_date);
	apitoa(hv_fzf.wt_date,	sizeof(phv_notpay->T_wt_date),phv_notpay->T_wt_date);
	vtcp_log("[%s][%d]hv_notpay.T_wt_date=[%.8s]",__FILE__,__LINE__,phv_notpay->T_wt_date);
	MEMCPY_DEBUG(phv_notpay->T_br_no,	hv_fzf.br_no,		sizeof(hv_fzf.br_no));
	phv_notpay->T_hv_fzf_sts[0]  =  hv_fzf.hv_fzf_sts[0];
	MEMCPY_DEBUG(phv_notpay->T_cmtno,	hv_fzf.cmtno,		sizeof(hv_fzf.cmtno));
	/**
	sprintf(phv_notpay->T_tx_time,	"%08s",			hv_fzf.tx_time);
	apitoa(hv_fzf.o_trace_no,sizeof(phv_notpay->T_o_trace_no),phv_notpay->T_o_trace_no);
	**/
	sprintf(phv_notpay->T_tx_time,	"%6d",			hv_fzf.tx_time);
	sprintf(phv_notpay->T_o_trace_no,"%6d",			hv_fzf.trace_no);
	MEMCPY_DEBUG(phv_notpay->T_pay_qs_no,	hv_fzf.pay_qs_no,	sizeof(hv_fzf.pay_qs_no));
	MEMCPY_DEBUG(phv_notpay->T_ac_br_no,	hv_fzf.ac_br_no,	sizeof(hv_fzf.ac_br_no));
	MEMCPY_DEBUG(phv_notpay->T_cash_qs_no,hv_fzf.cash_qs_no,	sizeof(hv_fzf.cash_qs_no));
	/**
	apitoa(hv_fzf.o_tx_date,sizeof(phv_notpay->T_o_wt_date),phv_notpay->T_o_wt_date);
	**/
	sprintf(phv_notpay->T_o_wt_date,"%8d",hv_fzf.o_wt_date);
	MEMCPY_DEBUG(phv_notpay->T_o_or_br_no,hv_fzf.o_or_br_no,	sizeof(hv_fzf.o_or_br_no));
	MEMCPY_DEBUG(phv_notpay->T_o_ac_br_no,hv_fzf.o_ac_br_no,	sizeof(hv_fzf.o_ac_br_no));
	MEMCPY_DEBUG(phv_notpay->T_o_payin_orderno,hv_fzf.o_orderno,	sizeof(hv_fzf.o_orderno));
	MEMCPY_DEBUG(phv_notpay->T_o_cur_no,	hv_fzf.o_cur_ind,	sizeof(hv_fzf.o_cur_ind));
	/**
	dbl2str(hv_fzf.o_tx_amt,2,sizeof(phv_notpay->T_o_tx_amt),0,phv_notpay->T_o_tx_amt);
	dot_dbl2str(&hv_fzf.o_tx_amt,0,sizeof(phv_notpay->T_o_tx_amt),0,phv_notpay->T_o_tx_amt);
	**/
	/****Modified by SSH,2006.9.7,取出带小数点的金额****/
	dot_dbl2str(hv_fzf.o_tx_amt,sizeof(phv_notpay->T_o_tx_amt),2,phv_notpay->T_o_tx_amt);
	
	vtcp_log("[%s][%d]o_tx_amt=[%f]T_o_tx_amt=[%s]\n",__FILE__,__LINE__,hv_fzf.o_tx_amt,phv_notpay->T_o_tx_amt);
	MEMCPY_DEBUG(phv_notpay->T_o_cmtno,	hv_fzf.o_cmtno,		sizeof(hv_fzf.o_cmtno));
	MEMCPY_DEBUG(phv_notpay->T_o_tx_type,	hv_fzf.o_tx_type,	sizeof(hv_fzf.o_tx_type));
	MEMCPY_DEBUG(phv_notpay->T_sendco,	hv_fzf.sendco,		sizeof(hv_fzf.sendco));
	MEMCPY_DEBUG(phv_notpay->T_receco,	hv_fzf.receco,		sizeof(hv_fzf.receco));
	apitoa(hv_fzf.req_date,	sizeof(phv_notpay->T_req_date),phv_notpay->T_req_date);
	MEMCPY_DEBUG(phv_notpay->T_req_br_no,	hv_fzf.req_br_no,	sizeof(hv_fzf.req_br_no));
	MEMCPY_DEBUG(phv_notpay->T_req_content,hv_fzf.req_content,	sizeof(hv_fzf.req_content));
	MEMCPY_DEBUG(phv_notpay->T_req_orderno,hv_fzf.req_orderno,	sizeof(hv_fzf.req_orderno));
	apitoa(hv_fzf.res_date,	sizeof(phv_notpay->T_res_date),phv_notpay->T_res_date);
	MEMCPY_DEBUG(phv_notpay->T_res_br_no,	hv_fzf.res_br_no,	sizeof(hv_fzf.res_br_no));
	MEMCPY_DEBUG(phv_notpay->T_res_content,hv_fzf.res_content,	sizeof(hv_fzf.res_content));
	MEMCPY_DEBUG(phv_notpay->T_res_orderno,hv_fzf.res_orderno,	sizeof(hv_fzf.res_orderno));
	phv_notpay->T_respcode[0] =  hv_fzf.res_sts[0];
	MEMCPY_DEBUG(phv_notpay->T_addid,	hv_fzf.addid,		sizeof(hv_fzf.addid));
	MEMCPY_DEBUG(phv_notpay->T_refno,	hv_fzf.refno,		sizeof(hv_fzf.refno));
	MEMCPY_DEBUG(phv_notpay->T_hv_prt_ind,hv_fzf.hv_prt_ind,sizeof(hv_fzf.hv_prt_ind));
	MEMCPY_DEBUG(phv_notpay->T_hv_brf,	hv_fzf.hv_brf,sizeof(hv_fzf.hv_brf));
	vtcp_log("[%s,%d]调用iHvTable2Notpayin已经结束",__FILE__,__LINE__);
	return 0;
}
/*************************************************
* 函 数 名:  iResetNotpay
* 功能描述： 将NOTPAY结构体中的T字段赋值到F字段中 
*
* 作    者:  dadary 
* 完成日期： 2006-8-9 14:57
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
int iResetNotpay(HV_NOTPAY_AREA *phv_notpay)
{
	sprintf(acErrMsg,"Begin to reset struct Notpay,T_*->F_*!");
	WRITEMSG
		MEMCPY_DEBUG(phv_notpay->F_or_br_no,	phv_notpay->T_or_br_no,	sizeof(phv_notpay->F_or_br_no));	
	MEMCPY_DEBUG(phv_notpay->F_orderno,	phv_notpay->T_orderno,	sizeof(phv_notpay->F_orderno));	
	MEMCPY_DEBUG(phv_notpay->F_tx_date,	phv_notpay->T_tx_date,	sizeof(phv_notpay->F_tx_date));	
	MEMCPY_DEBUG(phv_notpay->F_wt_date,	phv_notpay->T_wt_date,	sizeof(phv_notpay->F_wt_date));	
	MEMCPY_DEBUG(phv_notpay->F_tx_br_no,	phv_notpay->T_br_no,	sizeof(phv_notpay->F_tx_br_no));	
	MEMCPY_DEBUG(phv_notpay->F_cmtno,		phv_notpay->T_cmtno,	sizeof(phv_notpay->F_cmtno));	
	MEMCPY_DEBUG(phv_notpay->F_ac_br_no,	phv_notpay->T_ac_br_no,	sizeof(phv_notpay->F_ac_br_no));	
	MEMCPY_DEBUG(phv_notpay->F_o_wt_date,	phv_notpay->T_o_wt_date,sizeof(phv_notpay->F_o_wt_date));	
	MEMCPY_DEBUG(phv_notpay->F_o_or_br_no,phv_notpay->T_o_or_br_no,sizeof(phv_notpay->F_o_or_br_no));	
	MEMCPY_DEBUG(phv_notpay->F_o_ac_br_no,phv_notpay->T_o_ac_br_no,sizeof(phv_notpay->F_o_ac_br_no));	
	MEMCPY_DEBUG(phv_notpay->F_o_payin_orderno,	phv_notpay->T_o_payin_orderno,	sizeof(phv_notpay->F_o_payin_orderno));	
	MEMCPY_DEBUG(phv_notpay->F_o_cur_no,	phv_notpay->T_o_cur_no,sizeof(phv_notpay->F_o_cur_no));	
	MEMCPY_DEBUG(phv_notpay->F_o_txamt,	phv_notpay->T_o_tx_amt,sizeof(phv_notpay->F_o_txamt));	
	MEMCPY_DEBUG(phv_notpay->F_o_cmtno,	phv_notpay->T_o_cmtno,	sizeof(phv_notpay->F_o_cmtno));	
	MEMCPY_DEBUG(phv_notpay->F_o_tx_type,	phv_notpay->T_o_tx_type,sizeof(phv_notpay->F_o_tx_type));	

	if (!memcmp(phv_notpay->T_cmtno,"302",3)||!memcmp(phv_notpay->T_cmtno,"312",3)||!memcmp(phv_notpay->T_cmtno,"314",3) )
	{
		/******NOTE:对于应答类的交易,其附言部分都写入了res_content中，而对于申请类交易附言部分写入了req_content中*******/
		/******如果是申请类交易的话，T_req_orderno应该是空的，只是当位应答类的时候req_orderno中才有数据,而且对于字段T_res_orderno一直没有用到*****/
		/***只有交易类型为302 312 314的时候 req字段才有数据*********/

		MEMCPY_DEBUG(phv_notpay->F_content,	phv_notpay->T_res_content,sizeof(phv_notpay->F_content));	
		MEMCPY_DEBUG(phv_notpay->F_o_fzf_date,phv_notpay->T_req_date,sizeof(phv_notpay->F_o_fzf_date));
			MEMCPY_DEBUG(phv_notpay->F_o_notpay_orderno,	phv_notpay->T_req_orderno,sizeof(phv_notpay->F_o_notpay_orderno));
	}
	else
	{
		MEMCPY_DEBUG(phv_notpay->F_content,	phv_notpay->T_req_content,sizeof(phv_notpay->F_content));	
	}
	MEMCPY_DEBUG(phv_notpay->F_respcode,	phv_notpay->T_respcode,	sizeof(phv_notpay->F_respcode));	
	MEMCPY_DEBUG(phv_notpay->F_refno,		phv_notpay->T_refno,	sizeof(phv_notpay->F_refno));	
	MEMCPY_DEBUG(phv_notpay->F_hv_brf,		phv_notpay->T_hv_brf,	sizeof(phv_notpay->F_hv_brf));	
}

/****************************************************************************
 * 函数名称: dot_dbl2str(double, int size,int dot,char * str)               *
 * 功能描述: 将dbl转换为字符串类型函小数点                                  *
 * 参数描述: 要转换的double变量，总长度，小数点位置，转换数出的字符窜       *
 * 创建日期: 2006-8-19 15:58                                                *
 * 作者:     LiuHuafeng                                                     *
*****************************************************************************/
int dot_dbl2str(double dvalue, int size,int dot,char * str)
{
	char cTmpbuf[100];
	char cFormat[30];
	MEMSET_DEBUG(cTmpbuf, 0 , sizeof(cTmpbuf));
	MEMSET_DEBUG(cFormat, 0 , sizeof(cFormat));
	sprintf(cFormat,"%%%d.%df",size,dot);
	sprintf(cTmpbuf,cFormat,dvalue);
	zip_space(cTmpbuf);
	MEMCPY_DEBUG(str,cTmpbuf,size);
	if(size<dot) 
		return -1;
	else 
		return 0;
}
int dot_str2dbl(char * str,int size,double *d_value)
{
	char * endstr;
	char cStr[101];
	MEMSET_DEBUG(cStr, 0 , sizeof(cStr));
	if(size>sizeof(cStr)-1)
		return -1;
	else
		MEMCPY_DEBUG(cStr, str,size);
	vtcp_log("%s,%d cStr=%s",__FILE__,__LINE__,cStr);			
	*d_value=strtod(cStr,&endstr);
	vtcp_log("%s,%d txamt=%f",__FILE__,__LINE__,*d_value);	
	return 0;
}
int iHVSETTAG(THVPACKAGE * pack,char * tag,char * content,int size)
{
	vtcp_log("[%s][%d]content==[%s]\n",__FILE__,__LINE__,content);
	char *cContent = (char *)malloc(size+1);
	vtcp_log("[%s][%d]content==[%s]\n",__FILE__,__LINE__,content);
	MEMSET_DEBUG(cContent, 0 , size+1);
	MEMCPY_DEBUG(cContent,content,size);
	cContent[size]='\0';
	vtcp_log("[%s][%d]content==[%s],tag=[%.3s],pack=[%ld],size=[%d,%d]\n",__FILE__,__LINE__,cContent,tag,pack,size,sizeof(cContent));
	hv_set_tag( pack,tag,cContent);
	vtcp_log("[%s][%d]content==[%s]\n",__FILE__,__LINE__,content);
	free(cContent);
	/*	
	vtcp_log("[%s][%d]content==[%s]\n",__FILE__,__LINE__,content);
	*20061218**/
	return 0;
}
int iHVGETTAG(THVPACKAGE * pack,char * tag,char * content,int size)
{
	hv_get_tag( pack,tag,content);
	vtcp_log("%s,%d size='%d'",__FILE__,__LINE__,size);
	vtcp_log("%s,%d content='%s'",__FILE__,__LINE__,content);
	return 0;
}
/****************************************************
 *去掉金额(字符串形式)中的空格,并在前面补0到15位    *
 *主要用于处理拼报文时候的32A字段,用法:             *
 *del_dot(hv_notpayin->tx_amt,cTmp_buf+3)           *
 *返回结果直接赋到了target字段中*
 ****************************************************/
int  del_dot( char *source,char *target,int size)
{
	int index = 0;
	int iLen=0;
	int j=0;
	char cTmpbuf[40];
	char * endprt;
	double dValue=0.0;
	MEMSET_DEBUG(cTmpbuf, 0 , sizeof(cTmpbuf));
	MEMCPY_DEBUG(cTmpbuf,source,size);
	zip_space(cTmpbuf);
	iLen=strlen(cTmpbuf);
	for(index=0;index<iLen;index++) 
	{
		if(cTmpbuf[index]=='.')
		{
			for(j=index;j<iLen-1;j++)
				cTmpbuf[j]=cTmpbuf[j+1];
			iLen--;
		}
	}
	cTmpbuf[index]='\0';
	vtcp_log("%s,%d amt=[%s]",__FILE__,__LINE__,cTmpbuf);
	dValue=strtod(cTmpbuf,&endprt);
	dbl2str(&dValue,0,size,0,cTmpbuf);
	MEMCPY_DEBUG(target,cTmpbuf,size);
	vtcp_log("%s,%d target=[%s]",__FILE__,__LINE__,target);
	return 0;
}


/************************************************
* 由于人行报文中的金额不完全按照RMB+左补0的金额 *
* 所以要将没有左补零的金额自动找出来并返回回去  *
* 注意一定是原始的报文tag内容，就是前边有RMB的  *
************************************************/
void vGetRecvTagTxamt(char *tagamt,char *newtxamt)
{
	char cTagamt[16];
	int i=0;
	MEMSET_DEBUG(cTagamt, 0 , sizeof(cTagamt));
	MEMCPY_DEBUG(cTagamt,tagamt+3,sizeof(cTagamt)-1);
	for(i=0;i<19;i++)
	{
		if(cTagamt[i]>='0' && cTagamt[i]<='9')
		{
		}
		else
		{
			cTagamt[i]='\0';
		}
	}
	MEMCPY_DEBUG(newtxamt,cTagamt,sizeof(cTagamt)-1);
	return;
}

/* 16位字符串转换成 Double类型 */
int iHvStr2Dbl(char* cAmt, double* dAmt)
{
	char cTmp_amt[17];
	int i;
	MEMSET_DEBUG(cTmp_amt, 0, sizeof(cTmp_amt));
	MEMCPY_DEBUG(cTmp_amt, cAmt, sizeof(cTmp_amt)-1);
	for(i=0;i<16;i++)
	{
		if(cTmp_amt[i]>='0' && cTmp_amt[i]<='9')
		{
		}
		else
		{
			cTmp_amt[i]='\0';
		}
	}
	*dAmt = atof(cTmp_amt);
	return 0;
}

/******************************************
* 把支付交易往帐信息写入到文件中，
* 防止发送后数据库rollback再次发送或取消
*
*
*
******************************************/
int iInsertSendRecord(char* cmtno,char * txday,char *orderno,char *brno,char *wssrno,char *resp1)
{
	int  iFd=0,iSize=0;
	extern int errno;
	char cFilename[513];
	char cBasename[15];
	char cTxday[9];
	struct STR_BaseSend str_writeRecord;
	MEMCPY_DEBUG(cTxday, 0 , sizeof(cTxday));
	MEMCPY_DEBUG(cTxday,txday,sizeof(cTxday)-1);
	MEMSET_DEBUG(&str_writeRecord, 0 , sizeof(str_writeRecord));
	MEMSET_DEBUG(cFilename, 0 ,sizeof(cFilename));
	MEMSET_DEBUG(cBasename, 0 ,sizeof(cBasename));
	MEMCPY_DEBUG(cBasename,"sd",2);
	MEMCPY_DEBUG(cBasename+2,cTxday,8);
	MEMCPY_DEBUG(cBasename+10,".txt",4);
	sprintf(cFilename,"%s/%s",(char*)getenv("LOGDIR"),cBasename);
	vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
	iFd=open(cFilename,O_RDWR|O_CREAT|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
	if(iFd==-1)
	{
		vtcp_log("[%s][%d]文件描述符号==[%d] \n",__FILE__,__LINE__,iFd);
		vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
		vtcp_log("[%s][%d] 打开准备写入的文件[%s]错误 \n",__FILE__,__LINE__,cFilename);
		return -1;
	}
	MEMCPY_DEBUG(str_writeRecord.cmtno  ,cmtno  ,sizeof(str_writeRecord.cmtno  ));
	str_writeRecord.separator1='|';
	MEMCPY_DEBUG(str_writeRecord.txday  ,txday  ,sizeof(str_writeRecord.txday  ));
	str_writeRecord.separator2='|';
	MEMCPY_DEBUG(str_writeRecord.brno   ,brno  ,sizeof(str_writeRecord.brno   ));
	str_writeRecord.separator3='|';
	MEMCPY_DEBUG(str_writeRecord.wssrno,wssrno,sizeof(str_writeRecord.wssrno));
	str_writeRecord.separator4='|';
	MEMCPY_DEBUG(str_writeRecord.orderno,orderno,sizeof(str_writeRecord.orderno));
	str_writeRecord.separator5='|';/* add by lhf 050803 */
	MEMCPY_DEBUG(str_writeRecord.resp1,resp1,sizeof(str_writeRecord.resp1));
	str_writeRecord.separator6='|';/* add by ayy 2006-11-20 22:00 */
	str_writeRecord.nextline='\n';

	iSize=write(iFd,(char *)&str_writeRecord,sizeof(str_writeRecord));
	if(iSize<=0)
	{
		vtcp_log("没有数据被写入 errno=[%d] ,size=[%d] \n",errno,iSize);
		return -1;
	}
	if(close(iFd))
	{
		vtcp_log("[%s][%d] close id error \n",__FILE__,__LINE__);
		return -1;
	}
	return 0;
}
/*压缩全角空格*/
char *DBC2SBC(char *DBCstr,int cplen)
{
	int Length,i;
	char *dp;
	char *sp;
	static   char SBCstr[1024];
	dp = DBCstr;
	sp = SBCstr;
	MEMSET_DEBUG(SBCstr,'\0',sizeof(SBCstr));
	Length = 0;
	while(*dp!=0)
	{
		if(Length>=cplen)
		{
			break;
		}
		if((unsigned char)(*dp)>128)
		{
			*sp = *dp;
			*(sp+1) = *(dp+1);
			sp +=2;
			dp +=2;
			Length+=2;
		}else{
			if(*dp == ' ')
			{
				*sp = (char)(161);
				*(sp+1) = (char)(161); 
			}else{
				*sp = (char)(163);
				*(sp+1) = (char)( (unsigned char)(*dp)+128 );
			}
			sp +=2;
			dp ++;
			Length+=2;
		}
	}
	/*****20061110
	while(cplen>Length){
		*sp=(char)(161);
		*(sp+1)=(char)(161);
		sp+=2;
		Length+=2;
	}**************/	
	sp[cplen]='\0';	
	vtcp_log("[%s][%d]SBCstr===[%s]\n",__FILE__,__LINE__,SBCstr);
	return(SBCstr);
}
char *SBC2DBC(char *SBCstr)
{
	char *dp;
	char *sp;
	static  char DBCstr[1024];
	sp = SBCstr;
	dp = DBCstr;
	MEMSET_DEBUG(DBCstr,'\0',sizeof(DBCstr));
	if(strlen(sp) % 2!=0){
		MEMSET_DEBUG(dp,' ',strlen(sp));
		return(DBCstr);
	}
	while(*sp!=0)
	{
		if( *sp == (char)(161) ){
			*dp = ' ';
			sp +=2;
			dp +=1;
		}else if( *sp == (char)(163)){
			*dp = (unsigned char)(*(sp+1)) - 128;
			sp +=2;
			dp +=1;
		}else{
			*dp = *sp;
			*(dp+1) = *(sp+1);
			dp +=2;
			sp +=2;
		}
	}
	return(DBCstr);
}
char * dbc_zipspace(char *str)
{
	char str1[512];
	char str2[1024];
	char str3[512];
	char str4[1024];
	MEMSET_DEBUG(str1, 0, sizeof(str1));
	MEMSET_DEBUG(str2, 0, sizeof(str2));
	MEMSET_DEBUG(str3, 0, sizeof(str3));
	MEMSET_DEBUG(str4, 0, sizeof(str4));
	MEMCPY_DEBUG(str1, str, sizeof(str1));
	vtcp_log("[%s][%d]dbc_zipspace.str1==[%s]\n",__FILE__,__LINE__,str1);
	strcpy(str2,DBC2SBC(str1,strlen(str1)*2));
	vtcp_log("[%s][%d]dbc_zipspace.str2==[%s]\n",__FILE__,__LINE__,str2);
	strcpy(str3,SBC2DBC(str2));
	zip_space(str3);
	strcpy((char *)str,str3);
	vtcp_log("[%s][%d]dbc_zipspace.str==[%s]\n",__FILE__,__LINE__,str);
	return(str);
	/****************************/	
	vtcp_log("[%s][%d]dbc_zipspace.str3==[%s]\n",__FILE__,__LINE__,str3);
	strcpy(str4,DBC2SBC(str3,strlen(str3)*2));
	zip_space(str4);
	vtcp_log("[%s][%d]dbc_zipspace.str4==[%s]\n",__FILE__,__LINE__,str4);
	strcpy((char *)str,str4);
	vtcp_log("[%s][%d]dbc_zipspace.str==[%s]\n",__FILE__,__LINE__,str);
	return(str);
}
int iMbfeGetAcid(char * ac_no)
{
	struct mdm_ac_rel_c sMdm_ac_rel;
	MEMSET_DEBUG(&sMdm_ac_rel , 0 , sizeof(sMdm_ac_rel));
	MEMCPY_DEBUG(sMdm_ac_rel.ac_no,ac_no,sizeof(sMdm_ac_rel.ac_no)-1);
	pub_base_strpack(sMdm_ac_rel.ac_no);
	g_reply_int=Mdm_ac_rel_Sel(	g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",sMdm_ac_rel.ac_no);
	if(g_reply_int)
	{
		return g_reply_int;
	}
	else
		g_pub_tx.ac_id=sMdm_ac_rel.ac_id;
	return 0;
}

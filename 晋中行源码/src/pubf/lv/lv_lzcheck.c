/******************************************************************
模块名称: 来账交易处理的公共函数
函数功能: 来账交易处理的公共函数
输入参数:
输出参数:
使用说明:
编 写 者: dxy 
*******************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lvpack.h"
#include "exitlvpack.h"
#include "lv_pub.h"



static	char	cAcbrno[13];	/** 本行行号 **/
static	char	cQsactno[13];	/** 本行清算行号 **/
static	char	cSendco[5];		/** 本行发报中心代码 **/
static	char	cCmtno[4];		/** CMT号码 **/
static	char	cTxday[9];		/** 委托日期 **/
static	char	cCashno[13];	/** 接收行号 **/
static	char	cCashqsactno[13];	/** 接收清算行号 **/


lv_lzcheck()
{
	int ret=0,cmtno=0;

    TLVPACKAGE lv;
    TLVPACKAGE lv2;
	lv_create(&lv);		/** 初始化结构体 **/

	memset(cAcbrno,  '\0', sizeof(cAcbrno));
	memset(cQsactno, '\0', sizeof(cQsactno));
	memset(cSendco,  '\0', sizeof(cSendco));
	memset(cCmtno,   '\0', sizeof(cCmtno));
	memset(cTxday,   '\0', sizeof(cTxday));
	memset(cCashno,  '\0', sizeof(cCashno));
	memset(cCashqsactno,  '\0', sizeof(cCashqsactno));

	/** 先达到本机构的信息 **/
    if(find_netbrno(lvca.brno, cAcbrno)==NULL)
    {
        vtcp_log("[%s][%d]得到参与者号错误!",__FILE__,__LINE__);
        _lv_errmsg("得到参与者号错误!");
        return -1;
    }
	vtcp_log("[%s][%d]==========本行12位行号cAcbrno=[%s]\n",__FILE__,__LINE__,cAcbrno);

    if(find_qsactno(cAcbrno,cQsactno,0)==NULL)
    {
        vtcp_log("[%s][%d]得到发起清算行错误!",__FILE__,__LINE__);
        _lv_errmsg("得到发起清算行错误!");
        return -1;
    }
	vtcp_log("[%s][%d]==========本行清算行号cQsactno=[%s]\n",__FILE__,__LINE__,cQsactno);

    if(find_sendco(cAcbrno,cSendco,0)==NULL)
    {
        vtcp_log("[%s][%d]得到发报中心错误!",__FILE__,__LINE__);
        _lv_errmsg("得到发报中心错误!");
        return -1;
    }
	vtcp_log("[%s][%d]==========本行发报中心代码cSendco=[%s]\n",__FILE__,__LINE__,cSendco);


	/** 接收报文 **/

	/** 检查报文的合法性 **/
	ret = lv_analyze_pack(&lv);	
    if(ret<0){
        
        _lv_errmsg("接收小额报文非法!");
        vtcp_log("%s,%d,接收小额报文非法!\n",__FILE__,__LINE__);
        
		lv_destroy(&lv);
        return(-1);
    }

	if(lv.opeType != '1')
	{
        
        _lv_errmsg("来帐不是小额报文!");
        vtcp_log("%s,%d,来帐不是小额报文!\n",__FILE__,__LINE__);
        
		lv_destroy(&lv);
        return(-1);
    }

	memcpy(cCmtno, lv.CMTCode, sizeof(cCmtno) -1);
	vtcp_log("[%s][%d]=============来帐报文是=[%s]\n",__FILE__,__LINE__,cCmtno);
	
	/** 报文是批量 **/
	if(lv.isMulti == '1')
	{
		lv_create(&lv2);		/** 初始化结构体 **/

		ret = lv_fetchpkg(&lv,&lv2);		/** 解析批量报文 **/
    	if(ret<0){
        	
        	_lv_errmsg("解析报文错误!");
        	vtcp_log("%s,%d,解析报文错误!\n",__FILE__,__LINE__);
        	
			lv_destroy(&lv);
			lv_destroy(&lv2);
        	return(-1);
    	}
    	if(ret==1){
        	
        	_lv_errmsg("报文无记录!");
        	vtcp_log("%s,%d,报文无记录!\n",__FILE__,__LINE__);
        	
			lv_destroy(&lv);
			lv_destroy(&lv2);
        	return(-1);
		}

		cmtno = apatoi(lv.CMTCode,3);
		
		/** 得到接收行号 **/
		if(memcmp(lv.CMTNo,"006", 3) == 0 )
		{
			lv_get_tag(&lv2,"CC1",cCashno);	/** 从{3:}中得到包接收行号 **/
		}
		else if(memcmp(lv.CMTNo,"003", 3) == 0 )
		{
			lv_get_tag(&lv2,"CCM",cCashno);	/** 从{3:}中得到包接收行号 **/
			
		}
		else if(memcmp(lv.CMTNo,"004", 3) == 0 )
		{
			lv_get_tag(&lv2,"CCN",cCashno);	/** 从{3:}中得到包接收行号 **/
		}
		else
		{
			lv_get_tag(&lv2,"58A",cCashno);	/** 从{3:}中得到包接收行号 **/
		}	
		
		/** 得到接收清算行号 **/	
		if(memcmp(lv.CMTCode,"013", 3) == 0 || memcmp(lv.CMTCode,"012", 3) == 0)
		{
			lv_pkg_getHeaderTag(&lv,"30E",cCashqsactno);	/** 从{P:}中得到包接收清算行行号 **/
		}
		else
		{
			lv_pkg_getHeaderTag(&lv,"012",cCashqsactno);	/** 从{P:}中得到包接收清算行行号 **/
		}

		/** 得到委托日期 **/
		lv_get_tag(&lv2,"30A",cTxday);			/** 从{3:}中得到委托日期 **/
	}
	else	/** 得到非批量包的接收行号及日期 **/
	{
		cmtno = apatoi(lv.CMTNo,3);

		/** 委托日期 **/
		if(memcmp(lv.CMTNo,"301", 3) == 0 )
		{
			lv_get_tag(&lv2,"CJ9",cTxday);	
		}
		else if(memcmp(lv.CMTNo,"302", 3) == 0 )
		{
			lv_get_tag(&lv2,"CJ1",cTxday);	
		}
		else
		{
			lv_get_tag(&lv2,"30A",cTxday);	
		}

		/** 接收行号 **/ 
		if(memcmp(lv.CMTNo,"301", 3) == 0 )
		{
			lv_get_tag(&lv2,"01D",cCashno);	
		}
		else if(memcmp(lv.CMTNo,"302", 3) == 0 )
		{
			lv_get_tag(&lv2,"01C",cCashno);	
		}
		else if(memcmp(lv.CMTNo,"328", 3) == 0 )
		{
			lv_get_tag(&lv2,"CC1",cCashno);	
		}
		else if(memcmp(lv.CMTNo,"303", 3) == 0 && memcmp(lv.CMTNo,"319", 3) == 0 && 
				memcmp(lv.CMTNo,"320", 3) == 0 && memcmp(lv.CMTNo,"327", 3) == 0 )
		{
			lv_get_tag(&lv2,"58A",cCashno);	
		}

		/** 清算行号 **/
		if(memcmp(lv.CMTNo,"322", 3) == 0 || memcmp(lv.CMTNo,"324", 3) == 0 || memcmp(lv.CMTNo,"325", 3) == 0)
		{
			lv_get_tag(&lv2,"011",cCashqsactno);	
		}
		else if(memcmp(lv.CMTNo,"328", 3) == 0 )
		{
			lv_get_tag(&lv2,"CCO",cCashqsactno);	
		}
		else if(memcmp(lv.CMTNo,"920", 3) == 0 )
		{
			lv_get_tag(&lv2,"C37",cCashqsactno);	
		}
		else if(memcmp(lv.CMTNo,"254", 3) == 0 )
		{
			lv_get_tag(&lv2,"CC3",cCashqsactno);	
		}
		else if(memcmp(lv.CMTNo,"920", 3) == 0 )
		{
			lv_get_tag(&lv2,"C37",cCashqsactno);	
		}
		else 	/** 301,302,303,319,320,327,316 **/
		{
			lv_get_tag(&lv2,"012",cCashqsactno);	
		}
		
		if(memcmp(lv.CMTNo,"920", 3) == 0 || memcmp(lv.CMTNo,"316", 3) == 0	|| memcmp(lv.CMTNo,"254", 3) == 0
		   ||memcmp(lv.CMTNo,"324", 3) == 0 || memcmp(lv.CMTNo,"325", 3) == 0)
		{
			/** 得到接收行号 **/
			if(find_qsactno(cCashqsactno,cCashno)!=0)
			{
				vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,lvca.rtcode);
				_lv_errmsg("访问数据库错误!");
				
				
				return(-1);
			}
		}
	}


	/** 先判断是否是给本机构的报文 **/
	if(memcmp(cCashqsactno,cQsactno,12)!=0)
	{
        
        _lv_errmsg("接收清算行号不是本行!");
        vtcp_log("%s,%d,接收清算行号不是本行!\n",__FILE__,__LINE__);
        
		lv_destroy(&lv);
		lv_destroy(&lv2);
        return(-1);
	}

	if(memcmp(cCashno,cAcbrno,12)!=0)
	{
        
        _lv_errmsg("接收行号不是本行!");
        vtcp_log("%s,%d,接收行号不是本行!\n",__FILE__,__LINE__);
        
		lv_destroy(&lv);
		lv_destroy(&lv2);
        return(-1);
	}

	/** 先判断委托日期是否合法 **/
	if(memcmp(cTxday,lvca.tbsdy,8)!=0)
	{
        
        _lv_errmsg("来帐日期非法!");
        vtcp_log("%s,%d,来帐日期非法!\n",__FILE__,__LINE__);
        
		lv_destroy(&lv);
		lv_destroy(&lv2);
        return(-1);
	}

	vtcp_log("[%s][%d]============来帐报文编号是=cmtno=[%d]\n",__FILE__,__LINE__,cmtno);

	switch(cmtno)
	{
		case 1:
				lv_deal_ptdjlz(&lv);
				break;
		case 2:
				lv_deal_ptjjlz(&lv);
				break;
		case 3:
				lv_deal_ssdjlz(&lv);
				break;
		case 4:
				lv_deal_ssjjlz(&lv);
				break;
		case 5:
				lv_deal_dqdjlz(&lv);
				break;
		case 6:
				lv_deal_dqjjlz(&lv);
				break;
		case 7:
				lv_deal_djthlz(&lv);
				break;
		case 8:
				lv_deal_ptjjhzlz(&lv);
				break;
		case 9:
				lv_deal_ssdjhzlz(&lv);
				break;
		case 10:
				lv_deal_ssjjhzlz(&lv);
				break;
		case 11:
				lv_deal_dqjjhzlz(&lv);
				break;
		case 12:
				lv_deal_tyxxlz(&lv);
				break;
		case 13:
				lv_deal_ssxxlz(&lv);
				break;
		case 301:						/** 查询报文 **/
				lv_deal_cmt301(&lv);
				break;
		case 302:						/** 查复报文 **/
				lv_deal_cmt302(&lv);
				break;
		case 303:						/** 自由格式报文 **/
				lv_deal_cmt303(&lv);
				break;
		case 319:						/** 退回申请报文 **/
				lv_deal_cmt319(&lv);
				break;
		case 320:						/** 退回申请应答报文 **/
				lv_deal_cmt320(&lv);
				break;
		case 322:						/** 退回撤销应答报文 **/
				lv_deal_cmt322(&lv);
				break;
		case 324:						/** 实时业务冲正应答报文 **/
				lv_deal_cmt324(&lv);
				break;
		case 325:						/** 实时业务冲正通知报文 **/
				lv_deal_cmt325(&lv);
				break;
		case 327:						/** 借记业务止付申请报文 **/
				lv_deal_cmt327(&lv);
				break;
		case 328:						/** 借记业务止付应答报文 **/
				lv_deal_cmt328(&lv);
				break;
		case 254:						/** 小额轧差净额清算通知报文 **/
				lv_deal_cmt254(&lv);
				break;
		default:
        		
        		_lv_errmsg("来帐日期非法!");
        		vtcp_log("%s,%d,来帐日期非法!\n",__FILE__,__LINE__);
        		
				lv_destroy(&lv);
				lv_destroy(&lv2);
        		return(-1);
				
	}


	lv_destroy(&lv);
	lv_destroy(&lv2);

}

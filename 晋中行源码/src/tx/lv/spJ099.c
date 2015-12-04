/*************************************************
* 文 件 名:  spJ099.c
* 功能描述： 小额来账 登记来帐 子交易
*			       拆报文 登记来帐表
*			 
* 作    者:  ChenMing
* 完成日期： 2006年9月12日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"

int spJ099()
{
	char chkcmtno[4];
	char txtime[7];	
	char cErrmsg[9];
	long iSubpackIndex=0;
	int iStep=0;    
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("spJ098() 小额来账 登记来帐 开始  started! [%ld]",iSubpackIndex);

	vtcp_log(" --- spJ099 小额来账 登记来帐 开始 --- ");	 	
	pub_base_sysinit();
	memset(cErrmsg,0,sizeof(cErrmsg));
	memset(chkcmtno,'\0',sizeof(chkcmtno));
	memset(txtime,'\0',sizeof(txtime));

	get_zd_data("0510",chkcmtno);	
	vtcp_log("[%s][%d]--- chkcmtno =[%s] ",__FILE__,__LINE__,chkcmtno);	
	get_zd_long("0530",&iStep);	
	vtcp_log(" [%s][%d]--- iStep=[%d] ",__FILE__,__LINE__,iStep);
	
	if(iStep != -1) /* 处理序号，iStep==-1表示记帐处理结束 */
	{
    	/* ========= 322,324,325,920 报文帐务处理尚未结束    ========= 
    	   ========= 这些报文只有在最后帐务处理完毕时才登记  =========*/
    	if( (memcmp(chkcmtno,"322",3)==0 )||
    		  (memcmp(chkcmtno,"324",3)==0 )||
    		  (memcmp(chkcmtno,"325",3)==0 )||
    		  (memcmp(chkcmtno,"920",3)==0 ) )
    	{
    	    char cFinishflg[2];
    			memset(cFinishflg, 0 , sizeof(cFinishflg));
    			vtcp_log("[%s][%d]--- 322,324,325,920 报文帐务处理尚未结束 不登记 iStep=[%d] ",__FL__,iStep);
    		  	strcpy(g_pub_tx.reply,"0010" ); /* 跳回J098记帐 */
    		  	/* begin add by LiuHuafeng 2009-5-15 11:10:21 for 村镇银行 */
    			get_zd_data("0500",cFinishflg);
    			if(cFinishflg[0]=='1' && memcmp(chkcmtno,"920",3)==0)
    			{
    				strcpy(g_pub_tx.reply,"0000" ); /* 结束登记 */
    			}
    			/* end add by LiuHuafeng 2009-5-15 11:10:21 for 村镇银行 */
    			goto OkExit;
    	}
	}

	vLvBrnoDebug(__FILE__,__LINE__);
	/* 调用 小额来帐登记来帐 主函数 */
	if (pubf_lv_recv_reg_main(chkcmtno))
	{
		get_zd_data("0130",cErrmsg);
        if(memcmp(cErrmsg,"应答重复",8)==0)
        { 
           strcpy(g_pub_tx.reply,"P022" );
        }
        else
        {
           strcpy(g_pub_tx.reply,"0044" );
        }  
		vLvBrnoDebug(__FILE__,__LINE__);
		goto ErrExit;  
	}
	vLvBrnoDebug(__FILE__,__LINE__);
OkExit:
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("%s,%d spJ099() 小额来账 记帐 子交易结束 end! [%ld]",__FL__,iSubpackIndex);

	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("%s,%d spJ099() 小额来账 记帐 子交易结束 end! [%ld]",__FL__,iSubpackIndex);

	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

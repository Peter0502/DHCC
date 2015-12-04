/******************************************************************
模块名称: 大额交易汇总信息查询
程序名称: spJ028.c
函数原型: spJ028()
输入参数:
输出参数:
使用说明:
编 写 者: dxy
*******************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "hv_fd123.h"
#include "lv_pkgreg_c.h"
#include "hv_fzf_c.h"
#include "hv_zf_c.h"
#include "find_debug.h"
   


static struct {
	char brno[5];	/**交易机构**/
	char wtday[8];	/**委托日期**/
}tis19;



  
/*****************************************
*函数说明:获取一定日期内所有的交易的笔数 *
******************************************/
int spJ028()
{
	vtcp_log("[%s][%d]汇总交易交易查询\n",__FILE__,__LINE__);
	
	int	iLHD = 0;	/*汇兑*/
	int iLWT = 0;	/*委托*/
	int iLTS = 0; 	/*托收*/
	int iLGK = 0;	/*国库*/
	int iLCJ = 0;	/*拆借*/
	int iLTH = 0;	/*退汇*/
	int iLCX = 0;	/*查询*/
	int iLCF = 0;	/*查复*/

	int	iWHD = 0;	/*汇兑*/
	int iWWT = 0;	/*委托*/
	int iWTS = 0; 	/*托收*/
	int iWGK = 0;	/*国库*/
	int iWCJ = 0;	/*拆借*/
	int iWTH = 0;	/*退汇*/
	int iWCX = 0;	/*查询*/
	int iWCF = 0;	/*查复*/


	int iRecordNum = 0;

	char cWherelist[500];
	memset(cWherelist,0x00,sizeof(cWherelist));
	
	get_zd_data("0910",tis19.brno);
	get_zd_data("0440",tis19.wtday);
	
	
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) 
		&& ((tis19.brno[0]==0x00 ||tis19.brno[0]==' ')|| memcmp(g_pub_tx.tx_br_no,tis19.brno,5)))
	{
		vtcp_log("%s,%d,非清算中心不能查询他行交易\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
	if(!memcmp(g_pub_tx.tx_br_no, QS_BR_NO ,5))
	{
		memset(tis19.br_no,0x00,sizeof(tis19.br_no));
		if(tis19.brno[0]==0x00 ||tis19.brno[0]==' ')
		{
			MEMCPY_DEBUG(tis19.br_no,"%",1);	
		}
	}
	/*ret = Com_branch_Dec_Sel(g_pub_tx.reply,"br_no like '%%%s'  and wrk_sts!='*' and br_type not in('0','6') ) ",tis19.br_no);20150525 lwb 增加分行    */
	ret = Com_branch_Dec_Sel(g_pub_tx.reply,"br_no like '%%%s'  and wrk_sts!='*' and br_type not in('0','6','7') ) ",tis19.br_no);
	if(ret)
	{
		vtcp_log("[%s][%d]查询行号错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data("0130","查询行号错误");
		goto ErrExit;
	}
	while(1)
	{
		ret = Com_branch_Fet_Sel(&s_com_branch,g_pub_tx.reply);
		if(ret && ret!=100)
		{
			strcpy(g_pub_tx.reply,"HV01");
			set_zd_data("0130","提取机构号错误!");
			goto ErrExit;
		}
		if(ret==100)
			break;
		if(iRecordNum==0)
		{
			pub_base_AllDwnFilName( filename);
		    fp = fopen(filename,"w");
		    if(fp==NULL)
		   {
				sprintf(acErrMsg,"open file [%s] failed!\n",filename);
				WRITEMSG
				strcpy(g_pub_tx.reply,"S047");
				goto ErrExit;
			}
			/****显示标题****/
			
			fprintf(fp,"~@签发日期  |@签发行号      |@汇票号码      |$汇票金额        |@汇票种类        |@汇票状态       |@付款帐号                        |@付款姓名          |@附言|\n" );

		}
		iQuery_Tx_Cnt(char *tablename,char *cmtno,char lw_flag,char *wt_date,char *br_no)
			
	}
	
			
	
	vtcp_log("[%s][%d]tis19===[%s]\n",__FILE__,__LINE__,(char *)&tis19);

	vtcp_log("[%s][%d]开始执行汇总查询主程序!\n",__FILE__,__LINE__);
	

	
	
	
	
	
	set_zd_data("1230",(char *)&fd123);
	
	vtcp_log("[%s][%d]fd123====[%s]\n",__FILE__,__LINE__,(char *)&fd123);
	
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"查询汇总交易成功[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;

}

int iQuery_Tx_Cnt(char *tablename,char *cmtno,char lw_flag,char *wt_date,char *br_no)
{
	vtcp_log("[%s][%d]开始查询记录笔数,table_name=[%s],cmtno=[%s],lw_flag=[%c],date=[%s],br_no=[%s]\n",
						__FILE__,__LINE__,tablename,cmtno,lw_flag,wt_date,br_no);

	int iCnt = 0;
	iCnt = sql_count(tablename,"wt_date='%.8s and br_no='%.5s' and cmtno='%.3s' and lw_ind='%c'");

	vtcp_log("[%s][%d]查询到的记录笔数=[%d]\n",__FILE__,__LINE__,iCnt);

	return iCnt;
} 
	
	


/***************************************************************************/
/* 文件名称：大额支付营业状态公共函数。                                    */
/*                                                                         */
/* 功能描述：                                                              */
/*                                                                         */
/* 日期：  2005.1.20                                                       */
/* 作者：石升辉                                                            */
/***************************************************************************/
#include"stdio.h"
#include"stdlib.h"
#include"hvstat_c.h"
#include"com_sys_parm_c.h"
#include"hv_define.h"
#include "find_debug.h"
#define	HV_HD_MIN_MONEY 20000.00;
/*************************************************************************/
/* 函数名称：char hv_getstat()                                           */
/*                                                                       */
/* 功能描述：得到当前营业状态                                            */
/*                                                                       */
/* 输入：无                                                              */
/*                                                                       */
/* 返回值： 营业状态                                                     */
/*************************************************************************/
char hv_getstat(){
	int ret=0;
	char cReply[5];
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	ret=Hvstat_Sel(cReply,&hv_stat,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,取得营业状态错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(HV_STAT_UNKNOWN);
	}
	return(hv_stat.stat[0]);
}
/*************************************************************************/
/* 函数名称：char *hv_getday()                                           */
/*                                                                       */
/* 功能描述：得到当前营业日期                                            */
/*                                                                       */
/* 输入：无                                                              */
/*                                                                       */
/* 返回值： 营业日期                                                    */
/*************************************************************************/
int hv_getday(){
	int ret=0;
	char cReply[5];
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	ret=Hvstat_Sel(cReply,&hv_stat,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,取得营业时间错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(-1);
	}
	return(hv_stat.tx_date);
}
/*************************************************************************/
/* 函数名称：char hv_getmsg()                                           */
/*                                                                       */
/* 功能描述：得到当前描述                                         */
/*                                                                       */
/* 输入：无                                                              */
/*                                                                       */
/* 返回值： 描述                                                */
/*************************************************************************/
char *hv_getmsg(){
	int ret=0;
	static char pmsg[31];
	char cReply[5];
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(pmsg,'\0',sizeof(pmsg));

	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));

	ret=Hvstat_Sel(cReply,&hv_stat,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,取得当前信息描述错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(NULL);
	}
	MEMCPY_DEBUG(pmsg,hv_stat.message,sizeof(pmsg)-1);
	return(pmsg);
}
/*************************************************************************/
/* 函数名称：char hv_getfiller1()                                           */
/*                                                                       */
/* 功能描述：得到当前备注字段1                                         */
/*                                                                       */
/* 输入：无                                                              */
/*                                                                       */
/* 返回值： 备注字段1                                                 */
/*************************************************************************/
char *hv_getfiller1(){
	int ret=0;
	char pfiller1[31];
	char cReply[5];
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(pfiller1,'\0',sizeof(pfiller1));
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));

	ret=Hvstat_Sel(cReply,&hv_stat,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,取得当前备注错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(NULL);
	}
	MEMCPY_DEBUG(pfiller1,hv_stat.filler1,sizeof(pfiller1)-1);
	return(pfiller1);
}
/*************************************************************************/
/* 函数名称：void hv_setstat(char newstat)                               */
/*                                                                       */
/* 功能描述：设置当前营业状态                                            */
/*                                                                       */
/* 输入：要设置为的营业状态                                              */
/*                                                                       */
/* 返回值： 无                                                           */
/*************************************************************************/
int hv_setstat(char newstat){
	char cReply[5];
	int ret=0;
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	
	ret=Hvstat_Dec_Upd( cReply ,"1=1");
	if(ret)
	{
		vtcp_log("%s,%d,准备更新hv_stat表游标错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	ret=Hvstat_Fet_Upd(&hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,准备hv_stat数据错,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	hv_stat.stat[0]=newstat;
	/* Begin   Added by ChenMing   2006-10-31 14:33 */
	switch(newstat)
	{
		case '0':  MEMCPY_DEBUG(hv_stat.message,"营业准备",sizeof(hv_stat.message)-1); break;
		case '1':  MEMCPY_DEBUG(hv_stat.message,"日间处理",sizeof(hv_stat.message)-1); break;
		case '2':  MEMCPY_DEBUG(hv_stat.message,"截至准备",sizeof(hv_stat.message)-1); break;
		case '3':  MEMCPY_DEBUG(hv_stat.message,"业务截至",sizeof(hv_stat.message)-1); break;
		case '4':  MEMCPY_DEBUG(hv_stat.message,"清算窗口",sizeof(hv_stat.message)-1); break;
		case '5':  MEMCPY_DEBUG(hv_stat.message,"日终处理开始",sizeof(hv_stat.message)-1); break;
		case '6':  MEMCPY_DEBUG(hv_stat.message,"日终对账不平",sizeof(hv_stat.message)-1); break;
		case '7':  MEMCPY_DEBUG(hv_stat.message,"日终对账已经平",sizeof(hv_stat.message)-1); break;
		case 'F':  MEMCPY_DEBUG(hv_stat.message,"未知状态",sizeof(hv_stat.message)-1); break;
		default:
        sprintf(acErrMsg,"没有这种新状态!");
        WRITEMSG
        return -1;			
	}
	/* End   Added by ChenMing   2006-10-31 14:33 */	
	
	ret=Hvstat_Upd_Upd(hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,更新hv_stat数据错,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	return 0;
}
/*************************************************************************/
/* 函数名称：int hv_setday(int new_date)                                 */
/*                                                                       */
/* 功能描述：设置当前营业日期                                            */
/*                                                                       */
/* 输入：要设置为的营业日期                                              */
/*                                                                       */
/* 返回值： 无                                                           */
/*************************************************************************/
int hv_setday(int new_date){
	char cReply[5];
	int ret=0;
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	
	ret=Hvstat_Dec_Upd( cReply ,"1=1");
	if(ret)
	{
		vtcp_log("%s,%d,准备更新hv_stat表游标错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	ret=Hvstat_Fet_Upd(&hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,准备hv_stat数据错,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	hv_stat.tx_date=new_date;
	ret=Hvstat_Upd_Upd(hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,更新hv_stat数据错,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	return 0;
}
/*************************************************************************/
/* 函数名称：void hv_setmsg(char newmsg)                                 */
/*                                                                       */
/* 功能描述：设置当前营业状态更改描述                                    */
/*                                                                       */
/* 输入：要设置为的营业状态更改描述                                      */
/*                                                                       */
/* 返回值： 无                                                           */
/*************************************************************************/
int hv_setmsg(char *newmsg){
	char cReply[5];
	int ret=0;
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	
	ret=Hvstat_Dec_Upd( cReply ,"1=1");
	if(ret)
	{
		vtcp_log("%s,%d,准备更新hv_stat表游标错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	ret=Hvstat_Fet_Upd(&hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,准备hv_stat数据错,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	MEMCPY_DEBUG(hv_stat.message,newmsg,sizeof(hv_stat.message)-1);
	ret=Hvstat_Upd_Upd(hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,更新hv_stat数据错,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	return 0;
}
/*************************************************************************/
/* 函数名称：void hv_setfiller1(char newfiller1)                                 */
/*                                                                       */
/* 功能描述：设置当前备注字段1                                    */
/*                                                                       */
/* 输入：要设置为的备注字段1                                      */
/*                                                                       */
/* 返回值： 无                                                           */
/*************************************************************************/
int hv_setfiller1(char *newfiller1){
	char cReply[5];
	int ret=0;
	struct hvstat_c hv_stat;
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	
	ret=Hvstat_Dec_Upd( cReply ,"1=1");
	if(ret)
	{
		vtcp_log("%s,%d,准备更新hv_stat表游标错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	ret=Hvstat_Fet_Upd(&hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,准备hv_stat数据错,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	MEMCPY_DEBUG(hv_stat.filler1,newfiller1,sizeof(hv_stat.filler1)-1);
	ret=Hvstat_Upd_Upd(hv_stat,cReply);
	if(ret)
	{
		vtcp_log("%s,%d,更新hv_stat数据错,sqlcode=%d\n",__FILE__,__LINE__,ret);
		return(ret);
	}
	return 0;
}
/*************************************************************************/
/* 函数名称：void hv_checkstat()                                         */
/*                                                                       */
/* 功能描述：检查当前营业状态                                            */
/*                                                                       */
/* 输入：无                                                              */
/*                                                                       */
/* 返回值： 无                                                           */
/* 说明：  根据TITA检查当前状态是否允许做此（大额）业务，如果不允许，设置*/
/*         it_txcom.txrsut和错误描述，但不调用gs99或gs100                */
/*************************************************************************/
int iHv_checkstat(char * cmtno,char * yw_type){
	int ret=0;
	char cReply[5];
	struct hvstat_c hv_stat;
	struct com_sys_parm_c com_sys_parm;
	int iTx_tdat=0;
	char cStat;

	int  txno;
	int  txtype;
	char tmpBuf[3];
	char *cnapsday;
	cStat=txno=txtype=0;
	MEMSET_DEBUG(tmpBuf,'\0',sizeof(tmpBuf));

	MEMSET_DEBUG(cReply, 0 , sizeof(cReply));
	MEMSET_DEBUG(&hv_stat,'\0',sizeof(hv_stat));
	vtcp_log("%s,%d在这里设置1===1\n",__FILE__,__LINE__);
	ret=Hvstat_Sel(cReply,&hv_stat,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,取得营业状态错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		strncpy(acErrMsg,"取得营业状态错误",40);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	MEMSET_DEBUG(&com_sys_parm, 0 , sizeof(com_sys_parm));
	ret=Com_sys_parm_Sel(cReply,&com_sys_parm,"1=1");
	if(ret!=0){
		vtcp_log("%s,%d,取得公共系统参数表错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		strncpy(acErrMsg,"取得公共系统参数表错误",40);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	if(com_sys_parm.sys_sts!=0)
	{
		vtcp_log("%s,%d,取得公共系统参数表错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
		strncpy(acErrMsg,"取得公共系统参数表错误",40);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}

	if(com_sys_parm.sys_date!=hv_stat.tx_date){
		vtcp_log("%s,%d,综合业务与支付系统工作日期不一致[%d][%d]!\n",
			__FILE__,__LINE__,com_sys_parm.sys_date,hv_stat.tx_date);
		strncpy(acErrMsg,"综合业务与支付系统工作日期不一致",40);
		set_zd_data("0130",acErrMsg);
		return 1;
	}
	switch(hv_stat.stat[0]){
	case HV_STAT_PREPARE:
		/*营业准备状态,只能接收，不能发起业务*/
		vtcp_log("%s,%d,营业准备状态不能发起交易\n",__FILE__,__LINE__);
		strncpy(acErrMsg,"营业准备状态不能发起交易",40);
		set_zd_data("0130",acErrMsg);
		return 1;
	case HV_STAT_WORK:
		/*日间营业状态*/
		vtcp_log("%s,%d,日间营业状态能发起所有交易\n",__FILE__,__LINE__);
		return 0;
	case HV_STAT_WARN:
		/*截至警告状态*/
		vtcp_log("%s,%d,业务截至警告状态能发起所以交易\n",__FILE__,__LINE__);
		return 0;
	case HV_STAT_STOP:
		/*业务截至状态*/
		vtcp_log("%s,%d,业务截至状态不能发起交易\n",__FILE__,__LINE__);
		strncpy(acErrMsg,"业务截止状态不能发起交易",40);
		set_zd_data("0130",acErrMsg);
		return 1;
	case HV_STAT_QSWIN:
		/*清算窗口状态只能向支付系统发起弥补头寸不足之类的大额支付业务
		 即汇兑业务中的资金调拨、以及同业拆借业务等）、清算账户信息、自由格式报文
		*/
		if(!memcmp(cmtno,HV_HD,3)){
			/*汇兑业务*/
			if(!memcmp(yw_type,"40",2)){
				/*资金调拨*/
				return 0;
			}else{
				vtcp_log("%s,%d,清算窗口状态不能发起交易,汇兑业务,txtype=%02d\n",__FILE__,__LINE__,txtype);
				strncpy(acErrMsg,"清算窗口状态不能发起交易",40);
				set_zd_data("0130",acErrMsg);
				return 1;
			}
		}else if(!memcmp(cmtno,HV_CJ,3)){
			/*拆借业务*/
			return 0;
		}else if(memcmp(cmtno,"303",3)==0){
			/*自由格式*/
			return 0;
		}else if(memcmp(cmtno,"301",3)==0){
			/* 查询*/
			return 0;
		}else if(memcmp(cmtno,"302",3)==0){
			/* 查复*/
			return 0;
		}else if(memcmp(cmtno,"311",3)==0){
			/*申请撤销格式*/
			return 0;
		}else{
			vtcp_log("%s,%d,清算窗口状态不能发起交易,txno=%02d,txtype=%02d\n",__FILE__,__LINE__,txno,txtype);
			strncpy(acErrMsg,"清算窗口状态不能发起交易",40);
			return 1;
		}
	case HV_STAT_WORKEND:
		vtcp_log("%s,%d,日终状态不能发起交易\n",__FILE__,__LINE__);
		strncpy(acErrMsg,"日终状态不能发起交易",40);
		set_zd_data("0130",acErrMsg);
		return 1;
	case HV_STAT_UNKNOWN:
	default:
		vtcp_log("%s,%d,当前营业状态错误[%d]\n",__FILE__,__LINE__,cStat);
		strncpy(acErrMsg,"当前营业状态错误",40);
		set_zd_data("0130",acErrMsg);
		return 1;
	}
}
double dGetHvMinTxamt( )
{
	double dMinTxamt=0;
	char cMinTxamt[12];
	char cTxday[9];
	char cSqlbuf[1024];
	int iRet=0;
	MEMSET_DEBUG(cSqlbuf, 0 , sizeof(cSqlbuf));
	MEMSET_DEBUG(cTxday, 0 , sizeof(cTxday));
	MEMSET_DEBUG(cMinTxamt, 0 , sizeof(cMinTxamt));
	get_zd_data(DC_DATE,cTxday);
	sprintf(cSqlbuf,"(select max(sdate) from hv_syspara where sdate<='%s' and datatype='2')",cTxday);
	iRet=sql_count_sum_double ( "hv_syspara" , "startamt" , &dMinTxamt , "sdate=%s" , cSqlbuf );
	if(dMinTxamt==0){
		dMinTxamt=HV_HD_MIN_MONEY;
	}
	vtcp_log("%s,%d 得到的起点金额===[%f]",__FILE__,__LINE__,dMinTxamt);
	return dMinTxamt;
}

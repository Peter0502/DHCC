/***************************************************************
* 文 件 名:     spJ085.c
* 功能描述：    大小额系统状态查询
*
* 作    者:   LiuYue
* 完成日期：  2006-8-18	09:20 
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include "hvpack.h"
#include "hv_define.h"
#define EXTERN
#include "hvstat_c.h"
#include "lv_sysctl_c.h"
#include "hv_fd123.h"


struct HV_STAT{
	char cTx_date	[8]; 	/*交易日期*/
	char cStat		[1];    /*状态 */
	char cChflag	[1];  	/*最近一次更新标志:0-成功,1-失败*/
	char cMessage	[60]; 	/*更新描述*/
	char cFiller1	[30]; 	/*备注1*/
	char cFiller2	[30]; 	/*备注2*/
};

struct LV_STAT{
	char cTx_date	[8];		/*交易日期*/
	char cStat		[1];		/*交易状态*/
	char cNext_date	[8]; 		/*下一个工作日期*/
	char cChgnode	[4];		/*变更节点*/
	char cCurstat	[8];		/*营业状态*/
	char cChgtime	[14];		/*变更时间*/
	char cMessage	[60];		/*更新描述*/
	char cCheckstat	[1];		/*对帐标志0-对帐已平可以日切 1-未对帐不可日切*/
	char cFiller	[100];		/*备注*/
};



spJ085()
{
		int ret = 0;
		char flag[2]  ;/***0 --大额系统查询  1-小额系统查询****/

		char  filename[40];
		FILE *fp;
		struct hvstat_c hvstat;
		struct lv_sysctl_c lv_sysctl;
		struct HV_STAT  my_hvstat;
		struct LV_STAT  my_lvstat;
		HV_FD123_AREA   fd123;
		char cTxdate[9];
		char cNexdate[9];




		memset(filename,	0x00,	sizeof(filename));
		memset(&hvstat,		0x00,	sizeof(hvstat));
		memset(&lv_sysctl,	0x00,	sizeof(lv_sysctl));
		memset(&flag,		0x00,	sizeof(flag));
		memset(&fd123,		0x00,	sizeof(fd123));

		memset(cTxdate,		0x00,	sizeof(cTxdate));
		memset(cNexdate,	0x00,	sizeof(cNexdate));
		memset(&my_hvstat,	0x00,	sizeof(my_hvstat));
		memset(&my_lvstat,	0x00,	sizeof(my_lvstat));





		/********************
		* 1 初始化全局变量 *
		********************/

		pub_base_sysinit();


		/*******************
		* 2.获取前台数据  *
		*******************/

		get_fd_data("0670",flag);

		/********************
		* 3.执行基本的检查 *
		********************/

		if(flag[0]!='1' && flag[0]!='2')
		{
			vtcp_log("[%s][%d]查询类型错误!\n",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"请选择大额/小额");
			strcpy(g_pub_tx.reply,"NO");
			goto ErrExit;	
		}


		/*******************
		* 4.数据库操作    *
		*******************/

		if(flag[0]=='1')
		{
			vtcp_log("[%s][%d]开始查询大额系统状态!\n",__FILE__,__LINE__);
			ret = Hvstat_Sel(g_pub_tx.reply,&hvstat,"1=1");
			if(ret &&ret!=100)
			{
				vtcp_log("[%s][%d]数据库操作错误!\n",__FILE__,__LINE__);
				goto ErrExit;	
			}
			else if(ret==100)
			{
				vtcp_log("[%s][%d]数据库不存在记录!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"数据表中没有相关记录!");
				goto ErrExit;
			}
			vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
			/****由于状态中那些附言过长 不适合再列表中显示,特意放到123域中返回,获取填充界面**/
			sprintf(cTxdate,"%08d",hvstat.tx_date);
			vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
			vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
			vtcp_log("[%s][%d]hvstat.filler1==[%s][%d][%d]\n",__FILE__,__LINE__,hvstat.filler1,sizeof(fd123),sizeof(hvstat.filler1));
			memcpy(my_hvstat.cTx_date,	cTxdate,				sizeof(my_hvstat.cTx_date));
			my_hvstat.cStat[0]		 =		hvstat.stat[0];
			my_hvstat.cChflag[0] 	 =		hvstat.chflag[0];
			memcpy(my_hvstat.cMessage,		hvstat.message,			sizeof(hvstat.message)-1);
			memcpy(my_hvstat.cFiller1,		hvstat.filler1,			sizeof(hvstat.filler1)-1);
			memcpy(my_hvstat.cFiller2,		hvstat.filler2,			sizeof(hvstat.filler2)-1);



			vtcp_log("[%s][%d]my_hvstat==[%s]\n",__FILE__,__LINE__,(char *)&my_hvstat);
			memcpy(&fd123,&my_hvstat,sizeof(my_hvstat));
			
			

			vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
			
		}
		else if(flag[0]=='2')
		{
			vtcp_log("[%s][%d]开始小额的系统状态查询!\n",__FILE__,__LINE__);
			ret = Lv_sysctl_Sel(g_pub_tx.reply,&lv_sysctl,"1=1");
			if(ret &&ret!=100)
			{
				vtcp_log("[%s][%d]数据操作错误!\n",__FILE__,__LINE__);
				goto ErrExit;
			}
			else if(ret==100)
			{
				vtcp_log("[%s][%d]数据库记录不存在!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"小额系统记录不存在!");
				goto ErrExit;
			}
			sprintf(cTxdate,	"%08d",		lv_sysctl.tx_date);
			sprintf(cNexdate,	"%08d",		lv_sysctl.next_date);
		


			memcpy(my_lvstat.cTx_date,			cTxdate,				sizeof(cTxdate)-1);
			my_lvstat.cStat[0]			=		lv_sysctl.stat[0];
			memcpy(my_lvstat.cNext_date,		cNexdate,				sizeof(cNexdate));
			memcpy(my_lvstat.cChgnode,			lv_sysctl.chgnode,		sizeof(lv_sysctl.chgnode)-1);
			memcpy(my_lvstat.cCurstat,			lv_sysctl.curstat,		sizeof(lv_sysctl.curstat)-1);
			memcpy(my_lvstat.cChgtime,			lv_sysctl.chgtime,		sizeof(lv_sysctl.chgtime)-1);
			memcpy(my_lvstat.cMessage,			lv_sysctl.message,		sizeof(lv_sysctl.message)-1);
			my_lvstat.cCheckstat[0]			=	lv_sysctl.checkstat[0];
			memcpy(my_lvstat.cFiller,			lv_sysctl.filler,		sizeof(lv_sysctl.filler)-1);				

		

			vtcp_log("[%s][%d]my_lvstat===[%s]\n",__FILE__,__LINE__,(char *)&my_lvstat);
			memcpy(&fd123,&my_lvstat,sizeof(my_lvstat));

		}

		/********************
		 * 6.返回处理结果   *
		 ********************/
		pub_base_full_space((char *)&fd123,sizeof(fd123));
		set_zd_data("1230",(char *)&fd123);

		vtcp_log("[%s]\n",(char *)&fd123);
 


OkExit:
		strcpy(g_pub_tx.reply,"0000");
		sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
		WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 0;
ErrExit:
		sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
		WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;
}







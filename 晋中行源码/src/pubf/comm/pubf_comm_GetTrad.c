#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_comm_GetTrad.c
* 功能描述：
*              get_trad     取得回传比特图
*
* 作    者:    jack
* 完成日期：   2003年12月16日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
*************************************************/
#include <stdio.h>
#define EXTERN
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "svrpub_c.h"
#include "public.h"
#include "com_tel_c.h"
struct  tx_def_c g_tx_def_c;

int get_trad(char *snd_bit)
{
	char    tx_code[5];
	char    br_code[6];
	char    tel[5];/**char    tel[4]; update by spj 20140325**/
	long	sys_date;
	/**add by spj 20140325 BEG**/
	int ret;
	struct com_tel_c com_tel;
	/**add by spj 20140325 END**/
	struct  com_sys_parm_c sys_parm;

	/* 系统开始 */
	MEMSET_DEBUG(tx_code, 0, sizeof(tx_code));
	MEMSET_DEBUG(br_code, 0, sizeof(br_code));
	MEMSET_DEBUG(tel, 0, sizeof(tel));

	get_zd_char(DC_TX_CODE,tx_code);
	get_zd_char(DC_OPN_BR_NO,br_code);
	get_zd_char(DC_TEL,tel);
	get_zd_long(DC_DATE,&sys_date);
	tx_code[4]='\0';

	if ( Com_sys_parm_Sel( RtCode , &sys_parm, "%s" ,"1=1" ) )
	{
		strcpy(RtCode,"S005");
		if(strncmp(tx_code,"8101",4)) goto ERREXIT;
	}
	SysDate = sys_parm.sys_date;

	vtcp_log("[%s][%d]",__FILE__, __LINE__);
	/****** For Test   ******/
	if(memcmp(tx_code,"61",2)==0||memcmp(tx_code,"62",2)==0||memcmp(tx_code,"78",2)==0||memcmp(tx_code,"63",2)==0)  
	{
		vtcp_log("tx_code===[%s]",tx_code);
		vtcp_log("br_code===[%s]",br_code);
		vtcp_log("tel===[%s]",tel);
		vtcp_log("sys_date===[%ld]",sys_date);
		vtcp_log("SysDate===[%ld]",SysDate);
		if( sys_date != SysDate)
		{
			sys_date=SysDate;
			set_zd_long(DC_DATE,sys_date);
		}
	}
	
	/**add by spj 20140325 BEG**/
	ret = Com_tel_Sel(RtCode, &com_tel, "tel='%s'", tel);
	if (100 == ret)
	{
		sprintf(acErrMsg, "该柜员号不存在!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "O120");
		goto ERREXIT;
	}
	if (0 != ret)
	{
		sprintf(acErrMsg, "查询柜员信息表错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ERREXIT;
	}
	vtcp_log("tel.lvl===[%s]",com_tel.lvl);
	/**add by spj 20140325 END**/

	vtcp_log("[%s][%d],tx_code===[%s]",__FILE__,__LINE__,tx_code);
	/**Del by hxc 110215 for 批量翻盘外不控制卡系统以及电话银行部分交易**/
	/**
	if(sys_parm.sys_sts!=0 && tx_code[0]!='9' )
	{
		vtcp_log("sys_ind[%s]",sys_parm.sys_ind);
		strcpy(RtCode,"S006");
		goto ERREXIT;
	}
	**/
	/**Add by hxc 110215 for 批量翻盘外不控制卡系统以及电话银行部分交易**/
	if (sys_parm.sys_sts==3)
	{
		if(tx_code[0]!='9')
		{
			vtcp_log("%s,%d 批量最高级别，仅仅可以让9类型执行 ",__FILE__,__LINE__); 
			vtcp_log("sys_ind[%s]",sys_parm.sys_ind);
			strcpy(RtCode,"S006");
			goto ERREXIT;
		}
	}
	else if( sys_parm.sys_sts==2 )
	{
		/** 20110215 批量放过卡系统以及电话银行部分交易 9*** 6202 6203 8501 8105 1307 4405 8507**/
		if(tx_code[0]=='9' || !memcmp(tx_code,"6203",4) || !memcmp(tx_code,"6202",4)  || !memcmp(tx_code,"8105",4) || !memcmp(tx_code,"1307",4) )
		{
			vtcp_log("%s,%d 批量未翻盘,是9类和卡业务可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		else if( !memcmp(tx_code,"4405",4) || !memcmp(tx_code,"8501",4) || !memcmp(tx_code,"8507",4))
		{
			vtcp_log("%s,%d 批量未翻盘,是9类和卡业务可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		/** 20130922 核心处于此种系统状态下，仍允许网银业务进行 6221 8683 8286 6388 6389 8288 8689 8273 8287 hzh**/
		else if( !memcmp(tx_code,"6221",4) || !memcmp(tx_code,"8683",4) || !memcmp(tx_code,"8286",4) || !memcmp(tx_code,"6388",4)|| !memcmp(tx_code,"6389",4)|| !memcmp(tx_code,"8288",4)|| !memcmp(tx_code,"8689",4)|| !memcmp(tx_code,"8273",4) || !memcmp(tx_code,"8287",4))
		{
			vtcp_log("%s,%d 核心系统状态为[2],网银交易可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		/** 20131209 核心处于此种系统状态下，仍允许电话银行业务进行 lkd**/
		else if( !memcmp(tx_code,"9734",4) || !memcmp(tx_code,"8891",4) || !memcmp(tx_code,"9130",4)|| !memcmp(tx_code,"9102",4)|| !memcmp(tx_code,"9338",4)|| !memcmp(tx_code,"8100",4)|| !memcmp(tx_code,"4307",4)||!memcmp(tx_code,"7868",4))
		{
			vtcp_log("%s,%d 核心系统状态为[2],电话银行交易可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		/** 20140714 核心处于此种系统状态下，仍允许二代支付的相关交易进行 chenggx**/
		else if( !memcmp(tx_code,"7021",4) || !memcmp(tx_code,"7023",4) || !memcmp(tx_code,"6236",4) || !memcmp(tx_code,"6247",4) || !memcmp(tx_code,"7022",4) )
		{
			vtcp_log("%s,%d 核心系统状态为[2],二代支付交易可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		else if (!memcmp(tx_code,"6253",4) || !memcmp(tx_code,"6254",4) || !memcmp(tx_code,"6238",4))
		{
			vtcp_log("[%s][%d] 核心系统状态为[2],同城清算交易可以执行! txcode=[%s]", __FILE__, __LINE__, tx_code);
		}
		/**add by spj 20140325**/
		else if (0 == memcmp(com_tel.lvl, "9", 1))
		{
			vtcp_log("%s,%d 核心系统状态为[2],VTM交易可以执行,tel=%s ", __FILE__, __LINE__, tel);
		}
		else
		{
			vtcp_log("%s,%d 批量未翻盘,仅仅可以让9类和卡业务执行,tx_code=%s",__FILE__,__LINE__,tx_code); 
			vtcp_log("sys_ind[%s]",sys_parm.sys_ind);
			strcpy(RtCode,"S006");
			goto ERREXIT;
		}
	}
	else if(sys_parm.sys_sts==1)
	{
		/** 20110215 批量放过卡系统以及电话银行部分交易 9*** 6202 6203 8501 8105 1307 4405 8507**/
		if(tx_code[0]=='9' || !memcmp(tx_code,"6203",4) || !memcmp(tx_code,"6202",4)  || !memcmp(tx_code,"8105",4) || !memcmp(tx_code,"1307",4) )
		{
			vtcp_log("%s,%d 批量备份完成,9类和卡业务可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		else if( !memcmp(tx_code,"4405",4) || !memcmp(tx_code,"8501",4) || !memcmp(tx_code,"8507",4))
		{
			vtcp_log("%s,%d 批量备份完成,9类和卡业务可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		/** 20130922 核心处于此种系统状态下，仍允许网银业务进行 6221 8683 8286 6388 6389 8288 8689 8273 8287 hzh**/
		else if( !memcmp(tx_code,"6221",4) || !memcmp(tx_code,"8683",4) || !memcmp(tx_code,"8286",4) || !memcmp(tx_code,"6388",4)|| !memcmp(tx_code,"6389",4)|| !memcmp(tx_code,"8288",4)|| !memcmp(tx_code,"8689",4) || !memcmp(tx_code,"8273",4) || !memcmp(tx_code,"8287",4))
		{
			vtcp_log("%s,%d 核心系统状态为[1],网银交易可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		/** 20131209 核心处于此种系统状态下，仍允许电话银行业务进行 lkd**/
		else if( !memcmp(tx_code,"9734",4) || !memcmp(tx_code,"8891",4) || !memcmp(tx_code,"9130",4)|| !memcmp(tx_code,"9102",4)|| !memcmp(tx_code,"9338",4)|| !memcmp(tx_code,"8100",4)|| !memcmp(tx_code,"4307",4)||!memcmp(tx_code,"7868",4))
		{
			vtcp_log("%s,%d 核心系统状态为[1],电话银行交易可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		/** 20140714 核心处于此种系统状态下，仍允许二代支付的相关交易进行 chenggx**/
		else if( !memcmp(tx_code,"7021",4) || !memcmp(tx_code,"7023",4) || !memcmp(tx_code,"6236",4) || !memcmp(tx_code,"6247",4) || !memcmp(tx_code,"7022",4))
		{
			vtcp_log("%s,%d 核心系统状态为[1],二代支付交易可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		else if (!memcmp(tx_code,"6253",4) || !memcmp(tx_code,"6254",4) || !memcmp(tx_code,"6238",4))
		{
			vtcp_log("[%s][%d] 核心系统状态为[1],同城清算交易可以执行! txcode=[%s]", __FILE__, __LINE__, tx_code);
		}
		/**add by spj 20140325**/
		else if (0 == memcmp(com_tel.lvl, "9", 1))
		{
			vtcp_log("%s,%d 核心系统状态为[1],VTM交易可以执行,tel=%s ", __FILE__, __LINE__, tel);
		}
		else
		{
			vtcp_log("%s,%d 批量前备份完成,仅仅可以让9类和卡业务型执行,tx_code=%s ",__FILE__,__LINE__,tx_code); 
			vtcp_log("sys_ind[%s]",sys_parm.sys_ind);
			strcpy(RtCode,"S006");
			goto ERREXIT;
		}
	}
	/**End by hxc 110215 for 批量翻盘外不控制卡系统以及电话银行部分交易**/
	
	/****Add by SSH,2006.9.1,取后台时间为6域赋值****/
	{
		char pcTime[32];
		MEMSET_DEBUG(pcTime,'\0',sizeof(pcTime));
		iGetDateTime(pcTime);
		set_zd_data(DC_TIME,pcTime+8);
	}

	vtcp_log("[%s][%d]",__FILE__, __LINE__);
	if( sys_parm.sys_ind[2]=='1' && strcmp(br_code,"99999") )
	{
		vtcp_log("sys_ind[%s]",sys_parm.sys_ind);
		pub_base_wrt_log();
		strcpy(RtCode,"S006");
		goto ERREXIT;
	}
	vtcp_log("[%s][%d]",__FILE__, __LINE__);
	/**99999999表示不效验日期**/
	/****Modified by SSH,2006.6.12,9998交易取机构号不检查日期 ****/
	if( sys_date==99999999 || !strcmp(tx_code,"9988")||!strcmp(tx_code,"9998") )  
	{
		sys_date=SysDate;
		set_zd_long(DC_DATE,sys_date);
	}

	vtcp_log("[%s][%d]",__FILE__, __LINE__);
	if ( Tx_def_Sel( RtCode,&g_tx_def_c," tx_code=\'%s\' ", tx_code ) )
	{
		strcpy ( RtCode , "S007" );
		set_zd_data(DC_REPLY,RtCode);
		goto ERREXIT;
	}
	/**ADD BY XJ 20091228晋中**/
	if(memcmp(tx_code,"0990",4)==0){
		sys_date=SysDate;
		vtcp_log("sys_date===[%ld]",sys_date);
		}	
	if(SysDate!=sys_date && strcmp(tx_code,"9001")!=0)
	/*JYW add 批量处理在翻牌后SysDate与sys_date不相等所以不检查了,否则批量无法进行 */
	{
		if (0==strcmp(tx_code,"1799")||0==strcmp(tx_code,"9130") || 0==strcmp(tx_code,"8891")|| 0==strcmp(tx_code,"9734")|| 0==strcmp(tx_code,"9102")|| 0==strcmp(tx_code,"9338"))/*1799交易放过去 by zzz20121015*/
		{
	    vtcp_log("非税tx_code=[%s]SysDate=%d,qtdate=%d",tx_code,SysDate,sys_date);
		}
		/** 20131209 核心处于此种系统状态下，仍允许电话银行业务进行 lkd**/
		else if(!memcmp(tx_code,"6203",4) || !memcmp(tx_code,"6202",4)  || !memcmp(tx_code,"8105",4) || !memcmp(tx_code,"8100",4)|| !memcmp(tx_code,"4307",4)|| !memcmp(tx_code,"4405",4) )
		{
			vtcp_log("%s,%d 批量备份完成,9类和卡业务可以执行,txcode=%s ",__FILE__,__LINE__,tx_code); 
		}
		else
		{
	    vtcp_log("tx_code=[%s]SysDate=%d,qtdate=%d",tx_code,SysDate,sys_date);
	    strcpy(RtCode,"S008");
		/*if(g_tx_def_c.wrk_sts[0]!='0')**xxx20031107**/ 
			goto ERREXIT;
		}
	}
	

	strcpy ( snd_bit , g_tx_def_c.bit_map );
	strcpy ( RtCode , "0000" );
	get_zd_data ( DC_REPLY,RtCode );

	return 0;

ERREXIT:
	set_zd_data(DC_REPLY,RtCode);

    return 1;
}

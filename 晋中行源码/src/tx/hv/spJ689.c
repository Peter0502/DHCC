/*************************************************
* 文 件 名:	spJ689.c
* 功能描述：业务明细下载报文子交易
* 作    者: jane
* 完成日期: 2004年6月30日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/

#define	EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "com_parm_c.h"
#include "find_debug.h"

spJ689()
{
	char filename[50],batchpack[1024],tmpstr[2];
	FILE *fp;
	int itxcnt,otxcnt,totalcnt,i,j;
	struct mbfeghvsendlist_c mbfeghvsendlist_v;
	struct com_parm_c com_parm_v;

	sprintf(filename,"%s%s%s",getenv("HOME"),"/txt/",mbfetran.filename);
	fp=fopen(filename,"r");

	memset(batchpack,0x00,sizeof(batchpack));
	fread(batchpack,sizeof(char),44,fp);

	/*拆批量业务头块*/
	if (split_batchcmt(batchpack)) goto ErrExit;

	itxcnt=atol(mbfetran.itxlendcnt)+atol(mbfetran.itxloancnt);
	otxcnt=atol(mbfetran.otxlendcnt)+atol(mbfetran.otxloancnt);
	totalcnt=itxcnt+otxcnt;

	/*拆业务明细正文块*/
	g_pub_tx.tmp_amt1=0.00;
	g_pub_tx.tmp_amt2=0.00;
	for (i=0;i<totalcnt;i++)
	{
		j=0;
		memset(batchpack,0x00,sizeof(batchpack));
		memset(tmpstr,0x00,sizeof(tmpstr));
		while (fread(tmpstr,sizeof(char),1,fp))
		{
			strcat(batchpack,tmpstr);
			if (!strcmp(tmpstr,"}")) j++;
			if (j==2) break;
		}

		memset(&mbfetran,0x00,sizeof(struct MBFETRAN));
		if (split_ophead(batchpack)) goto ErrExit;
		if (split_body(&batchpack[24])) goto ErrExit;

		if (i<itxcnt)
		{
			/*查找是否有相对应的记录*/
			g_reply_int=sql_count("mbfeghvrecvlist","txssno='%s' and consigndate='%s' and odficode='%s'",mbfetran.txssno,mbfetran.consigndate,mbfetran.odficode);
			if (g_reply_int<0) goto ErrExit;
			else if (g_reply_int==0)
			{
				strcpy(mbfetran.procstate,"58");
				g_pub_tx.tmp_amt1+=atof(mbfetran.amount)/100;
				if (pub_mbfe_rlreg()) goto ErrExit;

				g_reply_int=sql_execute("update mbfeghvrecvlist set checkstate='10' where txssno='%s' and consigndate='%s' and odficode='%s'",mbfetran.txssno,mbfetran.consigndate,mbfetran.odficode);
				if (g_reply_int)
				{
					sprintf(acErrMsg,"更新来账对账状态错 [%d]",g_reply_int);
					WRITEMSG
					goto ErrExit;
				}
			}
			else
			{
				/*更新相应来账业务对账状态*/
				g_reply_int=sql_execute("update mbfeghvrecvlist set checkstate='11' where txssno='%s' and consigndate='%s' and odficode='%s'",mbfetran.txssno,mbfetran.consigndate,mbfetran.odficode);
				if (g_reply_int)
				{
					sprintf(acErrMsg,"更新来账对账状态错 [%d]",g_reply_int);
					WRITEMSG
					goto ErrExit;
				}
			}
		}
		else
		{
			/*更新相应往账业务对账状态*/
			memset(&mbfeghvsendlist_v,0x00,sizeof(struct mbfeghvsendlist_c));
			g_reply_int=Mbfeghvsendlist_Sel(g_pub_tx.reply,&mbfeghvsendlist_v,"txssno='%s' and consigndate='%s'",mbfetran.txssno,mbfetran.consigndate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"检索往帐业务出错 [%d]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			if (mbfeghvsendlist_v.procstate[0]=='0')
			{
				g_reply_int=sql_execute("update mbfeghvsendlist set checkstate='11' where txssno='%s' and consigndate='%s'",mbfetran.txssno,mbfetran.consigndate);
				if (g_reply_int)
				{
					sprintf(acErrMsg,"更新往账对账状态错 [%d]",g_reply_int);
					WRITEMSG
					goto ErrExit;
				}
			}
			else if (!strcmp(mbfeghvsendlist_v.procstate,"10") || 
					 !strcmp(mbfeghvsendlist_v.procstate,"20"))  /*NPC已清算，行内系统未正确接收253报文*/
			{
				g_pub_tx.tmp_amt2+=atof(mbfetran.amount)/100;
				g_reply_int=sql_execute("update mbfeghvsendlist set procstate='00',checkstate='10' where txssno='%s' and consigndate='%s'",mbfetran.txssno,mbfetran.consigndate);
				if (g_reply_int)
				{
					sprintf(acErrMsg,"更新往账对账状态错 [%d]",g_reply_int);
					WRITEMSG
					goto ErrExit;
				}
			}
		}
	}

	/*更新没有被对账的往账为滞留状态-已滞留和已撤销的改为11*/
	g_reply_int=sql_execute("update mbfeghvsendlist set checkstate='11' where procstate[1]='*' and checkstate='00'");
	if (g_reply_int)
	{
		sprintf(acErrMsg,"更新往账对账状态错 [%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int=sql_execute("update mbfeghvsendlist set procstate='30',checkstate='10' where checkstate='00'");
	if (g_reply_int)
	{
		sprintf(acErrMsg,"更新往账对账状态错 [%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	/*更新明细下载标志*/
	g_reply_int=sql_execute("update mbfe_sys_ctl set par_value='2' where par_code='06'");
	if (g_reply_int)
	{
		sprintf(acErrMsg,"更新明细下载标志错 [%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	/*更新系统状态*/
	g_reply_int=sql_execute("update mbfe_sys_ctl set par_value='00' where par_code='05'");
	if (g_reply_int)
	{
		sprintf(acErrMsg,"更新系统状态错 [%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	if (g_pub_tx.tmp_amt1>0)
	{
		memset(&com_parm_v,0x00,sizeof(struct com_parm_c));
		g_reply_int=Com_parm_Sel(g_pub_tx.reply, &com_parm_v, "parm_code='DEKM' and parm_seqn='3'");
		if (g_reply_int)
		{
			sprintf(acErrMsg,"查询日间清算账号出错 [%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1201",com_parm_v.val);
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",g_pub_tx.tmp_amt1);
		set_zd_data("120A","大额来账补记");

		memset(&com_parm_v,0x00,sizeof(struct com_parm_c));
		g_reply_int=Com_parm_Sel(g_pub_tx.reply, &com_parm_v, "parm_code='DEKM' and parm_seqn='2'");
		if (g_reply_int)
		{
			sprintf(acErrMsg,"查询来账挂账账号出错 [%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1211",com_parm_v.val);
		set_zd_data("1214","01");
		set_zd_data("1215","2");
		set_zd_double("1218",g_pub_tx.tmp_amt1);
		set_zd_data("121A","大额来账补记");

		strcpy(g_pub_tx.reply,"0001");
		goto OkExit1;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

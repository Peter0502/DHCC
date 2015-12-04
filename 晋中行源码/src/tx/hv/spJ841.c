/*************************************************
* 文 件 名:	spJ841.c
* 功能描述：日终对账报文处理
* 作    者: jane
* 完成日期: 2004年6月28日
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
#include "find_debug.h"

spJ841()
{
	long otxtotalcnt,itxtotalcnt;
	double otxtotalamnt,itxtotalamnt;
	char checkflg[2];

	if (register_hpreg()) goto ErrExit;

	/*往账校对*/
	otxtotalcnt=sql_count("mbfeghvsendlist","consigndate=%ld and procstate[1]='0'",g_pub_tx.tx_date);
	if (otxtotalcnt<0) 
	{
		sprintf(acErrMsg,"大额往账明细表数据库操作失败[%d]",otxtotalcnt);
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int=sql_sum_double("mbfeghvsendlist","amount",&otxtotalamnt,"consigndate=%ld and procstate[1]='0'",g_pub_tx.tx_date);
	if (g_reply_int) 
	{
		sprintf(acErrMsg,"大额往账明细表数据库操作失败[%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	if (otxtotalcnt!=atol(mbfetran.otxtotalcnt) ||
		otxtotalamnt!=atof(mbfetran.otxtotalamnt)/100)
	{
		vtcp_log("往账总笔数或总金额不符");
		vtcp_log("CCPCcnt[%ld] LOCALcnt[%ld] CCPCamt[%.2f] LOCALamt[%.2f]",
					atol(mbfetran.otxtotalcnt),otxtotalcnt,
					atof(mbfetran.otxtotalamnt)/100,otxtotalamnt);

		g_reply_int=sql_execute("update mbfe_sys_ctl set par_value='1' where par_code='07'");
		if (g_reply_int)
		{
			sprintf(acErrMsg,"修改与前置机对账标志出错[%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
	}

	/*来账校对*/
	itxtotalcnt=sql_count("mbfeghvrecvlist","rectime[1,8]='%ld'",g_pub_tx.tx_date);
	if (itxtotalcnt<0) 
	{
		sprintf(acErrMsg,"大额来账明细表数据库操作失败[%d]",itxtotalcnt);
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int=sql_sum_double("mbfeghvrecvlist","amount",&itxtotalamnt,"rectime[1,8]='%ld'",g_pub_tx.tx_date);
	if (g_reply_int) 
	{
		sprintf(acErrMsg,"大额来账明细表数据库操作失败[%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	if (itxtotalcnt!=atol(mbfetran.itxtotalcnt) ||
		itxtotalamnt!=atof(mbfetran.itxtotalamnt)/100)
	{
		vtcp_log("来账总笔数或总金额不符");
		vtcp_log("CCPCcnt[%ld] LOCALcnt[%ld] CCPCamt[%.2f] LOCALamt[%.2f]",
					atol(mbfetran.itxtotalcnt),itxtotalcnt,
					atof(mbfetran.itxtotalamnt)/100,itxtotalamnt);

		g_reply_int=sql_execute("update mbfe_sys_ctl set par_value='1' where par_code='07'");
		if (g_reply_int)
		{
			sprintf(acErrMsg,"修改与前置机对账标志出错[%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
	}

	if (get_sys_par_val("07",checkflg)) goto ErrExit;
	if (!strcmp(checkflg,"0")) vtcp_log("今日对账已平!");

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

int register_hpreg()
{
	int otxsortnum,itxsortnum,immtrasortnum,i;
	char cnt[11],amnt[23];
	char *ptr;
	struct mbfehppaycnt_c mbfehppaycnt_v;

	memset(&mbfehppaycnt_v,0x00,sizeof(struct mbfehppaycnt_c));

	/*往账分类汇总信息*/
	otxsortnum=atoi(mbfetran.otxsortnum);
	ptr=mbfetran.otxsortinfo;
	for (i=0;i<otxsortnum;i++)
	{
		strncpy(cnt,ptr,10); cnt[10]=0; ptr+=10;
		strncpy(amnt,ptr,22); amnt[22]=0; ptr+=22;

		switch (i)
		{
			case 0:
				mbfehppaycnt_v.cmt100sendcnt=atol(cnt);
				mbfehppaycnt_v.cmt100sendamt=atof(amnt)/100;
				break;
			case 1:
				mbfehppaycnt_v.cmt101sendcnt=atol(cnt);
				mbfehppaycnt_v.cmt101sendamt=atof(amnt)/100;
				break;
			case 2:
				mbfehppaycnt_v.cmt102sendcnt=atol(cnt);
				mbfehppaycnt_v.cmt102sendamt=atof(amnt)/100;
				break;
			case 3:
				mbfehppaycnt_v.cmt103sendcnt=atol(cnt);
				mbfehppaycnt_v.cmt103sendamt=atof(amnt)/100;
				break;
			case 4:
				mbfehppaycnt_v.cmt109sendcnt=atol(cnt);
				mbfehppaycnt_v.cmt109sendamt=atof(amnt)/100;
				break;
			case 5:
				mbfehppaycnt_v.cmt121sendcnt=atol(cnt);
				mbfehppaycnt_v.cmt121sendamt=atof(amnt)/100;
				break;
			case 9:
				mbfehppaycnt_v.cmt105sendcnt=atol(cnt);
				mbfehppaycnt_v.cmt105sendamt=atof(amnt)/100;
				break;
			case 10:
				mbfehppaycnt_v.cmt108sendcnt=atol(cnt);
				mbfehppaycnt_v.cmt108sendamt=atof(amnt)/100;
				break;
			default:
				break;
		}
	}

	/*来账分类汇总信息*/
	itxsortnum=atoi(mbfetran.itxsortnum);
	ptr=mbfetran.itxsortinfo;
	for (i=0;i<itxsortnum;i++)
	{
		strncpy(cnt,ptr,10); cnt[10]=0; ptr+=10;
		strncpy(amnt,ptr,22); amnt[22]=0; ptr+=22;

		switch (i)
		{
			case 0:
				mbfehppaycnt_v.cmt100reccnt=atol(cnt);
				mbfehppaycnt_v.cmt100recamt=atof(amnt)/100;
				break;
			case 1:
				mbfehppaycnt_v.cmt101reccnt=atol(cnt);
				mbfehppaycnt_v.cmt101recamt=atof(amnt)/100;
				break;
			case 2:
				mbfehppaycnt_v.cmt102reccnt=atol(cnt);
				mbfehppaycnt_v.cmt102recamt=atof(amnt)/100;
				break;
			case 3:
				mbfehppaycnt_v.cmt103reccnt=atol(cnt);
				mbfehppaycnt_v.cmt103recamt=atof(amnt)/100;
				break;
			case 4:
				mbfehppaycnt_v.cmt109reccnt=atol(cnt);
				mbfehppaycnt_v.cmt109recamt=atof(amnt)/100;
				break;
			case 6:
				mbfehppaycnt_v.cmt122reccnt=atol(cnt);
				mbfehppaycnt_v.cmt122recamt=atof(amnt)/100;
				break;
			case 7:
				mbfehppaycnt_v.cmt123reccnt=atol(cnt);
				mbfehppaycnt_v.cmt123recamt=atof(amnt)/100;
				break;
			case 8:
				mbfehppaycnt_v.cmt124reccnt=atol(cnt);
				mbfehppaycnt_v.cmt124recamt=atof(amnt)/100;
				break;
			case 9:
				mbfehppaycnt_v.cmt105reccnt=atol(cnt);
				mbfehppaycnt_v.cmt105recamt=atof(amnt)/100;
				break;
			case 10:
				mbfehppaycnt_v.cmt108reccnt=atol(cnt);
				mbfehppaycnt_v.cmt108recamt=atof(amnt)/100;
				break;
			default:
				break;
		}
	}

	/*即时转账分类汇总信息*/
	immtrasortnum=atoi(mbfetran.immtrasortnum);
	ptr=mbfetran.immtrasortinfo;
	for (i=0;i<immtrasortnum;i++)
	{
		strncpy(cnt,ptr,10); cnt[10]=0; ptr+=10;
		strncpy(amnt,ptr,22); amnt[22]=0; ptr+=22;

		switch (i)
		{
			case 0:
				mbfehppaycnt_v.cmt232lendcnt=atol(cnt);
				mbfehppaycnt_v.cmt232lendamt=atof(amnt)/100;
				break;
			case 1:
				mbfehppaycnt_v.cmt407lendcnt=atol(cnt);
				mbfehppaycnt_v.cmt407lendamt=atof(amnt)/100;
				break;
			case 2:
				mbfehppaycnt_v.cmt408lendcnt=atol(cnt);
				mbfehppaycnt_v.cmt408lendamt=atof(amnt)/100;
				break;
			case 3:
				mbfehppaycnt_v.cmt232loancnt=atol(cnt);
				mbfehppaycnt_v.cmt232loanamt=atof(amnt)/100;
				break;
			case 4:
				mbfehppaycnt_v.cmt407loancnt=atol(cnt);
				mbfehppaycnt_v.cmt407loanamt=atof(amnt)/100;
				break;
			case 5:
				mbfehppaycnt_v.cmt408loancnt=atol(cnt);
				mbfehppaycnt_v.cmt408loanamt=atof(amnt)/100;
				break;
			default:
				break;
		}
	}

	mbfehppaycnt_v.consigndate=atol(mbfetran.consigndate);
	g_reply_int=Mbfehppaycnt_Ins(mbfehppaycnt_v,g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"登记大额日汇总统计表数据库操作失败[%d]",g_reply_int);
		WRITEMSG
		return -1;
	}

	return 0;
}

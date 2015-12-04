/*************************************************************
* 文 件 名: gDX07.c
* 功能描述：结息日次日21日对欠息户发送催息短信(不含按揭贷款)
*
* 作    者: zgf
* 完成日期: 2013年9月11日
*
* 修改记录:
* 日    期:
* 修 改 人:
* 修改内容:

insert into com_eod_exec values(9004,'7777','gDX07','贷款结息次日催款提醒','1111111111','4','1','21','21','2','Y','Y','N','2','Y');
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "mob_acct_type_c.h"
#include "ln_mst_c.h"
#include "ln_reg_c.h"

gDX07()
{ 
	int count;
	char tmpacno[20];
	char pact_no_tmp[20];
	double ljqx,dqlo,dqyh,dqqx;
	struct	mob_acct_type_c sMob_acct_type;
	struct ln_mst_c sLn_mst;
	struct ln_reg_c	sLn_reg;
	memset(pact_no_tmp,0x00,sizeof(pact_no_tmp));	
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(&sLn_mst,0x00,sizeof(sLn_mst));
	memset(&sLn_reg,0x00,sizeof(sLn_reg));
	memset(tmpacno,0x00,sizeof(tmpacno));
	
	g_reply_int = sql_begin();	/* 打开事务 */
	if (g_reply_int != 0) 
	{
		sprintf(acErrMsg, "打开事务失败!!!");
		WRITEMSG
			goto ErrExit;
	}
	/* 初始化全局变量 */
	pub_base_sysinit();

	strcpy(g_pub_tx.tx_code,"7777");
	/*g_reply_int=Mob_acct_type_Dec_Sel1(g_pub_tx.reply," ac_type in('3') and mob_sts='1' ");*/
	g_reply_int=Mob_acct_type_Dec_Sel1(g_pub_tx.reply,"  mob_sts='1' and filler='1' order by ac_no ");/*mod by lwb  20150408  不按合同号了   按还款账号查*/
	if(g_reply_int)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		g_reply_int=Mob_acct_type_Fet_Sel1(&sMob_acct_type,g_pub_tx.reply);
		if(g_reply_int==100)
		{
			break;
		}
		else if(g_reply_int)
		{
			sprintf(acErrMsg, "FETCH ERROR [%d]", g_reply_int);
			WRITEMSG
			Mob_acct_type_Clo_Sel1();
			goto ErrExit;
		}
		/***批量处理时，对于同一笔合同只循环一次***/
		else
		{
			if(memcmp(sMob_acct_type.ac_no,tmpacno,strlen(sMob_acct_type.ac_no))==0)
			{				
				continue;
			} 
		}
		
		memset(tmpacno,0x00,sizeof(tmpacno));
		memcpy(tmpacno,sMob_acct_type.ac_no,strlen(sMob_acct_type.ac_no));
		
		/*if(sMob_acct_type.ac_type[0]=='3')/**普通贷款**/
		{
			g_reply_int=Ln_mst_Dec_Sel2(g_pub_tx.reply,"repay_ac_id=%ld and ac_sts<>'*' and in_lo_intst+out_lo_intst > 0 and intst_type in('3') ",sMob_acct_type.ac_id);
		}
		if(g_reply_int)
		{
			sprintf(acErrMsg, "[%s][%d]DECLARE FOR SELECT ERROR [%d]",__FILE__,__LINE__, g_reply_int);
			WRITEMSG
			Mob_acct_type_Clo_Sel1();
			goto ErrExit;
		}
		while(1)
		{
			memset(&sLn_mst,0,sizeof(sLn_mst));			
			g_reply_int=Ln_mst_Fet_Sel2(&sLn_mst,g_pub_tx.reply);
			if(g_reply_int==100)
				break;
			else if(g_reply_int)
			{
				sprintf(acErrMsg, "[%s][%d]DECLARE FOR FETCH ERROR [%d]",__FILE__,__LINE__, g_reply_int);
				WRITEMSG
				Ln_mst_Clo_Sel2();	
				Mob_acct_type_Clo_Sel1();
				goto ErrExit;
			}							
			g_reply_int=Ln_reg_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and type not in ('1','5') and sts = '0' order by reg_date_beg",sLn_mst.ac_id,sLn_mst.ac_seqn);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d]贷款登记簿查询错误%d",__FILE__,__LINE__,g_reply_int);
				strcpy(g_pub_tx.reply,"L234");
				Ln_mst_Clo_Sel2();	
				Mob_acct_type_Clo_Sel1();
				goto ErrExit;
			}
			
			ljqx = 0.0;		/*累积欠息*/
			dqlo = 0.0;		/*最后一笔欠息*/
			dqyh = 0.0;		/*已还利息*/
			dqqx = 0.0;		/*当期欠息*/
			count = 0;
			
			while(1) {
				memset(&sLn_reg,0x00,sizeof(sLn_reg));	
				count ++;				
				g_reply_int=Ln_reg_Fet_Sel(&sLn_reg,g_pub_tx.reply);
				if(g_reply_int==100)
					break;
				else if(g_reply_int)
				{
					sprintf(acErrMsg, "[%s][%d]DECLARE FOR FETCH ERROR [%d]",__FILE__,__LINE__, g_reply_int);
					WRITEMSG
					Ln_reg_Clo_Sel();
					Ln_mst_Clo_Sel2();	
					Mob_acct_type_Clo_Sel1();
					goto ErrExit;
				}				
				ljqx += (sLn_reg.lo_bal-sLn_reg.pay_bal);
				dqlo = sLn_reg.lo_bal;
				dqyh = sLn_reg.pay_bal;
			}
			Ln_reg_Clo_Sel();
			
			if(count == 1) {
				ljqx = sLn_reg.lo_bal;
				dqyh = sLn_reg.pay_bal;
			}
			else if(count > 1)
				ljqx = ljqx + dqyh;
			else
				continue;
					
			dqqx = ljqx - dqyh;
			strncpy(pact_no_tmp,sLn_mst.pact_no,strlen(sLn_mst.pact_no)-3);	
			pub_mob_sendmail_ln("贷款结息","7777",pact_no_tmp,"1",ljqx,dqyh,dqqx);	
			
		}
		Ln_mst_Clo_Sel2();	
	}
	Mob_acct_type_Clo_Sel1();
	
OkExit:
	sql_commit();	/*	--提交事务--*/
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "结息日次日对欠息户发送催息短信成功!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();		/*--事务回滚--*/
	strcpy(g_pub_tx.reply,"1111");
	sprintf(acErrMsg, "结息日次日对欠息户发送催息短信失败!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 1;
}

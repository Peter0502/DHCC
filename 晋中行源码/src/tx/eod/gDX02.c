/*************************************************************
* 文 件 名: gDX02.c
* 功能描述：贷款结息短信通知
						结息前7天通知 
*
* 作    者: houyi
* 完成日期: 2010年6月3日
*
* 修改记录:
* 日    期:
* 修 改 人:
* 修改内容:
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
#include "ln_pay_pln_c.h"
#include "ln_lo_c.h"
#include "com_sys_parm_c.h"
long get_next_inst_date(long date,char * intst_type);
extern char * pub_base_daynumLAT(char *date, int num);
gDX02()
{
	double	vbjlx,vbnlx,vbwlx,vfllx;
	double	dLx1,dLx2,dBj;
	double	dLx11,dLx21,dBj1;
	long		lNextdate;
	char		cTxdate[9];
	char		*cTp=NULL;
	long	jx_date=0;
	struct	mob_acct_type_c sMob_acct_type;
	struct ln_pay_pln_c    sPln;
	struct ln_lo_c           sLn_lo;
	struct com_sys_parm_c sCom_sys_parm;
	int			mail_flag;
	int count;
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(&sCom_sys_parm,0x00,sizeof(sCom_sys_parm));
	memset(cTxdate,0x00,sizeof(cTxdate));
	
	g_reply_int = sql_begin();	/* 打开事务 */
	if (g_reply_int != 0) 
	{
		sprintf(acErrMsg, "打开事务失败!!!");
		WRITEMSG
			goto ErrExit;
	}
	/* 初始化全局变量 */
	pub_base_sysinit();
	g_reply_int=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
	if(g_reply_int)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(cTxdate,"%ld",sCom_sys_parm.sys_date);
	cTp=pub_base_daynumLAT(cTxdate,7);
	jx_date=atol(cTp);
	/**
	sscanf(cTp,"%ld",&(g_pub_tx.tx_date));
	**/
	g_pub_tx.tx_date=jx_date;
vtcp_log("[%s][%d] g_pub_tx.tx_date=%ld",__FILE__,__LINE__,g_pub_tx.tx_date);

	strcpy(g_pub_tx.tx_code,"7771");
	g_reply_int=Mob_acct_type_Dec_Sel1(g_pub_tx.reply,"ac_type in('3','A') and mob_sts='1' ");	
	if(g_reply_int)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", g_reply_int);
		WRITEMSG
			goto ErrExit;
	}
	while(1)
	{
		mail_flag=1;
		vbjlx=vbnlx=vbwlx=vfllx=0.0;
		dLx1=dLx2=dBj=0.0;
		dLx11=dLx21=dBj1=0.0;
		g_reply_int=Mob_acct_type_Fet_Sel1(&sMob_acct_type,g_pub_tx.reply);
		if(g_reply_int==100)
		{
			break;
		}
		else if(g_reply_int)
		{
			sprintf(acErrMsg, "FETCH ERROR [%d]", g_reply_int);
			WRITEMSG
				goto ErrExit;
		}
		if(sMob_acct_type.ac_type[0]=='3')/**普通贷款和按揭贷款**/
		{
			g_reply_int=Ln_mst_Dec_Sel2(g_pub_tx.reply,"pact_no='%s' and ac_sts<>'*' and bal+in_lo_intst+out_lo_intst+Cmpd_lo_intst > 0 and intst_type in('3','4','6') ",sMob_acct_type.filler);
		}
		else/**授信贷款**/
		{
			g_reply_int=Ln_mst_Dec_Sel2(g_pub_tx.reply,"contract_no='%s' and ac_sts<>'*' and bal+in_lo_intst+out_lo_intst+Cmpd_lo_intst > 0 and intst_type in('3','4','6') and ln_type='A' ",sMob_acct_type.ac_no);
		}
		if(g_reply_int)
		{
			sprintf(acErrMsg, "[%s][%d]DECLARE FOR SELECT ERROR [%d]",__FILE__,__LINE__, g_reply_int);
			WRITEMSG
				goto ErrExit;
		}
		count = 0;
		while(1)
		{
			memset(&g_ln_mst,0,sizeof(g_ln_mst));
			vbjlx=0.00;
			vbnlx=0.00;
			vbwlx=0.00;
			vfllx=0.00;
			dLx1=0.00;
			dLx2=0.00;
			dBj=0.00;
			
			count ++;
			g_reply_int=Ln_mst_Fet_Sel2(&g_ln_mst,g_pub_tx.reply);
			if(g_reply_int==100)
			{
				/*---added by sunxw 20111111---*/
				if(count <= 1)
				/*-----added end----*/
					mail_flag=0;
				break;
			}
			else if(g_reply_int)
			{
				sprintf(acErrMsg, "[%s][%d]DECLARE FOR FETCH ERROR [%d]",__FILE__,__LINE__, g_reply_int);
				WRITEMSG
					goto ErrExit;
			}
			/**********下次结息日期是否本次结息***********
			lNextdate=get_next_inst_date(sCom_sys_parm.sys_date,g_ln_mst.intst_type);
			if(lNextdate<0)
			{
				vtcp_log("[%s][%d]计算下次结息日期错误:%ld",__FILE__,__LINE__,lNextdate);
				goto ErrExit;
			}
			if(lNextdate != g_pub_tx.tx_date)
			{
				vtcp_log("Not in the range!![%s][%d]intst_date=%ld,nextdate=%ld",__FILE__,__LINE__,g_pub_tx.tx_date,lNextdate);
				continue;
			}***/
			if(g_ln_mst.intst_type[0]=='3' || g_ln_mst.intst_type[0]=='6')
			{
			}
			else if(g_ln_mst.intst_type[0]=='4')
			{
				if(g_pub_tx.tx_date/100%100%3!=0)
				{
					mail_flag=0;
					continue;
				}
			}
			g_reply_int = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm , " prdt_no='%s' " ,g_ln_mst.prdt_no );
			if( g_reply_int==100 )
			{
					sprintf(acErrMsg,"贷款参数文件中无[%s]产品",g_ln_mst.prdt_no);
					WRITEMSG
					strcpy( g_pub_tx.reply ,"L001" );
					goto ErrExit;
			}
			else if( g_reply_int )
			{
	   			sprintf(acErrMsg,"取贷款参数文件异常 [%s][%d]",g_ln_mst.prdt_no,g_reply_int);
					WRITEMSG
					strcpy( g_pub_tx.reply ,"L001" );
	   			goto ErrExit;
   			}
			if(g_ln_mst.repay_type[0]>='3')/**按揭贷款**/
			{
				memset(&sPln,0x00,sizeof(sPln));
				memset(&sLn_lo,0x00,sizeof(sLn_lo));
				g_reply_int=Ln_pay_pln_Sel(g_pub_tx.reply,&sPln,"ac_id=%ld and ac_seqn=%d",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
				if(g_reply_int)
				{
					vtcp_log("[%s][%d]按揭还款计划表查询错误%d",__FILE__,__LINE__,g_reply_int);
					strcpy(g_pub_tx.reply,"L234");
					goto ErrExit;
				}
				dLx1+=sPln.curr_intst;
				dBj+=sPln.curr_amt;
				g_reply_int=Ln_lo_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and pay_type='0' ",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
				if(g_reply_int)
				{
					vtcp_log("[%s][%d]按揭欠款表查询游标定义错误%d",__FILE__,__LINE__,g_reply_int);
					strcpy(g_pub_tx.reply,"D101");
					goto ErrExit;
				}
				while(1)
				{
					memset(&sLn_lo,0x00,sizeof(sLn_lo));
					g_reply_int=Ln_lo_Fet_Sel(&sLn_lo,g_pub_tx.reply);
					if(g_reply_int==100)
					{
						break;
					}
					else if(g_reply_int)
					{
						vtcp_log("[%s][%d]按揭欠款表查询游标打开错误%d",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D102");
						Ln_lo_Clo_Sel();
						goto ErrExit;
					}
					/**计算罚息**/
					/*---------------modified by sunxw 20110511--------------*/
					/*g_reply_int= pub_ln_CalPunIntst(g_ln_mst, g_ln_parm,sLn_lo.lo_amt, sLn_lo.lo_intst,sLn_lo.shld_pay_date, g_pub_tx.tx_date, &vbnlx, &vbwlx);*/
					g_reply_int= pub_ln_CalPunIntst(g_ln_mst, g_ln_parm,sLn_lo.lo_amt-sLn_lo.pay_lo_amt, sLn_lo.lo_intst-sLn_lo.pay_lo_intst,sLn_lo.shld_pay_date, g_pub_tx.tx_date, &vbnlx, &vbwlx);
					if(g_reply_int)
					{
						vtcp_log("[%s][%d]罚息计算错误%d",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"L213");
						Ln_lo_Clo_Sel();
						goto ErrExit;
					}
					dLx1+=vbnlx+vbwlx;	/*dLx1=当期应还利息+罚息+复利*/
					dLx2+=sLn_lo.lo_intst-sLn_lo.pay_lo_intst;	/* dLx2=累计欠息,不含当期*/
					dBj+=sLn_lo.lo_amt-sLn_lo.pay_lo_amt;	/* dBj=当期应还本金+累计欠本金*/
				}
				Ln_lo_Clo_Sel();
			}
			else
			{
				/**********本金***********/
				g_reply_int = pub_ln_CalIntst(g_ln_mst, g_ln_parm, "0", &vbjlx, 1);/**本金利息**/
				if(g_reply_int)
					goto ErrExit;
				/*********************/
				if( g_ln_mst.cmpd_intst_ind[0]=='N' || g_ln_parm.cmpd_intst_ind[0]=='N')
				{
					vbnlx=0.00;
					vbwlx=0.00;
					vfllx=0.00;
				}
				else
				{
					/***********表内*********/
						g_reply_int = pub_ln_CalIntst(g_ln_mst, g_ln_parm, "1", &vbnlx, 1);/**表内利息**/
						if(g_reply_int)
							goto ErrExit;
					/***********表外*********/
						g_reply_int = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"2",&vbwlx,1 );/**表外利息**/
						if( g_reply_int ) goto ErrExit;
					/**********复利***********/
						g_reply_int = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"3",&vfllx,1 );/**复利利息**/
						if( g_reply_int ) goto ErrExit;
				}
				if(sMob_acct_type.ac_type[0]=='A')
				{
					dLx11+=vbjlx+vbnlx+vbwlx+vfllx;
					dLx21+=g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst;
					dBj1+=g_ln_mst.bal;
				}
				else
				{
				  dLx1=vbjlx+vbnlx+vbwlx+vfllx;
				  dLx2=g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst;
				  dBj=g_ln_mst.bal;
			  	}
			}
			
			if(sMob_acct_type.ac_type[0]=='3')
				pub_mob_sendmail_ln("贷款结息","7771",sMob_acct_type.filler,"1",dLx1,dBj,dLx2);
		}
		Ln_mst_Clo_Sel2();
      		
		vtcp_log("[%s][%d]mail_flag=[%d]",__FILE__,__LINE__,mail_flag);
      		if(sMob_acct_type.ac_type[0]=='A'&&mail_flag) {
			/**pub_mob_sendmail_ln("贷款结息","7771",sMob_acct_type.ac_no,"1",dLx11,dBj1,dLx2);		houyi 20100815银川授信贷款发错信息**/	
			pub_mob_sendmail_ln("贷款结息","7771",sMob_acct_type.filler,"1",dLx11,dBj1,dLx21);			
		}
	}
	Mob_acct_type_Clo_Sel1();
OkExit:
	g_pub_tx.tx_date=sCom_sys_parm.sys_date;
	sql_commit();	/*	--提交事务--*/
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "贷款结息提前7天通知成功!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 0;
ErrExit:
	Ln_mst_Clo_Sel2();
	Mob_acct_type_Clo_Sel1();
	g_pub_tx.tx_date=sCom_sys_parm.sys_date;
	sql_rollback();		/*--事务回滚--*/
	strcpy(g_pub_tx.reply,"1111");
	sprintf(acErrMsg, "贷款结息提前7天通知失败!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 1;
}
/**仅返回按月计息和按季计息的下次计息日期**/
long get_next_inst_date(long date,char * intst_type)
{
	int		iStep;
	long	year,mon,day,newdate;
	year=date/10000;
	mon=(date/100)%100;
	day=date%100;
	if( (intst_type[0]=='3') || (intst_type[0]=='6') )/**按月计息或按揭**/
	{
		if(day>21) iStep=1;
		else iStep=0;
	}
	else if(intst_type[0]=='4')/**按季计息**/
	{
		if((day>21)&&(mon%3==0))/**3，6，9，12月结息之后**/
		{
			iStep=3;
		}
		else if(mon%3==0)/**3，6，9，12月结息之前**/
		{
			iStep=0;
		}
		else/**其他日期**/
		{
			iStep=0;
			mon=(mon/3+1)*3;
		}
	}
	else return -1;
	day=21;
	mon=mon+iStep;
	if(mon>12)
	{
		mon-=12;
		year+=1;
	}
	newdate=year*10000+mon*100+day;
	vtcp_log("[%s][%d]下次结息日%ld",__FILE__,__LINE__,newdate);
	return newdate;
}

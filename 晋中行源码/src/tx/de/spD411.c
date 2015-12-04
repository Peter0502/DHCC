/***************************************************************
* 文 件 名: spD411.c
* 功能描述：通知登记/撤销/变更交易
* 作    者: mike
* 完成日期：2004年1月15日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"

spD411()
{
	char ac_no[20];
	char tmp_ind1[2],tmp_ind2[2],flag[2];
	char tmp_name[51],tmp_phone[20],tmp_id_knd[2],tmp_id_no[21];
	long tmp_date1,tmp_date2,deadline;
	double tmp_amt,tmp_bal;

	struct mo_infrm_c	g_mo_infrm;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_mo_infrm,0x00,sizeof(struct mo_infrm_c));

	g_reply_int=0;
	flag[0]='0';

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

        get_zd_data( "0670", tmp_ind1 );                /* 操作类型标志 */
	get_zd_data( "0310", g_pub_tx.ac_no );          /* 帐号 */
	get_zd_int( "0340",  &g_pub_tx.ac_seqn );       /* 账户序号 */
	get_zd_long( "0440", &tmp_date1 );              /* 支取日期 */
	get_zd_double( "0410", &g_pub_tx.tx_amt1 );     /* 支取金额 */
	get_zd_long( "0450", &tmp_date2 );              /* 变更后支取日期 */
	get_zd_double( "0420", &tmp_amt );              /* 变更后支取金额 */
	get_zd_double( "0430", &tmp_bal );              /* 帐户余额 */
	get_zd_data( "0690", tmp_ind2 );                /* 违约标志 */
	get_zd_data( "0250", tmp_name );         		/* 账户名称 */
	get_zd_data( "0610", tmp_phone );        		/* 电话号码 */
	get_zd_data( "0680", tmp_id_knd );       		/* 证件类型 */
	tmp_id_knd[1]='\0';
	get_zd_data( "0620", tmp_id_no );        		/* 证件号码 */
	tmp_id_no[20]='\0';

	vtcp_log(" tmp_date1 firs2 is [%ld],tmp_id_no is [%s]",tmp_date1,tmp_id_no);
	g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
								 g_pub_tx.ac_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"介质账户对照表中不存在该记录");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf(acErrMsg,"根据显示帐号查询介质账户对照表失败!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P147");
		goto ErrExit;
	}

	g_reply_int = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld", \
							 g_mdm_ac_rel.ac_id);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"帐号不存在 !");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	vtcp_log("------------------------prdt_no==[%s]",g_td_mst.prdt_no);

	g_reply_int = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
							  g_td_mst.prdt_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"产品不存在!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O215");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** 非通知存款户给出提示 ***/
	if( g_td_parm.td_type[0]!='4' )
	{
		sprintf(acErrMsg,"该帐户非通知存款户，请重新输入！");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O204");
		goto ErrExit;
	}

	switch( tmp_ind1[0] )
	{
		case '1':												/*1-登记*/
				if( pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00)<=0 )
				{
					sprintf(acErrMsg,"登记金额不正确，请重新输入!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D207");
					goto ErrExit;
				}

				if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.bal)>0 )
				{
					sprintf(acErrMsg,"登记金额不得大于账户余额，请重新输入!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D202");
					goto ErrExit;
				}

				/*** 一天、七天通知存款检验登记日期 ***/
				if( !strncmp(g_td_mst.prdt_no,"242",3) || \
					!strncmp(g_td_mst.prdt_no,"281",3) )
				{
					g_reply_int = pub_base_deadlineD( g_pub_tx.tx_date,1,&deadline );
					if( g_reply_int )
					{
						sprintf(acErrMsg,"计算日期函数出错");
						WRITEMSG
						goto ErrExit;
					}
					vtcp_log(" tmp_date1 is [%ld],deadline [%ld]",tmp_date1,deadline);
					if( tmp_date1<deadline || tmp_date1>deadline )
					{
						sprintf(acErrMsg,"登记日期超过规定日期限制");
						WRITEMSG
						strcpy(g_pub_tx.reply,"D205");
						goto ErrExit;
					}
				}
				else if( !strncmp(g_td_mst.prdt_no,"243",3) || \
			 			 !strncmp(g_td_mst.prdt_no,"282",3) )
				{
					g_reply_int = pub_base_deadlineD( g_pub_tx.tx_date,7,&deadline );
					if( g_reply_int )
					{
						sprintf(acErrMsg,"计算日期函数出错");
						WRITEMSG
						goto ErrExit;
					}

					if( tmp_date1<deadline || tmp_date1>deadline )
					{
						sprintf(acErrMsg,"登记日期超过规定日期限制");
						WRITEMSG
						strcpy(g_pub_tx.reply,"D205");
						goto ErrExit;
					}
				}

				if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.bal)>0 )
				{
					sprintf(acErrMsg,"登记金额大于账户余额，请重新输入!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D202");
					goto ErrExit;
				}

				/*** 储蓄户登记取款金额不得小于5万元,对公户不得低于10万元 ***/
				if( g_td_parm.cif_type[0]=='1' )
				{
					if( pub_base_CompDblVal(g_td_mst.bal-g_pub_tx.tx_amt1,50000.00)<0 
						&& pub_base_CompDblVal(g_td_mst.bal-g_pub_tx.tx_amt1,0.00)!=0 )
					{
						sprintf(acErrMsg,"留存金额不得小于5万元!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"O222");
						goto ErrExit;
					}
				}
				else 
				{
					if( pub_base_CompDblVal(g_td_mst.bal-g_pub_tx.tx_amt1,500000.00)<0
						&& pub_base_CompDblVal(g_td_mst.bal-g_pub_tx.tx_amt1,0.00)!=0 )
					{
						sprintf(acErrMsg,"留存金额不得小于50万元!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"O223");
						goto ErrExit;
					}
				}

				g_mo_infrm.ac_id = g_mdm_ac_rel.ac_id;
				g_mo_infrm.ac_seqn = g_pub_tx.ac_seqn;
				g_mo_infrm.infrm_date = g_pub_tx.tx_date;
				g_mo_infrm.prm_draw_date = tmp_date1;
				g_mo_infrm.prm_draw_amt = g_pub_tx.tx_amt1;
				g_mo_infrm.fct_wd_amt = 0.00;
				g_mo_infrm.occ_date = 0;
				strcpy (g_mo_infrm.sts,"0");
				strcpy (g_mo_infrm.name,tmp_name);
				strcpy (g_mo_infrm.phone,tmp_phone);
				strcpy (g_mo_infrm.id_knd,tmp_id_knd);
				strcpy (g_mo_infrm.id_no,tmp_id_no);
				g_mo_infrm.wrk_date = g_pub_tx.tx_date;
				g_mo_infrm.trace_no = g_pub_tx.trace_no;

				/*** 检查该账户是否已登记未支取 ***/
				g_reply_int = Mo_infrm_Sel(g_pub_tx.ac_no,&g_mo_infrm,
								"ac_id=%ld and ac_seqn=%d and sts='0'",
								g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( g_reply_int==0 )
				{
				  	sprintf(acErrMsg,"该帐户存在通知未取记录，不允许登记!");
				  	WRITEMSG
				  	strcpy (g_pub_tx.reply,"D203");
				 	goto ErrExit;
				}
				else if( g_reply_int==100 )
				{
					/*登记通知登记薄*/
					g_reply_int = Mo_infrm_Ins(g_mo_infrm,g_pub_tx.reply);
					if ( g_reply_int )
					{
						sprintf(acErrMsg,"登记通知登记薄错误[%d]",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D107" );
						goto ErrExit;
					}
				}
				else 
				{
					sprintf(acErrMsg,"查询通知登记薄异常!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"W056");
					goto ErrExit;
				}
			 	break;
		case '2':									/*2-撤销*/
				if( g_pub_tx.tx_date!=g_mo_infrm.infrm_date )
				{
					g_reply_int = Td_mst_Dec_Upd(g_pub_tx.reply, \
										"ac_id=%ld and ac_seqn=%d", \
										g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn );
					if( g_reply_int )
					{
						sprintf(acErrMsg,"定义定期文件游标错[%d]",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D101" );
						goto ErrExit;
					}

					g_reply_int = Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
					if( g_reply_int==100 )
					{
						sprintf(acErrMsg,"定期文件中无此记录[%d]",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D104" );
						goto ErrExit;
					}
					else if( g_reply_int )
					{
						sprintf(acErrMsg,"取定期文件出错[%d]",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D103" );
						goto ErrExit;
					}

					g_td_mst.ttl_cnt++;				/*违约次数加一*/

					g_reply_int=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
    				if ( g_reply_int )
					{
						sprintf(acErrMsg,"UPDATE td_mst error %d",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D105" );
						goto ErrExit;	
					}

					Td_mst_Clo_Upd( );
				}	

				/*修改通知登记薄*/
				g_reply_int=Mo_infrm_Dec_Upd(g_pub_tx.reply,
							"ac_id=%ld and ac_seqn=%d and prm_draw_date=%ld \
							and prm_draw_amt=%lf",g_mdm_ac_rel.ac_id, \
							g_pub_tx.ac_seqn,tmp_date1,g_pub_tx.tx_amt1 );
				if(g_reply_int)
				{
					sprintf(acErrMsg,"定义通知登记薄游标错[%d]",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D101" );
					goto ErrExit;
				}

				g_reply_int=Mo_infrm_Fet_Upd(&g_mo_infrm,g_pub_tx.reply);
				if( g_reply_int==100 )
				{
					sprintf(acErrMsg,"通知登记薄不存在该记录[%d]",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D104" );
					goto ErrExit;
				}
				else if( g_reply_int )
				{
					sprintf(acErrMsg,"取通知登记薄游标出错[%d]",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D103" );
					goto ErrExit;
				}

				strncpy(g_mo_infrm.sts,"4",1);
				g_mo_infrm.wrk_date = g_pub_tx.tx_date;
				g_mo_infrm.trace_no = g_pub_tx.trace_no;

				g_reply_int = Mo_infrm_Upd_Upd(g_mo_infrm,g_pub_tx.reply);
    			if( g_reply_int )
				{
					sprintf(acErrMsg,"UPDATE Mo_Infrm error %d",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D105" );
					goto ErrExit;	
				}

				Mo_infrm_Clo_Upd( );
				break;
		case '3':									/*3-变更*/
				g_reply_int = Mo_infrm_Dec_Upd(g_pub_tx.reply,
							"ac_id=%ld and ac_seqn=%d and prm_draw_date=%ld \
							 and prm_draw_amt=%lf",g_mdm_ac_rel.ac_id,
							 g_pub_tx.ac_seqn,tmp_date1,g_pub_tx.tx_amt1);
				if( g_reply_int )
				{
					sprintf(acErrMsg,"定义更改主文件游标出错[%d]",
							g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D101" );
					goto ErrExit;
				}

				g_reply_int = Mo_infrm_Fet_Upd(&g_mo_infrm,g_pub_tx.reply);
				if( g_reply_int==100 )
				{
					sprintf(acErrMsg,"通知登记薄不存在该记录[%d]",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W057" );
					goto ErrExit;
				}
				else if( g_reply_int )
				{
					sprintf(acErrMsg,"取通知登记薄游标出错[%d]",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D103" );
					goto ErrExit;
				}

				if(g_pub_tx.tx_date!=g_mo_infrm.infrm_date && (g_mo_infrm.prm_draw_date!=tmp_date2 || g_mo_infrm.prm_draw_amt!=tmp_amt))
				{
					strncpy(flag,"1",1);				/*违约标志*/
				}

				g_mo_infrm.prm_draw_date = tmp_date2;	/*更改后约定支取日期*/
				g_mo_infrm.prm_draw_amt = tmp_amt;		/*更改后约定支取金额*/
				g_mo_infrm.fct_wd_amt = 0.00;			/*更改后实际发生金额*/
				g_mo_infrm.occ_date = 0;				/*更改后实际发生日期*/
				strcpy (g_mo_infrm.name,tmp_name);		/*更改后姓名*/	
				strcpy (g_mo_infrm.phone,tmp_phone);	/*更改后电话号码*/	
				strcpy (g_mo_infrm.id_knd,tmp_id_knd);	/*更改后证件类型*/
				strcpy (g_mo_infrm.id_no,tmp_id_no);	/*更改后证件号码*/
				g_mo_infrm.wrk_date = g_pub_tx.tx_date;
				g_mo_infrm.trace_no = g_pub_tx.trace_no;

				g_reply_int=Mo_infrm_Upd_Upd(g_mo_infrm,g_pub_tx.reply);
    			if ( g_reply_int )
				{
					sprintf(acErrMsg,"UPDATE mo_infrm error %d",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D105" );
					goto ErrExit;	
				}

				Mo_infrm_Clo_Upd( );

				/*是否违约*/
				if(flag[0]=='1')
				{
					sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
					WRITEMSG

					g_reply_int=Td_mst_Dec_Upd(g_pub_tx.reply,
								"ac_id=%ld and ac_seqn=%d",
								g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn );
					if(g_reply_int)
					{
						sprintf(acErrMsg,"定义更改主文件游标出错[%d]",
								g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D101" );
						goto ErrExit;
					}

					g_reply_int = Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
					if( g_reply_int==100 )
					{
						sprintf(acErrMsg,"定期文件中无此记录[%d]",
								g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D103" );
						goto ErrExit;
					}
					else if(g_reply_int)
					{
						sprintf(acErrMsg,"打开更改主文件游标出错[%d]",
								g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D103" );
						goto ErrExit;
					}

					g_td_mst.ttl_cnt++;				/*违约次数加一*/

					g_reply_int = Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
    				if ( g_reply_int )
					{
						sprintf(acErrMsg,"UPDATE td_mst error %d",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D105" );
						goto ErrExit;	
					}

					Td_mst_Clo_Upd( );
				}	
				break;
		default :
				 sprintf(acErrMsg,"操作类型选择异常!!");
				 WRITEMSG
				 strcpy(g_pub_tx.reply,"W010");
				 goto ErrExit;
	}

    /*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"操作通知登记薄成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"操作通知登记薄失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

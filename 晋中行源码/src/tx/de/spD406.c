/***************************************************************
* 文 件 名: spD406.c
* 功能描述：冻结解冻交易
* 作    者: mike
* 完成日期：2004年1月18日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*update tx_def set bit_map = '10000000000000000001000000000000000000011000000010000000000000000000000000000000000000000000000000000000000000000000000000000000' where tx_code = '2406' *****增加41域值，用于返回账户最终的余额 zgf 20140220**
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_hold_c.h"

spD406()
{
	/*变量定义*/
	FILE *fp;
	int	 i=0;  
	int  cnt=0; 
	int	 ret=0;
	int  icnt=0;
	int	 unlock_flag=0;						/***是否解除以前不符冻结标志***/
	char tmp_ind1[2];						/***1-冻结、解冻类型***/
	char tmp_ind2[2];						/**冻结方式 1-完全 2-只进 3-部分**/
	char tmp_ind3[2];						/***是否计息***/
	char tmp_ind4[2];						/***自动解冻标志 Y/N***/
	char tmp_ind5[2];						/***解冻方式***/
	char tmp_ind6[2];						/***是否计息 Y/N***/
	char tmp_memo1[100];					/***冻结备注***/
	char tmp_memo2[100];	
	long tmp_date;							/***到期日期***/
	int  hold_seqn;							/***冻结序号***/		
	double ac_bal;	
        double  hold_amt;						/***帐户余额***/	
   
  double bal_tmp = 0.00;     
   	
	long beg_hold_date;
	char tmpname[100];
	char min_ht[2];             /***冻结权限 martin***/
	double  sum_amt=0.00;						/***冻结累计 martin***/		
	struct mo_hold_c g_mo_hold;
	struct mo_hold_c f_mo_hold;
	struct dd_mst_c  f_dd_mst;
	struct td_mst_c  f_td_mst;
	
	struct dd_mst_c  dd_mst_tmp;
	struct td_mst_c  td_mst_tmp;

	/*清空结构*/
	g_reply_int=0;
	memset(&g_mo_hold,0x00,sizeof(struct mo_hold_c));
	memset(&f_mo_hold,0x00,sizeof(struct mo_hold_c));
	memset(&f_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&f_td_mst,0x00,sizeof(struct td_mst_c));
	
	memset(&dd_mst_tmp,0x00,sizeof(struct dd_mst_c));
	memset(&td_mst_tmp,0x00,sizeof(struct td_mst_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*** !!!!加上计息不计息标志的控制 ***/
	/*** 取值 ***/
	get_zd_data("0310",g_pub_tx.ac_no);			/*取帐号*/
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/*取序号*/
	get_zd_data("0670",tmp_ind1);				/*处理类型 1-冻结 2-解冻 */
	get_zd_double("1001",&ac_bal);				/*帐户余额*/

	get_zd_data("0680",tmp_ind2);				/*冻结方式*/
	get_zd_double("0410",&g_pub_tx.tx_amt1);	/*冻结金额*/
	get_zd_data("0690",tmp_ind3);				/*是否计息(冻结)*/
	get_zd_data("0810",tmp_memo1);				/*冻结备注*/
	get_zd_long("0440",&tmp_date);				/*到期日期*/
	get_zd_data("0710",tmp_ind5);				/*解冻方式*/
	get_zd_int("0350",&hold_seqn);				/*冻结序号*/
	get_zd_double("0420",&g_pub_tx.tx_amt2);	/*解冻金额*/
	get_zd_data("0720",tmp_ind6);				/*是否计息(解冻)*/
	get_zd_data("0820",tmp_memo2);				/*解冻备注*/
	
	/*程序流程*/
	if(tmp_date) strcpy(tmp_ind4,"Y");
	else strcpy(tmp_ind4,"N");
		
	g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'", \
								 g_pub_tx.ac_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"帐号不存在!!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	g_pub_tx.ac_id = g_mdm_ac_rel.ac_id;

	g_reply_int=pub_base_disac_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
									g_pub_tx.ac_type);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"根据显示帐号和序号取主文件失败!!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"O017");
		goto ErrExit;
	}

	/* zy添加于2005.7.27 begin */	
	switch( g_pub_tx.ac_type[0] )
	{
		case '1':				
				if( strcmp( g_pub_tx.tx_br_no, g_dd_mst.opn_br_no ) != 0 )
				{
					sprintf( acErrMsg, "冻结交易不允许通兑!opn_br_no=[%s]", \
							g_dd_mst.opn_br_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "D208" );
					sprintf( acErrMsg,"此账号开户机构为【%s】", \
							g_dd_mst.opn_br_no );
					set_zd_data( DC_GET_MSG, acErrMsg );
					goto ErrExit;
				}
				break;
		case '2':
				if( strcmp( g_pub_tx.tx_br_no, g_td_mst.opn_br_no ) != 0 )
				{
					sprintf( acErrMsg, "冻结交易不允许通兑!opn_br_no=[%s]", \
							g_td_mst.opn_br_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "D208" );
					sprintf( acErrMsg,"此账号开户机构为【%s】", \
							g_td_mst.opn_br_no );
					set_zd_data( DC_GET_MSG, acErrMsg );
					goto ErrExit;
				}
				break;
		case '3':
				if( strcmp( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no ) != 0 )
				{
					sprintf( acErrMsg, "冻结交易不允许通兑!opn_br_no=[%s]", \
							 g_ln_mst.opn_br_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "D208" );
					sprintf( acErrMsg,"此账号开户机构为【%s】", \
							g_ln_mst.opn_br_no );
					set_zd_data( DC_GET_MSG, acErrMsg );
					goto ErrExit;
				}
				break;		
		case '9':
				if( strcmp( g_pub_tx.tx_br_no, g_in_mst.opn_br_no ) != 0 )
				{
					sprintf( acErrMsg, "冻结交易不允许通兑!opn_br_no=[%s]", \
							 g_in_mst.opn_br_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "D208" );
					sprintf( acErrMsg,"此账号开户机构为【%s】", \
							 g_in_mst.opn_br_no );
					set_zd_data( DC_GET_MSG, acErrMsg );
					goto ErrExit;
				}
				break;							
		default	:
				sprintf(acErrMsg,"账号对应产品类型错误,请与中心机房联系!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "C120" );
				goto	ErrExit;
	}
	/* zy添加于2005.7.27 end */	
	
	/**if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
	 	sprintf(acErrMsg,"该介质已作销户处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}	------- hao --------- @20050716@---
	else if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"该介质处于挂失换证状态,请先作挂失换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M010");
		goto ErrExit;
	}                 -----**/
/* del by martin 2008-11-19 15:55  lhr 提出
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"该介质已换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M011");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"该账户已作部提!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M017");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='5' )
	{
		sprintf(acErrMsg,"该账户已作撤销!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M019");
		goto ErrExit;
	}                       
*/
	if( tmp_ind1[0]=='1' )							/*1-冻结处理*/
	{
		i=1;
		/*** 取登记冻结登记薄信息 ***/
		g_mo_hold.ac_id = g_pub_tx.ac_id;
		g_mo_hold.ac_seqn = g_pub_tx.ac_seqn;
		strcpy(g_mo_hold.hold_typ,tmp_ind2);

		g_mo_hold.plan_hold_amt = g_pub_tx.tx_amt1;
		strcpy(g_mo_hold.auto_unhold_ind,tmp_ind4);
		strcpy(g_mo_hold.hold_sts,"0");
		g_mo_hold.beg_hold_date = g_pub_tx.tx_date;
		g_mo_hold.plan_unhold_date = tmp_date;
		strcpy(g_mo_hold.hold_br_no,g_pub_tx.tx_br_no);
		strcpy(g_mo_hold.hold_tel,g_pub_tx.tel);
		strcpy(g_mo_hold.hold_brf,tmp_memo1);

		strcpy(g_mo_hold.unhold_br_no,"");
		strcpy(g_mo_hold.unhold_tel,"");
		strcpy(g_mo_hold.unhold_brf,"");
		g_mo_hold.wrk_date = g_pub_tx.tx_date;
		g_mo_hold.trace_no = g_pub_tx.trace_no;

		ret = sql_count("mo_hold","ac_id=%ld and ac_seqn=%d", g_pub_tx.ac_id,
						g_pub_tx.ac_seqn);
		if( ret<0 )
		{
			sprintf(acErrMsg,"查询冻结登记薄异常!![%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==0 )
			i = 1;
		else 
			i = ret + 1 ;

		g_reply_int=Mo_hold_Dec_Upd(g_pub_tx.reply, \
									"ac_id=%ld and ac_seqn=%d and hold_sts='0'",
									g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"查询冻结登记薄异常!![%d]",g_reply_int);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O016");
			goto ErrExit;
		}

		while(1)
		{
			g_reply_int=Mo_hold_Fet_Upd(&f_mo_hold,g_pub_tx.reply); 
			if( g_reply_int==100 )break;
			else if( g_reply_int )
			{
				sprintf(acErrMsg,"查询冻结登记薄异常!![%d]",g_reply_int);
				WRITEMSG
				strcpy (g_pub_tx.reply,"O016");
				goto ErrExit;
			}
/***  del by martin 2008-11-19 15:50  不做此操作 lhr 提出
			if( f_mo_hold.hold_typ[0]=='1' )			*** 已完全冻结 ***
			{
				sprintf(acErrMsg,"该账户已完全冻结!");
				WRITEMSG
				strcpy (g_pub_tx.reply,"P110");
				goto ErrExit;
			}
			else if( f_mo_hold.hold_typ[0]=='2' )		*** 已只进不出 ***
			{
				*** 又选择了只进不出或部分冻结 ***
				if(  tmp_ind2[0]=='2' || tmp_ind2[0]=='3' )
				{
					sprintf(acErrMsg,"该账户已只进不出f_mo_hold.hold_typ=[%s]", 
							f_mo_hold.hold_typ);
					WRITEMSG
					strcpy (g_pub_tx.reply,"O161");
					goto ErrExit;
				}
			}

			if( tmp_ind2[0]=='1' && \
			  ( f_mo_hold.hold_typ[0]=='2' || f_mo_hold.hold_typ[0]=='3') )
			{
				strcpy(f_mo_hold.hold_sts,"1");		*解除冻结*
				unlock_flag=1;
				sprintf(f_mo_hold.unhold_br_no,g_pub_tx.tx_br_no);
				sprintf(f_mo_hold.unhold_tel,g_pub_tx.tel);
				sprintf(f_mo_hold.unhold_brf,"追加完全冻结");
			}

			if( tmp_ind2[0]=='2' && f_mo_hold.hold_typ[0]=='3' )
			{
				strcpy(f_mo_hold.hold_sts,"1");		*解除冻结*
				unlock_flag=1;
				sprintf(f_mo_hold.unhold_br_no,g_pub_tx.tx_br_no);
				sprintf(f_mo_hold.unhold_tel,g_pub_tx.tel);
				sprintf(f_mo_hold.unhold_brf,"追加只进不出冻结");
			}

			g_reply_int=Mo_hold_Upd_Upd(f_mo_hold,g_pub_tx.reply);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"查询冻结登记薄异常!![%d]",g_reply_int);
				WRITEMSG
				strcpy (g_pub_tx.reply,"O016");
				goto ErrExit;
			}***/
		}

		Mo_hold_Clo_Upd();
		
		/*登记冻结登记薄*/
		g_mo_hold.hold_seqn = i;
		g_reply_int = Mo_hold_Ins(g_mo_hold,g_pub_tx.reply);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"登记冻结登记薄异常!!g_reply_int=[%d]", \
					g_reply_int);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O015");
			goto ErrExit;
		}
		
		/*查询账户是否存在冻结记录*/
		icnt = sql_count( "mo_hold","ac_id=%ld and ac_seqn=%d and hold_sts='0'", \
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( icnt<0 )
		{
			sprintf(acErrMsg,"查询冻结登记薄失败!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
		ret = sql_min_string("mo_hold","hold_typ", min_ht, sizeof(min_ht)-1,\
                         "ac_id=%ld and ac_seqn=%d and hold_sts='0'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
     if( ret<0 )
		{
			sprintf(acErrMsg,"查询冻结登记薄失败!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
	  ret=sql_sum_double("mo_hold","plan_hold_amt",&sum_amt,\
                   "ac_id=%ld and ac_seqn=%d and hold_sts='0'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( ret<0 )
		{
			sprintf(acErrMsg,"查询冻结登记薄失败!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}

 	 	switch(g_pub_tx.ac_type[0])
  		{
    		case '1':									/*1-活期*/
				/*** 完全冻结不能再冻结 ***/
		 		/* del by lhr 2008-11-19 16:23
		 		if( g_dd_mst.hold_sts[0]=='1' )
		 		{
					sprintf(acErrMsg,"该账户已处于完全冻结状态!");
					WRITEMSG
					strcpy (g_pub_tx.reply,"P110");
					goto ErrExit;
		 		}
		 		*/

				g_reply_int=Dd_mst_Dec_Upd(g_pub_tx.reply, \
					"ac_id=%ld and ac_seqn=%d",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	    		if(g_reply_int)
	   			{
	   				sprintf(acErrMsg,"定义游标错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				
				if( icnt==0 )							/*** 无冻结纪录 ***/
					{
					  g_dd_mst.hold_sts[0]='0'; 
					  g_dd_mst.hold_amt=0.00; 
				  }
				else 
				{
					strcpy(g_dd_mst.hold_sts,min_ht);
					g_dd_mst.hold_amt=sum_amt; 
				}

				g_reply_int = Dd_mst_Fet_Upd(&f_dd_mst,g_pub_tx.reply);
				if( g_reply_int==100 )
				{
					sprintf(acErrMsg,"账号不存在!!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "M003");
					goto ErrExit;
				}
				else if( g_reply_int )
				{
					sprintf(acErrMsg,"取记录错误![%d][%ld][%d]", \
							g_reply_int,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}

				if( f_dd_mst.ac_sts[0]=='*' )
				{
					sprintf(acErrMsg,"该账户已销!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P245");
					goto ErrExit;
				}
				
				strcpy(f_dd_mst.hold_sts,g_dd_mst.hold_sts);
				f_dd_mst.hold_amt=g_dd_mst.hold_amt; 
				/* if(atoi(f_dd_mst.hold_sts)>atoi(tmp_ind2))add by lhr 2008-11-19 16:27 
					{
						strcpy(f_dd_mst.hold_sts,tmp_ind2);
					}*/

				/*** 部分冻结 
				if( tmp_ind2[0]=='3' )		
					f_dd_mst.hold_amt=f_dd_mst.hold_amt+g_mo_hold.plan_hold_amt;***/

				/*** 全部冻结、只进不出 del by lhr  2008-11-19 16:26
				if( tmp_ind2[0]=='1' || tmp_ind2[0]=='2' )
					f_dd_mst.hold_amt = f_dd_mst.bal;
					***/
				g_reply_int = Dd_mst_Upd_Upd(f_dd_mst,g_pub_tx.reply);
				if( g_reply_int )
				{	
		 			sprintf(acErrMsg,"修改记录错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				Dd_mst_Clo_Upd( );
				set_zd_double("0410", f_dd_mst.bal);/**用于返回账户最终的余额 zgf 20140220**/
		 		break;
			case '2':									/*2-定期*/
				/*** 完全冻结不能再冻结 ***/
				/* del by lhr 2008-11-19 16:23
		 		if( g_td_mst.hold_sts[0]=='1' )
		 		{
					sprintf(acErrMsg,"该账户已处于完全冻结状态!");
					WRITEMSG
					strcpy (g_pub_tx.reply,"P110");
					goto ErrExit;
		 		}*/

				g_reply_int=Td_mst_Dec_Upd(g_pub_tx.reply, \
							"ac_id=%ld and ac_seqn=%d",g_pub_tx.ac_id, \
							g_pub_tx.ac_seqn);
	    		if(g_reply_int)
	   			{
	   				sprintf(acErrMsg,"定义游标错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				
				if( icnt==0 )							/*** 无冻结纪录 ***/
				 {
					g_td_mst.hold_sts[0]='0'; 
					g_td_mst.hold_amt=0.00; 
				 }
				else 
				{
					strcpy(g_td_mst.hold_sts,min_ht);
					g_td_mst.hold_amt=sum_amt; 
				}

				g_reply_int=Td_mst_Fet_Upd(&f_td_mst,g_pub_tx.reply);
				if(g_reply_int&&g_reply_int!=100)
				{
					sprintf(acErrMsg,"取记录错误!");
		 			WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				else if(g_reply_int==100)
				{
					sprintf(acErrMsg,"账号不存在!");
		 			WRITEMSG
					strcpy(g_pub_tx.reply, "M003");
					goto ErrExit;
				}

				if( f_td_mst.ac_sts[0]=='*' )
				{
					sprintf(acErrMsg,"该账户已销!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P245");
					goto ErrExit;
				}
				
				strcpy(f_td_mst.hold_sts,g_td_mst.hold_sts);
				f_td_mst.hold_amt=g_td_mst.hold_amt; 
        /*if(atoi(f_td_mst.hold_sts)>atoi(tmp_ind2)) add by lhr 2008-11-19 16:27 
					{
						strcpy(f_td_mst.hold_sts,tmp_ind2);
					}*/
				

				/*** 部分冻结 
				if( tmp_ind2[0]=='3' )						
					f_td_mst.hold_amt=f_td_mst.hold_amt+g_mo_hold.plan_hold_amt;***/

				/*** 全部冻结、只进不出 del by lhr  2008-11-19 16:26
				if( tmp_ind2[0]=='1' || tmp_ind2[0]=='2' )
					f_td_mst.hold_amt = f_td_mst.bal;
				***/

				g_reply_int=Td_mst_Upd_Upd(f_td_mst,g_pub_tx.reply);
				if(g_reply_int)
				{	
		 			sprintf(acErrMsg,"修改记录错误!");
		 			WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				Td_mst_Clo_Upd( );
				set_zd_double("0410", f_td_mst.bal);/**用于返回账户最终的余额 zgf 20140220**/
		 		break;
			default :
		 		sprintf(acErrMsg,"该账户不属于冻结账户范围!!");
		 		WRITEMSG
		 		strcpy (g_pub_tx.reply,"O014");
		 		goto ErrExit;
		 		break;
 	 	}	/*switch结束*/

		/*登记冻结登记薄 2008-11-19 18:13 del by martin
		g_mo_hold.hold_seqn = i;
		g_reply_int = Mo_hold_Ins(g_mo_hold,g_pub_tx.reply);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"登记冻结登记薄异常!!g_reply_int=[%d]", \
					g_reply_int);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O015");
			goto ErrExit;
		}*/
 
                set_zd_int("0490",i);


                cnt = sql_count( "mo_hold","ac_id=%ld and ac_seqn=%d and hold_sts='0'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
                      /********* 2008-11-19 15:57 lhr
                      if( cnt<0)
                      {    sprintf(acErrMsg,"查询冻结登记薄失败!");
                           WRITEMSG
                           strcpy (g_pub_tx.reply,"O021");
                           goto ErrExit;
                      } 
                      else if( ret==0 )
                           cnt = 1;
                      else
                           cnt = ret + 1 ;
                      **********/
               pub_base_AllDwnFilName( tmpname );
                      fp=fopen(tmpname,"w");
                      if( fp==NULL )
                       {
                        sprintf(acErrMsg,"打开文件[%s]错",tmpname);
                        WRITEMSG
                        strcpy( g_pub_tx.reply,"P157" );
                        goto ErrExit;
                       }
                    fprintf(fp,"CKPT%d|\n",cnt);
                    fclose(fp);
                    set_zd_data(DC_FILE_SND,"1"); 

		/***返回冻结序号,初始为1***/
		set_zd_int("0490",i);	
	}
	else if( tmp_ind1[0]=='2' )							/*2-解冻处理*/	
	{
		/*更新冻结登记薄*/
		g_reply_int = Mo_hold_Dec_Upd(g_pub_tx.reply, \
							"ac_id=%ld and ac_seqn=%d and hold_seqn=%d", \
							g_pub_tx.ac_id,g_pub_tx.ac_seqn,hold_seqn);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"查询冻结登记薄异常!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O016");
			goto ErrExit;
		}

		g_reply_int = Mo_hold_Fet_Upd(&f_mo_hold,g_pub_tx.reply);
		if( g_reply_int==100 )
		{
			sprintf(acErrMsg,"--------->[%ld][%d][%d]",g_pub_tx.ac_id, g_pub_tx.ac_seqn, hold_seqn );
			WRITEMSG
			sprintf(acErrMsg,"冻结登记薄中不存在该笔记录!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O019");
			goto ErrExit;
		}
		else if( g_reply_int )
		{
			sprintf(acErrMsg,"冻结登记薄中不存在该笔记录!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		strcpy(g_mo_hold.rowid,f_mo_hold.rowid);
		beg_hold_date = f_mo_hold.beg_hold_date;
		g_mo_hold.ac_id = g_pub_tx.ac_id;
		g_mo_hold.ac_seqn = g_pub_tx.ac_seqn;
		g_mo_hold.hold_seqn = hold_seqn;
		strcpy(g_mo_hold.hold_typ,f_mo_hold.hold_typ);
		/**-------- hao ---------- **/
		g_mo_hold.plan_hold_amt = f_mo_hold.plan_hold_amt;

		/*** 冻结已解除 ***/
		if( f_mo_hold.hold_sts[0]=='1' )
		{
			sprintf(acErrMsg,"该账户已解除冻结!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"D225");
			goto ErrExit;
		}

		/*** 完全冻结、未解除、非完全解冻 ***/
		if( f_mo_hold.hold_typ[0]=='1' && f_mo_hold.hold_sts[0]=='0' \
			&& tmp_ind5[0]!='1' )
		{
			sprintf(acErrMsg,"该账户为完全冻结，请作完全解冻!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O162");
			goto ErrExit;
		}
		else if(tmp_ind5[0]=='1' && f_mo_hold.hold_sts[0]=='0')	/*完全解冻未解*/
			g_mo_hold.hold_sts[0]='1';						/*改为解除状态*/
		else if( tmp_ind5[0]=='2' )							/*部分解冻*/
		{
		if(f_mo_hold.hold_typ[0]=='3' )/* 2008-11-19 16:47 除了部分冻结判断*/
		 {
		 	vtcp_log("[%s][%d],hold_typ=[%s],tmp_ind5=[%s],f_mo_hold.hold_sts=[%s]\n",__FILE__,__LINE__,f_mo_hold.hold_typ,tmp_ind5,f_mo_hold.hold_sts);
			if(pub_base_CompDblVal(g_mo_hold.plan_hold_amt,g_pub_tx.tx_amt2)>0)
			{
				g_mo_hold.hold_typ[0]='3';				    /*改为部分冻结*/
				g_mo_hold.hold_sts[0]='0';					/*改为未解除状态*/
				g_mo_hold.plan_hold_amt = \
					f_mo_hold.plan_hold_amt - g_pub_tx.tx_amt2;
				g_pub_tx.tx_amt1 = f_mo_hold.plan_hold_amt;
			}
			else if(pub_base_CompDblVal( g_mo_hold.plan_hold_amt, \
										 g_pub_tx.tx_amt2 )==0 )
			{
				g_mo_hold.hold_sts[0]='1';					/*改为解除状态*/
				g_pub_tx.tx_amt1 = g_pub_tx.tx_amt2;
			}
			else
			{
				sprintf(acErrMsg,"解冻金额大于冻结金额，请核对!![%.2f][%.2f]", \
						g_mo_hold.plan_hold_amt,g_pub_tx.tx_amt2);
				WRITEMSG
				strcpy (g_pub_tx.reply,"P418");
				goto ErrExit;
			}
		 }else/*2008-11-19 16:49 */
		 	{
		 		vtcp_log("[%s][%d],hold_typ=[%s],tmp_ind5=[%s],f_mo_hold.hold_sts=[%s]\n",__FILE__,__LINE__,f_mo_hold.hold_typ,tmp_ind5,f_mo_hold.hold_sts);
		 		sprintf(acErrMsg,"全部冻结和只进不出不支持部分解冻!!" );
				WRITEMSG
				strcpy (g_pub_tx.reply,"P313");
				goto ErrExit;
		 	}
		 /*2008-11-19 16:49 end*/
		}

		strcpy(g_mo_hold.auto_unhold_ind,f_mo_hold.auto_unhold_ind);
		g_mo_hold.beg_hold_date = f_mo_hold.beg_hold_date;
		g_mo_hold.plan_unhold_date = f_mo_hold.plan_unhold_date;
		strcpy(g_mo_hold.hold_br_no,f_mo_hold.hold_br_no);
		strcpy(g_mo_hold.hold_tel,f_mo_hold.hold_tel);
		strcpy(g_mo_hold.hold_brf,f_mo_hold.hold_brf);

		strcpy(g_mo_hold.unhold_br_no,g_pub_tx.tx_br_no);
		strcpy(g_mo_hold.unhold_tel,g_pub_tx.tel);
		strcpy(g_mo_hold.unhold_brf,tmp_memo2);
		g_mo_hold.wrk_date = g_pub_tx.tx_date;
		g_mo_hold.fct_unhold_date = g_pub_tx.tx_date;
		g_mo_hold.trace_no = g_pub_tx.trace_no;

    	/*更新冻结登记薄*/
		g_reply_int = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
	vtcp_log("g_reply_int[%d]",g_reply_int);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"更新冻结登记薄失败!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}

		Mo_hold_Clo_Upd( );

		/*查询账户是否存在冻结记录*/
		i = sql_count( "mo_hold","ac_id=%ld and ac_seqn=%d and hold_sts='0'", \
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( i<0 )
		{
			sprintf(acErrMsg,"查询冻结登记薄失败!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
		ret = sql_min_string("mo_hold","hold_typ", min_ht, sizeof(min_ht)-1,\
                         "ac_id=%ld and ac_seqn=%d and hold_sts='0'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
     if( ret<0 )
		{
			sprintf(acErrMsg,"查询冻结登记薄失败!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
	  ret=sql_sum_double("mo_hold","plan_hold_amt",&sum_amt,\
                   "ac_id=%ld and ac_seqn=%d and hold_sts='0'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( ret<0 )
		{
			sprintf(acErrMsg,"查询冻结登记薄失败!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
		/*** 修改主文件 ***/
 		switch( g_pub_tx.ac_type[0] )
  		{
			case '1':
				if( g_dd_mst.hold_sts[0]=='0' )
				{
					sprintf(acErrMsg,"该账户已处于正常状态!");
					WRITEMSG
					strcpy (g_pub_tx.reply,"O018");
					goto ErrExit;
				}

				if( i==0 )							/*** 无冻结纪录 ***/
					{
					  g_dd_mst.hold_sts[0]='0'; 
					  g_dd_mst.hold_amt=0.00; 
				  }
				else 
				{
					strcpy(g_dd_mst.hold_sts,min_ht);
					g_dd_mst.hold_amt=sum_amt; 
					/* if( tmp_ind5[0]=='2' )			部分解冻时改为部分冻结 
						g_dd_mst.hold_sts[0]='3';*/
				}

				g_reply_int = Dd_mst_Dec_Upd(g_pub_tx.reply, \
											"ac_id=%ld and ac_seqn=%d", \
											g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	   			if( g_reply_int )
	   			{
	   				sprintf(acErrMsg,"定义游标错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}

				g_reply_int = Dd_mst_Fet_Upd(&f_dd_mst,g_pub_tx.reply);
				if( g_reply_int==100 )
				{
					sprintf(acErrMsg,"账号不存在!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				else if( g_reply_int )
				{
					sprintf(acErrMsg,"查找记录出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
/***  del by martin 2008-11-19 17:43
				strcpy( f_dd_mst.hold_sts,g_dd_mst.hold_sts );

				if( tmp_ind5[0]=='2' )
					f_dd_mst.hold_amt = f_dd_mst.hold_amt - g_pub_tx.tx_amt2;
				else if( tmp_ind5[0]=='1' )
					f_dd_mst.hold_amt = 0.00;
				else
					f_dd_mst.hold_amt = \
					f_dd_mst.hold_amt - f_mo_hold.plan_hold_amt;
***/
        strcpy(f_dd_mst.hold_sts,g_dd_mst.hold_sts);
				f_dd_mst.hold_amt=g_dd_mst.hold_amt; 

				g_reply_int = Dd_mst_Upd_Upd(f_dd_mst,g_pub_tx.reply);
				if( g_reply_int )
				{	
	 				sprintf(acErrMsg,"修改记录错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				Dd_mst_Clo_Upd( );
	 			break;
			case '2':
				if( g_td_mst.hold_sts[0]=='0' )
				{
					sprintf(acErrMsg,"该账户已处于正常状态!!");
					WRITEMSG
					strcpy (g_pub_tx.reply,"O018");
					goto ErrExit;
				}

				if( i==0 )							/*** 无冻结纪录 ***/
				 {
					g_td_mst.hold_sts[0]='0'; 
					g_td_mst.hold_amt=0.00; 
				 }
				else 
				{
					strcpy(g_td_mst.hold_sts,min_ht);
					g_td_mst.hold_amt=sum_amt; 
					/* if( tmp_ind5[0]=='2' )			部分解冻时改为部分冻结 
						g_td_mst.hold_sts[0]='3';*/
				}

				g_reply_int = Td_mst_Dec_Upd(g_pub_tx.reply, \
											"ac_id=%ld and ac_seqn=%d", \
											g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    			if( g_reply_int )
   				{
   					sprintf(acErrMsg,"定义游标错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}

				g_reply_int = Td_mst_Fet_Upd(&f_td_mst,g_pub_tx.reply);
				if( g_reply_int==100 )
				{
					sprintf(acErrMsg,"账号不存在!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				else if( g_reply_int )
				{
					sprintf(acErrMsg,"查找记录出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}

				/* 2008-11-19 17:46 by martin
				strcpy(f_td_mst.hold_sts,g_td_mst.hold_sts);

				if(tmp_ind5[0]=='2')
					f_td_mst.hold_amt = f_td_mst.hold_amt - g_pub_tx.tx_amt2;
				else
					f_td_mst.hold_amt=f_td_mst.hold_amt-f_mo_hold.plan_hold_amt;
	
	vtcp_log("hao--look[%d]",f_td_mst.ac_id);*/
	
	      strcpy(f_td_mst.hold_sts,g_td_mst.hold_sts);
				f_td_mst.hold_amt=g_td_mst.hold_amt; 
	      
				g_reply_int = Td_mst_Upd_Upd(f_td_mst,g_pub_tx.reply);
	vtcp_log("hao--look1");
				if( g_reply_int )
				{	
	 				sprintf(acErrMsg,"修改记录错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				Td_mst_Clo_Upd( );
	 			break;
		default:
			 	sprintf(acErrMsg,"该账户不属于解冻账户范围!!");
	 			WRITEMSG
	 			strcpy (g_pub_tx.reply,"O015");
	 			goto ErrExit;
	 			break;
      }/*switch结束*/

		pub_base_AllDwnFilName( tmpname );
		fp=fopen(tmpname,"a");
		if( fp==NULL )
		{
			sprintf(acErrMsg,"打开文件[%s]错",tmpname);
			WRITEMSG
			strcpy( g_pub_tx.reply,"P157" );
			goto ErrExit;
		}

		fprintf(fp,"%ld|\n",beg_hold_date);
		fclose(fp);
		set_zd_data(DC_FILE_SND,"1");
	}
	else 
	{
		sprintf(acErrMsg,"冻结解冻操作类型选择不正确!!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"O020");
		goto ErrExit;
	}

    /*** 登记交易流水 ***/
	WRITEMSG
	vtcp_log("ac_wrk_ind=[%s]",g_pub_tx.ac_wrk_ind);
	get_zd_data("0670",tmp_ind1);				/*处理类型 1-冻结 2-解冻 */
	if (tmp_ind1[0] == '1')
		strcpy(g_pub_tx.brf, "冻结");
	else
		strcpy(g_pub_tx.brf, "解冻");

	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
		strcpy (g_pub_tx.reply,"S024");
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"冻结解冻操作成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"冻结解冻操作失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

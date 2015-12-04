/*************************************************************
* 文 件 名:  sp9202.c
* 功能描述 可扣划金额查询
*
*************************************************************/

#define EXTERN
#include "public.h"
#include "mo_hold_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"  
int sp9202()
{
	struct mo_hold_c   g_mo_hold;
        struct mdm_ac_rel_c  g_mdm_ac_rel;
        struct dd_mst_c    g_dd_mst;
        struct td_mst_c    g_td_mst;
        int ret=0;
        long  hold_no;
        int i=1;
        double sum=0.00;
        long ac_seqn;
        int  icnt=0;
        char min_ht[2];

        memset(&g_mo_hold,0x00,sizeof(struct mo_hold_c));
        memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	pub_base_sysinit();
       
       get_zd_long("0350",&hold_no);
       get_zd_data("0310",g_pub_tx.ac_no);
       get_zd_long("0340",&ac_seqn);
         vtcp_log("%s %d oooooooooooo ===[%ld] ",__FILE__,__LINE__,hold_no);
        ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
               if(ret==100){
                   sprintf(acErrMsg,"数据库无此记录");
                   WRITEMSG
                   goto ErrExit;
                }
                if(ret!=0){
                    sprintf(acErrMsg,"查询介质关系对照表错误");
                    WRITEMSG
                    goto ErrExit;
                }
            g_mo_hold.ac_id=g_mdm_ac_rel.ac_id;
             vtcp_log("%s %d xxxxxxxxxxxx ===[%ld] ",__FILE__,__LINE__,g_mdm_ac_rel.ac_id);
        vtcp_log("%s %d oooooooooooo ===[%ld] ",__FILE__,__LINE__,hold_no);
/** del by martin 2008-11-22 15:35
        ret=sql_count("mo_hold","hold_seqn<=%ld and ( hold_typ='1' or hold_typ='2') and  ac_id=%ld ",hold_no,g_mo_hold.ac_id);
                 if(ret<0){
                            sprintf(acErrMsg,"出错!");
                            WRITEMSG
                            goto ErrExit;
                          }
                 if(ret>0){
                            sum=0.00;
                            set_zd_double("0390",sum);
                            goto OkExit;
                          }

          i=1;
          while(i<=hold_no){
         vtcp_log("%s %d pppppppppppp ===[%ld] ",__FILE__,__LINE__,hold_no);
                       ret=Mo_hold_Sel(g_pub_tx.reply,&g_mo_hold,"hold_seqn<=%ld  and  ac_id=%ld   and  hold_sts='0' and  hold_seqn=%d   ",hold_no,g_mo_hold.ac_id,i );

                          if(ret==100){
                             sprintf(acErrMsg,"数据库无此记录");
                             WRITEMSG
                             goto ErrExit;
                          }
                         
                         if(ret!=0){
                             sprintf(acErrMsg,"查询冻结等级簿出错!");
                             WRITEMSG
                             goto ErrExit;
                         }
                i=i+1;
                sum=sum+g_mo_hold.plan_hold_amt;
                continue;
           }
          
             vtcp_log("%s %d shishislhislhishi==[%.2f] ",__FILE__,__LINE__,sum);
           if(ac_seqn==1){
                    ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ",g_mo_hold.ac_id);
                        if(ret==100){
                           sprintf(acErrMsg,"数据库无此记录");
                           WRITEMSG
                           goto ErrExit;
                        }
                        if(ret!=0){
                          sprintf(acErrMsg,"查询主表错误");
                          WRITEMSG
                          goto ErrExit;
                        } 
                 sum=g_dd_mst.bal-sum;
           }
           if(ac_seqn==0){
                  ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ", g_pub_tx.ac_id);
                      if(ret==100){
                         sprintf(acErrMsg,"数据库无此记录");
                         WRITEMSG
                         goto ErrExit;
                      }
                      if(ret!=0){
                         sprintf(acErrMsg,"查询主表错误");
                         WRITEMSG
                         goto ErrExit;
                     }
                sum=g_td_mst.bal-sum;
         }
del by martin end  2008-11-22 15:36*/
	vtcp_log("进入pub_acct_check_hold!i=[%d]",i);
	ret=pub_acct_check_hold( i,hold_no,&icnt,&min_ht,&sum,g_mo_hold.ac_id,ac_seqn);/*冻结扣划解冻检查*/
	vtcp_log("pub_acct_check_hold出口[%d]sum==[%lf]!",ret,sum);
	if(ret!=0)
		{
			goto ErrExit;
		}
            set_zd_double("0390",sum);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
/***********************************
* 函 数 名:     pub_acct_check_hold
* 功能描述:   检查冻结模式 扣划解冻检查
*             用于更新登记簿后，更新主表前
*             判断之前冻结序号前的交易
*
* 输    入:    1、ac_id帐户id
*              2、ac_seqn
*              3、hold_seq
*              
*              mo_ind: 0=冻结，1=解冻
*              
* 返    回:    1、返回冻结次数icnt;
*              2、返回最大冻结权限min_ht;
*              3、返回部分冻结金额sum_amt;
*              
* 标志用法：
*
* 作    者:	    martin 	
* 完成日期：    2008-11-21 15:32
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
************************************/
int pub_acct_check_hold(
int  mo_ind,
int hold_seqn,
int *icnt,
char *min_ht,
double *sum_amt,
long ac_id,
long ac_seqn )
{
	struct mo_hold_c t_mo_hold;
	struct dd_mst_c  g_dd_mst;
	struct td_mst_c  g_td_mst;
	
	int ret=0;
	memset(&t_mo_hold,0x00,sizeof(struct mo_hold_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	
	/*查询账户是否存在冻结记录 end*/
if( mo_ind>0 )/* 解冻 */
	{
	/*查询账户是否存在冻结记录*/
		*icnt = sql_count( "mo_hold","ac_id=%ld and ac_seqn=%d and hold_sts='0' and hold_seqn<%d ", \
						ac_id,ac_seqn,hold_seqn);
		if( icnt<0 )
		{
			sprintf(acErrMsg,"查询冻结登记薄失败!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}

			sprintf(acErrMsg,"[%s][%d]进入了*icnt=[%d]!",__FILE__,__LINE__,*icnt);
			WRITEMSG
			
		ret = sql_min_string("mo_hold","hold_typ", min_ht, sizeof(min_ht)-1,\
                         "ac_id=%ld and ac_seqn=%d and hold_sts='0'  and hold_seqn<%d ",ac_id,ac_seqn,hold_seqn);
     if( ret<0 )
		{
			sprintf(acErrMsg,"查询冻结登记薄失败!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		sprintf(acErrMsg,"[%s][%d]进入了min_ht=[%s]!",__FILE__,__LINE__,min_ht);
			WRITEMSG

	  ret=sql_sum_double("mo_hold","plan_hold_amt",sum_amt,\
                   "ac_id=%ld and ac_seqn=%d and hold_sts='0'  and hold_seqn<%d ",ac_id,ac_seqn,hold_seqn);
		if( ret<0 )
		{
			sprintf(acErrMsg,"查询冻结登记薄失败!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
			sprintf(acErrMsg,"[%s][%d]进入了解冻*sum_amt=[%lf]!",__FILE__,__LINE__,*sum_amt);
			WRITEMSG
			
		ret = Mo_hold_Dec_Sel(g_pub_tx.reply, \
							"ac_id=%ld and ac_seqn=%d and hold_seqn=%d and hold_sts='0'", \
							ac_id,ac_seqn,hold_seqn);
		if( ret )
		{
			sprintf(acErrMsg,"查询冻结登记薄异常!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O016");
			goto ErrExit;
		}

		ret = Mo_hold_Fet_Sel(&t_mo_hold,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"--------->[%ld][%d][%d]",ac_id,ac_seqn, hold_seqn );
			WRITEMSG
			sprintf(acErrMsg,"冻结登记薄中不存在该笔记录!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O019");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"冻结登记薄中不存在该笔记录!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		
		
		sprintf(acErrMsg,"冻结登记薄中存在[%d]条记录之前的t_mo_hold.hold_typ=[%s],min_ht=[%s]!",*icnt,t_mo_hold.hold_typ,min_ht);
			WRITEMSG
		/* 权限判断 */
		if( t_mo_hold.hold_typ[0]=='1' )
			{
				if(min_ht[0]=='1')
					{
						sprintf(acErrMsg,"冻结登记薄中已存在完全冻结!");
			      WRITEMSG
			      strcpy (g_pub_tx.reply,"P110");
			      goto ErrExit;
					}
			}else
		if( t_mo_hold.hold_typ[0]=='2' )
			{
				if(min_ht[0]=='1')
					{
						sprintf(acErrMsg,"冻结登记薄中已存在完全冻结!");
			      WRITEMSG
			      strcpy (g_pub_tx.reply,"P110");
			      goto ErrExit;
					}else
				if(min_ht[0]=='2')
					{
						sprintf(acErrMsg,"冻结登记薄中已存在只进不出冻结!");
			      WRITEMSG
			      strcpy (g_pub_tx.reply,"P111");
			      goto ErrExit;
					}
			}else
		if( t_mo_hold.hold_typ[0]=='3' )
			{
				if(min_ht[0]=='1')
					{
						sprintf(acErrMsg,"冻结登记薄中已存在完全冻结!");
			      WRITEMSG
			      strcpy (g_pub_tx.reply,"P110");
			      goto ErrExit;
					}else
				if(min_ht[0]=='2')
					{
						sprintf(acErrMsg,"冻结登记薄中已存在只进不出冻结!");
			      WRITEMSG
			      strcpy (g_pub_tx.reply,"P111");
			      goto ErrExit;
					}else
				if(min_ht[0]=='3')
					{
					}
					*sum_amt=t_mo_hold.plan_hold_amt;
			}

			/*金额判断*/
			if(ac_seqn==1)
					{
					   ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id='%ld' ",ac_id);
			       if(ret==100){
                       sprintf(acErrMsg,"数据库无此记录");
                       WRITEMSG
                       goto ErrExit;
                    }
                    if(ret!=0){
                       sprintf(acErrMsg,"查询主表错误");
                       WRITEMSG
                       goto ErrExit;
                   }
					
					   if(*sum_amt>g_dd_mst.bal)
						  {
							  sprintf(acErrMsg,"冻结登记薄中金额大于帐户余额!");
			          WRITEMSG
			          strcpy (g_pub_tx.reply,"P418");
			          goto ErrExit;
						  }
						  if(t_mo_hold.hold_typ[0]!='3')
					    {
						    *sum_amt=g_dd_mst.bal-*sum_amt;
					    }
						  
					}else
				 if(ac_seqn==0)
				 	{
              ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ",ac_id);
              if(ret==100){
                       sprintf(acErrMsg,"数据库无此记录");
                       WRITEMSG
                       goto ErrExit;
                    }
                    if(ret!=0){
                       sprintf(acErrMsg,"查询主表错误");
                       WRITEMSG
                       goto ErrExit;
                   }
                   
              if(*sum_amt>g_dd_mst.bal)
						  {
							  sprintf(acErrMsg,"冻结登记薄中金额大于帐户余额!");
			          WRITEMSG
			          strcpy (g_pub_tx.reply,"D103");
			          goto ErrExit;
						  }
						  /**sum_amt=g_td_mst.bal-*sum_amt;*/
						  if(t_mo_hold.hold_typ[0]!='3')
					    {
						    *sum_amt=g_dd_mst.bal-*sum_amt;
					    }
					}
		
		Mo_hold_Clo_Sel( );
	}
		
	return 0;
ErrExit:
	return -1;
}

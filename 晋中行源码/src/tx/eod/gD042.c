/*************************************************************
* 文 件 名: gD042.c
* 功能描述：客户经理业绩考核统计表
*
* 作    者: jack
* 完成日期: 2003年7月21日
*
* 修改记录：
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
#include "prdt_ac_id_c.h"
#include "cif_mger_rel_c.h"
#include "ass_mger_ach_stat_c.h"
#include "ln_reg_c.h"
#include "find_debug.h"
#include "com_branch_c.h"
static int page,total;
struct ass_mger_ach_stat_c ass_mger_ach_stat_tmp;
struct com_branch_c sComBranch;
int get_ratio_gylcmxb(int bli,int bll,char str[100]);
gD042()
{
	int ret = 0;
  int flag ; /* 1 插入　2 更新 */

  struct prdt_ac_id_c sPrdt_ac_id;
  struct cif_mger_rel_c sCif_mger_rel;
  struct ass_mger_ach_stat_c ass_mger_ach_stat_c;
	struct ln_reg_c sLn_reg;

  memset(&sPrdt_ac_id,0x0,sizeof(struct prdt_ac_id_c));
  memset(&sCif_mger_rel,0x0,sizeof(struct cif_mger_rel_c));
  memset(&ass_mger_ach_stat_c,0x0,sizeof(struct ass_mger_ach_stat_c));
	memset(&sLn_reg, 0x0, sizeof(struct ln_reg_c));

	ret=sql_begin(); /*打开事务*/
	if(ret != 0) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

  /**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	/*
    ret = Cif_mger_rel_Dec_Sel(g_pub_tx.reply,"canl_date > %ld and \
		crt_date <= %ld order by mang", g_pub_tx.tx_date,
		g_pub_tx.tx_date);
	*/
	ret = Cif_mger_rel_Dec_Sel(g_pub_tx.reply,"crt_date <= %ld order by mang", g_pub_tx.tx_date);
  if (ret != 0)
  {
      sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
      WRITEMSG
      goto ErrExit;
  }

  while(1)
  {
    ret = Cif_mger_rel_Fet_Sel(&sCif_mger_rel, g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"FETCH FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        break;
    }

    strcpy(ass_mger_ach_stat_c.mang_no,sCif_mger_rel.mang);
    ass_mger_ach_stat_c.date = g_pub_tx.tx_date;

		/* 查询产品帐号对照表 */
		ret = Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"cif_no=%ld", sCif_mger_rel.cif_no);
		if (ret != 0)
		{
			sprintf(acErrMsg, "产品帐号对照表中不存在该记录");
			WRITEMSG
			goto ErrExit;
		}

		while(1)
		{
			memset( &sPrdt_ac_id, 0x00, sizeof( struct prdt_ac_id_c ) );
			ret = Prdt_ac_id_Fet_Sel(&sPrdt_ac_id, g_pub_tx.reply);
			vtcp_log("33333333333333333333==[%d]",ret);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "FETCH FOR SELECT ERROR !! [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			else if (ret == 100)
			{
				break;
			}

      sprintf(acErrMsg,"ac_id=[%ld], ac_seqn = [%ld]",sPrdt_ac_id.ac_id,
            	    sPrdt_ac_id.ac_seqn);
      WRITEMSG

			/* 查询帐户主文件 */
			/*ret = pub_base_idseqn_file(sPrdt_ac_id.ac_id, sPrdt_ac_id.ac_seqn,
				sPrdt_ac_id.ac_id_type);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询帐户主文件失败!!");
				WRITEMSG
				goto ErrExit;
			}
			*/
			
			switch(atoi(sPrdt_ac_id.ac_id_type))
	{
		case 1:									/*1-活期*/
			MEMSET_DEBUG( &g_dd_mst, 0x00, sizeof(struct dd_mst_c) );
			ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst,
							"ac_id=%ld and ac_seqn=%d",sPrdt_ac_id.ac_id,sPrdt_ac_id.ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"活期主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W014");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W015");
				return 1;
			}
			break;
		case 2:									/*2-定期*/
			MEMSET_DEBUG( &g_td_mst, 0x00, sizeof(struct td_mst_c) );

			ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst,
							"ac_id=%ld and ac_seqn=%d",sPrdt_ac_id.ac_id,sPrdt_ac_id.ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"定期主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W016");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"查询定期主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W017");
				return 1;
			}
			break;
		case 3:									/*3-贷款*/
			MEMSET_DEBUG( &g_ln_mst, 0x00, sizeof(struct ln_mst_c) );

			ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst,
							"ac_id=%ld and ac_seqn=%d",sPrdt_ac_id.ac_id,sPrdt_ac_id.ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"贷款主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W018");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"查询贷款主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W019");
				return 1;
			}
			break;
		case 4:									/*4-透支*/
			MEMSET_DEBUG( &g_od_mst, 0x00, sizeof(struct od_mst_c) );

			ret = Od_mst_Sel(g_pub_tx.reply, &g_od_mst,
							"ac_id=%ld and ac_seqn=%d",sPrdt_ac_id.ac_id,sPrdt_ac_id.ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"透支主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W020");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"查询透支主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W021");
				return 1;
			}
			break;
		case 9:									/*5-内部产品*/
			MEMSET_DEBUG( &g_in_mst, 0x00, sizeof(struct in_mst_c) );
			ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst,
							"ac_id=%ld and ac_seqn=%d",sPrdt_ac_id.ac_id,sPrdt_ac_id.ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"内部产品主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W022");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"查询内部主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W023");
				return 1;
			}
			break;
		default:
			sprintf( acErrMsg, "帐户类型错" );
			WRITEMSG
			return 1;
	}
			if (sPrdt_ac_id.ac_id_type[0] == '3') /* 贷款 */
			{
        		strcpy(ass_mger_ach_stat_c.br_no,g_ln_mst.opn_br_no);
			}
			else if (sPrdt_ac_id.ac_id_type[0] == '1')
			{
        		strcpy(ass_mger_ach_stat_c.br_no,g_dd_mst.opn_br_no);
			}
			else if (sPrdt_ac_id.ac_id_type[0] == '2')
			{
        		strcpy(ass_mger_ach_stat_c.br_no,g_td_mst.opn_br_no);
			}
			else if (sPrdt_ac_id.ac_id_type[0] == '4')
			{
        		strcpy(ass_mger_ach_stat_c.br_no,g_od_mst.opn_br_no);
			}
			else
			{
				sprintf(acErrMsg, "该账户的账户类型错误!! [%s]", 
					sPrdt_ac_id.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O238");
				goto ErrExit;
			}

      ret = Ass_mger_ach_stat_Sel(g_pub_tx.reply,&ass_mger_ach_stat_c,
            	" mang_no='%s' and br_no='%s' and \"date\"=%ld ", 
             	ass_mger_ach_stat_c.mang_no, ass_mger_ach_stat_c.br_no, 
              	ass_mger_ach_stat_c.date);
    	if (ret != 0 && ret != 100)
    	{
        	sprintf(acErrMsg,"查询客户经理业绩考核统计表错误!!");
        	WRITEMSG
        	goto ErrExit;
    	}
    	else if (ret == 100)
    	{
        	sprintf(acErrMsg,"客户经理业绩考核统计表中插入一条记录!!");
        	WRITEMSG
        	flag = 1; /* 插入 */
    	}
    	else
    	{
        	sprintf(acErrMsg,"更新客户经理业绩考核统计表中的记录");
        	WRITEMSG
        	flag = 2; /* 更新 */
    	}

			if (sPrdt_ac_id.ac_id_type[0] == '3') /* 贷款 */
			{
				if (flag == 1)
					ass_mger_ach_stat_c.intst_pay_intst = 0.00;

				ret = Ln_reg_Dec_Sel(g_pub_tx.reply, "ac_id=%ld and \
					ac_seqn=%ld order by ac_id, ac_seqn", g_ln_mst.ac_id,
					g_ln_mst.ac_seqn);
				if (ret != 0)
				{
					sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				while(1)
				{
					ret = Ln_reg_Fet_Sel(&sLn_reg, g_pub_tx.reply);
					if (ret != 0 && ret != 100)
					{
						sprintf(acErrMsg, "FETCH FOR SELECT ERROR [%d]", ret);
						WRITEMSG
						goto ErrExit;
					}
					else
					{
						break;
					}
					ass_mger_ach_stat_c.intst_pay_intst += sLn_reg.pay_bal;
				}
				Ln_reg_Clo_Sel();

    		if (flag == 1)
    		{
        		ass_mger_ach_stat_c.save_amt = 0.00;
        		ass_mger_ach_stat_c.ln_amt = g_ln_mst.bal;
        		ass_mger_ach_stat_c.ln_prvd_amt = g_ln_mst.ttl_prvd_amt;
        		if (g_ln_mst.ac_sts[0] == '1')
        		{
            		ass_mger_ach_stat_c.ln_norm_amt = g_ln_mst.bal;
        		}
        		else if (g_ln_mst.ac_sts[0] == '2')
        		{
         		   	ass_mger_ach_stat_c.ln_over_amt = g_ln_mst.bal;
        		}
        		else if (g_ln_mst.ac_sts[0] == '3')
        		{
            		ass_mger_ach_stat_c.ln_stgn_amt = g_ln_mst.bal;
        		}
        		else if (g_ln_mst.ac_sts[0] == '4')
        		{	
            		ass_mger_ach_stat_c.ln_die_amt = g_ln_mst.bal;
        		}
        		else
        		{
            		ass_mger_ach_stat_c.ln_norm_amt = 0.00;
            		ass_mger_ach_stat_c.ln_over_amt = 0.00;
            		ass_mger_ach_stat_c.ln_stgn_amt = 0.00;
            		ass_mger_ach_stat_c.ln_die_amt = 0.00;
        		}

 	          ass_mger_ach_stat_c.intst_pay_intst = 0.00;
            ass_mger_ach_stat_c.intst_lo_intst = \
						g_ln_mst.in_lo_intst + g_ln_mst.out_lo_intst + \
						g_ln_mst.cmpd_lo_intst;

            ret = Ass_mger_ach_stat_Ins(ass_mger_ach_stat_c,
						g_pub_tx.reply);
        		if (ret != 0)
        		{
            		sprintf(acErrMsg,"向客户经理业绩考核统计表中\
					插入数据错误!!");
            		WRITEMSG
            		goto ErrExit;
        		}
 		    }
    		else
    		{
        		ass_mger_ach_stat_c.ln_amt += g_ln_mst.bal;
        		ass_mger_ach_stat_c.ln_prvd_amt += g_ln_mst.ttl_prvd_amt;
        		if (g_ln_mst.ac_sts[0] == '1')
        		{
            		ass_mger_ach_stat_c.ln_norm_amt += g_ln_mst.bal;
        		}
        		else if (g_ln_mst.ac_sts[0] == '2')
        		{
            		ass_mger_ach_stat_c.ln_over_amt += g_ln_mst.bal;
        		}
        		else if (g_ln_mst.ac_sts[0] == '3')
        		{
            		ass_mger_ach_stat_c.ln_stgn_amt += g_ln_mst.bal;
        		}
        		else if (g_ln_mst.ac_sts[0] == '4')
        		{
            		ass_mger_ach_stat_c.ln_die_amt += g_ln_mst.bal;
        		}

        		ass_mger_ach_stat_c.intst_lo_intst += \
						g_ln_mst.in_lo_intst + g_ln_mst.out_lo_intst + \
						g_ln_mst.cmpd_lo_intst;

            ret = pub_ass_mger_ach_stat_upd(ass_mger_ach_stat_c,
						g_pub_tx.reply);
        		if (ret != 0)
        		{
            		sprintf(acErrMsg,"插入或更新揽存人业务量统计表错误!!");
            		WRITEMSG
            		goto ErrExit;
        		}
        }
			}
			else if (sPrdt_ac_id.ac_id_type[0] == '1' ||
				sPrdt_ac_id.ac_id_type[0] == '2' ||
				sPrdt_ac_id.ac_id_type[0] == '4')
			{
				/* 储蓄类 */
				if (flag == 1)
				{
					if (sPrdt_ac_id.ac_id_type[0] == '1')
					{
						vtcp_log("111111111111111");
						/* 活期 */
        		strcpy(ass_mger_ach_stat_c.br_no, g_dd_mst.opn_br_no);
						ass_mger_ach_stat_c.save_amt = g_dd_mst.bal; 
					}
					else if (sPrdt_ac_id.ac_id_type[0] == '2')
					{
						/* 定期 */
        		strcpy(ass_mger_ach_stat_c.br_no, g_td_mst.opn_br_no);
						ass_mger_ach_stat_c.save_amt = g_td_mst.bal;
					}
					else if (sPrdt_ac_id.ac_id_type[0] == '3')
					{
						/* 透支 */
        		strcpy(ass_mger_ach_stat_c.br_no, g_od_mst.opn_br_no);
						ass_mger_ach_stat_c.save_amt = g_od_mst.bal;
					}
					vtcp_log("22222222222222222");
					ass_mger_ach_stat_c.ln_amt = 0.00; 
					ass_mger_ach_stat_c.ln_prvd_amt = 0.00; 
					ass_mger_ach_stat_c.ln_norm_amt = 0.00; 
					ass_mger_ach_stat_c.ln_over_amt = 0.00; 
					ass_mger_ach_stat_c.ln_stgn_amt = 0.00; 
					ass_mger_ach_stat_c.ln_die_amt = 0.00; 
					ass_mger_ach_stat_c.intst_pay_intst = 0.00; 
					ass_mger_ach_stat_c.intst_lo_intst = 0.00; 
					vtcp_log("33333333333333333333");
					ret = Ass_mger_ach_stat_Ins(ass_mger_ach_stat_c,
							g_pub_tx.reply); 
							
					if (ret != 0) 
					{ vtcp_log("4444444444444444");
						sprintf(acErrMsg,"向客户经理业绩考核统计表中\
							插入数据错误!!"); 
						WRITEMSG 
						goto ErrExit;
					}
				}
				else
				{
					if (sPrdt_ac_id.ac_id_type[0] == '1')
					{
						ass_mger_ach_stat_c.save_amt += g_dd_mst.bal; 
					}
					else if (sPrdt_ac_id.ac_id_type[0] == '2')
					{
						ass_mger_ach_stat_c.save_amt += g_td_mst.bal;
					}
					else if (sPrdt_ac_id.ac_id_type[0] == '4')
					{
						ass_mger_ach_stat_c.save_amt += g_od_mst.bal;
					}

					ret = pub_ass_mger_ach_stat_upd(ass_mger_ach_stat_c,
						g_pub_tx.reply); 
					if (ret != 0) 
					{ 
						sprintf(acErrMsg,"插入或更新客户经理业绩考核\
							统计表错误!!"); 
						WRITEMSG 
						goto ErrExit;
					}
				}
			}	
		}
		Prdt_ac_id_Clo_Sel();
    }
	Cif_mger_rel_Clo_Sel();
rpt042();
    /* 修改存款总余额 
    ret = pub_ass_mger_upd_saveamt();
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新客户经理业绩考核统计表错误!!");
        WRITEMSG
        goto ErrExit;
    }*/

    /* 修改已还利息额 *
    ret = pub_ass_mger_upd_payintst();
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新客户经理业绩考核统计表错误!!");
        WRITEMSG
        goto ErrExit;
    }
	***/

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"客户经理业绩考核统计表统计成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    Trace_log_Clo_Sel();
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"客户经理业绩考核统计表统计失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*
gD042生成数据库记录后，在此处生成报表文件GD042.txt
*/
int rpt042()
{
		int line, opncnt;
		int hy_line;
		char vpfm[21];
		char fname[10];
		char pre_br_no[6];

		FILE *fp; 
		
  	memset(&ass_mger_ach_stat_tmp,0x0,sizeof(struct ass_mger_ach_stat_c));
		memset( pre_br_no, 0x00, sizeof( pre_br_no ) );
	
	  g_reply_int = sql_begin(); /*打开事务*/
	  if( g_reply_int != 0 )
	  {
	      sprintf( acErrMsg, "BEGIN WORK error!!!" );
	      WRITEMSG
	      return 1;
	  }
	  /**------- 初始化全局变量 --------**/
	  pub_base_sysinit();
	 
		strcpy( fname, "GD042");
		g_reply_int = pub_rpt_rmfile_all( fname );
		if ( g_reply_int != 0)
		{
			sprintf(acErrMsg,"删除柜客户经理业绩考核统计表错误!!");
			WRITEMSG
			return 1;
		}

		strcpy(vpfm, "GD042");

		/*g_reply_int = Com_branch_Dec_Sel(g_pub_tx.reply, " br_type not in('0','6') and wrk_sts!='*' ORDER BY br_no" );*/
		g_reply_int = Com_branch_Dec_Sel(g_pub_tx.reply, " br_type not in('0','6','7') and wrk_sts!='*' ORDER BY br_no" );
		if( g_reply_int ) 
		{ 
			sprintf(acErrMsg, "DECLARE com_branch CUR error![%d]", g_reply_int ); 
			WRITEMSG 
			return 1; 
		} 

		while(1) 
		{ 
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int && g_reply_int != 100 )
			{
				sprintf( acErrMsg,"FETCH com_branch CUR error![%d]", g_reply_int );
				WRITEMSG
				return 1;
			}
			else if( g_reply_int == 100 )
			{			
				g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt, get_ratio_gylcmxb );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					WRITEMSG
					return 1;
				}
				fclose( fp );
				break;
			}
			vtcp_log("sComBranch.br_no===[%s]",sComBranch.br_no);
			vtcp_log("sComBranch.br_no===[%s]",sComBranch.br_name);
			vtcp_log("pre_br_no==[%s]",pre_br_no);
			/*机构号变,下一个文件*/
			if( strcmp( sComBranch.br_no, pre_br_no ) )
			{
				/*不是第一个机构,打上个机构的结尾*/
				if( strlen( pre_br_no ) )
				{
					g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_gylcmxb);
					if( g_reply_int )
					{
						sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
						WRITEMSG
						return 1;
					}
					fclose( fp );
				}
				page = 1;
	
				g_reply_int = pub_rpt_openfile( &fp, sComBranch.br_no , fname );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
					WRITEMSG
					return 1;
				}

				hy_line = 0;
				
				if( page != 1 )
				{
					fprintf ( fp , "\f" );
				}
				g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_gylcmxb );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
					WRITEMSG
					return 1;
				}
	
				strcpy( pre_br_no, sComBranch.br_no );
			}

			g_reply_int = Ass_mger_ach_stat_Dec_Sel(g_pub_tx.reply, "\"date\"=%ld and br_no='%s' order by br_no" ,g_pub_tx.tx_date,sComBranch.br_no );
			if( g_reply_int ) 
			{ 
				sprintf(acErrMsg, "DECLARE com_branch CUR error![%d]", g_reply_int ); 
				WRITEMSG 
				return 1;
			} 

			while(1) 
			{ 
				g_reply_int = Ass_mger_ach_stat_Fet_Sel( &ass_mger_ach_stat_tmp, g_pub_tx.reply );
				if( g_reply_int && g_reply_int != 100 )
				{
					sprintf( acErrMsg,"FETCH com_branch CUR error![%d]", g_reply_int );
					WRITEMSG
					return 1;
				}
				else if( g_reply_int == 100 )
				{
					break;
				}
				g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_gylcmxb );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
					WRITEMSG
					return 1;
				}
				hy_line++;					
				if( hy_line % 25 == 0 )
				{
					g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_gylcmxb);
					if( g_reply_int )
					{
						sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
						WRITEMSG
						return 1;
					}
					hy_line = 0;
					page++;						
					if( page != 1 )
					{
						fprintf ( fp , "\f" );
					}
					g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_gylcmxb);
					if( g_reply_int )
					{
						sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
						WRITEMSG
						return 1;
					}
				}
				continue;				
			}
			
		}
}

int get_ratio_gylcmxb( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 

	char amt_tmp[21];
	
	switch( bli ) 
	{ 
		case 'X': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, g_pub_tx.tx_date/10000 ); 
			break; 
		case 'W': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, g_pub_tx.tx_date/100%100 ); 
			break; 
		case 'Y': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, g_pub_tx.tx_date%100 ); 
			break; 
		case 'U': 
			sprintf( str, "%02ld", page ); 
			break; 
		case 'A': 
			sprintf( vstr, "%s", ass_mger_ach_stat_tmp.mang_no );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'B':
			vtcp_log("11111111111111==[%f]",ass_mger_ach_stat_tmp.save_amt);
			sprintf( amt_tmp, "%.2lf" , ass_mger_ach_stat_tmp.save_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'C': 
			sprintf( amt_tmp, "%.2lf" , ass_mger_ach_stat_tmp.ln_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp );
			break; 
		case 'D': 
			sprintf( amt_tmp, "%.2lf" , ass_mger_ach_stat_tmp.ln_prvd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'T': 
			vtcp_log("yyyyyyyyyyyyyyyyyyyy===[%s]",sComBranch.br_name);
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s", sComBranch.br_name );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
	return 0; 
}

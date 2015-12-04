/*************************************************
* 文 件 名:  spE610.c
* 功能描述： 打印柜员流水
*
* 作    者:  andy
* 完成日期： 2004-07-06
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "trace_log_rz_c.h"
#include "com_item_c.h"
#include "com_tel_c.h"
#include "menu_c.h"
#include "prdt_ac_id_c.h"
#include "dc_acc_rel_c.h"
#include "tx_def_c.h"
#include "dd_mst_c.h"
#include "ln_mst_c.h"
#include "td_mst_c.h"
#include "in_mst_c.h"
#include "dc_log_c.h"
int spE610()
{
	char cFileName[100];
	char filename[100];
	char tel[5];
	char stat[20];
	char ac_name[61];
	char ct_ind[5];
	char dc_ind[3];
	char dc_code[5];
	double dTx_amt=0.00;
		
	int ret=0;
	int cnt=0;
							
	FILE *fp;

        struct trace_log_c sTrace_log; 
        struct dc_log_c dc_log;
	struct com_item_c sCom_item;
	struct com_tel_c sCom_tel;
	/**struct menu_c sMenu;**/
	struct tx_def_c  sTx_def;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct dc_acc_rel_c sDc_acc_rel;
	struct dd_mst_c s_dd_mst;
        struct ln_mst_c s_ln_mst;
        struct td_mst_c s_td_mst;
        struct in_mst_c s_in_mst;
		char tmp_sts[5];

        get_zd_long("0460",&sTrace_log.tx_date );
	memset(tel, 0x0, sizeof(tel));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(stat, 0x0, sizeof(stat));
	memset(ac_name, 0x0, sizeof(ac_name));
	memset(&sCom_item, 0x0, sizeof(struct com_item_c));
	memset(&sCom_tel, 0x0, sizeof(struct com_tel_c));
	memset(ct_ind, 0x0, sizeof(ct_ind));
	/**memset(&sMenu, 0x0, sizeof(struct menu_c));**/
	memset(&sTx_def,0x0, sizeof(struct tx_def_c));
	memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
	memset(&sDc_acc_rel, 0x0, sizeof(struct dc_acc_rel_c));
	memset(dc_code, 0x0, sizeof(dc_code));
	memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&s_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&s_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&s_in_mst,0x00,sizeof(struct in_mst_c));

	pub_base_sysinit();

	get_zd_data("0920",tel );

	pub_base_strpack( tel );

	/* 检查柜员号是否存在 */
	ret = Com_tel_Sel(g_pub_tx.reply,&sCom_tel,"tel='%s'",tel);
	if (ret==100)
	{
		sprintf(acErrMsg,"此柜员号不存在![%s]",tel);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O120" );			
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"读取柜员信息表异常出错!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O221" );			
		goto ErrExit;
	}									

	ret = pub_base_AllDwnFilName( cFileName ); 
	if (ret != 0) 
	{ 
		sprintf( acErrMsg, "生成文件名错错误!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"D126"); 
		goto ErrExit; 
	} 
	
	sprintf(acErrMsg,"filename=[%s]",cFileName); 
	WRITEMSG 
	
	fp = fopen( cFileName, "w" ); 
	if( fp==NULL ) 
	{ 
		sprintf( acErrMsg, "临时文件错误!" ); 
		WRITEMSG 
		strcpy( g_pub_tx.reply, "B080" ); 
		goto ErrExit; 
	}

    vtcp_log("!!! sTrace_log.tx_date is [%d],g_pub_tx.tx_date is [%d]\n",sTrace_log.tx_date,g_pub_tx.tx_date);
	if (sTrace_log.tx_date != g_pub_tx.tx_date)
	ret = Trace_log_rz_Dec_Sel( g_pub_tx.reply, "tel='%s' and tx_date=%ld and \
		no_show!='1' order by tx_date, trace_no, trace_cnt", 
		tel, sTrace_log.tx_date);
	else
	ret = Trace_log_Dec_Sel( g_pub_tx.reply, "tel='%s' and tx_date=%ld and \
		no_show!='1'  order by tx_date, trace_no, trace_cnt", 
		tel, sTrace_log.tx_date);

	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
		/**修改 序号 为 序 20070622*/
   fprintf(fp,"~         %s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","流水号","笔次", "交易名称", "账号","序","户    名",
		"金额", "现转", "借贷","状态" );
          		
	while(1)
	{
		if (sTrace_log.tx_date != g_pub_tx.tx_date)
		ret = Trace_log_rz_Fet_Sel(&sTrace_log, g_pub_tx.reply );
		else
		ret = Trace_log_Fet_Sel(&sTrace_log, g_pub_tx.reply );
		
		if( ret == 100 ) 
		{
			if (cnt == 0)
			{
				sprintf(acErrMsg, "该柜员没有流水!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "D226");
				goto ErrExit;
			}
			break;
		}
		else if (ret != 0)
		{
			sprintf(acErrMsg, "FETCH ERROR !! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
                if( memcmp(sTrace_log.tx_code,"2204",4)==0 && sTrace_log.trace_cnt==1) /*add by martin 20090423 修改去掉利息税的打印单独分录*/
                {
                 if(memcmp(sTrace_log.brf,"计息1",5)==0)
                  {
                    continue;
                  }
                }

		
		memset(ac_name, 0x0, sizeof(ac_name));
		/*in_mst*/ 
     	ret=In_mst_Sel( g_pub_tx.reply, &s_in_mst , "ac_id='%ld' and ac_seqn='%ld' ",sTrace_log.ac_id,sTrace_log.ac_seqn);
     		if(ret ==0)
     		{
     		  			pub_base_strpack( s_in_mst.name );
	        			strcpy(ac_name, s_in_mst.name);
     	  }
      /*td_mst*/
      ret=Td_mst_Sel( g_pub_tx.reply, &s_td_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",sTrace_log.ac_id,sTrace_log.ac_seqn);
    	 if(ret==0)
     		{
     		  pub_base_strpack( s_td_mst.name );
	        strcpy(ac_name, s_td_mst.name);
     	  } 
      /*dd_mst*/
      ret=Dd_mst_Sel( g_pub_tx.reply, &s_dd_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",sTrace_log.ac_id,sTrace_log.ac_seqn);
     		if(ret==0)
     		{
     		  pub_base_strpack( s_dd_mst.name );
	        strcpy(ac_name, s_dd_mst.name);
     	  }
      /*ln_mst*/
      ret=Ln_mst_Sel( g_pub_tx.reply, &s_ln_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",sTrace_log.ac_id,sTrace_log.ac_seqn);
    		if(ret==0)
     		{
     		  pub_base_strpack( s_ln_mst.name );
	        strcpy(ac_name, s_ln_mst.name);
     	  }
		pub_base_dic_show_str(ct_ind,"ct_ind",sTrace_log.ct_ind);
		memset(tmp_sts,'\0',sizeof(tmp_sts));
		pub_base_dic_show_str(tmp_sts,"log_sts",sTrace_log.sts);

		pub_base_strpack(sTrace_log.tx_code);

       	ret = Tx_def_Sel( g_pub_tx.reply, &sTx_def, "tx_code='%s'", sTrace_log.tx_code );

		memset(&dc_log,'\0',sizeof(dc_log));
		memset(dc_ind, 0x0, sizeof(dc_ind));
	if (sTrace_log.tx_date != g_pub_tx.tx_date)
		ret=Dc_log_rz_Sel( g_pub_tx.reply,&dc_log,"trace_no=%d and trace_cnt=%d and tx_date=%ld",sTrace_log.trace_no,sTrace_log.trace_cnt,sTrace_log.tx_date);
	else 
		ret=Dc_log_Sel( g_pub_tx.reply,&dc_log,"trace_no=%d and trace_cnt=%d",sTrace_log.trace_no,sTrace_log.trace_cnt);
		vtcp_log("1111111ind[%s]\n",dc_log.dc_ind);
            strcpy(sTrace_log.add_ind,dc_log.dc_ind);
		pub_base_dic_show_str(dc_ind,"trace_add_ind",sTrace_log.add_ind);

		/* 借贷的转化 *
		pub_base_strpack(sTrace_log.ac_no);
		if (strncmp(sTrace_log.ac_no, "91011", 5) == 0)
		{
			* 现金 *
			if (sTrace_log.add_ind[0] == '1') * 增 *
			{
				strcpy(dc_ind, "增");
			}
			else if (sTrace_log.add_ind[0] == '0') * 减 *
			{
				strcpy(dc_ind, "减");
			}
		}
		else if (strlen(sTrace_log.ac_no) <= 7)
		{
			memset(&sCom_item, 0x0, sizeof(struct com_item_c));
			ret = Com_item_Sel(g_pub_tx.reply, &sCom_item, "acc_no='%s'",
				sTrace_log.ac_no);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询科目表错误!! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "科目表中没有该科目!! [%s]", 
						sTrace_log.ac_no);
				WRITEMSG
			}
			if (sCom_item.dc_ind[0] == '2')
			{
				if (sTrace_log.add_ind[0] == '1')
				{
					strcpy(dc_ind, "减");
				}
				else if (sTrace_log.add_ind[0] == '0')
				{
					strcpy(dc_ind, "增");
				}
			}
			else
			{
				if (sTrace_log.add_ind[0] == '1')
				{
					strcpy(dc_ind, "增");
				}
				else if (sTrace_log.add_ind[0] == '0')
				{
					strcpy(dc_ind, "减");
				}
			}
		}
		else
		{
			* 查询产品账号对照表 *
			ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld and \
				ac_seqn=%d", sTrace_log.ac_id, sTrace_log.ac_seqn);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询产品账号对照表错误!! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "产品账号对照表中无此记录! ");
				WRITEMSG
			}
			else
			{
			if (sPrdt_ac_id.ac_id_type[0] == '1')
			{
				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'",
					sPrdt_ac_id.prdt_no);
				if (ret != 0 && ret != 100)
				{
					sprintf(acErrMsg, "查询活期产品表错误!! [%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				else if (ret == 100)
				{
					sprintf(acErrMsg, "dd_parm中不存在 [%s]", 
						sPrdt_ac_id.prdt_no);
					WRITEMSG
				}
				else
				{
					strcpy(dc_code, g_dd_parm.dc_code);
				}
			}
			else if (sPrdt_ac_id.ac_id_type[0] == '2')
			{
				ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'",
					sPrdt_ac_id.prdt_no);
				if (ret != 0 && ret != 100)
				{
					sprintf(acErrMsg, "查询定期产品表错误!! [%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				else if (ret == 100)
				{
					sprintf(acErrMsg, "td_parm中不存在 [%s]", 
						sPrdt_ac_id.prdt_no);
					WRITEMSG
				}
				else
				{
					strcpy(dc_code, g_td_parm.dc_code);
				}
			}
			else if (sPrdt_ac_id.ac_id_type[0] == '3')
			{
				ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'",
					sPrdt_ac_id.prdt_no);
				if (ret != 0 && ret != 100)
				{
					sprintf(acErrMsg, "查询贷款产品表错误!! [%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				else if (ret == 100)
				{
					sprintf(acErrMsg, "ln_parm中不存在[%s]",
						sPrdt_ac_id.prdt_no);
					WRITEMSG
				}
				else
				{
					strcpy(dc_code, g_ln_parm.dc_code);
				}
			}
			else if (sPrdt_ac_id.ac_id_type[0] == '4')
			{
				ret = Od_parm_Sel(g_pub_tx.reply, &g_od_parm, "prdt_no='%s'",
					sPrdt_ac_id.prdt_no);
				if (ret != 0 && ret != 100)
				{
					sprintf(acErrMsg, "查询透支产品表错误!! [%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				else if (ret == 100)
				{
					sprintf(acErrMsg, "od_parm不存在 [%s]", 
						sPrdt_ac_id.prdt_no);
					WRITEMSG
				}
				else
				{
					strcpy(dc_code, g_od_parm.dc_code);
				}
			}
			else if (sPrdt_ac_id.ac_id_type[0] == '9')
			{
				ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm, "prdt_no='%s'",
					sPrdt_ac_id.prdt_no);
				if (ret != 0 && ret != 100)
				{
					sprintf(acErrMsg, "查询透支产品表错误!! [%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				else if (ret == 100)
				{
					sprintf(acErrMsg, "in_parm中不存在[%s]", 
						sPrdt_ac_id.prdt_no);
					WRITEMSG
				}
				else
				{
					strcpy(dc_code, g_in_parm.dc_code);
				}
			}
			else 
			{
				sprintf(acErrMsg, "账号类型错误!! [%s]", 
						sPrdt_ac_id.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply, "P410");
				goto ErrExit;
			}
			ret = Dc_acc_rel_Sel(g_pub_tx.reply, &sDc_acc_rel, "dc_code='%s'",
				dc_code);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询会计代码对应科目表错误!! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "会计代码对应科目表中不存在[%s]", dc_code);
				WRITEMSG
			}

			memset(&sCom_item, 0x0, sizeof(struct com_item_c));
			ret = Com_item_Sel(g_pub_tx.reply, &sCom_item, "acc_hrt='%s'",
				sDc_acc_rel.acc_hrt);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询科目表错误!! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "科目表中没有该科目!! [%s]", 
						sTrace_log.ac_no);
				WRITEMSG
			}
			else 
			{
				if (sCom_item.dc_ind[0] == '2')
				{
					if (sTrace_log.add_ind[0] == '1')
					{
						strcpy(dc_ind, "减");
					}
					else if (sTrace_log.add_ind[0] == '0')
					{
						strcpy(dc_ind, "增");
					}
				}
				else
				{
					if (sTrace_log.add_ind[0] == '1')
					{
						strcpy(dc_ind, "增");
					}
					else if (sTrace_log.add_ind[0] == '0')
					{
						strcpy(dc_ind, "减");
					}
				}
			}
			}
		}
*/
    /*add by martin 2009/4/24 17:09:16 修改为打印最大金额*/
    if(sTrace_log.amt==0.00)
    	{
    		dTx_amt=dc_log.amt;
    	}else
    		{
    			dTx_amt=sTrace_log.amt;
    		}
		cnt ++;
				/**修改16.2为14.2 20070622*/
		/*fupengfei 2014/3/27 9:15:50 开封修改*/
		if(!strcmp(sTrace_log.tx_code,"7021")||!strcmp(sTrace_log.tx_code,"6236")||!strcmp(sTrace_log.tx_code,"7023")||!strcmp(sTrace_log.tx_code,"7022")||!strcmp(sTrace_log.tx_code,"6247")||!strcmp(sTrace_log.tx_code,"7024")||!strcmp(sTrace_log.tx_code,"8995"))
		{
			fprintf(fp, "         %ld|%ld|%s|%s|%d|%s|%14.2lf|%s|%s|%s|\n", sTrace_log.trace_no,sTrace_log.trace_cnt,
				sTrace_log.brf, sTrace_log.ac_no, sTrace_log.ac_seqn,ac_name, 
				dTx_amt, ct_ind, dc_ind,tmp_sts);
		}
		else
		{
			fprintf(fp, "         %ld|%ld|%s|%s|%d|%s|%14.2lf|%s|%s|%s|\n", sTrace_log.trace_no,sTrace_log.trace_cnt,
				sTx_def.tx_name, sTrace_log.ac_no, sTrace_log.ac_seqn,ac_name, 
				dTx_amt, ct_ind, dc_ind,tmp_sts);
			/*sTrace_log.amt, ct_ind, dc_ind,tmp_sts); add by martin 20090423*/
		}
	}
	/***add by martin 070523笔数流水***/
	fprintf(fp,"   流水笔数:|%d",cnt);  
  if (sTrace_log.tx_date!=g_pub_tx.tx_date)
	Trace_log_rz_Clo_Sel( );
	else 
	Trace_log_Clo_Sel( );
	
	fclose(fp);
	set_zd_data( DC_FILE_SND,"1" );

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

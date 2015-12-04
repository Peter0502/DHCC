/***************************************************************
* 文 件 名: sp1906.c
* 功能描述：揽存账户详细信息查询
* 作    者: 武大为
* 完成日期：2014年6月14日
*
* 修改记录：
*	日    期:
*	修 改 人:
*	修改内容:
*
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('1906', '柜员揽存关系查询', '00000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '4', '2');

insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('1906', 0, '1906', '#$');

insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('1906', '柜员揽存关系查询', 'sp1906', '0', '0');
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "tel_save_rel_c.h"

sp1906()
{
	int count = 0;
	char filename[100];
	char tmpname[512],tmpstr[150];
	char flag[3],tmp_ind[21];
	char sNoteSts[31]={0};
	long open_date = 0; /**add zgf 20131031 增加开户日期**/
	int ret=0;
	FILE *fp;
	char ac_no[20];
	char tel[5];
	long cif_no=0;
	char q_type[2];
	
	char ac_name_tmp[61];
	char note_sts_tmp[10];
	char ac_sts_tmp[16];
	char prdt_name_tmp[30];
	long cif_no_tmp = 0;
	double bal_tmp = 0.00;
	char rel_type_tmp[12];
	char br_no[6];
	int num = 0;
	
	struct prdt_ac_id_c sPrdt_ac_id;
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct cif_id_code_rel_c	g_cif_id_code_rel;
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct cif_basic_inf_c	g_cif_basic_inf;
	struct tel_save_rel_c	g_tel_save_rel;


	memset(br_no,0x00,sizeof(br_no));
	memset(q_type,0x00,sizeof(q_type));
	memset(ac_no,0x00,sizeof(ac_no));
	memset(tel,0x00,sizeof(tel));
	memset(ac_name_tmp,0x00,sizeof(ac_name_tmp));
	memset(note_sts_tmp,0x00,sizeof(note_sts_tmp));
	memset(ac_sts_tmp,0x00,sizeof(ac_sts_tmp));
	memset(rel_type_tmp,0x00,sizeof(rel_type_tmp));	
	
	memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));

	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data( "0370", ac_no );		/* 揽存账号 */
	get_zd_data( "0920", tel );		/* 柜员代码 */
	get_zd_long( "0290", &cif_no ); 
	get_zd_data( "0690", q_type );

	if(q_type[0] == '2')
	{
			ret = Tel_save_rel_Dec_Sel( g_pub_tx.reply , "ac_no='%s' and br_no='%s' and sts='1'" , ac_no ,g_pub_tx.tx_br_no );
			if (ret)
			{
		 		sprintf(acErrMsg,"[DECLARE SELECT Tel_save_rel_Dec_Sel]异常!!");
		 		WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
		 		goto ErrExit;
			}
	}else if(q_type[0] == '3')
	{
			ret = Tel_save_rel_Dec_Sel( g_pub_tx.reply ,  "cif_no='%ld' and br_no='%s' and sts='1'" , cif_no ,g_pub_tx.tx_br_no  );
			if (ret)
			{
		 		sprintf(acErrMsg,"[DECLARE SELECT Tel_save_rel_Dec_Sel]异常!!");
		 		WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
		 		goto ErrExit;
			}
	}else if(q_type[0] == '1')
	{
			ret = Tel_save_rel_Dec_Sel( g_pub_tx.reply , "tel='%s'  and br_no='%s' and sts='1'" , tel ,g_pub_tx.tx_br_no );
			if (ret)
			{
		 		sprintf(acErrMsg,"[DECLARE SELECT Tel_save_rel_Dec_Sel]异常!!");
		 		WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
		 		goto ErrExit;
			}
	}

	/*打印开始*/
	pub_base_GenDwnFilName(filename);
	pub_base_strpack(filename);
	sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
	fp=fopen(tmpname,"w");

	/**显示列标题**/
 	fprintf(fp,"~揽存柜员|@揽存账户|@户名|@客户号|$账户余额|@产品|@揽存开始日期|@关联类型|帐户状态|介质状态|\n");
 	
	while(1)
	{
		
			memset(&g_tel_save_rel,0x00,sizeof(struct tel_save_rel_c));
			ret = Tel_save_rel_Fet_Sel(&g_tel_save_rel , g_pub_tx.reply  );
			if( ret==100 && count==0)
			{
				sprintf(acErrMsg,"没有找到匹配的纪录!!");
		 		WRITEMSG
				strcpy(g_pub_tx.reply,"O043");
		 		goto ErrExit;							
			}
			else if(ret==100){
				break;
			}else if(ret){
				sprintf(acErrMsg,"FETCT Tel_save_rel_Fet_Sel异常!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D102");
				goto ErrExit;
			}
			/* 如果不是自己揽存的无权利查询 */
			if(strcmp(g_tel_save_rel.tel ,g_pub_tx.tel ) != 0)
			{
				sprintf(acErrMsg,"揽存柜员g_tel_save_rel.tel=[%s],查询柜员是g_pub_tx.tel=[%s],不是同一柜员，不允许查询!",g_tel_save_rel.tel ,g_pub_tx.tel );
				WRITEMSG
				strcpy(g_pub_tx.reply,"Z10B");
				goto ErrExit;
			}
			/* 20140620 wudawei end */
			if(g_tel_save_rel.tx_type[0] == '1')
			{
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
									"ac_no='%s' ",g_tel_save_rel.ac_no);
				if(ret==100)
				{
					sprintf(acErrMsg,"无此帐号请检查!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"N062");
					continue;
				}
				ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ", g_mdm_ac_rel.ac_id);
        if (ret != 0 && ret != 100)
        {
                sprintf(acErrMsg,"查询产品帐号对照表错误!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
        }
        else if (ret == 100)
        {
                ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
                if ( ret != 0)
                {
                        sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]",
                                g_pub_tx.cur_no);
                        WRITEMSG
                        goto ErrExit;
                }
        }
        else
        {
                g_pub_tx.ac_seqn = sPrdt_ac_id.ac_seqn;
        }

				/*根据显示帐号和账号序号返回账户类型和主文件*/
				ret = pub_base_disac_file( g_mdm_ac_rel.ac_no, sPrdt_ac_id.ac_seqn, g_pub_tx.ac_type); 
				if (ret != 0) 
				{ 
					sprintf(acErrMsg,"查询账号错误!! "); 
					WRITEMSG 
					strcpy(g_pub_tx.reply,"D169"); 
					goto ErrExit; 
				}
				
				memset(tel,0x00,sizeof(tel));
				memset(ac_name_tmp,0x00,sizeof(ac_name_tmp));
				memset(note_sts_tmp,0x00,sizeof(note_sts_tmp));
				memset(ac_sts_tmp,0x00,sizeof(ac_sts_tmp));
				memset(rel_type_tmp,0x00,sizeof(rel_type_tmp));	
				sprintf(acErrMsg,"查询结果!! prdt_ac_id.ac_id=[%ld],prdt_ac_id.ac_seqn=[%d],g_pub_tx.ac_type=[%s]",
                                sPrdt_ac_id.ac_id,sPrdt_ac_id.ac_seqn,g_pub_tx.ac_type);
        WRITEMSG
				strcpy(rel_type_tmp, "关联帐号");
				/* note_sts	char(1)	介质状态	mdm_sts	0：正常；1：挂失；2：挂失换证；3：正常换证；4：已经部提；5-撤销；*：销户 */
				if(g_mdm_ac_rel.note_sts[0] == '0')
				{
					strcpy(note_sts_tmp, "正常");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '1')
				{
					strcpy(note_sts_tmp, "挂失");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '2')
				{
					strcpy(note_sts_tmp, "挂失换证");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '3')
				{
					strcpy(note_sts_tmp, "正常换证");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '4')
				{
					strcpy(note_sts_tmp, "已经部提");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '5')
				{
					strcpy(note_sts_tmp, "撤销");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '*')
				{
					strcpy(note_sts_tmp, "销户");
				}
				/* 介质状态 */
				if (atoi(g_pub_tx.ac_type) == 1) 
				{ 
					strcpy(ac_name_tmp ,g_dd_mst.name); 
					strcpy(br_no, g_dd_mst.opn_br_no);
					ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
					if(ret==100)
					{
						sprintf(acErrMsg,"活期产品不存在\n");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
			
					}
					strcpy(prdt_name_tmp, g_dd_parm.title);
					cif_no_tmp = g_dd_mst.cif_no;
					bal_tmp = g_dd_mst.bal;
					if(g_dd_mst.ac_sts[0] == '1')
					{
						strcpy(ac_sts_tmp,"正常");
					}
					else if(g_dd_mst.ac_sts[0] == '*')
					{
						strcpy(ac_sts_tmp,"销户");
					}
				} 
				else if (atoi(g_pub_tx.ac_type) == 2) 
				{ 
					strcpy(ac_name_tmp ,g_td_mst.name); 
					strcpy(br_no, g_td_mst.opn_br_no);
					ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, "prdt_no='%s'", g_td_mst.prdt_no);
					if(ret==100)
					{
						sprintf(acErrMsg,"定期产品不存在\n");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
			
					}
					strcpy(prdt_name_tmp, g_td_parm.title);
					cif_no_tmp = g_td_mst.cif_no;
					bal_tmp = g_td_mst.bal;
					if(g_td_mst.ac_sts[0] == '1')
					{
						strcpy(ac_sts_tmp,"正常");
					}
					else if(g_td_mst.ac_sts[0] == '*')
					{
						strcpy(ac_sts_tmp,"销户");
					}
				} 
				else if (atoi(g_pub_tx.ac_type) == 3) 
				{ 
					strcpy(ac_name_tmp ,g_ln_mst.name); 
					strcpy(br_no, g_ln_mst.opn_br_no);
					ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, "prdt_no='%s'", g_ln_mst.prdt_no);
					if(ret==100)
					{
						sprintf(acErrMsg,"贷款产品不存在\n");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
			
					}
					strcpy(prdt_name_tmp, g_ln_parm.title);
					cif_no_tmp = g_ln_mst.cif_no;
					bal_tmp = g_ln_mst.bal;
					if(g_ln_mst.ac_sts[0] == '1')
					{
						strcpy(ac_sts_tmp,"正常");
					}
					else if(g_ln_mst.ac_sts[0] == '*')
					{
						strcpy(ac_sts_tmp,"销户");
					}
					else if(g_ln_mst.ac_sts[0] == '2')
					{
						strcpy(ac_sts_tmp,"逾期90天内");
					}
					else if(g_ln_mst.ac_sts[0] == '3')
					{
						strcpy(ac_sts_tmp,"呆滞");
					}
					else if(g_ln_mst.ac_sts[0] == '4')
					{
						strcpy(ac_sts_tmp,"呆账");
					}
					else if(g_ln_mst.ac_sts[0] == '5')
					{
						strcpy(ac_sts_tmp,"逾期90天外");
					}
				} 
				/* fprintf(fp,"~揽存柜员|@揽存账户|@户名|@客户号|$账户余额|@产品|@揽存开始日期|@关联类型|帐户状态|介质状态|\n"); */
				fprintf( fp, "%4s|%19s|%60s|%ld|%.2lf|%s|%ld|%s|%s|%s|\n", g_tel_save_rel.tel,g_tel_save_rel.ac_no,ac_name_tmp,cif_no_tmp,bal_tmp,prdt_name_tmp,g_tel_save_rel.tx_date,rel_type_tmp,ac_sts_tmp,note_sts_tmp );
			}
			/* 查询关联客户号的 */
			else if(g_tel_save_rel.tx_type[0] == '2')
			{
					ret=Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "cif_no=%ld", \
								   g_tel_save_rel.cif_no);
					if (ret)
					{
					 		sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id]异常!!");
					 		WRITEMSG
							strcpy(g_pub_tx.reply,"D101");
					 		goto ErrExit;
					}
					while(1)
					{
							ret=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
							if(ret==100)break;
							else if(ret)
							{
									sprintf(acErrMsg,"FETCT 产品帐号对照表异常!!");
									WRITEMSG
									strcpy(g_pub_tx.reply,"D102");
									goto ErrExit;
							}
							
							memset(tel,0x00,sizeof(tel));
							memset(ac_name_tmp,0x00,sizeof(ac_name_tmp));
							memset(note_sts_tmp,0x00,sizeof(note_sts_tmp));
							memset(ac_sts_tmp,0x00,sizeof(ac_sts_tmp));
							memset(rel_type_tmp,0x00,sizeof(rel_type_tmp));	
							
							ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
									"ac_id=%ld and opn_br_no='%s' and note_sts not in ('2','3','4')",g_prdt_ac_id.ac_id,g_pub_tx.tx_br_no);
							if(ret==100)
							{
								sprintf(acErrMsg,"无此帐号请检查!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");
								continue;
							}
							num = sql_count( "tel_save_rel" , " ac_no='%s' and sts='1' " , g_mdm_ac_rel.ac_no );
							sprintf(acErrMsg, "关联客户号的 如果账户在关联表里有 就跳过 ******* tel_save_rel_num = [%d]", num);
							WRITEMSG
							if(num > 0)
							{
								sprintf(acErrMsg,"此帐号已经做过关联，跳过!!");
								WRITEMSG
								continue;
							}
							strcpy(rel_type_tmp, "关联客户号");
							/* note_sts	char(1)	介质状态	mdm_sts	0：正常；1：挂失；2：挂失换证；3：正常换证；4：已经部提；5-撤销；*：销户 */
							if(g_mdm_ac_rel.note_sts[0] == '0')
							{
								strcpy(note_sts_tmp, "正常");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '1')
							{
								strcpy(note_sts_tmp, "挂失");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '2')
							{
								strcpy(note_sts_tmp, "挂失换证");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '3')
							{
								strcpy(note_sts_tmp, "正常换证");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '4')
							{
								strcpy(note_sts_tmp, "已经部提");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '5')
							{
								strcpy(note_sts_tmp, "撤销");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '*')
							{
								strcpy(note_sts_tmp, "销户");
							}
							/* 介质状态 */
							switch(g_prdt_ac_id.ac_id_type[0])
							{
								case '1':
										ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
											"ac_id=%ld and ac_seqn=%d",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
										if(ret==100)
										{
											sprintf(acErrMsg,"活期明细文件不存在该记录!!");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
											break;
										}
						
										strcpy(ac_name_tmp ,g_dd_mst.name); 
										strcpy(br_no, g_dd_mst.opn_br_no);
										ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
										if(ret==100)
										{
											sprintf(acErrMsg,"活期产品不存在\n");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
								
										}
										strcpy(prdt_name_tmp, g_dd_parm.title);
										cif_no_tmp = g_dd_mst.cif_no;
										bal_tmp = g_dd_mst.bal;
										if(g_dd_mst.ac_sts[0] == '1')
										{
											strcpy(ac_sts_tmp,"正常");
										}
										else if(g_dd_mst.ac_sts[0] == '*')
										{
											strcpy(ac_sts_tmp,"销户");
										}
										break;
								case '2':
										ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
											"ac_id=%ld and ac_seqn=%d",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
										if(ret==100)
										{
											sprintf(acErrMsg,"定期文件不存在该记录!!");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
											break;
										}
										strcpy(ac_name_tmp ,g_td_mst.name); 
										strcpy(br_no, g_td_mst.opn_br_no);
										ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, "prdt_no='%s'", g_td_mst.prdt_no);
										if(ret==100)
										{
											sprintf(acErrMsg,"定期产品不存在\n");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
								
										}
										strcpy(prdt_name_tmp, g_td_parm.title);
										cif_no_tmp = g_td_mst.cif_no;
										bal_tmp = g_td_mst.bal;
										if(g_td_mst.ac_sts[0] == '1')
										{
											strcpy(ac_sts_tmp,"正常");
										}
										else if(g_td_mst.ac_sts[0] == '*')
										{
											strcpy(ac_sts_tmp,"销户");
										}
										break;
								case '3':
										ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, \
											"ac_id=%ld and ac_seqn=%d",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
										if(ret==100)
										{
											sprintf(acErrMsg,"贷款文件不存在该记录!!");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
											break;
										}
										strcpy(ac_name_tmp ,g_ln_mst.name); 
										strcpy(br_no, g_ln_mst.opn_br_no);
										ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, "prdt_no='%s'", g_ln_mst.prdt_no);
										if(ret==100)
										{
											sprintf(acErrMsg,"贷款产品不存在\n");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
								
										}
										strcpy(prdt_name_tmp, g_ln_parm.title);
										cif_no_tmp = g_ln_mst.cif_no;
										bal_tmp = g_ln_mst.bal;
										if(g_ln_mst.ac_sts[0] == '1')
										{
											strcpy(ac_sts_tmp,"正常");
										}
										else if(g_ln_mst.ac_sts[0] == '*')
										{
											strcpy(ac_sts_tmp,"销户");
										}
										else if(g_ln_mst.ac_sts[0] == '2')
										{
											strcpy(ac_sts_tmp,"逾期90天内");
										}
										else if(g_ln_mst.ac_sts[0] == '3')
										{
											strcpy(ac_sts_tmp,"呆滞");
										}
										else if(g_ln_mst.ac_sts[0] == '4')
										{
											strcpy(ac_sts_tmp,"呆账");
										}
										else if(g_ln_mst.ac_sts[0] == '5')
										{
											strcpy(ac_sts_tmp,"逾期90天外");
										}
										break;
								default:
										break;
							}/*switch结束*/
							fprintf( fp, "%4s|%19s|%60s|%ld|%.2lf|%s|%ld|%s|%s|%s|\n", g_tel_save_rel.tel,g_mdm_ac_rel.ac_no ,ac_name_tmp,cif_no_tmp,bal_tmp,prdt_name_tmp,g_tel_save_rel.tx_date,rel_type_tmp,ac_sts_tmp,note_sts_tmp );
					}
					Prdt_ac_id_Clo_Sel ();
			}
			count++;
	}

	
	fclose(fp);
	Tel_save_rel_Clo_Sel ();
	
	set_zd_data(DC_FILE_SND,"1");

	

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"查询失败![%s]",g_pub_tx.reply);
	WRITEMSG
	fclose(fp);
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

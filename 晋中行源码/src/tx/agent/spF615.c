/*************************************************
* 文 件 名: spF610.c
* 功能描述：批量开户交易
*
* 作    者: z.s.wang
* 完成日期: 2012年03月13日
* 修改日期:
* 修 改 人:
* 修改内容:
*************************************************/

#define EXTERN
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "public.h"
#include "sub_dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "com_branch_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h"

spF615()
{
	int i = 0;
	int ret = -1 ;
	int str_flag = 0;
	long sub_ac_seqn = 0;
	long num = 0;
	long err_num = 0;
	long xq_num = 0;
	long lp_num = 0;
	long dy_num = 0;
	long fj_num = 0;
	double dRate_val = 0.0;
	char filename[128];
	char filename_e[128];
	char filename_r[128];
	char filename_bak[128];
	char tmp_filename_e[128];
	char tmp_filename_r[128];
	char tmp_msg[128];
	char tmpstr[1024];
	char strfld[20][128];
	char intst_type[2];
	char rate_type[2];
	char tmp_cmd[256];
	char str_jxlx[128];
	double open_amt = 0.0;
	char paper_type[2];
	char paper_no[21];
	char telephone[21];
	char bar_code[17];
	char cash_flag[2];
	char draw_ac_no[20];
	char draw_note_type[4];
	char draw_note_no[17];
	char draw_brf[21];
	char note_sts[2];
	char chk_flag[2];
	char tmp_ac_no[25];
	FILE *fp = NULL;
	FILE *fp_e = NULL;
	FILE *fp_r = NULL;
	struct sub_dd_mst_c s_sub_dd_mst;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	struct com_branch_c s_com_branch;
	struct dd_mst_c s_dd_mst;
	struct in_mst_c s_in_mst;

	memset(filename,0x00,sizeof(filename));
	memset(filename_e,0x00,sizeof(filename_e));
	memset(tmp_filename_e,0x00,sizeof(tmp_filename_e));
	memset(filename_r,0x00,sizeof(filename_r));
	memset(tmp_filename_r,0x00,sizeof(tmp_filename_r));
	memset(tmp_msg,0x00,sizeof(tmp_msg));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(strfld,0x00,sizeof(strfld));
	memset(intst_type,0x00,sizeof(intst_type));
	memset(rate_type,0x00,sizeof(rate_type));
	memset(str_jxlx,0x00,sizeof(str_jxlx));
	memset(paper_type,0x00,sizeof(paper_type));
	memset(paper_no,0x00,sizeof(paper_no));
	memset(telephone,0x00,sizeof(telephone));
	memset(bar_code,0x00,sizeof(bar_code));
	memset(cash_flag,0x00,sizeof(cash_flag));
	memset(draw_ac_no,0x00,sizeof(draw_ac_no));
	memset(draw_note_type,0x00,sizeof(draw_note_type));
	memset(draw_note_no,0x00,sizeof(draw_note_no));
	memset(draw_brf,0x00,sizeof(draw_brf));
	memset(note_sts,0x00,sizeof(note_sts));
	memset(chk_flag,0x00,sizeof(chk_flag));
	memset(tmp_ac_no,0x00,sizeof(tmp_ac_no));
	memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&s_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&s_in_mst,0x00,sizeof(struct in_mst_c));

	ret = sql_begin(); /* 打开事物 */
	if ( ret )
	{
		vtcp_log("打开事物失败! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT03");
		goto ErrExit;
	}

	pub_base_sysinit(); /* 初始化全局变量 */

	Gl_file_first = 0;
	pub_base_AllDwnFilName( filename );
	get_zd_data("0680",chk_flag);
	strcpy(filename_bak,filename);
	strcat(filename_bak,"_bak");
	/* 文件先转换,排序,还有去掉\r 再用备份的读取吧... */
	sprintf(tmp_cmd,"trans.sh %s",filename);
	ret=system(tmp_cmd);
	if(ret)
	{
		vtcp_log("处理文件出错! [%s] [%s] [%d]",tmp_cmd,__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	sprintf(tmp_cmd,"cp -f %s %s",filename,filename_bak);
	ret=system(tmp_cmd);
	if(ret)
	{
		vtcp_log("复制文件出错! [%s] [%s] [%d]",tmp_cmd,__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	vtcp_log("取得的文件名为[%s] [%s] [%d]",filename,__FILE__,__LINE__);

	sprintf(tmp_filename_e,"Err-%s-%s-%ld-%06d-subopen.txt",g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.tx_date,g_pub_tx.trace_no);
	sprintf( filename_e,"%s/%s",getenv("FILDIR"),tmp_filename_e);

	vtcp_log("错误数据的文件名为[%s] [%s] [%d]",filename_e,__FILE__,__LINE__);

	sprintf(tmp_filename_r,"Re-%s-%s-%ld-%06d-subopen.txt",g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.tx_date,g_pub_tx.trace_no);
	sprintf( filename_r,"%s/%s",getenv("FILDIR"),tmp_filename_r);

	vtcp_log("回显的文件名为[%s] [%s] [%d]",filename_r,__FILE__,__LINE__);

	fp = fopen(filename_bak,"r");
	if(fp == NULL)
	{
		vtcp_log("打开文件失败! [%s] [%s] [%d]",filename,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"L206");
		goto ErrExit;
	}

	fp_e = fopen(filename_e,"w");
	if(fp_e == NULL)
	{
		vtcp_log("打开文件失败! [%s] [%s] [%d]",filename_e,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"L206");
		goto ErrExit;
	}

	fp_r = fopen(filename_r,"w");
	if(fp_r == NULL)
	{
		vtcp_log("打开文件失败! [%s] [%s] [%d]",filename_r,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"L206");
		goto ErrExit;
	}

	/* --- 文件检查 --- */
	if(chk_flag[0]=='1')
	{
		while(1)
		{
			str_flag = 0;
			memset(tmpstr,0x00,sizeof(tmpstr));
			memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
			fgets(tmpstr,1024,fp);
			if(feof(fp))
			{
				vtcp_log("文件读取结束.[%s] [%d]",__FILE__,__LINE__);
				break;
			}

			memset(strfld,0x0,sizeof(strfld));
			for( i = 0;i<19;i++ )
			{
				ret = pub_base_cut_str( tmpstr,strfld[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,strfld[i]);
				pub_base_strpack( strfld[i] );
				if(strlen(strfld[i]) == 0 && i < 13)
				{
					str_flag = 1;
				}
			}

			/* 必输元素不可以为空 */
			if(str_flag == 1)
			{
				fprintf(fp_e,"%s|%s",tmpstr,"必须输入的元素不可以为空\n");
				err_num++;
				continue;
			}

			ret=spF_chk(strfld[4]);
			if(ret)
			{
				fprintf(fp_e,"%s|%s",tmpstr,"利率格式输入有误\n");
				err_num++;
				continue;
			}

			ret=spF_chk(strfld[9]);
			if(ret)
			{
				fprintf(fp_e,"%s|%s",tmpstr,"金额格式输入有误\n");
				err_num++;
				continue;
			}

			strcpy(g_pub_tx.ac_no,strfld[0]);
			/* sub_ac_seqn = atoi(strfld[1]); */
			strcpy(g_pub_tx.name,strfld[1]);
			strcpy(intst_type,strfld[2]);
			strcpy(rate_type,strfld[3]);
			sscanf(strfld[4],"%lf",&dRate_val);
			xq_num = atol(strfld[5]);
			lp_num = atol(strfld[6]);
			dy_num = atol(strfld[7]);
			fj_num = atol(strfld[8]);
			open_amt= atof(strfld[9]);
			strcpy(paper_type,strfld[10]);
			strcpy(paper_no,strfld[11]);
			strcpy(telephone,strfld[12]);
			strcpy(bar_code,strfld[13]);
			strcpy(cash_flag,strfld[14]);

			/* 如果开户金额不为0 则需要为后面的信息赋值 */

			if(open_amt > 0.001 && cash_flag[0]!='1')
			{
				fprintf(fp_e,"%s|%s",tmpstr,"现转标志错误 开户金额不为0 现转标志只能为1\n");
				err_num++;
				continue;
			}
			else if( cash_flag[0]=='1' && (strlen(strfld[15])==0 || strlen(strfld[16])==0
				|| strlen(strfld[17])==0 || strlen(strfld[18])==0 ))
			{
				fprintf(fp_e,"%s|%s",tmpstr,"开户金额不为0 现转标志之后的元素需要填入\n");
				err_num++;
				continue;
			}

			strcpy(draw_ac_no,strfld[15]);
			strcpy(draw_note_type,strfld[16]);
			strcpy(draw_note_no,strfld[17]);
			strcpy(draw_brf,strfld[18]);
			memset(draw_note_no,'0',sizeof(draw_note_no));
			strcpy(draw_note_no+(16-strlen(strfld[17])),strfld[17]);

			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no = '%s'",g_pub_tx.ac_no);
			if(ret)
			{
				vtcp_log("[%s][%d]查询 Mdm_ac_rel 错误 ,[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
				fprintf(fp_e,"%s|%s",tmpstr,"没有查到该主账户\n");
				err_num++;
				continue;
			}

			if(s_mdm_ac_rel.draw_pwd_chg[0]!= '1')
			{
				fprintf(fp_e,"%s|%s",tmpstr,"该主账户没有开通二级账户\n");
				err_num++;
				continue;
			}

			if(s_mdm_ac_rel.note_sts[0] != '0' )
			{
				fprintf(fp_e,"%s|%s",tmpstr,"该主账户状态不是正常\n");
				err_num++;
				continue;
			}

			if(strcmp(s_mdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no))
			{
				fprintf(fp_e,"%s|%s",tmpstr,"该主账户开户行不是本交易机构\n");
				err_num++;
				continue;
			}

			if(intst_type[0] != '0' && intst_type[0] != '4')
			{
				fprintf(fp_e,"%s|%s",tmpstr,"计息类型错误\n");
				err_num++;
				continue;
			}

			if(rate_type[0] != '0' && rate_type[0] != '1')
			{
				fprintf(fp_e,"%s|%s",tmpstr,"利率类型错误\n");
				err_num++;
				continue;
			}

			/* ret = sql_count("sub_dd_mst","ac_no = '%s' and sub_ac_seqn = ( select max(sub_ac_seqn) from sub_dd_mst where )",g_pub_tx.ac_no,sub_ac_seqn);
			if(ret)
			{
				fprintf(fp_e,"%s|%s",tmpstr,"该子序号已经被使用\n");
				err_num++;
				continue;
			} */

			ret = sql_count("sub_dd_mst","ac_no = '%s' and xq_num = %ld and lp_num = %ld and dy_num = %ld and fj_num = %ld",g_pub_tx.ac_no,xq_num,lp_num,dy_num,fj_num);
			if(ret)
			{
				fprintf(fp_e,"%s|%s",tmpstr,"该房间号已经被存在\n");
				err_num++;
				continue;
			}

			if(sub_ac_seqn == 0 || strcmp(g_pub_tx.ac_no,tmp_ac_no))
			{
				ret = sql_max_long_xier("sub_dd_mst","sub_ac_seqn",&sub_ac_seqn,"ac_no = '%s'",g_pub_tx.ac_no);
				if(ret==1403)
				{
					sub_ac_seqn = 0;
				}
				else if(ret)
				{
					fprintf(fp_e,"%s|%s",tmpstr,"查询子账户文件出错\n");
					err_num++;
					continue;
				}
				strcpy(tmp_ac_no,g_pub_tx.ac_no);
			}

			sub_ac_seqn++;
      vtcp_log("主账号是:[%s] 子账户序号为:[%ld] [%s] [%d]",g_pub_tx.ac_no,sub_ac_seqn,__FILE__,__LINE__);

			ret = sql_count("xqdz","xqdz100 = '%s' and xqdz101 = '%s' and xqdz30='1'",g_pub_tx.ac_no,strfld[5]);
			if(ret == 0)
			{
				fprintf(fp_e,"%s|%s",tmpstr,"没有此该主账户对应的小区编号或小区编号已被删除\n");
				err_num++;
				continue;
			}

			ret = sql_count("dic_data","pr_code='per_id' and pr_seq='%s'",paper_type);
			if(ret == 0)
			{
				fprintf(fp_e,"%s|%s",tmpstr,"证件类型错误\n");
				err_num++;
				continue;
			}

			if(paper_type[0] == '1' && strlen(paper_no) != 18)
			{
				fprintf(fp_e,"%s|%s",tmpstr,"身份证开户 证件号码需要为18位\n");
				err_num++;
				continue;
			}

			if(open_amt > 0.001)
			{
				if( draw_ac_no[0] != '9' && draw_ac_no[0] != '5' )
				{
					fprintf(fp_e,"%s|%s",tmpstr,"转出账户只允许为内部账或对公账户\n");
					err_num++;
					continue;
				}

				memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
				ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no = '%s'",draw_ac_no);
				if(ret)
				{
					fprintf(fp_e,"%s|%s",tmpstr,"查询内部账户错误\n");
					err_num++;
					continue;
				}

				if( draw_ac_no[0] == '9' )
				{
					ret = In_mst_Sel(g_pub_tx.reply,&s_in_mst,"ac_id = %ld",s_mdm_ac_rel.ac_id);
					if(ret)
					{
						fprintf(fp_e,"%s|%s",tmpstr,"查询内部账户错误\n");
						err_num++;
						continue;
					}

					if(open_amt > s_in_mst.bal)
					{
						fprintf(fp_e,"%s|%s",tmpstr,"开户金额超过内部账户余额\n");
						err_num++;
						continue;
					}
				}
				else
				{
					ret = Dd_mst_Sel(g_pub_tx.reply,&s_dd_mst,"ac_id = %ld",s_mdm_ac_rel.ac_id);
					if(ret)
					{
						fprintf(fp_e,"%s|%s",tmpstr,"查询对公账户错误\n");
						err_num++;
						continue;
					}

					if(open_amt > s_dd_mst.bal)
					{
						fprintf(fp_e,"%s|%s",tmpstr,"开户金额超过对公账户余额\n");
						err_num++;
						continue;
					}
				}

				if(strcmp(draw_note_type,"299") && strcmp(draw_note_type,"002"))
				{
					fprintf(fp_e,"%s|%s",tmpstr,"凭证类型错误!\n");
					err_num++;
					continue;
				}

				if(!strcmp(draw_note_type,"002"))
				{
					memset(draw_note_no,'0',sizeof(draw_note_no));
					strcpy(draw_note_no+(16-strlen(strfld[17])),strfld[17]);
				}

				if(!strcmp(draw_note_type,"002"))
				{
					ret = pub_com_ChkAcCheq( s_mdm_ac_rel.ac_id,s_mdm_ac_rel.ac_seqn,\
					draw_note_type,draw_note_no,draw_note_no,note_sts );
					if ( ret != 0 )
					{
						fprintf(fp_e,"%s|%s",tmpstr,"无此凭证或凭证状态不一致!\n");
						err_num++;
						continue;
					}

					if ( note_sts[0] == '0' )
					{
						fprintf(fp_e,"%s|%s",tmpstr,"无此凭证!\n");
						err_num++;
						continue;
					} else if ( note_sts[0] == '2' )
					{
						fprintf(fp_e,"%s|%s",tmpstr,"此凭证已挂失!\n");
						err_num++;
						continue;
					} else if ( note_sts[0] == '3' )
					{
						fprintf(fp_e,"%s|%s",tmpstr,"此凭证已回收!\n");
						err_num++;
						continue;
					} else if ( note_sts[0] == '4' )
					{
						fprintf(fp_e,"%s|%s",tmpstr,"此凭证已作废!\n");
						err_num++;
						continue;
					} else if ( note_sts[0] == '5' )
					{
						fprintf(fp_e,"%s|%s",tmpstr,"此凭证已使用!\n");
						err_num++;
						continue;
					}
				}
			}

			if(open_amt > 0.001 )
			{
				for(i=0;i<16;i++)
				{
					if(draw_note_no[i] < '0' || draw_note_no[i] > '9')
					{
						str_flag = 1;
					}
				}

				if(str_flag == 1)
				{
					fprintf(fp_e,"%s|%s\n",tmpstr,"凭证号码必须为数字!");
					err_num++;
					continue;
				}
			}

			fprintf(fp_r,"%s|%ld|%s|%s|%s|%lf|%ld|%ld|%ld|%ld|%lf|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",g_pub_tx.ac_no,sub_ac_seqn,\
							g_pub_tx.name,intst_type,rate_type,dRate_val,xq_num,lp_num,dy_num,fj_num,open_amt,paper_type,paper_no,telephone,\
							bar_code,cash_flag,draw_ac_no,draw_note_type,draw_note_no,draw_brf);
		}

		if(err_num != 0)
		{
			strcpy(g_pub_tx.reply,"B080");
			fclose(fp_e);
			sprintf(tmp_cmd,"cp -f %s %s",filename_e,filename);
			ret=system(tmp_cmd);
			if(ret)
			{
				vtcp_log("复制文件出错! [%s] [%s] [%d]",tmp_cmd,__FILE__,__LINE__);
				strcpy( g_pub_tx.reply, "S047" );
				goto ErrExit;
			}
			set_zd_data( DC_FILE_SND, "1" );
			goto ErrExit;
		}


		fclose(fp_r);
		sprintf(tmp_cmd,"cp -f %s %s",filename_r,filename);
		ret=system(tmp_cmd);
		if(ret)
		{
			vtcp_log("复制文件出错! [%s] [%s] [%d]",tmp_cmd,__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "S047" );
			goto ErrExit;
		}
		set_zd_data( DC_FILE_SND, "1" );
		goto OkExit;
	}else if(chk_flag[0]!='0')
	{
			vtcp_log("文件校验标志不对! [%s] [%s] [%d]",chk_flag,__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "P046" );
			goto ErrExit;
	}

	rewind(fp);

	while(1)
	{
		str_flag = 0;
		memset( str_jxlx,0x00,sizeof(str_jxlx));
		memset( tmpstr,0x00,sizeof(tmpstr));
		memset( &s_com_branch, 0x00, sizeof(struct com_branch_c));
		memset( &s_dd_mst,0x00,sizeof(struct dd_mst_c));
		memset( &s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
		memset( &s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		fgets(tmpstr,1024,fp);
		if(feof(fp))
		{
			vtcp_log("文件读取结束.[%s] [%d]",__FILE__,__LINE__);
			break;
		}

		memset(strfld,0x0,sizeof(strfld));
		for( i = 0;i<20;i++ )
		{
			ret = pub_base_cut_str( tmpstr,strfld[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,strfld[i]);
			pub_base_strpack( strfld[i] );
		}

		strcpy(g_pub_tx.ac_no,strfld[0]);
		sub_ac_seqn = atol(strfld[1]);
		strcpy(g_pub_tx.name,strfld[2]);
		strcpy(intst_type,strfld[3]);
		strcpy(rate_type,strfld[4]);
		sscanf(strfld[5],"%lf",&dRate_val);
		xq_num = atol(strfld[6]);
		lp_num = atol(strfld[7]);
		dy_num = atol(strfld[8]);
		fj_num = atol(strfld[9]);
		open_amt= atof(strfld[10]);
		strcpy(paper_type,strfld[11]);
		strcpy(paper_no,strfld[12]);
		strcpy(telephone,strfld[13]);
		strcpy(bar_code,strfld[14]);
		strcpy(cash_flag,strfld[15]);
		strcpy(draw_ac_no,strfld[16]);
		strcpy(draw_note_type,strfld[17]);
		strcpy(draw_note_no,strfld[18]);
		strcpy(draw_brf,strfld[19]);
		strcpy(g_pub_tx.brf,"子账户批量开户");
		memset(draw_note_no,'0',sizeof(draw_note_no));
		strcpy(draw_note_no+(16-strlen(strfld[18])),strfld[18]);

		/* 子账户开户 */
		pub_base_old_acno(g_pub_tx.ac_no);	/** 对旧帐号的处理 **/
		strcpy(g_pub_tx.cur_no, "01");	/*** 默认人民币 ***/
		strcpy(g_pub_tx.add_ind, "1");
		strcpy(g_pub_tx.ct_ind, "2");
		g_pub_tx.svc_ind = 1201;
		g_pub_tx.tx_amt1 = 0.00;
		strcpy(g_pub_tx.ac_wrk_ind,"01000000");
		strcpy(g_pub_tx.tx_code,"6615");
		strcpy(g_pub_tx.sub_tx_code,"F615");
		strcpy(g_pub_tx.hst_ind, "1");
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
		strcpy(g_pub_tx.end_note_no,"");

		memset( &s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no = '%s'",g_pub_tx.ac_no);
		if(ret)
		{
			vtcp_log("[%s][%d]查询 Mdm_ac_rel 错误 ,[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		g_pub_tx.ac_id=s_mdm_ac_rel.ac_id;
		if(intst_type[0] == '0')
		{
			dRate_val=0.0;
			strcpy(str_jxlx,"不计息");
		}
		else if(intst_type[0] == '4' && rate_type[0] == '0')
		{
			ret = pub_base_getllz(HQLLBH, "01", g_pub_tx.tx_date,&dRate_val);
			if(ret)
			{
				vtcp_log(" %s, %d, 取利率错误!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"O195");
				goto ErrExit;
			}
			strcpy(str_jxlx,"按季计息");
		}
		else
		{
			strcpy(str_jxlx,"按季计息");
		}

		ret = pub_base_opn_sub_ac(sub_ac_seqn,intst_type,dRate_val);
		if (ret)
		{
			vtcp_log(acErrMsg, "开立子帐号主控错误! [%s] [%d]",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D114");
			goto ErrExit;
		}

		ret = Sub_dd_mst_Dec_Upd(g_pub_tx.reply,"ac_no = '%s' and sub_ac_seqn = %ld",g_pub_tx.ac_no,sub_ac_seqn);
		if(ret)
		{
			vtcp_log("子账户表查询声明游标错误 [%s] [%d]",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		ret = Sub_dd_mst_Fet_Upd(&s_sub_dd_mst,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("子账户获取游标错误 [%s] [%d]",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}

		strncpy(s_sub_dd_mst.rate_type,rate_type,strlen(rate_type));
		s_sub_dd_mst.xq_num = xq_num;
		s_sub_dd_mst.lp_num = lp_num;
		s_sub_dd_mst.dy_num = dy_num;
		s_sub_dd_mst.fj_num = fj_num;
		strcpy(s_sub_dd_mst.paper_type,paper_type);
		strcpy(s_sub_dd_mst.paper_no,paper_no);
		strcpy(s_sub_dd_mst.telephone,telephone);
		strcpy(s_sub_dd_mst.bar_code,bar_code);
		ret = Sub_dd_mst_Upd_Upd(s_sub_dd_mst,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("子账户更新错误 [%s] [%d]",__FILE__,__LINE__);
			Sub_dd_mst_Debug(&s_sub_dd_mst);
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}

		Sub_dd_mst_Clo_Upd();

		if(open_amt > 0.001)
		{
			/* 内部账户扣款 */
			memset( &s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no = '%s'",draw_ac_no);
			if(ret)
			{
				vtcp_log("[%s][%d]查询 Mdm_ac_rel 错误 ,[%s]",__FILE__,__LINE__,draw_ac_no);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			if(draw_ac_no[0] == '9')
			{
				strcpy(g_pub_tx.ac_no,draw_ac_no);
				g_pub_tx.ac_id=s_mdm_ac_rel.ac_id;
				g_pub_tx.ac_seqn=s_mdm_ac_rel.ac_seqn;
				g_pub_tx.svc_ind=9001;
				g_pub_tx.ct_ind[0]='2';
				g_pub_tx.tx_amt1=open_amt;
				strcpy(g_pub_tx.ac_get_ind, "00");
				strcpy(g_pub_tx.ac_id_type, "9");
				strcpy(g_pub_tx.add_ind, "0");
				strcpy(g_pub_tx.hst_ind, "1");
				strcpy(g_pub_tx.ac_wrk_ind, "00000000");
				strcpy(g_pub_tx.note_type,draw_note_type);
				strcpy(g_pub_tx.beg_note_no,draw_note_no);
				strcpy(g_pub_tx.end_note_no,draw_note_no);
				strcpy(g_pub_tx.brf,draw_brf);
				strcpy(g_pub_tx.cur_no, "01");
				strcpy(g_pub_tx.tx_code,"6615");
				strcpy(g_pub_tx.sub_tx_code,"A016");
				ret = pub_acct_trance();
				if (ret)
				{
					sprintf(acErrMsg, "内部帐户取款错误!!");
					WRITEMSG
					goto ErrExit;
				}

				set_zd_data("1201",draw_ac_no);
				set_zd_double("1208",g_pub_tx.tx_amt1);
				set_zd_data("1206",g_pub_tx.note_type);
				set_zd_data("1207",g_pub_tx.beg_note_no);
				set_zd_data("120A",g_pub_tx.brf);
				set_zd_data("1205",g_pub_tx.ct_ind);
				set_zd_data("1204",g_pub_tx.cur_no);

				ret = pubf_acct_proc("A016");
				if(ret)
				{
					vtcp_log("内部账户取款失败 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"P126");
					goto ErrExit;
				}
			}else
			{
				strcpy(g_pub_tx.ac_no,draw_ac_no);
				g_pub_tx.ac_id=s_mdm_ac_rel.ac_id;
				g_pub_tx.ac_seqn=s_mdm_ac_rel.ac_seqn;
				g_pub_tx.svc_ind=1001;
				g_pub_tx.ct_ind[0]='2';
				g_pub_tx.tx_amt1=open_amt;
				strcpy(g_pub_tx.ac_get_ind, "00");
				strcpy(g_pub_tx.ac_id_type, "1");
				strcpy(g_pub_tx.add_ind, "0");
				strcpy(g_pub_tx.hst_ind, "1");
				strcpy(g_pub_tx.ac_wrk_ind, "00000000");
				strcpy(g_pub_tx.note_type,draw_note_type);
				strcpy(g_pub_tx.beg_note_no,draw_note_no);
				strcpy(g_pub_tx.end_note_no,draw_note_no);
				strcpy(g_pub_tx.brf,draw_brf);
				strcpy(g_pub_tx.cur_no, "01");
				strcpy(g_pub_tx.tx_code,"6615");
				strcpy(g_pub_tx.sub_tx_code,"D003");
				ret = pub_acct_trance();
				if (ret)
				{
					sprintf(acErrMsg, "内部帐户取款错误!!");
					WRITEMSG
					goto ErrExit;
				}

		    set_zd_data("1021",draw_ac_no);
		    set_zd_long("1022",s_mdm_ac_rel.ac_seqn);
		    set_zd_data("1023",draw_note_type);
		    set_zd_data("1024",draw_note_no);
		    set_zd_data("1025",s_mdm_ac_rel.draw_uncon_yn);
		    set_zd_data("1026",s_mdm_ac_rel.draw_pwd_yn);
		    set_zd_data("1027",s_mdm_ac_rel.qry_pwd);
		    set_zd_data("1028",s_mdm_ac_rel.draw_pwd);
		    set_zd_data("1029",s_mdm_ac_rel.draw_id_yn);
		    set_zd_data("102G",s_mdm_ac_rel.id_type);
		    set_zd_data("102A",s_mdm_ac_rel.id_no);
		    set_zd_data("102B",s_mdm_ac_rel.draw_seal_yn);
		    set_zd_data("102C",s_mdm_ac_rel.pwd_mach_yn);
		    set_zd_double("102F",g_pub_tx.tx_amt1);
		    set_zd_data("102J",g_pub_tx.cur_no);
		    set_zd_data("102K",g_pub_tx.ct_ind);
		    set_zd_data("102Y",draw_brf);

				ret = pubf_acct_proc("D003");
				if(ret)
				{
					vtcp_log("内部账户取款失败 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"P126");
					goto ErrExit;
				}
			}

			memset( &s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no = '%s'",strfld[0]);
			if(ret)
			{
				vtcp_log("[%s][%d]查询 Mdm_ac_rel 错误 ,[%s]",__FILE__,__LINE__,strfld[0]);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}

			ret = Dd_mst_Sel( g_pub_tx.reply, &s_dd_mst, "ac_id = %ld", s_mdm_ac_rel.ac_id );
			if(ret)
			{
				vtcp_log("取活期主文件出错 [%s] [%d]",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D102");
				goto ErrExit;
			}

			strcpy(g_pub_tx.note_type,"");
			strcpy(g_pub_tx.beg_note_no,"");
			strcpy(g_pub_tx.end_note_no,"");
			strcpy(g_pub_tx.prdt_code,s_dd_mst.prdt_no );
			strcpy(g_pub_tx.ac_no,s_mdm_ac_rel.ac_no);
			g_pub_tx.tx_amt1=open_amt;
			strcpy(g_pub_tx.ac_no1,s_sub_dd_mst.sub_ac_no);
			g_pub_tx.add_ind[0]='1';
			g_pub_tx.svc_ind=1201;
			strcpy(g_pub_tx.ct_ind,"2");
			strcpy(g_pub_tx.brf,"批量开户存款");
			strcpy(g_pub_tx.cur_no,"01");
			strcpy(g_pub_tx.tx_code,"6615");
			strcpy(g_pub_tx.sub_tx_code,"F615");
			ret=pub_base_sub_ac_trance(sub_ac_seqn);
			if(ret)
			{
				vtcp_log("子账户批量开户失败,[%s] [%d] ",__FILE__,__LINE__);
				goto ErrExit;
			}

			strcpy(g_pub_tx.ac_wrk_ind,"00000000");
			g_pub_tx.svc_ind=1001;
			strcpy(g_pub_tx.ct_ind,"2");
			strcpy(g_pub_tx.add_ind,"1");
			strcpy(g_pub_tx.prdt_code,s_dd_mst.prdt_no );
			g_pub_tx.ac_id=s_mdm_ac_rel.ac_id;
			g_pub_tx.ac_seqn=s_mdm_ac_rel.ac_seqn;
			strcpy(g_pub_tx.note_type,"");
			strcpy(g_pub_tx.beg_note_no,"");
			strcpy(g_pub_tx.end_note_no,"");
			strcpy(g_pub_tx.no_show,"0");
			strcpy(g_pub_tx.ac_get_ind,"000");
			strcpy(g_pub_tx.ac_id_type,"1");
			strcpy(g_pub_tx.hst_ind,"1");
			strcpy(g_pub_tx.brf,"开户存款");
			strcpy(g_pub_tx.tx_code,"6615");
			strcpy(g_pub_tx.sub_tx_code,"D099");

			ret=pub_acct_trance();
			if(ret)
			{
				vtcp_log("登记交易流水失败 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P126");
				goto ErrExit;
			}

			set_zd_data("1011", g_pub_tx.ac_no);
			set_zd_int ("1012", g_pub_tx.ac_seqn);
			set_zd_data("101A", g_pub_tx.cur_no);
			set_zd_data("1014", g_pub_tx.note_type);
			set_zd_data("1015", g_pub_tx.beg_note_no);
			set_zd_data("1015", g_pub_tx.end_note_no);
			set_zd_data("101B", g_pub_tx.ct_ind);
			set_zd_data("1016", g_pub_tx.brf);
			set_zd_double("1013", open_amt);

			ret = pubf_acct_proc("D099");
			if(ret)
			{
				vtcp_log("存入主账户失败 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P126");
				goto ErrExit;
			}
		}

		fprintf( fp_r,"%s|%d|%s|%lf|%s|%s|%s\n",s_mdm_ac_rel.ac_no,sub_ac_seqn,s_sub_dd_mst.name,open_amt,g_pub_tx.brf,s_sub_dd_mst.bar_code,s_sub_dd_mst.sub_ac_no);

		num++;
	}

	set_zd_long("0480",num);

	fclose(fp);
	fclose(fp_e);
	fclose(fp_r);

	if( num > 0)
	{
		sprintf(tmp_cmd,"cp -f %s %s",filename_r,filename);
		ret=system(tmp_cmd);
		if(ret)
		{
			vtcp_log("复制文件出错! [%s] [%s] [%d]",tmp_cmd,__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "S047" );
			goto ErrExit;
		}
		/* set_zd_data( DC_FILE_SND, "1" ); */
	}

OkExit:
	sql_commit(); /* 提交事物 */
	if(fp != NULL) fclose(fp);
	if(fp_e != NULL) fclose(fp_e);
	if(fp_r != NULL) fclose(fp_r);
	strcpy(g_pub_tx.reply,"0000");
	vtcp_log("二级子账户批量开户成功!!!!![%s]", g_pub_tx.reply);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return(0);

ErrExit:
	if(fp != NULL) fclose(fp);
	if(fp_e != NULL) fclose(fp_e);
	if(fp_r != NULL) fclose(fp_r);
	if(strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0] == '\0')
	{
		strcpy(g_pub_tx.reply, "D114");
	}
	sql_rollback();		/* 事务回滚 */
	vtcp_log("二级子账户批量开户失败!!!!![%s]", g_pub_tx.reply);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return(1);
}


int spF_chk(char *src)
{
	int i=0;
	int flag=0;

	for(i=0;i<strlen(src);i++)
	{
		if(*(src+i) < '0' || *(src+i) > '9')
		{
			if(*(src+i)=='.' && strlen(src+i)<=3)
			{
				flag++;
				continue;
			}
			else
			{
				return(-1);
			}
		}
	}

	if(flag>1)
	{
	return(-1);
	}

	return(0);
}












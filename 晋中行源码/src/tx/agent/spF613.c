/*************************************************
* 文 件 名: spF613.c
* 功能描述：批量扣款交易
*
* 作    者: z.s.wang
* 完成日期: 2012年03月19日
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

struct tmp_input {
	char   out_ac_no[19+1];
	long    sub_ac_seqn;
	char   sub_name[50+1];
	double amt;
	char   note_type[3+1];
	char   note_no[16+1];
	char   brf[20+1];
	char   in_ac_no[19+1];
};

spF613()
{
	int i=0;
	int ret=-1;
	int str_flag=0;
	int err_num=0;
	int num=0;
	char tmpstr[1024];
	char strfld[8][128];
	char note_sts[2];
	char filename[256];
	char filename_e[256];
	char filename_r[256];
	char tmp_filename_e[128];
	char tmp_filename_r[128];
	char tmp_cmd[512];
	char tmp_pzlx[11];
	char tmp_name[61];
	char chk_flag[2];
	FILE *fp=NULL;
	FILE *fp_e=NULL;
	FILE *fp_r=NULL;
	struct tmp_input s_input;
	struct sub_dd_mst_c s_sub_dd_mst;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	struct com_branch_c s_com_branch;
	struct dd_mst_c s_dd_mst;

	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(strfld,0x00,sizeof(strfld));
	memset(filename,0x00,sizeof(filename));
	memset(filename_e,0x00,sizeof(filename_e));
	memset(filename_r,0x00,sizeof(filename_r));
	memset(tmp_filename_e,0x00,sizeof(tmp_filename_e));
	memset(tmp_filename_r,0x00,sizeof(tmp_filename_r));
	memset(note_sts,0x00,sizeof(note_sts));
	memset(tmp_cmd,0x00,sizeof(tmp_cmd));
	memset(tmp_pzlx,0x00,sizeof(tmp_pzlx));
	memset(tmp_name,0x00,sizeof(tmp_name));
	memset(chk_flag,0x00,sizeof(chk_flag));
	memset(&s_input,0x00,sizeof(struct tmp_input));
	memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&s_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));

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

	vtcp_log("取得的文件名为[%s] [%s] [%d]",filename,__FILE__,__LINE__);
	/* 文件先转换,排序,还有去掉\r */
	sprintf(tmp_cmd,"trans.sh %s",filename);
	ret=system(tmp_cmd);
	if(ret)
	{
		vtcp_log("处理文件出错! [%s] [%s] [%d]",tmp_cmd,__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	sprintf(tmp_filename_e,"Err-%s-%ld-%06d-subdeal.txt",g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_pub_tx.trace_no);
	sprintf( filename_e,"%s/%s",getenv("FILDIR"),tmp_filename_e);

	vtcp_log("错误数据的文件名为[%s] [%s] [%d]",filename_e,__FILE__,__LINE__);

	sprintf(tmp_filename_r,"Re-%s-%ld-%06d-subdeal.txt",g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_pub_tx.trace_no);
	sprintf( filename_r,"%s/%s",getenv("FILDIR"),tmp_filename_r);

	vtcp_log("回显的文件名为[%s] [%s] [%d]",filename_r,__FILE__,__LINE__);

	fp = fopen(filename,"r");
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

	/* -- 处理之前先校验 -- */
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
			for( i = 0;i<8;i++ )
			{
				ret = pub_base_cut_str( tmpstr,strfld[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,strfld[i]);
				pub_base_strpack( strfld[i] );
				if(strlen(strfld[i]) == 0 && i != 6 )
				{
					str_flag = 1;
				}
			}

			if(str_flag == 1)
			{
				fprintf(fp_e,"%s|%s\n",tmpstr,"除摘要外 其他的元素不可以为空值");
				err_num++;
				continue;
			}

			ret=spF_chk(strfld[3]);
			if(ret)
			{
				fprintf(fp_e,"%s|%s\n",tmpstr,"金额格式有误");
				err_num++;
				continue;
			}

			strcpy(s_input.out_ac_no,strfld[0]);
			s_input.sub_ac_seqn=atoi(strfld[1]);
			strcpy(s_input.sub_name,strfld[2]);
			s_input.amt=atof(strfld[3]);
			strcpy(s_input.note_type,strfld[4]);
			pub_base_ltrim_zero(strfld[5]);
			strcpy(s_input.brf,strfld[6]);
			strcpy(s_input.in_ac_no,strfld[7]);
			memset(s_input.note_no,'0',sizeof(s_input.note_no));
			strcpy(s_input.note_no+(16-strlen(strfld[5])),strfld[5]);

			memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &s_mdm_ac_rel, "ac_no='%s'", s_input.in_ac_no);
			if( ret )
			{
				vtcp_log("查询介质帐号关系出错 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
				fprintf(fp_e,"%s|%s\n",tmpstr,"查询转入账户出错,无此账户");
				err_num++;
				continue;
			}

			if(s_mdm_ac_rel.note_sts[0] != '0')
			{
				fprintf(fp_e,"%s|%s\n",tmpstr,"转入账户状态不正常,请核对");
				err_num++;
				continue;
			}

			if(strlen(strfld[6])>20)
			{
				fprintf(fp_e,"%s|%s\n",tmpstr,"摘要过长!(20个字符以内)");
				err_num++;
				continue;
			}

			if( s_input.in_ac_no[0] != '9' )
			{
				fprintf(fp_e,"%s|%s\n",tmpstr,"转入账户只允许为内部账");
				err_num++;
				continue;
			}

			memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &s_mdm_ac_rel, "ac_no='%s' and draw_pwd_chg='1'", s_input.out_ac_no);
			if( ret )
			{
				vtcp_log("查询介质帐号关系出错 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
				fprintf(fp_e,"%s|%s\n",tmpstr,"查询主账户出错,无此账户");
				err_num++;
				continue;
			}

			if(s_mdm_ac_rel.note_sts[0] != '0')
			{
				fprintf(fp_e,"%s|%s\n",tmpstr,"转出主账户状态不正常,请核对");
				err_num++;
				continue;
			}

			ret=Sub_dd_mst_Sel(g_pub_tx.reply,&s_sub_dd_mst,"ac_no='%s' and sub_ac_seqn=%d",\
				s_input.out_ac_no,s_input.sub_ac_seqn);
			if( ret )
			{
				vtcp_log("查询子账户主文件出错 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
				fprintf(fp_e,"%s|%s\n",tmpstr,"查询子账户出错,无此对应子账户");
				err_num++;
				continue;
			}

			if( s_sub_dd_mst.ac_sts[0] != '1' )
			{
				fprintf(fp_e,"%s|%s\n",tmpstr,"该子账户状态不正常,请检查账户");
				err_num++;
				continue;
			}

			if(strcmp(s_input.sub_name,s_sub_dd_mst.name))
			{
				fprintf(fp_e,"%s|%s:[%s]\n",tmpstr,"子账户户名不符,请检查账户,子账户户名应为",s_sub_dd_mst.name);
				err_num++;
				continue;
			}

			if( s_input.amt > s_sub_dd_mst.bal )
			{
				fprintf(fp_e,"%s|%s\n",tmpstr,"交易金额大于子账户余额");
				err_num++;
				continue;
			}

			if( s_input.amt < 0.001 )
			{
				fprintf(fp_e,"%s|%s\n",tmpstr,"交易金额为0,请确认");
				err_num++;
				continue;
			}

			if(strcmp(s_input.note_type,"299") && strcmp(s_input.note_type,"002"))
			{
				fprintf(fp_e,"%s|%s\n",tmpstr,"凭证类型有误!");
				err_num++;
				continue;
			}

			if(!strcmp(s_input.note_type,"002"))
			{
				memset(s_input.note_no,'0',sizeof(s_input.note_no));
				strcpy(s_input.note_no+(16-strlen(strfld[5])),strfld[5]);
			}


			if(!strcmp(s_input.note_type,"002"))
			{
				ret = pub_com_ChkAcCheq( s_mdm_ac_rel.ac_id,s_mdm_ac_rel.ac_seqn,\
					s_input.note_type,s_input.note_no,s_input.note_no,note_sts );
				if ( ret != 0 )
				{
					fprintf(fp_e,"%s|%s\n",tmpstr,"客户无此凭证或凭证状态不一致!");
					err_num++;
					continue;
				}

				if ( note_sts[0] == '0' )
				{
					fprintf(fp_e,"%s|%s\n",tmpstr,"客户无此凭证!");
					err_num++;
					continue;
				} else if ( note_sts[0] == '2' )
				{
					fprintf(fp_e,"%s|%s\n",tmpstr,"此凭证已挂失!");
					err_num++;
					continue;
				} else if ( note_sts[0] == '3' )
				{
					fprintf(fp_e,"%s|%s\n",tmpstr,"此凭证已回收!");
					err_num++;
					continue;
				} else if ( note_sts[0] == '4' )
				{
					fprintf(fp_e,"%s|%s\n",tmpstr,"此凭证已作废!");
					err_num++;
					continue;
				} else if ( note_sts[0] == '5' )
				{
					fprintf(fp_e,"%s|%s\n",tmpstr,"此凭证已使用!");
					err_num++;
					continue;
				}
			}

			for(i=0;i<16;i++)
			{
				if(s_input.note_no[i] < '0' || s_input.note_no[i] > '9')
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

			fprintf(fp_r,"%s\n",tmpstr);
		}

		/* -- 有校验没通过的记录就stop,返回校验没通过的文件 -- */
		if(err_num != 0)
		{
			strcpy(g_pub_tx.reply,"B080");
			fclose(fp);
			fclose(fp_e);
			sprintf(tmp_cmd,"cp -f %s %s ",filename_e,filename);
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

		fclose(fp);
		fclose(fp_r);
		sprintf(tmp_cmd,"cp -f %s %s ",filename_r,filename);
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


	rewind(fp); /* -- 文件重新指向文件头 -- */

	/* -- 开始做账务处理 -- */
	while(1)
	{
		memset(tmp_name,0x00,sizeof(tmp_name));
		memset(tmpstr,0x00,sizeof(tmpstr));
		memset(tmp_pzlx,0x00,sizeof(tmp_pzlx));
		memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
		memset(&s_com_branch,0x00,sizeof(struct com_branch_c));
		memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
		fgets(tmpstr,1024,fp);
		if(feof(fp))
		{
			vtcp_log("文件读取结束.[%s] [%d]",__FILE__,__LINE__);
			break;
		}

		memset(strfld,0x0,sizeof(strfld));
		for( i = 0;i<8;i++ )
		{
			ret = pub_base_cut_str( tmpstr,strfld[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,strfld[i]);
			pub_base_strpack( strfld[i] );
		}

		strcpy(s_input.out_ac_no,strfld[0]);
		s_input.sub_ac_seqn=atoi(strfld[1]);
		strcpy(s_input.sub_name,strfld[2]);
		s_input.amt=atof(strfld[3]);
		strcpy(s_input.note_type,strfld[4]);
		pub_base_ltrim_zero(strfld[5]);
		strcpy(s_input.brf,strfld[6]);
		strcpy(s_input.in_ac_no,strfld[7]);
		memset(s_input.note_no,'0',sizeof(s_input.note_no));
		strcpy(s_input.note_no+(16-strlen(strfld[5])),strfld[5]);

		memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &s_mdm_ac_rel, "ac_no='%s' and draw_pwd_chg='1'", s_input.out_ac_no);
		if( ret )
		{
			vtcp_log("查询介质帐号关系出错 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			goto ErrExit;
		}

		ret = Dd_mst_Sel( g_pub_tx.reply, &s_dd_mst, "ac_id = %ld", s_mdm_ac_rel.ac_id );
		if(ret)
		{
			vtcp_log("取活期主文件出错 [%s] [%d]",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}
		strcpy(tmp_name,s_dd_mst.name);

		ret=Sub_dd_mst_Sel(g_pub_tx.reply,&s_sub_dd_mst,"ac_no='%s' and sub_ac_seqn=%d",\
			s_input.out_ac_no,s_input.sub_ac_seqn);
		if( ret )
		{
			vtcp_log("查询子账户主文件出错 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}

		strcpy(g_pub_tx.prdt_code,s_dd_mst.prdt_no);
		strcpy(g_pub_tx.cur_no,"01");
		g_pub_tx.svc_ind=1201;
		g_pub_tx.ac_seqn=s_mdm_ac_rel.ac_seqn;
		strcpy(g_pub_tx.ac_no,s_input.out_ac_no);
		g_pub_tx.add_ind[0]='0';
		strcpy(g_pub_tx.note_type,s_input.note_type);
		strcpy(g_pub_tx.beg_note_no,s_input.note_no);
		strcpy(g_pub_tx.end_note_no,s_input.note_no);
		g_pub_tx.tx_amt1=s_input.amt;
		g_pub_tx.ct_ind[0]='2';
		strcpy(g_pub_tx.brf,s_input.brf);
		strcpy(g_pub_tx.hst_ind, "1");
		strcpy(g_pub_tx.ac_wrk_ind, "00000000");

		ret=pub_base_sub_ac_trance(s_input.sub_ac_seqn);
		if( ret )
		{
			vtcp_log("调用子账户取款失败! [%s] [%d]",__FILE__,__LINE__);
			goto ErrExit;
		}

		strcpy(g_pub_tx.ac_no,s_input.out_ac_no);
		g_pub_tx.ac_id=s_mdm_ac_rel.ac_id;
		g_pub_tx.svc_ind=1001;
		g_pub_tx.ct_ind[0]='2';
		g_pub_tx.tx_amt1=s_input.amt;
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.ac_id_type, "1");
		strcpy(g_pub_tx.add_ind, "0");
		strcpy(g_pub_tx.hst_ind, "1");
		strcpy(g_pub_tx.ac_wrk_ind, "00000000");
		strcpy(g_pub_tx.brf,s_input.brf);
		g_reply_int = pub_acct_trance();
		if (g_reply_int)
		{
			sprintf(acErrMsg, "向活期对公帐户取款错误!!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data("1021",s_input.out_ac_no);
		set_zd_long("1022",&s_mdm_ac_rel.ac_seqn);
		set_zd_data("1023",s_input.note_type);
		set_zd_data("1024",s_input.note_no);
		set_zd_data("1024",s_input.note_no);
		set_zd_data("102J","01");
		set_zd_data("102K","2");
		set_zd_data("102Y",s_input.brf);
		set_zd_data("102O","0"); /*是否打折标志*/
		set_zd_data("0191","0");
		set_zd_data("0192","0000000000");
		set_zd_int("0193",1001);
		set_zd_double("102F",s_input.amt);

		ret = pubf_acct_proc("D003");
		if(ret)
		{
			vtcp_log("主账户取款失败 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P126");
			goto ErrExit;
		}

		memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &s_mdm_ac_rel, "ac_no='%s'", s_input.in_ac_no);
		if( ret )
		{
			vtcp_log("查询介质帐号关系出错 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			goto ErrExit;
		}

		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
		strcpy(g_pub_tx.end_note_no,"");
		strcpy(g_pub_tx.ac_no,s_input.in_ac_no);
		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.ac_id_type, "9");
		strcpy(g_pub_tx.add_ind, "1");	/** 不是减少，是借方 **/
		strcpy(g_pub_tx.ac_wrk_ind, "0000000J");
		strcpy(g_pub_tx.hst_ind, "1");
		g_pub_tx.svc_ind = 9001;
		g_pub_tx.tx_amt1 = s_input.amt;
		strcpy(g_pub_tx.brf,s_input.brf);
		ret = pub_acct_trance();
		if (ret)
		{
			sprintf(acErrMsg, "内部帐记交易流水错误!!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data("1211",s_input.in_ac_no);
		set_zd_double("1218",s_input.amt);
		set_zd_data("1216",g_pub_tx.note_type);
		set_zd_data("1217",g_pub_tx.beg_note_no);
		set_zd_data("121A",g_pub_tx.brf);
		set_zd_data("1215","2");
		set_zd_data("1214","01");

		ret = pubf_acct_proc("A017");
		if(ret)
		{
			vtcp_log("存入内部账户失败 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P126");
			goto ErrExit;
		}

		ret = Com_branch_Sel( g_pub_tx.reply, &s_com_branch, "br_no = '%s'", g_pub_tx.tx_br_no );
		if(ret)
		{
			vtcp_log("取交易机构出错 [%s] [%d]",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}

		fprintf( fp_r,"%s|%d|%s|%lf|%s|%s|%s\n",s_input.out_ac_no,s_input.sub_ac_seqn,s_input.sub_name,s_input.amt,s_input.brf,s_sub_dd_mst.sub_ac_no);

		/* g_pub_tx.trace_cnt++;
		set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt); */ /* --- 此行注掉可以让trace_cnt连续 wzs --- */

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
	strcpy(g_pub_tx.reply,"0000");
	vtcp_log("二级子账户批量扣款成功!!!!![%s]", g_pub_tx.reply);
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
	vtcp_log("二级子账户批量扣款失败!!!!![%s]", g_pub_tx.reply);
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

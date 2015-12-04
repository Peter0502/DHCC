/*************************************************
* 文 件 名:  spW003.c
* 功能描述： 账户明细查询 
*
* 作    者:  chengbo 
* 完成日期： 2009年3月27日 
*
* 修改记录： 
* 日   期:	2009年3月31日 
* 修 改 人:	gujingyu
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "td_mst_hst_c.h"
#include "com_branch_c.h"
#include "mdm_unprt_hst_c.h"
#define  PAGE_CNT  20		/* 网银分页查询，每页多少行 */
extern char *get_env_info(char *infoname);

spW003()
{
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_hst_c vdd_mst_hst;
	struct td_mst_hst_c vtd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hsta;
	struct com_branch_c sCom_branch;
	struct mdm_unprt_hst_c sMdm_unprt_hst;

	char            add_ind[3];
	char            ct_ind[5];
	char            ac_no3[25];
	char            f_note_type9[41];
	char            f_acno0[25];
	char            filename[100];
	char            pcfilename[100];
	char            wherelist[1024];	/**查询条件**/
	char            tmpstr[512];
	char            tmpmo[512];
	char						sPc_flag[2];   		/*对公对私标志*/
	char            fieldlist[1024];	/**要查询的字段列表**/
	char            tablelist[128];		/**要查询的表名**/
	char            titstr[1024];
	long						ttlnum = 0;				/**读取的总条数**/
	int             i = 0;
	int             ret = 0,  dPage = 0;
	double          j_tx_amt = 0.00;
	double          d_tx_amt = 0.00;
	FILE           *fp;
	char            tmp_date[9];
	char            tmp_time[7];
	int             currnum = 0;
	long            qs_date, jz_date;
	long            dBeg_num = 0, dEnd_num = 0, dTol_num = 0;
	char            bz[4];
	char            flag[3];
	char            td_mdm[5];  /***/
	char            tx_date[8+1];
	char            sw_traceno[10+1];
	char            cFtpfile[200];

	memset(add_ind, 0x00, sizeof(add_ind));
	memset(sPc_flag,0,sizeof(sPc_flag));
	memset(ct_ind, 0x00, sizeof(ct_ind));
	memset(ac_no3, 0x00, sizeof(ac_no3));
	memset(f_note_type9, 0x00, sizeof(f_note_type9));
	memset(f_acno0, 0x00, sizeof(f_acno0));
	memset(&vdd_mst_hst, 0x00, sizeof(vdd_mst_hst));
	memset(&vtd_mst_hst, 0x00, sizeof(vtd_mst_hst));
	memset(&f_dd_mst_hst, 0x00, sizeof(f_dd_mst_hst));
	memset(&f_dd_mst_hsta, 0x00, sizeof(f_dd_mst_hsta));
	memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
	memset(&sCom_branch, 0x00, sizeof(sCom_branch));
	memset(tmpstr, 0x00, sizeof(tmpstr));
	memset(filename, 0x00, sizeof(filename));
	memset(pcfilename, 0x00, sizeof(pcfilename));
	memset(tmpmo, 0x00, sizeof(tmpmo));
	memset(titstr, 0x00, sizeof(titstr));
	memset(tmp_date, 0x00, sizeof(tmp_date));
	memset(tmp_time, 0x00, sizeof(tmp_time));
	memset(&g_pub_tx, 0, sizeof(g_pub_tx));
	memset(wherelist, 0, sizeof(wherelist));
	memset(fieldlist, 0, sizeof(fieldlist));
	memset(tablelist, 0, sizeof(tablelist));
	memset(bz, 0, sizeof(bz));
	memset(td_mdm, 0, sizeof(td_mdm));
	memset(tx_date, 0, sizeof(tx_date));
	memset(sw_traceno, 0, sizeof(sw_traceno));
	memset(cFtpfile, 0, sizeof(cFtpfile));
	pub_base_sysinit();

	get_zd_data("0300", f_acno0);
	pub_base_old_acno(f_acno0);				/** 处理老账号**/
	get_zd_long("0440", &qs_date);
	get_zd_long("0450", &jz_date);
	get_zd_long("0540", &dBeg_num);		/* 开始的条数 */
	get_zd_long("0550", &dTol_num);		/* 要取到的总条数 */
	get_zd_data("0710", flag);				/* 查询标志 */
	get_zd_data("0660",sPc_flag);
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);
	vtcp_log("[%s],[%d],dBeg_num[%ld],dTol_num[%ld]", __FILE__, __LINE__, dBeg_num, dTol_num);
	dEnd_num = dBeg_num + dTol_num - 1;	/* 结束的条数 */
	vtcp_log("[%s],[%d],dEnd_num=[%ld]", __FILE__, __LINE__, dEnd_num);

	/**查询**/
	pub_base_strpack(f_acno0);
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", f_acno0);
	ERR_DEAL
	
	sprintf( filename,"1021netbank%s%s",tx_date,sw_traceno );
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
	fp = fopen(pcfilename, "w");
	if (fp == NULL) 
	{
		sprintf(acErrMsg, " open file [%s] error ", pcfilename);
		WRITEMSG
		strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	vtcp_log("[%s],[%d],下载全路径文件名为[%s],文件名为[%s]", __FILE__, __LINE__, pcfilename,filename);
	fprintf(fp, "~@交易日期|@交易时间|@借贷方向|@交易金额|@账户余额|@摘要|@凭证种类|@凭证号码|\n");

	ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply, "ac_id=%ld and tx_date>=%ld and tx_date<=%d order by hst_cnt desc", sMdm_ac_rel.ac_id, qs_date, jz_date);
	if(ret)
	{
		vtcp_log("%s %d::Dd_mst_hst_Dec_Sel [%d] error ",__FILE__,__LINE__,ret);
	 	strcpy(g_pub_tx.reply,"D102");
	 	set_zd_data("0130","打开游标出错");
	 	fclose(fp);
	 	goto ErrExit;		
	}

	while (1) 
	{
		ret = Dd_mst_hst_Fet_Sel(&vdd_mst_hst, g_pub_tx.reply);
		if(ret == 1403 || ret == 100)
		{
			vtcp_log("%s %d::所有的记录已查询完毕!",__FILE__,__LINE__);
			break;
		}
		else if(0 != ret && 100 != ret)
		{
			vtcp_log("%s %d::Dd_mst_hst_Fet_Sel error[%d]",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D001");
			set_zd_data("0130","查询错误");
			Dd_mst_hst_Clo_Sel();
			fclose(fp);
			goto ErrExit;
		}

		if (vdd_mst_hst.add_ind[0] == '0') 
		{
			strcpy(add_ind, "1");  /**1贷**/
			j_tx_amt = j_tx_amt + vdd_mst_hst.tx_amt;
		}
		else if (vdd_mst_hst.add_ind[0] == '1') 
		{
			strcpy(add_ind, "0");  /**0借**/
			d_tx_amt = d_tx_amt + vdd_mst_hst.tx_amt;
		}
		sprintf(tmp_date, "%ld", vdd_mst_hst.tx_date);
		sprintf(tmp_time, "%d", vdd_mst_hst.tx_time);
		strcpy(bz, "CNY");

		ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'", vdd_mst_hst.tx_br_no);
		if (0 != ret && 100 != ret)
		{
			vtcp_log("%s %d::Com_branch_Sel error[%d]",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D001");
			set_zd_data("0130","Com_branch_Sel查询错误");
			Dd_mst_hst_Clo_Sel();
			fclose(fp);
			goto ErrExit;
		}
		
		ttlnum++;
				
		if ( dTol_num!=0 && ttlnum>=dBeg_num && ttlnum<=dEnd_num ) 
		{
			fprintf(fp, "|%d|%06d|%s|%17.2f|%17.2f|%32s|%30s|%8s|\n", vdd_mst_hst.tx_date, vdd_mst_hst.tx_time, add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal, vdd_mst_hst.brf, vdd_mst_hst.note_type, vdd_mst_hst.note_no);
			vtcp_log("开始打印》|%d|%06d|%s|%17.2f|%17.2f|%s|%32s|%8s|\n", vdd_mst_hst.tx_date, vdd_mst_hst.tx_time, add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal, vdd_mst_hst.brf, vdd_mst_hst.note_type, vdd_mst_hst.note_no);
		}
		else if ( dTol_num ==0 )
		{
			fprintf(fp, "|%d|%06d|%s|%17.2f|%17.2f|%32s|%30s|%8s|\n", vdd_mst_hst.tx_date, vdd_mst_hst.tx_time, add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal, vdd_mst_hst.brf, vdd_mst_hst.note_type, vdd_mst_hst.note_no);
			vtcp_log("开始打印》|%d|%06d|%s|%17.2f|%17.2f|%s|%32s|%8s|\n", vdd_mst_hst.tx_date, vdd_mst_hst.tx_time, add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal, vdd_mst_hst.brf, vdd_mst_hst.note_type, vdd_mst_hst.note_no);
		}			
	}
	
	Dd_mst_hst_Clo_Sel();
	fclose(fp);

	if ( ttlnum==0 )
	{
		strcpy(g_pub_tx.reply, "S049");
		goto ErrExit;
	}

	set_zd_long("0530",ttlnum);
	set_zd_double("0410",j_tx_amt);  /*总支出额*/
	set_zd_double("0420",d_tx_amt);  /*总收入*/
	set_zd_data("0260",filename);
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
		sprintf(acErrMsg,"ftp文件[%s]到平台出错[%d]",filename,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P157");
		goto ErrExit;
	}
	sprintf(acErrMsg,"ftp文件[%s]到平台成功",filename);
	WRITEMSG
	vtcp_log("[%s][%d],ttlnum=[%d],j_tx_amt=[%f],d_tx_amt=[%f],filename=[%s]", __FILE__, __LINE__, ttlnum,j_tx_amt,d_tx_amt,filename);


GoodExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
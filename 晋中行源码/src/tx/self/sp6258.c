/********************************************************
交易名称：网银行内批量转账交易sp6258	 
功能描述：只处理转账类，不处理现金类
作    者: liuzengying (在6242上修改)
完成日期: 2014年12月11日
文件格式：网银流水号|交易日期|序号|转出账号|转入卡号|交易金额|转出户名|转入户名|摘要|
insert into tx_def values('6258','网银行内批量转帐','10000000000000000000000000100000000000001000000010000000000000000001000000000000000000000000000000000000000000000000000000000100','1','1','0');
insert into tx_flow_def values('6258','0','6258','#$');
insert into tx_sub_def values('6258','网银行内批量转帐','sp6258','0','0');
********************************************************/
#define EXTERN
#include "public.h"
#include <errno.h>
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "com_sys_parm_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
#include "chnl_batch_acct_c.h"

sp6258()
{
	struct dd_mst_c	out_dd_mst;
	struct dd_mst_c	in_dd_mst;
	struct com_sys_parm_c	com_sys_parm;
	struct mdm_ac_rel_c out_mdm_ac_rel;
	struct mdm_ac_rel_c in_mdm_ac_rel;
	struct com_item_c com_item;
	struct in_mst_c in_mst;
	struct chnl_batch_acct_c chnl_batch_acct;
	
	char filename[100];
	char filename_ret[100];
	char pcfilename[256]={0};
	char pcfilename_ret[256]={0};
	char fldstr[9][100];
	char tmpstr[1024];
	char errmsg[100]={0};
	char cmd[300];
	char ac_no[21];
	char brf[21];
	char bbmk[5];
	char cSeq[5];
	char cFtpfile[200]={0};
	char sw_traceno[11]={0};  /*平台流水*/
	double tot_amt=0.00;
	double fact_amt=0.00;
	double sucs_amt=0.00;
	long fact_num=0;
	long tot_num=0;
	long sucs_num=0;
	long pt_date=0;
	long lTrace_no=0;
		
	int ret=0;
	int in_flag=0;  /**内部账标志 0 活期户 1 内部账**/
	int trace_cnt=0;
	int i=0,j=0,k=0,l=0;
	char *p;
	FILE *fp=NULL;
	FILE *fp1=NULL;
	int temp=0;
	
	memset(filename,0x0,sizeof(filename));
	memset(filename_ret,0x0,sizeof(filename_ret));
	memset(cmd,0x0,sizeof(cmd));
	memset(cSeq,0x0,sizeof(cSeq));
	memset(ac_no,0x0,sizeof(ac_no));
	memset(bbmk,0x0,sizeof(bbmk));
	memset(brf,0x0,sizeof(brf));
	memset(fldstr,0,sizeof(fldstr));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(&out_dd_mst	,0x00,sizeof(out_dd_mst));
	memset(&in_dd_mst	,0x00,sizeof(in_dd_mst));
	memset(&com_sys_parm	,0x00,sizeof(com_sys_parm));
	memset(&out_mdm_ac_rel	,0x00,sizeof(out_mdm_ac_rel));
	memset(&in_mdm_ac_rel	,0x00,sizeof(in_mdm_ac_rel));
	memset(&com_item	,0x00,sizeof(com_item));
	memset(&in_mst	,0x00,sizeof(in_mst));
	memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
	
	if (pub_base_sysinit())
	{
		vtcp_log("初始化公用结构错误!!");
		strcpy(g_pub_tx.reply, "CU06");
		goto ErrExit;
	}	
	vtcp_log("[%s][%d],tx_date=[%ld]",__FILE__,__LINE__,g_pub_tx.tx_date);
	set_zd_long("0460",g_pub_tx.tx_date);
        
	/********获取平台数据************/
	get_zd_data("0300",ac_no);       /**主账号/科目**/
	get_zd_double("0400",&tot_amt);  /**总金额**/
	get_zd_long("0480",&tot_num);    /**总笔数**/
	get_zd_data("0810",brf);         /**摘要**/
	get_zd_long("0470",&pt_date);    /**平台日期**/
	get_zd_data("0520",sw_traceno);  /**平台流水**/
	get_zd_data("0260",filename);    /**记账文件名**/
	pub_base_strpack(ac_no);
	pub_base_strpack(brf);
	pub_base_strpack_all(filename);
	
	
	/*******测试数据begin*************
	tot_amt=46.00;
	tot_num=4;
	strcpy(brf,"网银行内批量转账");
	pt_date=20141212;
	sprintf(filename,"nbank_batch_20141212_62581.txt");
	*******测试数据end*********/
	
	sprintf(cFtpfile,"%s/bin/ftp_getfile_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename);
	vtcp_log("[%s],[%d],cFtpfile=[%s]",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
		vtcp_log("[%s][%d]::ftp文件[%s]到平台取文件出错[%d]",__FILE__,__LINE__,filename,ret);
		strcpy(g_pub_tx.reply,"P157");
		set_zd_data("0130","到平台取文件出错!");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]::到平台取文件成功[%s]",__FILE__,__LINE__,filename);
	
	
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(ret)
	{
		vtcp_log("[%s][%d]::查询系统日期错误! ret[%d]",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","查询系统日期错误!");	
		goto ErrExit;	
	}


	/******取返回文件名序号*****
	for ( i=0;i<strlen(filename);i++ )
	{
		if(filename[i]=='_') 
		{
			j++;
			if(j==3) k=i+1;
		}
		if(filename[i]=='.')
		{
			l=i+1;
		}
	}		
	memcpy(cSeq,filename+k,l-1-k);**/

	
	sprintf(filename_ret,"nb_bat_res%ld%s.txt",pt_date,sw_traceno);
	sprintf(pcfilename_ret,"%s/nb_bat_res%ld%s.txt",getenv("FILDIR"),pt_date,sw_traceno);
	
	/******读文件，检查文件是否完整*******/
	sprintf(pcfilename,"%s/tmp/%s",getenv("HOME"),filename);	
	vtcp_log("[%s][%s]::pcfilename[%s]",__FILE__,__LINE__,pcfilename);
	pub_base_strpack(pcfilename);
	fp = fopen( pcfilename,"r" );
	if ( fp==NULL )
	{
		vtcp_log("打开文件失败[%s]",pcfilename);
		strcpy(g_pub_tx.reply,"S047");
		set_zd_data("0130","打开文件失败!");
		goto ErrExit;
	}
	
	while(1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );		
    p=fgets(tmpstr,1024,fp);/*取出一条记录*/
		if(feof(fp))
		{
			break;
		}
		pub_base_strpack_all(tmpstr);
		
		memset(fldstr,0x0,sizeof(fldstr));		
		for ( i=0;i<9;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log("[%s][%d]第[%d]值[%s]",__FILE__,__LINE__,i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/************
		fldstr[0] 网银流水号
		fldstr[1] 交易日期
		fldstr[2] 序号
		fldstr[3] 转出账号
		fldstr[4] 转入卡号
		fldstr[5] 交易金额
		fldstr[6] 转出户名
		fldstr[7] 转入户名
		fldstr[8] 摘要
		*******************/
		fact_num++;
		fact_amt+=atof(fldstr[5]);
	}
	fclose(fp);
	
	if(fact_num != tot_num)
	{
		vtcp_log("[%s][%d]::输入总笔数【%ld】与文件总笔数【%ld】不一致!",__FILE__,__LINE__,tot_num,fact_num);
		strcpy(g_pub_tx.reply,"H019");
		set_zd_data("0130","输入总笔数与文件总笔数不一致!");
		goto ErrExit;
	}
		
	if(pub_base_CompDblVal(fact_amt,tot_amt))
	{
		vtcp_log("[%s][%d]::输入总金额【%lf】与文件总金额【%lf】不一致!",__FILE__,__LINE__,tot_amt,fact_amt);
		strcpy(g_pub_tx.reply,"H019");
		set_zd_data("0130","输入总金额与文件总金额不一致!");
		goto ErrExit;
	}
		
	if (pub_base_PubQlsh(&lTrace_no,g_pub_tx.reply))
	{
		sprintf(acErrMsg,"取主机流水号错[%d]",lTrace_no);
		WRITEMSG
		set_zd_data("0130","取主机流水号错!");
		goto ErrExit;
	}
	
	/********读文件，转账************/
	fp = fopen( pcfilename,"r" );
	if ( fp==NULL )
	{
		vtcp_log("打开文件失败[%s]",pcfilename);
		strcpy(g_pub_tx.reply,"S047");
		set_zd_data("0130","打开文件失败!");
		goto ErrExit;
	}
	
	j=0;
	while(1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );		
    p=fgets(tmpstr,1024,fp);/*取出一条记录*/
		if(feof(fp))
		{
			break;
		}
		pub_base_strpack_all(tmpstr);
		
		memset(fldstr,0x0,sizeof(fldstr));
		for ( i=0;i<9;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log("[%s][%d]第[%d]值[%s]",__FILE__,__LINE__,i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/******************
		fldstr[0] 网银流水号
		fldstr[1] 交易日期
		fldstr[2] 序号
		fldstr[3] 转出账号
		fldstr[4] 转入卡号
		fldstr[5] 交易金额
		fldstr[6] 转出户名
		fldstr[7] 转入户名
		fldstr[8] 摘要
		*******************/
		if(j==0)
		{
			fp1 = fopen( pcfilename_ret,"w" );
			if ( fp1==NULL )
			{
				vtcp_log("打开文件失败[%s]",pcfilename);
				strcpy(g_pub_tx.reply,"S047");
				set_zd_data("0130","打开文件失败!");
				goto ErrExit;
			}
		}
		j++;
		/*******转出账号检查**********/
		if(strlen(fldstr[3])==0)
		{
			sprintf(acErrMsg, "转出账号为空,序号[%d]", atoi(fldstr[2]));
			sprintf(errmsg, "转出账号为空");
			WRITEMSG
			goto FAILE;
		}
    else if(strlen(fldstr[3])<=8) /**科目号**/
    {
    	ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",fldstr[3]);
    	if(ret==100){
    		sprintf(acErrMsg, "科目号不存在[%d]", atoi(fldstr[3]));
        WRITEMSG
        goto FAILE;
    	}
    	else if(ret){
    		sprintf(acErrMsg, "查询科目表出错【%d】",ret);
        WRITEMSG
        goto ErrExit;
    	}
    	if(com_item.sub_acc_yn[0]=='Y'){
    		sprintf(acErrMsg, "该科目有下级科目[%d]", atoi(fldstr[3]));
        WRITEMSG
        goto FAILE;
    		}
    	if(com_item.acc_knd[0]!='3'){
    		sprintf(acErrMsg, "该科目下有内部账户[%d]", atoi(fldstr[3]));
        WRITEMSG
        goto FAILE;
    		}
    	/*****内部账标志 1 内部账  0 活期账**********/
    	in_flag=1;
    }
    else 
    {
    	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&out_mdm_ac_rel,"ac_no='%s'",fldstr[3]);
    	if(ret==100){
	    		sprintf(acErrMsg, "转出账号不存在,[%d]", atoi(fldstr[3]));
					sprintf(errmsg, "转出账号不存在");
	        WRITEMSG
	        goto FAILE;
			}
			else if(ret){
	    		sprintf(acErrMsg, "查询Mdm_ac_rel表出错,ret[%d]",ret);
	        WRITEMSG
	        goto ErrExit;
			}
	    if(out_mdm_ac_rel.note_sts[0]!='0'){
				sprintf(acErrMsg, "转出账号介质状态不正常,[%d]", atoi(fldstr[3]));
				sprintf(errmsg, "转出账号介质状态不正常");
				WRITEMSG				
				goto FAILE;
			}
	    	
	    if(fldstr[3][0]=='9') /**内部账号**/
	    {
	    	ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld and ac_seqn=0",out_mdm_ac_rel.ac_id);
	    	if(ret==100){
	    		sprintf(acErrMsg, "内部账号不存在,[%d]", atoi(fldstr[3]));
	        WRITEMSG
	        goto FAILE;
	    	}
	    	else if(ret){
	    		sprintf(acErrMsg, "查询内部主文件出错,ret[%d]",ret);
	        WRITEMSG
	        goto ErrExit;
	    	}
	    	if(in_mst.sts[0]=='*'){
	    		sprintf(acErrMsg, "内部账号已销户,[%d]", atoi(fldstr[3]));
	        WRITEMSG
	        goto FAILE;
				}
	    	in_flag=1;
	    }
	    else 
	    {
	    	ret=Dd_mst_Sel(g_pub_tx.reply,&out_dd_mst,"ac_id=%d and ac_seqn=1",out_mdm_ac_rel.ac_id);
	    	if(ret==100){
	    		sprintf(acErrMsg, "转出账号不存在,[%d]", atoi(fldstr[3]));
					sprintf(errmsg, "转出账号不存在");
	        WRITEMSG
	        goto FAILE;
	    	}
	    	else if(ret){
	    		sprintf(acErrMsg, "查询活期主文件出错,ret[%d]",ret);
	        WRITEMSG
	        goto ErrExit;
	    	}
	    	if(out_dd_mst.ac_sts[0]=='0'){
	    		sprintf(acErrMsg, "转出账号开户未确认,[%d]", atoi(fldstr[3]));
					sprintf(errmsg, "转出账号开户未确认");
	        WRITEMSG
	        goto FAILE;
				}
	    	if(out_dd_mst.ac_sts[0]=='*'){
	    		sprintf(acErrMsg, "转出账号已销户,[%d]", atoi(fldstr[3]));
					sprintf(errmsg, "转出账号已销户");
	        WRITEMSG
	        goto FAILE;
				}
				if(out_dd_mst.hold_sts[0]=='1'){
	    		sprintf(acErrMsg, "转出账号为完全冻结状态,[%d]", atoi(fldstr[3]));
					sprintf(errmsg, "转出账号为完全冻结状态");
	        WRITEMSG
	        goto FAILE;
				}
				if(out_dd_mst.hold_sts[0]=='2'){
	    		sprintf(acErrMsg, "转出账号为只进不出状态,[%d]", atoi(fldstr[3]));
					sprintf(errmsg, "转出账号为只进不出状态");
	        WRITEMSG
	        goto FAILE;
				}
				if( strcmp (out_dd_mst.name,fldstr[6]) ){
	    		sprintf(acErrMsg, "转出账号户名不符,实名[%s]", out_dd_mst.name);
					sprintf(errmsg, "转出账号户名不符,实名[%s]", out_dd_mst.name);
	        WRITEMSG
	        goto FAILE;
				}
				
				in_flag=0;
	    }
  	}
  	/******转入账号检查******/
  	if(strlen(fldstr[4])==0){
			sprintf(acErrMsg, "转入账号为空,序号[%d]", atoi(fldstr[2]));
			sprintf(errmsg, "转入账号为空");
			WRITEMSG
			goto FAILE;
		}
		if(strlen(fldstr[4])!=13 && strlen(fldstr[4])!=19){
			temp=strlen(fldstr[4]);
			sprintf(acErrMsg, "转入账号位数不对,序号[%d]长度[%d]", atoi(fldstr[2]),temp);
			sprintf(errmsg, "转入账号位数不对");
			WRITEMSG
			goto FAILE;
		}
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&in_mdm_ac_rel,"ac_no='%s'",fldstr[4]);
		if(ret==100){
			sprintf(acErrMsg, "转入账号不存在,[%d]", atoi(fldstr[4]));
			sprintf(errmsg, "转入账号不存在");
			WRITEMSG
			goto FAILE;
		}
		else if(ret){
			sprintf(acErrMsg, "查询Mdm_ac_rel表出错,ret[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		if(in_mdm_ac_rel.note_sts[0]!='0'){
			sprintf(acErrMsg, "转入卡号介质状态不正常,[%d]", atoi(fldstr[4]));
			sprintf(errmsg, "转入卡号介质状态不正常");
			WRITEMSG
			goto FAILE;
		}
		ret=Dd_mst_Sel(g_pub_tx.reply,&in_dd_mst,"ac_id=%ld and ac_seqn=1",in_mdm_ac_rel.ac_id);
		if(ret==100){
			sprintf(acErrMsg, "转入账号不存在,[%d]", atoi(fldstr[4]));
			sprintf(errmsg, "转入账号不存在");
			WRITEMSG
			goto FAILE;
		}
		else if(ret){
			sprintf(acErrMsg, "查询活期主文件出错,ret[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		if(in_dd_mst.ac_sts[0]=='0'){
			sprintf(acErrMsg, "转入账号开户未确认,[%d]", atoi(fldstr[4]));
			sprintf(errmsg, "转入账号开户未确认");
			WRITEMSG
			goto FAILE;
		}
		if(in_dd_mst.ac_sts[0]=='*'){
			sprintf(acErrMsg, "转入账号已销户,[%d]", atoi(fldstr[4]));
			sprintf(errmsg, "转入账号已销户");
			WRITEMSG
			goto FAILE;
		}
		if(in_dd_mst.hold_sts[0]=='1'){
			sprintf(acErrMsg, "转入账号为完全冻结状态,[%d]", atoi(fldstr[4]));
			sprintf(errmsg, "转入账号为完全冻结状态");
			WRITEMSG
			goto FAILE;
		}
		if( strcmp(in_dd_mst.name,fldstr[7]) ){
			sprintf(acErrMsg, "转入账号户名不符,实名[%s]", in_dd_mst.name);
			sprintf(errmsg, "转入账号户名不符,实名[%s]", in_dd_mst.name);
			WRITEMSG
			goto FAILE;
		}
		
		/******每次循环初始化全局变量************/
		memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
		pub_base_sysinit();
		/****g_pub_tx结构体赋值----转出账号*********/
		strcpy(g_pub_tx.cur_no, RMB);
		strcpy(g_pub_tx.ct_ind, "2");
		g_pub_tx.trace_no=lTrace_no;
		trace_cnt++;
		g_pub_tx.trace_cnt =trace_cnt;
		
		if(strlen(brf)==0)
			strcpy(g_pub_tx.brf,"网银批量转账转出");
		else
			strcpy(g_pub_tx.brf,brf);
		if(in_flag==0){
			g_pub_tx.svc_ind=1001;
			strcpy(g_pub_tx.sub_tx_code, "D003");
			g_pub_tx.ac_seqn=1;
		}
		else{
			g_pub_tx.svc_ind=9001;
			strcpy(g_pub_tx.sub_tx_code, "A016");
			strcpy(g_pub_tx.ac_id_type,"9");
			g_pub_tx.ac_seqn=0;
		}
		strcpy(g_pub_tx.ac_no,fldstr[3]);
		g_pub_tx.tx_amt1=atof(fldstr[5]);
		strcpy(g_pub_tx.add_ind,"0"); 
		strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
		strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
	
		if ( pub_acct_trance() )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			sprintf(errmsg, "转出账号取款失败");
			WRITEMSG
			goto FAILE;;
		}
		if(in_flag==0){
			set_zd_data("102J","01");
			set_zd_data("102K",g_pub_tx.ct_ind);
			set_zd_double("102F",g_pub_tx.tx_amt1);
		}
		else{
			set_zd_data("1204","01");
			set_zd_data("1205",g_pub_tx.ct_ind);
			set_zd_double("1208",g_pub_tx.tx_amt1);
		}
		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			sprintf(errmsg, "登记会计流水错误");
			WRITEMSG
			goto FAILE;;
		}
		
		/*******转入账号***********/
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
		g_pub_tx.ac_id=0;
		
		strcpy(g_pub_tx.ac_no,fldstr[4]);
		g_pub_tx.ac_seqn=1;
		
		if(strlen(brf)==0)
			strcpy(g_pub_tx.brf,"网银批量转账转入");
		else
			strcpy(g_pub_tx.brf,brf);
		
		g_pub_tx.svc_ind=1001;
		strcpy(g_pub_tx.sub_tx_code, "D099");
		
		g_pub_tx.tx_amt1=atof(fldstr[5]);
			
		strcpy(g_pub_tx.add_ind,"1");
		strcpy(g_pub_tx.ac_id_type,"");
		
		if ( pub_acct_trance() )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			sprintf(errmsg, "转入账号存款失败");
			WRITEMSG
			goto FAILE;;
		}
		set_zd_data("101A","01");
		set_zd_data("101B",g_pub_tx.ct_ind);
		set_zd_double("1013",g_pub_tx.tx_amt1);
		
		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			sprintf(errmsg, "登记会计流水错误");
			WRITEMSG
			goto FAILE;;
		}
		
		/********记完账，登记批量记账登记簿********/
		memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
		chnl_batch_acct.tx_date=com_sys_parm.sys_date;
		sprintf(chnl_batch_acct.tx_time,"%ld",g_pub_tx.tx_time);
		strcpy(chnl_batch_acct.batch_ref,filename);
		sprintf(chnl_batch_acct.dz_ref,"%s%s",fldstr[1],fldstr[2]);
		chnl_batch_acct.trace_no=g_pub_tx.trace_no;
		strcpy(chnl_batch_acct.cur_no,"01");
		strcpy(chnl_batch_acct.ac_no,ac_no);
		strcpy(chnl_batch_acct.ac_no1,fldstr[3]);
		strcpy(chnl_batch_acct.ac_no2,fldstr[4]);
		strcpy(chnl_batch_acct.tx_code,g_pub_tx.tx_code);
		strcpy(chnl_batch_acct.tx_br_no,g_pub_tx.tx_br_no);
		strcpy(chnl_batch_acct.opn_br_no,g_pub_tx.opn_br_no);
		strcpy(chnl_batch_acct.tel,g_pub_tx.tel);
		get_zd_data("0100",chnl_batch_acct.bbmk);
		strcpy(chnl_batch_acct.ttyn,g_pub_tx.tty);
		chnl_batch_acct.pt_date=pt_date;
		get_zd_data("0170",chnl_batch_acct.pt_time);
		get_zd_data("0520",chnl_batch_acct.pt_trace_no);
		chnl_batch_acct.tx_amt=atof(fldstr[5]);
		chnl_batch_acct.ct_ind[0]='2';
		strcpy(chnl_batch_acct.brf,brf);
		chnl_batch_acct.fee_amt=atof(fldstr[6]);
		chnl_batch_acct.sts[0]='1';
		chnl_batch_acct.dz_ind[0]='0';
		ret=Chnl_batch_acct_Ins(chnl_batch_acct,g_pub_tx.reply);
		if(ret){
			sprintf(acErrMsg, "插入批量记账登记簿出错,ret[%d]",ret);
			sprintf(errmsg, "登记批量记账登记簿出错");
			WRITEMSG
			goto FAILE;
		}
		/**fprintf(fp1,"%ld|%s|1|\n",com_sys_parm.sys_date,fldstr[1]);**/
		fprintf(fp1,"%s|%s|%s|%s|%s|%s|%s|%s|%s|Y|%s|\n",fldstr[0],fldstr[1],fldstr[2],fldstr[3],fldstr[4],fldstr[5],fldstr[6],fldstr[7],fldstr[8],"转账成功");
		sql_commit();
		sql_begin();
		vtcp_log("序号[%s]成功,继续下一笔",fldstr[2]);
		sucs_num++;
		sucs_amt+=atof(fldstr[5]);
		continue;

FAILE:
	  /**fprintf(fp1,"%ld|%s|0|\n",com_sys_parm.sys_date,fldstr[1]);**/
		fprintf(fp1,"%s|%s|%s|%s|%s|%s|%s|%s|%s|N|%s|\n",fldstr[0],fldstr[1],fldstr[2],fldstr[3],fldstr[4],fldstr[5],fldstr[6],fldstr[7],fldstr[8],errmsg);
	  sql_rollback();
	  sql_begin();
		vtcp_log("序号[%s]失败,继续下一笔",fldstr[2]);
	  continue;
	}
	
	fclose(fp);	
	fclose(fp1);
	
	memset(cmd,0x0,sizeof(cmd));
	sprintf(cmd,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename_ret);
	vtcp_log("[%s],[%d],cmd=[%s]",__FILE__,__LINE__,cmd);
	ret=system(cmd);
	if(ret)
	{
		vtcp_log("[%s][%d] ftp转账结果文件[%s]到平台出错[%d]", __FILE__, __LINE__,filename_ret,ret);
		set_zd_data("0130","ftp转账结果文件到平台出错!");
		goto ErrExit;
	}

	set_zd_data("0270",filename_ret);
	set_zd_double("0410",sucs_amt); /**成功总金额**/
	set_zd_long("0490",sucs_num);   /**成功总笔数**/

	
OkExit:
	sql_commit();
	set_zd_data("0120","0000");
	vtcp_log("%s,%d,批量转账记账成功!",__FILE__,__LINE__);
	return(0);
	
ErrExit:
	sql_rollback();
	set_zd_data("0120","H034");
	vtcp_log("%s,%d,批量转账记账失败!",__FILE__,__LINE__);
	if(fp!=NULL)
	fclose(fp);
	if(fp1!=NULL)
	fclose(fp1);
	return(1);	
	}
	
/********************************************************
交易名称：批量记账交易sp6242	 
功能描述：只处理转账类，不处理现金类
记账文件：hx_batch_yyyymmdd_序号.txt
文件格式：交易日期|序号|转出账号|转入卡号|交易金额|手续费|手续费出处|摘要
注：手续费出处 0：转出账号出  1：转入账号出
需要修改宏SXFKM----手续费科目
insert into tx_def values('6242','批量记帐','10000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000100','1','1','2','3','');
insert into tx_flow_def values('6242','0','6242','#$');
insert into tx_sub_def values('6242','批量记帐','sp6242','0','0');
wk 20110712
********************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define SXFKM "602101"
#define EXTERN
#include "public.h"
#include <errno.h>
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "com_sys_parm_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
#include "chnl_batch_acct_c.h"
#include "counter_tlog_c.h"
sp6242()
{
	struct dd_mst_c	out_dd_mst;
	struct dd_mst_c	in_dd_mst;
	struct com_sys_parm_c	com_sys_parm;
	struct mdm_ac_rel_c out_mdm_ac_rel;
	struct mdm_ac_rel_c in_mdm_ac_rel;
	struct com_item_c com_item;
	struct in_mst_c in_mst;
	struct chnl_batch_acct_c chnl_batch_acct;
	struct counter_tlog_c g_counter_tlog;
	
	char filename[248];
	char filename_ret[400];
	char fldstr[10][100];
	char tmpstr[1024];
	char cmd[300];
	char ac_no[21];
	char brf[21];
	char bbmk[5];
	char cSeq[5];
	double tot_amt=0.00;
	double fact_amt=0.00;
	long fact_num=0;
	long tot_num=0;
	long pt_date=0;
	long lTrace_no=0;
		
	int ret=0;
	int in_flag=0;/****内部账标志 1 内部账 2 活期户*******/
	int trace_cnt=0;
	int i=0;
	int j=0;
	int k=0;
	int l=0;
	char *p;
	FILE *fp=NULL;
	FILE *fp1=NULL;
	
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
	memset(&g_counter_tlog,0x00,sizeof(struct counter_tlog_c));
	
	pub_base_sysinit();
	vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
	set_zd_long("0460",g_pub_tx.tx_date);
        
	/********获取平台数据***************/
	get_zd_data("0300",ac_no);/*****主账号/科目********/
	pub_base_strpack(ac_no);
	get_zd_double("0400",&tot_amt);/**总金额**/
	get_zd_long("0480",&tot_num);/***总笔数***/
	get_zd_data("0810",brf);
	pub_base_strpack(brf);
	get_zd_long("0440",&pt_date);/****平台日期*****/
	get_zd_data("0830",filename);/**记账文件名***/
	pub_base_strpack_all(filename);
	
	
	
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(ret){
		sprintf( acErrMsg," 查询系统日期错误 " );
		WRITEMSG
		goto ErrExit;
		}
	if(pt_date!=com_sys_parm.sys_date)
	{
		sprintf( acErrMsg," 平台日期与核心日期不相等!!平台【%ld】 核心【%ld】 ",pt_date,com_sys_parm.sys_date);
		WRITEMSG
		goto ErrExit;
	}

	/******取返回文件名序号********/
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
		
	memcpy(cSeq,filename+k,l-1-k);
	
	sprintf(filename_ret,"%s/txt/CNAPS2_batch_%ld_%s.ret",getenv("HOME"),com_sys_parm.sys_date,cSeq);
	/******读文件，检查文件是否完整*******/
	vtcp_log("Filename[%s][%s][%d]",filename,__FILE__,__LINE__);
	pub_base_strpack(filename);
	fp = fopen( filename,"r" );
	if ( fp==NULL )
	{
		sprintf( acErrMsg," 打开文件出错 [%s]  ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	while(1){
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		
    p=fgets(tmpstr,1024,fp);/*取出一条记录*/
		if(feof(fp))
		{
			break;
		}
		pub_base_strpack_all(tmpstr);
		
		memset(fldstr,0x0,sizeof(fldstr));
		for ( i=0;i<8;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/************
		fldstr[0] 交易日期 
		fldstr[1] 序号
		fldstr[2] 转出账号
		fldstr[3] 转入卡号
		fldstr[4] 交易金额
		fldstr[5] 手续费
		fldstr[6] 手续费出处
		fldstr[7] 摘要
		*******************/
		fact_num++;
		fact_amt+=atof(fldstr[4]);
	}
	fclose(fp);
	
	if(fact_num!=tot_num){
		sprintf( acErrMsg," 输入总笔数与文件总笔数不一致【%ld】【%ld】!!！",tot_num,fact_num);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H019");
		goto ErrExit;
		}
		
	if(pub_base_CompDblVal(fact_amt,tot_amt)){
		sprintf( acErrMsg," 输入总金额与文件总金额不一致【%lf】【%lf】!!！",tot_amt,fact_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H019");
		goto ErrExit;
		}
		
		
		if (pub_base_PubQlsh(&lTrace_no, g_pub_tx.reply)){
       sprintf(acErrMsg, "取主机流水号错 [%d]", lTrace_no);
       WRITEMSG
       goto ErrExit;
   	}
	/********读文件，转账************/
	fp = fopen( filename,"r" );
	if ( fp==NULL )
	{
		sprintf( acErrMsg," 打开文件出错 [%s]  ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	j=0;
	while(1){
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		
    p=fgets(tmpstr,1024,fp);/*取出一条记录*/
		if(feof(fp))
		{
			break;
		}
		pub_base_strpack_all(tmpstr);
		
		memset(fldstr,0x0,sizeof(fldstr));
		for ( i=0;i<8;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/************
		fldstr[0] 交易日期 
		fldstr[1] 序号
		fldstr[2] 转出账号
		fldstr[3] 转入卡号
		fldstr[4] 交易金额
		fldstr[5] 手续费
		fldstr[6] 手续费出处
		fldstr[7] 摘要
		*******************/
		if(j==0){
			fp1 = fopen( filename_ret,"w" );
			if ( fp1==NULL )
			{
				sprintf( acErrMsg," 打开文件出错 [%s]  ", filename_ret );
				strcpy( g_pub_tx.reply, "S047" );
				goto ErrExit;
			}
		}
		j++;
		
		
		ret = Counter_tlog_Sel(g_pub_tx.reply, &g_counter_tlog, "", fldstr[0], fldstr[1]);
		vtcp_log("[%s][%d] 查询counter_tlog 平台日期[%ld],平台流水[%s]\n", __FILE__,__LINE__, fldstr[0], fldstr[1]);
		if(ret)
		{
			sprintf(acErrMsg, "查询柜面通业务表出错【%d】",ret);
	    WRITEMSG
	    goto ErrExit;
		}
		if(ret == 100)
		{
			/*******转出账号检查**********/
			if(strlen(fldstr[2])==0){
				sprintf(acErrMsg, "转出账号为空,序号[%d]", atoi(fldstr[1]));
	       WRITEMSG
	       goto FAILE;
	     }
	    else if(strlen(fldstr[2])<=8)/******科目号*******/
	    {
	    	ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",fldstr[2]);
	    	if(ret==100){
	    		sprintf(acErrMsg, "科目号不存在[%s]", atoi(fldstr[2]));
	        WRITEMSG
	        goto FAILE;
	    	}
	    	else if(ret){
	    		sprintf(acErrMsg, "查询科目表出错【%d】",ret);
	        WRITEMSG
	        goto ErrExit;
	    	}
	    	if(com_item.sub_acc_yn[0]=='Y'){
	    		sprintf(acErrMsg, "该科目有下级科目[%s]", atoi(fldstr[2]));
	        WRITEMSG
	        goto FAILE;
	    		}
	    	if(com_item.acc_knd[0]!='3'){
	    		sprintf(acErrMsg, "该科目下有内部账户[%s]", atoi(fldstr[2]));
	        WRITEMSG
	        goto FAILE;
	    		}
	    	/*****内部账标志 1 内部账  0 活期账**********/
	    	in_flag=1;
	    }
	    else 
	    {
	    	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&out_mdm_ac_rel,"ac_no='%s'",fldstr[2]);
	    	if(ret==100){
		    		sprintf(acErrMsg, "转出账号不存在,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	else if(ret){
		    		sprintf(acErrMsg, "查询Mdm_ac_rel表出错【%d】",ret);
		        WRITEMSG
		        goto ErrExit;
		    	}
		    if(out_mdm_ac_rel.note_sts[0]!='0'){
		    	sprintf(acErrMsg, "转出账号介质状态不正常,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	
		    if(fldstr[2][0]=='9')/***内部账号******/
		    {
		    	ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld and ac_seqn=0",out_mdm_ac_rel.ac_id);
		    	if(ret==100){
		    		sprintf(acErrMsg, "内部账号不存在,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	else if(ret){
		    		sprintf(acErrMsg, "查询内部主文件出错【%d】",ret);
		        WRITEMSG
		        goto ErrExit;
		    	}
		    	if(in_mst.sts[0]=='*'){
		    		sprintf(acErrMsg, "内部账号已销户,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    		}
		    	in_flag=1;
		    }
		    else 
		    {
		    	ret=Dd_mst_Sel(g_pub_tx.reply,&out_dd_mst,"ac_id=%d and ac_seqn=1",out_mdm_ac_rel.ac_id);
		    	if(ret==100){
		    		sprintf(acErrMsg, "活期账号不存在,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	else if(ret){
		    		sprintf(acErrMsg, "查询活期主文件出错【%d】",ret);
		        WRITEMSG
		        goto ErrExit;
		    	}
		    	if(out_dd_mst.ac_sts[0]=='0'){
		    		sprintf(acErrMsg, "转出账号开户未确认,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    		}
		    	if(out_dd_mst.ac_sts[0]=='*'){
		    		sprintf(acErrMsg, "转出账号已销户,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    		}
		    		if(out_dd_mst.hold_sts[0]=='1'){
		    		sprintf(acErrMsg, "转出账号为完全冻结状态,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    		}
		    		if(out_dd_mst.hold_sts[0]=='2'){
		    		sprintf(acErrMsg, "转出账号为只进不出状态,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    		}
		    		in_flag=0;
		    }
	  	}
	  	/****转入账号检查*****/
	  	if(strlen(fldstr[3])==0){
				sprintf(acErrMsg, "转出账号为空,序号[%d]", atoi(fldstr[1]));
	       WRITEMSG
	       goto FAILE;
	     }
	     if(strlen(fldstr[3])!=13 ||strlen(fldstr[3])!=19){
				sprintf(acErrMsg, "转出账号位数不对,序号[%d]", atoi(fldstr[1]));
	       WRITEMSG
	       goto FAILE;
	     }
	  	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&in_mdm_ac_rel,"ac_no='%s'",fldstr[3]);
	    	if(ret==100){
		    		sprintf(acErrMsg, "转入卡号不存在,[%s]", atoi(fldstr[3]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	else if(ret){
		    		sprintf(acErrMsg, "查询Mdm_ac_rel表出错【%d】",ret);
		        WRITEMSG
		        goto ErrExit;
		    	}
		   if(in_mdm_ac_rel.note_sts[0]!='0')
		   {
	   			sprintf(acErrMsg, "转入卡号介质状态不正常,[%s]", atoi(fldstr[3]));
	        WRITEMSG
	        goto FAILE;
		   }
		   ret=Dd_mst_Sel(g_pub_tx.reply,&in_dd_mst,"ac_id=%ld and ac_seqn=1",in_mdm_ac_rel.ac_id);
		   if(ret==100){
		    		sprintf(acErrMsg, "转入账号不存在,[%s]", atoi(fldstr[3]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	else if(ret){
		    		sprintf(acErrMsg, "查询活期主文件出错【%d】",ret);
		        WRITEMSG
		        goto ErrExit;
		    	}
		    
		    if(in_dd_mst.ac_sts[0]=='0'){
		    		sprintf(acErrMsg, "转入账号开户未确认,[%s]", atoi(fldstr[3]));
		        WRITEMSG
		        goto FAILE;
		    		}
	  		if(in_dd_mst.ac_sts[0]=='*'){
		  		sprintf(acErrMsg, "转入账号已销户,[%s]", atoi(fldstr[3]));
		      WRITEMSG
		      goto FAILE;
	  		}
	  		if(out_dd_mst.hold_sts[0]=='1'){
		  		sprintf(acErrMsg, "转入账号为完全冻结状态,[%s]", atoi(fldstr[3]));
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
				strcpy(g_pub_tx.brf,"批量转账转出");
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
			strcpy(g_pub_tx.ac_no,fldstr[2]);
			g_pub_tx.tx_amt1=atof(fldstr[4]);
			strcpy(g_pub_tx.add_ind,"0"); 
			strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
			strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
			strcpy(g_pub_tx.ac_get_ind, "00");
			strcpy(g_pub_tx.note_type,"");
			strcpy(g_pub_tx.beg_note_no,"");
		
			if ( pub_acct_trance() )
			{
				sprintf(acErrMsg,"调用存取款主控失败!");
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
				WRITEMSG
				goto FAILE;;
			}
			/*******转入账号***********/
			memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
			memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
			memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
			memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
			g_pub_tx.ac_id=0;
			
			strcpy(g_pub_tx.ac_no,fldstr[3]);
			g_pub_tx.ac_seqn=1;
			
			if(strlen(brf)==0)
				strcpy(g_pub_tx.brf,"批量转账转入");
			else 
				strcpy(g_pub_tx.brf,brf);
			
			g_pub_tx.svc_ind=1001;
			strcpy(g_pub_tx.sub_tx_code, "D099");
			
			g_pub_tx.tx_amt1=atof(fldstr[4]);
				
			strcpy(g_pub_tx.add_ind,"1"); 
			strcpy(g_pub_tx.ac_id_type,"");
			
			if ( pub_acct_trance() )
			{
				sprintf(acErrMsg,"调用存取款主控失败!");
				WRITEMSG
				goto FAILE;;
			}
			set_zd_data("101A","01");
			set_zd_data("101B",g_pub_tx.ct_ind);
			set_zd_double("1013",g_pub_tx.tx_amt1);
			
			
			if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
			{
				sprintf(acErrMsg,"登记会计流水错误!");
				WRITEMSG
				goto FAILE;;
			}
			
			/******手续费收入********/
			if(atof(fldstr[5])>0.0005)
			{
				/******收手续费*******/
				memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
				memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
				memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
				memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
				g_pub_tx.ac_id=0;
				g_pub_tx.ac_seqn=0;
				
				strcpy(g_pub_tx.ac_no, SXFKM); /* 手续费科目 */
				g_pub_tx.tx_amt1 =atof(fldstr[5]) ;
				strcpy(g_pub_tx.ac_id_type, "9");	/* 账户类型为内部 */
				strcpy(g_pub_tx.ac_get_ind, "00");
				g_pub_tx.svc_ind = 9001;
				strcpy(g_pub_tx.add_ind, "1");
	
				strcpy(g_pub_tx.sub_tx_code, "A017");
	
				set_zd_data("1214","01");
				set_zd_data("1215","2");
				set_zd_double("1218",g_pub_tx.tx_amt1);
				strcpy(g_pub_tx.brf,"手续费收入");/***摘要***/
				strcpy(g_pub_tx.ct_ind,"2");/**现转标志**/
				strcpy(g_pub_tx.ac_wrk_ind,"001001001");
				strcpy(g_pub_tx.hst_ind,"1");/***日间入明细***/
				
				ret = pub_acct_trance();
				if (ret) {
					sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
					WRITEMSG
					goto FAILE;
				}
				ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
				if (ret) {
					sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
					WRITEMSG
					goto FAILE;
				}
				/********扣手续费*************/
				memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
				memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
				memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
				memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
				g_pub_tx.ac_id=0;
				
				g_pub_tx.tx_amt1 =atof(fldstr[5]) ;
				strcpy(g_pub_tx.ac_get_ind, "00");
				strcpy(g_pub_tx.add_ind, "0");
				strcpy(g_pub_tx.brf,"手续费");/***摘要***/
				strcpy(g_pub_tx.ac_wrk_ind,"001001001");
				strcpy(g_pub_tx.hst_ind,"1");/***日间入明细***/
				if(fldstr[6][0]=='0'){/******转出账号出***********/
					strcpy(g_pub_tx.ac_no, fldstr[2]);
					if(in_flag==0){/***活期户******/
						g_pub_tx.ac_seqn=1;
						strcpy(g_pub_tx.sub_tx_code,"D003");
						g_pub_tx.svc_ind=1001;
						strcpy(g_pub_tx.ac_id_type, "");
						
						set_zd_data("102J","01");
						set_zd_data("102K","2");
						set_zd_double("102F",g_pub_tx.tx_amt1);
					}
					else{/****内部户******/
						g_pub_tx.ac_seqn=0;
						strcpy(g_pub_tx.sub_tx_code,"A016");
						g_pub_tx.svc_ind=9001;
						strcpy(g_pub_tx.ac_id_type, "9");	/* 账户类型为内部 */
						set_zd_data("1204","01");
						set_zd_data("1205","2");
						set_zd_double("1208",g_pub_tx.tx_amt1);
					}
				}
				else{/***转入账号出*******/
					strcpy(g_pub_tx.ac_no, fldstr[3]); 
					g_pub_tx.ac_seqn=1;
					strcpy(g_pub_tx.sub_tx_code,"D099");
					g_pub_tx.svc_ind=1001;
					strcpy(g_pub_tx.ac_id_type, "");
					
					set_zd_data("102J","01");
					set_zd_data("102K","2");
					set_zd_double("102F",g_pub_tx.tx_amt1);
				}
				
				
				ret = pub_acct_trance();
				if (ret) {
					sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
					WRITEMSG
					goto FAILE;
				}
				ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
				if (ret) {
					sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
					WRITEMSG
					goto FAILE;
				}
			}
			/********记完账，登记批量记账登记簿********/
			memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
			chnl_batch_acct.tx_date=com_sys_parm.sys_date;
			sprintf(chnl_batch_acct.tx_time,"%ld",g_pub_tx.tx_time);
			strcpy(chnl_batch_acct.batch_ref,filename);
			sprintf(chnl_batch_acct.dz_ref,"%s%s",fldstr[0],fldstr[1]);
			chnl_batch_acct.trace_no=g_pub_tx.trace_no;
			strcpy(chnl_batch_acct.cur_no,"01");
			strcpy(chnl_batch_acct.ac_no,ac_no);
			strcpy(chnl_batch_acct.ac_no1,fldstr[2]);
			strcpy(chnl_batch_acct.ac_no2,fldstr[3]);
			strcpy(chnl_batch_acct.tx_code,g_pub_tx.tx_code);
			strcpy(chnl_batch_acct.tx_br_no,g_pub_tx.tx_br_no);
			strcpy(chnl_batch_acct.opn_br_no,g_pub_tx.opn_br_no);
			strcpy(chnl_batch_acct.tel,g_pub_tx.tel);
			get_zd_data("0100",chnl_batch_acct.bbmk);
			strcpy(chnl_batch_acct.ttyn,g_pub_tx.tty);
			chnl_batch_acct.pt_date=pt_date;
			get_zd_data("0170",chnl_batch_acct.pt_time);
			get_zd_data("0520",chnl_batch_acct.pt_trace_no);
			chnl_batch_acct.tx_amt=atof(fldstr[4]);
			chnl_batch_acct.ct_ind[0]='2';
			strcpy(chnl_batch_acct.brf,brf);
			chnl_batch_acct.fee_amt=atof(fldstr[5]);
			chnl_batch_acct.sts[0]='1';
			chnl_batch_acct.dz_ind[0]='0';
			ret=Chnl_batch_acct_Ins(chnl_batch_acct,g_pub_tx.reply);
			if(ret){
				sprintf(acErrMsg, "插入批量记账登记簿出错[%d]",ret);
				WRITEMSG
				goto FAILE;
				}
			fprintf(fp1,"%ld|%s|1|\n",com_sys_parm.sys_date,fldstr[1]);	
			sql_commit();
			sql_begin();
			continue;
	FAILE:
			vtcp_log("wkwkwk111");
		  fprintf(fp1,"%ld|%s|0|\n",com_sys_parm.sys_date,fldstr[1]);
		  vtcp_log("wkwkwk222");
		  sql_rollback();
		  vtcp_log("wkwkwk333");
		  sql_begin();
		  vtcp_log("wkwkwk444");
		  continue;
		}
		fclose(fp);
		
		if(fp1!=NULL){
			fclose(fp1);
			memset(cmd,0x0,sizeof(cmd));
			sprintf(cmd,"send_back.sh %s",filename_ret);
			ret=system(cmd);
			if(ret){
				sprintf(acErrMsg, "回传文件出错[%d]",ret);
				WRITEMSG
				goto ErrExit;
				}
			set_zd_data("0830","");
			set_zd_data("0830",filename_ret);
		}
	}
OkExit:
	sql_commit();
	vtcp_log("%s,%d,批量转账记账成功!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	set_zd_data(DC_GET_MSG,g_pub_tx.reply);
	return(0);
ErrExit:
	sql_rollback();
	if(strlen(g_pub_tx.reply)==0||!strcmp(g_pub_tx.reply,"0000"))
	{
		strcpy(g_pub_tx.reply,"D103");
	}
	vtcp_log("%s,%d,批量转账记账失败!",__FILE__,__LINE__);
	if(fp!=NULL)
	fclose(fp);
	if(fp1!=NULL)
	fclose(fp1);
	WRITEMSG
	return(1);
}
	

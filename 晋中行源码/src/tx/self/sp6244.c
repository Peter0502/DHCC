/********************************************************
交易名称：批量对账交易sp6244	 
功能描述：只处理转账类，不处理现金类
对账文件：hx_batch_compare_yyyymmdd.txt
文件格式：平台日期|平台流水号|交易总金额|总笔数|
发来的对账文件中不包括冲正和交易无效状态的记录
产生的对帐结果文件名：txt目录下 机构号+终端号  
格式：对账日期|平台流水|平台总笔数|平台总金额|核心总笔数|核心总金额|对账结果|");
										汇总：成功笔数：。失败笔数：
对账成功的记录，chnl_batch_acct里dz_ind置为1；失败不变，为0。
insert into tx_def values('6244','批量对帐','10000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000100','1','1','2','3','');
insert into tx_flow_def values('6244','0','6244','#$');
insert into tx_sub_def values('6244','批量对帐','sp6244','0','0');
wk 20110712
********************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "com_sys_parm_c.h"
#include "chnl_batch_acct_c.h"
sp6244()
{
	struct com_sys_parm_c	com_sys_parm;
	struct chnl_batch_acct_c chnl_batch_acct;
	
	char filename[60];
	char cFilename[80];
	char all_filename[248];/***全路径**/
	char fldstr[10][100];
	char tmpstr[1024];
	char brf[21];
	char lst_trace_no[12];
	double tot_amt=0.00;
	double fact_amt=0.00;
	double fail_amt=0.00;
	long fact_num=0;
	long tot_num=0;
	long fail_num=0;
	long ttl_fail_num=0;/***失败总笔数**/
	long ttl_suc_num=0;/*****成功总笔数*****/
	long pt_date=0;
		
	int ret=0;
	int i=0;
	char *p;
	FILE *fp=NULL;
	FILE *new_fp=NULL;
	
	memset(filename,0x0,sizeof(filename));
	memset(cFilename,0x0,sizeof(cFilename));
	memset(lst_trace_no,0x0,sizeof(lst_trace_no));
	memset(brf,0x0,sizeof(brf));
	memset(fldstr,0,sizeof(fldstr));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(all_filename,0,sizeof(all_filename));
	memset(&com_sys_parm	,0x00,sizeof(com_sys_parm));
	memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
	
	pub_base_sysinit();
	vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
        set_zd_long("0460",g_pub_tx.tx_date);
	
	/********获取平台数据***************/
	
	get_zd_data("0810",brf);
	pub_base_strpack(brf);
	get_zd_long("0450",&pt_date);/****核心对账日期*****/
	get_zd_data("0830",filename);/**记账文件名***/
	
	
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(ret){
		sprintf( acErrMsg," 查询系统日期错误 " );
		WRITEMSG
		goto ErrExit;
		}
	if(pt_date!=com_sys_parm.sys_date)
	{
		sprintf( acErrMsg," 对账日期与核心日期不相等!!平台【%ld】 核心【%ld】 ",pt_date,com_sys_parm.sys_date);
		WRITEMSG
		goto ErrExit;
	}
	
	/*****打开对账结果文件**********/
	pub_base_AllDwnFilName( cFilename );
	vtcp_log("[%s][%d]cFilename=[%s]",__FILE__,__LINE__,cFilename);
	new_fp=fopen(cFilename,"w");
	if( new_fp==NULL )
	{
		vtcp_log("[%s][%d]打开文件[%s]错",__FILE__,__LINE__,filename);
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}
	fprintf(new_fp,"~@对账日期|@平台流水|@平台总笔数|@平台总金额|@核心总笔数|@核心总金额|@对账结果|\n");

	/******读文件，检查文件是否完整*******/
	vtcp_log("filename[%s][%s][%d]",filename,__FILE__,__LINE__);
	pub_base_strpack(filename);
	sprintf(all_filename,"%s/txt/%s",getenv("HOME"),filename);

	pub_base_strpack(all_filename);
	fp = fopen( all_filename,"r" );
	if ( fp==NULL )
	{
		sprintf( acErrMsg," 打开文件出错 [%s]  ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	while(1){
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		pt_date=0;
		tot_amt=0.00;
		fact_amt=0.00;
		tot_num=0;
		fact_num=0;
		
		
    p=fgets(tmpstr,1024,fp);/*取出一条记录*/
		if(feof(fp))
		{
			break;
		}
		pub_base_strpack_all(tmpstr);
		
		memset(fldstr,0x0,sizeof(fldstr));
		for ( i=0;i<4;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/************
		fldstr[0] 平台日期 
		fldstr[1] 平台流水号
		fldstr[2] 交易总金额
		fldstr[3] 交易总笔数
		*******************/
		pt_date=atol(fldstr[0]);
		
		tot_amt=atof(fldstr[2]);
		tot_num=atol(fldstr[3]);
		vtcp_log("wkwk==========");
		/*******查找批量登记簿*******/
		ret=sql_sum_double("chnl_batch_acct","tx_amt",&fact_amt,"tx_date=%ld and pt_trace_no='%s' and sts not in('0','4') ",pt_date,fldstr[1]);
		if(ret){
			sprintf( acErrMsg," 计算总金额出错，平台流水【%s】!!！",fldstr[1]);
			WRITEMSG
			goto ErrExit;
			}
		vtcp_log("fact_amt==[%lf]",fact_amt);
		fact_num=sql_count("chnl_batch_acct","tx_date=%ld and pt_trace_no='%s' and sts not in('0','4')",pt_date,fldstr[1]);
		vtcp_log("fact_num==[%ld]",fact_num);
		
		/****对帐*****/
		if(tot_num!=fact_num || pub_base_CompDblVal(tot_amt,fact_amt)!=0)/*******不等********/
		{
			ttl_fail_num++;
			fprintf(new_fp,"%ld|%s|%ld|%.2f|%ld|%.2f|失败|\n",pt_date,fldstr[1],tot_num,tot_amt,fact_num,fact_amt);
			
		}
		else{/*******相等*******/
			ret=sql_execute("update chnl_batch_acct set dz_ind='1' where tx_date=%ld and pt_trace_no='%s' and sts not in('0','4') ",pt_date,fldstr[1]);
			if(ret){
				sprintf( acErrMsg," 更新对账状态出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
				}
			ttl_suc_num++;
			fprintf(new_fp,"%ld|%s|%ld|%.2f|%ld|%.2f|成功|\n",pt_date,fldstr[1],tot_num,tot_amt,fact_num,fact_amt);
			
		}
		
	}
	fclose(fp);
	/*****检查一下，核心有没有多的**********/
	fact_num=0;/******总的笔数*******/
	fail_num=0;/*****同一流水的总笔数*******/
	fail_amt=0.00;/*******同一流水的总金额********/
	ret=Chnl_batch_acct_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and dz_ind='0' and sts not in('0','4') order by pt_trace_no",com_sys_parm.sys_date);
	if(ret){
		sprintf( acErrMsg," 打开游标出错!!ret【%d】",ret);
		WRITEMSG
		goto ErrExit;
		}
	while(1){
		memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
		ret=Chnl_batch_acct_Fet_Upd(&chnl_batch_acct,g_pub_tx.reply);
		if(ret==100) {/***最后一个*****/
			if(fact_num!=0){
				fprintf(new_fp,"%ld|%s|%ld|%.2f|%ld|%.2f|失败|\n",com_sys_parm.sys_date,lst_trace_no,0,0.00,fail_num,fail_amt);
				ttl_fail_num++;
				}
			break;
			}
		else if(ret){
		sprintf( acErrMsg," FET出错!!ret【%d】",ret);
		WRITEMSG
		goto ErrExit;
		}
		vtcp_log("lst_trace_no====[%s]",lst_trace_no);
		if(strlen(lst_trace_no)!=0 && memcmp(lst_trace_no,chnl_batch_acct.pt_trace_no,sizeof(chnl_batch_acct.pt_trace_no)-1)){/***换流水了***/
			fprintf(new_fp,"%ld|%s|%ld|%.2f|%ld|%.2f|失败|\n",com_sys_parm.sys_date,lst_trace_no,0,0.00,fail_num,fail_amt);
			ttl_fail_num++;
			fail_num=0;/********清0*********/
			fail_amt=0.00;
			}
		fact_num++;
		fail_num++;
		fail_amt+=chnl_batch_acct.tx_amt;
		memcpy(lst_trace_no,chnl_batch_acct.pt_trace_no,sizeof(chnl_batch_acct.pt_trace_no)-1);
		ret=Chnl_batch_acct_Upd_Upd(chnl_batch_acct,g_pub_tx.reply);
		if(ret){
			sprintf( acErrMsg," 更新登记簿状态出错【%d】!!",ret);
			WRITEMSG
			goto ErrExit;
			}
	}
	Chnl_batch_acct_Clo_Upd();
	
	/*****汇总**********/
	fprintf(new_fp,"汇总：成功笔数：%ld。失败笔数：%ld\n",ttl_suc_num,ttl_fail_num);
	fclose(new_fp);
	set_zd_data( DC_FILE_SND,"1" );

	
	
OkExit:
	sql_commit();
	vtcp_log("%s,%d,批量对账完成!",__FILE__,__LINE__);
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
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	set_zd_data(DC_GET_MSG,acErrMsg);
	vtcp_log("%s,%d,批量对账失败!",__FILE__,__LINE__);
	if(fp!=NULL)
	fclose(fp);
	WRITEMSG
	return(1);
	}
	
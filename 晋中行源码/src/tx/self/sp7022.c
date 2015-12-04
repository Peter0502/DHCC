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

insert into tx_def values('7022','批量记帐','10000000000000000000000000000000000000000000010000100010000000000001000000000000111000000000000000000000000000000000000000000100','1','1','0');
insert into tx_flow_def values('7022','0','7022','#$');
insert into tx_sub_def values('7022','批量记帐','sp7022','0','0');

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
#include "prdt_ac_id_c.h"
#include "chnl_batch_acct_c.h"

sp7022()
{
	struct dd_mst_c	out_dd_mst;
	struct dd_mst_c	in_dd_mst;
	struct com_sys_parm_c	com_sys_parm;
	struct mdm_ac_rel_c out_mdm_ac_rel;
	struct mdm_ac_rel_c in_mdm_ac_rel;
	struct com_item_c com_item;
	struct in_mst_c in_mst;
	struct chnl_batch_acct_c chnl_batch_acct;
	struct com_item_c sCom_item;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct in_mst_c sIn_mst;
	
	char filename[248];
	char filename_ret[400];
	char filename_rt[400];
	char filename_full[248];
	char fldstr[25][100];
	char tmpstr[1024];
	char cmd[300];
	char ac_no[21];
	char brf[21];
	char bbmk[5];
	char c_opn_br_no[BRNO_LEN+1];
	char cSeq[11];
	char cCtrlcmd[11] = {0};    /**控制标志 [5]:收款人挂帐标志**/
	char cGz_ind[2] = {0};/*挂账标志*/
	char sGZGZ[2];/*强制挂账临时存储*/
	char cGz_br_no[6];/*挂账机构*/
	char cGz_reason[2];/*挂账原因 1：账号不存在 2：户名不符
				3：账户状态非法 4：介质状态非法 5：冻结标志非法
				6：止付标志非法 7：其他原因 */
	char cHx_term[5];/*终端号 COMM-通用 HVPS-大额 BEPS-小额*/
	double tot_amt=0.00;
	double fact_amt=0.00;
	long fact_num=0;
	long tot_num=0;
	long pt_date=0;
	long lTrace_no=0;
	
	int i=0;
	int j=0;
	int k=0;
	int l=0;
	int ret=0;
	int in_flag=0;/****内部账标志 1 内部账 2 活期户*******/
	int in_flag2=0;/****内部账标志 1 内部账 2 活期户*******/
	int trace_cnt=0;
	int iParm_seqn = 0;/*com_parm 表 参数序号*/
	char sSndRcv[5] = {0};/*往来标识*/
	char *p;
	FILE *fp=NULL;
	FILE *fp1=NULL;
	
	char	sQdbs[10] = {0};/*Added by YangMeng 20140703 是否登记关联信息*/
	char	cFd125[282] = {0};/*Added by YangMeng 20140703 登记交易关联变量定义*/
	char	sRz_br_no[BRNO_LEN+1] = {0};/*Added by YangMeng 2014/7/9 10:54:46*/
	
	memset ( &g_pub_tx , 0x00 , sizeof(struct pub_tx) );
	memset(filename,0x0,sizeof(filename));
	memset(filename_full,0x0,sizeof(filename_full));
	memset(filename_ret,0x0,sizeof(filename_ret));
	memset(filename_rt,0x0,sizeof(filename_rt));
	memset(cmd,0x0,sizeof(cmd));
	memset(cSeq,0x0,sizeof(cSeq));
	memset(ac_no,0x0,sizeof(ac_no));
	memset(bbmk,0x0,sizeof(bbmk));
	memset(brf,0x0,sizeof(brf));
	memset(fldstr,0,sizeof(fldstr));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(cGz_br_no,0x00,sizeof(cGz_br_no));
	memset(cGz_reason,0x00,sizeof(cGz_reason));
	memset(cHx_term,0x00,sizeof(cHx_term));
	memset(c_opn_br_no,0x00,sizeof(c_opn_br_no));
	memset(&out_dd_mst	,0x00,sizeof(out_dd_mst));
	memset(&in_dd_mst	,0x00,sizeof(in_dd_mst));
	memset(&com_sys_parm	,0x00,sizeof(com_sys_parm));
	memset(&out_mdm_ac_rel	,0x00,sizeof(out_mdm_ac_rel));
	memset(&in_mdm_ac_rel	,0x00,sizeof(in_mdm_ac_rel));
	memset(&com_item	,0x00,sizeof(com_item));
	memset(&in_mst	,0x00,sizeof(in_mst));
	memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
	memset(&sCom_item,0x00,sizeof(sCom_item));
	memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
	memset(&sIn_mst,0x00,sizeof(sIn_mst));
	
	pub_base_sysinit();
	set_zd_long("0460",g_pub_tx.tx_date);
	vtcp_log("[%s][%d],核心日期tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    
	/********获取平台数据***************/
	get_zd_data("0100",cHx_term);
	get_zd_data("0280",sQdbs);/*****是否登记关联信息*******/
	get_zd_data("0300",ac_no);/*****主账号/科目*这东西基本没用*******/
	pub_base_strpack(ac_no);
	get_zd_double("0400",&tot_amt);/**总金额**/
	get_zd_long("0480",&tot_num);/***总笔数***/
	get_zd_long("0440",&pt_date);/****平台日期*****/
	get_zd_data("0650",sSndRcv);
	get_zd_data("0810",brf);
	pub_base_strpack(brf);
	get_zd_data("0830",filename);/**记账文件名***/
	vtcp_log("[%s][%d],往来标识sSndRcv=[%s]",__FILE__,__LINE__,sSndRcv);
	vtcp_log("[%s][%d],tot_amt=[%f]",__FILE__,__LINE__,tot_amt);
	pub_base_strpack_all(filename);
	
	memcpy(c_opn_br_no,g_pub_tx.opn_br_no,sizeof(g_pub_tx.opn_br_no)-1);
	vtcp_log("%s,%d,开户机构c_opn_br_no=[%s]!",__FILE__,__LINE__,c_opn_br_no);
	vtcp_log("%s,%d,开户机构g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
	
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(ret){
		sprintf( acErrMsg," 查询系统日期错误 " );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	/*  这里不检查日期了
	if(pt_date!=com_sys_parm.sys_date)
	{
		sprintf( acErrMsg," 平台日期与核心日期不相等!!平台【%ld】 核心【%ld】 ",pt_date,com_sys_parm.sys_date);
		WRITEMSG
		goto ErrExit;
	}
	*/
	
	if(memcmp(cHx_term,"HVPS",4) == 0)
	{
		iParm_seqn = 6;
	}else{
		iParm_seqn = 8;
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
	vtcp_log("I的值：[%d][%s][%d]",i,__FILE__,__LINE__);
	vtcp_log("J的值：[%d][%s][%d]",j,__FILE__,__LINE__);
	vtcp_log("K的值：[%d][%s][%d]",k,__FILE__,__LINE__);
	vtcp_log("L的值：[%d][%s][%d]",l,__FILE__,__LINE__);
	memcpy(cSeq,filename+k,l-1-k);
	vtcp_log("cSeq的值：[%s][%s][%d]",cSeq,__FILE__,__LINE__);
	
	sprintf(filename_ret,"%s/txt/CNAPS2_batch_%ld_%s.ret",getenv("HOME"),com_sys_parm.sys_date,cSeq);
	sprintf(filename_rt,"CNAPS2_batch_%ld_%s.ret",com_sys_parm.sys_date,cSeq);
	/******读文件，检查文件是否完整*******/
	pub_base_strpack(filename);
	sprintf(filename_full,"%s/txt/%s",getenv("HOME"),filename);
	vtcp_log("Filename[%s][%s][%d]",filename_full,__FILE__,__LINE__);
	fp = fopen(filename_full,"r");
	if (fp==NULL)
	{
		sprintf( acErrMsg," 打开文件出错 [%s]  ", filename_full );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	while(1){
		memset(tmpstr, 0x0, sizeof(tmpstr));
		p = fgets(tmpstr,1024,fp);/*取出一条记录*/
		
		if(feof(fp))
		{
			break;
		}
		pub_base_strpack_all(tmpstr);
		
		/*Modefied by YangMeng 20140704 渠道交易增加相应的业务信息(对方账号、户名等)begin*/
		memset(fldstr,0x0,sizeof(fldstr));
		for (i=0;i<25;i++)
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log("[%s]，[%d]第 [%d] 值[%s] ",__FILE__,__LINE__,i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/**************************************************
		fldstr[0] 委托日期             fldstr[12] 发起行行号         
		fldstr[1] 支付序号	           fldstr[13] 平台日期         
		fldstr[2] 转出账号             fldstr[14] 平台流水         
		fldstr[3] 转入卡号	           fdstr[15]  往来标示           
		fldstr[4] 交易金额             fldstr[16] 交易账号行内账户  
		fldstr[5] 手续费               fldstr[17] 现转标志 1现 2转  
		fldstr[6] 手续费出处           fldstr[18] 对方行行号        
		fldstr[7] 过渡账号             fldstr[19] 对方账号          
		fldstr[8] 入账户名             fldstr[20] 对方名称          
		fldstr[9] 强制挂账标识         fldstr[21] 业务类型编号      
		fldstr[10] 挂账机构            fldstr[22] 业务种类          
		fldstr[11] 摘要                fldstr[23] 收款人名称        
		*******************************************************/
		if (memcmp(sQdbs,"DSF",3)){
			sprintf(cFd125, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", 
				fldstr[13],fldstr[14],fldstr[0],fldstr[1],
				fldstr[12],fldstr[15],fldstr[16],fldstr[23],"",
				fldstr[17],fldstr[4],fldstr[18],fldstr[19],
				fldstr[20],"","",fldstr[21],fldstr[22],"3",fldstr[24]
			);
			vtcp_log("[%s],[%d],渠道信息cFd125==[%s]",__FILE__,__LINE__,cFd125);
		
			ret = pubf_com_checkin_txdetail(cFd125);
			if (ret)
			{
				sprintf(acErrMsg,"%s,%d,执行渠道交易增加相应业务信息函数失败",__FILE__,__LINE__,ret);
				/*WRITEMSG
				strcpy(g_pub_tx.reply,"D107");
				goto ErrExit;	
				continue;*/
				strcpy(g_pub_tx.reply,"0000");
			}
			strcpy(g_pub_tx.reply,"0000");
			vtcp_log("[%s][%d],渠道登记返回码，g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		}
		/*Modefied by YangMeng 20140704 渠道交易增加相应的业务信息(对方账号、户名等) end*/
		
		fact_num++;
		fact_amt+=atof(fldstr[4]);
		vtcp_log("%s,%d,fact_amt=[%s]![%lf]",__FILE__,__LINE__,fldstr[4],fact_amt);
		strcpy(cGz_br_no,fldstr[10]);
	}
	fclose(fp);
	
	if(fact_num != tot_num){
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
	/********读文件，转账************/
	fp = fopen( filename_full,"r" );
	if ( fp==NULL )
	{
		sprintf( acErrMsg," 打开文件出错 [%s]  ", filename_full );
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
		for ( i=0;i<13;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/************
		fldstr[0] 委托日期 
		fldstr[1] 序号
		fldstr[2] 转出账号
		fldstr[3] 转入卡号
		fldstr[4] 交易金额
		fldstr[5] 手续费
		fldstr[6] 手续费出处
		fldstr[7] 过渡账号
		fldstr[8] 入账户名
		fldstr[9] 强制挂账标识
		fldstr[10] 挂账机构
		fldstr[11] 摘要
		fldstr[12] 发起行号
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

		/* 循环的时候会产生变化，这里赋回来 */
		set_zd_data("0020",c_opn_br_no);
		memset(cCtrlcmd, 0x00, sizeof(cCtrlcmd));
		memset(cGz_ind,0x00,sizeof(cGz_ind));
		if (fldstr[9][0] != 'C'){/*村镇银行只检查账户状态是不是正确，不做其他处理。Added by YangMeng 2014/7/18 17:42:49*/
			get_zd_data("0730", cCtrlcmd);
		}else{
			strcpy(cCtrlcmd,"0000100003");
		}
		vtcp_log("[%s][%d]****控制标志****=[%s]",__FILE__,__LINE__,cCtrlcmd);
		cGz_ind[0] = cCtrlcmd[5];
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
				sprintf(acErrMsg, "科目号不存在[%s]", fldstr[2]);
				WRITEMSG
				goto FAILE;
			}
			else if(ret){
				sprintf(acErrMsg, "查询科目表出错【%d】",ret);
				WRITEMSG
				goto ErrExit;
			}
			if(com_item.sub_acc_yn[0]=='Y'){
				sprintf(acErrMsg, "该科目有下级科目[%s]", fldstr[2]);
				WRITEMSG
				goto FAILE;
			}
			if(com_item.acc_knd[0]!='3'){
				sprintf(acErrMsg, "该科目下有内部账户[%s]", fldstr[2]);
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
				sprintf(acErrMsg, "转出账号不存在,[%s]", fldstr[2]);
				WRITEMSG
				goto FAILE;
			}
			else if(ret){
				sprintf(acErrMsg, "查询Mdm_ac_rel表出错【%d】",ret);
				WRITEMSG
				goto ErrExit;
			}
			if(out_mdm_ac_rel.note_sts[0]!='0'){
				sprintf(acErrMsg, "转出账号介质状态不正常,[%s]", fldstr[2]);
				WRITEMSG
				goto FAILE;
			}

			if(fldstr[2][0]=='9')/***内部账号******/
			{
				ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld and ac_seqn=0",out_mdm_ac_rel.ac_id);
				if(ret==100){
					sprintf(acErrMsg, "内部账号不存在,[%s]", fldstr[2]);
					WRITEMSG
					goto FAILE;
				}
				else if(ret){
					sprintf(acErrMsg, "查询内部主文件出错【%d】",ret);
					WRITEMSG
					goto ErrExit;
				}
				if(in_mst.sts[0]=='*'){
					sprintf(acErrMsg, "内部账号已销户,[%s]", fldstr[2]);
					WRITEMSG
					goto FAILE;
				}
				in_flag=1;
				
				/*add xupignbo 2015/6/4 17:48:57 */ 
				memset(sRz_br_no,0x0,sizeof(sRz_br_no));
				memcpy(sRz_br_no,in_mst.opn_br_no,strlen(in_mst.opn_br_no));
				vtcp_log("[%s],[%d],入账机构，opn_br_no=[%s],sRz_br_no=[%s]!",__FILE__,__LINE__,in_mst.opn_br_no,sRz_br_no);
			}
			else 
			{
				ret=Dd_mst_Sel(g_pub_tx.reply,&out_dd_mst,"ac_id=%d and ac_seqn=1",out_mdm_ac_rel.ac_id);
				if(ret==100){
					sprintf(acErrMsg, "活期账号不存在,[%s]", fldstr[2]);
					WRITEMSG
					goto FAILE;
				}
				else if(ret){
					sprintf(acErrMsg, "查询活期主文件出错【%d】",ret);
					WRITEMSG
					goto ErrExit;
				}
				if(out_dd_mst.ac_sts[0]=='0'){
					sprintf(acErrMsg, "转出账号开户未确认,[%s]", fldstr[2]);
					WRITEMSG
					goto FAILE;
				}
				if(out_dd_mst.ac_sts[0]=='*'){
					sprintf(acErrMsg, "转出账号已销户,[%s]", fldstr[2]);
					WRITEMSG
					goto FAILE;
				}
				if(out_dd_mst.hold_sts[0]=='1'){
					sprintf(acErrMsg, "转出账号为完全冻结状态,[%s]", fldstr[2]);
					WRITEMSG
					goto FAILE;
				}
				if(out_dd_mst.hold_sts[0]=='2'){
					sprintf(acErrMsg, "转出账号为只进不出状态,[%s]", fldstr[2]);
					WRITEMSG
					goto FAILE;
				}
				in_flag=0;
				
				/*add xupignbo 2015/6/4 17:48:57 */ 
				memset(sRz_br_no,0x0,sizeof(sRz_br_no));
				memcpy(sRz_br_no,out_dd_mst.opn_br_no,strlen(out_dd_mst.opn_br_no));
				vtcp_log("[%s],[%d],入账机构，opn_br_no=[%s],sRz_br_no=[%s]!",__FILE__,__LINE__,out_dd_mst.opn_br_no,sRz_br_no);
			}
		}

		/****转入账号检查*****/
		/****这里增加挂账账号处理****/ /*增加fldstr[9][0]=='1' 挂账标识位，如果是1强制挂账*/
		vtcp_log("[%s][%d]****强制挂账标识位****=[%c]",__FILE__,__LINE__,fldstr[9][0]);
		if (fldstr[9][0] != 'C'){/*村镇银行不需要做挂账检查Modefied by YangMeng 2014/7/18 15:57:38*/
			
			if(cGz_ind[0]=='3'||cGz_ind[0]=='9'||fldstr[9][0]=='8'||fldstr[9][0]=='9')
			{
				memset(cGz_reason,0x00,sizeof(cGz_reason));
				memset(cGz_br_no,0x00,sizeof(cGz_br_no));
				memset(sGZGZ,0x00,sizeof(sGZGZ));
				/*add by xuhaisheng 单笔强制挂账 设置强制挂账位为 9,强制标志位优先*/
				if(fldstr[9][0]=='9'||fldstr[9][0]=='8')
				{
					sGZGZ[0]=fldstr[9][0];
				}
				else
				{
					sGZGZ[0]=cGz_ind[0];
				}
				vtcp_log("[%s][%d]****挂账标识位****=[%s]",__FILE__,__LINE__,cGz_ind);
				ret = pubf_com_CheckGZ(fldstr[3],fldstr[8],sGZGZ,fldstr[10],cGz_reason,iParm_seqn);
				if (ret){
					sprintf(acErrMsg, "取挂账账号失败,序号[%d]", atoi(fldstr[1]));
					WRITEMSG
					goto FAILE;
				}
				vtcp_log("[%s][%d]挂账账号=[%s],cAc_name2=[%s]",__FILE__,__LINE__,fldstr[3],fldstr[8]);
				vtcp_log("[%s][%d]****挂账机构****=[%s]",__FILE__,__LINE__,fldstr[10]);
				vtcp_log("[%s][%d]****挂账原因****=[%s]",__FILE__,__LINE__,cGz_reason);
				strcpy(cGz_br_no,fldstr[10]);
				strcpy(g_pub_tx.reply,"0000");
				vtcp_log("[%s][%d],挂账检查返回码，g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
			}
		}else{
			vtcp_log("[%s][%d],村镇银行不需要做挂账检查，fldstr[9][0] = [%c]",__FILE__,__LINE__,fldstr[9][0]);
			memset(cGz_reason,0x00,sizeof(cGz_reason));
			memset(cGz_br_no,0x00,sizeof(cGz_br_no));
			vtcp_log("[%s][%d]****挂账原因****=[%s]",__FILE__,__LINE__,cGz_reason);
			vtcp_log("[%s][%d]****挂账机构****=[%s]",__FILE__,__LINE__,cGz_br_no);
		}
		if(strlen(fldstr[3]) == 0){
			sprintf(acErrMsg, "转入账号为空,序号[%d]", atoi(fldstr[1]));
			WRITEMSG
			goto FAILE;
		}

		if (strlen (fldstr[3]) > 10) /***此种情况下,只能是账号***/
		{
			vtcp_log("[%s],[%d],客户账号 acc_no=[%s]!",__FILE__,__LINE__,fldstr[3]);
			if(strlen(fldstr[3]) != 13 && strlen(fldstr[3]) != 15 && strlen(fldstr[3]) != 19){
				sprintf(acErrMsg, "转入账号位数不对,序号[%d][%s]", atoi(fldstr[1]),fldstr[3]);
				WRITEMSG
				goto FAILE;
			}
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&in_mdm_ac_rel,"ac_no='%s'",fldstr[3]);
			if(ret==100){
				sprintf(acErrMsg, "转入卡号不存在,[%s]", atoi(fldstr[3]));
				WRITEMSG
				goto FAILE;
			}
			else if(ret)
			{
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

			ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",in_mdm_ac_rel.ac_id);
			if (ret)
			{
				sprintf(acErrMsg, "查询prdt_ac_rel错误,[%s]", atoi(fldstr[3]));
				WRITEMSG
				goto FAILE;
			}
			if(sPrdt_ac_id.ac_id_type[0] == '1')
			{
				ret=Dd_mst_Sel(g_pub_tx.reply,&in_dd_mst,"ac_id=%ld and ac_seqn=1",in_mdm_ac_rel.ac_id);
				if(ret==100){
					sprintf(acErrMsg, "转入账号不存在,[%s]", atoi(fldstr[3]));
					WRITEMSG
					goto FAILE;
				}
				else if(ret)
				{
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
				in_flag2=0;
				
	
				memset(sRz_br_no,0x0,sizeof(sRz_br_no));
				memcpy(sRz_br_no,in_mdm_ac_rel.opn_br_no,strlen(in_mdm_ac_rel.opn_br_no));
				vtcp_log("[%s],[%d],入账机构，opn_br_no=[%s],sRz_br_no=[%s]!",__FILE__,__LINE__,in_mdm_ac_rel.opn_br_no,sRz_br_no);
			}
			else if(sPrdt_ac_id.ac_id_type[0] == '9')
			{
				ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",in_mdm_ac_rel.ac_id);
				if (ret)
				{
					sprintf(acErrMsg, "转入账号不存在,[%s]", atoi(fldstr[3]));
					WRITEMSG
					goto FAILE;
				}
				if(sIn_mst.sts[0] != '1')
				{
					sprintf(acErrMsg, "该账户不正常,[%s]", atoi(fldstr[3]));
					WRITEMSG
					goto FAILE;
				}
				in_flag2=1;
				
				/*add fupengfei 2014/8/1 17:48:57 */ 
				memset(sRz_br_no,0x0,sizeof(sRz_br_no));
				memcpy(sRz_br_no,in_mdm_ac_rel.opn_br_no,strlen(in_mdm_ac_rel.opn_br_no));
				vtcp_log("[%s],[%d],入账机构，opn_br_no=[%s],sRz_br_no=[%s]!",__FILE__,__LINE__,in_mdm_ac_rel.opn_br_no,sRz_br_no);
			}
		}else{
			vtcp_log("[%s],[%d],科目 acc_no=[%s]!",__FILE__,__LINE__,fldstr[3]);
			/**** 否则是科目  **/
			ret = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",fldstr[3]);
			if(ret)
			{
				sprintf(acErrMsg, "该账户不存在,[%s]", atoi(fldstr[3]));
				WRITEMSG
				goto FAILE;
			}
			in_flag2=1;
		}

		/*******过渡账号检查**********/
		if (strlen(fldstr[7])>0){
			vtcp_log("[%s],[%d],过渡账号/科目检查 fldstr[7]=[%s]!",__FILE__,__LINE__,fldstr[7]);
			if (strlen(fldstr[7]) <= 8)/******科目号*******/
			{
				vtcp_log("%s,%d,过渡科目 acc_no=[%s]!",__FILE__,__LINE__,fldstr[7]);
				ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",fldstr[7]);
				if(ret==100){
					sprintf(acErrMsg, "科目号不存在[%s]", fldstr[7]);
					WRITEMSG
					goto FAILE;
				}
				else if(ret)
				{
					sprintf(acErrMsg, "查询科目表出错【%d】",ret);
					WRITEMSG
					goto ErrExit;
				}
				if(com_item.sub_acc_yn[0]=='Y'){
					sprintf(acErrMsg, "该科目有下级科目[%s]", fldstr[7]);
					WRITEMSG
					goto FAILE;
				}
				if(com_item.acc_knd[0]!='3'){
					sprintf(acErrMsg, "该科目下有内部账户[%s]", fldstr[7]);
					WRITEMSG
					goto FAILE;
				}
			}else{
				vtcp_log("%s,%d,过渡账号 acc_no=[%s]!",__FILE__,__LINE__,fldstr[7]);
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&out_mdm_ac_rel,"ac_no='%s'",fldstr[7]);
				if(ret==100){
					sprintf(acErrMsg, "过渡账号不存在,[%s]", fldstr[7]);
					WRITEMSG
					goto FAILE;
				}
				else if(ret)
				{
					sprintf(acErrMsg, "查询Mdm_ac_rel表出错【%d】",ret);
					WRITEMSG
					goto ErrExit;
				}
				if(out_mdm_ac_rel.note_sts[0]!='0'){
					sprintf(acErrMsg, "过渡账号介质状态不正常,[%s]", fldstr[7]);
					WRITEMSG
					goto FAILE;
				}

				if(fldstr[7][0]=='9')/***内部账号******/
				{
					ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld and ac_seqn=0",out_mdm_ac_rel.ac_id);
					if(ret==100){
						sprintf(acErrMsg, "内部账号不存在,[%s]", fldstr[7]);
						WRITEMSG
						goto FAILE;
					}
					else if(ret){
						sprintf(acErrMsg, "查询内部主文件出错【%d】",ret);
						WRITEMSG
						goto ErrExit;
					}
					if(in_mst.sts[0]=='*'){
						sprintf(acErrMsg, "内部账号已销户,[%s]", fldstr[7]);
						WRITEMSG
						goto FAILE;
					}
				}else{
					sprintf(acErrMsg, "过渡账号必须为内部帐或科目,[%s]", fldstr[7]);
					WRITEMSG
					goto FAILE;
				}
			}
		}
		
		/******每次循环初始化全局变量************/
		memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
		pub_base_sysinit();
		/****g_pub_tx结构体赋值----转出账号*********/
		strcpy(g_pub_tx.cur_no, RMB);
		strcpy(g_pub_tx.ct_ind, "2");
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
		}else{
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
		}else{
			set_zd_data("1204","01");
			set_zd_data("1205",g_pub_tx.ct_ind);
			set_zd_double("1208",g_pub_tx.tx_amt1);
		}
		if (pubf_acct_proc(g_pub_tx.sub_tx_code))
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
		vtcp_log("%s,%d,开户机构1g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
		if (!memcmp(fldstr[3],"303101",6)){
			memset(g_pub_tx.opn_br_no, 0x0, sizeof(g_pub_tx.opn_br_no));
			memcpy(g_pub_tx.opn_br_no,"88002",sizeof(g_pub_tx.opn_br_no)-1);
		}
		vtcp_log("%s,%d,开户机构2g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
		strcpy(g_pub_tx.ac_no,fldstr[3]);
		
		if(strlen(brf)==0)
			strcpy(g_pub_tx.brf,"批量转账转入");
		else 
			strcpy(g_pub_tx.brf,brf);
		
		strcpy(g_pub_tx.ac_id_type,"");

		if(in_flag2==0){
			g_pub_tx.svc_ind=1001;
			strcpy(g_pub_tx.sub_tx_code, "D099");
			g_pub_tx.ac_seqn=1;
		}else{
			g_pub_tx.svc_ind=9001;
			strcpy(g_pub_tx.sub_tx_code, "A017");
			strcpy(g_pub_tx.ac_id_type,"9");
			g_pub_tx.ac_seqn=0;
		}
		g_pub_tx.tx_amt1=atof(fldstr[4]);
		strcpy(g_pub_tx.add_ind,"1"); 
		
		if (pub_acct_trance())
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			goto FAILE;;
		}
		vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
		if(in_flag2==0){
			set_zd_data("101A","01");
			set_zd_data("101B",g_pub_tx.ct_ind);
			set_zd_double("1013",g_pub_tx.tx_amt1);
		}else{
			set_zd_data("1214",g_pub_tx.cur_no);
			set_zd_data("1215",g_pub_tx.ct_ind );
			set_zd_double("1218",g_pub_tx.tx_amt1);
		}
		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			WRITEMSG
			goto FAILE;
		}
		vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
		vtcp_log("[%s],[%d],chw_fldstr[7]=[%s],g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,fldstr[7],g_pub_tx.opn_br_no);
		
		/*******过渡账号过渡**********/
		if(strlen(fldstr[7])>0){
			
			/*Modified by YangMeng 20140622 begin*/
			vtcp_log("[%s],[%d],进入过度科目账务处理，ac_no=【%s】",__FILE__,__LINE__,fldstr[7]);
			vtcp_log("[%s],[%d],g_pub_tx.tx_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.tx_br_no);
			vtcp_log("[%s],[%d],g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
			memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
			pub_base_sysinit();
			/*memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
			memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
			memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
			memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));*/
			/* 这里用文件中挂帐机构处理过渡机构 41204 
			memset(g_pub_tx.opn_br_no, 0x0, sizeof(g_pub_tx.opn_br_no));*/
			
			strncpy(g_pub_tx.tx_br_no, fldstr[10] ,sizeof(g_pub_tx.tx_br_no)-1);
			strncpy(g_pub_tx.opn_br_no, fldstr[10] ,sizeof(g_pub_tx.opn_br_no)-1);
			vtcp_log("[%s],[%d],过度科目【%s】",__FILE__,__LINE__,fldstr[7]);
			vtcp_log("[%s],[%d],过度交易机构 2g_pub_tx.tx_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.tx_br_no);
			vtcp_log("[%s],[%d],过度开户机构 2g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
			/*Modified by YangMeng 20140622 end*/
			if(strlen(brf)==0)
				strcpy(g_pub_tx.brf,"批量转账过渡");
			else 
				strcpy(g_pub_tx.brf,brf);

			g_pub_tx.svc_ind=9001;
			strcpy(g_pub_tx.sub_tx_code, "A016");
			strcpy(g_pub_tx.ac_id_type,"9");
			g_pub_tx.ac_seqn=0;

			strcpy(g_pub_tx.ac_no,fldstr[7]);
			g_pub_tx.tx_amt1=atof(fldstr[4]);
			strcpy(g_pub_tx.add_ind,"0"); 
			strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
			strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
			strcpy(g_pub_tx.ac_get_ind, "00");
			strcpy(g_pub_tx.note_type,"");
			strcpy(g_pub_tx.beg_note_no,"");
			
			if (pub_acct_trance())
			{
				sprintf(acErrMsg,"调用存取款主控失败!");
				WRITEMSG
				goto FAILE;;
			}
			vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
			set_zd_data("1204","01");
			set_zd_data("1205",g_pub_tx.ct_ind);
			set_zd_double("1208",g_pub_tx.tx_amt1);

			if (pubf_acct_proc(g_pub_tx.sub_tx_code))
			{
				sprintf(acErrMsg,"登记会计流水错误!");
				WRITEMSG
				goto FAILE;;
			}
			vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
			/* 过渡贷方 */
			memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
			memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
			memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
			memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));

			g_pub_tx.svc_ind=9001;
			strcpy(g_pub_tx.sub_tx_code, "A017");
			strcpy(g_pub_tx.ac_id_type,"9");
			g_pub_tx.ac_seqn=0;

			strcpy(g_pub_tx.ac_no,fldstr[7]);
			g_pub_tx.tx_amt1=atof(fldstr[4]);
			strcpy(g_pub_tx.add_ind,"1"); 
			strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
			strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
			strcpy(g_pub_tx.ac_get_ind, "00");
			strcpy(g_pub_tx.note_type,"");
			strcpy(g_pub_tx.beg_note_no,"");
		
			if (pub_acct_trance())
			{
				sprintf(acErrMsg,"调用存取款主控失败!");
				WRITEMSG
				goto FAILE;;
			}
			vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
			set_zd_data("1214","01");
			set_zd_data("1215",g_pub_tx.ct_ind);
			set_zd_double("1218",g_pub_tx.tx_amt1);

			if (pubf_acct_proc(g_pub_tx.sub_tx_code))
			{
				sprintf(acErrMsg,"登记会计流水错误!");
				WRITEMSG
				goto FAILE;;
			}
			vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
		}
		
		/******手续费收入********/
		if(atof(fldstr[5]) > 0.0005)
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
			if (ret){
				sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
				WRITEMSG
				goto FAILE;
			}
			vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
			ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
			if (ret){
				sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
				WRITEMSG
				goto FAILE;
			}
			vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
			/********扣手续费*************/
			memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
			memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
			memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
			memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
			g_pub_tx.ac_id=0;
			
			g_pub_tx.tx_amt1 = atof(fldstr[5]) ;
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
			if (ret){
				sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
				WRITEMSG
				goto FAILE;
			}
			vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
			ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
			if (ret){
				sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
				WRITEMSG
				goto FAILE;
			}
			vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
		}
		/********记完账，登记批量记账登记簿********/
		memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
		chnl_batch_acct.tx_date=com_sys_parm.sys_date;
		sprintf(chnl_batch_acct.tx_time,"%ld",g_pub_tx.tx_time);
		strcpy(chnl_batch_acct.batch_ref,filename);
		
		chnl_batch_acct.trace_no=g_pub_tx.trace_no;
		strcpy(chnl_batch_acct.cur_no,"01");
		/*strcpy(chnl_batch_acct.ac_no,ac_no);*/
		if (!memcmp(sSndRcv,"SR01",4)){
			strcpy(chnl_batch_acct.ac_no,fldstr[3]);
			strcpy(chnl_batch_acct.ac_no1,fldstr[3]);
			strcpy(chnl_batch_acct.ac_no2,fldstr[2]);
			/*sprintf(chnl_batch_acct.dz_ref,"%s%08ld%s%s",fldstr[0],atol(fldstr[1]),fldstr[12],"1");*/
			sprintf(chnl_batch_acct.dz_ref,"%s%08ld",fldstr[0],atol(fldstr[1]));
		}
		if (!memcmp(sSndRcv,"SR00",4)){
			strcpy(chnl_batch_acct.ac_no,fldstr[2]);
			strcpy(chnl_batch_acct.ac_no1,fldstr[2]);
			strcpy(chnl_batch_acct.ac_no2,fldstr[3]);
			/*sprintf(chnl_batch_acct.dz_ref,"%s%08ld%s%s",fldstr[0],atol(fldstr[1]),fldstr[12],"0");*/
			sprintf(chnl_batch_acct.dz_ref,"%s%08ld%s%s",fldstr[0],atol(fldstr[1]));
		}
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
		
		ret = Chnl_batch_acct_Ins(chnl_batch_acct,g_pub_tx.reply);
		if(ret){
        	Chnl_batch_acct_Debug(&chnl_batch_acct);
			sprintf(acErrMsg, "插入批量记账登记簿出错[%d]",ret);
			WRITEMSG
			goto FAILE;
		}
		vtcp_log("[%s][%d]****挂账原因****=[%s]",__FILE__,__LINE__,cGz_reason);
		vtcp_log("[%s][%d]****挂账机构****=[%s]",__FILE__,__LINE__,cGz_br_no);
		vtcp_log("[%s][%d]****入账机构****=[%s]",__FILE__,__LINE__,sRz_br_no);
		fprintf(fp1,"%s|%s|1|%s|%s|%s|%s|\n",fldstr[0],fldstr[1],cGz_reason,sRz_br_no,cGz_br_no,fldstr[12]);	
		sql_commit();
		sql_begin();
		continue;
FAILE:
		vtcp_log("wkwkwk111");
		fprintf(fp1,"%s|%s|0|0|\n",fldstr[0],fldstr[1]);
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
		sprintf(cmd,"%s/bin/send_back.sh %s",getenv("HOME"),filename_rt);
		ret = system(cmd);
		if(ret){
			sprintf(acErrMsg, "回传文件出错[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply, "S047" );
			goto ErrExit;
		}
		/*set_zd_data("0830","");*/
		set_zd_data("0820",filename_rt);
		vtcp_log("%s,%d,filename_rt=[%s]!",__FILE__,__LINE__,filename_rt);
	}
	
OkExit:
	sql_commit();
	vtcp_log("%s,%d,批量转账记账成功!",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();
	if(memcmp(g_pub_tx.reply,"0000",4) == 0)
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

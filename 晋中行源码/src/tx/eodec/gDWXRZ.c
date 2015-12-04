/*************************************************************
*程序名称：微信收单批量入账
*程序功能：用于微信收单项目商户的账户数据清分，晋中银行的对公账户，
*          向商户的对公账户、个人账户结算资金批量入账
*程序作者：lwb
*完成时间：20150709
insert into com_parm (PARM_CODE, PARM_SEQN, PARM_NAME, VAL_TYPE, VAL) values ('WXLL', 1, '微信短期内固定费率', '4', '0.35');
insert into com_parm (PARM_CODE, PARM_SEQN, PARM_NAME, VAL_TYPE, VAL) values ('WXLL', 2, '威富通短期内固定费率', '4', '0.1');
insert into com_parm (PARM_CODE, PARM_SEQN, PARM_NAME, VAL_TYPE, VAL) values ('WXQS', 1, '微信收单清算账号', '1', '');
insert into com_parm (PARM_CODE, PARM_SEQN, PARM_NAME, VAL_TYPE, VAL) values ('WXQS', 2, '威富通清算账号', '1', '');

**************************************************************/

#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <string.h>
#include <time.h>
#define EXTERN
#define LEN 1024
#include "public.h"
#include "weixin_acct_reg_c.h"
#include "weixin_acct_hst_c.h"




int gDWXRZ()
{
	int ret = 0;
	int iSeqn = 0;
	char cSeqn[10];
	char cCount[10];
	char cSum[20];
	struct mdm_ac_rel_c sMdm_ac_rel_o,sMdm_ac_rel_i,sMdm_ac_rel_w;
	struct dd_mst_c sDd_mst_o,sDd_mst_i,sDd_mst_w;
	struct weixin_acct_reg_c sWeixin_acct_reg;
	struct weixin_acct_hst_c sWeixin;
	FILE *fp1 = NULL;  /*处理入账文件*/
	FILE *fp2 = NULL;  /*处理生成的返回文件*/
	char filename[50];
	char filename_r[50];
	char filename1[30];
	char cTmpstr[1024];
	char cTmpstr1[1024];
	char cAmt[20];
	int iLinenum = 1;
	int iCount = 0;
	double dSum=0.00;
	double dSum_r=0.00;
	double dWft_rate=0.00;
	double dWx_rate=0.00;
	int hour=0;
	int minute=0;
	int second=0;
	char cTx_cnt[10];
    char cTx_amt[20];
    char cRefund_cnt[10];
    char cRefund_amt[20];
    char cRetan_amt[20];
    char cBussi_rate[20];
    char cBussi_chrg[20];
    char cWx_chrg[20];
    char cBk_amt[20];
    char cWft_chrg[20];
    char cTx_bank_chrg[20];
    char cBussi_amt[20];
    char cTrace_no[20];
    char cAcct_date[20];
    char cQs_ac_no[20];
    char cWft_ac_no[20];
	
	memset(cSeqn,0x00,sizeof(cSeqn));
	memset(&sWeixin,0x00,sizeof(sWeixin));
	memset(filename,0x00,sizeof(filename));
	memset(filename_r,0x00,sizeof(filename_r));
	memset(&sMdm_ac_rel_o,0x00,sizeof(sMdm_ac_rel_o));
	memset(&sMdm_ac_rel_i,0x00,sizeof(sMdm_ac_rel_i));
	memset(&sMdm_ac_rel_w,0x00,sizeof(sMdm_ac_rel_w));
	memset(&sDd_mst_o,0x00,sizeof(sDd_mst_o));
	memset(&sDd_mst_i,0x00,sizeof(sDd_mst_i));
	memset(&sDd_mst_w,0x00,sizeof(sDd_mst_w));
	memset(&sWeixin_acct_reg,0x00,sizeof(sWeixin_acct_reg));
	memset(&sWeixin,0x00,sizeof(sWeixin));
	memset(cQs_ac_no,0x00,sizeof(cQs_ac_no));
	memset(cWft_ac_no,0x00,sizeof(cWft_ac_no));
	
	ret=sql_begin(); /*打开事务*/
	MYSQLERR
	
	pub_base_sysinit();
	strcpy(g_pub_tx.tx_code,"WXRZ");
	
	ret = pub_base_GetParm_double("WXLL",1,&dWx_rate);
	if(ret)
    {
        sprintf( acErrMsg, "pub_base_GetParm_Double取参数配置表错误!!!" );
        WRITEMSG
        goto ErrExit;
    }   
	ret = pub_base_GetParm_double("WXLL",2,&dWft_rate);
	if(ret)
    {
        sprintf( acErrMsg, "pub_base_GetParm_Double取参数配置表错误!!!" );
        WRITEMSG
        goto ErrExit;
    }   
	
	
/*取晋中银行清算账号*/
	ret = pub_base_GetParm_Str("WXQS",1,cQs_ac_no);
	if(ret)
    {
    	strcpy(g_pub_tx.reply,"O247");
        sprintf( acErrMsg, "pub_base_GetParm_str取参数配置表错误!!!" );
        WRITEMSG
        goto ErrExit;
    }   

	/*检查付款账号*/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_o,"ac_no='%s' ",cQs_ac_no);
	if(ret!=100&&ret)
	{
		vtcp_log("查询Mdm_ac_rel_Sel出错");
		strcpy(g_pub_tx.reply,"L015");
		goto ErrExit;	
	}
	else if(ret == 100)
	{
		strcpy(g_pub_tx.reply,"D103");
		sprintf(acErrMsg,"付款账号不存在 [%s]",sWeixin.bussi_ac_no);
		WRITEMSG
		goto ErrExit;	
	}
	
	
	memset( &sDd_mst_o, 0x00, sizeof(sDd_mst_o) );
	ret = Dd_mst_Sel( g_pub_tx.reply , &sDd_mst_o, "ac_id = %ld \
		and ac_seqn = 1", sMdm_ac_rel_o.ac_id );		
	if ( ret )
	{
		strcpy(g_pub_tx.reply,"L015");
		sprintf( acErrMsg,"查询活期主文件失败[%d][%ld]", ret, sMdm_ac_rel_o.ac_id);				
		WRITEMSG
		goto ErrExit;	
	}
	
	if ( sMdm_ac_rel_o.note_sts[0] != '0' || sDd_mst_o.ac_sts[0] != '1' \
		|| sDd_mst_o.hold_sts[0] == '1' || sDd_mst_o.hold_sts[0]=='2' )	/*销户或全冻*/
	{
		strcpy(g_pub_tx.reply,"L015");
		sprintf(acErrMsg,"账户状态不正常[%s]", sWeixin.bussi_ac_no );				
		WRITEMSG
		goto ErrExit;		
	}
	/*检查付款账号end*/
					
	
	
   /*取威富通账号*/     
	ret = pub_base_GetParm_Str("WXQS",1,cWft_ac_no);
	if(ret)
    {
    	strcpy(g_pub_tx.reply,"O247");
        sprintf( acErrMsg, "pub_base_GetParm_str取参数配置表错误!!!" );
        WRITEMSG
        goto ErrExit;
    }   
    		
	/*检查威富通账号  start*/
	memset(&sMdm_ac_rel_w,0x00,sizeof(sMdm_ac_rel_w));
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_w,"ac_no='%s' ",cWft_ac_no);
	if(ret!=100&&ret)
	{
		
		vtcp_log("查询Mdm_ac_rel_Sel出错");
		strcpy(g_pub_tx.reply,"L015");
		goto ErrExit;	
	}
	else if(ret == 100)
	{
		strcpy(g_pub_tx.reply,"L015");
		sprintf(acErrMsg,"收款账号不存在 [%s]",cWft_ac_no);
		WRITEMSG
		goto ErrExit;	
	}
					
	memset( &sDd_mst_w, 0x00, sizeof(sDd_mst_w) );
	ret = Dd_mst_Sel( g_pub_tx.reply , &sDd_mst_w, "ac_id = %ld \
		and ac_seqn = 1", sMdm_ac_rel_w.ac_id );		
	if ( ret )
	{
		strcpy(g_pub_tx.reply,"L015");
		sprintf( acErrMsg,"查询活期主文件失败[%d][%ld]", ret, sMdm_ac_rel_w.ac_id);
		WRITEMSG
		goto ErrExit;	
	}
	
	if ( sMdm_ac_rel_w.note_sts[0] != '0' || sDd_mst_w.ac_sts[0] != '1' \
		|| sDd_mst_w.hold_sts[0] == '1'   )	/*销户或全冻*/
	{
		strcpy(g_pub_tx.reply,"O085");	
		sprintf(acErrMsg,"威富通账户状态不正常[%s]", cWft_ac_no );					
		WRITEMSG
		goto ErrExit;	
	}

	/*检查威富通账号 end*/	
		
	iSeqn=sql_max_int("weixin_acct_reg", "acct_seqn", "acct_date=%ld", g_pub_tx.tx_date);
	while(1)
	{
		memset(filename,0x00,sizeof(filename));
		memset(filename1,0x00,sizeof(filename1));
		memset(cSeqn,0x00,sizeof(cSeqn));
		memset(cTx_cnt,0x00,sizeof(cTx_cnt));
		memset(cTx_amt,0x00,sizeof(cTx_amt));
		memset(cRefund_cnt,0x00,sizeof(cRefund_cnt));
		memset(cRefund_amt,0x00,sizeof(cRefund_amt));
		memset(cRetan_amt,0x00,sizeof(cRetan_amt));
		memset(cBussi_rate,0x00,sizeof(cBussi_rate));
		memset(cBussi_chrg,0x00,sizeof(cBussi_chrg));
		memset(cWx_chrg,0x00,sizeof(cWx_chrg));
		memset(cBk_amt,0x00,sizeof(cBk_amt));
		memset(cTx_bank_chrg,0x00,sizeof(cTx_bank_chrg));
		memset(cWft_chrg,0x00,sizeof(cWft_chrg));
		memset(cBussi_amt,0x00,sizeof(cBussi_amt));
		memset(cTrace_no,0x00,sizeof(cTrace_no));
		memset(cAcct_date,0x00,sizeof(cAcct_date));

		
		
		pub_base_GetTime2(&hour,&minute,&second);
		g_pub_tx.tx_time=hour*10000+minute*100+second;	
		
		vtcp_log("start");
		iLinenum=0;
		dSum_r=0.00;
		sprintf(cSeqn,"%03d",++iSeqn);
		sprintf(filename,"%s/zgf/JZBANK_WEIXIN_%ld_%s.txt", getenv("HOME"), g_pub_tx.tx_date,cSeqn );
		
		 
		fp1 = fopen(filename,"r");
		if(fp1 == NULL)
		{
			vtcp_log("微信入账文件已取完，最后一个文件序号为[%d]",iSeqn-1);
			break;	
		}
		vtcp_log("正在处理[%s]",filename);
		memset(filename_r,0x00,sizeof(filename_r));
		 
		sprintf(filename_r,"%s/zgf/JZBANK_WEIXIN_%ld_%s_R.txt", getenv("HOME"),g_pub_tx.tx_date,cSeqn);
		fp2 = fopen(filename_r,"w+");

		while(fgets(cTmpstr,LEN,fp1))
		{	
			if( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply) )
			goto ErrExit;
	
			memset(&sWeixin,0x00,sizeof(sWeixin));
			strcpy(sWeixin.filename,filename);
			strcpy(cTmpstr1,cTmpstr);
	 		pub_base_cut_str(cTmpstr,cTrace_no,"|",1);
	 		sWeixin.pt_trace_no=atoi(cTrace_no);
            pub_base_cut_str(cTmpstr,cAcct_date,"|",2);
            sWeixin.acct_date=atoi(cAcct_date);
            pub_base_cut_str(cTmpstr,sWeixin.bussi_name,"|",3);
            pub_base_cut_str(cTmpstr,sWeixin.bussi_ac_no,"|",4);
            pub_base_cut_str(cTmpstr,sWeixin.opn_br_no,"|",5);
            pub_base_cut_str(cTmpstr,sWeixin.br_name,"|",6);
            pub_base_cut_str(cTmpstr,sWeixin.tx_type,"|",7);
            pub_base_cut_str(cTmpstr,cTx_cnt,"|",8);
            sWeixin.tx_cnt=atoi(cTx_cnt);
            pub_base_cut_str(cTmpstr,cTx_amt,"|",9);
            sWeixin.tx_amt=atof(cTx_amt);
            pub_base_cut_str(cTmpstr,cRefund_cnt,"|",10);
            sWeixin.refud_cnt=atoi(cRefund_cnt);
            pub_base_cut_str(cTmpstr,cRefund_amt,"|",11);
            sWeixin.refud_amt=atof(cRefund_amt);
            pub_base_cut_str(cTmpstr,cRetan_amt,"|",12);
            sWeixin.retan_amt=atof(cRetan_amt);
            pub_base_cut_str(cTmpstr,cBussi_rate,"|",13);
            sWeixin.bussi_rate=atof(cBussi_rate);
            pub_base_cut_str(cTmpstr,cBussi_chrg,"|",14);
            sWeixin.bussi_chrg=atof(cBussi_chrg);
            pub_base_cut_str(cTmpstr,cWx_chrg,"|",15);
            sWeixin.wx_chrg=atof(cWx_chrg);
            pub_base_cut_str(cTmpstr,cBk_amt,"|",16);
            sWeixin.bk_amt=atof(cBk_amt);
            pub_base_cut_str(cTmpstr,cWft_chrg,"|",17);
            sWeixin.wft_chrg=atof(cWft_chrg);
            pub_base_cut_str(cTmpstr,cTx_bank_chrg,"|",18);
            sWeixin.tx_bk_chrg=atof(cTx_bank_chrg);
            pub_base_cut_str(cTmpstr,cBussi_amt,"|",19);
            sWeixin.bussi_amt=atof(cBussi_amt);
            
            
			memset(&sMdm_ac_rel_o,0x00,sizeof(sMdm_ac_rel_o));
			
		
			/*检查收款账号  start*/
			memset(&sMdm_ac_rel_i,0x00,sizeof(sMdm_ac_rel_i));
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_i,"ac_no='%s' ",sWeixin.bussi_ac_no);
			if(ret!=100&&ret)
			{
				vtcp_log("查询Mdm_ac_rel_Sel出错");
				strcpy(g_pub_tx.reply,"L015");
				goto ErrExit;	
			}
			else if(ret == 100)
			{
				sprintf(acErrMsg,"商户账号不存在",sWeixin.bussi_ac_no);
				strcpy(g_pub_tx.reply,"D137");
				writefile(cTmpstr1,sWeixin,fp2,acErrMsg);
				WRITEMSG
				continue;
			}
							
			memset( &sDd_mst_i, 0x00, sizeof(sDd_mst_i) );
			ret = Dd_mst_Sel( g_pub_tx.reply , &sDd_mst_i, "ac_id = %ld \
				and ac_seqn = 1", sMdm_ac_rel_i.ac_id );		
			if ( ret )
			{
				sprintf( acErrMsg,"查询活期主文件失败[%d][%ld]", ret, sMdm_ac_rel_i.ac_id);
				writefile(cTmpstr1,sWeixin,fp2,acErrMsg);
				WRITEMSG
				continue;
			}
			
			if ( sMdm_ac_rel_i.note_sts[0] != '0' || sDd_mst_i.ac_sts[0] != '1' \
				|| sDd_mst_i.hold_sts[0] == '1'   )	/*销户或全冻*/
			{
				
				sprintf(acErrMsg,"账户状态不正常[%s]", sWeixin.bussi_ac_no);	
				strcpy(g_pub_tx.reply,"O085");				
				writefile(cTmpstr1,sWeixin,fp2,acErrMsg);
				WRITEMSG
				continue;	
			}
			/*if(strcmp(sDd_mst_i.name,sWeixin.bussi_name))
			{
				vtcp_log("[%s][%s]",sDd_mst_i.name,sWeixin.bussi_name);
				sprintf(acErrMsg,"收款账户户名不正确");					
				writefile(cTmpstr1,sWeixin,fp2,acErrMsg);
				WRITEMSG
				continue;		
			}*/
			if(strcmp(sDd_mst_i.opn_br_no,sWeixin.opn_br_no))
			{
				vtcp_log("[%s][%s]",sDd_mst_i.opn_br_no,sWeixin.opn_br_no);
				strcpy(g_pub_tx.reply,"D245"); 
				sprintf(acErrMsg,"收款账户开户行不一致");					
				writefile(cTmpstr1,sWeixin,fp2,acErrMsg);
				WRITEMSG
				continue;	
			}
			
			/*检查收款账号 end*/	
			
			
			/*开始账务处理 start*/
			vtcp_log("bussi_rate=%lf dWx_rate+dWft_rate=%lf ",sWeixin.bussi_rate,dWx_rate+dWft_rate);
			 if(pub_base_CompDblVal(sWeixin.bussi_rate,dWx_rate+dWft_rate)>=0)
            {
            	if(pub_base_CompDblVal(sWeixin.bk_amt-sWeixin.bussi_amt-sWeixin.tx_bk_chrg-sWeixin.wft_chrg,0.00)!=0)
				{
					vtcp_log("sWeixin.bk_amt-sWeixin.bussi_amt-sWeixin.tx_bk_chrg-sWeixin.wft_chrg=[%lf]",sWeixin.bk_amt-sWeixin.bussi_amt-sWeixin.tx_bk_chrg-sWeixin.wft_chrg);
					strcpy(g_pub_tx.reply,"D128");
					sprintf(acErrMsg,"借贷金额不符");					
					writefile(cTmpstr1,sWeixin,fp2,acErrMsg);
					WRITEMSG
					continue;			
				
				}	
            }
            else 
            {
            	if(pub_base_CompDblVal(sWeixin.bk_amt-sWeixin.bussi_amt+sWeixin.tx_bk_chrg-sWeixin.wft_chrg,0.00)!=0)
				{
					vtcp_log("sWeixin.bk_amt-sWeixin.bussi_amt+sWeixin.tx_bk_chrg-sWeixin.wft_chrg=[%lf]",sWeixin.bk_amt-sWeixin.bussi_amt+sWeixin.tx_bk_chrg-sWeixin.wft_chrg);
					strcpy(g_pub_tx.reply,"D128");
					sprintf(acErrMsg,"借贷金额不符");					
					writefile(cTmpstr1,sWeixin,fp2,acErrMsg);
					WRITEMSG
					continue;			
				
				}	 
            }		
			
		 	/*商户账号*/
            strncpy(g_pub_tx.ac_no,sWeixin.bussi_ac_no,sizeof(g_pub_tx.ac_no));
            strcpy(g_pub_tx.tx_br_no,sDd_mst_i.opn_br_no);
            strcpy(g_pub_tx.name,"");
            g_pub_tx.cif_no = 0;
            g_pub_tx.ac_id=sMdm_ac_rel_i.ac_id;
            g_pub_tx.ac_seqn=1;
            g_pub_tx.tx_amt1=sWeixin.bussi_amt;
            g_pub_tx.add_ind[0]='1';
            strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
            strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
            g_pub_tx.svc_ind=1001;
            strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
            strcpy(g_pub_tx.ac_id_type,"1");
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"微信收单入账");
            if ( pub_acct_trance() )
            {                	
            	sprintf(acErrMsg,"调用存取款主控失败!");
            	goto ErrExit;                	
            }
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("1002",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC02") )
            {
                vtcp_log("登记会计流水错误!");
                goto ErrExit;
            }
			
			
			/*晋中银行账号*/
			strncpy(g_pub_tx.ac_no,cQs_ac_no,sizeof(cQs_ac_no));
			strcpy(g_pub_tx.tx_br_no,sDd_mst_o.opn_br_no);					 
            strcpy(g_pub_tx.name,"");
            g_pub_tx.cif_no = 0;
            g_pub_tx.ac_id=sMdm_ac_rel_o.ac_id;
            g_pub_tx.ac_seqn=1;
            g_pub_tx.tx_amt1=sWeixin.bk_amt;
            g_pub_tx.add_ind[0]='0';
            strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
            strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
            g_pub_tx.svc_ind=1001;
            strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
            strcpy(g_pub_tx.ac_id_type,"1");
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"微信收单扣款");
            if ( pub_acct_trance() )
            {
            	if(!strcmp(g_pub_tx.reply,"LS51"))
            	{
            		sprintf(acErrMsg,"付款账户余额不足!");
            		writefile(cTmpstr1,sWeixin,fp2,acErrMsg);
            		continue;
            	}
            	else
            	{
                	vtcp_log("调用存取款主控失败!");
                	goto ErrExit;
           		}
            }
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("102I",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC01") )
            {
                vtcp_log("登记会计流水错误!");
                goto ErrExit;
            }
            
            
            /*威富通账号*/
            strncpy(g_pub_tx.ac_no,cWft_ac_no,sizeof(g_pub_tx.ac_no));
            strcpy(g_pub_tx.tx_br_no,sDd_mst_w.opn_br_no);
            strcpy(g_pub_tx.name,"");
            g_pub_tx.cif_no = 0;
            g_pub_tx.ac_id=sMdm_ac_rel_w.ac_id;
            g_pub_tx.ac_seqn=1;
            g_pub_tx.tx_amt1=sWeixin.wft_chrg;
            g_pub_tx.add_ind[0]='1';
            strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
            strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
            g_pub_tx.svc_ind=1001;
            strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
            strcpy(g_pub_tx.ac_id_type,"1");
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"威富通手续费入账");
            if ( pub_acct_trance() )
            {                	
            	sprintf(acErrMsg,"调用存取款主控失败!");
            	goto ErrExit;                	
            }
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("1002",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC02") )
            {
                vtcp_log("登记会计流水错误!");
                goto ErrExit;
            }
            
            
            
            /*利息科目记账*/
            if(pub_base_CompDblVal(sWeixin.bussi_rate,dWx_rate+dWft_rate)>=0)
            {
            	strcpy(g_pub_tx.ac_no,"5111703");
            	strcpy(g_pub_tx.brf,"银行利息收入");
            	g_pub_tx.add_ind[0]='1';
            }
            else 
            {
            	/*strcpy(g_pub_tx.ac_no,"52704");	*/
            	strcpy(g_pub_tx.ac_no,"304");	
            	strcpy(g_pub_tx.brf,"银行卡手续费支出");
            	g_pub_tx.add_ind[0]='0';
            }		
			g_pub_tx.tx_amt1=sWeixin.tx_bk_chrg;			
			strcpy(g_pub_tx.name,"");
			g_pub_tx.cif_no = 0;
			strcpy(g_pub_tx.opn_br_no,sWeixin.opn_br_no);
			vtcp_log("g_pub_tx.opn_br_no = %s",g_pub_tx.opn_br_no);
			g_pub_tx.ac_id=0;
			g_pub_tx.ac_seqn=0;
			
			strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
			strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
			g_pub_tx.svc_ind=1001;
			strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
			strcpy(g_pub_tx.ac_id_type,"");
			strcpy(g_pub_tx.note_type,"");
			strcpy(g_pub_tx.beg_note_no,"");
	
			if ( pub_acct_trance() )
			{
			  vtcp_log("调用存取款主控失败!");
			  goto ErrExit;
			}
			strcpy(g_pub_tx.opn_br_no,sWeixin.opn_br_no);
			set_zd_data("102K",g_pub_tx.ct_ind);
			set_zd_double("1002",g_pub_tx.tx_amt1);
			if ( pubf_acct_proc("LC02") )
			{
			  vtcp_log("登记会计流水错误!");
			  goto ErrExit;
			}     
			
            sprintf(acErrMsg,"入账成功");                     
			writefile(cTmpstr1,sWeixin,fp2,acErrMsg);
			WRITEMSG
  		
			/*账务处理  end*/	
			iLinenum++;	
			dSum_r+=sWeixin.bussi_amt;		
			memset(cTmpstr,0x00,sizeof(cTmpstr));	
			
		
		}
				
		sprintf(filename1,"JZBANK_WEIXIN_%ld_%s.txt",g_pub_tx.tx_date,cSeqn );
		strcpy(sWeixin_acct_reg.filename,filename1);
		 
		sWeixin_acct_reg.acct_seqn=iSeqn;
		sWeixin_acct_reg.acct_date=g_pub_tx.tx_date;			
		sWeixin_acct_reg.acct_time=g_pub_tx.tx_time;
		strcpy(sWeixin_acct_reg.sts,"2");  /*已处理*/
		
		ret=Weixin_acct_reg_Ins(sWeixin_acct_reg,g_pub_tx.reply);	
		if(ret)
		{			
			sprintf(acErrMsg,"登记weixin_acct_reg错误[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}
		
	}
	fclose(fp1);
	fclose(fp2);
	
	
	
OkExit:
	if(strcmp(g_pub_tx.reply,"0000"))
		strcpy(g_pub_tx.reply,"0000");
	vtcp_log("处理成功reply[%s]！",g_pub_tx.reply);
	ret=sql_commit();
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;	
	
ErrExit:
	vtcp_log("处理失败！[%s]",g_pub_tx.reply);
	ret=sql_rollback();
	fclose(fp1);
	fclose(fp2);
	fp1=NULL;
	fp2=NULL;
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;	
	

}


int writefile(  char cTmpstr[1024],struct weixin_acct_hst_c sWeixin,FILE * fp,char acErrMsg[30])
{
	int ret=0;
	char cTx_date[20];
	char cTx_date1[15];
	memset(cTx_date,0x00,sizeof(cTx_date));
	memset(cTx_date1,0x00,sizeof(cTx_date1));
	
	appGetTime(NULL,0,cTx_date);	
	strncpy(cTx_date1,cTx_date,14);
	cTmpstr[strlen(cTmpstr)-1]='';
	fprintf(fp,"%s|%ld|%s|%s\n",cTmpstr,g_pub_tx.trace_no,g_pub_tx.reply,acErrMsg);	
	
	if(!strcmp(g_pub_tx.reply,"0000"))
	{			
		
		sWeixin.sts[0]='1';
		strcpy(sWeixin.filler1,acErrMsg);
	}
	else 
	{
		sql_rollback();
		sql_begin();
		sWeixin.sts[0]='2';
		strcpy(sWeixin.filler1,acErrMsg);
	}
	sWeixin.hx_trace_no=g_pub_tx.trace_no;
	Weixin_acct_hst_Debug(&sWeixin);
	ret=Weixin_acct_hst_Ins(sWeixin,g_pub_tx.reply);
	if(ret)
	{			
		sprintf(acErrMsg,"登记weixin_acct_hst错误[%d]",ret );
		WRITEMSG
		return -1;
	}
	sql_commit();
	sql_begin();
	return 0;
}

int writehead(FILE* fp,double sum,int count)
{
	fprintf(fp,"%d|%lf|\n",count,sum);	
	return 0;
}
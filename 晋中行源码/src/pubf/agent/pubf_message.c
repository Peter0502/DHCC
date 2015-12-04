/***************************************************************
* 文 件 名: pubf_message.c
* 功能描述: 短信银行公共函数
* 入口参数：
* 作    者: jack
* 完成日期：2003年7月7日 
*
* 修改记录：
* 日    期: 2013-5-30
* 修 改 人: 刘烜
* 修改内容: 增加对方信息,原理是行内转账则取101,102域内容,支付系统取123域

insert into mob_server_type values ('1001', '对私活期', '0', 2.00, '1');
insert into mob_server_type values ('1002', '对公', '0', 0, '1');
insert into mob_server_type values ('1003', '贷款', '0', 0, '1');
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "mob_sendmail_c.h"
#include "mob_acct_type_c.h"
#include "hv_define.h"
#include "lv_define.h"
#include "lv_fd123.h"

/***************************************************************
* 函 数 名: nShouldSendMail 
* 功能描述: 判断是否需要记录短信表 
* 入口参数: 无
* 返 回 值: TRUE/FALSE
* 作    者: SSH 
* 完成日期: 20100707
* 工作原理: 在会计流水中查找201/211,找到则记录,找不到则不记录
*           此函数用于在基本确定需要记录短信的时候做的补充判断
*           此函数直接判断科目,不适于新会计准则系统 
*           此函数使用oracle的substr函数,不适于其他数据库
***************************************************************/
int nShouldSendMail(long ac_id){
	int count=0;
	count=sql_count("dd_mst_hst","tx_date=%ld and trace_no=%ld and ac_id=%ld",g_pub_tx.tx_date,g_pub_tx.trace_no,ac_id);
	vtcp_log("nSholdSendMail,sssssssssssssss%d条",count);
	if(count<0){
		vtcp_log("nShouldSendMail,数据库错误,SQLCODE=%d",count);
		return(0);
	}else if(count>0){
		vtcp_log("nSholdSendMail,发现记账记录%d条",count);
		return(1);
	}else{
		vtcp_log("nShouldSendMail,未发现记账记录,SQLCODE=%d",count);
		return(0);
	}		
}
/***************************************************************
* 函 数 名: pub_mob_sendmail
* 功能描述: 存取款交易（日间交易），写入mob_sendmail表，标志timeflag = 0
* 入口参数：
* 作    者: 
* 完成日期：
***************************************************************/
pub_mob_sendmail( char *Remark, char *cTrcode,  char *cAcno, char *cAdd_ind, double dTram, double dAcbl )
{
	int ret = 0;
	int iLog_flag = 0;
	char cMessage[141],EndMsg[61];
	char cRemark[64];
	char cAcno_tmp[21],tmp_name[61];
	int iLen = 0;
	int h,m,t,year,month,day;/*处理时间变量 h 小时 m 分 t 临时 */
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mob_sendmail_c sMob_sendmail;
	struct mob_acct_type_c sMob_acct_type;
	HV_FD123_AREA hv_fd123;	/* 大额系统信息 */
	LV_FD123_AREA lv_fd123;	/* 小额系统信息 */
	
	sprintf( acErrMsg,"进入发生额 dTram=[%ld],",dTram);
	WRITEMSG
	dTram=fabs(dTram);
	sprintf( acErrMsg,"进入发生额 dTram=[%ld],",dTram);
	WRITEMSG
	
	memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
	memset(EndMsg,'\0',sizeof(EndMsg));
	memset(tmp_name,'\0',sizeof(tmp_name));
	memset(&hv_fd123 , '\0', sizeof(hv_fd123));
	memset(&lv_fd123 , '\0', sizeof(lv_fd123));
	sprintf( acErrMsg,"交易码为========[%s]!!",cTrcode );
	WRITEMSG
	
	sprintf( acErrMsg,"进入参数 remark=[%s],cTrcode=[%s],cAcno=[%s],cAdd_ind=[%s],dTram=[%ld],dAcbl=[%ld]",\
								Remark,cTrcode,cAcno,cAdd_ind,dTram,dAcbl);
	WRITEMSG
		vtcp_log("g_pub_tx.tx_code=[%s]-----------\n",g_pub_tx.tx_code);
		vtcp_log("g_pub_tx.sub_tx_code=[%s]-----------\n",g_pub_tx.sub_tx_code);
     /*** 20100208增加tx_code6310且su_tx_code=F040不写短信   ***/
	if(memcmp(g_pub_tx.tx_code,"6310",strlen(g_pub_tx.tx_code))==0 && memcmp(g_pub_tx.sub_tx_code,"F040",strlen(g_pub_tx.sub_tx_code))==0 )
	{
		sprintf(acErrMsg,"sssss[%s][%d]交易[%s]不写短信表",__FILE__,__LINE__,g_pub_tx.sub_tx_code);
		WRITEMSG
		goto ErrExit;
	}    
	 /*** 20100621增加tx_code=9001不写短信 ***/
	if(memcmp(g_pub_tx.tx_code,"9001",strlen(g_pub_tx.tx_code))==0 )
	{
		sprintf(acErrMsg,"sssss[%s][%d]交易[%s]不写短信表",__FILE__,__LINE__,g_pub_tx.tx_code);
		WRITEMSG
		goto ErrExit;
	} 
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
		"ac_no = '%s'", cAcno );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply , "ac_id = %ld and mob_sts = '1'", \
		sMdm_ac_rel.ac_id );
	if ( ret )
	{
		sprintf( acErrMsg,"执行Mob_acct_type_Dec_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	vtcp_log("--------[%s]-----------\n",g_pub_tx.sub_tx_code);
	
	while (1)
	{	
		memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg,"执行Mob_acct_type_Fet_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		strcpy(cRemark,Remark);
		/**初始赋值timeflag=0为日间操作***/
		strcpy(	sMob_sendmail.timeflag, "0" );
vtcp_log("1111111111111111111111=[%s]\n",g_pub_tx.tx_code);
vtcp_log("2222222222222222222222=[%c]\n",cAdd_ind[0]);
vtcp_log("3333333333333333333333=[%c]\n",g_pub_tx.ct_ind[0]);
vtcp_log("4444444444444444444444=[%s]\n",g_pub_tx.sub_tx_code);
vtcp_log("zzzzzzzzzzzzzzzzzzzzzz=[%s]\n",cTrcode);



		if ( cAdd_ind[0] == '0' )
		{
			if(strcmp(cTrcode,"9330")==0){
				strcpy(cRemark,"电话转出");
			}else if(strcmp(cTrcode,"6203")==0)
			{
				if(strlen(cRemark)<=0){
					strcpy(cRemark,"ATM取款");
				}
				if(strcmp(cRemark,"网银转账")==0){
					strcat(cRemark,"转出");
				}else if(strcmp(cRemark,"他代本取款")==0){
					strcpy(cRemark,"自助机具取款");
				}else if(strcmp(cRemark,"他代本转出")==0){
					strcpy(cRemark,"自助机具转出");
				}else if(strcmp(cRemark,"他代本消费")==0){
					strcpy(cRemark,"消费");
				}else if(strcmp(cRemark,"他代本代收")==0){
					strcpy(cRemark,"代扣");
				}else if(strcmp(cRemark,"他代本预授权")==0){
					strcpy(cRemark,"预授权");
				}else if(strcmp(cRemark,"他代本预完成")==0){
					strcpy(cRemark,"预授权完成");
				}else if(strcmp(cRemark,"他代本柜面取款")==0){
					strcpy(cRemark,"银联柜面通取款");
				}else if(strcmp(cRemark,"柜面通他代本转出")==0){
					strcpy(cRemark,"银联柜面通转出");
				}else if(strcmp(cRemark,"柜面通他代本转出")==0){
					strcpy(cRemark,"银联柜面通转出");
				}else if(strcmp(cRemark,"财付通网上支付")==0){
					strcpy(cRemark,"财付通充值");
				}else if(strcmp(cRemark,"卡通网上支付")==0){
					strcpy(cRemark,"支付宝充值");
				}else if(strcmp(cRemark,"他行POS冲正")==0){
					strcpy(cRemark,"消费冲正");
				}else if(strcmp(cRemark,"银联柜面通冲正")==0){
					strcpy(cRemark,"银联柜面通取款冲正");
				}else if(strcmp(cRemark,"IC行内自助指定圈冲")==0){
					strcpy(cRemark,"自助机具指定圈存转出冲正");
				}else if(strcmp(cRemark,"IC卡非指圈转出冲")==0){
					strcpy(cRemark,"自助机具非指定圈存转出冲正");	
				}else if(strcmp(cRemark,"IC卡非指圈存转出")==0){
					strcpy(cRemark,"自助机具非指定圈存转出");					
				}else if(strcmp(cRemark,"IC卡指定圈存转冲")==0){
					strcpy(cRemark,"自助机具指定圈存转出冲正");	
				}else if(strcmp(cRemark,"POS非指定圈存转出冲")==0){
					strcpy(cRemark,"POS非指定圈存转出冲正");
				}else if(strcmp(cRemark,"银联柜面通存款撤销冲")==0){
					strcpy(cRemark,"银联柜面通存款撤销冲正");				
				}else if(strcmp(cRemark,"柜面通他代本存款撤销")==0){
					strcpy(cRemark,"柜面通存款撤销");
				}
			}else if(strcmp(cTrcode,"6205")==0){
				strcpy(cRemark,"卡转取"); 
			}else if(strcmp(cTrcode,"3703")==0){
				strcpy(cRemark,"柜面圈存转出");
			}else if(sMob_acct_type.ac_no[0] == '6' && strcmp(cTrcode,"5802")==0){
				if(strcmp(g_pub_tx.sub_tx_code,"DEWZ")==0)
				{
					strcpy(cRemark,"大额转出"); 
				}
				else if(strcmp(g_pub_tx.sub_tx_code,"SF02")==0)
				{
					strcpy(cRemark,"大额手续费支出"); 
				}
			}else if(sMob_acct_type.ac_no[0] == '6' && strcmp(cTrcode,"5842")==0){
				if(strcmp(g_pub_tx.sub_tx_code,"XEJZ")==0)
				{
					strcpy(cRemark,"小额转出"); 
				}
				else if(strcmp(g_pub_tx.sub_tx_code,"SF02")==0)
				{
					strcpy(cRemark,"小额手续费支出"); 
				}
			}else if(sMob_acct_type.ac_no[0] == '6' && strcmp(cTrcode,"5701")==0){
				if(strcmp(g_pub_tx.sub_tx_code,"DEWZ")==0)
				{
					strcpy(cRemark,"大额转出"); 
				}
				else if(strcmp(g_pub_tx.sub_tx_code,"XEJZ")==0)
				{
					strcpy(cRemark,"小额转出"); 
				}
				else if(strcmp(g_pub_tx.sub_tx_code,"SF02")==0)
				{
					strcpy(cRemark,"手续费"); 
				}
		  }else if(strcmp(cTrcode,"5802")==0 && strcmp(g_pub_tx.sub_tx_code,"DEWZ")==0){
					strcpy(cRemark,"汇划"); 
			}else if(strcmp(cTrcode,"5802")==0 && strcmp(g_pub_tx.sub_tx_code,"SF02")==0){
		    	strcpy(cRemark,"汇划费");
		  }else if(strcmp(cTrcode,"5842")==0 && strcmp(g_pub_tx.sub_tx_code,"XEJZ")==0){
		    	strcpy(cRemark,"汇划");
		  }else if(strcmp(cTrcode,"5842")==0 && strcmp(g_pub_tx.sub_tx_code,"SF02")==0){
		    	strcpy(cRemark,"汇划费");
		  }else if(strcmp(cTrcode,"5844")==0 && strcmp(g_pub_tx.sub_tx_code,"XEJZ")==0){
		    	strcpy(cRemark,"汇划");
		  }else if(strcmp(cTrcode,"5844")==0 && strcmp(g_pub_tx.sub_tx_code,"SF02")==0){
		    	strcpy(cRemark,"汇划费");
		  }else if(strcmp(cTrcode,"4335")==0){
		    	strcpy(cRemark,"柜面转出");  
	    }else if(Remark[0] =='\0'&& strcmp(cTrcode,"6306")==0){
	  		strcpy(cRemark,"预授权");
	    }else if(Remark[0] =='\0'&& strcmp(cTrcode,"6310")==0){
	  		strcpy(cRemark,"预授权完成");
	  	}else if(g_pub_tx.ct_ind[0] == '2'){
	  		if(strcmp(cTrcode,"2206")==0 || strcmp(cTrcode,"2202")==0 ||strcmp(cTrcode,"2101")==0 || strcmp(cTrcode,"2106")==0 || strcmp(cTrcode,"2502")==0){/*柜面转出交易*/
	  			strcpy(cRemark,"柜面转出");
	  		}
	  	}else if(g_pub_tx.ct_ind[0] == '1'){
	  		if(strcmp(cTrcode,"2206")==0 || strcmp(cTrcode,"2202")==0){/*柜面取款交易*/
	  			strcpy(cRemark,"柜面取款");
	  		}
	    }else if(strcmp(cTrcode,"5807")==0 && strcmp(g_pub_tx.sub_tx_code,"HPDF")==0){
	    	vtcp_log("aaaaaaaaaaaaaaaaa=[%s][%s][%s]\n",cRemark,cTrcode,g_pub_tx.sub_tx_code);
	  		strcpy(cRemark,"签发汇票");
	    /*}else if(memcmp(g_pub_tx.sub_tx_code,"Z027",strlen(g_pub_tx.sub_tx_code))==0){
	  		strcpy(	sMob_sendmail.timeflag, "1" );
	  	}else if(strcmp(cTrcode,"DX09")==0){
	  		strcpy(	sMob_sendmail.timeflag, "1" );	  		
	  	}else if(memcmp(g_pub_tx.sub_tx_code,"Z161",strlen(g_pub_tx.sub_tx_code))==0){
	  		strcpy(cRemark,"划出");
	  		strcpy(	sMob_sendmail.timeflag, "1" );*/
	    }else if(Remark[0] =='\0'){
	    	strcpy(cRemark,"取款");
			}
			/* 新增对方信息功能 added by liuxuan 2013-5-30 */
			if(g_pub_tx.ct_ind[0]=='2' && memcmp(g_pub_tx.sub_tx_code,"SF",2)!=0){	/* SF是收费 */
				get_zd_data("1018",tmp_name);				
				if(strlen(tmp_name)<4){	/* 从101域取出空值说明是通过支付系统转账 */
					if(memcmp(g_pub_tx.tx_code,"6388",4)==0||memcmp(g_pub_tx.tx_code,"5842",4)==0){	/* 小额往账 */
						get_fd_data("1230",(char *)&lv_fd123);
						/*memcpy(tmp_name,lv_fd123+295,60);*/
						memcpy(tmp_name,lv_fd123.cash_name,60);
					}
					else if(memcmp(g_pub_tx.tx_code,"6389",4)==0||memcmp(g_pub_tx.tx_code,"5802",4)==0){ /* 大额 */
							get_fd_data("1230",(char *)&hv_fd123);
							/*memcpy(tmp_name,fd123+270,60);*/
							memcpy(tmp_name,hv_fd123.cash_name,60);
					}
				}			
				sprintf(EndMsg,"收款人:%s",tmp_name);
			}
		}
		else if ( cAdd_ind[0] == '1' )
		{
			if(strcmp(cRemark,"网银转账")==0){
					strcat(cRemark,"转入");
			}
			if(strcmp(cTrcode,"9330")==0){
				strcpy( cRemark, "电话转入" );
			}else if(strcmp(cTrcode,"6203")==0)
			{
				if(strlen(cRemark)<=0){
					strcpy(cRemark,"ATM存款");
				}
				if(strcmp(cRemark,"网银转账")==0){
					strcat(cRemark,"转入");
				}else if(strcmp(cRemark,"他代本存款")==0){
					strcpy(cRemark,"自助机具存款");
				}else if(strcmp(cRemark,"自助机具转出")==0){
					strcpy(cRemark,"自助机具转入");
				}else if(strcmp(cRemark,"他代本转入")==0){
					strcpy(cRemark,"自助机具转入");
				}else if(strcmp(cRemark,"他代本消撤销")==0){
					strcpy(cRemark,"消费撤销");
				}else if(strcmp(cRemark,"他代本退货")==0){
					strcpy(cRemark,"退货");
				}else if(strcmp(cRemark,"他代本代发")==0){
					strcpy(cRemark,"代发");
				}else if(strcmp(cRemark,"他代本柜面存款")==0){
					strcpy(cRemark,"银联柜面通存款");
				}else if(strcmp(cRemark,"柜面通他代本转入")==0){
					strcpy(cRemark,"银联柜面通转入");
				}else if(strcmp(cRemark,"他代本预撤销")==0){
					strcpy(cRemark,"预授权撤销");
				}else if(strcmp(cRemark,"他代本预完撤销")==0){
					strcpy(cRemark,"预授权完成撤销");
				}else if(strcmp(cRemark,"财付通单笔提现")==0){
					strcpy(cRemark,"财付通提现");
				}else if(strcmp(cRemark,"卡通单笔退货")==0){
					strcpy(cRemark,"支付宝退款");
				}else if(strcmp(cRemark,"卡通单笔提现")==0){
					strcpy(cRemark,"支付宝提现");
				}else if(strcmp(cRemark,"他行POS冲正")==0){
					strcpy(cRemark,"消费冲正");
				}else if(strcmp(cRemark,"他代本预完撤冲正")==0){
					strcpy(cRemark,"预授权完成撤销冲正");
				}else if(strcmp(cRemark,"IC卡非指定圈存转入")==0){
					strcpy(cRemark,"自助机具非指定圈存转入");
				}else if(strcmp(cRemark,"POS非指定圈存转入冲")==0){
					strcpy(cRemark,"POS非指定圈存转入冲正");	
				}else if(strcmp(cRemark,"他代本预撤冲")==0){
					strcpy(cRemark,"预授权撤销冲正");
				}
			}else if(strcmp(cTrcode,"6206")==0){
				strcpy( cRemark,"CDM存款"); 
			}else if(strcmp(cTrcode,"6205")==0){
				strcpy( cRemark,"卡转存"); 
			}else if(sMob_acct_type.ac_no[0] == '6' && strcmp(cTrcode,"5839")==0){
					strcpy(cRemark,"大额来帐"); 
			}else if(sMob_acct_type.ac_no[0] == '6' && strcmp(cTrcode,"5840")==0){
		    	strcpy(cRemark,"小额来帐");
		  }else if(sMob_acct_type.ac_no[0] == '5' && strcmp(cTrcode,"5810")==0)
			{
					strcpy(cRemark,"行外来账转入"); 
			}else if(sMob_acct_type.ac_no[0] == '5' && strcmp(cTrcode,"5811")==0)
			{
					strcpy(cRemark,"行外来账转入"); 
			}else if(sMob_acct_type.ac_no[0] == '5' && strcmp(cTrcode,"5817")==0)
			{
					strcpy(cRemark,"行外来账转入"); 
			}else if(sMob_acct_type.ac_no[0] == '5' && strcmp(cTrcode,"5818")==0)
			{
					strcpy(cRemark,"行外来账转入"); 
			}else if(sMob_acct_type.ac_no[0] == '5' && strcmp(cTrcode,"4373")==0)
			{
					strcpy(cRemark,"柜面转入"); 
			}else if(strcmp(cTrcode,"3706")==0){
				strcpy(cRemark,"柜面圈提转入");
	  	}else if(g_pub_tx.ct_ind[0] == '2'){
	  		if(strcmp(cTrcode,"2206")==0 || strcmp(cTrcode,"2202")==0 ||strcmp(cTrcode,"2207")==0 ||strcmp(cTrcode,"2208")==0 || strcmp(cTrcode,"2502")==0){/*柜面转入交易*/
	  			strcpy(cRemark,"柜面转入");
	  		}		
	  	}else if(g_pub_tx.ct_ind[0] == '1'){
	  		if(strcmp(cTrcode,"2206")==0 || strcmp(cTrcode,"2201")==0 || strcmp(cTrcode,"2205")==0){/*柜面存款交易*/
	  			strcpy(cRemark,"柜面存款");
	  		}
		  /*}
		  else if(memcmp(g_pub_tx.sub_tx_code,"Z027",strlen(g_pub_tx.sub_tx_code))==0){
		  		strcpy(	sMob_sendmail.timeflag, "1" );
		  }else if(memcmp(g_pub_tx.sub_tx_code,"Z161",strlen(g_pub_tx.sub_tx_code))==0){
		  		
		  		strcpy(	sMob_sendmail.timeflag, "1" );*/
		  }else if(strcmp(cTrcode,"5807")==0 && strcmp(g_pub_tx.sub_tx_code,"HPDF")==0){
	    	vtcp_log("bbbbbbbbbbbbbbbbbb=[%s][%s][%s]\n",cRemark,cTrcode,g_pub_tx.sub_tx_code);
	  		strcpy(cRemark,"签发汇票");
		  }
		  
		  /**添加遂心存发短信内容 zqbo 20141205**/
		  else if ( memcmp(cTrcode,"Z156",4)==0 )
			{
			       	strcat(Remark, "存入");
			}
			else if ( memcmp(cTrcode,"Z157",4)==0 )
			{
			       	strcat(Remark, "存入");
			}
			else if ( memcmp(cTrcode,"2316",4)==0 )
			{
			       	strcat(Remark, "结息存入");
			}
			
			else if(Remark[0] =='\0'){
				strcpy(cRemark,"存款"); 
			}
			/* 新增对方信息功能 added by liuxuan 2013-5-30 */
			if(g_pub_tx.ct_ind[0]=='2'){
				get_zd_data("102H",tmp_name);				
				if(strlen(tmp_name)<4){	/* 从102域取出空值说明是通过支付系统转账 */
					if(memcmp(g_pub_tx.tx_code,"5840",4)==0){	/* 小额来账 */						
						get_fd_data("1230",(char *)&lv_fd123);
						/*memcpy(tmp_name,fd123+111,60);*/
						memcpy(tmp_name,lv_fd123.pay_name,60);
					}
					else if(memcmp(g_pub_tx.tx_code,"5839",4)==0){ /* 大额 */
						get_fd_data("1230",(char *)&hv_fd123);
						/*memcpy(tmp_name,fd123+78,60);*/
						memcpy(tmp_name,hv_fd123.pay_name,60);
					}
				}
				sprintf(EndMsg,"付款人:%s",tmp_name);	
			}
		}
vtcp_log("55555555555555555555555=[%s]\n",cRemark);
		pub_base_strpack(EndMsg);

		iLog_flag = 0;
		pub_base_strpack( sMob_acct_type.server_type );
vtcp_log("66666666666666666666666=[%s]\n",sMob_acct_type.server_type);
vtcp_log("66666666666666666666666=[%s]\n",cTrcode);
		/*----传了摘要后--disply----很多无用的代码---------
		if ((strcmp( sMob_acct_type.server_type, "1001" ) == 0 \
			 || strcmp( sMob_acct_type.server_type,"1007") == '1' ) \
			&& cAdd_ind[0] == '0' && ( strcmp( cTrcode, "6212" ) == 0 \
			|| strcmp( cTrcode, "6601" ) == 0 \
			|| strcmp( cTrcode, "6604" ) == 0 \
			|| strcmp( cTrcode, "6705" ) == 0 \
			|| strcmp( cTrcode, "9001" ) == 0 ) )
		{
			iLog_flag = 1;
			strcpy( cRemark, "行内代扣" );
		}
		else if (strcmp( sMob_acct_type.server_type, "1001" ) == 0 || strcmp( sMob_acct_type.server_type, "1002" ) == 0)
		{
			iLog_flag = 1;
		}else if ( (strcmp( sMob_acct_type.server_type, "1001" ) == 0 || strcmp( sMob_acct_type.server_type, "1006" ) == 0 ) \
			&& cAdd_ind[0] == '1' && ( strcmp( cTrcode, "6601" ) == 0 \
		 	|| strcmp( cTrcode, "6604" ) == 0  || strcmp( cTrcode, "9001" ) == 0 ) )
		{
			iLog_flag = 1;
			strcpy( cRemark, "代发" );
		}else if( (strcmp( sMob_acct_type.server_type, "1001" ) == 0 )&&(strcmp(cTrcode,"2405")==0 || strcmp(cTrcode,"G008")==0 || strcmp(cTrcode,"6641")==0 || strcmp(cTrcode,"D806"))==0)
		{
					iLog_flag=2;
					sprintf( acErrMsg,"不写记录交易码为[%s]!!",cTrcode );
					WRITEMSG
		}else if(strcmp( sMob_acct_type.server_type, "1001" ) == 0 )
		{
					iLog_flag=1;
					sprintf( acErrMsg,"写记录交易码为[%s]!!",cTrcode );
					WRITEMSG
		}
		*/
		
		if(strcmp( sMob_acct_type.server_type, "1001" ) == 0 || strcmp( sMob_acct_type.server_type, "1002" ) == 0 ||strcmp( sMob_acct_type.server_type, "1003" ) == 0 )
		{
					iLog_flag=1;
					sprintf( acErrMsg,"写记录交易码为[%s]!!",cTrcode );
					WRITEMSG
		}
		
		if( strcmp(cTrcode,"2405")==0 || strcmp(cTrcode,"G008")==0 || strcmp(cTrcode,"6641")==0 || strcmp(cTrcode,"D806")==0)
		{
					iLog_flag=0;
					sprintf( acErrMsg,"不写记录交易码为[%s]!!",cTrcode );
					WRITEMSG
		}/***燃气缴费发送短信通知20140616 前台代码9923****/
		else if( strcmp(cTrcode,"6203")==0 && strcmp(g_pub_tx.sub_tx_code,"D003")==0 )
		{
					iLog_flag=1;
					sprintf( acErrMsg,"不写记录交易码为[%s]!!",cTrcode );
					WRITEMSG
		}/****不加如下语句，会导致存款，取款不发送短信*****/
		
/***Add by SSH,20100707,追加判断如果大小额没记客户帐则不发送短信**/
		if(iLog_flag && 
			(strcmp( cTrcode, "5839" ) == 0 ||strcmp( cTrcode, "5840" ) == 0)){
			if(!nShouldSendMail(sMdm_ac_rel.ac_id)){
				iLog_flag=0;
			}
		}/**七天通知存款在解约时，是将虚拟账户钱转入到账户上，不进行短信发送**/
		if(strcmp( cTrcode, "0013" ) == 0 && memcmp(g_pub_tx.sub_tx_code,"D099",strlen(g_pub_tx.sub_tx_code))==0)
		{
				iLog_flag=0;
		}
		/**七天通知存款划出生产已上线，宋江柄要求先屏蔽掉，对公上线时再放开20140626**/
		if(memcmp(g_pub_tx.sub_tx_code,"D322",strlen(g_pub_tx.sub_tx_code))==0)
		{
				iLog_flag=1;
		}
		/***七天通知存款划入生产已上线，宋江柄要求先屏蔽掉，对公上线时再放开20140626***/
		if(memcmp(g_pub_tx.sub_tx_code,"Z161",strlen(g_pub_tx.sub_tx_code))==0)
		{
				iLog_flag=1;
		}
		/**预授权撤销冲正F039，预授权F036，预授权冲正F037，预授权撤销F038，不进行短信发送**/
		if((memcmp(g_pub_tx.sub_tx_code,"F039",strlen(g_pub_tx.sub_tx_code))==0) \
			|| (memcmp(g_pub_tx.sub_tx_code,"F036",strlen(g_pub_tx.sub_tx_code))==0) \
			|| (memcmp(g_pub_tx.sub_tx_code,"F037",strlen(g_pub_tx.sub_tx_code))==0) \
			|| (memcmp(g_pub_tx.sub_tx_code,"F038",strlen(g_pub_tx.sub_tx_code))==0))
		{
				iLog_flag=0;
		}
		if (iLog_flag==1 )
		{
			memset( cMessage, 0x0, sizeof(cMessage) );
			pub_base_strpack( cAcno );
			memset( cAcno_tmp, 0x0, sizeof(cAcno_tmp) );
			strcpy( cAcno_tmp, cAcno );
			iLen = strlen(cAcno_tmp);

			/*
			cAcno_tmp[iLen-3] = '*';
			cAcno_tmp[iLen-4] = '*';
			cAcno_tmp[iLen-5] = '*';
			cAcno_tmp[iLen-6] = '*';
			*/

			strcpy(cAcno_tmp,cAcno+iLen-4);
			/*处理时间*/
			t=g_pub_tx.tx_time/100;	
			h=t/100;
			m=t%100;
			/*month=(g_pub_tx.tx_date/100)%100;
			day=g_pub_tx.tx_date%100;*/
			pub_base_get_date(&year, &month, &day);  /*mod by lwb 20150519   存取款取自然日期  不是com_sys_parm中的日期*/
			vtcp_log("vvvvvvv year=%d ,month=%d, day=%d",year,month,day);
			
			if(sMob_acct_type.ac_no[0] == '1')
			{
				if(strcmp(cTrcode,"0008")==0 || strcmp(cTrcode,"0013")==0)
				{
					sprintf( cMessage, "贵账户%s于%d月%d日%d时%02d分%s成功。疑电4006535666", \
							cAcno_tmp, month,day,h,m,cRemark);
				}
			}
			/*
			if(sMob_acct_type.ac_no[0] == '6')
			{
				if(strcmp(cTrcode,"0008")==0 || strcmp(cTrcode,"0013")==0)
				{
					sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d%s成功。", \
							cAcno_tmp, month,day,h,m,cRemark);
				}
				if(strcmp(cTrcode,"2410")==0)
				{
					sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d进行短信签约%s。", \
							cAcno_tmp, month,day,h,m,cRemark);
				}
			}
		
			if(strcmp(cTrcode,"2405")==0){
					sprintf( cMessage, "您尾号%s%s账户成功!疑电4006535666",cAcno_tmp,cRemark);
			}else if(strcmp(cTrcode,"9306")==0){
				sprintf( cMessage, "您尾号%s账户电话银行口挂成功,请五日内到柜台办理书面挂失!疑电4006535666",cAcno_tmp);
			}else if(strcmp(cTrcode,"2602")==0){
				sprintf( cMessage, "您尾号%s于%d月%d日%d:%02d%s。疑电4006535666", \
					cAcno_tmp, month,day,h,m,cRemark);
			}else if(cAdd_ind[0] == '0')
*/
			if(cAdd_ind[0] == '0')
			{
				if(sMob_acct_type.ac_no[0] == '1')
				{
					if(strcmp(cTrcode,"Z161")==0){
						sprintf( cMessage, "贵账户%s七天通知存款户已划入%.2lf元，将按七天通知存款利率计息，余额%.2lf元。", \
						cAcno_tmp,dTram, dAcbl);
						strcpy(	sMob_sendmail.timeflag, "1" );
					}else if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '1' ){
						sprintf( cMessage, "贵账户%s于%d月%d日%d时%02d分现金%s%.2lf元记账错误，撤销处理，余额%.2lf元。疑电4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}else if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '2'){
						sprintf( cMessage, "贵账户%s于%d月%d日%d时%02d分转账%s%.2lf元记账错误，撤销处理，余额%.2lf元。疑电4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}else if(strcmp(cTrcode,"2219")==0 ){
						sprintf( cMessage, "贵账户%s于%d月%d日%d时%02d分成功%s，金额%.2lf元。详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram);
					}else if(strcmp(cTrcode,"6238")==0 ){
						sprintf( cMessage, "贵折%s于%d月%d日%d时%02d分购买%s%.2lf元,余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl); 
					}else if(strcmp(cTrcode,"G188")==0 ){

						sprintf( cMessage, "贵账户%s于%d月%d日%d时%02d分%s，金额%.2lf元，余额%.2lf元。如有疑问请咨询您的理财经理或详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram,dAcbl);
					}else{
						sprintf( cMessage, "贵折%s于%d月%d日%d时%02d分%s，金额%.2lf元，余额%.2lf元。疑电4006535666", \
						cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}
				}
				else if(sMob_acct_type.ac_no[0] == '6')
				{
					if(strcmp(cTrcode,"Z161")==0){
						sprintf( cMessage, "贵账户%s七天通知存款户已划入%.2lf元，将按七天通知存款利率计息，余额%.2lf元。", \
						cAcno_tmp,dTram, dAcbl);
						strcpy(	sMob_sendmail.timeflag, "1" );
					}else if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '1' ){
						sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d%s撤销%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}else if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '2'){
						sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d%s撤销%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}else if(strcmp(cTrcode,"5702")==0){
						sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d柜面转出冲正%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,dTram, dAcbl);
					}else if(strcmp(cTrcode,"2219")==0 ){
						sprintf( cMessage, "您尾号%s于%d月%d日%d时%02d分成功%s，金额%.2lf元。详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram);
					}else if(strcmp(cTrcode,"6238")==0 ){
						sprintf( cMessage, "您尾号%s于%d月%d日%d时%02d分购买%s%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl); 
					}else if(strcmp(cTrcode,"G188")==0 ){

						sprintf( cMessage, "您尾号%s于%d月%d日%d时%02d分%s，金额%.2lf元，余额%.2lf元。如有疑问请咨询您的理财经理或详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram,dAcbl);
					}
					else{
						sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d%s%.2lf元，余额%.2lf元。", \
						cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}
				}
				else if(sMob_acct_type.ac_no[0] == '5')
				{
					if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '1' ){
						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分柜面取款%.2lf元记账错误，撤销处理，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,dTram, dAcbl);
					}else if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '2'){
						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分柜面转出%.2lf元记账错误，撤销处理，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,dTram, dAcbl);
					}else if(strcmp(cTrcode,"5702")==0){
						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分记账错误%s%.2lf元，余额%.2lf元。", \
						cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}else if(strcmp(cTrcode,"DX08")==0){
						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分短信扣费失败，扣费金额%.2lf元，余额%.2lf元，请充值。", \
						cAcno_tmp, month,day,h,m,dTram, dAcbl);
					}else if(strcmp(cTrcode,"DX07")==0){
						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分短信欠费超3个月，现暂停短信通知业务，请尽快充值。", \
						cAcno_tmp, month,day,h,m,dTram, dAcbl);
					}else if(strcmp(cTrcode,"2219")==0 ){
						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分成功%s，金额%.2lf元。详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram);
					}else if(strcmp(cTrcode,"G188")==0 ){

						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分%s，金额%.2lf元，余额%.2lf元。如有疑问请咨询您的理财经理或详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram,dAcbl);
					}
					else
					{
							sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分%s，金额%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}
				}
			}else if(cAdd_ind[0] == '1')
			{
				if(sMob_acct_type.ac_no[0] == '1'){
					if(memcmp(g_pub_tx.sub_tx_code,"D322",strlen(g_pub_tx.sub_tx_code))==0)
					{
						sprintf( cMessage, "您尾号%s于%d月%d日七天通知存款产生利息%.2lf元，本息合计%.2lf元，余额%.2lf元。", \
						cAcno_tmp,month,day,dTram, dAcbl,dAcbl);
						strcpy(	sMob_sendmail.timeflag, "1" );
					}else if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '1' ){
						sprintf( cMessage, "贵账户%s于%d月%d日%d时%02d分现金%s%.2lf元记账错误，撤销处理，余额%.2lf元。疑电4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}else if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '2'){
						sprintf( cMessage, "贵折于%d月%d日%d时%02d分转账%s%.2lf元记账错误，撤销处理，余额%.2lf元。疑电4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}else if(strcmp(cTrcode,"0089") == 0){
						sprintf( cMessage, "贵账户%s认购的理财产品已失败处理，于%d月%d日%d时%02d分成功撤销%s金额%.2lf元。详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram);
					}
					else if(strcmp(cTrcode,"2219")==0 ){

						sprintf( cMessage, "贵账户%s于%d月%d日%d时%02d分成功%s，金额%.2lf元。详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram);
					}
	        else if(strcmp(cTrcode,"G188")==0 ||strcmp(cTrcode,"2224")==0){

						sprintf( cMessage, "贵账户%s于%d月%d日%d时%02d分%s，金额%.2lf元，余额%.2lf元。如有疑问请咨询您的理财经理或详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram,dAcbl);
					}else if(strcmp(cTrcode,"6238")==0 ){
						sprintf( cMessage, "贵折%s于%d月%d日%d时%02d分赎回%s%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl); 
					}
					else if(strcmp(cTrcode,"2410")==0){
				
					sprintf( cMessage, "贵折%s于%d月%d日%02d:%02d进行短信签约%s。", \
							cAcno_tmp, month,day,h,m,cRemark);
					}
					else if(strcmp(cTrcode,"2602")==0){
					sprintf( cMessage, "贵折%s于%d月%d日%d:%02d%s。疑电4006535666", \
					cAcno_tmp, month,day,h,m,cRemark);
					}
					else{
						sprintf( cMessage, "贵折%s于%d月%d日%d时%02d分%s，金额%.2lf元，余额%.2lf元。疑电4006535666", \
						cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}
				}
				else if(sMob_acct_type.ac_no[0] == '6')
				{
					if(memcmp(g_pub_tx.sub_tx_code,"D322",strlen(g_pub_tx.sub_tx_code))==0)
					{
						sprintf( cMessage, "您尾号%s于%d月%d日七天通知存款产生利息%.2lf元，本息合计%.2lf元，余额%.2lf元。", \
						cAcno_tmp,month,day,dTram, dAcbl,dAcbl);
						strcpy(	sMob_sendmail.timeflag, "1" );
					}else if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '1' ){
						sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d%s撤销%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}else if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '2'){
						sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d%s撤销%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}else if(strcmp(cTrcode,"5702")==0){
						sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d柜面取款冲正%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,dTram, dAcbl);
					}else if(strcmp(cTrcode,"0089") == 0){
						sprintf( cMessage, "您尾号%s认购的理财产品已失败处理，于%d月%d日%d时%02d分成功撤销%s金额%.2lf元。详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram);
					}	else if(strcmp(cTrcode,"2219")==0 ){
						sprintf( cMessage, "您尾号%s于%d月%d日%d时%02d分成功%s，金额%.2lf元。详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram);
					}
					else if(strcmp(cTrcode,"G188")==0 ||strcmp(cTrcode,"2224")==0){

						sprintf( cMessage, "您尾号%s于%d月%d日%d时%02d分%s，金额%.2lf元,余额%.2lf。如有疑问请咨询您的理财经理或详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram,dAcbl);
					}else if(strcmp(cTrcode,"6238")==0 ){
						sprintf( cMessage, "您尾号%s于%d月%d日%d时%02d分赎回%s%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl); 
					}
					else if(strcmp(cTrcode,"2410")==0)
					{
						sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d进行短信签约%s。", \
							cAcno_tmp, month,day,h,m,cRemark);
					}
					else if(strcmp(cTrcode,"2602")==0){
					sprintf( cMessage, "您尾号%s于%d月%d日%d:%02d%s。疑电4006535666", \
					cAcno_tmp, month,day,h,m,cRemark);
					}
					else{
						sprintf( cMessage, "您尾号%s于%d月%d日%02d:%02d%s%.2lf元，余额%.2lf元。", \
						cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}
				}
				else if(sMob_acct_type.ac_no[0] == '5')
				{
					if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '1' ){
						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分柜面存款%.2lf元记账错误，撤销处理，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,dTram, dAcbl);
					}else if(strcmp(cTrcode,"5701")==0 && g_pub_tx.ct_ind[0] == '2'){
						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分柜面转入%.2lf元记账错误，撤销处理，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,dTram, dAcbl);
					}else if(strcmp(cTrcode,"5702")==0){
						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分记账错误%s%.2lf元，余额%.2lf元。", \
						cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}else if(strcmp(cTrcode,"0089") == 0){
						sprintf( cMessage, "贵单位%s认购的理财产品已失败处理，于%d月%d日%d时%02d分成功撤销%s金额%.2lf元。详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram);
					}
					else if(strcmp(cTrcode,"2219")==0 ){

						sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分成功%s，金额%.2lf元。详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram);
					}
					else if(strcmp(cTrcode,"G188")==0 ||strcmp(cTrcode,"2224" )==0){

						sprintf( cMessage, "贵单位%s于%d月%d日%d时%02d分%s，金额%.2lf元，余额%.2lf。如有疑问请咨询您的理财经理或详询4006535666", \
							cAcno_tmp, month,day,h,m,cRemark,dTram,dAcbl);
					}
					else
					{
							sprintf( cMessage, "贵单位%s账户于%d月%d日%d时%02d分%s，金额%.2lf元，余额%.2lf元。", \
							cAcno_tmp, month,day,h,m,cRemark,dTram, dAcbl);
					}
				}
			}
			
			else if(cAdd_ind[0] == '2')   /*纯通知类   没有金额变化   add by lwb  20141029*/
			{
				if(memcmp(g_pub_tx.sub_tx_code,"FXPG",strlen(g_pub_tx.sub_tx_code))==0)
				{
					sprintf( cMessage, "您在本行做的理财风险等级评估%s，请及时到营业厅进行续估或重新评估，本行将于近期发行新的理财产品，敬请购买。详询4006535666", \
					cRemark);
		
				}
				if(memcmp(g_pub_tx.sub_tx_code,"D084",strlen(g_pub_tx.sub_tx_code))==0)
				{
					sprintf(cMessage,"您购买理财产品的尾号为%s的账号于%d月%d日%d时%02d分已成功变更为尾号为%s的账号。详询4006535666",cAcno_tmp,month,day,h,m,cRemark);
				}
			}
			
			if(strcmp(cTrcode,"DX09")==0 || strcmp(cTrcode,"DX08")==0 || strcmp(cTrcode,"DX07")==0)
			{
	  		strcpy(	sMob_sendmail.timeflag, "1" );
	  		vtcp_log("vvvvvvvvvvvvvvvvvvvvvvv=[%s]\n",sMob_sendmail.timeflag);
			}	
			strcpy( sMob_sendmail.tx_code, cTrcode );
			strcpy( sMob_sendmail.ac_no, cAcno );
			sMob_sendmail.ac_id = sMdm_ac_rel.ac_id;
			strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
			sMob_sendmail.trace_no = g_pub_tx.trace_no;
			sMob_sendmail.tx_date = g_pub_tx.tx_date;
			sMob_sendmail.tx_time = g_pub_tx.tx_time;
			strcpy( sMob_sendmail.opt_no, "110" );
			strcpy( sMob_sendmail.mobile, sMob_acct_type.mobile );
			strcpy( sMob_sendmail.message, cMessage );
			sMob_sendmail.tt_no = 0;
			sMob_sendmail.t_date = 0;
			sMob_sendmail.t_time = 0;
			strcpy(	sMob_sendmail.send_level, "1" );
			sMob_sendmail.sene_times = 1;
			sMob_sendmail.send_date = 0;
			sMob_sendmail.send_time = 0;
			sMob_sendmail.sxf_amt = 0.00;
			strcpy(	sMob_sendmail.proc_flag, "0" );
			sMob_sendmail.mobseq = 0;
	        strcpy(sMob_sendmail.tx_type, "0" );	
	    Mob_sendmail_Debug(&sMob_sendmail);
        	ret = Mob_sendmail_Ins(sMob_sendmail,g_pub_tx.reply );
			if ( ret != 0 ) 
			{
				sprintf( acErrMsg, "写mob_sendmail表失败!!![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		/*break;*/
	}

	Mob_acct_type_Clo_Sel();


OkExit:
	strcpy(g_pub_tx.reply,"0000");

	if (iLog_flag==1 ){
		sprintf( acErrMsg,"填写账号待发送短信信息成功!!" );
		WRITEMSG
	}else{
		sprintf( acErrMsg,"未写账号待发送短信信息成功!!" );
		WRITEMSG
	}
	return 0;
ErrExit:
	sprintf( acErrMsg,"填写账号待发送短信信息失败[%s]!!", g_pub_tx.reply );
	WRITEMSG
	return 1;
}



/***************************************************************
* 函 数 名: pub_mob_sendmail_ln
* 功能描述:
* 入口参数：备注，交易代码，账号，增减标志，交易金额，余额，金额2
* 作    者:
* 完成日期：对贷款在结息的时候要传本次结息和累次结息，以及本金，多增减一个金额参数
***************************************************************/

pub_mob_sendmail_ln( char *cRemark_in, char *cTrcode, char *cAcno, char *cAdd_ind, double dTram_in, double dAcbl,double dTram2 )
{
    char    cCompany[5];/**单位**/
    char    cZqfs[7];/**支取方式：现金，转账**/
    char    cMessage[141];
    char    cAcno_tmp[21];
    char    cTmpType[5];
    char		Remark[30];
    char		cAc_Pact[9];
    char		cAc_Pact2[9];
    char	strPeriod[5];

    struct mdm_ac_rel_c            sMdm_ac_rel_c;
    struct dd_mst_c                    sDd_mst_c;
    struct ln_mst_c                sLn_mst;
    struct mob_acct_type_c    sMob_acct_type;
    struct mob_sendmail_c        sMob_sendmail;
    int    iLen;

    double dTram=0.0;
    memset(cTmpType , 0 , sizeof(cTmpType));
    memset(cMessage,0x00,sizeof(cMessage));
    memset(cAcno_tmp,0x00,sizeof(cAcno_tmp));
    memset(cCompany,0x00,sizeof(cCompany));
    memset(cZqfs,0x00,sizeof(cZqfs));
    memset(&sMdm_ac_rel_c,0x00,sizeof(sMdm_ac_rel_c));
    memset(&sDd_mst_c,0x00,sizeof(sDd_mst_c));
    memset(&sLn_mst,0x00,sizeof(sLn_mst));
    memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
    memset(&sMob_sendmail,0x00,sizeof(sMob_sendmail));
    memset(Remark,0x00,sizeof(Remark));
    memset(cAc_Pact,0x00,sizeof(cAc_Pact));/**合同,协议号或账号**/
    memset(cAc_Pact2,0x00,sizeof(cAc_Pact2));
    memset(strPeriod,0x00,sizeof(strPeriod));
    dTram=fabs(dTram_in); /*houyi消除冲正时传入的负金额**/
    memcpy(Remark,cRemark_in,sizeof(Remark)-1);/**houyi防止传入变量的长度不足**/
		
	
    sprintf( acErrMsg,"进入参数 dTram=[%f] remark=[%s],cAcno=[%s] tx_code=[%s] ",dTram,Remark,cAcno,cTrcode);
    WRITEMSG
    sprintf( acErrMsg,"进入参数 cAdd_ind=[%s],dTram_in=[%.2f],dAcbl=[%.2f],dTram2=[%.2f]",cAdd_ind,dTram_in,dAcbl,dTram2);
    WRITEMSG

    iLen=strlen(cAcno);
    cAcno_tmp[0]=cAcno[iLen-4];
    cAcno_tmp[1]=cAcno[iLen-3];
    cAcno_tmp[2]=cAcno[iLen-2];
    cAcno_tmp[3]=cAcno[iLen-1];
    cAcno_tmp[4]=0x00;
/* 
 		g_reply_int = Ln_mst_Sel( g_pub_tx.reply, &sLn_mst, "ac_no = '%s'", cAcno );
		if ( g_reply_int )
		{
			sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错[%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
   
		g_reply_int = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel_c, \
		"ac_no = '%s'", cAcno );
		if ( g_reply_int )
		{
			sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错[%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
 */	 
     /*add by lwb  20150324  根据合同号找还款账号 */
     if(strlen(cAcno)>=15&&memcmp(cAcno ,"621780" , 6)!=0&&memcmp(cAcno,"621223",6)!=0)
     {
		  		g_reply_int = Ln_mst_Sel( g_pub_tx.reply, &sLn_mst, "pact_no  like '%s%%'", cAcno );
				if ( g_reply_int )
				{
					sprintf( acErrMsg, "执行Ln_mst_Sel错[%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				
				g_reply_int = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel_c, \
				"ac_id = %ld",sLn_mst.repay_ac_id );
				if ( g_reply_int )
				{
					sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错[%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
		  
			 	/*g_reply_int = Mob_acct_type_Dec_Sel( g_pub_tx.reply , "ac_no like '%s%%' and mob_sts = '1' ", \
					cAcno);*/
				g_reply_int = Mob_acct_type_Dec_Sel( g_pub_tx.reply , "ac_no = '%s' and mob_sts = '1' and filler = '1'", \
					sMdm_ac_rel_c.ac_no);
		}
		else
		{
			g_reply_int = Mob_acct_type_Dec_Sel( g_pub_tx.reply , "ac_no = '%s' and mob_sts = '1' ", \
			cAcno);	
		}
		/*add end lwb */
		if ( g_reply_int )
		{
			sprintf( acErrMsg,"执行Mob_acct_type_Dec_Sel错[%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		
		vtcp_log("--------[%s]-----------\n",g_pub_tx.sub_tx_code);
	 	while (1)
		{	
			memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
			g_reply_int = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
			if ( g_reply_int == 100 )
			{
				break;
			}
			else if ( g_reply_int )
			{
				sprintf( acErrMsg,"执行Mob_acct_type_Fet_Sel错[%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
 
	    vtcp_log("tx_code=[%s]\n",cTrcode);
			vtcp_log("add_ind=[%c]\n",cAdd_ind[0]);
			vtcp_log("ct_ind=[%c]\n",g_pub_tx.ct_ind[0]);
			vtcp_log("sub_tx_code=[%s]\n",g_pub_tx.sub_tx_code);		
		
      /***贷款到期提前一个月提醒7778***/	
    	if(strcmp(cTrcode,"7770")==0)
    	{
	        sprintf(cMessage,"您****************%s的借款将于%d月%d日到期***************-。"\
	        	,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100);	
	        	strcpy(sMob_sendmail.timeflag, "1" );        	
    	}/***贷款到期提前3天提醒7770***/	
    	else if(strcmp(cTrcode,"7770")==0)
    	{
    	    sprintf(cMessage,"您贷款账号尾号%s的借款将于%d月%d日到期，请及时通过网银或到营业网点办理贷款还本，借款逾期后人行征信系统将显示违约记录。"\
	       ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100);
					strcpy(    sMob_sendmail.timeflag, "1" );
    	}
    	else if(strcmp(cTrcode,"G088")==0 )
    	{
    		if(cAdd_ind[0]=='1')
    		{
					sprintf(cMessage,"贵单位合同号%s的借款于%d月%d日%d时%02d分%s成功，归还金额%.2f元。"\
        	,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,Remark,dTram);
					strcpy(    sMob_sendmail.timeflag, "0" );	
    		}
    		else if(cAdd_ind[0]=='3')
    		{
	         sprintf(cMessage,"您合同号%s的借款已逾期，3日后人行征信系统将显示违约记录，请及时办理还款手续。"\
	        ,cAcno_tmp);
	        strcpy(    sMob_sendmail.timeflag, "1" );
				}
    	}
    	/**每月结息后次日，对未还息户进行短信催缴还息（21日）*/
    	else if(strcmp(cTrcode,"7777")==0)
    	{   
    	    sprintf(cMessage,"贵单位贷款合同号%s的借款欠息%.2f未还，请及时通过网银或到营业网点办理贷款还息，借款欠息后人行征信系统将显示违约记录。"\
	       ,cAcno_tmp,dTram_in);
					strcpy(    sMob_sendmail.timeflag, "1" );
    	}
    	/**每月结息前3日17号，对还息户进行短信通知（17日）*/
    	else if(strcmp(cTrcode,"7776")==0)
    	{	
    	    sprintf(cMessage,"20号为本行贷款结息日，请贵单位及时通过网银或到营业网点办理归还合同号%s的借款利息，借款欠息后人民银行征信系统将显示违约纪录。"\
	       ,cAcno_tmp);
					strcpy(    sMob_sendmail.timeflag, "1" );
    	}
    	/**每月月底前3日27号，对未还息户进行短信通知（27日）*/
    	else if(strcmp(cTrcode,"7775")==0)
    	{		
    	    sprintf(cMessage,"贵单位贷款合同号%s的借款欠息%.2f未还，请及时通过网银或到营业网点办理贷款还息，借款欠息后人行征信系统将显示违约记录。"\
	       ,cAcno_tmp,dTram_in);
					strcpy(    sMob_sendmail.timeflag, "1" );
    	}
    	/***贷款到期提前一个月提醒7778***/	
    	else if(strcmp(cTrcode,"7778")==0)
    	{
    			long note_day;		/* 通知日期 */
    			pub_base_deadlineM(g_pub_tx.tx_date,1,&note_day);
    		  sprintf(cMessage,"贵单位贷款合同号%s的借款将于%d月%d日到期，请及时筹措资金，按期还本付息，借款逾期后人行征信系统将显示违约记录。"\
	        	,cAcno_tmp,(note_day%10000)/100,note_day%100-1);	
	        	strcpy(sMob_sendmail.timeflag, "1" );        	
    	}
    	/***贷款到期提前3天提醒7779***/	
    	else if(strcmp(cTrcode,"7779")==0)
    	{
    			long note_day;		/* 通知日期 */
    			pub_base_deadlineD(g_pub_tx.tx_date,3,&note_day);
	        sprintf(cMessage,"贵单位贷款合同号%s的借款将于%d月%d日到期，请及时通过网银或到营业网点办理还本付息，借款逾期后人行征信系统将显示违约记录。"\
	        	,cAcno_tmp,(note_day%10000)/100,note_day%100-1);	
	        	strcpy(sMob_sendmail.timeflag, "1" );        	
    	}
    	/***贷款转逾期提醒批量程序gD088.c发短信时cTrcode设置成Z089，便于区分***/	
    	else if(strcmp(cTrcode,"Z089")==0)
    	{		
	        sprintf(cMessage,"贵单位贷款合同号%s的借款已逾期，人行征信系统将显示违约记录，请及时办理还款手续。"\
	        	,cAcno_tmp);	
	        	strcpy(sMob_sendmail.timeflag, "1" );        	
    	}
    	/**每月1号电子对账短信通知（1日）*/
    	else if(strcmp(cTrcode,"7774")==0)
    	{										
    	    sprintf(cMessage,"贵单位%s的账户%d月电子对账单已出，请及时网银对账。"\
	       ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100 -1);
					strcpy(sMob_sendmail.timeflag, "1" );
    	}
    	/**每月1号纸质对账短信通知（1日）*/
    	else if(strcmp(cTrcode,"7773")==0)
    	{				
    	    sprintf(cMessage,"贵单位%s的账户%d月纸质对账单已出，请及时到营业网点进行对账。"\
	       ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100 -1);
					strcpy(sMob_sendmail.timeflag, "1" );
    	}
    	else
    	{
        sprintf(cMessage,"贵单位贷款合同号%s的借款于%d月%d日%d时%02d分%s成功，归还金额%.2f元。"\
        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,Remark,dTram);
				strcpy(    sMob_sendmail.timeflag, "0" );			
			}
			
	    strcpy( sMob_sendmail.tx_code, cTrcode );
	    sMob_sendmail.ac_id = sMob_acct_type.ac_id;
	    if(strcmp(sMdm_ac_rel_c.ac_no,"") == 0)
	    {
	    	strcpy( sMob_sendmail.ac_no, sMob_acct_type.ac_no );
	    }
	    else
	    	strcpy( sMob_sendmail.ac_no, sMdm_ac_rel_c.ac_no );
	    strcpy(    sMob_sendmail.tel, g_pub_tx.tel );
	    sMob_sendmail.trace_no = g_pub_tx.trace_no;
	   	sMob_sendmail.tx_date = g_pub_tx.tx_date;
	    sMob_sendmail.tx_time = g_pub_tx.tx_time;
	    strcpy( sMob_sendmail.opt_no, "110" );
			strcpy( sMob_sendmail.mobile, sMob_acct_type.mobile );
	    strcpy( sMob_sendmail.message, cMessage );
	    
	    sMob_sendmail.t_date = 0;
	    sMob_sendmail.t_time = 0;
	    strcpy(    sMob_sendmail.send_level, "1" );
	    sMob_sendmail.sene_times = 1;
	    sMob_sendmail.send_date = 0;
	    sMob_sendmail.send_time = 0;
	    sMob_sendmail.sxf_amt = 0.00;
	    strcpy(    sMob_sendmail.proc_flag, "0" );
	    sMob_sendmail.mobseq = 0;
	    strcpy(sMob_sendmail.tx_type, "0" );
	    sMob_sendmail.tt_no=0;
	    Mob_sendmail_Debug(&sMob_sendmail);
  		g_reply_int = Mob_sendmail_Ins(sMob_sendmail,g_pub_tx.reply );
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]数据库Mob_sendmail插入记录异常[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"D107");
	        goto  ErrExit;
	  	}
	  }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    return (0);
ErrExit:
    return g_reply_int;
}


/***************************************************************
* 函 数 名: pub_mob_sendmail_bat
* 功能描述: 批量收付 写短信表，写入mob_sendmail表，标志timeflag = 0
* 调用程序：在gD008.c spF641.c里面调
* 作    者: 武大为
* 完成日期：20140321
* 参    数：pay_type【单位代理类型】,sUnit_no【单位编号】,cTrcode【交易编号】,cAcno【帐号】,cOpr_attri【收付标志】,dTram【交易金额】,dAcbl【帐号余额】,ys_amt【应收金额】
***************************************************************/
pub_mob_sendmail_bat( char *pay_type, char *sUnit_no, char *cTrcode,  char *cAcno, char *cOpr_attri, double dTram, double dAcbl, double ys_amt)
{
	int ret = 0, g_reply_int = 0, iLen = 0;
	char cMessage[140+1];
	char cAcno_tmp[19+1];
	struct mdm_ac_rel_c sMdm_ac_rel;
  struct mob_sendmail_c sMob_sendmail;
  struct mob_acct_type_c sMob_acct_type;

	memset(cMessage, 0x00, sizeof(cMessage));
	memset(&sMob_sendmail, 0x00, sizeof(sMob_sendmail));
	memset(&sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
	memset( cAcno_tmp, 0x0, sizeof(cAcno_tmp) );
	
	strcpy( cAcno_tmp, cAcno );
	iLen = strlen(cAcno_tmp);
	strcpy(cAcno_tmp,cAcno+iLen-4);
	
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
                "ac_no = '%s'", cAcno );
  if ( ret )
  {
      sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错[%d]", ret );
      WRITEMSG
      goto ErrExit;
  }
	vtcp_log("[%s][%d] 帐号ac_no=[%s],ac_id=[%ld]",__FILE__,__LINE__,sMdm_ac_rel.ac_no,sMdm_ac_rel.ac_id);
	
	g_reply_int = Mob_acct_type_Dec_Sel( g_pub_tx.reply , "ac_no = '%s' and mob_sts = '1' ", \
		cAcno);
	if ( g_reply_int )
	{
		sprintf( acErrMsg,"执行Mob_acct_type_Dec_Sel错[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	
	vtcp_log("--------[%s]-----------\n",g_pub_tx.sub_tx_code);
	while (1)
	{	
			memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
			g_reply_int = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
			if ( g_reply_int == 100 )
			{
				break;
			}
			else if ( g_reply_int )
			{
				sprintf( acErrMsg,"执行Mob_acct_type_Fet_Sel错[%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			
			if(cOpr_attri[0] == '0')
			{
				/* 43 是代收自来水*/
				if(memcmp(pay_type,"43",2)==0)
				{
					if(dAcbl == 0.00 && dTram == 0.00)
					{		/* 您尾号#1#于#2#余额为0，无法自动代扣燃气费，请您尽快到晋中银行网点缴费 */
							sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分余额0.00元，无法自动代扣自来水费，请您尽快到晋中银行网点缴费。"\
							        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);
					}
					else if(ys_amt > dAcbl )
					{		/*  您尾号%s的账户于#2#代扣燃气费#3#.余额#4#，还欠燃气费金额#8#，请您尽快到晋中银行网点缴费   */
							sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分应扣自来水费%.2f元，余额%.2f元，余额不足无法完成代扣，请您尽快到晋中银行网点缴费。"\
						        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,ys_amt,dAcbl);
				    }else{/*  您尾号%s于%d月%d日%d时%02d分代扣燃气费%.2f元.余额%.2f元.#6#    */
						  sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分代扣自来水费%.2f元，余额%.2f元。"\
							        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,dTram,dAcbl);
					}
				}
				/* 44 是代收燃气*/
				else if(memcmp(pay_type,"44",2)==0)
				{
					if(dAcbl == 0.00 && dTram == 0.00)
					{		/* 您尾号#1#于#2#余额为0，无法自动代扣燃气费，请您尽快到晋中银行网点缴费 */
							sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分余额0.00元，无法自动代扣燃气费，请您尽快到晋中银行网点缴费。"\
							        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);
					}
					else if(ys_amt > dTram && dTram != 0.00)
					{		/*  您尾号%s的账户于#2#代扣燃气费#3#.余额#4#，还欠燃气费金额#8#，请您尽快到晋中银行网点缴费   */
							sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分代扣燃气费%.2f元，余额%.2f元，还欠燃气费金额%.2f元，请您尽快到晋中银行网点缴费。"\
						        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,dTram,dAcbl,ys_amt-dTram);
				  }else{/*  您尾号%s于%d月%d日%d时%02d分代扣燃气费%.2f元.余额%.2f元.#6#    */
						  sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分代扣燃气费%.2f元，余额%.2f元。"\
							        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,dTram,dAcbl);
					}
				}
				/* 51 是瑞阳供热  吴海帆 20150729*/
				else if(memcmp(pay_type,"51",2)==0)
				{	
					if(dAcbl == 0.00 && dTram == 0.00)
					{		/* 您尾号#1#于#2#余额为0，无法自动代扣瑞阳供热费用，请您尽快到晋中银行网点缴费 */
							sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分余额0.00元，无法自动代扣瑞阳供热费用，请您尽快到晋中银行网点缴费。"\
							        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);
					}
					else if(ys_amt > dAcbl && dAcbl != 0.00)
					{		/*  您尾号%s的账户于#2#代扣瑞阳供热费用#3#.余额#4#，可用余额不足，无法自动代扣瑞阳供热费用，请您尽快到晋中银行网点缴费   */
							sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分应代扣瑞阳供热费用%.2f元,余额%.2f元,无法自动代扣瑞阳供热费用,请您尽快到晋中银行网点缴费。"\
						        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,ys_amt,dAcbl);
				  	}
				  	else
				  	{/*  您尾号%s于%d月%d日%d时%02d分代扣瑞阳供热费用%.2f元.余额%.2f元.#6#    */
						  sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分代扣瑞阳供热费用%.2f元,余额%.2f元;供热卡用户,请持卡到瑞阳营业厅补写卡信息"\
							        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,dTram,dAcbl);
					}
				}
				/* 49 是代收理财费用*/
				else if(memcmp(pay_type,"49",2)==0)
				{ 
					sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分代扣理财费用%.2f元，余额%.2f元。"\
				        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,dTram,dAcbl);
				}
				/* 50 是代收幼儿园费用*/
				else if(memcmp(pay_type,"50",2)==0)
				{ /*您尾号#1#于#2#代扣幼儿园费用#3#,余额#4# */
					sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分代扣幼儿园费用%.2f元，余额%.2f元。"\
				        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,dTram,dAcbl);
				}
				else
				{
					sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分代扣费成功，扣费金额%.2f元，余额%.2f元。"\
				        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,dTram,dAcbl);
				}
			}
			else if(cOpr_attri[0] == '1')
			{  /* 您尾号#1#于#2#代发工资#3#.余额#4#  */
				if(memcmp(pay_type,"30",2)==0)
				{ 
						sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分代发工资%.2f元，余额%.2f元。"\
					        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,dTram,dAcbl);
			  }else if(memcmp(pay_type,"36",2)==0)
				{ 
						sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分代发收益%.2f元，余额%.2f元。"\
					        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,dTram,dAcbl);
			  }
			  else
				{ 
						sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分代发%.2f元，余额%.2f元。"\
					        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,dTram,dAcbl);
			  }
			}
			else if(cOpr_attri[0] == 'Q')  /*亲自关联余额不足的短信也放这发 无奈啊 wudawei 20151127*/
			{
				if(memcmp(pay_type,"1",1)==0)/*父母卡*/
				{
						sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分%s，亲子关联转出失败，请您及时查看。"\
					        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,sUnit_no);
				}
				else if(memcmp(pay_type,"2",1)==0)/*金太阳卡*/
				{
						sprintf(cMessage,"您尾号%s于%d月%d日%d时%02d分%s，亲子关联转入失败，请您及时查看。"\
					        ,cAcno_tmp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,sUnit_no);
				}
			}
			
			strcpy( sMob_sendmail.tx_code, cTrcode );
			strcpy( sMob_sendmail.ac_no, cAcno );
			sMob_sendmail.ac_id = sMdm_ac_rel.ac_id;
			strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
			sMob_sendmail.trace_no = g_pub_tx.trace_no;
			sMob_sendmail.tx_date = g_pub_tx.tx_date;
			sMob_sendmail.tx_time = g_pub_tx.tx_time;
			strcpy( sMob_sendmail.opt_no, "110" );
			strcpy( sMob_sendmail.mobile, sMob_acct_type.mobile );
			strcpy( sMob_sendmail.message, cMessage );
			sMob_sendmail.tt_no = 0;
			sMob_sendmail.t_date = 0;
			sMob_sendmail.t_time = 0;
			strcpy(	sMob_sendmail.send_level, "1" );
			sMob_sendmail.sene_times = 1;
			sMob_sendmail.send_date = 0;
			sMob_sendmail.send_time = 0;
			sMob_sendmail.sxf_amt = 0.00;
			strcpy(	sMob_sendmail.proc_flag, "0" );
			sMob_sendmail.mobseq = 0;
			strcpy(sMob_sendmail.tx_type, "0" );
			strcpy(sMob_sendmail.timeflag, "0" );	
			Mob_sendmail_Debug(&sMob_sendmail);
		  ret = Mob_sendmail_Ins(sMob_sendmail,g_pub_tx.reply );
			if ( ret != 0 ) 
			{
				sprintf( acErrMsg, "写mob_sendmail表失败!!![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf( acErrMsg,"待发送短信信息成功!!" );
	WRITEMSG
	return 0;
ErrExit:
	sprintf( acErrMsg,"待发送短信信息失败[%s]!!", g_pub_tx.reply );
	WRITEMSG
	return 1;
}



/*************************************************
* 文 件 名:  sp6687.c
* 功能描述： 借据打印
*
* 作    者:  gongliangliang
* 完成日期： 20110824
*
* 修改记录：
* 日    期:
* 修 改 人: 
* 修改内容:
* 说明：tx_code='4400' sub_tx_code='4400'
        insert into tx_def values ('4400', '信贷借款借据打印', '10000000001000000000000011010110000000111101100001000000000000000000000000000000000000001000000010000000000000000000000000000000', '0', '4', '0');
        insert into tx_flow_def values('4400','0','4400','#$');
        insert into tx_sub_def values ('4400', '信贷贷款信息查询', 'sp6687', '0', '0');
        输入：0300域获得借据号 
        输出：0250客户名称 0970月利率 0400借款金额 0440开始日期 0450还款日期 0890产品种类 0260产品名称 0280客户号 0310 合同号 0500 合同笔数 0410 合同金额
*************************************************/
#define EXTERN
#include "public.h"
#include "ln_auth_c.h"
#include "ln_parm_c.h"
#include "pact_gaga_rel_c.h"
int sp6687()
{
	int iRet=0;
	long iAmt=0;
	char tempstr[17];
	char tmpstr[4];
	char cAmt1[51];
	char cSum_bal[17];
	char cAmt2[26];
	char cDate1[17];
	char cDate2[17];
	char cRate[9];
	char cCif_no[10];
	double dSum_gaga_bal=0.00;
	int  iJs=0;
	long lSum_bal=0;
	char filename[64];
	char cDzy_name[201];
	FILE *fp = NULL;
	char vstr1[64],vstr[65];
	struct ln_auth_c sLn_auth;
	struct ln_parm_c sLn_parm;
	struct pact_gaga_rel_c sPact_gaga_rel;

	memset(vstr1,0x00,sizeof(vstr1));
	memset(vstr,0x00,sizeof(vstr));
	memset(tempstr,0x00,sizeof(tempstr));
	memset(cSum_bal,0x00,sizeof(cSum_bal));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(&g_pub_tx,0,sizeof(g_pub_tx));
	memset(&g_pub_tx,0,sizeof(g_pub_tx));
	memset(&sLn_auth,0,sizeof(sLn_auth));
	memset(&sPact_gaga_rel,0,sizeof(sPact_gaga_rel));
	memset(cDzy_name,0,sizeof(cDzy_name));
	
	memset( filename, 0x0, sizeof(filename) );
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		strcpy( g_pub_tx.reply, "P159" );
		sprintf( acErrMsg,"打开写存折信息文件错!!" );
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	
	get_zd_data("0300",sLn_auth.pact_no);
	
	iRet = Ln_auth_Sel(g_pub_tx.reply,&sLn_auth," pact_no='%s' and sts='X' ",sLn_auth.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "查询介质关系表出错!pact_no[%s]ret=[%d]" ,sLn_auth.pact_no ,iRet);
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	
	iRet = Ln_parm_Sel(g_pub_tx.reply,&sLn_parm," prdt_no='%s' ",sLn_auth.prdt_no);
	if(iRet)
	{
		sprintf( acErrMsg, "贷款产品参数表出错!pact_no[%s]ret=[%d]" ,sLn_auth.pact_no ,iRet);
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}

	memcpy(tempstr,sLn_auth.pact_no,16);
	memcpy(tmpstr,sLn_auth.pact_no+16,3);
	
	/*对打印金额的处理*/
	numtomoney(sLn_auth.amt, vstr1);	/* 大写金额 */
	vtcp_log("[%s][%d] vstr1===[%s]\n",__FILE__,__LINE__,vstr1);
	
	sprintf( cAmt1, "%.0f" ,sLn_auth.amt*100);
	vtcp_log("[%s][%d] cAmt1===[%s]\n",__FILE__,__LINE__,cAmt1);
	cLongToSmall(cAmt1,cAmt1);
	iAmt=(long)sLn_auth.intst_amt;/*对合同总金额的处理*/
	/**使用long型造成溢出，将iAmt*100改为iAmt by jk 20120311 begin
	if(iAmt/1000000<1)
	by jk20120311 end**/
	if(iAmt/10000<1)
	{
		sprintf(cAmt2,"%.2f元",sLn_auth.intst_amt);
	}else{vtcp_log("[%s][%d] jkjkr2===[%ld]\n",__FILE__,__LINE__,iAmt);
		/**使用long型造成溢出，将iAmt*100改为iAmt by jk 20120311 begin
		sprintf(cAmt2,"%ld.%ld%ld%ld万元",iAmt/1000000,iAmt%1000000/10000,iAmt%10000/100,iAmt%100);
		by jk20120311 end **/
		sprintf(cAmt2,"%ld.%ld%ld%ld万元",iAmt/10000,iAmt%10000/1000,iAmt%1000/100,iAmt%100/10);
	}
vtcp_log("[%s][%d] jkjk3===[%s]\n",__FILE__,__LINE__,cAmt2);
	
	/*对日期的处理*/
	sprintf( cDate1 , "%4d    %2d    %2d  " , sLn_auth.beg_date/10000 , sLn_auth.beg_date%10000/100 , sLn_auth.beg_date%100 );
	sprintf( cDate2 , "%4d    %2d    %2d  " , sLn_auth.mtr_date/10000 , sLn_auth.mtr_date%10000/100 , sLn_auth.mtr_date%100 );	
	
	/*对利率的处理*/
	sprintf( cRate, "%.5f" ,sLn_auth.rate);
	
	sprintf(cCif_no,"%ld",sLn_auth.cif_no);
	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	if(atoi(tmpstr)==1)
	{
		iRet = Pact_gaga_rel_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", tempstr );
		if ( iRet )
		{
			sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", iRet );
			WRITEMSG
			set_zd_data(DC_GET_MSG, "执行数据库更新出错");
			goto ErrExit;
		}
		
		while(1)
		{
			memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
			iRet = Pact_gaga_rel_Fet_Upd( &sPact_gaga_rel, g_pub_tx.reply );
			if( iRet && iRet != 100 )
			{
				sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", iRet );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"执行数据库更新出错");
				goto ErrExit;
			}else if (iRet==100)
			{
				break;
			}
			if(iJs>0)
			{
				strcat(cDzy_name,"，");
			}
			pub_base_strpack(sPact_gaga_rel.name);
			strcat(cDzy_name,sPact_gaga_rel.name);
			dSum_gaga_bal+=sPact_gaga_rel.gaga_bal;
			iJs++;
		}
		Pact_gaga_rel_Clo_Upd();
		/* 会溢出 20131213 wudawei
		lSum_bal=(long)dSum_gaga_bal*100;
		if(lSum_bal/1000000<1)
		{
			sprintf(cSum_bal,"%.2f元",dSum_gaga_bal);
		}else{
			sprintf(cSum_bal,"%ld.%ld%ld%ld万元",lSum_bal/1000000,lSum_bal%1000000/10000,lSum_bal%10000/100,lSum_bal%100);
		}
		*/
		lSum_bal=(long)dSum_gaga_bal;
		if(lSum_bal/10000<1)
		{
			sprintf(cSum_bal,"%.2f元",dSum_gaga_bal);
		}else{
			sprintf(cSum_bal,"%ld.%ld万元",lSum_bal/10000,lSum_bal%10000/100);
		}
		vtcp_log("[%s][%d] cDzy_name===[%s]\n",__FILE__,__LINE__,cDzy_name);
		vtcp_log("[%s][%d] cSum_bal===[%s]\n",__FILE__,__LINE__,cSum_bal);
	}
	/*借款单位名称|月利率|借款金额（大写）|借款金额（小写）|合同金额|开始日期|结束日期|种类|用途|客户号|合同号|笔数|抵质押描述|抵质押金额|担保客户号|*/
	fprintf( fp, "JKJJ%s|%s|%s|%28.28s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
			sLn_auth.name, cRate, vstr1, cAmt1, cAmt2, cDate1, cDate2, sLn_auth.filler1, sLn_auth.filler2, cCif_no, tempstr, tmpstr,cDzy_name,cSum_bal,sLn_auth.bail_cif_no);

	vtcp_log("[%s][%d] tempstr===[%s]\n",__FILE__,__LINE__,tempstr);
	
	fclose( fp );
	set_zd_data( DC_FILE_SND, "1" );
OKExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
/** add by chenchao 20110915 为适应借据的小格打印 **/
int cLongToSmall(char * cIn_num,char * cOut_num)
{
	char cIn_amt[101];
	char cOut_amt[101];
	char cTemp_amt[101];
	int  iTemp=0;
	int  iCc=0;
	memset(cIn_amt,0x00,sizeof(cIn_amt));
	memset(cOut_amt,0x00,sizeof(cOut_amt));
	memset(cTemp_amt,0x00,sizeof(cTemp_amt));
	memcpy(cIn_amt,cIn_num,101);
	pub_base_strpack(cIn_amt);
	for(iTemp=0;iTemp<=strlen(cIn_amt);iTemp++)
	{
		cOut_amt[iCc*5]=cIn_amt[iTemp];
		cOut_amt[iCc*5+1]=' ';
		cOut_amt[iCc*5+2]=' ';
		cOut_amt[iCc*5+3]=cIn_amt[++iTemp];
		cOut_amt[iCc*5+4]=' ';
		iCc++;
	}
	cOut_num[strlen(cIn_amt)*3+1]='\0';
	memcpy(cTemp_amt,"￥",2);
	for(iTemp=0;iTemp<=strlen(cOut_amt);iTemp++)
	{
		cTemp_amt[iTemp+2]=cOut_amt[iTemp];
	}
	memcpy(cOut_num,cTemp_amt,sizeof(cOut_amt));
	return 0;
}

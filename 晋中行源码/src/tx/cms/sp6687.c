/*************************************************
* �� �� ��:  sp6687.c
* ���������� ��ݴ�ӡ
*
* ��    ��:  gongliangliang
* ������ڣ� 20110824
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��: 
* �޸�����:
* ˵����tx_code='4400' sub_tx_code='4400'
        insert into tx_def values ('4400', '�Ŵ�����ݴ�ӡ', '10000000001000000000000011010110000000111101100001000000000000000000000000000000000000001000000010000000000000000000000000000000', '0', '4', '0');
        insert into tx_flow_def values('4400','0','4400','#$');
        insert into tx_sub_def values ('4400', '�Ŵ�������Ϣ��ѯ', 'sp6687', '0', '0');
        ���룺0300���ý�ݺ� 
        �����0250�ͻ����� 0970������ 0400����� 0440��ʼ���� 0450�������� 0890��Ʒ���� 0260��Ʒ���� 0280�ͻ��� 0310 ��ͬ�� 0500 ��ͬ���� 0410 ��ͬ���
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
		sprintf( acErrMsg,"��д������Ϣ�ļ���!!" );
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	
	get_zd_data("0300",sLn_auth.pact_no);
	
	iRet = Ln_auth_Sel(g_pub_tx.reply,&sLn_auth," pact_no='%s' and sts='X' ",sLn_auth.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "��ѯ���ʹ�ϵ�����!pact_no[%s]ret=[%d]" ,sLn_auth.pact_no ,iRet);
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	
	iRet = Ln_parm_Sel(g_pub_tx.reply,&sLn_parm," prdt_no='%s' ",sLn_auth.prdt_no);
	if(iRet)
	{
		sprintf( acErrMsg, "�����Ʒ���������!pact_no[%s]ret=[%d]" ,sLn_auth.pact_no ,iRet);
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}

	memcpy(tempstr,sLn_auth.pact_no,16);
	memcpy(tmpstr,sLn_auth.pact_no+16,3);
	
	/*�Դ�ӡ���Ĵ���*/
	numtomoney(sLn_auth.amt, vstr1);	/* ��д��� */
	vtcp_log("[%s][%d] vstr1===[%s]\n",__FILE__,__LINE__,vstr1);
	
	sprintf( cAmt1, "%.0f" ,sLn_auth.amt*100);
	vtcp_log("[%s][%d] cAmt1===[%s]\n",__FILE__,__LINE__,cAmt1);
	cLongToSmall(cAmt1,cAmt1);
	iAmt=(long)sLn_auth.intst_amt;/*�Ժ�ͬ�ܽ��Ĵ���*/
	/**ʹ��long������������iAmt*100��ΪiAmt by jk 20120311 begin
	if(iAmt/1000000<1)
	by jk20120311 end**/
	if(iAmt/10000<1)
	{
		sprintf(cAmt2,"%.2fԪ",sLn_auth.intst_amt);
	}else{vtcp_log("[%s][%d] jkjkr2===[%ld]\n",__FILE__,__LINE__,iAmt);
		/**ʹ��long������������iAmt*100��ΪiAmt by jk 20120311 begin
		sprintf(cAmt2,"%ld.%ld%ld%ld��Ԫ",iAmt/1000000,iAmt%1000000/10000,iAmt%10000/100,iAmt%100);
		by jk20120311 end **/
		sprintf(cAmt2,"%ld.%ld%ld%ld��Ԫ",iAmt/10000,iAmt%10000/1000,iAmt%1000/100,iAmt%100/10);
	}
vtcp_log("[%s][%d] jkjk3===[%s]\n",__FILE__,__LINE__,cAmt2);
	
	/*�����ڵĴ���*/
	sprintf( cDate1 , "%4d    %2d    %2d  " , sLn_auth.beg_date/10000 , sLn_auth.beg_date%10000/100 , sLn_auth.beg_date%100 );
	sprintf( cDate2 , "%4d    %2d    %2d  " , sLn_auth.mtr_date/10000 , sLn_auth.mtr_date%10000/100 , sLn_auth.mtr_date%100 );	
	
	/*�����ʵĴ���*/
	sprintf( cRate, "%.5f" ,sLn_auth.rate);
	
	sprintf(cCif_no,"%ld",sLn_auth.cif_no);
	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	if(atoi(tmpstr)==1)
	{
		iRet = Pact_gaga_rel_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", tempstr );
		if ( iRet )
		{
			sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", iRet );
			WRITEMSG
			set_zd_data(DC_GET_MSG, "ִ�����ݿ���³���");
			goto ErrExit;
		}
		
		while(1)
		{
			memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
			iRet = Pact_gaga_rel_Fet_Upd( &sPact_gaga_rel, g_pub_tx.reply );
			if( iRet && iRet != 100 )
			{
				sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", iRet );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"ִ�����ݿ���³���");
				goto ErrExit;
			}else if (iRet==100)
			{
				break;
			}
			if(iJs>0)
			{
				strcat(cDzy_name,"��");
			}
			pub_base_strpack(sPact_gaga_rel.name);
			strcat(cDzy_name,sPact_gaga_rel.name);
			dSum_gaga_bal+=sPact_gaga_rel.gaga_bal;
			iJs++;
		}
		Pact_gaga_rel_Clo_Upd();
		/* ����� 20131213 wudawei
		lSum_bal=(long)dSum_gaga_bal*100;
		if(lSum_bal/1000000<1)
		{
			sprintf(cSum_bal,"%.2fԪ",dSum_gaga_bal);
		}else{
			sprintf(cSum_bal,"%ld.%ld%ld%ld��Ԫ",lSum_bal/1000000,lSum_bal%1000000/10000,lSum_bal%10000/100,lSum_bal%100);
		}
		*/
		lSum_bal=(long)dSum_gaga_bal;
		if(lSum_bal/10000<1)
		{
			sprintf(cSum_bal,"%.2fԪ",dSum_gaga_bal);
		}else{
			sprintf(cSum_bal,"%ld.%ld��Ԫ",lSum_bal/10000,lSum_bal%10000/100);
		}
		vtcp_log("[%s][%d] cDzy_name===[%s]\n",__FILE__,__LINE__,cDzy_name);
		vtcp_log("[%s][%d] cSum_bal===[%s]\n",__FILE__,__LINE__,cSum_bal);
	}
	/*��λ����|������|������д��|����Сд��|��ͬ���|��ʼ����|��������|����|��;|�ͻ���|��ͬ��|����|����Ѻ����|����Ѻ���|�����ͻ���|*/
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
/** add by chenchao 20110915 Ϊ��Ӧ��ݵ�С���ӡ **/
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
	memcpy(cTemp_amt,"��",2);
	for(iTemp=0;iTemp<=strlen(cOut_amt);iTemp++)
	{
		cTemp_amt[iTemp+2]=cOut_amt[iTemp];
	}
	memcpy(cOut_num,cTemp_amt,sizeof(cOut_amt));
	return 0;
}

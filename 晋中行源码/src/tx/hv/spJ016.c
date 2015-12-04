/***************************************************************
* �� �� ��:     spJ016.c
* ����������    ��Ʊ��/��Ѻ 
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-26  18:56
* ����:       ���ݼ���Ҫ������һ����Ѻ�������ж�
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hvnet.h"
#include "hv_pub.h"
#include "find_debug.h"

/**********************************
*��ʧ���ڷŵ�ԭί�������ֶ�       *
*��Ʊ���� ���ļ�����			  *
*���������һ����ͨѶ             *
***********************************/
struct HP_PASSWD{
	char  type	[1+1];	/*�������� 0:��Ѻ 1:��Ѻ*/
	char po_type	[1+1];	/*1.�ֽ��Ʊ 2.��ת�û�Ʊ 3.����ת�û�Ʊ 4.�鸴��ҵ��*/
	char sign_date	[8+1];	/*��Ʊǩ������*/
	char po_no	[8+1];	/*��Ʊ����*/
	char tx_amt	[15+1];	/*���,�Է�Ϊ��λ,����15λǰ��0*/
	char pay_brno	[12+1];	/*ǩ�����к�*/
	char cash_brno	[12+1];	/*�Ҹ����к�,������ȫ0*/
	char cr_txnum	[8+1];	/*ǰ8λΪ�鸴����,����ǰ��0,����ȫ��0,�ǲ鸴��ҵ����ȫ0*/
	char pwd	[10+1];	/*��Ѻʱ���صõ�����Ѻ,��ѺʱΪҪ��֤�Ļ�Ʊ��Ѻ*/
}hp_passwd;
spJ016()
{
	int ret	= 0;
	HV_FD123_AREA	   fd123;
	char cOtype	[2];
	double dTxamt = 0.0;
	char cHptype;
	char cOtherInfo[61];
	char cQry_num[9];
	vtcp_log("[%s][%d]��ʼ��Ʊ��Ѻ/��Ѻ����!\n",__FILE__,__LINE__);
	
	memset(&hp_passwd,0x00,sizeof(hp_passwd));
	memset(&fd123,0x00,sizeof(fd123));
	memset(cOtype,0x00,sizeof(cOtype));
	memset(cOtherInfo, 0,sizeof(cOtherInfo));
	memset(cQry_num,0x00,sizeof(cQry_num));
	memcpy(cQry_num,"00000000",8);
 	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
   	 /*****************************
     	 *PART 1   �õ�ǰ̨����      *
     	 *****************************/
	
	get_fd_data("1230",(char *)&fd123);/**��ȡ123����������**/
	vtcp_log("[%s][%d]fd123===[%s]\n",__FILE__,__LINE__,(char *)&fd123);
	get_zd_data("0720",hp_passwd.type);   /*��������**/
	get_zd_data("0720",cOtype);   /*��������**/
	vtcp_log("%s,%d, optype===[%c] ",__FILE__,__LINE__,cOtype[0]);
	
	hp_passwd.po_type[0] = fd123.po_type[0];
	get_zd_data("0460",hp_passwd.sign_date);
	MEMCPY_DEBUG(hp_passwd.po_no,fd123.po_no,sizeof(fd123.po_no));
	get_fd_data("0390",hp_passwd.tx_amt);
	vtcp_log("[%s][%d]hp_passwd.tx_amt===[%s]\n",__FILE__,__LINE__,hp_passwd.tx_amt);
	str2dbl(hp_passwd.tx_amt,strlen(hp_passwd.tx_amt),0,&dTxamt);
	vtcp_log("[%s][%d]dTxmat===[%lf]\n",__FILE__,__LINE__,dTxamt);
	sprintf(hp_passwd.tx_amt,"%015.0f",dTxamt*100);
	MEMCPY_DEBUG(hp_passwd.pay_brno,fd123.pay_opn_br_no,sizeof(fd123.pay_opn_br_no));
	
	/**��Ѻ/��Ѻ�ֶ�***/
	if(hp_passwd.type[0]=='1')/**��Ѻ**/
	{
		get_zd_data("0880",hp_passwd.pwd);
	}
	
	/**��Ʊ����***/
	if(hp_passwd.po_type[0]=='1' || hp_passwd.po_type[0]=='5')/**�ֽ��Ʊ ����ƾ֤ʱ.Ҳ�ô˱���**/
	{
		MEMCPY_DEBUG(hp_passwd.cash_brno,fd123.cash_opn_br_no,sizeof(fd123.cash_opn_br_no));
	}
	else 
	{
		memset(hp_passwd.cash_brno,'0',12);
	}
	/**�Ƿ�鸴��***/
	if(hp_passwd.po_type[0]=='4')/**�鸴��**/
	{
		MEMCPY_DEBUG(hp_passwd.cr_txnum,fd123.cr_tx_num,sizeof(fd123.cr_tx_num));
	}
	else 
		memset(hp_passwd.cr_txnum,'0',sizeof(hp_passwd.cr_txnum)-1);
	 
	
	vtcp_log("[%s][%d]��ȡ����ǰ̨����\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]hp_passwd===[%s]\n",__FILE__,__LINE__,(char *)&hp_passwd);
	

	/**********************
         *PART 2   �������   *
	 **********************/
	  
	ret = SetParamAll(HP_SERVERNAME1,HP_SERVERNAME2,TIME_OUT);
	if(ret)
	{
	 	sprintf(acErrMsg,"���������������!\n");
		set_zd_data("0130",acErrMsg);
	 	WRITEMSG 
	 	strcpy(g_pub_tx.reply,"P013");
	 	goto ErrExit;
	}
	vtcp_log("%s,%d ��������=[%d]",__FILE__,__LINE__,cOtype[0]-'0');
	vtcp_log("%s,%d signdate=[%s]",__FILE__,__LINE__,hp_passwd.sign_date);
	vtcp_log("%s,%d pono=[%s]",__FILE__,__LINE__,hp_passwd.po_no);
	vtcp_log("%s,%d amt=[%s]",__FILE__,__LINE__,hp_passwd.tx_amt);
	vtcp_log("%s,%d paybrno=[%s]",__FILE__,__LINE__,hp_passwd.pay_brno);
	vtcp_log("%s,%d cashbrno=[%s]",__FILE__,__LINE__,hp_passwd.cash_brno);
	vtcp_log("%s,%d cr_txnum=[%s]",__FILE__,__LINE__,hp_passwd.cr_txnum);
	/*vtcp_log("%s,%d pwd=[%s]",__FILE__,__LINE__,hp_passwd.pwd);*/
	vtcp_log("%s,%d po_type=[%c]",__FILE__,__LINE__,hp_passwd.po_type[0]);
	if(hp_passwd.po_type[0]=='4') cHptype=4;
	if(hp_passwd.po_type[0]=='1') cHptype=1;
	if(hp_passwd.po_type[0]=='2') cHptype=3;
	if(hp_passwd.po_type[0]=='3') cHptype=2;
	if(hp_passwd.po_type[0]=='5') cHptype=5;/**add by zyl 20110705 ����ƾ֤**/
	vtcp_log("%s,%d hptype=[%d]",__FILE__,__LINE__,cHptype);


	memset(cOtherInfo, 0 , sizeof(cOtherInfo));
	memcpy(cQry_num,hp_passwd.cr_txnum,8);
    cOtherInfo[0]=((cQry_num[0]&0x0F)<<4)+(cQry_num[1]&0x0F);
    cOtherInfo[1]=((cQry_num[2]&0x0F)<<4)+(cQry_num[3]&0x0F);
    cOtherInfo[2]=((cQry_num[4]&0x0F)<<4)+(cQry_num[5]&0x0F);
    cOtherInfo[3]=((cQry_num[6]&0x0F)<<4)+(cQry_num[7]&0x0F);

    vtcp_log("[%s][%d]cOtherInfo=[%x]",__FILE__,__LINE__,cOtherInfo[0]);
    vtcp_log("[%x]",cOtherInfo[1]);
    vtcp_log("[%x]",cOtherInfo[2]);
    vtcp_log("[%x]\n",cOtherInfo[3]);

	vtcp_log("[%s][%d] ���˵�cOtherInfo===[%s]\n",__FILE__,__LINE__,&cOtherInfo[0]);
	/****add by liuyue 20070209****/
	memset(cOtherInfo, 0 , sizeof(cOtherInfo));
	cOtherInfo[0]=(char) (  (cQry_num[0]-'0')*16+(cQry_num[1]-'0') );
	cOtherInfo[1]=(char) (  (cQry_num[2]-'0')*16+(cQry_num[3]-'0') );
	cOtherInfo[2]=(char) (  (cQry_num[4]-'0')*16+(cQry_num[5]-'0') );
	cOtherInfo[3]=(char) (  (cQry_num[6]-'0')*16+(cQry_num[7]-'0') );
	/**************add end************/
    vtcp_log("[%s][%d]cOtherInfo=[%x]",__FILE__,__LINE__,cOtherInfo[0]);
    vtcp_log("[%x]",cOtherInfo[1]);
    vtcp_log("[%x]",cOtherInfo[2]);
    vtcp_log("[%x]\n",cOtherInfo[3]);
	vtcp_log("[%s][%d]cOtherInfo===[%s]\n",__FILE__,__LINE__,&cOtherInfo[0]);
		
	ret = DraftEncrypt(cOtype[0]-'0',cHptype,hp_passwd.sign_date,
		hp_passwd.po_no,hp_passwd.tx_amt,hp_passwd.pay_brno,hp_passwd.cash_brno,
		cOtherInfo,hp_passwd.pwd);
	if(ret)
	{
		if(ret==9001)
			sprintf(acErrMsg,"������Ѻ�����������쳣[%d]",ret);
		if(ret==9002)
			sprintf(acErrMsg,"��������Ѻ�������쳣[%d]",ret);
		if(ret==9003)
			sprintf(acErrMsg,"������Ѻ��������Ϣ�쳣[%d]",ret);
		if(ret==9004)
			sprintf(acErrMsg,"��Ѻ����[%d]",ret);
		if(ret==9005)
			sprintf(acErrMsg,"��Ѻ����[%d]",ret);
		if(ret==9006)
			sprintf(acErrMsg,"�˶�ǩ������[%d]",ret);
		if(ret==9007)
			sprintf(acErrMsg,"�����б�ʶ����[%d]",ret);
		if(ret==9008)
			sprintf(acErrMsg,"�ձ��б�ʶ����[%d]",ret);
		if(ret==9009)
			sprintf(acErrMsg,"���ܿ������쳣[%d]",ret);
		if(ret==9010)
			sprintf(acErrMsg,"���ݱ��Ĵ���[%d]",ret);
		if(ret==9011)
			sprintf(acErrMsg,"����ͨ�ų�ʱ[%d]",ret);
		if(ret==9012)
			sprintf(acErrMsg,"�������ޱ�ѺȨ��[%d]",ret);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT08");
		goto ErrExit;
	}
	/*vtcp_log("_______cPwd____[%s]_____\n",hp_passwd.pwd);*/
	
	

	/*********************
	 *PART 3  �������ݿ� *
         *********************/
	
	 

	/*******************
         *PART 4  ��β���� *
         *******************/	
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	/*** ������� ***/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	if(hp_passwd.type[0] =='0')
		set_zd_data("0880",hp_passwd.pwd);/***���ɵ���Ѻ**/
	else 
		set_zd_data("0880","�ɹ�!");/***���سɹ�***/
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

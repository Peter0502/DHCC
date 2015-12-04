/*******************************************
*��������: gDZP1.c
*������: ����ֽƱǰ��ϵͳ��Ҫ����������ļ�
*��    ��: chengbo
*���ʱ��: 20100531
*�޸ļ�¼:
*    1���� �� ��:SSH
*    2���޸�ʱ��:2010.6.18
*    3���޸�����:�������������
insert into tx_def values('9576','ֽƱ��Ϣ����','10000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0','3','');
insert into tx_flow_def values('9576','0','9576','#$');
insert into tx_sub_def values('9576','ֽƱ��Ϣ����','sp9576','0','0'); 
*******************************************/
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "mo_discnt_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "hv_brno_orno_c.h"

#define EXTERN
#define ERR_DEAL if(ret){\
       sprintf(acErrMsg,"[%s],[%d],sqlcode=[%d]",__FILE__,__LINE__,ret);\
       WRITEMSG \
       goto ErrExit;\
}
int nGetBrno12AndName(char *brno5,char *brno12,char *name){
	int  iRc;
	struct hv_brno_orno_c st;
	memset(&st,'\0',sizeof(st));
	strncpy(st.br_no,brno5,sizeof(st.br_no));
	iRc=Hv_brno_orno_Sel(g_pub_tx.reply,&st,"br_no='%s'",st.br_no);	
	if(iRc){
		vGet_Hv_Qs_no(st.or_br_no);
		iRc=Hv_brno_orno_Sel(g_pub_tx.reply,&st,"or_br_no='%s'",st.or_br_no);	
	}
	if(iRc){
		vtcp_log("����hv_brno_orno����[%d]",iRc);
		return(-1);
	}
	strcpy(brno12,st.or_br_no);
	strcpy(name,st.or_br_name);
	return(0);
}
sp9576()
{
	int   ret;
	FILE  *fp;
	char  cLine[1024];
	char  filename[100];
	char  cAmt[19];
	char  cDateTmp[11];
	char  cTmp[61];
	long  lRegDate=0;
	struct com_sys_parm_c    sCom_sys_parm;
	struct mo_discnt_c       sMo_discnt;
	struct mo_bank_acc_po_c  sMo_bank_acc_po;
	struct mo_bank_po_c      sMo_bank_po;
	char  brno12[13];
	char  name60[61];
	memset(brno12,           0x00,sizeof(brno12));
	memset(name60,           0x00,sizeof(name60));
	memset(cLine,           0x00,sizeof(cLine));
	memset(filename,        0x00,sizeof(filename));
	memset(cAmt,            0x00,sizeof(cAmt));
	memset(cDateTmp,        0x00,sizeof(cDateTmp));
	memset(cTmp,            0x00,sizeof(cTmp));
	memset(&sCom_sys_parm,  0x00,sizeof(struct com_sys_parm_c));
	memset(&sMo_discnt,     0x00,sizeof(struct mo_discnt_c));
	memset(&sMo_bank_acc_po,0x00,sizeof(struct mo_bank_acc_po_c));
	memset(&sMo_bank_po,    0x00,sizeof(struct mo_bank_po_c));

	pub_base_AllDwnFilName( filename );
	fp = fopen(filename, "w");
	if (fp == NULL) {
		strcpy(g_pub_tx.reply, "S047");
		sprintf(acErrMsg, "��дֽƱ�����ļ���!!!");
		WRITEMSG
		goto ErrExit;
	}


	pub_base_sysinit();
	lRegDate=0;
	get_zd_long("0450",&lRegDate);
	/* ����Ҫ���� 107 108 �����  */
	/* mod by chenhw 20100619
	ret=Mo_bank_po_Dec_Sel(g_pub_tx.reply,"po_sts='1' and sign_date=%d and length(po_no)<=16 order by sign_br_no",lRegDate);  ����ֻͳ��ǩ��״̬�� */
	/* ret=Mo_bank_po_Dec_Sel(g_pub_tx.reply,"po_sts in ('1','2','4') and sign_date=%d and length(po_no)<=16 order by sign_br_no",lRegDate); */
	ret=Mo_bank_po_Dec_Sel(g_pub_tx.reply,"po_sts in ('1','2','4') and (sign_date=%d or cash_date=%d or receive_date=%d) and length(po_no)<=16 order by sign_br_no",lRegDate,lRegDate,lRegDate);

	ERR_DEAL

	while(1)
	{
		memset(&sMo_bank_po,0x00,sizeof(struct mo_bank_po_c));
		ret=Mo_bank_po_Fet_Sel(&sMo_bank_po,g_pub_tx.reply);
		if (ret && ret != 100) {
                        sprintf(acErrMsg, "Fet FOR Sel ERROR [%d]", ret);
                        WRITEMSG
			goto ErrExit;
                } else if (ret == 100) {
                        WRITEMSG
			break;
                }

		memset(&sMo_bank_acc_po,0x00,sizeof(struct mo_bank_acc_po_c));
		ret=Mo_bank_acc_po_Sel(g_pub_tx.reply,&sMo_bank_acc_po,"pact_no='%s' ",sMo_bank_po.pact_no);
		ERR_DEAL

		memset(cLine,           0x00,sizeof(cLine));
		if(sMo_bank_po.po_sts[0]=='1')
		{
			/*add by chengbo 20100713 �жһ�Ʊ��ʧ״̬��Ҫ���Ǽ�������Ϊ��ʧֹ������ʾ�߸�*/
			if(sMo_bank_po.po_ind[0]=='0')
			{
				strcat(cLine,"110");
			}
			else
			{
				strcat(cLine,"101");
			}
		}
		/**wk
		else if(sMo_bank_po.po_sts[0]=='4')**/
		else if(sMo_bank_po.po_sts[0]=='2')
		{
			strcat(cLine,"108");
		}
		else if(sMo_bank_po.po_sts[0]=='4')
		{
			strcat(cLine,"107");
		}
		strcat(cLine,",");
		strcat(cLine,"AC01");
		strcat(cLine,",");
		sMo_bank_po.po_no[16]='\0';		/**Ʊ���16λ**/
		strcat(cLine,sMo_bank_po.po_no);
		strcat(cLine,",");
		memset(cAmt,        0x00,sizeof(cAmt));
		sprintf(cAmt,"%f",sMo_bank_po.po_amt);
		strcat(cLine,cAmt);
		strcat(cLine,",");
		memset(cDateTmp,        0x00,sizeof(cDateTmp));

		if(sMo_bank_po.po_sts[0]=='1')
			/****���ﲻ��ת����wk
			date2newDate(sMo_bank_po.sign_date,&cDateTmp);**/
			sprintf(cDateTmp,"%ld",sMo_bank_po.sign_date);
		else if(sMo_bank_po.po_sts[0]=='4')
			/***
			date2newDate(sMo_bank_po.receive_date,&cDateTmp);***/
			sprintf(cDateTmp,"%ld",sMo_bank_po.receive_date);
		else if(sMo_bank_po.po_sts[0]=='2')
			/***date2newDate(sMo_bank_po.cash_date,&cDateTmp);****/
			sprintf(cDateTmp,"%ld",sMo_bank_po.cash_date);

		strcat(cLine,cDateTmp);
		strcat(cLine,",");
		memset(cDateTmp,        0x00,sizeof(cDateTmp));
		if(sMo_bank_acc_po.matr_date<=0){
			sMo_bank_acc_po.matr_date=sMo_bank_po.sign_date;
		}
		/****date2newDate(sMo_bank_acc_po.matr_date,&cDateTmp);***/
		sprintf(cDateTmp,"%ld",sMo_bank_acc_po.matr_date);
		strcat(cLine,cDateTmp);
		strcat(cLine,",");
		memset(cDateTmp,        0x00,sizeof(cDateTmp));
	/***	date2newDate(sMo_bank_po.sign_date,&cDateTmp); ***/ 
		sprintf(cDateTmp,"%ld",sMo_bank_po.sign_date);
		strcat(cLine,cDateTmp);
		strcat(cLine,",");
		/***�����ǳж�������***/
		memset(brno12,           0x00,sizeof(brno12));
		memset(name60,           0x00,sizeof(name60));
		
		if(nGetBrno12AndName(sMo_bank_acc_po.payer_br_no,brno12,name60)){
			sprintf(acErrMsg, "%s,%d,ȡ�к���Ϣʧ��",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit;
		}
		strcat(cLine,name60);
		strcat(cLine,",");
		strcat(cLine,brno12);
		strcat(cLine,",");
		if(sMo_bank_acc_po.payer_name[0]=='\0'){
			strcat(cLine,"��Ʊ��");
		}else{	
			strcat(cLine,sMo_bank_acc_po.payer_name);
		}
		strcat(cLine,",");
		strcat(cLine,brno12);
		strcat(cLine,",");
		if(sMo_bank_acc_po.payee_name[0]=='\0' || strcmp(sMo_bank_acc_po.payee_name,"0")==0){
			strcat(cLine,"��Ʊ��");
		}else{
			strcat(cLine,sMo_bank_acc_po.payee_name);
		}
		strcat(cLine,",");
		strcat(cLine,brno12);  					/* ����Ҳû���к� TODO */
		strcat(cLine,",");
		strcat(cLine,""); /* ��ע */
		if(sMo_bank_po.po_sts[0]=='4')
			{
				strcat(cLine,",");
				strcat(cLine," "); /* �����������Ҫ��ǰ̨¼�� */
			}

		fprintf(fp,"%s\n",cLine);

	}
	Mo_bank_acc_po_Clo_Sel();

	/** ���濪ʼ���� mo_discnt **/
	ret=Mo_discnt_Dec_Sel(g_pub_tx.reply,"wrk_date=%d and sts!='*' and length(pnote_no)<=16 order by br_no",lRegDate);  /* sts='*' ����һ���α� */
	ERR_DEAL

	while(1)
	{
		memset(&sMo_discnt,0x00,sizeof(struct mo_discnt_c));
		ret=Mo_discnt_Fet_Sel(&sMo_discnt,g_pub_tx.reply);
		if (ret && ret != 100) {
                        sprintf(acErrMsg, "Fet FOR Sel ERROR [%d]", ret);
                        WRITEMSG
			goto ErrExit;
                } else if (ret == 100) {
                        WRITEMSG
			break;
                }

		memset(cLine,           0x00,sizeof(cLine));
		if(sMo_discnt.pn_type[0]=='1')
			strcat(cLine,"102");
		else if(sMo_discnt.pn_type[0]=='2')
			strcat(cLine,"103");
		else if(sMo_discnt.pn_type[0]=='3')
			strcat(cLine,"103");
		else if(sMo_discnt.pn_type[0]=='4')
			strcat(cLine,"103");
		else if(sMo_discnt.pn_type[0]=='5')
			strcat(cLine,"103");
		else if(sMo_discnt.pn_type[0]=='6')
			strcat(cLine,"104");
		else if(sMo_discnt.pn_type[0]=='7')
			strcat(cLine,"104");
		else if(sMo_discnt.pn_type[0]=='8')
			strcat(cLine,"107");
		else 
			strcat(cLine,"125");  /* 9-�����ջ� TODO*/

		strcat(cLine,",");
		if(memcmp(sMo_discnt.note_type,"111",3)==0)
			strcat(cLine,"AC01");
		else
			strcat(cLine,"AC02");

		strcat(cLine,",");
		sMo_discnt.pnote_no[16]='\0';		/***Ʊ���16λ****/
		strcat(cLine,sMo_discnt.pnote_no);
		strcat(cLine,",");
		memset(cAmt,        0x00,sizeof(cAmt));
		sprintf(cAmt,"%f",sMo_discnt.par_amt);
		strcat(cLine,cAmt);

		strcat(cLine,",");
		memset(cDateTmp,        0x00,sizeof(cDateTmp));
		/***date2newDate(sMo_discnt.wrk_date,&cDateTmp); ***/
		sprintf(cDateTmp,"%ld",sMo_discnt.wrk_date);/* ��Ʊ����û�У�����Ϊ�˱��ȥwrk_date TODO */
		strcat(cLine,cDateTmp);

		strcat(cLine,",");
		memset(cDateTmp,        0x00,sizeof(cDateTmp));
		/***date2newDate(sMo_discnt.not_matr_date,&cDateTmp);****/
		sprintf(cDateTmp,"%ld",sMo_discnt.not_matr_date);
		strcat(cLine,cDateTmp);

		strcat(cLine,",");
		memset(cDateTmp,        0x00,sizeof(cDateTmp));
		if(sMo_discnt.pn_type[0]=='1')
			/****date2newDate(sMo_discnt.pn_date,&cDateTmp); *****/
			sprintf(cDateTmp,"%ld",sMo_discnt.pn_date);
		else if(sMo_discnt.pn_type[0]=='2'||sMo_discnt.pn_type[0]=='3'
			||sMo_discnt.pn_type[0]=='4'||sMo_discnt.pn_type[0]=='5')
			/*****date2newDate(sMo_discnt.tpn_date,&cDateTmp); *****/
			sprintf(cDateTmp,"%ld",sMo_discnt.tpn_date);
		else if(sMo_discnt.pn_type[0]=='6'||sMo_discnt.pn_type[0]=='7')
			/***date2newDate(sMo_discnt.rtpn_date,&cDateTmp);****/ 
			sprintf(cDateTmp,"%ld",sMo_discnt.rtpn_date);
		else if(sMo_discnt.pn_type[0]=='8')
		/***	date2newDate(sMo_discnt.not_matr_date,&cDateTmp); ****/
			sprintf(cDateTmp,"%ld",sMo_discnt.not_matr_date);
		else
			/**date2newDate(sMo_discnt.pn_date,&cDateTmp); ***/ 
		sprintf(cDateTmp,"%ld",sMo_discnt.pn_date);/* �����ջ�û������ TODO */
		strcat(cLine,cDateTmp);

		strcat(cLine,",");
		/**sMo_bank_acc_po������жҵ�**/
		/**wk
		if(sMo_bank_acc_po.payer_br_name[0]=='\0'){
			strcat(cLine,"�ж���");
		}else{
			strcat(cLine,sMo_bank_acc_po.payer_br_name);  
		}
		strcat(cLine,",");
		strcat(cLine,sMo_bank_acc_po.payer_br_name);  ****/
		if(sMo_discnt.pn_name[0]=='\0'){
			strcat(cLine,"�ж���"); 				 /**�ж���***/
		}else{
			strcat(cLine,sMo_discnt.pn_name); 			 /**�ж���***/
		}
		memset(brno12,           0x00,sizeof(brno12));
		memset(name60,           0x00,sizeof(name60));
		
		if(nGetBrno12AndName(sMo_discnt.br_no,brno12,name60)){
			sprintf(acErrMsg, "%s,%d,ȡ�к���Ϣʧ��",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit;
		}
		
		strcat(cLine,",");
		strcat(cLine,brno12); /**�ж����к� ����*/
		
		strcat(cLine,",");
		if(sMo_discnt.pn_name[0]=='\0'){
			strcat(cLine,"����������"); 				 /**��������������***/
		}else{
			strcat(cLine,sMo_discnt.pn_name); 			 /**��������������***/
		}
		strcat(cLine,",");
		/***������ȥwk***
		memset(brno12,           0x00,sizeof(brno12));
		memset(name60,           0x00,sizeof(name60));
		
		if(nGetBrno12AndName(sMo_discnt.br_no,brno12,name60)){
			sprintf(acErrMsg, "%s,%d,ȡ�к���Ϣʧ��",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit;
		}****/
		strcat(cLine,brno12); 				/* �������к�,���� */
		strcat(cLine,",");
		strcat(cLine,name60);				/**����������,����**/
		strcat(cLine,",");
		strcat(cLine,brno12);  				/* �������к�,���� */

		strcat(cLine,",");
		strcat(cLine,""); /* ��ע */

		if(sMo_discnt.pn_type[0]=='1')
		{
			strcat(cLine,",");
			memset(cTmp,            0x00,sizeof(cTmp));
			/**����wk**
			sprintf(cTmp,"%f",sMo_discnt.pn_int);
			***/
			sprintf(cTmp,"%f",sMo_discnt.pn_rate);
			strcat(cLine,cTmp);
		}
		else if(sMo_discnt.pn_type[0]=='2'||sMo_discnt.pn_type[0]=='3'
			||sMo_discnt.pn_type[0]=='4'||sMo_discnt.pn_type[0]=='5')
		{
			strcat(cLine,",");
			memset(cTmp,            0x00,sizeof(cTmp));
			/***wk
			sprintf(cTmp,"%f",sMo_discnt.tpn_int);***/
			sprintf(cTmp,"%f",sMo_discnt.tpn_rate);
			strcat(cLine,cTmp);
		}
		else if(sMo_discnt.pn_type[0]=='6'||sMo_discnt.pn_type[0]=='7')
		{
			strcat(cLine,",");
			memset(cTmp,            0x00,sizeof(cTmp));
			/***wk
			sprintf(cTmp,"%f",sMo_discnt.rpn_int);***/
			sprintf(cTmp,"%f",sMo_discnt.rtpn_rate);
			strcat(cLine,cTmp);
		}
		else if(sMo_discnt.pn_type[0]=='8')
		{
			strcat(cLine,",");
			memset(cTmp,            0x00,sizeof(cTmp));
			sprintf(cTmp,"%s"," ");   /* ί���տ�������Ҫд������� ��Ҫ��ǰ̨¼�� TODO*/
			strcat(cLine,cTmp);
		}

		if(sMo_discnt.pn_type[0]=='1') /* ���102��3����ѡ�� */
		{
			strcat(cLine,",");
			strcat(cLine,sMo_discnt.pact_no);
			/* ��Ʊ��źͳж�Э����û��  */
		}

		fprintf(fp,"%s\n",cLine);
	}
	Mo_discnt_Clo_Sel();

	/* ����� �����������ļ�¼,�����α� */
	ret=Mo_discnt_Dec_Sel(g_pub_tx.reply,"wrk_date=%d and sts='*' and pn_type ='1' and length(pnote_no)<=16 order by br_no",lRegDate);  /* sts='*' ����һ���α� */
	ERR_DEAL

	while(1)
	{
		memset(&sMo_discnt,0x00,sizeof(struct mo_discnt_c));
		ret=Mo_discnt_Fet_Sel(&sMo_discnt,g_pub_tx.reply);
		if (ret && ret != 100) {
                        sprintf(acErrMsg, "Fet FOR Sel ERROR [%d]", ret);
                        WRITEMSG
			goto ErrExit;
                } else if (ret == 100) {
                        WRITEMSG
			break;
                }

		memset(cLine,           0x00,sizeof(cLine));

		strcat(cLine,"108");

		strcat(cLine,",");
		if(memcmp(sMo_discnt.note_type,"111",3)==0)
			strcat(cLine,"AC01");
		else
			strcat(cLine,"AC02");

		strcat(cLine,",");
		sMo_discnt.pnote_no[16]='\0';		/**Ʊ���16λ**/
		strcat(cLine,sMo_discnt.pnote_no);
		strcat(cLine,",");
		memset(cAmt,        0x00,sizeof(cAmt));
		sprintf(cAmt,"%f",sMo_discnt.par_amt);
		strcat(cLine,cAmt);

		strcat(cLine,",");
		memset(cDateTmp,        0x00,sizeof(cDateTmp));
		/***date2newDate(sMo_discnt.wrk_date,&cDateTmp);***/
		sprintf(cDateTmp,"%ld",sMo_discnt.wrk_date); /* ��Ʊ����û�У�����Ϊ�˱��ȥwrk_date TODO */
		strcat(cLine,cDateTmp);

		strcat(cLine,",");
		memset(cDateTmp,        0x00,sizeof(cDateTmp));
		/***date2newDate(sMo_discnt.not_matr_date,&cDateTmp);***/
		sprintf(cDateTmp,"%ld",sMo_discnt.not_matr_date);
		strcat(cLine,cDateTmp);

		strcat(cLine,",");
		memset(cDateTmp,        0x00,sizeof(cDateTmp));
		/***date2newDate(sMo_discnt.pn_date,&cDateTmp); ***/
		sprintf(cDateTmp,"%ld",sMo_discnt.pn_date);
		strcat(cLine,cDateTmp);

		/***wk
		if(sMo_bank_acc_po.payer_br_name[0]=='\0'){
			strcat(cLine,"�ж���");
		}else{
			strcat(cLine,sMo_bank_acc_po.payer_br_name);  
		}
		strcat(cLine,",");
		strcat(cLine,sMo_bank_acc_po.payer_br_name);  
		****/
		strcat(cLine,",");
		if(sMo_discnt.pn_name[0]=='\0'){
			strcat(cLine,"�ж���"); 				 /**�ж���***/
		}else{
			strcat(cLine,sMo_discnt.pn_name); 			 /**�ж���***/
		}
		
		memset(brno12,           0x00,sizeof(brno12));
		memset(name60,           0x00,sizeof(name60));
		
		if(nGetBrno12AndName(sMo_discnt.br_no,brno12,name60)){
			sprintf(acErrMsg, "%s,%d,ȡ�к���Ϣʧ��",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit;
		}	
		strcat(cLine,",");
		strcat(cLine,brno12); 				/* �ж����к�,���� */	
		
		strcat(cLine,",");
		if(sMo_discnt.pn_name[0]=='\0'){
			strcat(cLine,"����������"); 				 /**��������������***/
		}else{
			strcat(cLine,sMo_discnt.pn_name); 			 /**��������������***/
		}
		strcat(cLine,",");
		/*****������ȥwk
		memset(brno12,           0x00,sizeof(brno12));
		memset(name60,           0x00,sizeof(name60));
		
		if(nGetBrno12AndName(sMo_discnt.br_no,brno12,name60)){
			sprintf(acErrMsg, "%s,%d,ȡ�к���Ϣʧ��",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit;
		}****/
		strcat(cLine,brno12); 				/* �������к�,���� */
		strcat(cLine,",");
		strcat(cLine,name60);				/**����������,����**/
		strcat(cLine,",");
		strcat(cLine,brno12);  				/* �������к�,���� */


		strcat(cLine,",");
		strcat(cLine,""); /* ��ע */

		strcat(cLine,",");
		memset(cTmp,            0x00,sizeof(cTmp));
		sprintf(cTmp,"%f",sMo_discnt.pn_rate);
		strcat(cLine,cTmp);
		strcat(cLine,",");
		strcat(cLine,sMo_discnt.pact_no);
		/* ��Ʊ��źͳж�Э����û��  */

		fprintf(fp,"%s\n",cLine);
	}
	Mo_discnt_Clo_Sel();

	fclose(fp);
    
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "��������ֽƱ�����ļ��ɹ�!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	set_zd_data(DC_FILE_SND, "1");
	return 0;
ErrExit:
	if (strcmp(g_pub_tx.reply, "0000") == 0 || g_pub_tx.reply[0] == '\0')
		strcpy(g_pub_tx.reply, "G010");
	sprintf(acErrMsg, "��������ֽƱ�����ļ�ʧ��!!!!![%s]", g_pub_tx.reply);
	fclose(fp);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

/***********************************
*ת�����ڸ�ʽΪ YYYY-MM-DD         *
***********************************/
int date2newDate(int date, char *newDate)
{
	char  date1[9];
	if(date<2000){
		strcpy(newDate,"");
		return(0);
	}
	sprintf(date1,"%d",date);
	memcpy(newDate,date1,4);
	strcat(newDate,"-");
	memcpy(newDate+5,date1+4,2);
	strcat(newDate,"-");
	memcpy(newDate+8,date1+6,2);

	return 0;
}

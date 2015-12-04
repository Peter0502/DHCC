/***************************************************************
* �� �� ��:     spJ026.c
* ����������    ���л�Ʊ��ѯ����¼��
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-18  17:27
* ����:���»�Ʊ��״̬,ͬʱ�ڷ�֧���������һ����ѯ����,�����ͳ�ȥ
* �޸ļ�¼�� �޸�Ʊ�ݺ�Ϊ��59��ȡֵ,֧��16λƱ�ݺ�
* ��    ��:  2011��04��15��
* �� �� ��: 
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "hv_fzf_c.h"
#include "hvnet.h"
#include "hv_pub.h"
#include "hv_fzf_c.h"
#include "find_debug.h"

/**********************************
*��ʧ���ڷŵ�ԭί�������ֶ�       *
*��Ʊ���� ���ļ�����		  *
*���������һ����ͨѶ             *
***********************************/

spJ026()
{
	int ret = 0;
	HV_NOTPAY_AREA     hv_notpayin;
	HV_PAYIN_AREA      hv_payin;
	HV_FD123_AREA      fd123;
	struct hv_fzf_c    hv_fzf;
	char cCD_date[9]         ;/***�ж�����****/
	/****
	char cTmplist[256];       
	char cFrontContent[256];
	****/
	char cTmplist[512];
	char cContent[512+128];       /***�齨�Ĳ�ѯ���ĵ������ֶ�**/
        char l_po_no[17];         /** �ӳ�Ʊ�ݺ��ֶΣ�֧��16λ��Ʊ�ݺ� 2011-4-15 **/
	char cFrontContent[101];
	char fmt[10];
	char cShowmsg[200];

	THVPACKAGE  pack301;


	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&hv_payin,     0x00 ,  sizeof(hv_payin));
	memset(&fd123,        0x00 ,  sizeof(fd123));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(cCD_date,      0x00 ,  sizeof(cCD_date));
	memset(cContent,      0x00 ,  sizeof(cContent)); 
	memset(&pack301,      0x00 ,  sizeof(pack301));
        memset(l_po_no ,      0x00 ,  sizeof(l_po_no));           /** ֧��16λ��Ʊ�ݺ� 2011-4-15 **/
	memset(cTmplist,	  0x00 ,  sizeof(cTmplist));
	memset(cFrontContent, 0x00 ,  sizeof(cFrontContent));
	memset(fmt,0x00,sizeof(fmt));	
	memset(cShowmsg,0x00,sizeof(cShowmsg));


    /*****************************
     *PART 1   �õ�ǰ̨����      *
     *****************************/
	/***�õ����֧��ҵ��8583����***/
	ret=iHvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		vtcp_log("[%s][%d]����ǰ̨��ȡ����ʧ����!\n",__FILE__,__LINE__);
		WRITEMSG
		goto ErrExit;
	}
	ret = iHvTistoPayin(&hv_payin);
	if( ret )
	{
		vtcp_log("[%s][%d]����ǰ̨��ȡ����ʧ����!\n",__FILE__,__LINE__);
		
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("_________O_txamt==[%16s]\n",hv_notpayin.F_o_txamt);


	get_fd_data("0470",cCD_date);/***�ж�����****/
	get_fd_data("1230",(char *)&fd123);/**��ȡ123��****/
        get_fd_data("0590",l_po_no);       /** ֧��16λ��Ʊ�ݺ� 2011-4-15 **/	

	/**********************************/
	ret = GetOr_br_noByBr_no(g_pub_tx.tx_br_no,hv_notpayin.F_or_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"���ݽ��׻�����ȡ�кŴ���!\n");
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	MEMCPY_DEBUG(hv_notpayin.F_ac_br_no,hv_payin.F_ac_br_no,sizeof(hv_notpayin.F_ac_br_no));
        /** 20131025 ��hv_notpayin.F_content�ӽ����� hzh  start**/
        hv_notpayin.F_content[sizeof(hv_notpayin.F_content)-1]='\0';
        /** 20131025 ��hv_notpayin.F_content�ӽ����� hzh  end**/
        vtcp_log("�ӳ�Ʊ�ݺ���==[%s]\n",l_po_no);   /** ֧��16λ��Ʊ�ݺ� 2011-4-15 **/
	vtcp_log("��Ʊ����==[%s]\n",hv_notpayin.F_o_wt_date);
	vtcp_log("��Ʊ��������==[%.60s]\n",hv_notpayin.F_o_fzf_date);
	vtcp_log("��Ʊ�˵��ʺ�==[%.32s]\n",hv_payin.F_pay_ac_no);
	vtcp_log("��Ʊ�˵�ȫ��==[%.60s]\n",hv_payin.F_pay_name);
	vtcp_log("�տ��˵��ʺ�==[%.32s]\n",hv_payin.F_cash_ac_no);
	vtcp_log("�տ��˵�ȫ��==[%s]\n",hv_payin.F_cash_name);
	vtcp_log("�����е�ȫ��==[%s]\n",hv_payin.F_pay_addr);
	vtcp_log("��ѯ�����====[%s]\n",hv_notpayin.F_orderno);
	vtcp_log("��ѯ������====[%s]\n",hv_notpayin.F_wt_date);
	vtcp_log("�鸴���к�====[%s]\n",hv_payin.F_ac_br_no);
	vtcp_log("Ʊ�ݵĽ��====[%s]\n",hv_notpayin.F_o_txamt);
	vtcp_log("�����к�  ====[%s]\n",hv_notpayin.F_or_br_no);
	vtcp_log("����  ====[%s]\n",hv_notpayin.F_content);

	vtcp_log("��ʼƴ��ѯ�ĸ��Բ���[%s][%d]\n",__FILE__,__LINE__);	

/**     ��Ϊ�ӳ�16ΪƱ�ݺ� MOD BY YANQingQing 2011-4-15
	sprintf(cTmplist,"��Ʊ��%.8s",fd123.po_no);
**/
        sprintf(cTmplist,"��Ʊ��%.16s",l_po_no);

	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));

	sprintf(cTmplist,"��Ʊ��%.8s",hv_notpayin.F_o_wt_date);
	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));


	sprintf(cTmplist,"������%.8s",hv_notpayin.F_o_fzf_date);
	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));


	/*********add by liuyue 20061116  �������ũ�еı����� ������ ����  103��ũ��****/
	if(memcmp(hv_payin.F_ac_br_no,"103",3))
	{
		sprintf(cTmplist,"��Ʊ�ʺ�%.32s",hv_payin.F_pay_ac_no);
		strcat(cContent,cTmplist);
		memset(cTmplist,0x00,sizeof(cTmplist));
		vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));
	}
	
	sprintf(cTmplist,"��Ʊ��%.60s",hv_payin.F_pay_name);
	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));


	/*********add by liuyue 20061116  �������ũ�еı����� ������ ����****/
	if(memcmp(hv_payin.F_ac_br_no,"103",3))
	{
		sprintf(cTmplist," �տ��ʺ�%.32s",hv_payin.F_cash_ac_no);
		strcat(cContent,cTmplist);
		memset(cTmplist,0x00,sizeof(cTmplist));
		vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));
	}

	sprintf(cTmplist,"�տ���%.60s",hv_payin.F_cash_name);
	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));

	sprintf(cTmplist," ������%.40s",hv_payin.F_pay_addr);
	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));

	vtcp_log("[%s][%d] F_pay_addr==[%s]",__FILE__,__LINE__,hv_payin.F_pay_addr);
	vtcp_log("[%s][%d] CCONTENT==[%s]",__FILE__,__LINE__,cContent);
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));
	
	vtcp_log("[%s][%d]HV_NOTPAYIN.CONTENT==[%s]\n",__FILE__,__LINE__,hv_notpayin.F_content);
	vtcp_log("[%s][%d]CFRONTCONTENT==[%s]\n",__FILE__,__LINE__,cFrontContent);
	MEMCPY_DEBUG(cFrontContent,hv_notpayin.F_content,sizeof(hv_notpayin.F_content));
	zip_space(cFrontContent);
	sprintf(fmt,"%%%ds",strlen(cFrontContent));
	strcat(cContent,"  ");
	sprintf(cTmplist,fmt,cFrontContent);
	strcat(cContent,cTmplist);

	vtcp_log("ƴ�ĸ��Ե�����Ϊ:[%s]\n",cContent);

	if(strlen(cContent)>255)
	{
		sprintf(cShowmsg,"��ѯ���Գ���255С�� [%d]��ü����������Լ�����",strlen(cContent));
		strcpy(g_pub_tx.reply,"HV01");
		vtcp_log("[%s][%d]��ƴ���Գ���255,strlen=[%d]\n",__FILE__,__LINE__,strlen(cContent));
		set_zd_data("0130",cShowmsg);
		goto ErrExit;
	}

	ret = iCharacterType(cContent,strlen(cContent),'g');
	if(ret)
	{
		vtcp_log("[%s][%d]��ɵĸ����д��ڷǷ��ַ�\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data("0130","��������д��ڰ�����ֻ��߰�Ǳ��");
		goto ErrExit;
	}

	deal_half_charcter(cContent);
	vtcp_log("[%s]\n",cContent);




	/**********************
         *PART 2   �������   *
	 **********************/
	ret = iHv_checkstat(hv_notpayin.F_cmtno);
	if(ret)
	{
		sprintf(acErrMsg,"����״̬������!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	if(!memcmp(hv_notpayin.F_or_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_notpayin.F_ac_br_no)))
	{
		sprintf(acErrMsg,"��ѯ�кͲ鸴�в�����ͬһ���е�!\n");
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;	
	}		

	/*********************
	 *PART 3  �������ݿ� *
         *********************/
	hv_notpayin.T_hv_fzf_sts[0] = '1'; /*****�ı�״̬*****/	
	MEMCPY_DEBUG(hv_notpayin.F_content,cContent,sizeof(hv_notpayin.F_content));/****���ø�������***/
	/***��ȡ�ձ������Լ����������к�****/
	/*
	ret = iGet_full_hv_no(hv_notpayin.F_ac_br_no,hv_notpayin.F_receco,hv_notpayin.F_cash_qs_no);*/
	/***����ԭ��������Ϊ�Է��Ľ�����*****/
	MEMCPY_DEBUG(hv_notpayin.F_o_ac_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_notpayin.F_o_ac_br_no));


	vtcp_log("[%s][%d]cmtno===[%s]\n",__FILE__,__LINE__,hv_notpayin.F_cmtno);	
	ret = iHvfFrontToTable(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"F->T����!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P018");
		goto ErrExit;
	}
	vtcp_log("T_ac_br_no==[%s]\n",hv_notpayin.T_ac_br_no);
	vtcp_log("T_ac_br_no==[%s]\n",hv_notpayin.T_ac_br_no);
	vtcp_log("T_ac_br_no==[%s]\n",hv_notpayin.T_ac_br_no);
	/****��Ҫ��ȡ�ձ�������****/
	print_notpay(&hv_notpayin,'A');
	ret = iHvUpdFzfRecFromPayin(&hv_notpayin,1);
	if(ret)
	{
		sprintf(acErrMsg,"�����֧�����ݱ����!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	/***д��ʷ��ϸ****/
	ret = dg_hv_fzfhtr(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"�����֧����ʷ��ϸ�����!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	/*****��ʼֱ�Ӹ����ͳ�ȥ��*****/
	vtcp_log("��ʼ��������![%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("��ʼ��������![%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("��ʼ��������![%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("��ʼ��������![%s][%d]\n",__FILE__,__LINE__);



	MEMCPY_DEBUG(hv_notpayin.T_req_content,cContent,sizeof(hv_notpayin.T_req_content));
	vtcp_log("��ʼ��������![%s][%d]\n",__FILE__,__LINE__);
	MEMCPY_DEBUG(hv_notpayin.T_o_or_br_no,hv_notpayin.T_or_br_no,sizeof(hv_notpayin.T_or_br_no));
	vtcp_log("��ʼ��������![%s][%d]\n",__FILE__,__LINE__);
	pub_com_CMT301_T2TAG(&hv_notpayin,&pack301);
	vtcp_log("��ʼ��������![%s][%d]\n",__FILE__,__LINE__);
	ret=hv_gen_pack(&pack301);
	vtcp_log("��ʼ��������![%s][%d]\n",__FILE__,__LINE__);
	if(ret<0){
 		sprintf(acErrMsg,"��֯���֧�����Ĵ���!\n");
                WRITEMSG
                strcpy( g_pub_tx.reply, "P021" );
                goto ErrExit;
        }
        if(hv_insert_sendlist(&pack301,hv_notpayin.F_tx_br_no,hv_notpayin.F_tel\
                        ,hv_notpayin.T_trace_no,hv_notpayin.T_orderno)<0){
                sprintf(acErrMsg,"���뷢�͵Ǽǲ�ʧ��!\n");
                WRITEMSG
                strcpy( g_pub_tx.reply, "P010" );
                goto ErrExit;
        }

        if(hv_sendpack(&pack301)){
                sprintf(acErrMsg,"����301���ĵ�ǰ�û�ʧ��!\n");
                strcpy( g_pub_tx.reply, "LS88" );
                WRITEMSG
                goto ErrExit;
        }

	vtcp_log("[%s][%d]���ͳɹ���!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]���ͳɹ���!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]���ͳɹ���!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]���ͳɹ���!\n",__FILE__,__LINE__);

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
	MEMCPY_DEBUG(fd123.orderno,hv_notpayin.T_orderno,sizeof(hv_notpayin.T_orderno));
	MEMCPY_DEBUG(fd123.ac_br_no,hv_notpayin.T_ac_br_no,sizeof(hv_notpayin.T_ac_br_no));
	vtcp_log("[%s]\n",cContent);
	MEMCPY_DEBUG(fd123.notpay_content,cContent,strlen(cContent));
	/*
	deal_half_charcter(cContent,fd123.notpay_content,sizeof(cContent));*/

	

OkExit:
	 pub_base_full_space((char *)&fd123,sizeof(fd123));
     set_zd_data("1230",(char *)&fd123);
	
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0280",hv_notpayin.T_orderno);/****����һ����ž��㹻��*/
	set_zd_data("0440",hv_notpayin.T_wt_date);/****��ί�����ڹ�ȥ******/
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


int deal_half_charcter(char *buf)
{
		int i = 0;
		int j = 0;
		int size = 0;
		int index =0;
		
		
		

		printf("[%s]====\n",buf);
		size = strlen(buf);
		
		char cBuf[1024];
		
		memset(cBuf,0x00,sizeof(cBuf));


		for(i=0;i<size;i++)
		{
				if((int)buf[i]<128 &&(int)buf[i+1]>128 && (i+1+index)%2)
				{
						cBuf[j] = buf[i];
						cBuf[j+1]= ' ';
						j = j+2;
						index++;
				}
				else
				{
						cBuf[j] = buf[i];
						cBuf[j] = buf[i];
						j++;
				}
		}

		MEMCPY_DEBUG(buf,cBuf,sizeof(cBuf));
		return 0;
}



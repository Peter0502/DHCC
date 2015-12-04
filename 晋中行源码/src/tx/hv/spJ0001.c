/***************************************************************
* �� �� ��:     spJ0001.c
* ����������    ���л�Ʊ��Ϣ��ѯ			
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-17 16:51
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"

typedef struct{
	char po_ind[1];		/***��Ʊ����***/
	char deal_mode[1];	/***ת������***/
	char po_no[16];		/***��Ʊ����***/
}struct_hp;
	

spJ0001()
{
	int ret = 0;
	char *cFilename ;
	FILE *fp;
	HV_PAYIN_AREA hv_payin;
	struct_hp	hv_hp;
	struct hv_poinfo_c hv_poinfo;
	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_payin,  0x00 ,  sizeof(hv_payin));
	memset(cFilename,  0x00 ,  sizeof(cFilename));
	memset(&hv_hp,	   0x00 ,  sizeof(hv_hp));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));

	/***�õ����֧��ҵ��8583����***/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
   	/*****�õ����������������**********/
	MEMCPY_DEBUG(cFilename,"../tmp/",7);
	strcat(cFilename,hv_payin.F_tx_br_no);
	strcat(cFilename,hv_payin.F_tel);
	sprintf(acErrMsg,"�������Ӧ���ļ�����[%s]\n",cFilename);
	WRITEMSG
	pub_base_AllDwnFilName(cFilename);
	fp = fopen(cFilename,"rb");
	if(fp==NULL)
	{
		sprintf(acErrMsg,"�������Ӧ���ļ�����[%s]������\n",cFilename);
		return (-1);
	}
	fget(&hv_hp,sizeof(hv_hp),fp);
	fclose(fp);


    /************************************************************************	
     *1.������õ��˸������������������									*
	 *2.��Ϊ��Ʊ��Ϣ��¼�����ר�ŵĻ�Ʊ������Ϣ��							*
	 *3.��ʼ�Ի�Ʊ������Ϣ�ṹ����и�ֵ								    *	
     ************************************************************************/
	
		/******��ȡ����********************/
		MEMCPY_DEBUG(hv_poinfo.po_no,hv_hp.po_no,sizeof(hv_poinfo.po_no)-1);
		MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_payin->F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);

		ret = Hv_poinfo_Sel(g_pub_tx.reply,&hv_poinfo,"po_no=%s and pay_br_no=%s ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
		if(ret)
		{
			sprintf(acErrMsg,"���һ�Ʊ��Ϣ�������룽[%s] �к�==[%s]ret=[%d]\n",hv_poinfo.po_no,hv_poinfo.pay_br_no);
			WRITEMSG
			return (-1);
		}
	 
	/* д��ҵ����ˮ */
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	else 
		goto OkExit;
	/*** ������� ***/

OkExit:
	set_zd_data("1235",hv_poinfo.pay_ac_no);
	set_zd_data("1236",hv_poinfo.pay_name);
	set_zd_data("1237",hv_poinfo.pay_addr);
	set_zd_data("0390",hv_poinfo.po_bal);
	set_zd_data("123D",hv_poinfo.cash_ac_no);
	set_zd_data("123E",hv_poinfo.cash_name);
	set_zd_data("123C",hv_ppinfo.cash_br_no);
	set_zd_data("123F",hv_poinfo.cash_br_name);/***�ŵ��տ��˵�ַ�д�***/
	set_zd_data("123H",hv_poinfo.brf);
	set_zd_data("0700",hv_poinfo.po_ind);/**��Ʊ����**/
	set_zd_data("0710",hv_poinfo.deal_mode);/**ת������**/
	
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		MEMCPY_DEBUG(g_pub_tx.reply,"T062",4);
	}
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

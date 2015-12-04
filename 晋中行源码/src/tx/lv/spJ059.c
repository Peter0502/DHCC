/***************************************************************
* �� �� ��:     spJ059.c
* ����������    С���˻�Ӧ��		
*
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hv_define.h"
#include "hvpack.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"
#include "hv_fzfhtr_c.h"

spJ059()
{
	int ret = 0;
	int iOrderno = 0;
	int iRecordNum = 0;
	HV_NOTPAY_AREA hv_notpayin;
        struct	lv_pkgreg_c    lv_pkgreg;
	struct  hv_fzf_c       hv_fzf;
	struct  hv_fzfhtr_c    hv_fzfhtr;
	HV_FD123_AREA          fd123;
	int flag = 1;/**��־λ,������־ԭ��֧�����׼�¼�Ƿ����,0 ������ 1����*/

	char cPackid[9]		;
	char cPack_date[9]	;/***��ΪС��ķ�֧����������Ҫ�õ������ֶ�***/

	
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&lv_pkgreg,	      0x00 ,  sizeof(lv_pkgreg));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(&hv_fzfhtr,    0x00 ,  sizeof(hv_fzfhtr));
	memset(&fd123,	      0x00 ,  sizeof(fd123));

	memset(cPackid,	      0x00 ,  sizeof(cPackid));
	memset(cPack_date,    0x00 ,  sizeof(cPack_date));



	/***********************
	* 1.  ��ʼ��ȫ�ֱ���   *
	************************/

	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}



	 /************************
     	 * 2.  ��ȡ8583������    *
    	 *************************/


	ret=iLvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
	/********************
         * 3.     �������  *
	 ********************/

	ret = iLvFzFCommonCheck(&hv_notpayin);
	if(ret)
	{
		vtcp_log("[%s][%d]ִ�����ݻ������ʧ��!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P054");
		goto ErrExit;
	}

	/******���ԭ�����ཻ���Ƿ����,״̬�Ƿ���ȷ****/	
	ret = lv_check_notpay_record(&hv_notpayin,&hv_fzf,"320");
	if(ret)
	{
		vtcp_log("���ԭ��֧������ʧ��!\n",__FILE__,__LINE__);
		goto ErrExit;
	}

		

	/********************
         * 4.   ���ݿ����  *
	 ********************/


	vtcp_log("[%s][%d]������ϸ��!\n",__FILE__,__LINE__);
	iLvFzfToFzfhtr(&hv_fzf,&hv_fzfhtr);
	get_zd_long("0060",&hv_fzfhtr.tx_time);/* ����ʱ��*/
	get_zd_long("0040",&hv_fzfhtr.trace_no);/* ��ˮ��*/
	vtcp_log("[%s][%d]!!!!!!!!!!![%ld][%ld]\n",hv_fzfhtr.tx_time,hv_fzfhtr.trace_no);
	ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"����hv_fzfhtr����!\n");
		WRITEMSG
		vtcp_log("ret===[%d]\n",ret);
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}


	/*******************
   	 *PART 5  д��ˮ�� *
     	 *******************/	
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}



	/*******************
	 * 6.���ͱ���      *
	 *******************/

	ret = lv_get_cmt320_tag(&hv_fzf);
	if(ret)
	{
		vtcp_log("[%s][%d]����320���Ĵ���!ret=[%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	}

	/********************
	 * 7.׼���������   *
	 ********************/
	memcpy(fd123.orderno,hv_notpayin.F_orderno,sizeof(fd123.orderno));
	pub_base_full_space((char *)&fd123,sizeof(fd123));
        set_zd_data("1230",(char *)&fd123);

	

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


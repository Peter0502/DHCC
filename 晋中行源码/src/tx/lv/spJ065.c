/***************************************************************
* �� �� ��:   spJ065.c
* ����������  С��ҵ��鸴����¼��
*
* ��    ��:   dongxy
* ������ڣ�  2006-8-29 
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "lv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#define loc_t sys_loc_t


/****ע����  ���ڲ鸴��û�б�Ҫȡ�ҵ�ԭ��ѯ������״̬��******/

spJ065()
{
	int  ret = 0;
	int  iOrderno=0;
   	char cBrno[6];		/** ���׻��� **/

	NOTPAY_IN_AREA lv_notpay;
	HV_NOTPAY_AREA hv_notpay;
	struct hv_fzf_c hv_fzf;
	struct hv_fzfhtr_c hv_fzfhtr;
	LV_NOTPAY_ADD lv_addinfo;
	HV_FD123_AREA fd123;

	memset(cBrno ,0 ,sizeof(cBrno));
	memset(&lv_notpay,0,sizeof(lv_notpay));
	memset(&hv_notpay,0x00,sizeof(hv_notpay));
	memset(&hv_fzf,	  0x00,sizeof(hv_fzf));
	memset(&hv_fzfhtr,0x00,sizeof(hv_fzfhtr));
	memset(&fd123,	  0x00,sizeof(fd123));

	vtcp_log("[%s][%d]spJ065С��ҵ��鸴����¼��BEGIN",__FILE__,__LINE__);


	/************************
	 * 1. ��ʼ��ȫ�ֱ���    *
	 ************************/
	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	/************************
	 * 2.��ȡ8583������     *
         ************************/
	ret  = iLvTistoNotpay(&hv_notpay);
	if(ret )
	{
		sprintf( acErrMsg,"����iHvTistoNotpay����![%s][%d]",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P053");
		WRITEMSG
		goto ErrExit;
	}


	/**********************
	 * 3.���׻������     *
 	 **********************/


	/* �����ԭ��¼�Ƿ����; ״̬�Ƿ���ȷ; ��hv_fzf�����˸���**/
	 ret = lv_check_notpay_record(&hv_notpay,&hv_fzf,"302");
	 if(ret)
	 {
		vtcp_log("[%s][%d]��ԭ��֧�����׼�¼�Ĵ���ʧ��\n",__FILE__,__LINE__);
		/***���g_pub_tx.reply��ֵ*****/
		goto ErrExit;
	 }	

	/**********************
	 * 4. ���ݿ����      *
 	 **********************/

        vtcp_log("[%s][%d]��ʼ����hv_fzfhtr\n",__FILE__,__LINE__);

        iLvFzfToFzfhtr(&hv_fzf,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/

	get_zd_long("0060",&hv_fzfhtr.tx_time);/* ����ʱ��*/
        get_zd_long("0040",&hv_fzfhtr.trace_no);/* ��ˮ��*/
	
	memcpy(hv_fzfhtr.orderno,hv_fzf.res_orderno,sizeof(hv_fzfhtr.orderno)-1);

        ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
        if(ret)
        {
                sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"HV01");
                goto ErrExit;
        }
	
	/**********************
	 * 5. ׼����ˮ        *
 	 **********************/

	ret = pub_ins_trace_log();
	if ( ret )
	{
 		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	
	


	/**********************
	 * 6.����ҵ���       *
	 **********************/
	
	 ret = lv_get_cmt302_tag(&hv_fzf);
	
	 if(ret)
	 {
		vtcp_log("[%s][%d]����302����!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	 }

	 	
	/**********************
	 * 7.   ׼������ֵ    *
 	 **********************/

	vtcp_log("ORDERNO===[%s]\n",hv_notpay.F_orderno);
	memcpy(fd123.orderno,hv_notpay.F_orderno,sizeof(fd123.orderno));
	memcpy(fd123.qr_tx_num,hv_fzf.req_orderno,sizeof(fd123.qr_tx_num));
	vtcp_log("qtyxnum===[%s]\n",hv_fzf.req_orderno);
	memcpy(fd123.ac_br_no,hv_fzf.ac_br_no,sizeof(fd123.ac_br_no));
	memcpy(fd123.notpay_content,hv_fzf.req_content,sizeof(fd123.notpay_content));
 	pub_base_full_space((char *)&fd123,sizeof(fd123));
        set_zd_data("1230",(char *)&fd123);
        vtcp_log("[%s]\n",(char *)&fd123);

OkExit:
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
	
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
	

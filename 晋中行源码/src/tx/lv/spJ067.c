/***************************************************************
* �� �� ��:   spJ067.c
* ����������  С��ͨ����Ϣҵ��¼��
*
* ��    ��:   dongxy
* ������ڣ�  2006-8-29 
*
* �޸ļ�¼��
* ��    ��: 2006-9-18
* �� �� ��: liuyue 
* �޸�����:
**************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "lv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_fd123.h"
#define loc_t sys_loc_t

spJ067()
{
	int  			iOrderno = 0;
   	char 			cBrno[6];		/** ���׻��� **/
	PAY_IN_AREA 		lv_payin;
	struct lv_pkgreg_c 	lv_pkgreg;
	LV_FD123_AREA		fd123;	
	int			ret = 0;


	memset(cBrno ,	  	0x00,		sizeof(cBrno));
	memset(&lv_payin,	0x00,		sizeof(lv_payin));
	memset(&lv_pkgreg,	0x00,		sizeof(lv_pkgreg));
	memset(&fd123,		0x00,		sizeof(fd123));

	vtcp_log("[%s][%d]spJ067 Begin!ͨ����Ϣҵ��¼��",__FILE__,__LINE__);


	/**********************
	 * 1.��ʼ��ȫ�ֱ���   *
	 **********************/

	 pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}



	/**********************
	 * 2.��ȡ8583������   *
	 **********************/

	 ret = iLvTisToPayin(&lv_payin);
	 if(ret)
	 {
		vtcp_log("[%s][%d]��ǰ̨��ȡ����ʧ��!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P053");
		goto ErrExit;
	 }

    ret =  iLvPayinCheck1(&lv_payin);
    if(ret)
    {
        vtcp_log("[%s][%d]�кż�����!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P053");
        goto ErrExit;
    }

	 /***********************
	  * 3. ִ�����ݻ������ *
	  ***********************/

	  ret = iLvFzFCommonCheck2(&lv_payin);
	  if(ret)
	  {
		vtcp_log("[%s][%d]��ȡ8583������ʧ��!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P054");
		goto ErrExit;
	  }


	  /********************
	   * 4.ִ�����ݿ���� *
	   ********************/



	/*****���ɽ������*********/
	iOrderno=iLvGetOrderno();

	if(iOrderno<=0)
	{
		vtcp_log("%s,%d,�õ�֧��������Ŵ���!\n",__FL__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		goto ErrExit;
	}
	apitoa(iOrderno,sizeof(lv_payin.F_orderno),lv_payin.F_orderno);
 
	ret = iLvNewRecFromPayin(&lv_payin);
	if(ret)
	{
		vtcp_log("[%s][%d]ִ�к���iLvNewRecFromPayin����!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P046");
		goto ErrExit;
	}


	ret = iLvUpdRecFromPayin(&lv_payin,1);
	if(ret)
	{
		vtcp_log("[%s][%d]ִ�к���iLvUpdRecFromPayin����!\n",__FILE__,__LINE__);	
		strcpy(g_pub_tx.reply,"P046");
		goto ErrExit;
	}

	vtcp_log("[%s][%d]���ݿ�����ɹ�!\n",__FILE__,__LINE__);
	  

	/***********************
	 * 5.׼����ˮ��־      *
	 ***********************/

        ret  = pub_ins_trace_log();
        if (ret)
        {
                sprintf( acErrMsg, "����ˮ��־��" );
                WRITEMSG
                goto ErrExit;
        }

	/*****************
	 * 6.׼�����    *
	 *****************/
	memcpy(fd123.orderno,lv_payin.F_orderno,sizeof(fd123.orderno));
	vtcp_log("[%s][%d]F_ordenro===[%.8s]\n",__FILE__,__LINE__,fd123.orderno);
	pub_base_full_space((char *)&fd123,sizeof(fd123));
	set_zd_data("1230",(char *)&fd123);
	vtcp_log("[%s]\n",(char *)&fd123);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
	

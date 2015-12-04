/***************************************************************
* �� �� ��:     spJ019.c
* ����������    ��������˻�¼��		
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-18  17:27
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
#include "find_debug.h"

spJ019()
{
	int ret = 0;
	HV_NOTPAY_AREA hv_notpayin;
        struct	hv_zf_c        hv_zf;
	struct  hv_fzf_c       hv_fzf;
	HV_FD123_AREA          fd123 ;
	THVPACKAGE   pack313;
	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&hv_zf,	      0x00 ,  sizeof(hv_zf));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(&fd123 ,       0x00 ,  sizeof(fd123));
	memset(&pack313, 0 , sizeof(pack313));


	/*****************************
     *PART 1   �õ�ǰ̨����      *
     *****************************/
	/***�õ����֧��ҵ��8583����***/
	ret=iHvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
	/**********************
     *PART 2   �������   *
	 **********************/
	 /***********����һ��֧����************/
	MEMCPY_DEBUG(hv_zf.orderno,hv_notpayin.F_o_payin_orderno,sizeof(hv_zf.orderno)-1);
        GetOr_br_noByBr_no(g_pub_tx.tx_br_no,hv_notpayin.F_or_br_no);
	MEMCPY_DEBUG(hv_zf.or_br_no,hv_notpayin.F_or_br_no,sizeof(hv_zf.or_br_no)-1);
	hv_zf.wt_date = apatoi(hv_notpayin.F_o_wt_date,sizeof(hv_notpayin.F_o_wt_date));
	MEMCPY_DEBUG(hv_zf.cmtno, hv_notpayin.F_o_cmtno,sizeof(hv_zf.cmtno)-1);

	vtcp_log("orderno=[%s]\n",hv_zf.orderno);
	vtcp_log("or_br_no==[%s]\n",hv_zf.or_br_no);
	vtcp_log("wt_date==[%lf]\n",hv_zf.wt_date);
	vtcp_log("cmtno====[%s]\n",hv_zf.cmtno);

	ret = Hv_zf_Dec_Upd(g_pub_tx.reply,"orderno='%s' and or_br_no='%s' and wt_date=%d and cmtno='%s' ",hv_zf.orderno,hv_zf.or_br_no,hv_zf.wt_date,hv_zf.cmtno);
	if(ret)
	{
		sprintf(acErrMsg,"ִ�к���hv_zf_dec_upd����!ret=[%d]\n",ret);
		strcpy(g_pub_tx.reply,"P017");	
		WRITEMSG
		goto ErrExit;
	}
	ret = Hv_zf_Fet_Upd(&hv_zf,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"ִ�к���hv_zf_fet_Upd����!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P017");	
		goto ErrExit;
	}
	/*****����״̬********/
	if(hv_zf.hv_sts[0]!=SEND_ACCT_QS)
	{
		sprintf(acErrMsg,"ԭ֧�����ײ��������˻�����[%c]\n",hv_zf.hv_sts[0]);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P016");
		goto ErrExit;
	}
	
	ret = Hv_zf_Upd_Upd(hv_zf,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"ִ�к���hv_zf_upd_updʧ��!ret==[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P017");	
		goto ErrExit;
	}

		

	/*********************
	 *PART 3  �������ݿ� *
         *********************/
	MEMCPY_DEBUG(hv_notpayin.F_o_or_br_no,hv_notpayin.F_or_br_no,sizeof(hv_notpayin.F_o_or_br_no));
	MEMCPY_DEBUG(hv_notpayin.F_o_ac_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_notpayin.F_o_ac_br_no));
	ret = iHvfFrontToTable(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"ִ�к���F->T����!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P015");
		goto ErrExit;
	}
	/**************��ȡ�µķ�֧�����׵����
	ret = iGetHvOrderno(hv_notpayin.T_orderno);
	if(ret)
	{
		sprintf(acErrMsg,"�����µĽ�����Ŵ���!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P019");
		goto ErrExit;
	}
	************************************************/


	/*****************ֻ�ܼӵ�������***********/
	hv_notpayin.T_hv_fzf_sts[0]=SEND_ACCT_WAIT;	

	ret = iHvUpdFzfRecFromPayin(&hv_notpayin,1);
	if(ret)
	{
		sprintf(acErrMsg,"�����¼ʧ��!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P010");
		goto ErrExit;
	}

	/************�����֧����ϸ��*****************/
	ret = dg_hv_fzfhtr(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"�����֧����ϸ�����!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P011");
		goto ErrExit;
	}
	/******��ȡ����ֵ*****/
	MEMCPY_DEBUG(fd123.orderno,hv_notpayin.T_orderno,sizeof(fd123.orderno));
	MEMCPY_DEBUG(fd123.or_br_no,hv_notpayin.T_or_br_no,sizeof(fd123.or_br_no));

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
  /****Add by SSH,2006.8.28,ֱ�ӷ���****/
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s]",__FILE__,__LINE__,hv_notpayin.T_req_content);
	pub_com_CMT313_T2TAG(&hv_notpayin,&pack313);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s],sizeof(pack313)=[%d] ",__FILE__,__LINE__,hv_notpayin.T_req_content);
	ret=hv_gen_pack(&pack313);
	if(ret<0){
		sprintf(acErrMsg,"��֯���֧�����Ĵ���!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack313,hv_notpayin.F_tx_br_no,hv_notpayin.F_tel\
			,hv_notpayin.T_trace_no,hv_notpayin.T_orderno)<0){
		sprintf(acErrMsg,"���뷢�͵Ǽǲ�ʧ��!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack313)){
		sprintf(acErrMsg,"����313���ĵ�ǰ�û�ʧ��!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	pub_base_full_space((char *)&fd123,sizeof(fd123));
	set_zd_data("1230",(char *)&fd123);
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


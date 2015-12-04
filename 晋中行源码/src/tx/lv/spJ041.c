/***************************************************************************/
/* �ļ�����: dh651000.pc                                                   */
/* ��������: ��ͨ����ҵ��¼��                                              */
/* ��������: 2005.08.01                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "lv_define.h"
#include "hv_define.h"
#include "note_parm_c.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"

static PAY_IN_AREA payin;
/******************************************************************************
*                                                                             *
*   FUNCTIONS:         dh65100_Work                                           *
*                                                                             *
*   DESCRIPTION:       1.��ʼ��������                                         *
*                      2.ִ�м��                                             *
*                      3.����(����)�Ǽǲ�                                     *
*                      4.׼���������   �                                     *
******************************************************************************/
int spJ041()
{
	int ret=0;
	int iRc=0;
	long ori_ac_id;
vtcp_log("AP MSG: Begin of dh65100_Work\n");
	/*********************/
	/* Initial variables */
	/*********************/
	ori_ac_id=g_pub_tx.ac_id;
	iRc=ilv10_Initial();
vtcp_log("[%s][%d]\n", __FL__);
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d init error!",__FL__);
		WRITEMSG
		goto ErrExit;
	}
	/**********************************/
	/* verify data for basic checking */
	/**********************************/
	iRc=ilv10_Verify();
	vtcp_log("[%s][%d]\n", __FL__);
	/********************************/
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d verify error!",__FL__);
		WRITEMSG
		goto ErrExit;
	}
	/********************************************************/
	/* Process -- 1.Initial PBMR record to opening stauts   */
	/*            2.Insert PBMR record/Delete(reversal)     */
	/********************************************************/
	iRc=ilv10_Process();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d process error!",__FL__);
		WRITEMSG
		goto ErrExit;
	}
	/*******************************/
	/* prepare XDTL data for batch */
	/*******************************/
	/******************************/
	/*       Prepare TOS          */
	/******************************/
	iRc=ilv10_PreTos();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
		WRITEMSG
		goto ErrExit;
	}
	
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        strncpy(g_pub_tx.reply,"S023",4);
        WRITEMSG
        goto ErrExit;
    }
	/* ���Ӷ�ҵ��ί��ƾ֤�Ĵ���  added by liuxuan 2007-3-31*/
	get_zd_data("1021",g_pub_tx.ac_no);
	get_zd_data("1023",g_pub_tx.note_type);
	get_zd_data("1024",g_pub_tx.beg_note_no);
	get_zd_data("1024",g_pub_tx.end_note_no);
	get_zd_int("1022",&g_pub_tx.ac_seqn);
	g_pub_tx.ac_id=ori_ac_id;
	vtcp_log("sssssssspayin.F_pkgno[%s],g_pub_tx.note_type=[%s]\n",payin.F_pkgno,g_pub_tx.note_type);
	/* del by martin 20071108 
	if((memcmp(payin.F_pkgno,"001",3)==0)&&(memcmp(g_pub_tx.note_type,"005",3)==0))
	{
		vtcp_log("��ʼƾ֤���� ac_no[%s]type[%s][%s][%s][%d]\n",g_pub_tx.ac_no,g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
		iRc=pub_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);        
		if(iRc)
		{
			sprintf(acErrMsg,"����ƾ֤����!");
			WRITEMSG
			goto ErrExit;
		}
	}
	***/
	/* ���Ӷ�ҵ��ί��ƾ֤�ļ�鲻����  added by martin 2007-11-13*/
	if((memcmp(payin.F_pkgno,"001",3)==0)&&(memcmp(g_pub_tx.note_type,"005",3)==0))
	{
		vtcp_log("��ʼƾ֤���� ac_no[%s]type[%s][%s][%s][%d]\n",g_pub_tx.ac_no,g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
		iRc=this_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);        
		if(iRc)
		{
			sprintf(acErrMsg,"����ƾ֤����!");
			WRITEMSG
			goto ErrExit;
		}
	}

	/*20110803 �������У����Ŵ�ϵͳ�޸� */
  char m_draw_no[25];
  memset(m_draw_no,'\0',sizeof(m_draw_no));
	get_zd_data("1188",m_draw_no);
	zip_space(m_draw_no);
	if(strlen(m_draw_no)>0){
		ret = sql_execute( "UPDATE draw_loan_hst SET brf1='LV%s%8d%.8s' WHERE draw_no ='%s' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,payin.T_orderno ,m_draw_no);/*�����������*/
		if( ret < 0 )
		{
        	sprintf( acErrMsg, "����draw_loan_hst�����!ret=[%d]",ret );
        	WRITEMSG
        	goto ErrExit;
		}
	}
	
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
/******************************************************************************
*                                                                             *
*   FUNCTION:          a10_Initial()                                         *
*                                                                             *
*   DESCRIPTION:       1.Initial all variables.                               *
******************************************************************************/
int ilv10_Initial()
{
	memset(&payin,'\0',sizeof(payin));
	pub_base_sysinit();
	/**************/
	/* Initial WD */
	/**************/
	/****���payin*********/
	if(iLvchkstat())
	{
		if(strlen(acErrMsg)!=0)
			set_zd_data("0130",acErrMsg);
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		return(-1);
	}
	if(iLvTisToPayin(&payin)){
		if(strlen(acErrMsg)!=0)
			set_zd_data("0130",acErrMsg);
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]\n", __FL__);
	return(0);	

}
/******************************************************************************
*                                                                             *
*   FUNCTION:                            lv10_Verify()                        *
*                                                                             *
*   DESCRIPTION:                         1.Initial all variables.             *
******************************************************************************/
int ilv10_Verify()
{
	int iRc;
	/****�������****/
vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
	iRc=iLvPayinCheck0(&payin);
	if(iRc){
		vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,iRc);
		if(strlen(acErrMsg)!=0)
			set_zd_data("0130",acErrMsg);
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
	/****Ҫ�ؼ��****/
vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	iRc=iLvPayinCheck4(&payin);
	if(iRc){
		vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,iRc);
		if(strlen(acErrMsg)!=0)
			set_zd_data("0130",acErrMsg);
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
vtcp_log("[%s][%d]\n", __FL__);
	/****�кż��****/
vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	iRc=iLvPayinCheck1(&payin);
	if(iRc){
		if(strlen(acErrMsg)!=0)
			set_zd_data("0130",acErrMsg);
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,iRc);
		return(-1);
	}
vtcp_log("[%s][%d]\n", __FL__);
vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	/****�����ʺż��****/
			
	return(0);	
}
/******************************************************************************
*   FUNCTION:          lv10_Process()                                         *
*   DESCRIPTION:  1.Initial PBMR record to opening stauts                     *
*                 2.Insert PBMR record/Delete(reversal)                       *
******************************************************************************/
int ilv10_Process()
{
	int iOrderno;
	int iRc;
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	/****������****/
	iOrderno=iLvGetOrderno();
	if(iOrderno<=0){
		vtcp_log("%s,%d,�õ�֧��������Ŵ���!\n",__FL__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("[%s][%d]\n", __FL__);
	apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);
	/***����������������һ���µļ�¼****/
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	iRc=iLvNewRecFromPayin(&payin);
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	if(iRc){
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("[%s][%d]\n", __FL__);
	/***�����¼�¼****/
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	iRc=iLvUpdRecFromPayin(&payin,1);
	if(iRc){
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	return(0);

}

/******************************************************************************
*  																			  *
*	FUNCTION:		   lv10_PreTos()										  *
*   DESCRIPTION:	   Ϊ���ذ���ֵ											  *
******************************************************************************/
ilv10_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin);
	return(0);
}


/**********************************************************************
 * ��  �� ����  this_com_NoteUse
 * ���� ����:   ƾ֤���Ŵ������׹�����ʹ��ƾ֤��
 *	      �������ͻ�ƾ֤�͹�Աƾ֤�����֣�����ƾ֤�������жϣ�������Գ���
 *		���ͻ��������ͻ�ƾ֤��������ɳ��۸��ͻ�������Աƾ֤���������
 *		����룬��ֱ�ӷ���0��������ƾ֤����
 * ����/ʱ�䣺  Martin /2007��11��8��
 * 
 * ��  ����
 *     ���룺      
 *        char     note_type[4]    ƾ֤����
 *        char     beg_no[17]      ��ʼ����
 *        char     end_no[17]      ��ֹ����
 *        char     tel[5]          ��Ա��
 *
 *     �����  char g_pub_tx.reply[]     ��Ӧ��
 *
 *   ����ֵ��  0: �ɹ�    -1: ���ɹ�
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int this_com_NoteUse(
	long	ac_id,
	int	ac_seqn,
    	char    *note_type,
    	char    *beg_no,
    	char    *end_no,
    	char    *tel)
{
	struct note_parm_c sNoteParm;
	
	int	ret;
	
	memset( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );
	
	/* ȡƾ֤������:����ƾ֤����ȡƾ֤�ṹ */
	ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type );
	if( ret == 100 )
	{
	    sprintf(acErrMsg,"ȡƾ֤����note_parm���!note_type=[%s]",note_type);
	    WRITEMSG
	    strcpy( g_pub_tx.reply, "N001" );
	    return (-1);
	}else if ( ret )
	{
	    sprintf( acErrMsg, "ִ��Note_parm_Sel����![%d]", ret );
	    WRITEMSG
	    return(-1);
	}	
	
	/* �ж��Ƿ�������������ֱ�ӷ�����ȷ */
	if ( sNoteParm.ctl_no_ind[0] == 'N' )
	{
		return(0);	/* ��������룬ֱ�ӷ�����ȷ0 */
	}

	
	sprintf( acErrMsg, "111111111111111sNoteParm.sell_ind=[%s]",sNoteParm.sell_ind);
	WRITEMSG
	sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
	WRITEMSG

	/* �ж��Ƿ����������ͻ� */
	if ( sNoteParm.sell_ind[0] == 'N' )	/* ��������۸��ͻ� */
	{
		sprintf( acErrMsg, "ƾ֤���ܳ��۸��ͻ�" );
	   		WRITEMSG
	   		return(-1);			
		
	}else if ( sNoteParm.sell_ind[0] == 'Y' )
   	{
   		ret = this_com_CheqClsNo( ac_id, ac_seqn, note_type, beg_no, end_no );
		if ( ret )
		{
	   		sprintf( acErrMsg, "���ͻ�ƾ֤��" );
	   		WRITEMSG
	   		return(-1);				
		}
   	}else
   	{
   		sprintf( acErrMsg, "ƾ֤������۱�־��sell_ind=[%s]",sNoteParm.sell_ind );
   		WRITEMSG
   		return(-1);
   	}
   	
   	return(0);
}

/**********************************************************************
 * ��  �� ����  this_com_CheqClsNo
 * ���� ����:   �ͻ�ƾ֤���Ŵ���
 * ����/ʱ�䣺  martin/2007��11��8��
 * 
 * ��  ����
 *     ���룺  	
 *       	long ac_id		�ʻ�ID
 *		int  ac_seqn		�ʻ����
 *              char *note_type         ƾ֤����
 *		char *cheq_beg_no	��ʼ����
 *		char *cheq_end_no	��ֹ����
 *
 *     �����  char g_pub_tx.reply[] 	��Ӧ��
 *
 *   ����ֵ��  0: �ɹ� 	��0: ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int this_com_CheqClsNo( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no )
	long	ac_id;
	int	ac_seqn;
	char	*note_type;
	char	*cheq_beg_no;
	char	*cheq_end_no;
{
	struct	note_cheq_mst_c	sCheqMst;	/* ��¼�ͻ�ƾ̨֤�� */
	struct	note_mst_c	sNoteMst;
	char	sts[51];	/* ֧Ʊ״̬ */
	long	tx_date;	/* �������� */
	char	tw_br_no[6];/* �Է������� */
   	char   	tw_tel[6];	/* �Է���Ա�� */
	int	ret;
	
	memset( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	
	/* �����ʼ���롢��ֹ�����������,����ͷ�Ƿ���ͬ */
	ret = pub_com_ChkBoeNote( note_type, cheq_beg_no, cheq_end_no);
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "���������,����ͷʧ��!ret=[%d]",ret );
		WRITEMSG
		return(-1);		
	}
	/*******************************/
	
	/* �����ʻ�ID���֧Ʊ�Ƿ�Ϊ���ʺ����� */
	ret=pub_com_ChkAcCheq( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no, sts );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "�ͻ��޴�ƾ֤��ƾ֤״̬��һ��!ac_id=[%d],\
		         note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",\
		         ac_id,note_type,cheq_beg_no, cheq_end_no );
		WRITEMSG
		return(-1);
	}

	if ( sts[0] == '0' )
	{
		sprintf( acErrMsg, "�ͻ��޴�ƾ֤!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N080" );
		return(-1);
	}else if ( sts[0] == '2' )
	{
		sprintf( acErrMsg, "��ƾ֤�ѹ�ʧ!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N081" );
		return(-1);
	}else if ( sts[0] == '3' )
	{
		sprintf( acErrMsg, "��ƾ֤�ѻ���!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N082" );
		return(-1);
	}else if ( sts[0] == '4' )
	{
		sprintf( acErrMsg, "��ƾ֤������!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N083" );
		return(-1);
	}else if ( sts[0] == '5' )
	{
		sprintf( acErrMsg, "��ƾ֤��ʹ��!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N084" );
		return(-1);
	}
	
	/*****************************/
	
	/* ����ƾ֤���� */
	ret = pub_com_CalNoteCnt( note_type,  cheq_beg_no, cheq_end_no );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "����ƾ֤������ret = [%d]", ret );	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N010" );
		return(-1);
	}
	
	return 0;
}

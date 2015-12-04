/***************************************************************
* �� �� ��:     spJ001.c
* ����������	���֧��ҵ��¼��
*
* ��    ��:   LiuHuafeng
* ������ڣ�  2006-8-9 9:28
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "find_debug.h"
#include "note_parm_c.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"

spJ001()
{
	int ret = 0;
	HV_PAYIN_AREA hv_payin;
	char cAc_no[25];
	char cKinbr[BRNO_LEN+1];
	char node0[10]; 
	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_payin, 0 , sizeof(hv_payin));
	memset(cAc_no, 0 , sizeof(cAc_no));
	memset(cKinbr, 0 ,sizeof(cKinbr));
	memset(node0, 0x0, sizeof(node0));
	
	get_zd_data(DC_TX_BR_NO,cKinbr);
	get_zd_data("0270",node0); 
	/***�õ����֧��ҵ��8583����***/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	/* ׼��д��Ǽǲ������� */
	ret=iHvFrontToTable(&hv_payin);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	/**��������������ֱ�ӽ�״̬��Ϊ����״̬ hzh 20130610**/
	if(memcmp( g_pub_tx.tx_code,"6389",sizeof( g_pub_tx.tx_code))==0 &&
		memcmp(g_pub_tx.sub_tx_code,"J001",sizeof(g_pub_tx.sub_tx_code))==0)
		{
		memcpy(hv_payin.F_opcd, "30", 2);
		hv_payin.T_hv_sts[0] = SEND_ACCT_CHECK;	
		vtcp_log("%s %d ����������¼��ʱ�Ͱ�״̬�³��˸���״̬g_pub_tx.tx_code=[%s],g_pub_tx.sub_tx_code=[%s],hv_payin.F_opcd=[%s],hv_payin.T_hv_sts=[%s]",__FILE__,__LINE__,g_pub_tx.tx_code,g_pub_tx.sub_tx_code,hv_payin.F_opcd,hv_payin.T_hv_sts);
		}
	/**��������������ֱ�ӽ�״̬��Ϊ����״̬ hzh 20130610**/
	/* ���Ƿ��ַ� */
	ret=iHvPayin_characterCheck(&hv_payin,acErrMsg);
	if(ret)
	{
		vtcp_log("%s,%d errmsg %s",__FILE__,__LINE__,acErrMsg);
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	ret=iHv_checkstat(hv_payin.F_cmtno,hv_payin.F_yw_type);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	/* д��Ǽǲ� */
	ret=iHvUpdZfRecFromPayin(&hv_payin,1);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	iHvResetPayin(&hv_payin);
	iHvReset8583(&hv_payin);
	/* д��ҵ����ˮ */
	vtcp_log("%s %d ac_no[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	vtcp_log("%s %d !!!svc_ind[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
/*
	g_pub_tx.svc_ind=0;
*/	
	ret=pub_ins_trace_log();
	if(ret)
	{
		strcpy( g_pub_tx.reply, "HV01");
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
        /* ���Ӷ�ҵ��ί��ƾ֤�Ĵ���  added by liuxuan 2007-3-31*/
        get_zd_data("1023",g_pub_tx.note_type);
        get_zd_data("1024",g_pub_tx.beg_note_no);
        get_zd_data("1024",g_pub_tx.end_note_no);
        get_zd_int("1022",&g_pub_tx.ac_seqn);
  	vtcp_log("���ҵ�������g_pub_tx.tx_br_no==[%s]HV_CZ_QSBRNO==[%s][%s][%d]\n",g_pub_tx.tx_br_no,HV_CZ_QSBRNO,__FILE__,__LINE__);
  /* del by martin 20071108 
  if( memcmp( g_pub_tx.tx_br_no,HV_CZ_QSBRNO,sizeof(g_pub_tx.tx_br_no)-1 ) )
  {
	  if (!((strcmp(hv_payin.F_cmtno,"100"))&&(strcmp(g_pub_tx.note_type,"005"))))
	  {
          	vtcp_log("��ʼƾ֤���� type[%s][%s][%s][%d]\n",g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
	  	ret=pub_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);
	  	if(ret)
	  	{
	  		sprintf(acErrMsg,"����ƾ֤����!");
	  		WRITEMSG
	  		goto ErrExit;
	  	}
	  }
  }*/
  /* ���Ӷ�ҵ��ί��ƾ֤�ļ�鲻����  added by martin 2007-11-8*/
  if( memcmp( g_pub_tx.tx_br_no,HV_CZ_QSBRNO,sizeof(g_pub_tx.tx_br_no)-1 ) )
  {
	  if ( (memcmp(hv_payin.F_cmtno,"100",3)==0) && (memcmp(g_pub_tx.note_type,"005",3)==0) )
	  {
          	vtcp_log("��ʼƾ֤���� type[%s][%s][%s][%d]\n",g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
	  	ret=this_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);
	  	if(ret)
	  	{
	  		sprintf(acErrMsg,"����ƾ֤����!");
	  		WRITEMSG
	  		goto ErrExit;
	  	}
	  }
  }
  /*20110803 �������У����Ŵ�ϵͳ�޸� */
  char m_draw_no[25];
  memset(m_draw_no,'\0',sizeof(m_draw_no));
  sprintf(acErrMsg,"����5801���װɡ�");
	WRITEMSG
	get_zd_data("1188",m_draw_no);
	zip_space(m_draw_no);
	vtcp_log("[%s][%d],draw_no=[%s]",__FILE__,__LINE__,m_draw_no);
	if(strlen(m_draw_no)>0){
		ret = sql_execute( "UPDATE draw_loan_hst SET brf1='HV%s%8d%.8s' WHERE draw_no ='%s' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,hv_payin.F_orderno ,m_draw_no);/*�����������*/
		if( ret < 0 )
		{
        	sprintf( acErrMsg, "����draw_loan_hst�����!ret=[%d]",ret );
        	WRITEMSG
        	goto ErrExit;
		}
	}
	/**��������������õ���������������ʺ� 20130605 hzh**/
	 if(memcmp(node0, "nbanklvhv", 9) == 0) 
		{
				ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
			if(ret)
			{
				sprintf(acErrMsg,"�õ���������������ʺŴ���!\n");
				WRITEMSG
				vtcp_log("%s,%d �õ���������������ʺŴ���\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P073");
				goto ErrExit;
			}
				set_zd_data("0310",cAc_no);
				vtcp_log("[%s][%d],ȡ�ñ�������������ʺ�=[%s]",__FILE__,__LINE__,cAc_no);
		}
	/**��������������õ���������������ʺ� 20130605 hzh**/
	/*** ������� ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("112A",g_pub_tx.ac_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	}
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
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
	
	MEMSET_DEBUG( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );
	
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
	
	MEMSET_DEBUG( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
	MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	
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

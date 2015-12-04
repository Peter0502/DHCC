/*************************************************
* �� �� ��: pubf_ln_AcCheck.c
* ����������   
*              pub_ln_AcCheck     ���ݻ�����ʾ�˻������Ӧ״̬�����ؿͻ���
*
* ��    ��:    rob
* ������ڣ�   2004��01��11��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"

/**********************************************************************
* �� �� ����  pub_ln_AcCheck
* �������ܣ�  ��������Ļ�����ʾ�˺Ų�ѯ�����״̬��
*             ���ݲ�ѯ�ó����˻�ID���˻���ż��������ļ����鿴�˻�״̬
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��01��11��
*
* ��    ���� 
*     ��  �룺ac_no      char(19)  ��ʾ�˺�    
*      	      ac_seqn   integer	  �˻���� 
*     ��  ��: cif_no    long      �ͻ���
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/

int pub_ln_AcCheck( char *ac_no,int ac_seqn,long *cif_no,long *ac_id )
/*
int pub_ln_AcCheck( char *ac_no,int ac_seqn,long *cif_no )
*/
{	
	int ret=0;
	
	sprintf(acErrMsg,"����pub_ln_AcCheck����!");
	WRITEMSG	
	
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , 
					" ac_no='%s' " , ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�������˻���ϵ��Ϣ���޴˿��Ż��˺ţ�����![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L014");
		goto ErrExit;
	}else if( ret )
	{
		sprintf(acErrMsg,"��ȡ�������˻���ϵ��Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L015");
		goto ErrExit;
	}
	sprintf(acErrMsg,"��������Ļ�����ʾ�˺Ų�ѯ�����״̬ PASS!");
	WRITEMSG
		
	if( g_pub_tx.tx_date>g_mdm_ac_rel.end_date )
	{
		sprintf(acErrMsg,"�ý����Ѿ����ڣ�������!");
		WRITEMSG
		/**
		strcpy(g_pub_tx.reply,"P148");
		goto ErrExit;
		**/
	}
	sprintf(acErrMsg,"�ý����Ѿ����ڣ������� PASS!");
	WRITEMSG
		
	if ( strcmp( g_mdm_ac_rel.note_sts , "0" ))
	{
		sprintf(acErrMsg,"�ý��ʷ�����״̬������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L016");
		goto ErrExit;
	}
	sprintf(acErrMsg,"�ý����Ѿ����ڣ������� PASS!");
	WRITEMSG
				
	/* ��� ���ڻ����� ��״̬ */
	sprintf(acErrMsg,"�ý��ʷ�����״̬������[%ld][%d] PASS!",g_mdm_ac_rel.ac_id , ac_seqn);
	WRITEMSG
			
	ret = Dd_mst_Sel( g_pub_tx.reply , &g_dd_mst , 
					" ac_id=%ld and ac_seqn=%d " , 
					g_mdm_ac_rel.ac_id , ac_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�������ļ����޴˿��Ż��˺ţ�����![%s]" , 
					g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L017");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"��ȡ�������ļ���Ϣ�쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}	
	sprintf(acErrMsg,"��ȡ�������ļ� PASS!");
	WRITEMSG
		
	if ( strcmp( g_dd_mst.ac_sts , "1" ))
	{
		sprintf(acErrMsg,"�ÿ�/�ʺŴ��ڷ�����״̬������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L018");
		goto ErrExit;
	}	
	sprintf(acErrMsg,"�ÿ�/�ʺŴ�������״̬ PASS!");
	WRITEMSG

	sprintf(acErrMsg, "ȡ���Ŀͻ���Ϊ[%ld] ", g_dd_mst.cif_no );
	WRITEMSG
		
	*cif_no = g_dd_mst.cif_no;
	TRACE	
	
	*ac_id = g_dd_mst.ac_id;
	TRACE
	sprintf(acErrMsg, "���ȡ���Ŀͻ���Ϊ[%ld][%ld] ", *cif_no,*ac_id );
	WRITEMSG
	TRACE
	sprintf(acErrMsg, "���ȡ���Ŀͻ���Ϊ[%ld]", *cif_no );	
	WRITEMSG
		
	return 0;
	
ErrExit:
	return 1; 
}

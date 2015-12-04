/***************************************************************
* �� �� ��:     pubf_acct_check_tctd.c
* ��������: 	����ʻ��Ƿ�ͨͨ��ͨ��ҵ��
*
* ��    ��:     wl
* �������:     2007��3��31��
*
* �޸ļ�¼:
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "tctd_ktzh_c.h"

int pub_acct_check_tctd(char * ac_no)
{
	int ret = 0;
	struct tctd_ktzh_c    g_tctd_ktzh;
	
	memset(&g_tctd_ktzh,0x00,sizeof(struct tctd_ktzh_c));
	
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s' ",ac_no);
	if ( ret == 100) 
	{
		sprintf(acErrMsg,"�ʺ��ڽ����˺Ŷ��ձ�����! [%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"���ҽ����˺Ŷ��ձ����! [%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	vtcp_log("%s %d ",__FILE__,__LINE__);
	
	ret=Tctd_ktzh_Sel(g_pub_tx.reply,&g_tctd_ktzh,"tctd_acno = %s", ac_no);
	if ( ret == 100) 
	{
		sprintf(acErrMsg,"�ʺ���ͨ��ͨ��Э����в�����! [%s]",ac_no);
		strcpy(g_pub_tx.reply,"0000");
		WRITEMSG
		return 0; /****���˱���û�У�����ʺ�û�еǼ�Э��****/
	}
	else if(ret)
	{
		sprintf(acErrMsg,"���ҽ����˺Ŷ��ձ����! [%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		return -1;
	}
	
	if(g_tctd_ktzh.tc_kt_flag[0] == '1')/****ֻ��ͨͨ��****/
	{
		return 1;
	}
	else if(g_tctd_ktzh.td_kt_flag[0] == '1')/****ֻ��ͨͨ��****/
	{
		return 2;
	}
	else if(g_tctd_ktzh.cx_kt_flag[0] == '1')/****ֻ��ͨ��ѯ****/
	{
		return 3;
	}
	else if(g_tctd_ktzh.tc_kt_flag[0] == '1' && g_tctd_ktzh.td_kt_flag[0] == '1')
	{/****ͨ��ͨ�Ҷ���ͨ****/
		return 4;
	}
	else if(g_tctd_ktzh.tc_kt_flag[0] == '1' && g_tctd_ktzh.td_kt_flag[0] == '1' && g_tctd_ktzh.cx_kt_flag[0] == '1')
	{/****ͨ��ͨ�Ҳ�ѯ����ͨ****/
		return 5;
	}
	else
	{
		return 0;
	}	
	
	ErrExit:
		return -1;
}
/**********************************************************************
* �� �� ����  pub_dd_dddis_file
* �������ܣ�  ���ݴ����ʾ�˺ź��˺����ȡ������ļ�
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��12��30��
*
* ��    ���� 
*     ��  �룺  ac_no       char(19)  ��ʾ�˺�  	
*               ac_seqn     smallint  �˺����	       
*
*     ��  ��:                         �����ʺŶ����ļ��ṹ
*                                     �������ļ��ṹ
*                                     ��������ļ��ṹ
*                           char(4)   ��Ӧ��
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_dd_dddis_file(char *ac_no,int ac_seqn , 
struct mdm_ac_rel_c *mdm_ac_rel,struct dd_mst_c *dd_mst,
struct dd_parm_c *dd_parm )
{
	int ret = 0 ;
	
	/* ������ʾ�˺ţ���ƾ֤(����)���˻���ϵmdm_ac_rel���в�ѯ��ȡ���˻�ID */
	sprintf( acErrMsg,"!!!![%s]",ac_no);
	WRITEMSG
	sprintf( acErrMsg,"!!!![%s]",ac_no);
	WRITEMSG
	    
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , mdm_ac_rel, "ac_no='%s'", ac_no );
	if ( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼[%d][%s]",ret,ac_no);
		WRITEMSG
		    strcpy( g_pub_tx.reply ,"O001" );
		return 1;
	}
	else if ( ret )
	{
		sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣[%d][%s]",ret,ac_no);
		WRITEMSG
		    strcpy( g_pub_tx.reply ,"D103" );
		return -1;
	}

	if( mdm_ac_rel->ac_seqn ==9999)
	{
	sprintf( acErrMsg,"���˻�������ת��mdm[%d]input[%d]",
	ac_seqn,mdm_ac_rel->ac_seqn);
	WRITEMSG
	strcpy( g_pub_tx.reply ,"D033" );
	return -1;
	}
	sprintf( acErrMsg,"������ 1111�˺�=[%s],ID==[%d],seqn==[%d]",ac_no,mdm_ac_rel->ac_id,ac_seqn);
	WRITEMSG
	/* ȡ�������ļ���Ϣ */
	ret = Dd_mst_Sel(g_pub_tx.reply, dd_mst , "ac_id=%ld and ac_seqn=%d" ,
	    mdm_ac_rel->ac_id,ac_seqn );
	if ( ret==100 )
	{
		sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		sprintf( acErrMsg,"mdm_ac_rel->ac_id=[%ld]ac_seqn=[%d]",mdm_ac_rel->ac_id,ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;
		
	}
	else if ( ret )
	{
		sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
		return -1;
	}
	sprintf( acErrMsg,"������ 111111=[%s]",dd_mst->prdt_no);
	WRITEMSG
	/* ȡ���������Ϣ */
	ret = Dd_parm_Sel(g_pub_tx.reply, dd_parm , " prdt_no='%s' " ,
	    dd_mst->prdt_no );
	if ( ret==100 )
	{
		sprintf(acErrMsg,"��������ļ����޴˼�¼ [%s][%d]",dd_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}
	else if ( ret )
	{
		sprintf( acErrMsg,"ȡ��������ļ��쳣 [%s][%d]",dd_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return -1;
	}
	return 0 ;
}


/*************************************************
* �� �� ��:    sp9873.c
* ����������   
*              �����ʺźʹ�����ֻ��������Ϣ
*
* ��    ��:    lance
* ������ڣ�   2003��03��25��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"

sp9873()  
{ 		
	int ret=0,ac_seqn,ln_ac_seqn;
	char title[40],cur_no[3];
	char ac_no[20],ln_ac_no[20],name[50];
	double bal;

	struct prdt_ac_id_c g_prdt_ac_id;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("0310",ac_no);							/*�ʺ�*/
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/	
	
	get_zd_data("0370",ln_ac_no);							/*�����˺�*/
	get_zd_int("0340",&ln_ac_seqn);						/*�������*/	

	/* ���ø��ݴ�����ʾ�˺ź��˺����ȡ�������ļ�����*/
	ret = pub_ln_lndis_file(ln_ac_no,ln_ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}
	/* ���ݱ��������˻���� */
	ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &ac_seqn );
	if ( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			ERR_DEAL;	
		}
	sprintf(acErrMsg,"���ݱ��������˻���� PASS![%s]",cur_no);
	WRITEMSG


	
	/* �����ʺź��˻����ȡ��Ϣ */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
	    sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
	   	sprintf( acErrMsg, "��ѯ�����ʺű��쳣!!ret=[%d]", ret );
	   	WRITEMSG
	   	goto ErrExit;
	}

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,
			   			"ac_id=%ld and ac_seqn=%d",
					   	g_mdm_ac_rel.ac_id,ac_seqn);
   	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
		 		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}else if( ret ){
		 sprintf( acErrMsg, "��ѯ��Ʒ�˻����쳣!!ret=[%d]", ret );
		 WRITEMSG
		 goto ErrExit;
 	}
 	sprintf( acErrMsg, "ac_id_type=[%s]", g_prdt_ac_id.ac_id_type );
	WRITEMSG	
	if(g_prdt_ac_id.ac_id_type[0]!='1')
	{
		 sprintf( acErrMsg, "����˺ŷǻ����˺�,����������" );
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"L078");
		 goto ErrExit;
	}	

	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
			"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
				g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",g_prdt_ac_id.prdt_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸò�Ʒ���!!prdt_no=[%ld]",g_prdt_ac_id.prdt_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P145");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"��ѯ���ڲ������쳣!!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	strcpy(name,g_dd_mst.name);

	/* ������� */
	set_zd_data("1166",name);
	set_zd_int("0350" , ac_seqn);
	set_zd_data("0310",ac_no);	/** ������ʺ� **/
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
 

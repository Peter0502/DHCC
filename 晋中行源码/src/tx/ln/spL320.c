/***************************************************************
* �� �� ��: spL320.c
* �����������Ĵ������
* ��    ��: mike
* ������ڣ�2003��3��9��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spL320()
{   
	int ret; 
	char ac_no[20],ac_type[2],intst_type[2];
	char name[51];
	long tmp_date1,tmp_date2;
	double tmp_amt=0,intst_acm=0;

	struct ln_mst_c     g_ln_mst;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);          	/* �˺� */
	get_zd_int("0340",&g_pub_tx.ac_seqn);  			/* �˻���� */
	get_zd_double("0560",&intst_acm);  			    /* ������Ļ��� */

	/*�����˺Ų����˺�ID*/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'", \
						 g_pub_tx.ac_no);
  	if( ret==100 )
  	{
	   sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"W010");
	   goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	
				ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret )
				{
	 				sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				
				ret = Ln_mst_Fet_Upd(&g_ln_mst,g_pub_tx.reply);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"�����ļ����޴˼�¼");
	  				WRITEMSG
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
					WRITEMSG
					goto ErrExit;
				}
				if(g_ln_mst.ac_sts[0]=='*'){
					sprintf(acErrMsg,"�ʻ�״̬��[%s]",g_ln_mst.ac_sts);
					WRITEMSG
					strcpy(g_pub_tx.reply,"L016");
					goto ErrExit;
				}
				g_pub_tx.ac_id=g_ln_mst.ac_id;
				g_pub_tx.tx_amt1=intst_acm;
				g_ln_mst.intst_acm=intst_acm;
				vtcp_log("�»���[%.2f]",g_ln_mst.intst_acm);
				g_ln_mst.lst_date=g_pub_tx.tx_date;

				ret = Ln_mst_Upd_Upd( g_ln_mst,g_pub_tx.reply );
				if( ret )
				{
	 				sprintf(acErrMsg,"Update Error!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				Ln_mst_Clo_Upd();

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������������ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�����������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

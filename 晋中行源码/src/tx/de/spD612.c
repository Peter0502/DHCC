/***************************************************************
* �� �� ��: spD612.c
* ����������Э�������ʵ���
* ��    ��: wanglei
* ������ڣ�2007��7��2��
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
#include "mo_infrm_c.h"
#include "prdt_ac_id_c.h"

spD612()
{   
	int ret; 
	char ac_no[20],ac_type[2],intst_type[2];
	char name[51];
	char cFlt_ratio[9];                     /****��������****/
	char cRate[9];
	char cFlag[2];
	long tmp_date1,tmp_date2;
	double tmp_amt=0,intst_acm=0;

	struct mo_infrm_c	g_mo_infrm;
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct dd_mst_c     g_dd_mst;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_mo_infrm,0x00,sizeof(struct mo_infrm_c));
	
	memset(cFlt_ratio,0x00,sizeof(cFlt_ratio));
	memset(cRate,0x00,sizeof(cRate));
	memset(cFlag,0x00,sizeof(cFlag));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);          	/* �˺� */
	get_zd_int("0340",&g_pub_tx.ac_seqn);  			/* �˻���� */
	get_zd_data("0290",cFlt_ratio);  		    /* �������� */
	vtcp_log("[%s][%d]  flt_ratio = %.2f",__FILE__,__LINE__,atof(cFlt_ratio));
	get_zd_data("0670",cFlag);
	get_zd_data("0280",cRate);
vtcp_log("[%s][%d]  cRate = %.2f",__FILE__,__LINE__,atof(cRate));
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
	 /***  ***/
	 if(strcmp(g_mdm_ac_rel.opn_br_no, g_pub_tx.tx_br_no) != 0)
	 {
	 	   sprintf(acErrMsg,"��Ҫ�������а����ҵ��");
			 WRITEMSG
			 strcpy(g_pub_tx.reply,"O171");
			 goto ErrExit;
	 }
	 /***  ***/
				ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret )
				{
	 				sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				
				ret = Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"�����ļ����޴˼�¼");
	  				WRITEMSG
	    			strcpy(g_pub_tx.reply,"P172");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_pub_tx.ac_id=g_dd_mst.ac_id;
				g_pub_tx.tx_amt1=atof(cFlt_ratio);
				/**** ���ڲ������ ���Բ���lst_date***/
				/**g_dd_mst.lst_date=g_pub_tx.tx_date;**/
				g_dd_mst.flt_ratio=atof(cFlt_ratio);      /****���¸�������  20070628****/
				if(memcmp(g_pub_tx.tx_code,"4371",4) == 0)
				{
					if(pub_base_CompDblVal(g_dd_mst.flt_ratio,-1.00) == 0)
					{
						g_dd_mst.intst_type[0] = '0';
					}else{
						g_dd_mst.intst_type[0] = '4';
					}
				}else{
					if( (1+g_dd_mst.flt_ratio)*g_dd_mst.rate<0.00001)
					{
						sprintf(acErrMsg,"Update Error!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"A005");
						goto ErrExit;
					}
				}	
				/** 3512���׵����ֻ�����  **/
				if(cFlag[0]=='1')
				{
					g_dd_mst.rate=atof(cRate);
				}
				ret = Dd_mst_Upd_Upd( g_dd_mst,g_pub_tx.reply );
				if( ret )
				{
	 				sprintf(acErrMsg,"Update Error!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				Dd_mst_Clo_Upd();
				/** 3512���׵����ֻ�����  **/
			
	g_pub_tx.hst_ind[0]='1';
	strcpy(g_pub_tx.brf,"������");
	/*** ������Ҫ�󲻵Ǽǽ�����ϸ 20121106 wudw
	if(pub_ins_dd_hst())     
		goto ErrExit;
	****/
	
    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�Թ����ʵ����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�Թ����ʵ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

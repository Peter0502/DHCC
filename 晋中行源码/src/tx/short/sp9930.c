/*************************************************
* �� �� ��: sp9930.c
* ���������������ʺţ��ʺ���Ż��������Ϣ[���߼��ʽ���][���ܼ���]
*
* ��    ��: 
* ������ڣ�2003��06��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#include <math.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "cash_mst_c.h"

sp9930()  
{ 		
	char	trance_type[2];
	int i=0,ret=0;

	struct  prdt_ac_id_c   sPrdt_ac_id;
	struct  com_item_c   g_com_item;
	struct  gl_mst_c     g_gl_mst;
	struct  cash_mst_c   sCash_mst;

	memset(&g_com_item,0x00,sizeof(struct com_item_c));
	memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
	memset(&sCash_mst,0x00,sizeof(struct cash_mst_c));
	memset(trance_type,0x00,sizeof(trance_type));

	/** ȡֵ����ֵ **/	
	get_zd_data("0680",trance_type);						/*��������*/
	vtcp_log(" [%s][%d]trance_type is [%s]\n",__FILE__,__LINE__,trance_type);
	if( trance_type[0]=='7' )	/* ��ԱǮ�� */
	{
		get_zd_data("0210",g_pub_tx.cur_no);				/*����*/
		get_zd_data("0070",g_pub_tx.tel);					/*����Ա*/
		get_zd_data("0030",g_pub_tx.tx_br_no);				/*������*/
		ret=Cash_mst_Sel(g_pub_tx.reply,&sCash_mst," br_no='%s' and tel='%s' and cur_no='%s' ",g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.cur_no);
		if( ret==100 )
		{
		 	sprintf(acErrMsg,"�ù�Աû�д˱��ֵ�Ǯ��[%s][%s][%s]",g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.cur_no);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P414");
		 	goto ErrExit;
		}
		if( ret )
		{
		 	sprintf(acErrMsg,"ȡ��ԱǮ�����[%s][%s][%s]",g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.cur_no);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P153");
		 	goto ErrExit;
		}
		set_zd_data("0220","��");			
		sCash_mst.bal=floor(sCash_mst.bal);
		set_zd_double("0410",sCash_mst.bal);			
		set_zd_data("0250","Ǯ��");       			/*����*/
		set_zd_data("0310",sCash_mst.ac_no);
		goto OkExit;
	}
	else
	{
		get_zd_data("0300",g_pub_tx.ac_no);					/*�ʺ�*/
		pub_base_old_acno( g_pub_tx.ac_no );
	}

	i=strlen(g_pub_tx.ac_no); /***����ac_no�ĳ������ж��Ƿ��ǿ�Ŀ***/
	if(i==3||i==5||i==7)							/*�����Ŀ*/
	{
		if( trance_type[0]!='6' )
	 	{
		 	sprintf(acErrMsg,"��ѡ����ڼ���!!trance_type=[%s]",trance_type);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P405");
		 	goto ErrExit;
	 	}
	 	/*���ݿ�Ŀ�Ų�ѯ��Ŀ����*/
	 	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",
					  	 g_pub_tx.ac_no);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����!!acc_hrt=[%s]", g_in_parm.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O074");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf( acErrMsg, "��ѯ������Ŀ���쳣!!ret=[%d]", ret );
		 	WRITEMSG
		 	goto ErrExit;
	 	}
		if( g_com_item.acc_knd[0]!='3' )
		{
		 	sprintf(acErrMsg,"�ÿ�Ŀ�����˻������ӿ�Ŀ!!acc_knd=[%s]",g_com_item.acc_knd);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P406");
		 	goto ErrExit;
		}

	 	/*���ݿ�Ŀ�Ų�ѯ���������*/
	 	ret=Gl_mst_Sel(g_pub_tx.reply,&g_gl_mst,"acc_hrt='%s'",
					   g_com_item.acc_hrt);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����!!acc_hrt=[%s]",g_com_item.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O075");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"��ѯ������Ŀ���쳣!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}

		/*��Ŀ�������*/
		if(g_com_item.dc_ind[0]=='1')
		{
			set_zd_data("0220","��");			
			set_zd_double("0410",g_gl_mst.dr_bal);			
		}
		else if(g_com_item.dc_ind[0]=='2')
		{
			set_zd_data("0220","��");			
			set_zd_double("0410",g_gl_mst.cr_bal);			
		}
		else if(g_com_item.dc_ind[0]=='0')		/*˫��*/
		{										
			ret=pub_base_CompDblVal(g_gl_mst.dr_bal,g_gl_mst.cr_bal);
			if(ret==1)
			{
				set_zd_data("0220","��");			
				set_zd_double("0410",g_gl_mst.dr_bal);			
			}
			else if(ret==-1)
			{
				set_zd_data("0220","��");
				set_zd_double("0410",g_gl_mst.cr_bal);			
			}
		}
		set_zd_data("0250",g_com_item.acc_name);       /*����*/
		set_zd_data("0210",g_gl_mst.cur_no);           /*����*/
	}
	else					/*����ǿ�Ŀ�ʺ�*/
	{
		/*�����ʺ����˻�ID����� */
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
		if( ret==100 )
		{
	 		sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%ld]",g_pub_tx.ac_no);
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

		g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn;
		
		/****����Ϊ����Ļ�������ر�־����ʾ��Ա add by wanglei 20070621****/
		ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"repay_ac_id = %ld and ac_sts in('1','2','3','4','5') \
			and (bal >0 or in_lo_intst >0 or out_lo_intst >0 or cmpd_lo_intst >0) ", g_mdm_ac_rel.ac_id);
		if(ret == 0)
		{
			sprintf(acErrMsg,"������ʺ����б������Ϣδ��!!");
			WRITEMSG
			set_zd_data("0710","1"); /****������ʾ��־****/
		}
		/****ended 20070621****/

		/*** ȡ��Ʒ���� ***/
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%d and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
		if( ret==100 )
		{
	 		sprintf(acErrMsg,"ȡprdt_ac_id�����!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 		WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}
		else if( ret )
		{
	 		sprintf(acErrMsg,"ȡprdt_ac_id�����!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
			WRITEMSG
			goto ErrExit;
		}
				
		if( sPrdt_ac_id.ac_id_type[0]=='1' )	/*** ���� ***/	
		{
			/*	���ھ��ʺ�ǰ̨û���ж�����ֱ�ӴӺ�̨��ֵ 
			if( trance_type[0]!='1' )
			{
				sprintf(acErrMsg,"�����˻�����ѡ����ڽ�������!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P409");
				goto ErrExit;
			}
			*/
			trance_type[0]=sPrdt_ac_id.ac_id_type[0];

			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
					 	g_dd_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"�����ڲ�Ʒ��!!prdt_no=[%s]",g_dd_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"�����ڸò�Ʒ��!!prdt_no=[%s],ret=[%d]",g_dd_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}
			if( g_dd_parm.cif_type[0]=='1' )
			{
				sprintf(acErrMsg,"������Թ��˻�!!cif_type=[%s]",g_dd_parm.cif_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O165");
				goto ErrExit;
			}
			/* ������� */
			set_zd_data("0220","��");
			set_zd_data("0250",g_dd_mst.name);				/*����*/
			set_zd_data("0210",g_dd_parm.cur_no);			/*����*/
			set_zd_double("0410",g_dd_mst.bal);				/*���*/
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='2' )	/*** ���� ***/	
		{
			sprintf(acErrMsg,"�����ʺŲ������˽���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P403");
			goto ErrExit;

			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",
					 	g_td_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"�����ڲ�Ʒ��!!prdt_no=[%s]",g_td_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"�����ڸò�Ʒ��!!prdt_no=[%s],ret=[%d]",g_td_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}
			/* ������� */
			set_zd_data("0220","��");
			set_zd_data("0250",g_td_mst.name);				/*����*/
			set_zd_data("0210",g_td_parm.cur_no);			/*����*/
			set_zd_double("0410",g_td_mst.bal);				/*���*/
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='3' )	/*** ���� ***/	
		{
			ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",
					 	g_ln_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"�����ڲ�Ʒ��!!prdt_no=[%s]",g_ln_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"�����ڸò�Ʒ��!!prdt_no=[%s],ret=[%d]",g_ln_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}
			/* ������� */
			set_zd_data("0220","��");
			set_zd_data("0250",g_ln_mst.name);				/*����*/
			set_zd_data("0210",g_ln_parm.cur_no);			/*����*/
			if( trance_type[0]=='2' )
				set_zd_double("0410",g_ln_mst.bal);				/*�������*/
			else if( trance_type[0]=='3' )
				set_zd_double("0410",g_ln_mst.in_lo_intst);		/*�������*/
			else if( trance_type[0]=='4' )
				set_zd_double("0410",g_ln_mst.out_lo_intst);	/*�������*/
			else if( trance_type[0]=='5' )
				set_zd_double("0410",g_ln_mst.cmpd_lo_intst);	/*�������*/
			else
			{
				sprintf(acErrMsg,"�����˻�����ѡ����������!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P407");
				goto ErrExit;
			}
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='4' )	/*** ͸֧ ***/	
		{
			ret=Od_mst_Sel(g_pub_tx.reply,&g_od_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'",
					 	g_od_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"�����ڲ�Ʒ��!!prdt_no=[%s]",g_od_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"�����ڸò�Ʒ��!!prdt_no=[%s],ret=[%d]",g_od_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}


			/* ������� */
			set_zd_data("0220","��");
			set_zd_data("0210",g_in_parm.cur_no);			/*����*/
			set_zd_double("0410",g_in_mst.bal);				/*���*/

			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",
						g_od_mst.pay_ac_id,g_od_mst.pay_ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld],ac_seqn=[%d]",g_od_mst.pay_ac_id,g_od_mst.pay_ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]",g_od_mst.pay_ac_id,g_od_mst.pay_ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}
			set_zd_data("0250",g_dd_mst.name);				/*����*/
		}
		else
		{
			if( trance_type[0]!='6' )
			{
				sprintf(acErrMsg,"�ڲ����ʻ�����ѡ���ڲ��ʽ�������!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P408");
				goto ErrExit;
			}

			ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "��ѯ�ڲ��ʻ����ļ��쳣!!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s'",
					 	g_in_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"�����ڸò�Ʒ��!!prdt_no=[%ld]",g_in_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "��ѯ�ڲ��ʻ��������쳣!!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			/*���ݿ�Ŀ�Ų�ѯ���������*/
			ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_hrt='%s'",
						 g_in_parm.acc_hrt);
	    	if( ret==100 )
	    	{
		   		sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����!!acc_hrt=[%s]",
		   			g_in_parm.acc_hrt);
		   		WRITEMSG
		   		strcpy(g_pub_tx.reply,"O074");
		   		goto ErrExit;
			}
			else if( ret )
			{
		   		sprintf( acErrMsg, "��ѯ�ڲ��ʻ��������쳣!!ret=[%d]", ret );
		   		WRITEMSG
		   		goto ErrExit;
			}

			/*����*/
			if(g_com_item.dc_ind[0]=='1')
				set_zd_data("0220","��");
			else if(g_com_item.dc_ind[0]=='2')
				set_zd_data("0220","��");
			else if(g_com_item.dc_ind[0]=='0')      /*˫��*/
			{
				ret=pub_base_CompDblVal(g_in_mst.bal,0.00);
				if(ret==1)
			   		set_zd_data("0220","��");
				else 
			   		set_zd_data("0220","��");
	  		}

			/* ������� */
			set_zd_data("0250",g_in_mst.name);				/*����*/
			set_zd_data("0210",g_in_parm.cur_no);			/*����*/
			set_zd_double("0410",g_in_mst.bal);				/*���*/
		}
		set_zd_data("0300",g_pub_tx.ac_no);
	}
	/* ǰ̨������ʾ��ֵ���Ը�ֵ����ʾ��0����ȥ */
	double tmp_bal=0;
	get_zd_double("0410",&tmp_bal);
	if(tmp_bal<0){
		vtcp_log("!!!!!!!С��0[%.2f]\n",tmp_bal);
		set_zd_double("0410",0);	
	}

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
   

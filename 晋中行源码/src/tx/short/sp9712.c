/*************************************************
* �� �� ��:    sp9712.c
* ����������   
*              �����ʺŻ��������Ϣ[ת�ʽ�����ʽ���--��]
*
* ��    ��:    rob
* ������ڣ�   2003��02��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "gl_mst_c.h"

sp9712()  
{ 		
	int i=0,ret=0,ac_seqn;
	char ac_no[20];

	struct  com_item_c   g_com_item;
	struct  gl_mst_c   	 g_gl_mst;

	memset(&g_com_item,0x00,sizeof(struct com_item_c));
	memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("1201",ac_no);						/*�ʺ�*/
	pub_base_old_acno(ac_no);
  vtcp_log("[%s][%d]xyy ac_no====[%s]",__FILE__,__LINE__,ac_no);
	i=strlen(ac_no);
	if(i==3||i==5||i==7)							/*�����Ŀ*/
	{
	 	/*���ݿ�Ŀ�Ų�ѯ��Ŀ����*/
	 	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",
					  	 ac_no);
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
			set_zd_data("120B","��");			
			set_zd_double("1203",g_gl_mst.dr_bal);			
		}
		else if(g_com_item.dc_ind[0]=='2')
		{
			set_zd_data("120B","��");			
			set_zd_double("1203",g_gl_mst.cr_bal);			
		}
		else if(g_com_item.dc_ind[0]=='0')		/*˫��*/
		{										
			ret=pub_base_CompDblVal(g_gl_mst.dr_bal,g_gl_mst.cr_bal);
			if(ret==1)
			{
				set_zd_data("120B","��");			
				set_zd_double("1203",g_gl_mst.dr_bal);			
			}
			else if(ret==-1)
			{
				set_zd_data("120B","��");
				set_zd_double("1203",g_gl_mst.cr_bal);			
			}
		}
		set_zd_data("1202",g_com_item.acc_name);       /*����*/
		set_zd_data("1204",g_gl_mst.cur_no);           /*����*/
	}
	else												/*�����ڲ��ʺ�*/
	{
		/*�����ʺ����˻�ID����� */
	 	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%ld]",ac_no);
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

	 	ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	 	if( ret==100 )
	 	{
			sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%ld]",ac_no);
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

		/*���ݲ�Ʒ���ҵ���Ŀ��*/
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
			set_zd_data("120B","��");
		else if(g_com_item.dc_ind[0]=='2')
			set_zd_data("120B","��");
		else if(g_com_item.dc_ind[0]=='0')      /*˫��*/
		{
			ret=pub_base_CompDblVal(g_in_mst.bal,0.00);
			if(ret==1)
		   		set_zd_data("120B","��");
		   	else
			  	set_zd_data("120B","��");
		}

		/* ������� */
		set_zd_data("1202",g_in_mst.name);              /*����*/
		set_zd_data("1204",g_in_parm.cur_no);           /*����*/
		set_zd_double("1203",g_in_mst.bal);             /*���*/
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
   

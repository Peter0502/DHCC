#include "find_debug.h" 
/*************************************************
* �� �� ��: pubf_base_getacname.c
* ����������1��ͨ��������ʺ�ȡ����
*  			2��
*        	3��
*        	4��
*        	5��
* ��    ��: dong   
* ������ڣ�2003��12��16��
* �޸ļ�¼��    
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*************************************************/

#include "stdio.h"
#include <varargs.h>
#include <sys/stat.h>
#include <sys/types.h>

#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "prdt_ac_id_c.h"
#include "gl_mst_c.h"

#define loc_t sys_loc_t

/**********************************************************************
* ��������    pub_base_Getacname
* �������ܣ�  ͨ���ʺ�ȡ����
* ����/ʱ�䣺    
* 
* ������
*     ���룺�ʺ�
*           
*     ��������� �ʺ�ID ��� �ͻ���
*
*
* �� �� ֵ: >=0:�ɹ� <0:ʧ��
*           
* �޸���ʷ��
*
********************************************************************/
int Pub_base_Getacname( char  ac_no[25] ,char name[51] ,long *ac_id, long *cif_no, double *bal,char *reply)
{
	struct	com_item_c		g_com_item;
	struct	prdt_ac_id_c	sPrdt_ac_id;
	struct  gl_mst_c     	g_gl_mst;
	int	i;
	int	ret;

	MEMSET_DEBUG(&g_com_item,0x00,sizeof(struct com_item_c));
	MEMSET_DEBUG(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
	i=0;
	ret=0;

	strcpy(g_pub_tx.ac_no,ac_no);					/*�ʺ�*/
	pub_base_old_acno( g_pub_tx.ac_no );

	ldchar(g_pub_tx.ac_no,strlen(g_pub_tx.ac_no),g_pub_tx.ac_no);
	i=strlen(g_pub_tx.ac_no);

	if(i==3||i==5||i==7)							/*�����Ŀ*/
	{
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
		/**
			set_zd_data("0220","��");			
		**/
			*bal = g_gl_mst.dr_bal;			
		}
		else if(g_com_item.dc_ind[0]=='2')
		{
		/**
			set_zd_data("0220","��");			
		**/
			*bal = g_gl_mst.cr_bal;			
		}
		else if(g_com_item.dc_ind[0]=='0')		/*˫��*/
		{										
			ret=pub_base_CompDblVal(g_gl_mst.dr_bal,g_gl_mst.cr_bal);
			if(ret==1)
			{
			/**
				set_zd_data("0220","��");			
			**/
				*bal = g_gl_mst.dr_bal;			
			}
			else if(ret==-1)
			{
			/**
				set_zd_data("0220","��");
			**/
				*bal = g_gl_mst.cr_bal;			
			}
		}
		strcpy(name,g_com_item.acc_name);       /*����*/
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

		/**g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn; Del by hxc 20101019**/
		/*add by hxc 20101019*/	
		if((!memcmp(g_mdm_ac_rel.mdm_code,"0020",4)||!memcmp(g_mdm_ac_rel.mdm_code,"0024",4)) && g_mdm_ac_rel.ac_seqn==9999)
		{
			g_pub_tx.ac_seqn=1;
		}else{
		/*add by hxc 20101019*/
		g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn;
		}

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
			/***
			if( g_dd_parm.cif_type[0]=='1' )
			{
				sprintf(acErrMsg,"������Թ��˻�!!cif_type=[%s]",g_dd_parm.cif_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O165");
				goto ErrExit;
			}
			***/
			/* ������� */
			strcpy(name,g_dd_mst.name);				/*����*/
			*ac_id  = g_dd_mst.ac_id;
			*cif_no = g_dd_mst.cif_no;
			*bal    = g_dd_mst.bal;
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='2' )	/*** ���� ***/	
		{
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
			strcpy(name,g_td_mst.name);				/*����*/
			*bal = g_td_mst.bal;
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
			strcpy(name,g_ln_mst.name);				/*����*/
			*ac_id  = g_ln_mst.ac_id;
			*cif_no = g_ln_mst.cif_no;	
			*bal    = g_ln_mst.bal;
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
			strcpy(name,g_dd_mst.name);				/*����*/
			*ac_id  = g_dd_mst.ac_id;
			*cif_no = g_dd_mst.cif_no;	
			*bal    = g_dd_mst.bal;
		}
		else
		{
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
			/***
			if(g_com_item.dc_ind[0]=='1')
				set_zd_data("0220","��");
			else if(g_com_item.dc_ind[0]=='2')
				set_zd_data("0220","��");
			else if(g_com_item.dc_ind[0]=='0')      *˫��*
			{
				ret=pub_base_CompDblVal(g_in_mst.bal,0.00);
				if(ret==1)
			   		set_zd_data("0220","��");
				else 
			   		set_zd_data("0220","��");
	  		}
			**/

			/* ������� */
			strcpy(name,g_in_mst.name);				/*����*/
			*ac_id  = g_in_mst.ac_id;
			*cif_no = 0;
			*bal    = g_in_mst.bal;
		}
	}
	return 0;

ErrExit:
	strcpy(reply,g_pub_tx.reply);
	return 1;
}

#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "mo_trust_prot_c.h"

/**********************************************************************
* �� �� ����  pub_ln_trust_info
* �������ܣ�  ����ί��Э����ȡί��Э����Ϣ,�����4���˻�״̬�Ƿ�����
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��04��04��
*
* ��    ���� 
*     ��  �룺  trust_no        ί��Э����  	
*               t_mo_trust_prot ί��Э��Ǽǲ��ṹ	       
*
*     ��  ��:                         
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_trust_info(char *trust_no, struct mo_trust_prot_c *t_mo_trust_prot )
{
	int ret = 0 ;
	int ac_seqn;
	
	struct ln_mst_c    ln_mst;    
	struct dd_mst_c	   dd_mst_tmp;
	struct mo_trust_prot_c  mo_trust_prot;
	
	memset( &ln_mst , 0x00 , sizeof( struct ln_mst_c ) );
	memset( &dd_mst_tmp , 0x00 , sizeof( struct dd_mst_c ) );
	memset( &mo_trust_prot , 0x00 , sizeof( struct mo_trust_prot_c ) );
		
		sprintf(acErrMsg,"�˴����Ӧ�Ĵ�ί��Э����![%s], *trust_no{%s} trust_no[%s]", g_pub_tx.reply, *trust_no, trust_no);
		WRITEMSG
	/* ����ί��Э����ȡί��Э����Ϣ */
	ret = Mo_trust_prot_Sel( g_pub_tx.reply,&mo_trust_prot,"trust_no='%s'", trust_no ); /*trust_no��Ӧ��*ѽ��gujy 20060828*/
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˴����Ӧ�Ĵ�ί��Э�鲻����,����![%s]", g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L089");
		return -1;
	}else if( ret ){
		sprintf(acErrMsg,"��ȡί��Э��Ǽǲ���Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L090");
		return -1;
	}	
	
	/* ί���˴����� */

	memcpy(t_mo_trust_prot , &mo_trust_prot, sizeof(mo_trust_prot)); /*t_mo_trust_protû��ֵѽ? gujy 20060828*/

	ret = Ln_mst_Sel(g_pub_tx.reply, &ln_mst , "ac_id=%ld and ac_seqn=%d" ,
				        t_mo_trust_prot->ln_ac_id,t_mo_trust_prot->ln_ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;
	}else if( ret ){
   		sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		return -1;
   	}	
	if ( ln_mst.ac_sts[0]!='1' )
	{
		sprintf(acErrMsg,"ί��Э���д����˻�״̬�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L091");
		return -1;
	}
	
	/* ���ݱ���ȡ�˻���� */
	ret = pub_base_CurToSeqn( t_mo_trust_prot->cur_no , &ac_seqn );
	if ( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			return -1;	
		}
	sprintf(acErrMsg,"[%d]",ac_seqn);
	WRITEMSG
	
	/* ί�����˻���� */		
	ret = pub_ln_check_ddsts( t_mo_trust_prot->afund_ac_id , ac_seqn , "1" );
	if ( ret ) 
	{
		sprintf(acErrMsg,"ί�����˻����ʧ��");
		WRITEMSG
		return -1;
	}		
		
	/* ί���˻����˻���� */		
	ret = pub_ln_check_ddsts( t_mo_trust_prot->amt_ac_id , ac_seqn , "1" );
	if ( ret ) 
	{
		sprintf(acErrMsg,"ί���˻����˻����ʧ��");
		WRITEMSG
		return -1;
	}
	
	/* ί���˻�Ϣ�˻���� */		
	ret = pub_ln_check_ddsts( t_mo_trust_prot->intst_ac_id , ac_seqn , "1" );
	if ( ret ) 
	{
		sprintf(acErrMsg,"ί���˻�Ϣ�˻����ʧ��");
		WRITEMSG
		return -1;
	}
	
  	return 0;
}
/**********************************************************************
* �� �� ����  pub_ln_check_ddsts
* �������ܣ�  �����ڻ�״̬�Ƿ��ʺϴ��
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��04��04��
*
* ��    ���� 
*     ��  �룺  ac_id       interger  �˻�ID  	
*               ac_seqn     smallint  �˺����	       
*		info	    char      �˺���Ϣ��־1-ί�����˺�2-�������˺�3-����Ϣ�˺�
*     ��  ��:                         
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_check_ddsts( long ac_id , int ac_seqn , char *info)
{	
	int ret=0;
	struct dd_mst_c t_dd_mst;
	struct dd_parm_c t_dd_parm;

	memset(&t_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&t_dd_parm,0x00,sizeof(struct dd_parm_c));
	
	/* �������˺�״̬ */
	ret = pub_base_currentid_file( ac_id , ac_seqn , &t_dd_mst );
	if( ret )
	{
		if(info[0]=='0')
		{
			sprintf(acErrMsg,"�˺Ų�����[%d][%d]",ac_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			return 1;
		}
		if(info[0]=='1')
		{
			sprintf(acErrMsg,"ί��Э����ί�л��˺Ų�����[%d][%d]",ac_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L092");
			return 1;
		}
		if(info[0]=='2')
		{
			sprintf(acErrMsg,"ί��Э���л������˺Ų�����[%d][%d]",ac_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L093");
			return 1;
		}
		if(info[0]=='3')
		{
			sprintf(acErrMsg,"ί��Э���л���Ϣ�˺Ų�����[%d][%d]",ac_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L094");
			return 1;
		}				
		
	}
	      	
        ret=Dd_parm_Sel(g_pub_tx.reply,&t_dd_parm,"prdt_no='%s'",t_dd_mst.prdt_no);
        if( ret==100 )
        {
            sprintf(acErrMsg,"�����ڸò�Ʒ���!!prdt_no=[%ld]",
                             t_dd_mst.prdt_no);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P145");
            return 1;
        }else if( ret ){
            sprintf(acErrMsg,"��ѯ���ڲ������쳣!!ret=[%d]",ret);
            WRITEMSG
            return 1;
        }		
	if( t_dd_mst.ac_sts[0]!='1' )
	{
		if(info[0]=='0')
		{
			sprintf(acErrMsg,"�˺�״̬������״̬!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P109");
			return 1;
		}
		if(info[0]=='1')
		{
			sprintf(acErrMsg,"ί��Э����ί�л��˺�״̬������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L095");
			return 1;
		}
		if(info[0]=='2')
		{
			sprintf(acErrMsg,"ί��Э���л������˺�״̬������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L096");
			return 1;
		}
		if(info[0]=='3')
		{
			sprintf(acErrMsg,"ί��Э���л���Ϣ�˺�״̬������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L097");
			return 1;
		}					
		
	}
	if( t_dd_mst.hold_sts[0]=='1')
	{
		if(info[0]=='0')
		{
			sprintf(acErrMsg,"���˺��ѱ�����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L098");
			return 1;
		}
		if(info[0]=='1')
		{
			sprintf(acErrMsg,"ί��Э����ί�л��˺��ѱ�����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L099");
			return 1;
		}
		if(info[0]=='2')
		{
			sprintf(acErrMsg,"ί��Э���л������˺��ѱ�����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L100");
			return 1;			
		}
		if(info[0]=='3')
		{
			sprintf(acErrMsg,"ί��Э���л���Ϣ�˺��ѱ�����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L101");	
			return 1;		
		}						
		
	}
	if( t_dd_parm.dpst_ind[0]=='N' )
	{
		if(info[0]=='0')
		{
			sprintf(acErrMsg,"��������[%s]!",t_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A009");
			return 1;
		}
		if(info[0]=='1')
		{
			sprintf(acErrMsg,"ί��Э����ί�л��˺Ų�������[%s]!",t_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L102");
			return 1;
		}
		if(info[0]=='2')
		{
			sprintf(acErrMsg,"ί��Э���л������˺Ų�������[%s]!",t_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L103");
			return 1;
		}
		if(info[0]=='3')
		{
			sprintf(acErrMsg,"ί��Э���л���Ϣ�˺Ų�������[%s]!",t_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L104");
			return 1;
		}		
		
	}
	if( t_dd_parm.thr_dpst_ind[0]=='N' && strcmp(g_pub_tx.tx_br_no,t_dd_mst.opn_br_no) )
	{
		if(info[0]=='0')
		{	
			sprintf(acErrMsg,"����ͨ��[%s]!",t_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A010");
			return 1;
		}
		if(info[0]=='1')
		{	
			sprintf(acErrMsg,"ί��Э����ί�л��˺Ų���ͨ��[%s]!",t_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L105");
			return 1;
		}
		if(info[0]=='2')
		{	
			sprintf(acErrMsg,"ί��Э���л������˺Ų���ͨ��[%s]!",t_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L106");
			return 1;
		}
		if(info[0]=='3')
		{	
			sprintf(acErrMsg,"ί��Э���л���Ϣ�˺Ų���ͨ��[%s]!",t_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L107");
			return 1;
		}			
		
	}
	return 0;
}

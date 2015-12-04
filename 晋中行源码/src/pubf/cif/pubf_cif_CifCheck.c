/**********************************************************************
 * �� �� ����pubf_cif_CifCheck();
 * �������ܣ�
 *	     pub_cif_CifCheck()		����֤�����ͺ�֤������ȡ�ÿͻ��ż��ͻ�����
 *	     pub_cif_GetCifType()	���ݿͻ���ȡ�ÿͻ�����
 *       pub_cif_AcToCifType()      ������ʾ�˺�ȡ�ÿͻ�����  
 *		 pub_cif_AcToIdNoType() �����˻�id �� ac_seqnȡ֤�����ͺͺ���
 *		 pub_cif_Ins_Chg_inf()	�Ǽǿͻ���Ϣ�޸ĵǼǲ�
 *
 * ����/ʱ�䣺xxxxx/20031011
 * 
 * ������
 *     ���룺	��
 *          
 *     ���: 	
 *
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "cif_inf_chg_log_c.h"


/**********************************************************************
 * �� �� ����pub_cif_CifCheck();
 * �������ܣ�����֤�����ͺ�֤������ȡ�ÿͻ��ż��ͻ�����
 *			 
 * ����/ʱ�䣺xxxxx/20030111
 * 
 * ������
 *     ���룺	
 *		id_type		֤������		
 *		id_no		֤������
 *     ���: 	
 *		cif_no		�ͻ���
 *		cif_type	�ͻ�����
 *
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_cif_CifCheck ( char *id_type , char *id_num , long *cif_no , char *cif_type )
{
	struct cif_id_code_rel_c      cif_id_code_rel; 
	struct cif_basic_inf_c        cif_basic_inf; 
	int ret=0;

	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	
	/* ����֤�����ͺ�֤������ȡ�ͻ��� */

	ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
					" id_type='%s' and id_no='%s' " , id_type , id_num );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��֤����Ϣû�еǼ�,�뵽�ͻ����ĵǼ�[%s][%s]",id_type,id_num);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C019");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C002");
		goto ErrExit;
	}
	*cif_no = cif_id_code_rel.cif_no ;
					  
	/* ���ݿͻ��Ų�ѯ�ͻ����� */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , cif_id_code_rel.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��֤����Ӧ�Ŀͻ��Ų�����[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C020");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}					  
	strcpy ( cif_type , cif_basic_inf.type );				  

	return 0;
ErrExit:

    return 1;
}

/**********************************************************************
 * �� �� ����pub_cif_GetCifType();
 * �������ܣ����ݿͻ���ȡ�ÿͻ�����
 *			 
 * ����/ʱ�䣺xxxxx/20030112
 * 
 * ������
 *     ���룺	cif_mo		�ͻ���		long
 *          
 *     ���: 	
 *		cif_type	�ͻ�����	char
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_cif_GetCifType ( long cif_no , char *cif_type )
{

	struct cif_basic_inf_c        cif_basic_inf; 
	int ret=0;

	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	sprintf(acErrMsg,"�ͻ���[%d]",cif_no);
	WRITEMSG

						  
	/* ���ݿͻ��Ų�ѯ�ͻ����� */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ÿͻ��Ų�����[%s][%ld]",g_pub_tx.reply,cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}					  
	strcpy ( cif_type , cif_basic_inf.type );				  

	return 0;
ErrExit:

    return 1;
}


/**********************************************************************
 * �� �� ����pub_cif_AcToCifType();
 * �������ܣ�������ʾ�˺�ȡ�ÿͻ�����
 *			 
 * ����/ʱ�䣺xxxxx/20030117
 * 
 * ������
 *     ���룺	ac_no		��ʾ�˺�	char
 *          
 *     ���: 	
 *		cif_type	�ͻ�����	char
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_cif_AcToCifType ( char *ac_no , char *cif_type )
{			
	int ret=0;
	long cif_no;
						  
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	/* ������ʾ�˺Ų�ѯ�������˻���ϵ��Ϣ */ 
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
	sprintf(acErrMsg,"������ʾ�˺Ų�ѯ�����״̬ PASS!");
	WRITEMSG  

	/* ���ú��� */
	
	ret = pub_cif_CifCheck ( g_mdm_ac_rel.id_type ,g_mdm_ac_rel.id_no ,&cif_no , cif_type );
	if( ret )
		{
			sprintf(acErrMsg,"���ú���pub_cif_CifCheck����!");
			WRITEMSG
			goto ErrExit;	
		}
	return 0;
ErrExit:

    return 1;
}

/**********************************************************************
 * �� �� ����pub_cif_AcToIdNoType();
 * �������ܣ�����֤�����ͺ�֤������ȡ�ÿͻ��ż��ͻ�����
 *			 
 * ����/ʱ�䣺rob/20030728
 * 
 * ������
 *     ���룺	
 *		ac_id		�˻�id
 *		ac_seqn	    �˻����
 *     ���: 	
 *		id_type		֤������		
 *		id_no		֤������
 *
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_cif_AcToIdNoType(long ac_id, long ac_seqn, char *id_type, char *id_no)
{
	int ret = 0;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct cif_id_code_rel_c sCif_id_code_rel;

	memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
	memset(&sCif_id_code_rel, 0x0, sizeof(struct cif_id_code_rel_c));

	sprintf(acErrMsg, " CGH -------> ac_id=[%ld], ac_seqn=[%ld]", 
		ac_id, ac_seqn);
	WRITEMSG
	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld and \
			ac_seqn=%ld", ac_id, ac_seqn);
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg, "��ѯ��Ʒ�˺Ŷ��ձ����!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "W013");
		return ret;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg, "��Ʒ�˺Ŷ��ձ��в����ڸü�¼!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "W012");
		return ret;
	}

	ret = Cif_id_code_rel_Sel(g_pub_tx.reply, &sCif_id_code_rel, 
		"cif_no=%ld", sPrdt_ac_id.cif_no);
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg, "�ͻ�֤����ͻ��Ŷ��ձ����!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "C002");
		return ret;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg, "�ͻ�֤����ͻ��Ŷ��ձ�û�д˼�¼!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "B115");
		return ret;
	}

	pub_base_strpack(sCif_id_code_rel.id_type);
	pub_base_strpack(sCif_id_code_rel.id_no);
	strcpy(id_type, sCif_id_code_rel.id_type);
	strcpy(id_no, sCif_id_code_rel.id_no);

	return 0;
}

/**********************************************************************
 * �� �� ����pub_cif_Ins_Chg_inf();
 * �������ܣ��Ǽǿͻ���Ϣ�޸ĵǼǲ�
 *			 
 * ����/ʱ�䣺xxxxx/20031130
 * 
 * ������
 *     ���룺	
 *		cif_no	integer	�ͻ�����
 *		chg_tablename	char(20)	���޸Ŀͻ���Ϣ����
 *		chg_columnname			���޸Ŀͻ���Ϣ����
 *		bef_content	char(40)	�޸�ǰ����
 *		aft_content			�޸ĺ�����
 *		memo		char(40)	˵��

 *     ���: 	
 *
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_cif_Ins_Chg_inf ( long cif_no ,char *chg_tablename ,char *chg_columnname ,char *bef_content, char *aft_content, char *memo )
{
	struct cif_inf_chg_log_c      S_cif_inf_chg_log; 
	int ret=0;

	memset (&S_cif_inf_chg_log, 0x00, sizeof(struct cif_inf_chg_log_c));
	
	S_cif_inf_chg_log.cif_no=cif_no;
	/****��������ʱ���ָ�ֵ��������ͳһ��strcpy�޸�Ϊʹ��memcpy����  modify at 20130110****/
	memcpy( S_cif_inf_chg_log.chg_tablename,chg_tablename,sizeof(S_cif_inf_chg_log.chg_tablename));
	memcpy( S_cif_inf_chg_log.chg_columnname,chg_columnname,sizeof(S_cif_inf_chg_log.chg_columnname));
	S_cif_inf_chg_log.chg_date=g_pub_tx.tx_date;
	S_cif_inf_chg_log.chg_time=g_pub_tx.tx_time;
	memcpy( S_cif_inf_chg_log.bef_content,bef_content,sizeof(S_cif_inf_chg_log.bef_content));
	memcpy( S_cif_inf_chg_log.aft_content,aft_content,sizeof(S_cif_inf_chg_log.aft_content));	
	memcpy( S_cif_inf_chg_log.clerk_no,g_pub_tx.tel,sizeof(S_cif_inf_chg_log.clerk_no));
	memcpy( S_cif_inf_chg_log.memo,memo,sizeof(S_cif_inf_chg_log.memo));
	/****end at 20130110****/
	vtcp_log("[%s][%d] cif_no==[%ld]  S_cif_inf_chg_log.cif_no==[%ld]",__FILE__,__LINE__,cif_no,S_cif_inf_chg_log.cif_no);
	vtcp_log("[%s][%d] CHG_TABLENAME==[%s]  S_cif_inf_chg_log.chg_tablename==[%s]",__FILE__,__LINE__,chg_tablename,S_cif_inf_chg_log.chg_tablename);
	vtcp_log("[%s][%d] CHG_COLUNMNAME==[%s] S_cif_inf_chg_log.chg_columnname==[%s]",__FILE__,__LINE__,chg_columnname,S_cif_inf_chg_log.chg_columnname);
	vtcp_log("[%s][%d] BEF_CONTENT==[%s]  S_cif_inf_chg_log.bef_content==[%s]",__FILE__,__LINE__,bef_content,S_cif_inf_chg_log.bef_content);
	vtcp_log("[%s][%d] ATF_CONTENT==[%s]  S_cif_inf_chg_log.aft_content==[%s]",__FILE__,__LINE__,aft_content,S_cif_inf_chg_log.aft_content);
	vtcp_log("[%s][%d] clerk_no==[%s]",__FILE__,__LINE__,g_pub_tx.tel);
	vtcp_log("[%s][%d] S_cif_inf_chg_log.clerk_no==[%s]",__FILE__,__LINE__,S_cif_inf_chg_log.clerk_no);

	ret = Cif_inf_chg_log_Ins( S_cif_inf_chg_log , g_pub_tx.reply );
	if( ret )
	{
		Cif_inf_chg_log_Debug(&S_cif_inf_chg_log);
		sprintf(acErrMsg,"�Ǽ�Cif_inf_chg_log_Ins��Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}			  

	return 0;
ErrExit:

    return 1;
}

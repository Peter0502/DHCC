/**********************************************************************
 * �� �� ����pubf_cif_CreCifNo.c;
 * �������ܣ����ɿͻ�ID����
 *			 
 * ����/ʱ�䣺
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
#include "cif_no_per_c.h"
#include "cif_no_com_c.h"
#include "cif_no_in_c.h"
#include "cif_no_yzh_c.h"

/**********************************************************************
 * �� �� ����pub_cif_CrePer_cif_no();
 * �������ܣ����ɿͻ�ID����(����)
 *			 
 *
 * ����/ʱ�䣺
 * 
 * ������
 *     ���룺	��
 *          
 *     ���: 	
 *				�ͻ�ID		long;
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_cif_CrePer_cif_no( cif_no )
	long	*cif_no;
{
	struct cif_no_per_c      cif_no_per; /* �ͻ������(����) */
	int ret=0;
	char no[8];
	char no_tmp[9];
	
	/*ȡ���ͻ�id,���޸ģ����ؿ������ͻ��˺�id*/

	ret = Cif_no_per_Dec_Upd (g_pub_tx.reply, " 1=1 " );
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}
	memset(&cif_no_per,'\0',sizeof(cif_no_per));
	ret = Cif_no_per_Fet_Upd (&cif_no_per,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"fetch cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	cif_no_per.cif_no = cif_no_per.cif_no + 1;

	ret = Cif_no_per_Upd_Upd (cif_no_per,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	Cif_no_per_Clo_Upd( );
	sprintf(acErrMsg,"LONG�Ϳͻ���=[%d]",cif_no_per.cif_no);
	WRITEMSG

	/* ��ȡ���ĳ����Ϳͻ���ת�����ַ��� */
	sprintf( no, "%ld" , cif_no_per.cif_no );
	sprintf(acErrMsg,"ת�ַ��Ŀͻ���=[%s]",no);
	WRITEMSG

	/* ���ݿͻ������ɴ�У��λ�Ŀͻ��� */
	ret = pub_base_CrtAcChkb( no , no_tmp , 4 , 1 );
	if ( ret  )
	{
		return (1);
	}
	sprintf(acErrMsg,"�ַ�����ɵĿͻ���=[%s]",no_tmp);
	WRITEMSG
	
	/* ���ַ��͵Ŀͻ���ת���ɳ����� */	
	*cif_no = atol( no_tmp );
	sprintf(acErrMsg,"��ɵĿͻ���=[%d]",*cif_no);
	WRITEMSG
	/**cif_no = cif_no_per.cif_no;*/
	return 0;
}

/**********************************************************************
 * �� �� ����pub_cif_CreCom_cif_no();
 * �������ܣ����ɿͻ�ID����(��˾)
 *			 
 *
 * ����/ʱ�䣺
 * 
 * ������
 *     ���룺	��
 *          
 *     ���: 	
 *				�ͻ�ID		long;
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_cif_CreCom_cif_no( cif_no )
	long	*cif_no;
{
	struct cif_no_com_c      cif_no_com; /* �ͻ������(��˾) */
	int ret=0;
	char no[8];
	char no_tmp[9];	
	
	/*ȡ���ͻ�id,���޸ģ����ؿ������ͻ��˺�id*/
	ret = Cif_no_com_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_no_com error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	ret = Cif_no_com_Fet_Upd (&cif_no_com,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"fetch cif_no_com error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	cif_no_com.cif_no = cif_no_com.cif_no + 1;

	ret = Cif_no_com_Upd_Upd (cif_no_com,&g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_no_com error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	Cif_no_com_Clo_Upd( );
	
	/* ��ȡ���ĳ����Ϳͻ���ת�����ַ��� */
	sprintf( no, "%ld" , cif_no_com.cif_no );
	sprintf(acErrMsg,"ת�ַ��Ŀͻ���=[%s]",no);
	WRITEMSG
	
	/* ���ݿͻ������ɴ�У��λ�Ŀͻ��� */
	ret = pub_base_CrtAcChkb( no , no_tmp , 4 , 1 );
	if ( ret  )
            {
                return (1);
            }
        sprintf(acErrMsg,"�ַ�����ɵĿͻ���=[%s]",no_tmp);
	WRITEMSG
	
	/* ���ַ��͵Ŀͻ���ת���ɳ����� */	
	*cif_no = atol( no_tmp );
	sprintf(acErrMsg,"��ɵĿͻ���=[%d]",*cif_no);
	WRITEMSG

	/**cif_no = cif_no_com.cif_no;*/

	return 0;

}

/**********************************************************************
 * �� �� ����pub_cif_CreIn_cif_no();
 * �������ܣ����ɿͻ�ID����(�ڲ�)
 *			 
 *
 * ����/ʱ�䣺
 * 
 * ������
 *     ���룺	��
 *          
 *     ���: 	
 *				�ͻ�ID		long;
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_cif_CreIn_cif_no( cif_no )
	long	*cif_no;
{
	struct cif_no_in_c      cif_no_in; /* �ͻ������(�ڲ�) */
	int ret=0;	
	char no[8];
	char no_tmp[9];
		
	/*ȡ���ͻ�id,���޸ģ����ؿ������ͻ��˺�id*/
	ret = Cif_no_in_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_no_in error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	ret = Cif_no_in_Fet_Upd (&cif_no_in,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"fetch cif_no_in error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	cif_no_in.cif_no = cif_no_in.cif_no + 1;

	ret = Cif_no_in_Upd_Upd (cif_no_in,&g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_no_in error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	Cif_no_in_Clo_Upd( );

	/* ��ȡ���ĳ����Ϳͻ���ת�����ַ��� */
	sprintf( no, "%ld" , cif_no_in.cif_no );
	sprintf(acErrMsg,"ת�ַ��Ŀͻ���=[%s]",no);
	WRITEMSG
	
	/* ���ݿͻ������ɴ�У��λ�Ŀͻ��� */
	ret = pub_base_CrtAcChkb( no , no_tmp , 4 , 1 );
	if ( ret  )
            {
                return (1);
            }
        sprintf(acErrMsg,"�ַ�����ɵĿͻ���=[%s]",no_tmp);
	WRITEMSG
	
	/* ���ַ��͵Ŀͻ���ת���ɳ����� */	
	*cif_no = atol( no_tmp );
	sprintf(acErrMsg,"��ɵĿͻ���=[%d]",*cif_no);
	WRITEMSG
	
	/**cif_no = cif_no_in.cif_no;*/

	return 0;

}
/**********************************************************************
 * �� �� ����pub_cif_CreYzh_cif_no();
 * �������ܣ����ɿͻ�ID����(���ʻ�)
 *			 
 *
 * ����/ʱ�䣺
 * 
 * ������
 *     ���룺	��
 *          
 *     ���: 	
 *				�ͻ�ID		long;
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_cif_CreYzh_cif_no( cif_no )
	long	*cif_no;
{
	struct cif_no_yzh_c      cif_no_yzh; /* �ͻ������(���ʻ�) */
	int ret=0;
	char no[8];
	char no_tmp[9];
	
	/*ȡ���ͻ�id,���޸ģ����ؿ������ͻ��˺�id*/

	ret = Cif_no_yzh_Dec_Upd (g_pub_tx.reply, " 1=1 " );
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}
	memset(&cif_no_yzh,'\0',sizeof(cif_no_yzh));
	ret = Cif_no_yzh_Fet_Upd (&cif_no_yzh,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"fetch cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	cif_no_yzh.cif_no = cif_no_yzh.cif_no + 1;

	ret = Cif_no_yzh_Upd_Upd (cif_no_yzh,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	Cif_no_yzh_Clo_Upd( );
	sprintf(acErrMsg,"LONG�Ϳͻ���=[%d]",cif_no_yzh.cif_no);
	WRITEMSG

	/* ��ȡ���ĳ����Ϳͻ���ת�����ַ��� */
	sprintf( no, "%ld" , cif_no_yzh.cif_no );
	sprintf(acErrMsg,"ת�ַ��Ŀͻ���=[%s]",no);
	WRITEMSG

	/* ���ݿͻ������ɴ�У��λ�Ŀͻ��� */
	ret = pub_base_CrtAcChkb( no , no_tmp , 4 , 1 );
	if ( ret  )
	{
		return (1);
	}
	sprintf(acErrMsg,"�ַ�����ɵĿͻ���=[%s]",no_tmp);
	WRITEMSG
	
	/* ���ַ��͵Ŀͻ���ת���ɳ����� */	
	*cif_no = atol( no_tmp );
	sprintf(acErrMsg,"��ɵĿͻ���=[%d]",*cif_no);
	WRITEMSG
	/**cif_no = cif_no_yzh.cif_no;*/
	return 0;
}


#include "find_debug.h" 
/*************************************************
* �� �� ��: pubf_base_GetParm.c
* ����������   
*          pub_base_GetParm_Str     ��ȡ�ַ��Ͳ���
*	       pub_base_GetParm_double  ��ȡ����Ͳ���
*	       pub_base_GetParm_long	��ȡ�����Ͳ���
*	       pub_base_GetParm_int		��ȡ���Ͳ���
*
* ��    ��:    lance
* ������ڣ�   2003��01��11��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "com_parm_c.h"

/**********************************************************************
* �� �� ����  pub_base_GetParm_Str
* �������ܣ�  ��ȡ�ַ��Ͳ���
*            
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��18��
*
* ��    ���� 
*     ��  �룺parm_code  char(5)  	��������    
*      	      seqn  	 smallint	��� 
*     ��  ��: val    	 char(20)	����ֵ
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_base_GetParm_Str( char *parm_code , int parm_seqn , char *val )
{	
	int ret=0;
	int flag=0;
	struct com_parm_c	com_parm;

	sprintf(acErrMsg,"�����ȡ�ַ��Ͳ�������!");
	WRITEMSG	
	
	MEMSET_DEBUG (&com_parm, 0x00, sizeof(struct com_parm_c));

	/* ��������Ĳ����������Ų�ѯ���������� */	
	ret = Com_parm_Sel( g_pub_tx.reply , &com_parm , 
					" parm_code='%s' and parm_seqn=%d" , parm_code ,parm_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���������޶�Ӧ��Ϣ������![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O004");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"��ȡ�����������쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O005");
			goto ErrExit;
		}
	sprintf(acErrMsg,"��������Ĳ����������Ų�ѯ���������� PASS!");
	WRITEMSG
		
	/* ��������Ĳ������� */		
	flag = atoi(com_parm.val_type);
	
	if ( flag==2 || flag==3 || flag==4 )
	{
		sprintf(acErrMsg, "ȡ���Ĳ������ʹ���[%s]", com_parm.val_type );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O006");
		goto ErrExit;
	}
	sprintf(acErrMsg,"��������Ĳ������� PASS!");
	WRITEMSG	
	
	pub_base_strpack( com_parm.val );

	strcpy( val , com_parm.val );
	sprintf(acErrMsg, "ȡ���Ĳ���ֵΪ[%s] ", val );
	WRITEMSG
		
	return 0;
	
ErrExit:
	return 1; 
}


/**********************************************************************
* �� �� ����  pub_base_GetParm_double
* �������ܣ�  ��ȡ����Ͳ���
*            
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��18��
*
* ��    ���� 
*     ��  �룺parm_code  char(5)  	��������    
*      	      seqn  	 smallint	��� 
*     ��  ��: val    	 double 	����ֵ
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_base_GetParm_double( char *parm_code , int parm_seqn , double *val )
{	
	int ret=0;
	int flag=0;
	struct com_parm_c	com_parm;

	MEMSET_DEBUG (&com_parm, 0x00, sizeof(struct com_parm_c));
	
	sprintf(acErrMsg,"�����ȡ����Ͳ�������!");
	WRITEMSG	

	/* ��������Ĳ����������Ų�ѯ���������� */	
	ret = Com_parm_Sel( g_pub_tx.reply , &com_parm , 
					" parm_code='%s' and parm_seqn=%d" , parm_code ,parm_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���������޶�Ӧ��Ϣ������![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O004");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"��ȡ�����������쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O005");
			goto ErrExit;
		}
	sprintf(acErrMsg,"��������Ĳ����������Ų�ѯ���������� PASS!");
	WRITEMSG
		
	/* ��������Ĳ������� */		
	flag = atoi(com_parm.val_type);
	
	if ( flag!=4 )
	{
		sprintf(acErrMsg, "ȡ���Ĳ������ʹ���[%s]", com_parm.val_type );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O006");
		goto ErrExit;
	}
	sprintf(acErrMsg,"��������Ĳ������� PASS!");
	WRITEMSG	
	
	*val = atof(com_parm.val) ;
	sprintf(acErrMsg, "ȡ���Ĳ���ֵΪ[%lf] ", *val );
	WRITEMSG
		
	return 0;
	
ErrExit:
	return 1; 
}


/**********************************************************************
* �� �� ����  pub_base_GetParm_long
* �������ܣ�  ��ȡ�����Ͳ���
*            
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��18��
*
* ��    ���� 
*     ��  �룺parm_code  char(5)  	��������    
*      	      seqn  	 smallint	��� 
*     ��  ��: val    	 long		����ֵ
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_base_GetParm_long( char *parm_code , int parm_seqn , long *val )
{	
	int ret=0;
	int flag=0;
	
	struct com_parm_c	com_parm;
	MEMSET_DEBUG (&com_parm, 0x00, sizeof(struct com_parm_c));

	sprintf(acErrMsg,"�����ȡ�����Ͳ�������!");
	WRITEMSG	

	/* ��������Ĳ����������Ų�ѯ���������� */	
	ret = Com_parm_Sel( g_pub_tx.reply , &com_parm , 
					" parm_code='%s' and parm_seqn=%d" , parm_code ,parm_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���������޶�Ӧ��Ϣ������![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O004");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"��ȡ�����������쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O005");
			goto ErrExit;
		}
	sprintf(acErrMsg,"��������Ĳ����������Ų�ѯ���������� PASS!");
	WRITEMSG
		
	/* ��������Ĳ������� */		
	flag = atoi(com_parm.val_type);
	
	if ( flag!=2 )
	{
		sprintf(acErrMsg, "ȡ���Ĳ������ʹ���[%s]", com_parm.val_type );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O006");
		goto ErrExit;
	}
	sprintf(acErrMsg,"��������Ĳ������� PASS!");
	WRITEMSG	
	
	*val = atol(com_parm.val) ;
	sprintf(acErrMsg, "ȡ���Ĳ���ֵΪ[%ld] ", *val );
	WRITEMSG
		
	return 0;
	
ErrExit:
	return 1; 
}


/**********************************************************************
* �� �� ����  pub_base_GetParm_int
* �������ܣ�  ��ȡ���Ͳ���
*            
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��18��
*
* ��    ���� 
*     ��  �룺parm_code  char(5)  	��������    
*      	      seqn  	 smallint	��� 
*     ��  ��: val    	 int		����ֵ
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_base_GetParm_int( char *parm_code , int parm_seqn , int *val )
{	
	
	int ret=0;
	int flag=0;
	
	struct com_parm_c	com_parm;
	MEMSET_DEBUG (&com_parm, 0x00, sizeof(struct com_parm_c));

	sprintf(acErrMsg,"�����ȡ���Ͳ�������!");
	WRITEMSG	
	/* ��������Ĳ����������Ų�ѯ���������� */	
	ret = Com_parm_Sel( g_pub_tx.reply , &com_parm , 
					" parm_code='%s' and parm_seqn=%d" , parm_code ,parm_seqn );
	if( ret==100 )
	{
		vtcp_log(" parm_code='%s' and parm_seqn=%d" , parm_code ,parm_seqn );
		sprintf(acErrMsg,"���������޶�Ӧ��Ϣ������![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O004");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"��ȡ�����������쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O005");
			goto ErrExit;
		}
	sprintf(acErrMsg,"��������Ĳ����������Ų�ѯ���������� PASS!");
	WRITEMSG
		
	/* ��������Ĳ������� */		
	flag = atoi(com_parm.val_type);
	
	if ( flag!=3 )
	{
		sprintf(acErrMsg, "ȡ���Ĳ������ʹ���[%s]", com_parm.val_type );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O006");
		goto ErrExit;
	}
	sprintf(acErrMsg,"��������Ĳ������� PASS!");
	WRITEMSG	
	
	*val = atoi(com_parm.val) ;
	sprintf(acErrMsg, "ȡ���Ĳ���ֵΪ[%ld] [%s]", *val,com_parm.val );
	WRITEMSG
		
	return 0;
	
ErrExit:
	return 1; 
}

/*************************************************
* �� �� ��: spL001.c
* ����������   
*              ��������������������
*
* ��    ��:    ��־��
* ������ڣ�   2015��03��30��

insert into tx_flow_def values('3201','5','L001','#$');
insert into tx_sub_def values('L001','���´���������','spL001','0','0');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "ln_auth_c.h"

static	struct ln_auth_c	ln_auth;
spL001()  
{ 	
	int ret = 0;
	char jjh[20];
	memset (&ln_auth, 0x00, sizeof(struct ln_auth_c));
	memset(jjh,0x00,sizeof(jjh));
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();	
	/** ȡֵ����ֵ **/
	get_zd_data("0240",g_pub_tx.prdt_code);	/* ��Ʒ�� */
	get_zd_data("0370",jjh);	/* ��ݺ� */
	sprintf(acErrMsg,"��Ʒ��Ϊ[%s]!!",g_pub_tx.prdt_code);
	WRITEMSG	
	sprintf(acErrMsg,"��ݺ�Ϊ[%s]!!",jjh);
	WRITEMSG	
	if(!strcmp(g_pub_tx.prdt_code,"142") || !strcmp(g_pub_tx.prdt_code,"144"))
		{
			ret = Ln_auth_Dec_Upd(g_pub_tx.reply," pact_no='%s' " ,jjh);
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Ln_auth_Dec_Upd��!ret=[%d]",ret );
					WRITEMSG
					return(-1);
				}
				ret = Ln_auth_Fet_Upd( &ln_auth, g_pub_tx.reply );
				if( ret==100)
				{
					sprintf(acErrMsg,"�����������޴˼�¼",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D104");
					ERR_DEAL;
				}
				else if ( ret )
				{
					sprintf(acErrMsg,"ȡ������������Ϣ�쳣",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"L181");
					ERR_DEAL;
				}	
				strcpy( ln_auth.sts , "*" );	 /* �޸�״̬:ʧЧ */
				ret = Ln_auth_Upd_Upd( ln_auth, g_pub_tx.reply);
				if ( ret )
				{
					sprintf( acErrMsg,"Ln_auth_Upd_Upd ����[%d]",ret);
					WRITEMSG
					return 1;
				}		
	
				Ln_auth_Clo_Upd( );
				sprintf(acErrMsg,"���´��������� PASS");
				WRITEMSG
		}
	else
		{
			goto OkExit;
		}
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�����ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
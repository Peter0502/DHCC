/*************************************************
* �� �� ��: spC403.c
* ������������Ա�����ϵ���
*              
*
* ��    ��: andy
* ������ڣ�2004��06��02��
* �޸ļ�¼��   
*     1. ��    ��: 
*        �� �� ��: 
*        �޸�����:
*************************************************/
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "tel_save_inf_c.h"
#include "com_tel_c.h"

spC403()  
{
	struct 	tel_save_inf_c		tel_save_inf;		/* ��Ա������Ϣ�� */
	struct 	tel_save_inf_c		tel_save_inf_tmp;		/* ��Ա������Ϣ�� */	
	struct 	com_tel_c		com_tel_tmp;		/* ��Ա��Ϣ�� */
	int ret=0;
	char tbwhere[101];

	memset ( &tel_save_inf, 0x00, sizeof( struct tel_save_inf_c ) );
	memset ( &tel_save_inf_tmp, 0x00, sizeof( struct tel_save_inf_c ) );	
	memset ( &com_tel_tmp, 0x00, sizeof( struct com_tel_c ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0370", tel_save_inf.ac_no );		/* �����˺� */
	get_zd_data( "0920", tel_save_inf.tel );		/* ��Ա���� */
	get_zd_long( "0340",  &tel_save_inf.ac_seqn ); 		/* �˻���� */
	
	/* ����˺��Ƿ���� */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , "ac_no='%s'" , tel_save_inf.ac_no );
	if( ret && ret!=100 )
	{
		sprintf( acErrMsg,"��ȡ�����˺Ŷ��ձ�ʧ��,�������Ļ�����ϵ!ret=[%d]",ret);
                WRITEMSG
                strcpy( g_pub_tx.reply, "C114" );
                goto ErrExit;
        }else if ( ret==100 ){
  		sprintf( acErrMsg,"���˺Ų�����,����!");
                WRITEMSG
                strcpy( g_pub_tx.reply, "C115" );
                goto ErrExit;
        }   
        
        /* ����˺ţ�����Ƿ���� */
 	/*������ʾ�ʺź��˺���ŷ����˻����ͺ����ļ�*/
	ret = pub_base_disac_file( tel_save_inf.ac_no , tel_save_inf.ac_seqn ,
								g_pub_tx.ac_id_type);
	if( ret )
	{
		sprintf(acErrMsg,"��������ʾ�ʺź��˺���ŷ����˻����ͺ����ļ�����!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto ErrExit;
	}
	          			
	/* ����Ա���Ƿ���� */
	ret = Com_tel_Sel( g_pub_tx.reply , &com_tel_tmp , "tel='%s'" , tel_save_inf.tel );
	if( ret && ret!=100 )
	{
		sprintf( acErrMsg,"��ȡ��Ա��Ϣʧ��,�������Ļ�����ϵ!ret=[%d]",ret);
                WRITEMSG
                strcpy( g_pub_tx.reply, "C118" );
                goto ErrExit;
        }else if ( ret==100 ){
  		sprintf( acErrMsg,"�ù�Ա������,����!");
                WRITEMSG
                strcpy( g_pub_tx.reply, "C119" );
                goto ErrExit;
        }     	
	
	/* ��ѯ��Ա������Ϣ�����Ƿ�����ؼ�¼ */
	sprintf(acErrMsg,"ac_no='%s' and ac_seqn=%d and tel='%s'",
				tel_save_inf.ac_no,tel_save_inf.ac_seqn,tel_save_inf.tel);		
	WRITEMSG
		
	ret = Tel_save_inf_Sel( g_pub_tx.reply , &tel_save_inf_tmp , " ac_no='%s' and ac_seqn=%d and tel='%s'",tel_save_inf.ac_no,tel_save_inf.ac_seqn,tel_save_inf.tel);
	if( ret==100 )
	{
  		sprintf( acErrMsg,"��Ա������Ϣ�ǼǱ����޴˼�¼!ret=[%d]", ret );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C122" );                
                goto ErrExit;
	}else if(ret){
  		sprintf( acErrMsg,"��ѯ��Ա������Ϣ�ǼǱ��쳣����,�������Ļ�����ϵ!ret=[%d]", ret );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C123" );                
                goto ErrExit;
	}		
	
	/* ɾ����Ա������Ϣ������ؼ�¼ */
	memset(tbwhere,0,sizeof(tbwhere));
	sprintf(tbwhere,"ac_no='%s' and ac_seqn=%d and tel='%s'",
				tel_save_inf.ac_no,tel_save_inf.ac_seqn,tel_save_inf.tel);
	sprintf(acErrMsg,"ac_no='%s' and ac_seqn=%d and tel='%s'",
				tel_save_inf.ac_no,tel_save_inf.ac_seqn,tel_save_inf.tel);		
	WRITEMSG
	
	ret = deldb("tel_save_inf", tbwhere );	
	WRITEMSG				
	if( ret )
	{
  		sprintf( acErrMsg,"ɾ����Ա������Ϣ�ǼǱ��쳣����,�������Ļ�����ϵ!ret=[%d]", ret );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C121" );                  
                goto ErrExit;
	}		
	
    	
	/* ����ˮ��־ */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"��Ա�����ϵ����ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"��Ա�����ϵ���ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

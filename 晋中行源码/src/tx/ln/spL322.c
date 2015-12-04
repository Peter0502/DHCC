/*************************************************
* �� �� ��: spL322.c
* ����������   
*              �����Ʒ��ֵת���Ǽ�
*
* ��    ��:    lpj
* ������ڣ�   2015��11��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "new_jz_prdt_c.h"
#include "prdt_jz_dz_c.h"

char prod_no[3];
char type[2];
		
spL322()  
{ 	
	
	int ret=0;


	struct	ln_mst_c	ln_mst_new;
	struct	ln_parm_c	ln_parm_new;	
	struct	new_jz_prdt_c sNew_jz_prdt;
	struct	new_jz_prdt_c sNew_jz_prdt1;
	struct	prdt_jz_dz_c sPrdt_jz_dz;
	char wherelist[512];
	char old_prdt_no[4];
	
	memset ( &ln_mst_new , 0x00 , sizeof( struct ln_mst_c ) );
	memset ( &ln_parm_new , 0x00 , sizeof( struct ln_parm_c ) );
	memset ( &sNew_jz_prdt , 0x00 , sizeof( struct new_jz_prdt_c ) );
	memset ( &sNew_jz_prdt1 , 0x00 , sizeof( struct new_jz_prdt_c ) );
	memset ( &sPrdt_jz_dz , 0x00 , sizeof( struct prdt_jz_dz_c ) );
	memset ( wherelist , 0x00 , sizeof( wherelist) );			
	memset ( old_prdt_no , 0x00 , sizeof( old_prdt_no) );	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	

	/** ȡֵ����ֵ **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
		
	/** �������� **/
	
	/* ȡ�ϲ�Ʒ��Ϣ */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG

	/* ȡ�²�Ʒ��Ϣ */
	ret = Ln_parm_Sel( g_pub_tx.reply , &ln_parm_new, "prdt_no='%s'", prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"�����Ʒ�������޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		goto ErrExit;
	}else if( ret ){
    		sprintf( acErrMsg,"ȡ�����Ʒ�������쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
	    		goto ErrExit;
    	}
	sprintf(acErrMsg,"ȡ�����Ʒ������Ϣ PASS!");
	WRITEMSG		
	sprintf(acErrMsg,"�����ִ����Ʒ[%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
	WRITEMSG
	/*��ֵ��Ʒ���ռ��*/
	if(type[0]=='1')
	{
		sprintf(wherelist,"prdt_no='%s' and jz_prdt_no='%s'",g_ln_parm.prdt_no,prod_no);
	}else if(type[0]=='2')
	{
		sprintf(wherelist,"jz_prdt_no='%s' and prdt_no='%s'",g_ln_parm.prdt_no,prod_no);
	}
	ret=Prdt_jz_dz_Sel(g_pub_tx.reply,&sPrdt_jz_dz,wherelist);
	if(ret)
	{
			sprintf( acErrMsg,"ȡ�����ֵ��Ʒ���ձ��쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L300" );
	    		goto ErrExit;
	}
	if(type[0]=='1')
	{
		strcpy(old_prdt_no,sPrdt_jz_dz.jz_prdt_no);
	}else if(type[0]=='2')
	{
		strcpy(old_prdt_no,sPrdt_jz_dz.prdt_no);
	}
	if(strcmp(prod_no,old_prdt_no)!=0)
	{
			sprintf( acErrMsg,"���Ǹô����Ʒ��Ӧ�ļ�ֵ��Ʒ�� ԭ��Ʒ����=[%s]���²�Ʒ����=��%s��",old_prdt_no,prod_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L301" );
	    		goto ErrExit;
	}
	
	/*��ʼ���¼�ֵ��Ʒת����*/
	if(type[0]=='1')
	{
				
				ret=New_jz_prdt_Sel(g_pub_tx.reply,&sNew_jz_prdt,"pact_no='%s' and ac_no='%s'",g_ln_mst.pact_no,g_mdm_ac_rel.ac_no);				
				if(ret&&ret!=100)
				{
						sprintf( acErrMsg,"��ѯ�����ֵת������� ret=%d,g_pub_tx.reply=[%s]",ret,g_pub_tx.reply);
							WRITEMSG
						strcpy( g_pub_tx.reply ,"L302" );
				    		goto ErrExit;
				}else if(ret==100)
				{
					vtcp_log("û�м�ֵ���ݣ���ʼ����");
					strcpy(sNew_jz_prdt1.pact_no,g_ln_mst.pact_no);
					strcpy(sNew_jz_prdt1.ac_no,g_mdm_ac_rel.ac_no);
					strcpy(sNew_jz_prdt1.ac_name,g_ln_mst.name);
					strcpy(sNew_jz_prdt1.old_prdt_no,g_ln_mst.prdt_no);
					strcpy(sNew_jz_prdt1.old_prdt_name,g_ln_parm.title);					
					strcpy(sNew_jz_prdt1.new_prdt_no,prod_no);
					strcpy(sNew_jz_prdt1.new_prdt_name,ln_parm_new.title);
					strcpy(sNew_jz_prdt1.id_no,g_mdm_ac_rel.id_no);
					sNew_jz_prdt1.bj_amt=g_ln_mst.bal;
					sNew_jz_prdt1.in_lo_intst=g_ln_mst.in_lo_intst;
					sNew_jz_prdt1.out_lo_intst=g_ln_mst.out_lo_intst;
					strcpy(sNew_jz_prdt1.sts,"0");
					
					/*��ʼ�����ֵת��������*/
						New_jz_prdt_Debug(&sNew_jz_prdt1);
					ret=New_jz_prdt_Ins(sNew_jz_prdt1,g_pub_tx.reply);
					if(ret)
					{
						
							sprintf( acErrMsg,"��������ֵת������� ret=%d,g_pub_tx.reply=[%s]",ret,g_pub_tx.reply);
							WRITEMSG
							strcpy( g_pub_tx.reply ,"D107" );
				    		goto ErrExit;
						
					}
				}
				
						if(sNew_jz_prdt.sts[0]=='1')
						{
									sprintf( acErrMsg,"���˺��Ѿ��Ǽ�ֵ��Ʒ������������ֵ������ ");
									WRITEMSG
									strcpy( g_pub_tx.reply ,"L303" );
						    		goto ErrExit;
						}
				
	}else if(type[0]=='2')
	{
				ret=New_jz_prdt_Sel(g_pub_tx.reply,&sNew_jz_prdt,"pact_no='%s' and ac_no='%s'",g_ln_mst.pact_no,g_mdm_ac_rel.ac_no);				
				if(ret)
				{
						sprintf( acErrMsg,"��ѯ�����ֵת������� ret=%d,g_pub_tx.reply=[%s]",ret,g_pub_tx.reply);
							WRITEMSG
						strcpy( g_pub_tx.reply ,"L302" );
				    		goto ErrExit;
				}
				if(sNew_jz_prdt.sts[0]=='0')
				{
							sprintf( acErrMsg,"���˺��Ѿ���������Ʒ����������ת�������� ");
							WRITEMSG
							strcpy( g_pub_tx.reply ,"L303" );
				    		goto ErrExit;
				}
		
	}
				
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�����ֵת��¼��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����ֵת��¼��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    if ( strcmp( g_pub_tx.reply , "L018" )==0 )
    {
    	strcpy( g_pub_tx.reply , "L115" );
    }
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    

int data_set_struct()
{    
	int ret=0;
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("0310" , g_pub_tx.ac_no); 		/* �����˺� */
	g_pub_tx.ac_seqn = 0;				/* �˺���� */
	get_zd_data("0930" , type); 			/* ��������1-����ת��ֵ��2-��ֵת���� */
	get_zd_data("0240" , prod_no); 			/* �²�Ʒ���� */

	WRITEMSG						
	return 0;
}

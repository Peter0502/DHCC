/*************************************************
* �� �� ��: spD114.c
* ����������   
*              ������--ת��
*
* ��    ��:jack
* �������:2004��12��16��
* �޸ļ�¼��   
* ��    ��:
* �� �� ��:
* �޸�����:
* 
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"

char brno[6];
char old_brno[6];
int dt_flag;

spD114()
{
	int ret=0;
	char wherelist[200];
	char comm[100];
	
	dt_flag=0;
	pub_base_sysinit();
		
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
		
	get_zd_data("1011",g_pub_tx.ac_no);	
	get_zd_int("1012",&g_pub_tx.ac_seqn);
	get_zd_data("0910",brno);			
		
	strcpy(old_brno,g_pub_tx.tx_br_no);
	strcpy(g_pub_tx.opn_br_no,brno);
	
	if( strcmp( old_brno , brno )==0 )
	{
		sprintf(acErrMsg,"ת�������������������Ϊͬһ����,���ɰ���!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L221" );		
		goto ErrExit;
	}
	set_zd_double("0410",0.00 );
	
	sprintf( wherelist,"opn_br_no='%s'",old_brno );
	if(strlen(g_pub_tx.ac_no))
	{
		ret=Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_no='%s' and ac_seqn=%d ", g_pub_tx.ac_no,g_pub_tx.ac_seqn);
		if(ret)	goto ErrExit;
		else if(ret==0)
		{
		 	ret=Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst , "ac_id=%ld and ac_seqn=%d ",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		 	if(ret==0) dt_flag=1;
		  
		  ret=Td_mst_Sel(g_pub_tx.reply, &g_td_mst , "ac_id=%ld and ac_seqn=%d ",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);	
		 	if(ret==0) dt_flag=2;
		}
		sprintf( comm ," and ac_id=%ld and ac_seqn=%d " , g_mdm_ac_rel.ac_id , g_mdm_ac_rel.ac_seqn );
		strcat( wherelist , comm );
	}
	
	if(dt_flag==1)
	   ret= Dd_mst_Dec_Upd( g_pub_tx.reply,wherelist);	
	else if(dt_flag==2)
	   ret= Td_mst_Dec_Upd( g_pub_tx.reply,wherelist);
	else if((dt_flag==0)&&(!strlen(g_pub_tx.ac_no)))
		{
			ret= Dd_mst_Dec_Upd( g_pub_tx.reply,wherelist);	
			ret= Td_mst_Dec_Upd( g_pub_tx.reply,wherelist);
		}
	 else ERR_DEAL 	 
	 	
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	if((dt_flag==0)&&(!strlen(g_pub_tx.ac_no)))
	{
	  while(1)
	  {
	   ret=Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);	
	   if( ret==100 )
		 {
		 	sprintf( acErrMsg,"�޼�¼[%d]",ret);
		 	WRITEMSG
		 	break;
		 }else if(ret){
		 	sprintf(acErrMsg,"FETCH FOR UPDATE ERROR [%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;	
		 }
		 
		 ret=Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld ", g_dd_mst.ac_id );

		 if(ret==100)
	 	 {
	 	  sprintf(acErrMsg,"���˺Ų�����,����!!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"L161");
      goto ErrExit;		
	 	 }else ERR_DEAL
	 	
	 	 ret=Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm , "prdt_no='%s' ",g_dd_mst.prdt_no );	
		 ERR_DEAL  	
		 set_zd_data(DC_CODE,g_dd_parm.dc_code);
	 		 	
	 	 strcpy( g_pub_tx.opn_br_no , brno );		
		 set_zd_double("0410",g_dd_mst.bal);	
		 set_zd_data("0210","01");
		 set_zd_data("0660","1" );	/*�跽����*/
		 strcpy(g_pub_tx.sub_tx_code,"D114");
		 strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
	   ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		 if (ret != 0)
		 {
		 	sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
		 	WRITEMSG
		 	goto ErrExit;
		 }
		 set_zd_double("0410",0.00 );
		 
		 strcpy( g_dd_mst.opn_br_no, brno );
		 
		 ret = Dd_mst_Upd_Upd(g_dd_mst, g_pub_tx.reply  );
	   if ( ret )
		 {
		 	sprintf( acErrMsg,"Dd_mst_Upd_Upd ����[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
		 }	
	  }
	  while(1)
	  {
	   ret=Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);	
	   if( ret==100 )
		 {
		 	sprintf( acErrMsg,"�޼�¼[%d]",ret);
		 	WRITEMSG
		 	break;
		 }else if(ret){
		 	sprintf(acErrMsg,"FETCH FOR UPDATE ERROR [%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;	
		 }
		 
		 ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",g_td_mst.ac_id);

		 if(ret==100)
	 	 {
	 	  sprintf(acErrMsg,"���˺Ų�����,����!!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"L161");
      goto ErrExit;		
	 	 }else ERR_DEAL
	 	 
	   ret=Td_parm_Sel(g_pub_tx.reply, &g_td_parm , "prdt_no='%s' ",g_td_mst.prdt_no );	
		 ERR_DEAL  	
	 	 set_zd_data(DC_CODE,g_td_parm.dc_code);
	 	 		 	 
	 	 strcpy( g_pub_tx.opn_br_no , brno );		
		 set_zd_double("0410",g_td_mst.bal);	
		 	
		 set_zd_data("0210","01");
		 set_zd_data("0660","1" );	/*�跽����*/
		 strcpy(g_pub_tx.sub_tx_code,"D114");
		 strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);
	   ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		 if (ret != 0)
		 {
		 	sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
		 	WRITEMSG
		 	goto ErrExit;
		 }
		 set_zd_double("0410",0.00 );
		 
		 strcpy( g_td_mst.opn_br_no, brno );
		 
		 ret = Td_mst_Upd_Upd(g_td_mst, g_pub_tx.reply  );
	   if ( ret )
		 {
		 	sprintf( acErrMsg,"Td_mst_Upd_Upd ����[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
		 }		
	  }		
	}
else
	while(1)
	{
	 if(dt_flag==1)
	 	 ret=Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
	 else if(dt_flag==2)
	 	 ret=Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
	 	 
	 if( ret==100 )
		{
			sprintf( acErrMsg,"�޼�¼[%d]",ret);
			WRITEMSG
			break;
		}else if(ret){
			sprintf(acErrMsg,"FETCH FOR UPDATE ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;	
		}
	
	  if(dt_flag==1)
		  ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",g_dd_mst.ac_id);
		else if(dt_flag==2)
			ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",g_td_mst.ac_id);

	 	if(ret==100)
	 	{
	 	 sprintf(acErrMsg,"���˺Ų�����,����!!");
     WRITEMSG
     strcpy(g_pub_tx.reply,"L161");
     goto ErrExit;		
	 	}else ERR_DEAL
	 	  	
	  if(dt_flag==1)
	  {
	  	ret=Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm , "prdt_no='%s' ",g_dd_mst.prdt_no );	
		  ERR_DEAL  	
		  set_zd_data(DC_CODE,g_dd_parm.dc_code);
	  }
	  else if(dt_flag==2)
	  {
	   ret=Td_parm_Sel(g_pub_tx.reply, &g_td_parm , "prdt_no='%s' ",g_td_mst.prdt_no );	
		 ERR_DEAL  	
	 	 set_zd_data(DC_CODE,g_td_parm.dc_code);	
	  }
	  
	  strcpy( g_pub_tx.opn_br_no , brno );		
		if(dt_flag==1)  
		  set_zd_double("0410",g_dd_mst.bal);		
	  else if(dt_flag==2)
		  set_zd_double("0410",g_td_mst.bal);
		
		set_zd_data("0210","01");
		set_zd_data("0660","1" );	/*�跽����*/
		strcpy(g_pub_tx.sub_tx_code,"D114");
		if(dt_flag==1)
	    strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
	  else if(dt_flag==2)
		  strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_double("0410",0.00 );
		
		if(dt_flag==1)
		  strcpy( g_dd_mst.opn_br_no, brno );
		else if(dt_flag==2)
			strcpy( g_td_mst.opn_br_no, brno );
		
		if(dt_flag==1)
	    ret = Dd_mst_Upd_Upd(g_dd_mst, g_pub_tx.reply  );
	  else if(dt_flag==2)		
      ret = Td_mst_Upd_Upd(g_td_mst, g_pub_tx.reply  );
		
		if ( ret )
		{
			sprintf( acErrMsg,"mst_Upd_Upd ����[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}	
	}
 Dd_mst_Clo_Upd( );
 Td_mst_Clo_Upd( );
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"������ת��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;    
ErrExit:
    sprintf(acErrMsg,"������ת��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

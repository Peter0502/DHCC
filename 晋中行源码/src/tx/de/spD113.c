/*************************************************
* 文 件 名: spD113.c
* 功能描述：   
*              存款剥离--转出
*
* 作    者:jack
* 完成日期:2004年12月16日
* 修改记录：   
* 日    期:
* 修 改 人:
* 修改内容:
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

spD113()
{
  int ret=0;
	char wherelist[200];
	char comm[100];	
	dt_flag=0;
		
	memset(&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));

	pub_base_sysinit();
	
	get_zd_data("1011",g_pub_tx.ac_no);		
	get_zd_int("1012",&g_pub_tx.ac_seqn);
	get_zd_data("0910",brno);	
	
	strcpy(old_brno,g_pub_tx.tx_br_no );
	
	if( strcmp( old_brno , brno )==0 )
	{
		sprintf(acErrMsg,"转入机构与存款现所属机构为同一机构,不可剥离!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L221" );		
		goto ErrExit;
	}
	
	sprintf( wherelist," opn_br_no='%s' ",old_brno );
	if( strlen(g_pub_tx.ac_no) )
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel , "ac_no='%s' and ac_seqn=%d ", g_pub_tx.ac_no,g_pub_tx.ac_seqn);
		if(ret)	goto ErrExit;
		else if(ret==0)
		{
		 	ret=Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst , "ac_id=%ld and ac_seqn=%d ",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		 	if(ret==0) dt_flag=1;
		  
		  ret=Td_mst_Sel(g_pub_tx.reply, &g_td_mst , "ac_id=%ld and ac_seqn=%d ",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);	
		 	if(ret==0) dt_flag=2;
		}
		sprintf( comm ,"and ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		strcat( wherelist , comm );
	}
	
	if (dt_flag==1)
		ret=Dd_mst_Dec_Upd(g_pub_tx.reply,wherelist);
	else if(dt_flag==2)
	  ret=Td_mst_Dec_Upd(g_pub_tx.reply,wherelist);
	else if((dt_flag==0)&&(!strlen(g_pub_tx.ac_no)))	  
		{
			ret=Dd_mst_Dec_Upd(g_pub_tx.reply,wherelist);
			ret=Td_mst_Dec_Upd(g_pub_tx.reply,wherelist);
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
			ret=Dd_mst_Fet_Upd(&g_dd_mst, g_pub_tx.reply );
			if( ret==100 )
	    {
	    	sprintf( acErrMsg,"无记录[%d]",ret);
	    	WRITEMSG
	    	break;
	    }else ERR_DEAL
	    
	    ret=Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld",g_dd_mst.ac_id);
	    	
	    if(ret==100)
	 	  {
	 	   sprintf(acErrMsg,"该账号不存在,请检查!!");
       WRITEMSG
       strcpy(g_pub_tx.reply,"L161");
       goto ErrExit;		
	 	  }else ERR_DEAL
	 	
	 	  ret=Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm , "prdt_no='%s' ",g_dd_mst.prdt_no );	
		  ERR_DEAL  	
		  set_zd_data(DC_CODE,g_dd_parm.dc_code);
		  
	 	 	set_zd_double("0410",-g_dd_mst.bal);		
	  	set_zd_data("0210","01");
	  	set_zd_data("0660","1" );
	  	strcpy(g_pub_tx.sub_tx_code,"D113");
	   	strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
	    ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
	  	if (ret != 0)
	   	{
	  		sprintf(acErrMsg,"会计记帐不成功!!");
	  		WRITEMSG
	  		goto ErrExit;
	   	}
	  	set_zd_double("0410",0.00 );
	  
	 		ret = Dd_mst_Upd_Upd(g_dd_mst, g_pub_tx.reply  );
	  	if ( ret )
	 		{
	 		 sprintf( acErrMsg,"mst_Upd_Upd 出错[%d]",ret);
		   WRITEMSG
		   goto ErrExit;
 		  }
	 }
	 while(1)
		{
			ret=Td_mst_Fet_Upd(&g_td_mst, g_pub_tx.reply );
			if( ret==100 )
	    {
	    	sprintf( acErrMsg,"无记录[%d]",ret);
	    	WRITEMSG
	    	break;
	    }else ERR_DEAL
	    
	    ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",g_td_mst.ac_id);
	    if(ret==100)
	 	  {
	 	   sprintf(acErrMsg,"该账号不存在,请检查[%ld]!!",g_td_mst.ac_id);
       WRITEMSG
       strcpy(g_pub_tx.reply,"L161");
       goto ErrExit;		
	 	  }else ERR_DEAL
	 	  	 	 
	 	  ret=Td_parm_Sel(g_pub_tx.reply, &g_td_parm , "prdt_no='%s' ",g_td_mst.prdt_no );	
	  	ERR_DEAL  	
 	 	  set_zd_data(DC_CODE,g_td_parm.dc_code);

	 	  set_zd_double("0410",-g_td_mst.bal);		
	  	set_zd_data("0210","01");
	  	set_zd_data("0660","1" );
	  	strcpy(g_pub_tx.sub_tx_code,"D113");
	 	 	strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);
	    ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
	  	if (ret != 0)
	   	{
	  		sprintf(acErrMsg,"会计记帐不成功!!");
	  		WRITEMSG
	  		goto ErrExit;
	   	}
	  	set_zd_double("0410",0.00 );
	  
	 		ret = Td_mst_Upd_Upd(g_td_mst, g_pub_tx.reply  );
	  	if ( ret )
	 		{
	 		 sprintf( acErrMsg,"mst_Upd_Upd 出错[%d]",ret);
		   WRITEMSG
		   goto ErrExit;
 		  }
	 }
 }	
else
	while(1)
	{
	 if(dt_flag==1)
	 	 ret=Dd_mst_Fet_Upd(&g_dd_mst, g_pub_tx.reply );
	 else if(dt_flag==2)
	 	 ret=Td_mst_Fet_Upd(&g_td_mst, g_pub_tx.reply);
	 	 
	 if( ret==100 )
	  {
	  	sprintf( acErrMsg,"无记录[%d]",ret);
	  	WRITEMSG
	  	break;
	  }else ERR_DEAL
	  
	 /******
	  if(dt_flag==1)
	    ret=Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld and ac_seqn=%d ", g_dd_mst.ac_id,g_dd_mst.ac_seqn);
	  else if(dt_flag==2)
	  	ret=Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld and ac_seqn=%d ", g_td_mst.ac_id,g_td_mst.ac_seqn);
	 *******/
	  if(dt_flag==1)
	    ret=Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld  ", g_dd_mst.ac_id);
	  else if(dt_flag==2)
	  	ret=Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld  ", g_td_mst.ac_id);
	 
	  if(ret==100)
	 	{
	 	 sprintf(acErrMsg,"该账号不存在,请检查!!");
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
	  /* 进行会计记帐 */   
	  if(dt_flag==1)  
	  set_zd_double("0410",-g_dd_mst.bal);		
	  else if(dt_flag==2)
	  set_zd_double("0410",-g_td_mst.bal);
	  
	  set_zd_data("0210","01");
	  set_zd_data("0660","1" );
	  strcpy(g_pub_tx.sub_tx_code,"D113");
	  
	  if(dt_flag==1)
	   strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
	  else if(dt_flag==2)
	   strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);
	  
	  ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
	  if (ret != 0)
	   {
	  	sprintf(acErrMsg,"会计记帐不成功!!");
	  	WRITEMSG
	  	goto ErrExit;
	   }
	  set_zd_double("0410",0.00 );
	  
	 if(dt_flag==1)
	     ret = Dd_mst_Upd_Upd(g_dd_mst, g_pub_tx.reply  );
	 else if(dt_flag==2)		
      ret = Td_mst_Upd_Upd(g_td_mst, g_pub_tx.reply  );
	 
	  if ( ret )
	  {
	   sprintf( acErrMsg,"mst_Upd_Upd 出错[%d]",ret);
	   WRITEMSG
	   goto ErrExit;
   }
 }
 Dd_mst_Clo_Upd( );
 Td_mst_Clo_Upd( );
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"存款剥离转出成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"存款剥离转出失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return -1;
}

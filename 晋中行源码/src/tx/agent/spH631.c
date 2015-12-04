/*************************************************
* 文 件 名:  spH631.c
* 功能描述： 查询代理客户帐号状态
*
* 作    者:  jack
* 完成日期： 2004年5月26日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_peritemmap_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "mo_loss_c.h"
int spH631()
{
		struct mdm_ac_rel_c sMdm_ac_rel;
   		struct ag_peritemmap_c sAg_peritemmap;
		struct dd_mst_c sDd_mst1;
		struct mo_loss_c sMo_loss;
		int ret = 0;
  		char cUnit_no[5];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		char cMsg[51];
		FILE *fp=NULL;
		char filename[100];
		char f_id_type[21];
		char f_ac_sts[15];
		char f_hold_sts[10];
		char f_sttl_type[7];
		char cAc_no[20];
		char cNacno[20];
		memset(cUnit_no,0x00,sizeof(cUnit_no));
	  	memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	  	memset(&sDd_mst1,0x00,sizeof(struct dd_mst_c));
	  	memset(&sMo_loss,0x00,sizeof(struct mo_loss_c));
	  	memset(&sAg_peritemmap,0x00,sizeof(struct ag_peritemmap_c));
	  	memset(cNacno,0x00,sizeof(cNacno));
	  	memset(cMsg,0x00,sizeof(cMsg));
	  	memset(f_id_type,0x00,sizeof(f_id_type));
	  	memset(f_id_type,0x00,sizeof(f_id_type));
	  	memset(f_ac_sts,0x00,sizeof(f_ac_sts));
	  	memset(f_sttl_type,0x00,sizeof(f_sttl_type));
	  	pub_base_sysinit();
	  	get_zd_data( "0920", cUnit_no);
	  	vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,cUnit_no);
	  	ret=Ag_peritemmap_Dec_Sel(g_pub_tx.reply,"unit_no='%s' and stat='0'",cUnit_no);
	  	ERR_DEAL
    		ttlnum=0;
    		while(1)
	  	{
	    	memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	  		memset(&sDd_mst1,0x00,sizeof(struct dd_mst_c));
	  		memset(&sMo_loss,0x00,sizeof(struct mo_loss_c));
	  		memset(cNacno,0x00,sizeof(cNacno));
	  		memset(&sAg_peritemmap,0x00,sizeof(struct ag_peritemmap_c));
	  		memset(f_id_type,0x00,sizeof(f_id_type));
	  		memset(f_ac_sts,0x00,sizeof(f_ac_sts));
	  		memset(f_hold_sts,0x00,sizeof(f_hold_sts));
	  		memset(f_sttl_type,0x00,sizeof(f_sttl_type));
	  		ret=Ag_peritemmap_Fet_Sel(&sAg_peritemmap,g_pub_tx.reply);
	  		if( ret==100 ) break;
			ERR_DEAL
			pub_base_old_acno(sAg_peritemmap.ac_no);
	  	vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,sAg_peritemmap.ac_no);
      ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",sAg_peritemmap.ac_no);
	  	ERR_DEAL
	  	ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst1,"ac_id='%ld' ",sMdm_ac_rel.ac_id);
			ERR_DEAL			
			if(sDd_mst1.hold_sts[0]=='0'||sMdm_ac_rel.note_sts[0]=='0'||sDd_mst1.sttl_type[0]=='D')
			{
	       continue;
	    }
	  	if( !ttlnum )
	  		{	  		
	  		pub_base_AllDwnFilName( filename );
	  		fp = fopen( filename,"w" );
	  		if( fp==NULL )
	  		{
	  			sprintf(acErrMsg," open file [%s] error ",filename );
	  			WRITEMSG
	  			strcpy( g_pub_tx.reply,"S047" );
	  			goto ErrExit;
	  		}
	  		/**显示列标题**/
	  		fprintf( fp,"~单位编号|代理编号|个人账号|姓    名|证件号码|证件类型|账号状态|冻结状态|结算类型|新开账号|\n");
	  	  }
			    
			    pub_base_dic_show_str(f_id_type,"id_type",sMdm_ac_rel.id_type);
				  pub_base_dic_show_str(f_hold_sts,"hold_sts",sDd_mst1.hold_sts);
				  pub_base_dic_show_str(f_sttl_type,"sttl_type",sDd_mst1.sttl_type);
	    		ret=Mo_loss_Sel(g_pub_tx.reply,&sMo_loss,"ac_no='%s'",sAg_peritemmap.ac_no);
	    		memcpy(cNacno,sMo_loss.new_ac_no,sizeof(cNacno));
	  			pub_base_dic_show_str(f_ac_sts,"loss_type",sMo_loss.loss_ind);
	  			fprintf( fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",sAg_peritemmap.unit_no,sAg_peritemmap.item_no,sAg_peritemmap.ac_no,sDd_mst1.name,sMdm_ac_rel.id_no,sMdm_ac_rel.id_type,f_ac_sts,f_hold_sts,f_sttl_type,cNacno);
	  	 		ttlnum++;
	  }
	  Pay_agent_bk_Clo_Sel();
	  
	  if( !ttlnum )
	  {
	  	sprintf(cMsg,"该单位的客户账号正常!");	  	
	  }
	  else
	  {
	  	fclose(fp);   
	  	set_zd_data( DC_FILE_SND,"1" );
	  }
	  vtcp_log("[%s][%d]=============[%d]\n",__FILE__,__LINE__,ttlnum);
	 	set_zd_data("0130",cMsg);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}                                           

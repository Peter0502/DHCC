/*************************************************
* 文 件 名:  sp8215.c
* 功能描述： 查询打印个人存款证明
*
* 作    者:  
* 完成日期： 
*
* 修改记录：
* 日   期:2004.12.7
* 修 改 人:
* 修改内容:非本网点或本网点下属网点帐户信息不能查询
 交易配置：
	insert into tx_def values('8215','查询打印个人存款证明','00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
	insert into tx_flow_def values('8215','0','8215','#$');
	insert into tx_sub_def values('8215','查询打印个人存款证明','sp8215','0','0');
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

int sp8215()
{
	struct dd_mst_c vdd_mst;
	struct td_mst_c vtd_mst;
	struct dd_parm_c vdd_parm;
	struct td_parm_c vtd_parm;
	struct mdm_attr_c mdm_attr;
	struct prdt_ac_id_c prdt_ac_id_c;
  struct mdm_ac_rel_c mdm_ac_rel;

		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char t_name[41];
		char ac_sts[41];
		char parm_title[41];
		char prt_type[2];
		char cTel[5];  /*操作员号*/
		char cVtype[4];/*凭证类型*/
	  char cVocnum[17];/*凭证号码*/
		double  t_bal=0.00;
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		long ac_num=0;
		long end_date=0;
		int ret=0;
		FILE *fp;
		
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
  memset( wherelist,0,sizeof(wherelist) );
	memset( &mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );

	pub_base_sysinit();

  get_zd_data("0070",cTel);
	get_zd_data("0900",cVtype);
	get_zd_data("0600",cVocnum);
	get_zd_data("0680",prt_type );
	get_zd_data("0670",mdm_ac_rel.id_type );
	get_zd_data("0620",mdm_ac_rel.id_no );
/*
 if( strlen(prt_type) )
 	{
    if(prt_type[0]=='1')
     {
     	
	     ret=pub_com_ChkNoteUse(cVtype);
	     if( ret )
	     {
	    	sprintf(acErrMsg,"凭证处于停用状态!");
	    	WRITEMSG
	    	goto ErrExit;
	     } 
	    	ret=pub_com_NoteUse(0,0,cVtype,cVocnum,cVocnum,cTel);
	    	if( ret )
	    	{
	    		sprintf(acErrMsg,"柜员使用凭证错误!");
	    		WRITEMSG
	    		goto ErrExit;
	    	} 
	    	
	    	vtcp_log("AP MSG: 得到要打印的纪录![%s][%d]",__FILE__,__LINE__);
     	
     	sprintf(acErrMsg," print NO.[%s] ",prt_type );
	   			WRITEMSG
	   	goto GoodExit;
     }else
     {
     	sprintf(acErrMsg," print NO.[%s] ",prt_type );
	   			WRITEMSG
     	goto GoodExit;
     }
   }
  */
	/**组成查询条件**/
	
	pub_base_strpack( mdm_ac_rel.id_type );
	if(  strlen(mdm_ac_rel.id_type) )
	{
		sprintf( tmpstr," id_type='%s' and",mdm_ac_rel.id_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( mdm_ac_rel.id_no );
	if(  strlen(mdm_ac_rel.id_no) )
	{
		sprintf( tmpstr," id_no='%s' and",mdm_ac_rel.id_no );
		strcat( wherelist,tmpstr );
	}
	


	/**组成查询**/
	strcat(wherelist," note_sts<>'*' ORDER BY opn_br_no,ac_id , ac_seqn ") ;
	
	ret=Mdm_ac_rel_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mdm_ac_rel_Fet_Sel( &mdm_ac_rel, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
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
			fprintf( fp,"~账号|户名|@产品名称|$余额|开户日期|到期日期|@凭证类型|@凭证号码|@帐户状态|\n" );
		}
		/* 加上ac_seqn 老报错 20131030 wudawei
    if( mdm_ac_rel.ac_seqn>0 )
			ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_seqn=%d and ac_id_type in ('1','2') ",mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn);
		else
		end 20131030 */
			ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_id_type in ('1','2') ",mdm_ac_rel.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"账号不存在![%ld][%d]",mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		
    switch( prdt_ac_id_c.ac_id_type[0] )
	 {
		 case '1':	/*活期*/
		 	/* 加上ac_seqn 老报错 20131030 wudawei
				ret = Dd_mst_Sel( g_pub_tx.reply,&vdd_mst," ac_id=%ld and ac_seqn=%d ",mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn );
				end 20131030*/
				ret = Dd_mst_Sel( g_pub_tx.reply,&vdd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id);
				ERR_DEAL
				
				/**读取产品参数**/
	      ret = Dd_parm_Sel( g_pub_tx.reply,&vdd_parm," prdt_no='%s' ",vdd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vdd_mst.bal;
				end_date=vdd_mst.val_date;
				strcpy(t_name,vdd_mst.name);
				strcpy(parm_title,vdd_parm.title);
        pub_base_dic_show_str(ac_sts,"dd_ac_sts",vdd_mst.ac_sts);
        break;
     case '2':	/*定期*/
     	  ret = Td_mst_Sel( g_pub_tx.reply,&vtd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id );
				ERR_DEAL
				
				/**读取产品参数**/
	      ret = Td_parm_Sel( g_pub_tx.reply,&vtd_parm," prdt_no='%s' ",vtd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vtd_mst.bal;
				end_date=vtd_mst.mtr_date;
				strcpy(t_name,vtd_mst.name);
				strcpy(parm_title,vtd_parm.title);
    		pub_base_dic_show_str(ac_sts,"td_ac_sts",vtd_mst.ac_sts);
    		break;
     default:	
				sprintf(acErrMsg,"无此账号类型![%s]",prdt_ac_id_c.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P104");
				goto ErrExit;
   }
   ret = Mdm_attr_Sel( g_pub_tx.reply,&mdm_attr," mdm_code='%s' ",mdm_ac_rel.mdm_code );
	 ERR_DEAL
      
		/**显示内容**/
	  fprintf( fp, "%s|%10s|%s|%.2lf|%ld|%ld|%s|%s|%s|\n", \ 
		 mdm_ac_rel.ac_no,t_name,parm_title,t_bal,mdm_ac_rel.beg_date,end_date,mdm_attr.mdm_name,mdm_ac_rel.note_no,ac_sts); 
	 ttlnum++;
	}

	ret=Mdm_ac_rel_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

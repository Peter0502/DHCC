/*************************************************
* 文 件 名:  sp9983.c
* 功能描述： 备注管理快捷查询
*			
*
* 作    者:  
* 完成日期： 2005年8月08日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#define EXTERN
#include "public.h"
#include "remark_ctl_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

int sp9983()
{
	int		ret;
	struct	remark_ctl_c rem;
	struct prdt_ac_id_c g_prdt_ac_id;
	struct mdm_attr_c g_mdm_attr;

	char    cur_no[5],cif_type[2],note_no[20],prdt_name[31];
	char	flag[2];
	char	filename[100];
	char	rem_code[11];
	int		i;
	int		tag;
	long	num;
	FILE	*fp;

	memset( &rem, 0x00, sizeof(struct remark_ctl_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	ret=0;
	i=0;

	/** 取值、赋值 **/	

	get_zd_data("0660", flag);		/*处理标志*/
	get_zd_data("0860", rem.rem_code);	/*备注编号*/
	get_zd_data("1011", rem.ac_no);
	get_zd_long("1012",&rem.ac_seqn);
	sprintf(acErrMsg,"CHK flag=[%s] ac_no=[%s] seqn=[%ld] rem_code=[%s]",
		flag,rem.ac_no,rem.ac_seqn,rem.rem_code);
	WRITEMSG

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",rem.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质帐号对照表不存在该记录!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找帐号出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='1' )
	{
		sprintf(acErrMsg,"该介质已挂失!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"该介质已挂失换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"该介质已被换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"该介质已部提换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"该介质已做销户处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

/***
	if( !strncmp(g_mdm_ac_rel.mdm_code,"0016",4) )
	{
		sprintf(acErrMsg,"定期一本通帐户不允许存款!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O209");
		goto ErrExit;
	}
***/

	ret = Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr, "mdm_code='%s'", \
						g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质属性表不存在该记录!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( rem.ac_seqn<0 || rem.ac_seqn>9999 )
	{
		ret = Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
						 g_mdm_ac_rel.ac_id);
	}
	else
	{
		ret = Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld and ac_seqn=%d ", \
						 g_mdm_ac_rel.ac_id,rem.ac_seqn);
	}
	if( ret==100 )
	{
		sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%ld][%d][%d]",g_mdm_ac_rel.ac_id,rem.ac_seqn,ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找帐号出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( rem.ac_seqn<0 || rem.ac_seqn>9999 )
	{
		if( g_mdm_ac_rel.ac_seqn==9999 )
		{
			ret = sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
			if( ret<0 )
			{
				sprintf(acErrMsg,"sql_count error!! [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			else if( ret==1 ) 
				g_pub_tx.ac_seqn = g_prdt_ac_id.ac_seqn;
			else if( ret>1 )
			{
				set_zd_long("101E",g_prdt_ac_id.ac_seqn);
		 		g_pub_tx.ac_seqn = g_prdt_ac_id.ac_seqn;
				tag = 1;
			}
		}
		else if(g_mdm_ac_rel.ac_seqn)
			g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn;
	}
	else
	{
		tag=0;
		g_pub_tx.ac_seqn = rem.ac_seqn;
	}

vtcp_log("AAAA prdt_no=[%s] acid=[%d]",g_prdt_ac_id.prdt_no,g_prdt_ac_id.ac_id);
	/*取信息*/
	if(tag==0)
	{
vtcp_log("AAAA prdt_no=[%s] acid=[%d]",g_prdt_ac_id.prdt_no,g_prdt_ac_id.ac_id);
		if(g_prdt_ac_id.prdt_no[0]=='1')
		{
			ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d ", \
							g_prdt_ac_id.ac_id,g_pub_tx.ac_seqn );
			if( ret==100 )
			{
				sprintf(acErrMsg,"帐号不存在!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"查找错误!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}

			strcpy(g_pub_tx.name,g_dd_mst.name);
			g_pub_tx.tx_amt1=g_dd_mst.bal;

			if(g_dd_mst.ac_sts[0]=='*')	
			{
				sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
 				WRITEMSG
  				strcpy(g_pub_tx.reply,"P245");
   				goto ErrExit;
			}

			ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
						 	  g_dd_mst.prdt_no);
			strcpy(prdt_name,g_dd_parm.title);
			strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
			strcpy(cif_type,g_dd_parm.cif_type);
		}
		else if(g_prdt_ac_id.prdt_no[0]=='2')
		{
			ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d", \
						 	 g_prdt_ac_id.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
				sprintf(acErrMsg,"帐号不存在!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"查找错误!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}

			strcpy(g_pub_tx.name,g_td_mst.name);
			g_pub_tx.tx_amt1=g_td_mst.bal;

			if(g_td_mst.ac_sts[0]=='*')	
			{
				sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
 				WRITEMSG
  				strcpy(g_pub_tx.reply,"P245");
   				goto ErrExit;
			}

			ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
								g_td_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"产品不存在!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"查找错误!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}

			strcpy(prdt_name,g_td_parm.title);
			strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
			strcpy(cif_type,g_td_parm.cif_type);
		}
		/***
		else
		{
			sprintf(acErrMsg,"非村取款类帐户![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O166");
			goto ErrExit;
		}
		***/

	}
	/* 输出变量 */
	set_zd_data("1018",g_pub_tx.name);          	/*户名*/
	set_zd_data("101A",g_pub_tx.cur_no);          	/*户名*/
vtcp_log("------look------ac_seqn=[%d]",g_pub_tx.ac_seqn);
	set_zd_long("1012",g_pub_tx.ac_seqn);          	/*户名*/

	set_zd_double("1019",g_pub_tx.tx_amt1);         /*余额*/
	set_zd_data("0670",g_pub_tx.intst_type);    	/*计息类型*/
    set_zd_long("0280",g_pub_tx.cif_no);        	/*客户号*/
/**
	ret = pub_base_my_func("9951","0",g_pub_tx.reply);
	if ( ret )
	{	
		if ( !strncmp(g_pub_tx.reply,"O164",4))
		{ 
			ret = pub_base_my_func("9954","0",g_pub_tx.reply);
			if ( ret )
			{
				sprintf(acErrMsg,"取对公处理失败");
				WRITEMSG
				goto ErrExit;
			}
		}
		else
		{
			sprintf(acErrMsg,"取对私处理失败");
			WRITEMSG
			goto ErrExit;
		}
	}

**/

	if ( flag[0]=='2' || flag[0]=='3' )
	{
		pub_base_strpack(rem.rem_code);
		if ( strlen(rem.rem_code))
		{
			if( flag[0]!='2' )
			{
				ret = Remark_ctl_Dec_Sel(g_pub_tx.reply,"ac_id=%ld AND ac_seqn=%d AND rem_code='%s'",
				g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,rem.rem_code);
			}else{
				 ret = Remark_ctl_Dec_Sel(g_pub_tx.reply,"ac_id=%ld AND ac_seqn=%d AND rem_code='%s' \
						AND rem_sts='1' ", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,rem.rem_code);
			}		
		}else{
			if( flag[0]!='2' )
			{
				ret = Remark_ctl_Dec_Sel(g_pub_tx.reply,"ac_id=%ld AND ac_seqn=%d ",g_mdm_ac_rel.ac_id,
								g_pub_tx.ac_seqn);
			}else{
				ret = Remark_ctl_Dec_Sel(g_pub_tx.reply,"ac_id=%ld AND ac_seqn=%d AND rem_sts='1' ",
					g_mdm_ac_rel.ac_id, g_pub_tx.ac_seqn);
			}
		}
		ret = Remark_ctl_Fet_Sel( &rem,g_pub_tx.reply);
		if ( ret == 100 )
		{
			sprintf(acErrMsg,"备注登记簿中无记录ac_no=[%s]ac_seqn=[%d]rem_code=[%s]",
					rem.ac_no,rem.ac_seqn,rem.rem_code);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"备注登记簿错！ac_no=[%s]ac_seqn=[%d]rem_code=[%s]",
				rem.ac_no,rem.ac_seqn,rem.rem_code);
			WRITEMSG
			goto ErrExit;
		}
vtcp_log("CHK ETMP amt=[%lf] rem=[%s]",rem.amt,rem.rem);
		if ( rem.rem_sts[0]=='1' && i ==0 ) 
		{
			i++;
			set_zd_data("0860",rem.rem_code);

			if ( rem.rem_sts[0]=='1' )
				set_zd_data("1016","备注登记");
			else
				set_zd_data("1016","备注注销");
	
			set_zd_data  ("0250",rem.rem);
			set_zd_double("1013",rem.amt);
			set_zd_data  ("0910",rem.br_no);
			set_zd_data  ("0920",rem.tx_tel);
			set_zd_long  ("0440",rem.tx_date);
			set_zd_data( DC_FILE_SND,"2" );
		}
		pub_base_AllDwnFilName( filename );
		fp=fopen(filename,"w");
		if( fp==NULL )
		{
			sprintf( acErrMsg,"open file error [%s]",filename );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047");
			goto ErrExit;
		}
		while(!ret)
		{
vtcp_log("CHK ETMP amt=[%lf] rem=[%s] ret=[%d]",rem.amt,rem.rem,ret);
			/***del by l 20051001
			if ( rem.rem_sts[0]=='1' ) 
			{
			***********/
				if ( i ==0 ) 
				{
					i++;
					set_zd_data("0860",rem.rem_code);

					if ( rem.rem_sts[0]=='1' )
						set_zd_data("1016","备注登记");
					else
						set_zd_data("1016","备注注销");
	
					set_zd_data  ("0250",rem.rem);
					set_zd_double("1013",rem.amt);
					set_zd_data  ("0910",rem.br_no);
					set_zd_data  ("0920",rem.tx_tel);
					set_zd_long  ("0440",rem.tx_date);
					set_zd_data( DC_FILE_SND,"2" );
				}
				rem.rem[20]='\0';
				pub_base_strpack(rem.rem_code);
				pub_base_strpack(rem.rem);
				fprintf(fp,"0860|%s|%s|\n",rem.rem,rem.rem_code);
			/**********
			}
			*********/
			ret = Remark_ctl_Fet_Sel( &rem,g_pub_tx.reply);
			if ( ret ) break;
		}
		Remark_ctl_Clo_Sel();
		fclose(fp);
	}
	if ( flag[0]=='1')
	{
		/**
		ret = Remark_ctl_Dec_Sel(g_pub_tx.reply,"ac_no='%s' ",rem.ac_no);
		ret = Remark_ctl_Fet_Sel( &rem,g_pub_tx.reply);
		**/
		ret = sql_max_string("remark_ctl","rem_code",rem.rem_code,10,"ac_id=%ld and ac_seqn=%d ",
		g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn );
		if (  ret==100 )
		{
			sprintf(acErrMsg,"备注登记簿ac_no=[%s]ac_seqn=[%d]rem_code=[%s]",
				rem.ac_no,rem.ac_seqn,rem.rem_code);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H001");
			goto ErrExit;
		}
		num = atol(rem.rem_code);
		vtcp_log("num =[%d] rem_code=[%s]",num,rem.rem_code);
		sprintf(rem_code,"%06d",num+1);
		set_zd_data("0860",rem_code);
	}

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

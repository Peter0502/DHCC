#include "find_debug.h" 
/*************************************************
* 文 件 名: pubf_base_getacname.c
* 功能描述：1、通过输入的帐号取户名
*  			2、
*        	3、
*        	4、
*        	5、
* 作    者: dong   
* 完成日期：2003年12月16日
* 修改记录：    
* 1. 日    期:
*    修 改 人:
*    修改内容:
*************************************************/

#include "stdio.h"
#include <varargs.h>
#include <sys/stat.h>
#include <sys/types.h>

#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "prdt_ac_id_c.h"
#include "gl_mst_c.h"

#define loc_t sys_loc_t

/**********************************************************************
* 函数名：    pub_base_Getacname
* 函数功能：  通过帐号取户名
* 作者/时间：    
* 
* 参数：
*     输入：帐号
*           
*     输出：户名 帐号ID 序号 客户号
*
*
* 返 回 值: >=0:成功 <0:失败
*           
* 修改历史：
*
********************************************************************/
int Pub_base_Getacname( char  ac_no[25] ,char name[51] ,long *ac_id, long *cif_no, double *bal,char *reply)
{
	struct	com_item_c		g_com_item;
	struct	prdt_ac_id_c	sPrdt_ac_id;
	struct  gl_mst_c     	g_gl_mst;
	int	i;
	int	ret;

	MEMSET_DEBUG(&g_com_item,0x00,sizeof(struct com_item_c));
	MEMSET_DEBUG(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
	i=0;
	ret=0;

	strcpy(g_pub_tx.ac_no,ac_no);					/*帐号*/
	pub_base_old_acno( g_pub_tx.ac_no );

	ldchar(g_pub_tx.ac_no,strlen(g_pub_tx.ac_no),g_pub_tx.ac_no);
	i=strlen(g_pub_tx.ac_no);

	if(i==3||i==5||i==7)							/*输入科目*/
	{
	 	/*根据科目号查询科目名称*/
	 	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",
					  	 g_pub_tx.ac_no);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"该科目号不存在!!acc_hrt=[%s]", g_in_parm.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O074");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf( acErrMsg, "查询公共科目表异常!!ret=[%d]", ret );
		 	WRITEMSG
		 	goto ErrExit;
	 	}
	 	/*根据科目号查询余额及借贷方向*/
	 	ret=Gl_mst_Sel(g_pub_tx.reply,&g_gl_mst,"acc_hrt='%s'",
					   g_com_item.acc_hrt);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"该科目号不存在!!acc_hrt=[%s]",g_com_item.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O075");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"查询公共科目表异常!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}

		/*科目借贷类型*/
		if(g_com_item.dc_ind[0]=='1')
		{
		/**
			set_zd_data("0220","借");			
		**/
			*bal = g_gl_mst.dr_bal;			
		}
		else if(g_com_item.dc_ind[0]=='2')
		{
		/**
			set_zd_data("0220","贷");			
		**/
			*bal = g_gl_mst.cr_bal;			
		}
		else if(g_com_item.dc_ind[0]=='0')		/*双方*/
		{										
			ret=pub_base_CompDblVal(g_gl_mst.dr_bal,g_gl_mst.cr_bal);
			if(ret==1)
			{
			/**
				set_zd_data("0220","借");			
			**/
				*bal = g_gl_mst.dr_bal;			
			}
			else if(ret==-1)
			{
			/**
				set_zd_data("0220","贷");
			**/
				*bal = g_gl_mst.cr_bal;			
			}
		}
		strcpy(name,g_com_item.acc_name);       /*户名*/
	}
	else					/*输入非科目帐号*/
	{
		/*根据帐号找账户ID和序号 */
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
		if( ret==100 )
		{
	 		sprintf(acErrMsg,"不存在该帐号!!ac_no=[%ld]",g_pub_tx.ac_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "查询介质帐号表异常!!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/**g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn; Del by hxc 20101019**/
		/*add by hxc 20101019*/	
		if((!memcmp(g_mdm_ac_rel.mdm_code,"0020",4)||!memcmp(g_mdm_ac_rel.mdm_code,"0024",4)) && g_mdm_ac_rel.ac_seqn==9999)
		{
			g_pub_tx.ac_seqn=1;
		}else{
		/*add by hxc 20101019*/
		g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn;
		}

		/*** 取产品种类 ***/
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%d and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
		if( ret==100 )
		{
	 		sprintf(acErrMsg,"取prdt_ac_id表出错!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 		WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}
		else if( ret )
		{
	 		sprintf(acErrMsg,"取prdt_ac_id表出错!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
			WRITEMSG
			goto ErrExit;
		}
		if( sPrdt_ac_id.ac_id_type[0]=='1' )	/*** 活期 ***/	
		{
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
					 	g_dd_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"不存在产品号!!prdt_no=[%s]",g_dd_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"不存在该产品号!!prdt_no=[%s],ret=[%d]",g_dd_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}
			/***
			if( g_dd_parm.cif_type[0]=='1' )
			{
				sprintf(acErrMsg,"请输入对公账户!!cif_type=[%s]",g_dd_parm.cif_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O165");
				goto ErrExit;
			}
			***/
			/* 输出变量 */
			strcpy(name,g_dd_mst.name);				/*户名*/
			*ac_id  = g_dd_mst.ac_id;
			*cif_no = g_dd_mst.cif_no;
			*bal    = g_dd_mst.bal;
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='2' )	/*** 定期 ***/	
		{
			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",
					 	g_td_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"不存在产品号!!prdt_no=[%s]",g_td_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"不存在该产品号!!prdt_no=[%s],ret=[%d]",g_td_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}
			/* 输出变量 */
			strcpy(name,g_td_mst.name);				/*户名*/
			*bal = g_td_mst.bal;
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='3' )	/*** 贷款 ***/	
		{
			ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",
					 	g_ln_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"不存在产品号!!prdt_no=[%s]",g_ln_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"不存在该产品号!!prdt_no=[%s],ret=[%d]",g_ln_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}
			/* 输出变量 */
			strcpy(name,g_ln_mst.name);				/*户名*/
			*ac_id  = g_ln_mst.ac_id;
			*cif_no = g_ln_mst.cif_no;	
			*bal    = g_ln_mst.bal;
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='4' )	/*** 透支 ***/	
		{
			ret=Od_mst_Sel(g_pub_tx.reply,&g_od_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'",
					 	g_od_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"不存在产品号!!prdt_no=[%s]",g_od_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"不存在该产品号!!prdt_no=[%s],ret=[%d]",g_od_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}


			/* 输出变量 */

			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",
						g_od_mst.pay_ac_id,g_od_mst.pay_ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d]",g_od_mst.pay_ac_id,g_od_mst.pay_ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]",g_od_mst.pay_ac_id,g_od_mst.pay_ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_dd_mst.name);				/*户名*/
			*ac_id  = g_dd_mst.ac_id;
			*cif_no = g_dd_mst.cif_no;	
			*bal    = g_dd_mst.bal;
		}
		else
		{
			ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "查询内部帐户主文件异常!!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s'",
					 	g_in_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"不存在该产品号!!prdt_no=[%ld]",g_in_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "查询内部帐户参数表异常!!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			/*根据科目号查询余额借贷方向*/
			ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_hrt='%s'",
						 g_in_parm.acc_hrt);
	    	if( ret==100 )
	    	{
		   		sprintf(acErrMsg,"该科目号不存在!!acc_hrt=[%s]",
		   			g_in_parm.acc_hrt);
		   		WRITEMSG
		   		strcpy(g_pub_tx.reply,"O074");
		   		goto ErrExit;
			}
			else if( ret )
			{
		   		sprintf( acErrMsg, "查询内部帐户参数表异常!!ret=[%d]", ret );
		   		WRITEMSG
		   		goto ErrExit;
			}

			/*余额方向*/
			/***
			if(g_com_item.dc_ind[0]=='1')
				set_zd_data("0220","借");
			else if(g_com_item.dc_ind[0]=='2')
				set_zd_data("0220","贷");
			else if(g_com_item.dc_ind[0]=='0')      *双方*
			{
				ret=pub_base_CompDblVal(g_in_mst.bal,0.00);
				if(ret==1)
			   		set_zd_data("0220","借");
				else 
			   		set_zd_data("0220","贷");
	  		}
			**/

			/* 输出变量 */
			strcpy(name,g_in_mst.name);				/*户名*/
			*ac_id  = g_in_mst.ac_id;
			*cif_no = 0;
			*bal    = g_in_mst.bal;
		}
	}
	return 0;

ErrExit:
	strcpy(reply,g_pub_tx.reply);
	return 1;
}

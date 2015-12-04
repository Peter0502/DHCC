#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "mo_trust_prot_c.h"

/**********************************************************************
* 函 数 名：  pub_ln_trust_info
* 函数功能：  根据委托协议编号取委托协议信息,并检查4种账户状态是否正常
* 作    者：  rob
* 完成时间：  2004年04月04日
*
* 参    数： 
*     输  入：  trust_no        委托协议编号  	
*               t_mo_trust_prot 委托协议登记簿结构	       
*
*     输  出:                         
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_trust_info(char *trust_no, struct mo_trust_prot_c *t_mo_trust_prot )
{
	int ret = 0 ;
	int ac_seqn;
	
	struct ln_mst_c    ln_mst;    
	struct dd_mst_c	   dd_mst_tmp;
	struct mo_trust_prot_c  mo_trust_prot;
	
	memset( &ln_mst , 0x00 , sizeof( struct ln_mst_c ) );
	memset( &dd_mst_tmp , 0x00 , sizeof( struct dd_mst_c ) );
	memset( &mo_trust_prot , 0x00 , sizeof( struct mo_trust_prot_c ) );
		
		sprintf(acErrMsg,"此贷款对应的此委托协议检查![%s], *trust_no{%s} trust_no[%s]", g_pub_tx.reply, *trust_no, trust_no);
		WRITEMSG
	/* 根据委托协议编号取委托协议信息 */
	ret = Mo_trust_prot_Sel( g_pub_tx.reply,&mo_trust_prot,"trust_no='%s'", trust_no ); /*trust_no不应加*呀，gujy 20060828*/
	if( ret==100 )
	{
		sprintf(acErrMsg,"此贷款对应的此委托协议不存在,请检查![%s]", g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L089");
		return -1;
	}else if( ret ){
		sprintf(acErrMsg,"读取委托协议登记簿信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L090");
		return -1;
	}	
	
	/* 委托人贷款户检查 */

	memcpy(t_mo_trust_prot , &mo_trust_prot, sizeof(mo_trust_prot)); /*t_mo_trust_prot没赋值呀? gujy 20060828*/

	ret = Ln_mst_Sel(g_pub_tx.reply, &ln_mst , "ac_id=%ld and ac_seqn=%d" ,
				        t_mo_trust_prot->ln_ac_id,t_mo_trust_prot->ln_ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;
	}else if( ret ){
   		sprintf( acErrMsg,"取贷款主文件异常 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		return -1;
   	}	
	if ( ln_mst.ac_sts[0]!='1' )
	{
		sprintf(acErrMsg,"委托协议中贷款账户状态异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L091");
		return -1;
	}
	
	/* 根据币种取账户序号 */
	ret = pub_base_CurToSeqn( t_mo_trust_prot->cur_no , &ac_seqn );
	if ( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			return -1;	
		}
	sprintf(acErrMsg,"[%d]",ac_seqn);
	WRITEMSG
	
	/* 委托人账户检查 */		
	ret = pub_ln_check_ddsts( t_mo_trust_prot->afund_ac_id , ac_seqn , "1" );
	if ( ret ) 
	{
		sprintf(acErrMsg,"委托人账户检查失败");
		WRITEMSG
		return -1;
	}		
		
	/* 委托人还本账户检查 */		
	ret = pub_ln_check_ddsts( t_mo_trust_prot->amt_ac_id , ac_seqn , "1" );
	if ( ret ) 
	{
		sprintf(acErrMsg,"委托人还本账户检查失败");
		WRITEMSG
		return -1;
	}
	
	/* 委托人还息账户检查 */		
	ret = pub_ln_check_ddsts( t_mo_trust_prot->intst_ac_id , ac_seqn , "1" );
	if ( ret ) 
	{
		sprintf(acErrMsg,"委托人还息账户检查失败");
		WRITEMSG
		return -1;
	}
	
  	return 0;
}
/**********************************************************************
* 函 数 名：  pub_ln_check_ddsts
* 函数功能：  检查活期户状态是否适合存款
* 作    者：  rob
* 完成时间：  2004年04月04日
*
* 参    数： 
*     输  入：  ac_id       interger  账户ID  	
*               ac_seqn     smallint  账号序号	       
*		info	    char      账号信息标志1-委托人账号2-还本金账号3-还利息账号
*     输  出:                         
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_check_ddsts( long ac_id , int ac_seqn , char *info)
{	
	int ret=0;
	struct dd_mst_c t_dd_mst;
	struct dd_parm_c t_dd_parm;

	memset(&t_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&t_dd_parm,0x00,sizeof(struct dd_parm_c));
	
	/* 检查活期账号状态 */
	ret = pub_base_currentid_file( ac_id , ac_seqn , &t_dd_mst );
	if( ret )
	{
		if(info[0]=='0')
		{
			sprintf(acErrMsg,"账号不存在[%d][%d]",ac_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			return 1;
		}
		if(info[0]=='1')
		{
			sprintf(acErrMsg,"委托协议中委托户账号不存在[%d][%d]",ac_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L092");
			return 1;
		}
		if(info[0]=='2')
		{
			sprintf(acErrMsg,"委托协议中还本金账号不存在[%d][%d]",ac_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L093");
			return 1;
		}
		if(info[0]=='3')
		{
			sprintf(acErrMsg,"委托协议中还利息账号不存在[%d][%d]",ac_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L094");
			return 1;
		}				
		
	}
	      	
        ret=Dd_parm_Sel(g_pub_tx.reply,&t_dd_parm,"prdt_no='%s'",t_dd_mst.prdt_no);
        if( ret==100 )
        {
            sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%ld]",
                             t_dd_mst.prdt_no);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P145");
            return 1;
        }else if( ret ){
            sprintf(acErrMsg,"查询活期参数表异常!!ret=[%d]",ret);
            WRITEMSG
            return 1;
        }		
	if( t_dd_mst.ac_sts[0]!='1' )
	{
		if(info[0]=='0')
		{
			sprintf(acErrMsg,"账号状态非正常状态!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P109");
			return 1;
		}
		if(info[0]=='1')
		{
			sprintf(acErrMsg,"委托协议中委托户账号状态非正常!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L095");
			return 1;
		}
		if(info[0]=='2')
		{
			sprintf(acErrMsg,"委托协议中还本金账号状态非正常!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L096");
			return 1;
		}
		if(info[0]=='3')
		{
			sprintf(acErrMsg,"委托协议中还利息账号状态非正常!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L097");
			return 1;
		}					
		
	}
	if( t_dd_mst.hold_sts[0]=='1')
	{
		if(info[0]=='0')
		{
			sprintf(acErrMsg,"该账号已被冻结!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L098");
			return 1;
		}
		if(info[0]=='1')
		{
			sprintf(acErrMsg,"委托协议中委托户账号已被冻结!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L099");
			return 1;
		}
		if(info[0]=='2')
		{
			sprintf(acErrMsg,"委托协议中还本金账号已被冻结!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L100");
			return 1;			
		}
		if(info[0]=='3')
		{
			sprintf(acErrMsg,"委托协议中还利息账号已被冻结!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L101");	
			return 1;		
		}						
		
	}
	if( t_dd_parm.dpst_ind[0]=='N' )
	{
		if(info[0]=='0')
		{
			sprintf(acErrMsg,"不可续存[%s]!",t_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A009");
			return 1;
		}
		if(info[0]=='1')
		{
			sprintf(acErrMsg,"委托协议中委托户账号不可续存[%s]!",t_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L102");
			return 1;
		}
		if(info[0]=='2')
		{
			sprintf(acErrMsg,"委托协议中还本金账号不可续存[%s]!",t_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L103");
			return 1;
		}
		if(info[0]=='3')
		{
			sprintf(acErrMsg,"委托协议中还利息账号不可续存[%s]!",t_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L104");
			return 1;
		}		
		
	}
	if( t_dd_parm.thr_dpst_ind[0]=='N' && strcmp(g_pub_tx.tx_br_no,t_dd_mst.opn_br_no) )
	{
		if(info[0]=='0')
		{	
			sprintf(acErrMsg,"不可通存[%s]!",t_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A010");
			return 1;
		}
		if(info[0]=='1')
		{	
			sprintf(acErrMsg,"委托协议中委托户账号不可通存[%s]!",t_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L105");
			return 1;
		}
		if(info[0]=='2')
		{	
			sprintf(acErrMsg,"委托协议中还本金账号不可通存[%s]!",t_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L106");
			return 1;
		}
		if(info[0]=='3')
		{	
			sprintf(acErrMsg,"委托协议中还利息账号不可通存[%s]!",t_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L107");
			return 1;
		}			
		
	}
	return 0;
}

#include "find_debug.h" 
/*************************************************
* 文 件 名: pubf_base_acm.c
* 功能描述：   
*              pub_base_CalAcm          计算积数(不计算角分积数)
*              pub_base_Get_SectIntst   计算分段积数利息
*              pub_base_CalAcm_Ln       计算积数(计算角分积数)
*
* 作    者:    lance
* 完成日期：   2004年12月28日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "com_sect_acm_c.h"
#include "com_parm_c.h"
#include "ln_reg_c.h"
#include "ln_acm_reg_c.h"
#include "mo_sxc_c.h"
#include "mo_sxc_hst_c.h"

double pub_base_PubDround_1 ( double lx ); /*利息四舍五入*/
double pub_base_floor(double number);
/**********************************************************************
* 函 数 名：  pub_base_CalAcm
* 函数功能：  计算积数
* 作    者：  lance
* 完成时间：  2003年01月14日
*
* 参    数： 
*     输  入：beg_date   long	  起始日期(上笔发生日期)   
*      	      end_date   ilong	  终止日期(截至日期（交易日期）) 
*	      	  day_type	 char	  计息天数类型（0按实际天数、1按30天计算）
*             acm_type   char     积数计算类型（1每日累计2变动累计3变动累计倒算
*             bal        double   交易前余额(倒算利息用)
*             amt        double   金额(金额应传入发生额，且增为正减为负)
*             acm        double   利息积数
*(倒算利息用)     
* intst_type计息类型 0不计息1利随本清2按日计息3按月计息4按季计息5按年计息
* intst_mon计息月份
* intst_date计息日期
*     输  出: acm        double   利息积数
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_base_CalAcm( beg_date,end_date,day_type,acm_type,bal,amt,acm,intst_type,intst_moon,intst_date )
 long beg_date ;
 long end_date ;
 char day_type[2] ; 
 char acm_type[2] ;
 double bal ;
 double amt ;
 double *acm ;
 char intst_type[2];
 long intst_moon ;
 long intst_date ;
{
	long days;/* 天数 */
	int ret;
	long end_acm_day; /* 结息日期 */
	double vamt;
	double vacm;
	
	struct mo_sxc_c      sMo_sxc;
  struct mo_sxc_hst_c  sMo_sxc_hst;
  memset(&sMo_sxc,     0x00, sizeof(sMo_sxc));
  memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
    
	sprintf(acErrMsg,"积数为[%lf], day_type [%s], acm_type [%s], amt=[%lf]", 
		*acm, day_type, acm_type, amt);
	WRITEMSG	

	days=0;
	vamt=bal;
	vtcp_log("[%s] [%d], 查看滚动积数金额[%lf]", __FILE__, __LINE__, vamt);
	
	if ( acm_type[0]=='0' )	/* 不计 */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='1' )	/* 每日累计 */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='2' )	/* 变动累计 */
	{
			/* 计算天数 */	
			vtcp_log("beg_date=[%ld],end_date=[%ld],day_type=[%s]",beg_date,end_date,day_type);
			ret = pub_base_CalDays( beg_date , end_date , day_type , &days );
			if ( ret )
			{
				sprintf(acErrMsg,"调用计算天数函数错误!");
				WRITEMSG
				return 1;	
			}
			sprintf(acErrMsg,"调用计算天数函数 PASS!天数为[%ld]",days);
			WRITEMSG
			
	}
	else if ( acm_type[0]=='3' )	/* 变动累计倒算 */
	{
        /* 检查计息类型 */
		TRACE
		/* 零存整取 是利随本清不能变动累计倒算积数 ****
        if (intst_type[0] == '1')
        {
             sprintf(acErrMsg,"利随本清不能变动累计倒算积数!!");
             WRITEMSG
             return 1;
        }
		***********************************************/

		vamt=amt;
			pub_base_next_intst_date(intst_type,intst_moon,
				intst_date,&end_acm_day);
		vtcp_log( "NEXT INTST[%d]",end_acm_day );
			if ( end_acm_day<=19000101 )
			{
				sprintf(acErrMsg,"结息日期为[%ld]",end_acm_day);
				WRITEMSG					
				days=0;
			}
			else
			{
				/* 计算天数 */	
				ret = pub_base_CalDays( end_date,end_acm_day,day_type,&days );
				if ( ret )
				{
					sprintf(acErrMsg,"调用计算天数函数错误!");
					WRITEMSG
					return 1;	
				}
				days = days + 1;
				sprintf(acErrMsg,"调用计算天数函数 PASS!天数为[%ld]",days);
				WRITEMSG	
			}
	}
	else
	{
		sprintf(acErrMsg,"算积数方式错误[%s]!",acm_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P146" );
		return 1;	
	}

	/**20030617 rob**/
	if( days<0 ) days=0;

	/* 计算积数 */
	vacm=*acm;
	
	sprintf(acErrMsg,"遂心存业务判断 g_pub_tx.tx_code==[%s] g_pub_tx.sub_tx_code==[%s] g_dd_mst.ac_id==[%ld]",g_pub_tx.tx_code, g_pub_tx.sub_tx_code,g_dd_mst.ac_id);
			WRITEMSG
	/**若签约遂心存业务，滚积数时减去遂心存余额 add by zqbo 20141120***/
  if( (strcmp(g_pub_tx.tx_code,"Z157")==0 || strcmp(g_pub_tx.tx_code,"2316")==0 || strcmp(g_pub_tx.tx_code,"2313")==0 || 
    	strcmp(g_pub_tx.tx_code,"8314")==0 || strcmp(g_pub_tx.tx_code,"8315")==0 || strcmp(g_pub_tx.tx_code,"2314")==0 || 
    	strcmp(g_pub_tx.tx_code,"2315")==0 || strcmp(g_pub_tx.sub_tx_code,"D313")==0 || strcmp(g_pub_tx.sub_tx_code,"D316")==0 || 
    	strcmp(g_pub_tx.tx_code,"2201")==0 || strcmp(g_pub_tx.tx_code,"2202")==0 || strcmp(g_pub_tx.sub_tx_code,"D099")==0 || strcmp(g_pub_tx.sub_tx_code,"D003")==0)
    	&& g_dd_mst.ac_id > 0) /**防止日终批量嵌套出错、同时提高效率**/
  {
      ret = Mo_sxc_Sel(g_pub_tx.reply, &sMo_sxc, "ac_id=%ld and ac_seqn=%d and sts='1'", g_dd_mst.ac_id, g_dd_mst.ac_seqn);
      if(ret != 100 && ret != 0)
      {
          sprintf(acErrMsg,"Mo_sxc_Dec_Sel错误[%d]!",ret);
          WRITEMSG
          strcpy( g_pub_tx.reply,"P146" );
          return 1; 
      }
      else if(ret == 0) /**若存在签约处理**/
      {
          if( pub_base_CompDblVal(vamt, g_dd_mst.bal) == 0)
          {
              vamt -= sMo_sxc.bal;    /** 减去遂心存余额 **/
              vtcp_log("[%s] [%d], ac_id[%ld]余额[%lf]	[%lf]", __FILE__, __LINE__, sMo_sxc.ac_id, sMo_sxc.bal, vamt);
          }
          if(sMo_sxc.flag[0]=='0')   /***滚积数后修改当日发生业务标志***/
          {
              ret = sql_execute("update mo_sxc set flag='1' where ac_id=%ld and ac_seqn=%d and sts='1'", g_dd_mst.ac_id, g_dd_mst.ac_seqn);
              if(ret)
              {
                  sprintf(acErrMsg,"sql_execute(update mo_sxc...) 出错[%d]!",ret);
                  WRITEMSG
                  strcpy( g_pub_tx.reply,"P146" );
                  return 1;
              }
              vtcp_log("[%s] [%d], ac_id[%ld]余额[%lf]查看滚动积数金额[%lf]", __FILE__, __LINE__, sMo_sxc.ac_id, sMo_sxc.bal, vamt);
          }
      }/**否则不处理**/
  }
    
	/****角分不算积数***/
	sprintf(acErrMsg,"amt[%.2lf] floor(amt)[%.2lf]",vamt,pub_base_floor(vamt));
	WRITEMSG
	vacm += days*pub_base_floor(vamt);
	sprintf(acErrMsg,"积数为[%lf]days[%d]amt[%.2lf]",vacm,days,vamt);
	WRITEMSG
	/* 四舍五入 */
	vacm= pub_base_PubDround_1( vacm );	
	sprintf(acErrMsg,"积数为[%lf]",vacm);
	WRITEMSG
	
	/***存在遂心存签约时，如果冲补金额造成积数为负数 直接取0 zqbo 20141120***/
  if(sMo_sxc.sts[0] == '1' && pub_base_CompDblVal(vacm,0.00) < 0)
  {
      vacm = 0.00;
  }
  sprintf(acErrMsg,"----积数为[%lf]----",vacm);
  WRITEMSG
  
	*acm =vacm;
	return 0;
}
/**计算下次结息日期**/
int pub_base_next_intst_date(intst_type,intst_moon,intst_date,end_acm_day)
 char intst_type[2];
 long intst_moon;
 long intst_date;
 long *end_acm_day;
{
	long vy,vm,vd;
	long vy1,vm1,vd1;
	long next_date,tmpdate;

sprintf(acErrMsg,"计算下次结息日期[%s][%d][%d]",intst_type,intst_moon,intst_date);
WRITEMSG

	vy=g_pub_tx.tx_date/10000;
	vm=g_pub_tx.tx_date/100%100;
	vd=g_pub_tx.tx_date%100;

/* intst_type计息类型 0不计息1利随本清2按日计息3按月计息4按季计息5按年计息*/
	switch(intst_type[0])
	{
		case '0':
			*end_acm_day=0;
			break;
		case '1':
			sprintf(acErrMsg,"------> 到期日期为 : [%ld]", g_pub_tx.mach_date);
			WRITEMSG
			*end_acm_day=g_pub_tx.mach_date;
			break;
		case '2':
			*end_acm_day=g_pub_tx.tx_date;
			break;
		case '3':
			if( intst_date==99 )
			{
				vd1=1; vy1=vy;
				if( vm==12 ) { vm1=1; vy1++; }
				else { vm1=vm+1; }
				next_date=vy1*10000+vm1*100+vd1;
				pub_base_deadlineD( next_date,-1,&tmpdate );
				if( tmpdate%100>vd ) *end_acm_day=tmpdate;
				else
				{
					if( vm==12 ) { vm1=1; vy1++; }
					next_date=vy1*10000+vm1*100+vd1;
					pub_base_deadlineD( next_date,-1,end_acm_day );
				}
			}
			else
			{
				if( vd<intst_date )
					*end_acm_day=vy*10000+vm*100+intst_date;
				else
				{
					if( vm==12 ) { vm1=1; vy1=vy+1; }
					*end_acm_day=vy1*10000+vm1*100+intst_date;
				}
			}
			break;
		case '4':
			break;
		case '5':
			if( intst_date==99 )
			{
				vy1=vy; vm1=intst_moon; vd1=1; 
				if( vm1==12 ) { vm1=1; vy1++; }
				else { vm1=vm1+1; }
				next_date=vy1*10000+vm1*100+vd1;
				pub_base_deadlineD( next_date,-1,&tmpdate );
				if( g_pub_tx.tx_date<tmpdate ) *end_acm_day=tmpdate;
				else
				{
					*end_acm_day=tmpdate+10000;
				}
			}
			else
			{
				tmpdate=vy*10000+intst_moon*100+intst_date;
				if( g_pub_tx.tx_date<tmpdate ) *end_acm_day=tmpdate;
				else
				{
					*end_acm_day=tmpdate+10000;
				}
			}
			break;
		default :
			*end_acm_day=0;
			break;
	}

sprintf(acErrMsg,"计算下次结息日期over[%d]",*end_acm_day);
WRITEMSG
	return 0;
}

/**********************************************************************
* 函 数 名：  pub_base_Get_SectIntst
* 函数功能：  计算分段积数利息
* 作    者：  lance
* 完成时间：  2003年01月14日
*
* 参    数：  
*     输  入：ac_id   	  long 	  账户id   
*      	      ac_seqn     int	  账户序号
*             intst_type  char    利息种类（0存款1贷款本金 2贷款表内欠息3贷款表外欠息4贷款复利5透支）
*             mode        int     处理模式 0修改数据库1不修改数据库
*      
*     输  出: intst	  double  利息总数
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_base_Get_SectIntst( long ac_id , int ac_seqn , 
	char intst_type[2] , double *intst , int mode )
{
	int ret;	
	
	struct com_sect_acm_c	com_sect_acm;	
	struct ln_acm_reg_c	ln_acm_reg;
	
	MEMSET_DEBUG( &com_sect_acm, 0x00, sizeof(struct com_sect_acm_c) );
	MEMSET_DEBUG( &ln_acm_reg, 0x00, sizeof(struct ln_acm_reg_c) );
		
	/* 更新分段积数表中状态标志 */
	ret = Com_sect_acm_Dec_Upd( g_pub_tx.reply, 
		"ac_id=%ld and ac_seqn=%d and intst_type='%s' and sts='0' ", 
		ac_id , ac_seqn , intst_type);
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Com_sect_acm_Dec_Sel 游标出错[%d]",ret);
		WRITEMSG
		return 1;
	}	
	
	while(1)
	{
		ret = Com_sect_acm_Fet_Upd( &com_sect_acm, g_pub_tx.reply );
		if( ret==100 ) break;
   		if( ret )
 		{
	 		sprintf( acErrMsg,"fetch com_sect_acm error code = [%d]",ret);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply ,"D103" );
	 		return 1;
 		}
					
		/* 算利息 */
		vtcp_log("[%s][%d]com_sect_acm.intst_acm=[%lf]com_sect_acm.rate==[%ld]\n",__FILE__,__LINE__,com_sect_acm.intst_acm,com_sect_acm.rate);
		*intst += com_sect_acm.intst_acm*com_sect_acm.rate/30/1000;		

		if( intst_type[0]=='1'||intst_type[0]=='2'||intst_type[0]=='3'||intst_type[0]=='4' )
		{

		if( !mode )
		{
			strcpy( com_sect_acm.sts , "1" );	
			ret = Com_sect_acm_Upd_Upd( com_sect_acm, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg,"Com_sect_acm_Upd_Upd 出错[%d]",ret);
				WRITEMSG
				return 1;
			}
			TRACE
			ret= Ln_mst_Sel( g_pub_tx.reply, &g_ln_mst , "ac_id=%ld and ac_seqn=%d ",com_sect_acm.ac_id,
																		com_sect_acm.ac_seqn );
			if( ret ) return 1;

			/* 添加贷款结息积数登记薄 */
			ln_acm_reg.ac_id = com_sect_acm.ac_id;
			ln_acm_reg.ac_seqn = com_sect_acm.ac_seqn;
			strcpy( ln_acm_reg.br_no, g_ln_mst.opn_br_no );
			ln_acm_reg.beg_date = com_sect_acm.beg_date;
			ln_acm_reg.end_date = com_sect_acm.end_date;
			ln_acm_reg.cal_date = g_pub_tx.tx_date;
			ln_acm_reg.rate = com_sect_acm.rate;
			ln_acm_reg.intst_acm = com_sect_acm.intst_acm;
			ln_acm_reg.part_intst_acm = com_sect_acm.intst_acm;
			ln_acm_reg.intst = com_sect_acm.intst_acm*com_sect_acm.rate/30/1000;
			strcpy( ln_acm_reg.intst_type , com_sect_acm.intst_type );
			strcpy( ln_acm_reg.type , "0" );	/*由分段积数表取出*/
			ln_acm_reg.trace_no = g_pub_tx.trace_no;
			TRACE
			sprintf( acErrMsg,"ln_acm_reg.intst=[%lf]",ln_acm_reg.intst);
			WRITEMSG
			ret = Ln_acm_reg_Ins( ln_acm_reg , g_pub_tx.reply );
			if( ret )	return 1;
			TRACE
						
		}
		}
	}
	
	Com_sect_acm_Clo_Upd( );
	sprintf(acErrMsg,"更新分段积数表中状态标志 PASS");
	WRITEMSG
	*intst = pub_base_PubDround_1( *intst );
vtcp_log("[%s][%d]g_ln_mst.opn_br_no=[%s]\n",__FILE__,__LINE__,g_ln_mst.opn_br_no);
										 
	return 0;	
}
int pub_base_CalAcm_tt( beg_date,end_date,day_type,acm_type,bal,amt,acm
	,intst_type,intst_moon,intst_date )
 long beg_date ;
 long end_date ;
 char day_type[2] ; 
 char acm_type[2] ;
 double bal ;
 double amt ;
 double *acm ;
 char intst_type[2];
 long intst_moon ;
 long intst_date ;
{
	long days;/* 天数 */
	int ret;
	long end_acm_day; /* 结息日期 */
	double vamt;
	double vacm;

	sprintf(acErrMsg, "FUNCTIONS %s bd[%ld] ed[%ld]  t[%lf] [%ld] {%s}{%s}", __FUNCTION__, beg_date, end_date, *acm, (long)acm, day_type, acm_type);	
	WRITEMSG
sprintf(acErrMsg, "FUNCTIONS %s bd[%ld] ed[%ld]  t[%lf] [%ld] {%s}{%s}", __FUNCTION__, beg_date, end_date, *acm, (long)acm, day_type, acm_type);
   WRITEMSG

	sprintf(acErrMsg,"积数acm为[%lf][%ld], day_type [%s], acm_type [%s], amt=[%lf], bal[%lf] beg_date[%d] end_date[%d]", 
		*acm, (long)acm, day_type, acm_type, amt, bal, beg_date, end_date);
	WRITEMSG	
	days=0;
	vamt=bal;
	
	if ( acm_type[0]=='0' )	/* 不计 */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='1' )	/* 每日累计 */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='2' )	/* 变动累计 */
	{
			/* 计算天数 */	
TRACE
			ret = pub_base_CalDays( beg_date , end_date , day_type , &days );
			if ( ret )
			{
				sprintf(acErrMsg,"调用计算天数函数错误!");
				WRITEMSG
				return 1;	
			}
			sprintf(acErrMsg,"调用计算天数函数 PASS!天数为[%ld]",days);
			WRITEMSG
			
	}
	else if ( acm_type[0]=='3' )	/* 变动累计倒算 */
	{
        /* 检查计息类型 */
		TRACE
		/* 零存整取 是利随本清不能变动累计倒算积数 ****
        if (intst_type[0] == '1')
        {
             sprintf(acErrMsg,"利随本清不能变动累计倒算积数!!");
             WRITEMSG
             return 1;
        }
		***********************************************/

		vamt=amt;
			pub_base_next_intst_date(intst_type,intst_moon,
				intst_date,&end_acm_day);
		vtcp_log( "NEXT INTST[%d]",end_acm_day );
			if ( end_acm_day<=19000101 )
			{
				sprintf(acErrMsg,"结息日期为[%ld]",end_acm_day);
				WRITEMSG					
				days=0;
			}
			else
			{
				/* 计算天数 */	
				ret = pub_base_CalDays( end_date,end_acm_day,day_type,&days );
				if ( ret )
				{
					sprintf(acErrMsg,"调用计算天数函数错误!");
					WRITEMSG
					return 1;	
				}
				days = days + 1;
				sprintf(acErrMsg,"调用计算天数函数 PASS!天数为[%ld]",days);
				WRITEMSG	
			}
	}
	else
	{
		sprintf(acErrMsg,"算积数方式错误[%s]!",acm_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P146" );
		return 1;	
	}

	/**20030617 rob**/
	if( days<0 ) days=0;

	/* 计算积数 */
	vacm=*acm;
	vacm+=days*vamt;
	sprintf(acErrMsg,"积数为[%lf]days[%d]amt[%.2lf]",vacm,days,vamt);
	WRITEMSG
	/* 四舍五入 */
	vacm= pub_base_PubDround_1( vacm );	
	sprintf(acErrMsg,"积数为[%lf]",vacm);
	WRITEMSG	
	/**/		 
	*acm =vacm;
	return 0;
}
/**********************************************************************
* 函 数 名：  pub_base_CalAcm_Ln
* 函数功能：  计算积数
* 作    者：  lance
* 完成时间：  2003年01月14日
*
* 参    数： 
*     输  入：beg_date   long	  起始日期(上笔发生日期)   
*      	      end_date   ilong	  终止日期(截至日期（交易日期）) 
*	      	  day_type	 char	  计息天数类型（0按实际天数、1按30天计算）
*             acm_type   char     积数计算类型（1每日累计2变动累计3变动累计倒算
*             bal        double   交易前余额(倒算利息用)
*             amt        double   金额(金额应传入发生额，且增为正减为负)
*             acm        double   利息积数
*(倒算利息用)     
* intst_type计息类型 0不计息1利随本清2按日计息3按月计息4按季计息5按年计息
* intst_mon计息月份
* intst_date计息日期
*     输  出: acm        double   利息积数
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_base_CalAcm_Ln( beg_date,end_date,day_type,acm_type,bal,amt,acm
	,intst_type,intst_moon,intst_date )
 long beg_date ;
 long end_date ;
 char day_type[2] ; 
 char acm_type[2] ;
 double bal ;
 double amt ;
 double *acm ;
 char intst_type[2];
 long intst_moon ;
 long intst_date ;
{

	long days;/* 天数 */
	int ret;
	long end_acm_day; /* 结息日期 */
	double vamt;
	double vacm;

sprintf(acErrMsg, "FUNCTIONS %s bd[%ld] ed[%ld]  t[%lf] [%ld] {%s}{%s}", __FUNCTION__, beg_date, end_date, *acm, (long)acm, day_type, acm_type);
   WRITEMSG

	sprintf(acErrMsg,"积数acm为[%lf][%ld], day_type [%s], acm_type [%s], amt=[%lf], bal[%lf] beg_date[%d] end_date[%d]", 
		*acm, (long)acm, day_type, acm_type, amt, bal, beg_date, end_date);
	WRITEMSG	
	days=0;
	vamt=bal;
	
	if ( acm_type[0]=='0' )	/* 不计 */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='1' )	/* 每日累计 */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='2' )	/* 变动累计 */
	{
			/* 计算天数 */	
TRACE
			ret = pub_base_CalDays( beg_date , end_date , day_type , &days );
			if ( ret )
			{
				sprintf(acErrMsg,"调用计算天数函数错误!");
				WRITEMSG
				return 1;	
			}
			sprintf(acErrMsg,"调用计算天数函数 PASS!天数为[%ld]",days);
			WRITEMSG
			
	}
	else if ( acm_type[0]=='3' )	/* 变动累计倒算 */
	{
        /* 检查计息类型 */
		TRACE
		/* 零存整取 是利随本清不能变动累计倒算积数 ****
        if (intst_type[0] == '1')
        {
             sprintf(acErrMsg,"利随本清不能变动累计倒算积数!!");
             WRITEMSG
             return 1;
        }
		***********************************************/

		vamt=amt;
			pub_base_next_intst_date(intst_type,intst_moon,
				intst_date,&end_acm_day);
		vtcp_log( "NEXT INTST[%d]",end_acm_day );
			if ( end_acm_day<=19000101 )
			{
				sprintf(acErrMsg,"结息日期为[%ld]",end_acm_day);
				WRITEMSG					
				days=0;
			}
			else
			{
				/* 计算天数 */	
				ret = pub_base_CalDays( end_date,end_acm_day,day_type,&days );
				if ( ret )
				{
					sprintf(acErrMsg,"调用计算天数函数错误!");
					WRITEMSG
					return 1;	
				}
				days = days + 1;
				sprintf(acErrMsg,"调用计算天数函数 PASS!天数为[%ld]",days);
				WRITEMSG	
			}
	}
	else
	{
		sprintf(acErrMsg,"算积数方式错误[%s]!",acm_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P146" );
		return 1;	
	}

	/**20030617 rob**/
	if( days<0 ) days=0;

	/* 计算积数 */
	vacm=*acm;
	vacm+=days*vamt;
	sprintf(acErrMsg,"积数为[%lf]days[%d]amt[%.2lf]",vacm,days,vamt);
	WRITEMSG
	/* 四舍五入 */
	vacm= pub_base_PubDround_1( vacm );	
	sprintf(acErrMsg,"积数为[%lf]",vacm);
	WRITEMSG	
	/**/		 
	*acm =vacm;
	return 0;
}

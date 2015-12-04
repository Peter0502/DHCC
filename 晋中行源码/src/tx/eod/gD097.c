/*************************************************************
* 文 件 名: gD097.c
* 功能描述：日终－免息日之后贷款积数滚动
*
* 作    者: lance
* 完成日期: 2003年11月15日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define MYSQLERR if(ret) {\
		sprintf(acErrMsg,"SQLERROR [%d]",ret);\
		WRITEMSG\
		goto ErrExit;\
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "cif_basic_inf_c.h"
#include "com_sect_acm_c.h"

gD097()
{
	int  ret=0;
	long lst_free_date=0;
	long old_ic_day=0;
	double sect_acm=0.00;
	struct com_sect_acm_c	com_sect_acm;
	memset( &com_sect_acm ,0x00, sizeof(struct com_sect_acm_c));
	
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}
	
   	/* 初始化全局变量 */
   	pub_base_sysinit();
 	/* 修改主文件中数据 */  		
	ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"1=1");
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret=Ln_mst_Fet_Upd(&g_ln_mst,g_pub_tx.reply);
		if (ret&&ret!=100 )
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if( ret==100 ){
			break;
		}
		/* 账户状态正常的按揭贷款不处理 */
		if( (g_ln_mst.repay_type[0]=='3' || g_ln_mst.repay_type[0]=='4') &&g_ln_mst.ac_sts[0]=='1')
		{
			sprintf(acErrMsg,"正常的按揭贷款不处理[%ld]",g_ln_mst.ac_id);
			WRITEMSG			
			continue;	
		}
		/* 利随本清、按日计息、不计息 不予考虑 */
		if( g_ln_mst.intst_type[0]=='0' || g_ln_mst.intst_type[0]=='1' || g_ln_mst.intst_type[0]=='2' )
		{
			sprintf(acErrMsg,"结息类型不符合不处理[%s]",g_ln_mst.intst_type);
			WRITEMSG
			continue;
		}
		
		/* 确定最后减免日期 */
		ret=get_lst_ic_date( g_ln_mst.ic_date,g_ln_mst.intst_type,&lst_free_date,&old_ic_day);
		if(ret)	goto ErrExit;
		
		/* 免息期过后第一天 */
		if( g_pub_tx.tx_date != lst_free_date )
		{
			sprintf(acErrMsg,"超过减免期，不处理!当前日期[%ld]减免期限[%ld]",g_pub_tx.tx_date,lst_free_date);
			WRITEMSG
			continue;		
		}else{  
			/* 取贷款参数 */
			ret= Ln_parm_Sel( g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
			if(ret)	 goto ErrExit;
			
			/* 本金积数不处理 */
			
			/* 其他积数 */
			/* 滚动主文件积数 */
			/* 表内积数 */
			ret = pub_base_CalAcm_Ln( g_ln_mst.ic_date , g_pub_tx.tx_date , 
				g_ln_parm.intst_days_type , "2" , 
				g_ln_mst.in_lo_intst ,g_pub_tx.tx_amt2, &g_ln_mst.in_lo_acm,"0",0,0 );
			if(ret) goto ErrExit;		
			/* 表外积数 */
			ret = pub_base_CalAcm_Ln( g_ln_mst.ic_date , g_pub_tx.tx_date , 
				g_ln_parm.intst_days_type , "2" , 
				g_ln_mst.out_lo_intst ,g_pub_tx.tx_amt3, &g_ln_mst.out_lo_acm,"0",0,0 );
			if(ret) goto ErrExit;
			/* 复利积数 */
			ret = pub_base_CalAcm_Ln( g_ln_mst.ic_date , g_pub_tx.tx_date , 
				g_ln_parm.intst_days_type , "2" , 
				g_ln_mst.cmpd_lo_intst ,g_pub_tx.tx_amt4, &g_ln_mst.cmpd_lo_acm,"0",0,0 );
			if(ret) goto ErrExit;
									
			g_ln_mst.lst_date=g_pub_tx.tx_date;
			
			ret = Ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
			if (ret)
			{
				 sprintf(acErrMsg,"更新贷款主文件错误!!");
				 WRITEMSG
				 goto ErrExit;
			}
		}       
   	}
	Ln_mst_Clo_Upd();

	/* 修改分段积数中数据 */
  /* 免息期过后第一天 */
  if( g_pub_tx.tx_date == lst_free_date )
  {
	ret=Com_sect_acm_Dec_Upd(g_pub_tx.reply,"sts='2' and beg_date>=%ld and end_date<=%ld ",old_ic_day,lst_free_date);
	if(ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret=Com_sect_acm_Fet_Upd(&com_sect_acm,g_pub_tx.reply);
		if (ret&&ret!=100 )
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if( ret==100 ){
			break;
		}
		memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
		/*表内欠息积数*/
		if( com_sect_acm.intst_type[0]=='2' )
		{
			ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,
					"ac_id=%ld and ac_seqn=%d",com_sect_acm.ac_id,com_sect_acm.ac_seqn);		
			if(ret)
			{
				sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			if( pub_base_CompDblVal( g_ln_mst.in_lo_intst,0.00 ))
			{
				strcpy( com_sect_acm.sts,"0");
			}
		}
        /*表外欠息积数*/
		if( com_sect_acm.intst_type[0]=='3' )
		{
			ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,
					"ac_id=%ld and ac_seqn=%d",com_sect_acm.ac_id,com_sect_acm.ac_seqn);
			if(ret)
			{
				sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			if( pub_base_CompDblVal( g_ln_mst.out_lo_intst,0.00 ))
			{
				strcpy( com_sect_acm.sts,"0");
			}
		}
        /*复利欠息积数*/
		if( com_sect_acm.intst_type[0]=='4' )
		{
			ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,
                    "ac_id=%ld and ac_seqn=%d",com_sect_acm.ac_id,com_sect_acm.ac_seqn);
			if(ret)
			{
				sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			if( pub_base_CompDblVal( g_ln_mst.cmpd_lo_intst,0.00 ))
			{
				strcpy( com_sect_acm.sts,"0");
			}
		}
		ret = Com_sect_acm_Upd_Upd(com_sect_acm,g_pub_tx.reply);
		if (ret)
		{
			 sprintf(acErrMsg,"更新分段积数表错误!!");
			 WRITEMSG
			 goto ErrExit;
	   }
   }
   Com_sect_acm_Clo_Upd();
  }

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"免息日之后贷款积数滚动程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"AT12");/* 待修改 */
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"免免息日之后贷款积数滚动程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/**********************************************************************
* 函 数 名：  get_lst_ic_date
* 函数功能：  取最后减免日期
* 作    者：  lance
* 完成时间：  2003年11月03日
*
* 参    数：
*     输  入：   now_ic_date           	贷款主文件中结息日期   
*                intst_type		贷款主文件中结息期限  
*                lst_free_date          最后减免日期
*          
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int get_lst_ic_date( long now_ic_date , char *intst_type , long *lst_free_date , long *old_ic_day )
{
	int  yy=0;
	int  mm=0;
	int  dd=0;
	int  ret_tmp=0;
	long jx_date=0;
	char jx_limt_type[2];
	long jx_limt_val=0;
	char jx_limt_tmp[11];
	long lst_jx_day=0;
	long ic_date=0;
	long lst_ic_date=0;
		
	yy=now_ic_date/10000;
	mm=now_ic_date%10000/100;
	dd=now_ic_date%100;
			
	/* 日期检查 */
	/* 取系统结息日期 */
	ret_tmp= pub_base_GetParm_int( "L0003" , 1 , &jx_date );
	if(ret_tmp) return 1;

	/* 取缓息期限类型 */
	ret_tmp= pub_base_GetParm_Str( "L0004" , 1 , jx_limt_type );
	if(ret_tmp) return 1;	
	
	/* 取缓息期限 */
	ret_tmp= pub_base_GetParm_int( "L0005" , 1 , &jx_limt_val );
	if(ret_tmp) return 1;	
	
	/* 若年结 */  
	if( intst_type[0]=='5' )
	{
		if( mm==1 && dd<jx_date )/*上次结息在去年*/ 
		{
			lst_ic_date=(now_ic_date/10000-1)*10000+100+jx_date;
		}else{
			lst_ic_date=now_ic_date/10000*10000+100+jx_date;
		}
		sprintf(acErrMsg,"年结的结息日期为  [%ld]",lst_ic_date);
		WRITEMSG						
	}else if( intst_type[0]=='4' ){	/*季结*/
		if( now_ic_date%10000<300+jx_date)
		{
			lst_ic_date=(now_ic_date/10000-1)*10000+1200+jx_date;
		}else if( now_ic_date%10000<600+jx_date){
			lst_ic_date=now_ic_date/10000*10000+300+jx_date;
		}else if( now_ic_date%10000<900+jx_date){
			lst_ic_date=now_ic_date/10000*10000+600+jx_date;
		}else if( now_ic_date%10000<1200+jx_date){
			lst_ic_date=now_ic_date/10000*10000+900+jx_date;
		}else if( (now_ic_date%10000>=1200+jx_date) && (now_ic_date%10000<=1299)){
			lst_ic_date=now_ic_date/10000*10000+1200+jx_date;
		}else{
			sprintf(acErrMsg,"季结的结息日期出错!计算后日期[%ld]原日期[%ld]",lst_ic_date,now_ic_date);
			WRITEMSG
			return 1;
		}
		sprintf(acErrMsg,"季结的结息日期为  [%ld]",lst_ic_date);
		WRITEMSG
	}else if ( intst_type[0]=='3' ){	/*月结*/
		if( mm==12&&dd>=21 )
		{
			lst_ic_date=now_ic_date/10000*10000+1200+jx_date;
		}else if( mm==1&&dd<21 ){	
			lst_ic_date=(now_ic_date/10000-1)*10000+1200+jx_date;
		}else if( dd<21 ){
			lst_ic_date=now_ic_date/10000*10000+(now_ic_date%10000/100-1)*100+jx_date;
		}else if( dd>=21 ){
			lst_ic_date=now_ic_date/100*100+jx_date;
		}else{
			sprintf(acErrMsg,"月结的结息日期出错!计算后日期[%ld]原日期[%ld]",lst_ic_date,now_ic_date);
			WRITEMSG
			return 1;
		}
		sprintf(acErrMsg,"月结的结息日期为 [%ld]",lst_ic_date);
		WRITEMSG		
	}else{	/*利随本清、按日计息、不计息 不予考虑 */
		sprintf(acErrMsg,"利随本清、按日计息、不计息 不予考虑");
		WRITEMSG
	}	
	
	/* 计算最后缓息日期 */
	if( jx_limt_type[0]=='D' )	/* 缓息期限以"日"为单位 */
	{
		ret_tmp=pub_base_deadlineD( lst_ic_date, jx_limt_val, &lst_jx_day );
		if(ret_tmp)	return 1;
	}
	if( jx_limt_type[0]=='M' )	/* 缓息期限以"月"为单位 */
	{
		ret_tmp=pub_base_deadlineM( lst_ic_date, jx_limt_val, &lst_jx_day );
		if(ret_tmp)	return 1;
	}
	if( jx_limt_type[0]=='J' )	/* 缓息期限以"季"为单位 */
	{
		ret_tmp=pub_base_deadlineM( lst_ic_date, 3*jx_limt_val, &lst_jx_day );
		if(ret_tmp)	return 1;
	}	
	if( jx_limt_type[0]=='Y' )	/* 缓息期限以"年"为单位 */
	{
		ret_tmp=pub_base_deadlineM( lst_ic_date, 12*jx_limt_val, &lst_jx_day );
		if(ret_tmp)	return 1;
	}		
	sprintf(acErrMsg,"最后缓息日期为  [%ld]",lst_jx_day);
	WRITEMSG	
	*lst_free_date=lst_jx_day;
	sprintf(acErrMsg,"最后缓息日期为  [%ld]",*lst_free_date);
	WRITEMSG
	*old_ic_day=lst_ic_date;
	return 0;		
}

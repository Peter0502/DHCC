/**********************************************************************
 * 函 数 名：pubf_cif_CreCifNo.c;
 * 函数功能：生成客户ID函数
 *			 
 * 作者/时间：
 * 
 * 参数：
 *     输入：	无
 *          
 *     输出: 	
 *
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "cif_no_per_c.h"
#include "cif_no_com_c.h"
#include "cif_no_in_c.h"
#include "cif_no_yzh_c.h"

/**********************************************************************
 * 函 数 名：pub_cif_CrePer_cif_no();
 * 函数功能：生成客户ID函数(个人)
 *			 
 *
 * 作者/时间：
 * 
 * 参数：
 *     输入：	无
 *          
 *     输出: 	
 *				客户ID		long;
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_cif_CrePer_cif_no( cif_no )
	long	*cif_no;
{
	struct cif_no_per_c      cif_no_per; /* 客户代码表(个人) */
	int ret=0;
	char no[8];
	char no_tmp[9];
	
	/*取最大客户id,并修改，返回可用最大客户账号id*/

	ret = Cif_no_per_Dec_Upd (g_pub_tx.reply, " 1=1 " );
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}
	memset(&cif_no_per,'\0',sizeof(cif_no_per));
	ret = Cif_no_per_Fet_Upd (&cif_no_per,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"fetch cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	cif_no_per.cif_no = cif_no_per.cif_no + 1;

	ret = Cif_no_per_Upd_Upd (cif_no_per,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	Cif_no_per_Clo_Upd( );
	sprintf(acErrMsg,"LONG型客户号=[%d]",cif_no_per.cif_no);
	WRITEMSG

	/* 将取出的长整型客户号转换成字符型 */
	sprintf( no, "%ld" , cif_no_per.cif_no );
	sprintf(acErrMsg,"转字符的客户号=[%s]",no);
	WRITEMSG

	/* 根据客户号生成带校验位的客户号 */
	ret = pub_base_CrtAcChkb( no , no_tmp , 4 , 1 );
	if ( ret  )
	{
		return (1);
	}
	sprintf(acErrMsg,"字符型完成的客户号=[%s]",no_tmp);
	WRITEMSG
	
	/* 将字符型的客户号转换成长整型 */	
	*cif_no = atol( no_tmp );
	sprintf(acErrMsg,"完成的客户号=[%d]",*cif_no);
	WRITEMSG
	/**cif_no = cif_no_per.cif_no;*/
	return 0;
}

/**********************************************************************
 * 函 数 名：pub_cif_CreCom_cif_no();
 * 函数功能：生成客户ID函数(公司)
 *			 
 *
 * 作者/时间：
 * 
 * 参数：
 *     输入：	无
 *          
 *     输出: 	
 *				客户ID		long;
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_cif_CreCom_cif_no( cif_no )
	long	*cif_no;
{
	struct cif_no_com_c      cif_no_com; /* 客户代码表(公司) */
	int ret=0;
	char no[8];
	char no_tmp[9];	
	
	/*取最大客户id,并修改，返回可用最大客户账号id*/
	ret = Cif_no_com_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_no_com error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	ret = Cif_no_com_Fet_Upd (&cif_no_com,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"fetch cif_no_com error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	cif_no_com.cif_no = cif_no_com.cif_no + 1;

	ret = Cif_no_com_Upd_Upd (cif_no_com,&g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_no_com error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	Cif_no_com_Clo_Upd( );
	
	/* 将取出的长整型客户号转换成字符型 */
	sprintf( no, "%ld" , cif_no_com.cif_no );
	sprintf(acErrMsg,"转字符的客户号=[%s]",no);
	WRITEMSG
	
	/* 根据客户号生成带校验位的客户号 */
	ret = pub_base_CrtAcChkb( no , no_tmp , 4 , 1 );
	if ( ret  )
            {
                return (1);
            }
        sprintf(acErrMsg,"字符型完成的客户号=[%s]",no_tmp);
	WRITEMSG
	
	/* 将字符型的客户号转换成长整型 */	
	*cif_no = atol( no_tmp );
	sprintf(acErrMsg,"完成的客户号=[%d]",*cif_no);
	WRITEMSG

	/**cif_no = cif_no_com.cif_no;*/

	return 0;

}

/**********************************************************************
 * 函 数 名：pub_cif_CreIn_cif_no();
 * 函数功能：生成客户ID函数(内部)
 *			 
 *
 * 作者/时间：
 * 
 * 参数：
 *     输入：	无
 *          
 *     输出: 	
 *				客户ID		long;
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_cif_CreIn_cif_no( cif_no )
	long	*cif_no;
{
	struct cif_no_in_c      cif_no_in; /* 客户代码表(内部) */
	int ret=0;	
	char no[8];
	char no_tmp[9];
		
	/*取最大客户id,并修改，返回可用最大客户账号id*/
	ret = Cif_no_in_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_no_in error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	ret = Cif_no_in_Fet_Upd (&cif_no_in,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"fetch cif_no_in error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	cif_no_in.cif_no = cif_no_in.cif_no + 1;

	ret = Cif_no_in_Upd_Upd (cif_no_in,&g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_no_in error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	Cif_no_in_Clo_Upd( );

	/* 将取出的长整型客户号转换成字符型 */
	sprintf( no, "%ld" , cif_no_in.cif_no );
	sprintf(acErrMsg,"转字符的客户号=[%s]",no);
	WRITEMSG
	
	/* 根据客户号生成带校验位的客户号 */
	ret = pub_base_CrtAcChkb( no , no_tmp , 4 , 1 );
	if ( ret  )
            {
                return (1);
            }
        sprintf(acErrMsg,"字符型完成的客户号=[%s]",no_tmp);
	WRITEMSG
	
	/* 将字符型的客户号转换成长整型 */	
	*cif_no = atol( no_tmp );
	sprintf(acErrMsg,"完成的客户号=[%d]",*cif_no);
	WRITEMSG
	
	/**cif_no = cif_no_in.cif_no;*/

	return 0;

}
/**********************************************************************
 * 函 数 名：pub_cif_CreYzh_cif_no();
 * 函数功能：生成客户ID函数(验资户)
 *			 
 *
 * 作者/时间：
 * 
 * 参数：
 *     输入：	无
 *          
 *     输出: 	
 *				客户ID		long;
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_cif_CreYzh_cif_no( cif_no )
	long	*cif_no;
{
	struct cif_no_yzh_c      cif_no_yzh; /* 客户代码表(验资户) */
	int ret=0;
	char no[8];
	char no_tmp[9];
	
	/*取最大客户id,并修改，返回可用最大客户账号id*/

	ret = Cif_no_yzh_Dec_Upd (g_pub_tx.reply, " 1=1 " );
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}
	memset(&cif_no_yzh,'\0',sizeof(cif_no_yzh));
	ret = Cif_no_yzh_Fet_Upd (&cif_no_yzh,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"fetch cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	cif_no_yzh.cif_no = cif_no_yzh.cif_no + 1;

	ret = Cif_no_yzh_Upd_Upd (cif_no_yzh,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_no_per error code=[%d]",ret);
		WRITEMSG
		return 1;
	}

	Cif_no_yzh_Clo_Upd( );
	sprintf(acErrMsg,"LONG型客户号=[%d]",cif_no_yzh.cif_no);
	WRITEMSG

	/* 将取出的长整型客户号转换成字符型 */
	sprintf( no, "%ld" , cif_no_yzh.cif_no );
	sprintf(acErrMsg,"转字符的客户号=[%s]",no);
	WRITEMSG

	/* 根据客户号生成带校验位的客户号 */
	ret = pub_base_CrtAcChkb( no , no_tmp , 4 , 1 );
	if ( ret  )
	{
		return (1);
	}
	sprintf(acErrMsg,"字符型完成的客户号=[%s]",no_tmp);
	WRITEMSG
	
	/* 将字符型的客户号转换成长整型 */	
	*cif_no = atol( no_tmp );
	sprintf(acErrMsg,"完成的客户号=[%d]",*cif_no);
	WRITEMSG
	/**cif_no = cif_no_yzh.cif_no;*/
	return 0;
}


#include "find_debug.h" 
/*************************************************
* 文 件 名:  pubf_base_medium.c
* 功能描述：
*           包括 产品与介质关系校验函数 pub_base_check_promedi();
*           	 设定介质的支取方式函数 pub_base_set_drawtype();
*                校验介质的支取方式函数 pub_base_check_draw();
*                校验介质的支取方式函数 pub_base_check_drawtype();
*                检查介质状态函数 		pub_base_check_medstate();
* 作    者: rob
* 完成日期：2003/12/24
* 修改记录：  
* 1. 日   期:
*    修 改 人:
*    修改内容:
* 2. ...
*************************************************/

#include <stdio.h>
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "mdm_prdt_rel_c.h"
#include "mdm_unprt_hst_c.h"
#include "com_chrg_hst_c.h"
#include "com_parm_c.h"

struct  mdm_prdt_rel_c g_mdm_prdt_rel;
struct  mdm_unprt_hst_c g_mdm_unprt_hst;     /*未打印明细结构*/
struct	com_parm_c sCom_parm;

static int ret=0;
/**********************************************************************
 * 函 数 名：pub_base_check_promedi();
 * 函数功能：开户时，根据产品，检验已有介质和输入的介质组合
 *           后是否存在于产品介质关系表中。
 * 作者/时间：
 * 
 * 参数：
 *     输入：产品编号、		char(4)        (唯一)
 *           介质代号1		char(4)
 *           介质代号2		char(4)
 *           介质代号3		char(4)		注:空介质,入口参数为NULL_MDM
 *     输出: 
 *           产品介质关系结构
 *   返回值：
 *           响应代码 
 *           返回0关系存在，
 *           返回非0关系不存在；
 * 修改历史：
 *          
********************************************************************/
int pub_base_check_promedi(char *prdt_num,char *mdm_num1,
							char *mdm_num2,char *mdm_num3)
{
	int count1=0;						/*计算入口非空介质个数*/
	int count2=0;						/*计算游标所取介质非空个数*/
	int count3=0;						/*一条记录中匹配的个数*/
	char flag2[3];						/*表内记录介质是否为空标志*/
	int overflag=0;

	pub_base_strpack(mdm_num1);
	pub_base_strpack(mdm_num2);
	pub_base_strpack(mdm_num3);
	
	sprintf(acErrMsg," prdt_num [%s]",prdt_num);
	WRITEMSG
	sprintf(acErrMsg," MDM [%s][%s][%s]",mdm_num1,mdm_num2,mdm_num3);
	WRITEMSG

	overflag=0;
	count1=0;
	if ( strcmp(mdm_num1,NULL_MDM) )
		count1++;
	if ( strcmp(mdm_num2,NULL_MDM) )
		count1++;
	if ( strcmp(mdm_num3,NULL_MDM) )
		count1++;

	MEMSET_DEBUG( &g_mdm_prdt_rel, 0x00, sizeof(struct mdm_prdt_rel_c) );
	ret = Mdm_prdt_rel_Dec_Sel(g_pub_tx.reply, "prdt_no='%s'",prdt_num);
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Mdm_prdt_rel 游标出错[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D102" );
		return 1;
	}

	while(1)
	{
		if( overflag==1 ) break;
		ret = Mdm_prdt_rel_Fet_Sel( &g_mdm_prdt_rel, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch mdm_prdt_rel error code = [%d]",ret);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply ,"D103" );
	 		return 1;
 		}
		else if( ret==100 )
		{
			if( overflag==0 )
			{
				overflag=1;
				/**没有记录表示仅能是无介质**/
				strcpy(g_mdm_prdt_rel.mdm_code1,NULL_MDM_CODE);
				strcpy(g_mdm_prdt_rel.mdm_code2,NULL_MDM);
				strcpy(g_mdm_prdt_rel.mdm_code3,NULL_MDM);
			}
			else
				break;
		}

		if( overflag==0 ) overflag=2;

		pub_base_strpack(g_mdm_prdt_rel.mdm_code1);
		pub_base_strpack(g_mdm_prdt_rel.mdm_code2);
		pub_base_strpack(g_mdm_prdt_rel.mdm_code3);

		count2=0;
		if ( strcmp(g_mdm_prdt_rel.mdm_code1,NULL_MDM) )
			count2++;
		if ( strcmp(g_mdm_prdt_rel.mdm_code2,NULL_MDM) )
			count2++;
		if ( strcmp(g_mdm_prdt_rel.mdm_code3,NULL_MDM) )
			count2++;
		
		if( count1!=count2 )  continue;

		strcpy(flag2,"000");

		if ( !strcmp(mdm_num1,NULL_MDM) )
			;
		else if ( !strcmp(mdm_num1,g_mdm_prdt_rel.mdm_code1) )
			flag2[0]='1';
		else if ( !strcmp(mdm_num1,g_mdm_prdt_rel.mdm_code2) )
			flag2[1]='1';
		else if ( !strcmp(mdm_num1,g_mdm_prdt_rel.mdm_code3) )
			flag2[2]='1';
		else
			continue;

		if ( !strcmp(mdm_num2,NULL_MDM) )
			;
		else if ( flag2[0]!='1' && !strcmp(mdm_num2,g_mdm_prdt_rel.mdm_code1) )
			flag2[0]='1';
		else if ( flag2[1]!='1' && !strcmp(mdm_num2,g_mdm_prdt_rel.mdm_code2) )
			flag2[1]='1';
		else if ( flag2[2]!='1' && !strcmp(mdm_num2,g_mdm_prdt_rel.mdm_code3) )
			flag2[2]='1';
		else
			continue;

		if ( !strcmp(mdm_num3,NULL_MDM) )
			;
		else if ( flag2[0]!='1' && !strcmp(mdm_num3,g_mdm_prdt_rel.mdm_code1) )
			flag2[0]='1';
		else if ( flag2[1]!='1' && !strcmp(mdm_num3,g_mdm_prdt_rel.mdm_code2) )
			flag2[1]='1';
		else if ( flag2[2]!='1' && !strcmp(mdm_num3,g_mdm_prdt_rel.mdm_code3) )
			flag2[2]='1';
		else
			continue;

		count3=0;
		if( flag2[0]=='1' ) count3++;
		if( flag2[1]=='1' ) count3++;
		if( flag2[2]=='1' ) count3++;
		if( count1==count3 ) break;
	}

	Mdm_prdt_rel_Clo_Sel ();

	if( count1!=count3 ) 
	{
		sprintf( acErrMsg,"产品介质对照表中无此记录[%d][%s]",ret,prdt_num);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"W043" );
		return 1;
	}

    return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_set_drawtype();
 * 函数功能：设定介质的支取方式
 *           
 * 作者/时间:
 *
 * 参数：
 *     输入：介质帐户关系结构
 *          
 *     输出：响应码
 *         
 *   返回值：返回0设定成功，
 *           返回非0设定不成功
 * 修改历史：
 *           
********************************************************************/
int pub_base_set_drawtype(mdm_ac_rel_c)
	struct mdm_ac_rel_c mdm_ac_rel_c;
{
  	ret = Mdm_ac_rel_Ins(mdm_ac_rel_c, g_pub_tx.reply) ;
	
	if( ret && ret!=-239 )
	{
		sprintf( acErrMsg,"增加介质账户对照表出错[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D107" );
		return 1;
	}
	else if( ret ==-239 )
	{
		sprintf( acErrMsg,"增加介质账户对照表出错[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D108" );
		return 1;
	}
  
 	return 0;
}

/**********************************************************************
 * 函 数 名：pub_base_check_draw(char mode[4]);
 * 函数功能：检验介质的支取方式
 *
 * 作者/时间:
 *
 * 参数：
 *     输入：mode[4]  	第一位是否读取mdm_ac_rel:0-否；1-是；
 *						第二位如果留密校验查询密码还是支取密码：0-支取；1-查询；
 *						第三位是否加密密码:0-否；1-是；
 *     输出：
 *
 *   返回值：
 *			 响应码
 *			 返回0成功
 *           返回非0不成功
 * 修改历史：
 *
********************************************************************/
int pub_base_check_draw(char mode[4])
{
	char tmp_draw_pwd[7];
	char tmp_qry_pwd[7];
	
	char filename[128];
	FILE *fp = NULL;
	char tmpstr[128];
	int i = 0;
	int iVal = 0;
	char command[128];
	
	MEMSET_DEBUG( filename, 0x0, sizeof(filename) );
	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf(acErrMsg,"%s,%d,draw_pwd_yn=[%c],draw_id_yn=[%c]",
		__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn[0],g_mdm_ac_rel.draw_id_yn[0]);
	WRITEMSG
	
    /*mod by ChengGX 晋中 2010-1-27 15:10*/
    if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)==0)
    {
        sprintf(acErrMsg,"%s,%d,晋中卡不检查密码 直接返回",__FILE__,__LINE__);
	    WRITEMSG
	    return 0;
    }
	if(strcmp(g_pub_tx.tx_code,"2410")==0)
	{
		sprintf(acErrMsg,"%s,%d,晋中卡不检查密码 直接返回",__FILE__,__LINE__);
		    WRITEMSG
		return 0;
	}
    /*mod by ChengGX 晋中 2010-1-27 15:10*/
	strcpy(tmp_draw_pwd,g_pub_tx.draw_pwd);
	strcpy(tmp_qry_pwd,g_pub_tx.qry_pwd);
	sprintf(acErrMsg,"mode===[%s]",&mode[0]);
        
	pub_base_strpack(g_pub_tx.ac_no);
	WRITEMSG
	if( mode[0]=='1' )
	{		
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
		if( ret==100 )
		{  
			sprintf( acErrMsg,"介质帐号对照表中不存在该记录[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"M002" );
			return 1;
		}
		else if(ret)
		{
			sprintf( acErrMsg,"查询介质帐号对账表异常[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
			return 1;
		}
	}

	WRITEMSG
	/*** if( g_mdm_ac_rel.draw_uncon_yn[0]=='Y' )	return 0;  XXXXX **/

	sprintf( acErrMsg,"%s,%d,g_mdm_ac_rel.draw_pwd_yn=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn[0]);
	WRITEMSG
	/*如果支取方式为密码支取*/
	if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y')
	{    
		/*--add by dgf 20051114 for 判断密码输入是否超限-------------------*/
		sprintf( filename, "/backup/pass_error/%s.%ld",  g_pub_tx.ac_no, g_pub_tx.tx_date );
		vtcp_log( "/backup/pass_error/%s.%ld",  g_pub_tx.ac_no, g_pub_tx.tx_date );
		fp = fopen( filename, "r" );
		if ( fp == NULL )
		{
			goto NoErr;
		}
	sprintf( acErrMsg,"%s,%d,g_mdm_ac_rel.draw_pwd_yn=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn[0]);
	WRITEMSG
		
		while( fgets( tmpstr, 1023, fp ) != NULL )
		{
			i = i + 1;
		}
		
		fclose( fp );
		
		MEMSET_DEBUG( &sCom_parm, 0x0, sizeof(sCom_parm) );
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code = 'MMSR'" );
		if ( ret )
		{
			strcpy( acErrMsg, "查询公共参数表出错!!" );
			WRITEMSG
		  	return 1;
		}
		
		iVal = atoi(sCom_parm.val);
		vtcp_log("密码连续错误最大次数iVal=[%d],i=[%d]!",iVal,i);
		
		if ( i >= iVal )
		{
			strcpy( acErrMsg, "当日密码输入错误已经连续超限,本日不能继续输入!!" );
			WRITEMSG
			/* add by martin 2008-11-19 14:30*/
					{
						sprintf( acErrMsg,"今日密码已锁!" );
				    set_zd_data( DC_GET_MSG,acErrMsg );
					}
		  	strcpy( g_pub_tx.reply, "M036" );
		  	return 1;
		}
		
NoErr:		
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG
		if( mode[1]=='0' )
		{
			/*不可
			pub_base_strpack(g_pub_tx.draw_pwd);
			pub_base_strpack(g_mdm_ac_rel.draw_pwd);
			*/

			if(mode[2]=='0' )
			{
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG
				if( strcmp(g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd)
					&& strcmp(g_pub_tx.tx_code,"2410")!=0)
				{
				/*sprintf(acErrMsg,">>>>密码[%s]----[%s]",g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
				WRITEMSG*/
				strcpy( acErrMsg,"PASSWORD is not correct!!");
				WRITEMSG
				/*if( 4-i>0 )2008-11-19 14:35*/
				if(((iVal-1)-i)>0)
				{
				sprintf( acErrMsg,"还可以试 %d 次",(iVal-1)-i );
				set_zd_data( DC_GET_MSG,acErrMsg );
				}
		  		strcpy( g_pub_tx.reply ,"M005" );
		  		goto ErrExit;
				}
			}
			else if( pub_base_DesChk(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd) 
				&& strcmp(g_pub_tx.tx_code,"2410")!=0 )
			{
				sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
				WRITEMSG
				strcpy( acErrMsg,"PASSWORD is not correct!!");
				WRITEMSG
				/*if( 4-i>0 )2008-11-19 14:35*/
				if((iVal-1)-i>0)
				{
				sprintf( acErrMsg,"还可以试 %d 次",(iVal-1)-i );
				set_zd_data( DC_GET_MSG,acErrMsg );
				}
		  		strcpy( g_pub_tx.reply ,"M005" );
		  		goto ErrExit;
			}
		}
		else if( mode[1]=='1' )
		{
			/*
			pub_base_strpack(g_pub_tx.qry_pwd);
			pub_base_strpack(g_mdm_ac_rel.qry_pwd);
			*/
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG

			if(mode[2]=='0' )
			{
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG
				if(strcmp(g_pub_tx.qry_pwd,g_mdm_ac_rel.qry_pwd)
				  &&strcmp(g_pub_tx.tx_code,"2410")!=0)
				{
				strcpy( acErrMsg,"PASSWORD is not correct!!");
				WRITEMSG
		  		strcpy( g_pub_tx.reply ,"M005" );
		  		goto ErrExit;
				}
			}
			else if( pub_base_DesChk(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.qry_pwd,g_mdm_ac_rel.qry_pwd) )
			{
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG
				strcpy( acErrMsg,"PASSWORD is not correct!!");
				WRITEMSG
		  		strcpy( g_pub_tx.reply ,"M005" );
		  		goto ErrExit;
			}
		}
		else
		{
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG
			strcpy( acErrMsg,"PASSWORD is not correct!!");
			WRITEMSG
		  	strcpy( g_pub_tx.reply ,"M005" );
		  	goto ErrExit;
		}
		
		MEMSET_DEBUG( command, 0x00, sizeof(command) );
    		sprintf( command,"rm -f -r %s", filename );

    		ret = system( command );		
    }
	WRITEMSG
	sprintf(acErrMsg,"%s,%d,draw_pwd_yn=[%c],draw_id_yn=[%c]",
		__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn[0],g_mdm_ac_rel.draw_id_yn[0]);
	WRITEMSG

	if(g_mdm_ac_rel.draw_id_yn[0]=='Y')
	{
		pub_base_strpack(g_mdm_ac_rel.id_no);
		pub_base_strpack(g_pub_tx.id_no);

	sprintf( acErrMsg,"g_mdm_ac_rel.id_type=[%s]g_pub_tx.id_type=[%s]",
		g_mdm_ac_rel.id_type,g_pub_tx.id_type);
	WRITEMSG
	sprintf( acErrMsg,"g_mdm_ac_rel.id_no=[%s],g_pub_tx.id_no=[%s]",
		g_mdm_ac_rel.id_no,g_pub_tx.id_no);
	WRITEMSG
		if( strcmp(g_mdm_ac_rel.id_type,g_pub_tx.id_type) )
		{
			sprintf( acErrMsg,"证件类型不正确!!g_mdm_ac_rel.id_type=[%s], \
					g_pub_tx.id_type=[%s]",g_mdm_ac_rel.id_type,g_pub_tx.id_type);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"P210" );
		   	return 1;
		}
		else if( strcmp(g_mdm_ac_rel.id_no,g_pub_tx.id_no) )
		{	
			sprintf( acErrMsg,"证件号码不正确!!g_mdm_ac_rel.id_no=[%s], \
					g_pub_tx.id_no=[%s]",g_mdm_ac_rel.id_no,g_pub_tx.id_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"M004" );
		   	return 1;
		}	
    }

	if( g_mdm_ac_rel.pwd_mach_yn[0]=='Y' )
	{
		/*** 校验密码器 ***/	
	}

	strcpy(g_pub_tx.draw_pwd,tmp_draw_pwd);
	strcpy(g_pub_tx.qry_pwd,tmp_qry_pwd);

  return 0;
  
ErrExit:

	sprintf( acErrMsg, "----------->密码出错文件名为[%s]!!", filename );
	WRITEMSG

	fp = fopen( filename, "a+" );
	if ( fp == NULL )
	{
		strcpy( acErrMsg, "打开密码错误日志文件错!!" );
		WRITEMSG
	  	strcpy( g_pub_tx.reply, "M005" );
		return 1;
	}
	
	fprintf( fp, "账/卡号[%s]交易码[%s]交易机构[%s]时间[%ld]密码输入错\n", g_pub_tx.ac_no, \
		g_pub_tx.tx_code, g_pub_tx.tx_br_no, g_pub_tx.tx_time );
	
	fclose( fp );
	
	if ( i == iVal - 2 )
	{
		strcpy( acErrMsg, "密码错误,再连续输入错误一次当日不能使用" );
		WRITEMSG
	  	strcpy( g_pub_tx.reply, "M037" );
	  	return 1;
	}

	if ( i == iVal - 1 )
	{
		strcpy( acErrMsg, "当日密码输入错误已经连续超限,本日不能继续输入!!" );
		WRITEMSG
	  	strcpy( g_pub_tx.reply, "M036" );
	  	return 1;
	}

	return 1;
}
/**********************************************************************
 * 函 数 名：pub_base_check_medstate();
 * 函数功能：检查介质状态
 *
 * 作者/时间:
 *
 * 参数：
 *     输入：帐/卡号：			char(19);
 *			 介质代号：			char(5);
 *     输出：
 *			 介质状态:			char(2);
 *			 响应码				char(4);
 *   返回值：
 *			 返回0成功，
 *
********************************************************************/
int pub_base_check_medstate(char *ac_num,char *mdm_num,char *note_sts)
{
	/*检查帐户拥有介质的状态是否存在*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel( &g_pub_tx.reply, &g_mdm_ac_rel, 
			  			"ac_no='%s' and mdm_code='%s'", ac_num,mdm_num);
	if (ret==100)
	{
		sprintf( acErrMsg,"介质帐号对照表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"M002" );
		return 1;
	}
	else if( ret && ret !=100 )
	{
	 	sprintf( acErrMsg,"查询介质帐号对照表异常[%d]",ret);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"D103" );
	 	return 1;
	}

	strcpy(note_sts,g_mdm_ac_rel.note_sts);

	return 0;
}

int pub_base_ins_chrg_mst(char chrg_rate_code[4],char sts[2],double tx_amt)
{
	struct com_chrg_hst_c	sCom_chrg_hst;
	int	ret=0;
	MEMSET_DEBUG(&sCom_chrg_hst,0x00,sizeof(struct com_chrg_hst_c));

	sCom_chrg_hst.tx_date=g_pub_tx.tx_date;
	sCom_chrg_hst.trace_no=g_pub_tx.trace_no;
	strcpy(sCom_chrg_hst.ac_no,g_pub_tx.ac_no);
	strcpy(sCom_chrg_hst.tx_code,g_pub_tx.tx_code);
	strcpy(sCom_chrg_hst.chrg_rate_code,chrg_rate_code);
	strcpy(sCom_chrg_hst.name,g_pub_tx.brf);
	sCom_chrg_hst.chrg_amt=g_pub_tx.tx_amt1;
	sCom_chrg_hst.tx_amt=tx_amt;
	strcpy(sCom_chrg_hst.sts,sts);
	ret=Com_chrg_hst_Ins(sCom_chrg_hst,g_pub_tx.reply);
	if( ret )
	{
	 	sprintf( acErrMsg,"登记收费明细表错误[%d]",ret);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"P448" );
	 	return 1;
	}
	return 0;
}

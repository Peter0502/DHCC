/***************************************************************
* 文 件 名:     pubf_acct_loss.c
* 功能描述：
*              1、检查账号、账号ID和账号序号的合法性；
*              2、登记交易流水；
*
* 入口参数：	mode 挂失解挂方式 11-口挂；12-书挂；13-密挂；14-书挂兼密挂
*								  15-印挂；16-申请书挂失；
*								  21-解除口挂；22-解除书挂；23-解除密挂
*								  24-解除密挂兼书挂
*				iAuto 是否自动解挂 0-正常柜台解挂；1-日终自动解挂
*
* 作    者:	xxx
* 完成日期：    2003年01月20日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "mo_loss_c.h"
#include "mo_coll_c.h"
#include "com_branch_c.h"
#include "trace_log_c.h"

int	LOSS;	/*** 1-挂失；0-解挂 ***/
int  tmpcs=0;
int	 tmpcs1=0;

pub_acct_loss(int mode,int iAuto)
{
	struct	mo_coll_c		g_mo_coll;
	struct	com_branch_c	g_com_branch;

	char br_name[6];
	char tmpstr[6];
	g_reply_int=0;

	memset( &g_mo_coll, 0x00, sizeof(struct mo_coll_c) );
	memset( &g_com_branch, 0x00, sizeof(struct com_branch_c) );

	if( mode==11 ) strcpy(g_pub_tx.brf,"口挂");
	else if( mode==12 ) strcpy(g_pub_tx.brf,"书挂");
	else if( mode==13 ) strcpy(g_pub_tx.brf,"密挂");
	else if( mode==14 ) strcpy(g_pub_tx.brf,"密挂兼书挂");
	else if( mode==15 ) strcpy(g_pub_tx.brf,"印挂");
	else if( mode==16 ) strcpy(g_pub_tx.brf,"申请书挂失");
	else if( mode==21 ) strcpy(g_pub_tx.brf,"解口挂");
	else if( mode==22 ) strcpy(g_pub_tx.brf,"解书挂");
	else if( mode==23 ) strcpy(g_pub_tx.brf,"解密挂");
	else if( mode==24 ) strcpy(g_pub_tx.brf,"解密挂兼书挂");
	else if( mode==25 ) strcpy(g_pub_tx.brf,"解印挂");

	if( mode>20 )												/*解挂*/
	{
		LOSS=0;
		if( mode%10==2 || mode%10==4 ) strcpy(tmpstr,"SGTS"); 	/*解书挂*/
		else if( mode%10==3 )	strcpy(tmpstr,"MGTS");			/*解密挂*/
		else	strcpy(tmpstr,"KGTS");							/*解口挂*/

		g_reply_int=pub_base_GetParm_int(tmpstr,1,&tmpcs);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取参数错误[%s]",tmpstr);
			WRITEMSG
			goto ErrExit;
		}
		g_reply_int=pub_base_GetParm_int("SGTS",2,&tmpcs1);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取参数错误[%s]",tmpstr);
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
		LOSS=1;
	}

	g_reply_int=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
								g_pub_tx.ac_no);	
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"账号不存在[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"账号不存在[%s][%d]",g_pub_tx.ac_no,g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	
	strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);
	pub_base_strpack(g_mdm_ac_rel.id_type);				/*证件类型*/
	pub_base_strpack(g_mdm_ac_rel.id_no);				/*证件号码*/

	/*
	if( mode!=13 && mode!=23 && mode!=15 && mode!=25 &&
		!strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) )
	{
		sprintf(acErrMsg,"该账号没有介质不能作挂失解挂![%s]",
				g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P187");
		goto ErrExit;
	}
	*/

	if( g_mdm_ac_rel.note_sts[0]=='*' || g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"账号已经销户![%s]",g_mdm_ac_rel.note_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}

	/*** Add commence by xxx ***/
	if( g_mdm_ac_rel.coll_sts[0]=='1' && LOSS==1 )
	{
		if( pub_base_checkcollyn( g_mdm_ac_rel.ac_no ) )
		{
			sprintf(acErrMsg,"调用捡拾检查函数出错");
			WRITEMSG
			goto ErrExit;
		}
	}

	/**除口挂外不得通兑**/
	if( strncmp(g_mdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no,5) 
	&& mode!=11 && mode!=21 ) 
		/* XRB LOSS==1 ) */
	{
		if( pub_base_getbrname( g_mdm_ac_rel.opn_br_no,br_name ) )
		{
			sprintf(acErrMsg,"取行名函数错!");
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack(br_name);
		strcpy(g_pub_tx.reply,"D209");
		sprintf(acErrMsg,"交易机构为[%s],开户机构为[%s]",g_pub_tx.tx_br_no,g_mdm_ac_rel.opn_br_no);
		set_zd_data( DC_GET_MSG,acErrMsg );
		WRITEMSG
		return(2);
	}
	/*** complete ***/
	
	if( (mode==13 || mode==14) && g_mdm_ac_rel.draw_pwd_yn[0]=='N' )
	{
		sprintf(acErrMsg,"该户非留密户不能作密挂!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P436");
		goto ErrExit;
	}

	if( LOSS==1 )									/*1-挂失*/
	{
		if( pub_loss( mode ) )
		{
			sprintf(acErrMsg,"挂失函数错误!");
			WRITEMSG
			goto ErrExit;
		}
	}
	else if( pub_un_loss( mode, iAuto ) )				/*0-解挂*/
	{
		sprintf(acErrMsg,"解挂函数错误!");
		WRITEMSG
		goto ErrExit;
	}

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}

OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"挂失解挂主控处理成功!");
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
	
ErrExit:
	sprintf(acErrMsg,"挂失解挂主控处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int pub_loss( int mode )							/*挂失函数*/
{
	struct mo_loss_c	mo_loss_c;
	struct mdm_ac_rel_c	f_mdm_ac_rel;

	memset(&mo_loss_c,0x00,sizeof(struct mo_loss_c));
	memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	if( g_mdm_ac_rel.ac_no[0]=='5' && (mode==13 ||mode==14) )
	{
		sprintf(acErrMsg,"ac_no[%s]!",g_mdm_ac_rel.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P508");
		return 1;
	}
	if( g_mdm_ac_rel.ac_no[0]!='5' && mode==15 )
	{
		sprintf(acErrMsg,"ac_no[%s]!",g_mdm_ac_rel.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P506");
		return 1;
	}
	if( !strcmp(g_mdm_ac_rel.mdm_code,"0000") && mode!=15 && mode!=16 )
	{
		sprintf(acErrMsg,"ac_no[%s]!",g_mdm_ac_rel.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P507");
		return 1;
	}

	/*** 证件检查 *** 前台短交易已经检查了,而且这里检查有问题,前台没传证件号**/
	if(strcmp(g_pub_tx.tx_code,"4405")==0 && memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)!=0 && memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)!=0 )
	{/**存折挂失必须校验密码--wangwenkui-20101230**/
		if (strcmp(g_mdm_ac_rel.id_no,g_pub_tx.id_no)
		|| strcmp(g_mdm_ac_rel.id_type,g_pub_tx.id_type) )
		{
			vtcp_log("机器的[%s][%s]",g_mdm_ac_rel.id_type,g_mdm_ac_rel.id_no);
			vtcp_log("输入的[%s][%s]",g_pub_tx.id_type,g_pub_tx.id_no);
			sprintf(acErrMsg,"证件不符!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P186");
			return 1;
		}
	}
	/******************************************************/

	if( g_mdm_ac_rel.note_sts[0]=='1' )				/*如果该账户已挂失*/
	{
		g_reply_int=Mo_loss_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and unloss_ind='0' ",g_pub_tx.ac_no);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"挂失登记簿异常，请于中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P189");
			return 1;
		}

		g_reply_int=Mo_loss_Fet_Upd(&mo_loss_c,g_pub_tx.reply);
		if( g_reply_int==100 )
		{
			sprintf(acErrMsg,"挂失登记簿无此记录!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P188");
			return 1;
		}
		else if( g_reply_int )
		{
			sprintf(acErrMsg,"挂失登记簿异常，请于中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P189");
			return 1;
		}

		/*** 口挂可以多次挂失 ***/
		if( mode!=11 && (mode%10)==atoi(mo_loss_c.loss_ind) )
		{
			sprintf(acErrMsg,"该账户已经作过此种挂失!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P190");
			return 1;
		}
		
		if( mo_loss_c.loss_ind[0]=='1' )
		{
			if( mode==11 || mode==12 || mode==13 || mode==14 || mode==16 )
			{
				;
			}
			else
			{
				strcpy( g_pub_tx.reply,"P501" );
				return 1;
			}
		}
		else if( mo_loss_c.loss_ind[0]=='2' )
		{
			if( mode==14 || mode==16 )
			{
				;
			}
			else
			{
				strcpy( g_pub_tx.reply,"P502" );
				return 1;
			}
		}
		else if( mo_loss_c.loss_ind[0]=='3' )
		{
			if( mode==14 || mode==16 )
			{
				;
			}
			else
			{
				strcpy( g_pub_tx.reply,"P503" );
				return 1;
			}
		}
		else if( mo_loss_c.loss_ind[0]=='4' )
		{
			if( mode==14 || mode==16 )
			{
				;
			}
			else
			{
				strcpy( g_pub_tx.reply,"P504" );
				return 1;
			}
		}

		if( mo_loss_c.loss_ind[0]=='4' && mode!=16)
		{
			sprintf(acErrMsg,"该账户已经挂失!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P170");
			return 1;
		}
		else if( (mo_loss_c.loss_ind[0]=='2' || mo_loss_c.loss_ind[0]=='3')
			&&  mode!=14 && mode!=16 )
		{
			sprintf(acErrMsg,"该账户已经挂失!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P170");
			return 1;
		}

		if( mode!=16 )
		{
			strcpy(mo_loss_c.unloss_ind,mo_loss_c.loss_ind);
			mo_loss_c.unloss_date=g_pub_tx.tx_date;
			strcpy(mo_loss_c.unloss_tel,g_pub_tx.tel);
			strcpy(mo_loss_c.unloss_chk,g_pub_tx.chk);
			strcpy(mo_loss_c.unloss_auth,g_pub_tx.auth);
			strcpy(mo_loss_c.new_ac_no,mo_loss_c.ac_no);
			mo_loss_c.wrk_date=g_pub_tx.tx_date;
			mo_loss_c.trace_no=g_pub_tx.trace_no;
		}
		else	/*** 申请书挂失 ***/
		{
			mo_loss_c.loss_date=g_pub_tx.tx_date;
			mo_loss_c.wrk_date=g_pub_tx.tx_date;
			mo_loss_c.trace_no=g_pub_tx.trace_no;
			strcpy(mo_loss_c.prop_book_loss,"1");
		}
		g_reply_int=Mo_loss_Upd_Upd(mo_loss_c,g_pub_tx.reply);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"挂失登记簿异常，请于中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P189");
			return 1;
		}
		Mo_loss_Clo_Upd();	
	}
	else if( mode==16 )	/*** 该户没有挂失,且申请书挂失 ***/
	{
		sprintf(acErrMsg,"该户没有挂失，不能挂失申请书!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P441");
		return 1;
	}

	/*** 检查检查密码 ***/
	/***应行里要求去掉密码检查--王文奎--20101217
	TODO:电话银行需要交易密码**/
	/**if( mode!=11 && mode!=13 && mode!=14 
		&& g_mdm_ac_rel.draw_pwd_yn[0]=='Y' ) 
	{
		if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)==0)   *核心不需要校验卡密码 Add by hxc 100620
			{
					sprintf(acErrMsg,"[%s][%d],ac_no[%s]是卡,挂失时核心不需要校验密码!",__FILE__,__LINE__,g_pub_tx.ac_no);
		      WRITEMSG
			}
		else
			{
					** 密挂或书挂兼密挂再挂失申请书的时候不校验密码 **
		      if(mode==16&&(mo_loss_c.loss_ind[0]=='3'||mo_loss_c.loss_ind[0]=='4')) 
		      {
			     ;
		      }
		      else if( pub_base_DesChk(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd) )
		     {
			     sprintf(acErrMsg,"密码不符!");
			     WRITEMSG
			     strcpy(g_pub_tx.reply,"M005");
			     return 1;
		      }
	     }
	}**/
	/**增加存折电话银行挂失密码交易wangwk-20101221**/
	vtcp_log("%s %d g_pub_tx.tx_code===[%s],_pub_tx.draw_pwd_yn[%s] ",
		__FILE__,__LINE__,g_pub_tx.tx_code,g_pub_tx.draw_pwd_yn);
	if(strcmp(g_pub_tx.tx_code,"4405") == 0
	   && strcmp(g_pub_tx.draw_pwd_yn,"N") != 0
	   && memcmp(g_pub_tx.ac_no,CARDHEADCODE,6) !=0 && memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)!=0 )
	{
		vtcp_log("%s %d 检查密码 ",__FILE__,__LINE__);
	   if( pub_base_DesChk(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd) )
	 {
	     sprintf(acErrMsg,"密码不符!");
	     WRITEMSG
	     strcpy(g_pub_tx.reply,"M005");
	     return 1;
	  }
	}
	/**add end --wangwk**/
	if( mode!=16 )
	{
		strcpy(mo_loss_c.ac_no,g_pub_tx.ac_no);
		strcpy(mo_loss_c.id_type,g_pub_tx.tmp_ind);	/** 申请人证件类型 **/
		strcpy(mo_loss_c.id_no,g_pub_tx.cal_code);	/** 申请人证件号码 **/
		strcpy(mo_loss_c.name,g_pub_tx.name);
		sprintf(mo_loss_c.loss_ind,"%1d",mode%10);
		strcpy(mo_loss_c.prop_book_loss,"0");
		strcpy(mo_loss_c.unloss_ind,"0");
	vtcp_log("%s %d tx_br_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.tx_br_no);
		strcpy(mo_loss_c.opn_br_no,g_pub_tx.tx_br_no);
	vtcp_log("%s %d tx_br_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.tx_br_no);
		mo_loss_c.loss_date=g_pub_tx.tx_date;
		strcpy(mo_loss_c.loss_br_no,g_pub_tx.tx_br_no);
		strcpy(mo_loss_c.loss_tel,g_pub_tx.tel);
		strcpy(mo_loss_c.loss_chk,g_pub_tx.chk);
		strcpy(mo_loss_c.loss_auth,g_pub_tx.auth);
		mo_loss_c.unloss_date=0;
		strcpy(mo_loss_c.unloss_tel,"");
		strcpy(mo_loss_c.unloss_chk,"");
		strcpy(mo_loss_c.unloss_auth,"");
		strcpy(mo_loss_c.new_ac_no,"");
		mo_loss_c.wrk_date=g_pub_tx.tx_date;
		mo_loss_c.trace_no=g_pub_tx.trace_no;

		g_reply_int=Mo_loss_Ins(mo_loss_c,g_pub_tx.reply);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"挂失登记簿异常，请于中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P189");
			return 1;
		}

	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.ac_no);
  		if(g_reply_int)
		{
	   		sprintf(acErrMsg,"定义游标错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"取记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(f_mdm_ac_rel.note_sts,"1");

		g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"修改记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );
	}

	return 0;
}

/**** flag=0 正常解挂 flag=1 口挂和挂失换证自动解挂 ****/
/*--------------------------------------------------
* 修改内容: 处理口头挂失自动解挂
*-------------------------------------------------*/
int pub_un_loss( int mode ,int flag )
{
	int	tmpts=0;
	char   br_name[6];
	char   f_loss_ind[2];					/*** 原挂失类型 ***/
	struct mo_loss_c	mo_loss_c;
	struct mdm_ac_rel_c	f_mdm_ac_rel;
	char tmpstr[21];

	memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&mo_loss_c,0x00,sizeof(struct mo_loss_c));
	vtcp_log("%s,%d,mode=[%d][%d]",__FILE__,__LINE__,mode,mode%10);
	sprintf(f_loss_ind,"%1d",mode%10);
	vtcp_log("f_loss_ind=[%s]",f_loss_ind);
	
	/**XRB***/
	{
		if( mode%10==2 || mode%10==4 ) strcpy(tmpstr,"SGTS"); 	/*解书挂*/
		else if( mode%10==3 )	strcpy(tmpstr,"MGTS");			/*解密挂*/
		else	strcpy(tmpstr,"KGTS");							/*解口挂*/

		g_reply_int=pub_base_GetParm_int(tmpstr,1,&tmpcs);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取参数错误[%s]",tmpstr);
			WRITEMSG
			return -1;
		}
		g_reply_int=pub_base_GetParm_int("SGTS",2,&tmpcs1);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取参数错误[%s]",tmpstr);
			WRITEMSG
			return -1;
		}
	}

	g_reply_int=Mo_loss_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and unloss_ind='0'",
								g_pub_tx.ac_no);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"挂失登记簿异常，请于中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P189");
		return 1;
	}

	g_reply_int=Mo_loss_Fet_Upd(&mo_loss_c,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"挂失登记簿无此记录!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P188");
		return 1;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"挂失登记簿异常，请于中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P189");
		return 1;
	}
	vtcp_log("%s %d tx_br_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.tx_br_no);
	pub_base_strpack(mo_loss_c.opn_br_no);
	/**所有挂失都必须到开户所解挂**/
	/*if( strcmp(mo_loss_c.opn_br_no,g_pub_tx.tx_br_no) && mode!=21 )*/
	if( strcmp(mo_loss_c.opn_br_no,g_pub_tx.tx_br_no) && mode!=21 )
	{
		if( pub_base_getbrname( mo_loss_c.opn_br_no,br_name ) )
		{
			sprintf(acErrMsg,"取行名函数错!");
			WRITEMSG
			return 1;
		}

		pub_base_strpack(br_name);
		sprintf(acErrMsg,"请到原挂失网点办理解挂手续![%s][%s]", \
				mo_loss_c.opn_br_no,g_pub_tx.tx_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D210");
		sprintf( acErrMsg,"开户机构为:(%s).",br_name);
		set_zd_data( DC_GET_MSG,acErrMsg );
		WRITEMSG
		return(2);
	}

	pub_base_strpack(mo_loss_c.id_type);
	pub_base_strpack(mo_loss_c.id_no);
			
	/**
	if( atoi(mo_loss_c.loss_ind)!=(mode%10) && flag==0 )
	**/
	if( atoi(mo_loss_c.loss_ind)!=(mode%10) )
	{
		sprintf(acErrMsg,"解挂方式不符![%s][%d]",mo_loss_c.loss_ind,mode);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P191");
		return 1;
	}

	/*计算解挂日期*/
	tmpts=pub_base_CalTrueDays(mo_loss_c.loss_date,g_pub_tx.tx_date);
	if( (mode==22 || mode==24) && tmpts>=tmpcs && tmpcs1==0 && flag==0 )
	{
		sprintf(acErrMsg,"书挂解挂期限已过请作挂失换证或挂失结清!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P192");
		return 1;
	}

	if( flag==1 && tmpts < tmpcs )
	{
		sprintf(acErrMsg,"未到挂失解挂天数,不能解挂![%ld]",mo_loss_c.loss_date);
		WRITEMSG
		sprintf(acErrMsg,"ts=[%d],tmpcs=[%d]",tmpts,tmpcs);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P198");
		return 1;
	}
	vtcp_log("%s,%d,ts=[%d],tmpcs=[%d]",__FILE__,__LINE__,tmpts,tmpcs);

	if (flag == 0)	/*----- 自动解挂不检查证件和密码 -----*/
	{
		/*** 检查检查密码 ***/
		/**不验证密码-wangwenkui-20110106**
		if( g_mdm_ac_rel.draw_pwd_yn[0]=='Y' && ( mode==21 || mode==22
			|| ( (mode==23 || mode==24) && tmpts<tmpcs) ) ) 

		**密户解除口书挂必须凭密**
		if( g_mdm_ac_rel.draw_pwd_yn[0]=='Y' && (mode==21 || mode==22) )
		{
			if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)==0 )   **核心不需要校验卡密码 Add by hxc 100620**
				{
					sprintf(acErrMsg,"[%s][%d],ac_no[%s]是卡,解挂时核心不需要校验密码!",__FILE__,__LINE__,g_pub_tx.ac_no);
		      WRITEMSG
				}
		  else
		  	{ *** 检查检查密码 ***
					***应行里要求去掉密码检查--王文奎--20101217
					TODO:电话银行需要交易密码**
		  		***if( pub_base_DesChk(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd) )
			     {
				     sprintf(acErrMsg,"密码不符!");
				     WRITEMSG
				     strcpy(g_pub_tx.reply,"M005");
				     return 1;
			      }
			     **
		     }
		}
		**/
		
	vtcp_log("%s,%d,ts=[%d],tmpcs=[%d]",__FILE__,__LINE__,tmpts,tmpcs);
	if( g_mdm_ac_rel.draw_pwd_yn[0]=='Y' && (mode==23 || mode==24) )
	{
		if( tmpts<tmpcs )
		{
			if( pub_base_DesChk(g_pub_tx.tx_date,
				g_pub_tx.ac_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd) )
			{
				sprintf(acErrMsg,"密码挂失在挂失期限内解挂，必须凭原密码");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P505" );
				return 1;
			}
		}
	}

		/*** 证件检查 ***/
		if( ( strcmp(mo_loss_c.id_no,g_pub_tx.cal_code)
			||  strcmp(mo_loss_c.id_type,g_pub_tx.tmp_ind) ) 
			&& ( strcmp(g_mdm_ac_rel.id_no,g_pub_tx.cal_code)
			|| strcmp(g_mdm_ac_rel.id_type,g_pub_tx.tmp_ind) )
			&& mo_loss_c.loss_date>=20050827 )
		{
			if( mode!=21 )
			{
			vtcp_log("[%s][%s]",mo_loss_c.id_type,mo_loss_c.id_no);
			vtcp_log("[%s][%s]",g_mdm_ac_rel.id_type,g_mdm_ac_rel.id_no);
			vtcp_log("[%s][%s]",g_pub_tx.tmp_ind,g_pub_tx.cal_code);
			sprintf(acErrMsg,"证件不符!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P186");
			return 1;
			}
		}
	}

	strcpy(mo_loss_c.unloss_ind,mo_loss_c.loss_ind);
	mo_loss_c.unloss_date=g_pub_tx.tx_date;
	strcpy(mo_loss_c.unloss_tel,g_pub_tx.tel);
	strcpy(mo_loss_c.unloss_chk,g_pub_tx.chk);
	strcpy(mo_loss_c.unloss_auth,g_pub_tx.auth);
	strcpy(mo_loss_c.new_ac_no,mo_loss_c.ac_no);
	mo_loss_c.wrk_date=g_pub_tx.tx_date;
	mo_loss_c.trace_no=g_pub_tx.trace_no;

	g_reply_int=Mo_loss_Upd_Upd(mo_loss_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"挂失登记簿异常，请于中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P189");
		return 1;
	}
	Mo_loss_Clo_Upd();	
	vtcp_log("%s,%d,mode=[%d]", __FILE__, __LINE__, mode);
	/**只要口挂书挂的的时候必须重置密码--wangwenkui-20110106**/
	if( (mode==21 || mode==22 || mode==23 || mode==24) /**&& tmpts>=tmpcs**/ )
	{
		/*vtcp_log("%s,%d,draw_pwd[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
		pub_base_EnDes(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd);

		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
										g_pub_tx.ac_no);
  		if(g_reply_int)
		{
	   		sprintf(acErrMsg,"定义游标错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"取记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(f_mdm_ac_rel.note_sts,"0");
		vtcp_log("[%s][%d] Pwd_yn = [%s] \n",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn);
		/****解密挂时，如果选择'N'(无密)，则更新mdm_ac_rel 密码标志为'N'  modify by wanglei 20070517****/
		if(flag == 0)
			{
				if(g_pub_tx.draw_pwd_yn[0] == 'Y')
				{
					strcpy(f_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd);
					/**添加以防无密改为有密是密码标志不符-wangwk2010-3-23 17:13**/
					f_mdm_ac_rel.draw_pwd_yn[0] = 'Y';       
				}else{
					f_mdm_ac_rel.draw_pwd_yn[0] = 'N';
				}
			}
		/**** end  20070517 23:00****/

		g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"修改记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );
	}
	else
	{
		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
										g_pub_tx.ac_no);
  		if(g_reply_int)
		{
	   		sprintf(acErrMsg,"定义游标错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"取记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(f_mdm_ac_rel.note_sts,"0");

		g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"修改记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );
	}

	if( g_reply_int )
	{
		sprintf(acErrMsg,"更新介质账户对照表错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P147");
		return 1;
	}

	return 0;
}

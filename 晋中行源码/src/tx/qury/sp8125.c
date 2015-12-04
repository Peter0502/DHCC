/*************************************************
* 文 件 名:  sp8125.c
* 功能描述： 活期账户明细信息
*
* 作    者:Jarod  
* 完成日期： 
*
* 修改记录：
* 日   期:20070315
* 修 改 人:
* 修改内容:非本网点或本网点下属网点帐户信息不能查询
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"

int sp8125()
{
	char prdt_name[51];
	char note_sts[10];
	long pri_line=0;
	long ac_num=0;
	int ret=0;
	char cTypename[21];

	struct dd_mst_c vdd_mst;
	struct mdm_ac_rel_c mdm_ac_rel;
	struct com_branch_c g_com_branch;
	char ac_no[25];
	int ac_seqn=0;
	long ac_id=0;
	char f_sttl_type9[41];
	char f_ac_type10[41];
	char f_intst_type11[41];
	char f_odtt_ind15[41];
	char f_ac_sts16[41];
	char f_hold_sts17[41];
	char f_od_ind20[41];
	char f_draw_pwd21[41];
	char mdm_ind[11];

	memset(f_sttl_type9,0x00,sizeof(f_sttl_type9));
	memset(f_ac_type10,0x00,sizeof(f_ac_type10));
	memset(f_intst_type11,0x00,sizeof(f_intst_type11));
	memset(f_odtt_ind15,0x00,sizeof(f_odtt_ind15));
	memset(f_ac_sts16,0x00,sizeof(f_ac_sts16));
	memset(f_hold_sts17,0x00,sizeof(f_hold_sts17));
	memset(f_od_ind20,0x00,sizeof(f_od_ind20));
	memset(f_draw_pwd21,0x00,sizeof(f_draw_pwd21));
	memset(&vdd_mst,0x00,sizeof(struct dd_mst_c));
	memset(ac_no,0x00,sizeof(ac_no));
	memset(&ac_seqn,0x00,sizeof(ac_seqn));	
	memset( &mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );
	memset(opn_br_no,'\0',sizeof(opn_br_no));

	pub_base_sysinit();

	get_zd_data("0300",ac_no);
	get_zd_int("0200",&ac_seqn);
    pub_base_old_acno(ac_no);

	pub_base_strpack(ac_no);
	if ( pub_base_acno_acid(&ac_id,ac_no) )
	{
		TRACE
		goto ErrExit;
	}


	ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst,"ac_id='%ld' and ac_seqn='%d'",ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"[%s][%d]没有找到对应的记录!![%d]",__FILE__,__LINE__,ret);
		WRITEMSG
		goto GoodExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"[%s][%d]查找记录出错!![%d]",__FILE__,__LINE__,ret);
		WRITEMSG
		goto ErrExit;
	}
		/**显示列标题**/
		/*fprintf( fp,"~账号|序号|@户名|@产品名称|$余额|@开户行|$昨日余额|利率|@结算类型|@账户类型|@计息类型|开户日期|上笔日期|@帐户状态|@冻结|@介质状态|$冻结金额|$控制金额|@其他介质|客户号|@凭证号|@证件类型|@证件号码|\n" );*/
		fprintf( fp,"~账号|序号|@户名|@产品名称|$余额|@开户行|开户日期|@帐户状态|\n" );
	}


	if(vdd_mst.cif_no<-2147000000) vdd_mst.cif_no=0;
	if(vdd_mst.bal<-2147000000) vdd_mst.bal=0.0;
	if(vdd_mst.ys_bal<-2147000000) vdd_mst.ys_bal=0.0;
	if(vdd_mst.rate<-2147000000) vdd_mst.rate=0.0;
	pub_base_dic_show_str(f_sttl_type9,"sttl_type",vdd_mst.sttl_type);
	pub_base_dic_show_str(f_ac_type10,"ac_type",vdd_mst.ac_type);
	pub_base_dic_show_str(f_intst_type11,"intst_knd",vdd_mst.intst_type);
	if(vdd_mst.opn_date<-2147000000) vdd_mst.opn_date=0;
	if(vdd_mst.ic_date<-2147000000) vdd_mst.ic_date=0;
	if(vdd_mst.lst_date<-2147000000) vdd_mst.lst_date=0;
	pub_base_dic_show_str(f_odtt_ind15,"odtt_ind",vdd_mst.odtt_ind);
	pub_base_dic_show_str(f_ac_sts16,"dd_ac_sts",vdd_mst.ac_sts);
	pub_base_dic_show_str(f_hold_sts17,"hold_sts",vdd_mst.hold_sts);
	if(vdd_mst.hold_amt<-2147000000) vdd_mst.hold_amt=0.0;
	if(vdd_mst.ctl_amt<-2147000000) vdd_mst.ctl_amt=0.0;
	pub_base_dic_show_str(f_od_ind20,"yes_no",vdd_mst.od_ind);
	pub_base_dic_show_str(f_draw_pwd21,"yes_no",vdd_mst.draw_pwd);
			
	/* 取出账户 */
	if( strlen(ac_no) )
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_t1," ac_no='%s' ",ac_no );
	}
	else
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_t1," ac_id=%ld and ac_seqn=%d ",vdd_mst.ac_id,vdd_mst.ac_seqn );
	}
		if( ret==100 )
		{
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_t1," ac_id=%ld and ac_seqn=%d ",vdd_mst.ac_id,ALL_SEQN );
			TRACE
			if( ret==100 )
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_t1," ac_id=%ld ",vdd_mst.ac_id );
			TRACE
		}else{
			TRACE
			pri_line = mdm_ac_rel_t1.prt_line ;
		}
 
		/* 统计介质账号对照表中ac_id与该账号ac_id的账户个数*/
		ac_num = sql_count( "mdm_ac_rel" , " ac_id=%ld and note_sts not in('4','5','*') " , vdd_mst.ac_id );
		sprintf(acErrMsg, "CGH ******* ac_num = [%d]", ac_num);
		WRITEMSG
		if( mdm_ac_rel_t1.note_sts[0]!='*' )
		{
			if( ac_num==1 )
			{
				strcpy( mdm_ind , "无" );
			}else{				
				strcpy( mdm_ind , "有" );
			}
		}else{
			if( ac_num==0 )
			{
				strcpy( mdm_ind , "无" );
			}else{				
				strcpy( mdm_ind , "有" );
			}
		}
								
		g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", vdd_mst.prdt_no);
		if(g_reply_int==100)
		{
			sprintf(acErrMsg,"活期产品不存在\n");
			WRITEMSG
			strcpy(g_pub_tx.reply,"N062");

		}
		strcpy(prdt_name,g_dd_parm.title);			/*产品名称*/			

		/*冻结状态*/
		memset(cTypename,0x0,sizeof(cTypename));
		pub_base_dic_show_str(note_sts,"mdm_sts",mdm_ac_rel_t1.note_sts);
		pub_base_dic_show_str(cTypename,"id_type",mdm_ac_rel_t1.id_type);

		/*fprintf( fp,"~账号|序号|@户名|@产品名称|$余额|@开户行|$昨日余额|利率|@结算类型|@账户类型|@计息类型|开户日期|上笔日期|@帐户状态|@冻结|@介质状态|$冻结金额|$控制金额|@其他介质|客户号|@凭证号|@证件类型|@证件号码|\n" );
			fprintf( fp,"~账号|序号|@户名|@产品名称|$余额|@开户行|开户日期|@帐户状态|\n" );
	fprintf( fp, "%s|%d|%s|%s|%.2lf|%s|%.2lf|%.5lf|%s|%s|%s|%ld|%ld|%s|%s|%s|%.2lf|%.2lf|%s|%ld|%.16s|%s|%s|\n",
				ac_no,vdd_mst.ac_seqn,vdd_mst.name,prdt_name,
				vdd_mst.bal,vdd_mst.opn_br_no,vdd_mst.ys_bal,
				vdd_mst.rate,f_sttl_type9,f_ac_type10,f_intst_type11,
				vdd_mst.opn_date,vdd_mst.lst_date,
				f_ac_sts16,f_hold_sts17,note_sts,vdd_mst.hold_amt,
				vdd_mst.ctl_amt,mdm_ind,
				vdd_mst.cif_no,mdm_ac_rel_t1.note_no,cTypename,mdm_ac_rel_t1.id_no);*/

 
	fprintf( fp, "%s|%d|%s|%s|%.2lf|%s|%ld|%s|\n", ac_no0,vdd_mst.ac_seqn,vdd_mst.name,prdt_name,vdd_mst.bal,vdd_mst.opn_br_no, vdd_mst.opn_date, f_ac_sts16); 
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

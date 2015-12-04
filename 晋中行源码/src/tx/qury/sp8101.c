/*************************************************
* 文 件 名:  sp8101.c
* 功能描述： 活期账户信息查询
*
* 作    者:  
* 完成日期： 
*
* 修改记录：
* 日   期:2004.12.7
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
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "cif_addr_inf_c.h"
#include "com_rate_c.h"

int sp8101()
{
	struct com_rate_c g_com_rate;
	struct dd_mst_c vdd_mst;
	struct dd_mst_c f_dd_mst;
	struct dd_mst_c f_dd_msta;
	char ac_no_str[25];
	char ac_no0[25];
	char f_sttl_type9[41];
	char f_ac_type10[41];
	char f_intst_type11[41];
	char f_odtt_ind15[41];
	char f_ac_sts16[41];
	char f_hold_sts17[41];
	char f_od_ind20[41];
	char f_draw_pwd21[41];
	char f_acno0[25];
	char mdm_ind[11];
	char prdt_name[51];
	char note_sts[10];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		long pri_line=0;
		long unprt_line=0;
		long ac_num=0;
		int ret=0;
		FILE *fp;
		int needjg=0;
	int f_acno=0;
	int f_name=0;
	int f_cif=0;
	int f_amt=0;
	int f_brno=0;
	int f_prdt=0;
	int f_opn=0;
	char cPwdind[3];
	char cIdind[3];   /****证件支取标志****/
	char cTypename[21];
	struct mdm_ac_rel_c mdm_ac_rel_t1;
	struct com_branch_c g_com_branch;
	struct cif_addr_inf_c vdd_cif_addr_inf;
	char opn_br_no[6];
	memset(cPwdind,0x0,sizeof(cPwdind));
	memset(cIdind,0x0,sizeof(cIdind));
	memset(cTypename,0x0,sizeof(cTypename));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
  memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &mdm_ac_rel_t1 , 0x00 , sizeof( struct mdm_ac_rel_c ) );
	memset(opn_br_no,'\0',sizeof(opn_br_no));

	pub_base_sysinit();

	get_zd_data("0030",opn_br_no);
	get_zd_data("0380",f_acno0);
    pub_base_old_acno( f_acno0 );
	get_zd_long("0510",&f_dd_mst.ac_seqn );
	get_zd_long("0540",&f_dd_mst.cif_no );
	get_zd_data("0700",f_dd_mst.ac_sts);
	get_zd_data("0260",f_dd_mst.name );
	get_zd_data("0230",f_dd_mst.prdt_no );
	get_zd_double("1001",&f_dd_mst.bal );
	get_zd_double("1002",&f_dd_msta.bal );
	get_zd_data("0910",f_dd_mst.opn_br_no );
	get_zd_long("0290",&f_dd_mst.opn_date );
	get_zd_long("0440",&f_dd_msta.opn_date );
	while(1){
		/**Add by XJ,20100704,查询条件检查****/
		/***
		规则(依次检查)：
			1.如果输入了账号,允许查询
			2.如果输入了客户号,允许查询
			3.如果输入了户名,允许查询
			4.如果输入了身份证号,允许查询	
			5.如果未输入开户机构,不允许查询
			6.如果开户日期范围在同一天,允许查询
			8.业务种类、产品代码和科目号都未输入,不允许查询
		***/
		if(strlen(f_acno0)>0){
			break;
		}	
		if(f_dd_mst.cif_no>0){
			break;
		}
		if(strlen(f_dd_mst.ac_sts)>0){
			break;
		}
		if(strlen(f_dd_mst.name)>0){
			break;
		}
		if(strlen(g_mdm_ac_rel.id_no)>0){
			break;
		}
		if(strlen(f_dd_mst.opn_br_no)==0 && memcmp(f_dd_mst.opn_br_no,"10001",sizeof(f_dd_mst.opn_br_no)-1)!=0){
			strcpy(g_pub_tx.reply,"AT08");
			set_zd_data(DC_GET_MSG,"请输入开户机构!");
			goto ErrExit;
		}
		if(f_dd_mst.opn_date>0 && f_dd_msta.opn_date==f_dd_mst.opn_date){
			break;
		}
		if(strlen(f_acno0)==0 && f_dd_mst.cif_no ==0 &&
				strlen(f_dd_mst.name)==0 && strlen(g_mdm_ac_rel.id_no)==0 && f_dd_mst.opn_date==0 && f_dd_msta.opn_date==0&&strlen(f_dd_mst.ac_sts)==0){
			strcpy(g_pub_tx.reply,"AT08");
			set_zd_data(DC_GET_MSG,"不能所有条件都为空!");
			goto ErrExit;
		}
		break;
	}

	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_dd_mst.ac_id,f_acno0) )
		{
			TRACE
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_dd_mst.ac_id );
		strcat( wherelist,tmpstr );
		needjg=1;
		f_acno=1;
	}
	if( f_dd_mst.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_dd_mst.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	if( f_dd_mst.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_dd_mst.cif_no );
		strcat( wherelist,tmpstr );
		needjg=1;
		f_cif=1;
	}
	pub_base_strpack( f_dd_mst.name );
	if( strlen(f_dd_mst.name) )
	{
		/****Modified by SSH,2006.8.28,match->like****/
		sprintf( tmpstr," name like '%%%%%s%%%%' and ",f_dd_mst.name );
		strcat( wherelist,tmpstr );
		needjg=1;
		f_name=1;
	}
	pub_base_strpack( f_dd_mst.prdt_no );
	/****允许查询所有对公活期户 "999"代表所有对公活期  modify by wanglei  20070514****/
	/****对公活期查询 把产品为107 7天通知存款 去掉  modify by wudawei  20130820****/
	if(memcmp(f_dd_mst.prdt_no,"999",sizeof(f_dd_mst.prdt_no)) == 0)
	{
		sprintf( tmpstr," prdt_no <> '101' and prdt_no <> '107' and ",f_dd_mst.prdt_no );
		strcat( wherelist,tmpstr );
		f_prdt=1;
	}
	else if( strlen(f_dd_mst.prdt_no) )
	{
		sprintf( tmpstr," prdt_no='%s' and ",f_dd_mst.prdt_no );
		strcat( wherelist,tmpstr );
		f_prdt=1;
	}
	/****end 20070514****/
	if( f_dd_mst.bal )
	{
		sprintf( tmpstr," bal>=%lf and",f_dd_mst.bal );
		strcat( wherelist,tmpstr );
		f_amt+=1;
	}
	if( f_dd_msta.bal )
	{
		sprintf( tmpstr," bal<=%lf and",f_dd_msta.bal );
		strcat( wherelist,tmpstr );
		f_amt+=1;
	}
	pub_base_strpack( f_dd_mst.opn_br_no );
	if( strlen(f_dd_mst.opn_br_no) )
	{
		if(memcmp(f_dd_mst.opn_br_no,QS_BR_NO, 5) == 0||memcmp(f_dd_mst.opn_br_no, KJ_BR_NO, 5) == 0)
		{
		}
		else
		{
			sprintf( tmpstr," opn_br_no='%s' and",f_dd_mst.opn_br_no );
			strcat( wherelist,tmpstr );
		}
	}
	if( f_dd_mst.opn_date )
	{
		sprintf( tmpstr," opn_date>=%ld and",f_dd_mst.opn_date );
		strcat( wherelist,tmpstr );
		f_opn++;
	}
	if( f_dd_msta.opn_date )
	{
		sprintf( tmpstr," opn_date<=%ld and",f_dd_msta.opn_date );
		strcat( wherelist,tmpstr );
		f_opn++;
	}
	/**add 20150508**/
	if(strlen(f_dd_mst.ac_sts))
	{
		sprintf( tmpstr," ac_sts='%s' and",f_dd_mst.ac_sts);
		strcat(wherelist,tmpstr);
	}
	strcpy( tablelist,"dd_mst" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY opn_br_no,ac_id , ac_seqn ") ;

	strcpy( fieldlist,"ac_id,ac_seqn,name,cif_no,opn_br_no,prdt_no,bal,ys_bal,rate,sttl_type,ac_type,intst_type,opn_date,ic_date,lst_date,odtt_ind,ac_sts,hold_sts,hold_amt,ctl_amt,od_ind,draw_pwd");

	ret=Dd_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Dd_mst_Fet_Sel( &vdd_mst, g_pub_tx.reply );
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
			/*fprintf( fp,"~账号|序号|@户名|@产品名称|$余额|@开户行|$昨日余额|利率|@结算类型|@账户类型|@计息类型|开户日期|上笔日期|@帐户状态|@冻结|@介质状态|$冻结金额|$控制金额|@其他介质|客户号|@凭证号|@证件类型|@证件号码|\n" );*/
			fprintf( fp,"~账号|@户名|@产品名称|$余额|@开户行|开户日期|@帐户状态|\n" );
		}


		pub_base_acid_acno( vdd_mst.ac_id,vdd_mst.ac_seqn,ac_no0 );
		if(vdd_mst.cif_no<-2147000000) vdd_mst.cif_no=0;
		if(vdd_mst.bal<-2147000000) vdd_mst.bal=0.0;
		if(vdd_mst.ys_bal<-2147000000) vdd_mst.ys_bal=0.0;
		if(vdd_mst.rate<-2147000000) vdd_mst.rate=0.0;
		pub_base_dic_show_str(f_sttl_type9,"sttl_type",vdd_mst.sttl_type);
		
		memset(f_ac_type10,0x0,sizeof(f_ac_type10));
		vtcp_log( "f_ac_type10=== [%s]",f_ac_type10 );
		vtcp_log( "vdd_mst.ac_type=== [%s]",vdd_mst.ac_type );
		pub_base_dic_show_str(f_ac_type10,"ac_type",vdd_mst.ac_type);
		vtcp_log( "f_ac_type10=== [%s]",f_ac_type10 );
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
		if(vdd_mst.intst_acm<-2147000000) vdd_mst.intst_acm=0.0;  /****积数****/
		/**pub_base_dic_show_str(cPwdind,"yes_no",vdd_mst.draw_pwd); del by hxc091207**/
				
		/* 取出账户 */
	if( strlen(f_acno0) )
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_t1," ac_no='%s' ",f_acno0 );
	}
	else
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_t1," ac_id=%ld and ac_seqn=%d ",vdd_mst.ac_id,vdd_mst.ac_seqn );
	}
		/**Add by hxc 091207**/
		memset(cPwdind,0x0,sizeof(cPwdind));
		vtcp_log( "[%s][%d]mdm_ac_rel_t1.draw_pwd_yn=== [%s]",__FILE__,__LINE__,mdm_ac_rel_t1.draw_pwd_yn );
		pub_base_dic_show_str(cPwdind,"yes_no",mdm_ac_rel_t1.draw_pwd_yn);
		vtcp_log( "[%s][%d]cPwdind=== [%s]",__FILE__,__LINE__,cPwdind );
		/**End by hxc 091207**/

	    memset(cIdind,0x0,sizeof(cIdind));
		vtcp_log( "[%s][%d]mdm_ac_rel_t1.draw_id_yn=== [%s]",__FILE__,__LINE__,mdm_ac_rel_t1.draw_id_yn );
		pub_base_dic_show_str(cIdind,"yes_no",mdm_ac_rel_t1.draw_id_yn);
		vtcp_log( "[%s][%d]cIdind=== [%s]",__FILE__,__LINE__,cIdind );


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
		/*地址,劳保号*/
		ret=Cif_addr_inf_Sel( g_pub_tx.reply , &vdd_cif_addr_inf , "cif_no='%ld' and addr_seqn='1'" , vdd_mst.cif_no);						
		if( ret==100 )
			{
				sprintf(acErrMsg,"地址/劳保号不存在\n");
				WRITEMSG
				strcpy(g_pub_tx.reply,"C067");
			}
		g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", vdd_mst.prdt_no);
		if(g_reply_int==100)
		{
			sprintf(acErrMsg,"活期产品不存在\n");
			WRITEMSG
			strcpy(g_pub_tx.reply,"N062");

		}
		strcpy(prdt_name,g_dd_parm.title);			/*产品名称*/			
		pub_base_strpack( vdd_cif_addr_inf.addr );
		/*** 如果产品号是142担保保证金 或 账户类型是6 协定户  就显示分户利率***/
		 vtcp_log( "%s,%d, vdd_mst.ac_type[0]=[%c]",__FILE__,__LINE__,vdd_mst.ac_type[0]);
		if(vdd_mst.ac_type[0]!='6' && strcmp(vdd_mst.prdt_no,"142")!=0 )
		{
			memset(&g_com_rate,'\0',sizeof(g_com_rate));
			ret=Com_rate_Sel(g_pub_tx.reply,&g_com_rate,"cur_no='01' and rate_code='%s' and end_date>=99999999",g_dd_parm.rate_no);
		  vdd_mst.rate=(1+vdd_mst.flt_ratio)*g_com_rate.rate_val;
    }
		/*冻结状态*/
		memset(cTypename,0x0,sizeof(cTypename));
		pub_base_dic_show_str(note_sts,"mdm_sts",mdm_ac_rel_t1.note_sts);
		pub_base_dic_show_str(cTypename,"id_type",mdm_ac_rel_t1.id_type);
		/*fprintf( fp,"~账号|序号|@户名|@产品名称|$余额|@开户行| $昨日余额|利率|结算类型|@账户类型|@计息类型|开户日期| 上笔日期|@帐户状态| @冻结|@介质状态|$冻结金额|$控制金额| @其他介质|客户号|@凭证号|@证件类型|@证件号码|\n" );
	fprintf( fp, "%s|%d|%s|%s|%.2lf|%s| %.2lf|%.5lf|%s|%s|%s|%ld| %ld|%s| %s|%s|%.2lf|%.2lf| %s|%ld|%.16s|%s|%s|\n",
				ac_no0,vdd_mst.ac_seqn,vdd_mst.name,prdt_name,
				vdd_mst.bal,vdd_mst.opn_br_no,vdd_mst.ys_bal,
				vdd_mst.rate,f_sttl_type9,f_ac_type10,f_intst_type11,
				vdd_mst.opn_date,vdd_mst.lst_date,
				f_ac_sts16,f_hold_sts17,note_sts,vdd_mst.hold_amt,
				vdd_mst.ctl_amt,mdm_ind,
				vdd_mst.cif_no,mdm_ac_rel_t1.note_no,cTypename,mdm_ac_rel_t1.id_no);*/

   /** 7天虚拟账户 不显示后5位 **代替 add by wudawei 20130820  **/
 	 if(strcmp(vdd_mst.prdt_no,"107")==0)
	 {
	 		memset(ac_no_str,0x0,sizeof(ac_no_str));
	 	 	memcpy(ac_no_str,ac_no0,8);
	 	 	strcat(ac_no_str,"*****");
	 	 	memcpy(ac_no0,ac_no_str,13);
	 }
	 /** add end 20130820**/
	 /*账号|序号|@户名|@产品名称|$余额|@开户行|开户日期|@帐户状态*/
	 /*昨日余额|利率|@结算类型|@账户类型|@计息类型|上笔日期|*/
	 /*冻结|@介质状态|$冻结金额|$控制金额|*/
	 /*@其他介质|客户号|@凭证号|@证件类型|@证件号码|*/
	fprintf( fp, "%s|%20s|%s|%.2lf|%s|%ld|%s|%.2lf|%.5lf|%s|%s|%s|%ld|%s|%s|%.2lf|%.2lf|%s|%ld|%.16s|%s|%s|%.2lf|%s|%s|%s|\n", \ 
		 ac_no0,vdd_mst.name,prdt_name,vdd_mst.bal,vdd_mst.opn_br_no, vdd_mst.opn_date, f_ac_sts16 \
		,vdd_mst.ys_bal,vdd_mst.rate,f_sttl_type9,f_ac_type10,f_intst_type11,vdd_mst.lst_date, \
		f_hold_sts17,note_sts,vdd_mst.hold_amt,vdd_mst.ctl_amt, \
		mdm_ind,vdd_mst.cif_no,mdm_ac_rel_t1.note_no,cTypename,mdm_ac_rel_t1.id_no,vdd_mst.intst_acm,cPwdind,cIdind,vdd_cif_addr_inf.addr); 
	 ttlnum++;
	}

	ret=Dd_mst_Clo_Sel( );
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

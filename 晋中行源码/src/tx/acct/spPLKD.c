/*************************************************
* 文 件 名: spPLKD.c
* 功能描述: 一贷多借,仿照spWNJZ.c写
*
* 作    者: XJ
* 完成日期: 20101007
*
* 修改日期: 
* 修 改 人: 
* 修改内容:
*
insert into tx_flow_def  values ('2508','1','PLKD','#$');
insert into tx_sub_def  values ('PLKD','一贷多借记账','spPLKD','0','0');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include <stdio.h>
#include "prdt_ac_id_c.h"
#include "sub_dd_mst_c.h"
#include "dc_acc_rel_c.h"

spPLKD()
{
	char cInput_acno1[20],cInput_acno2[20];
	int ret=0;
	int iflag1=0,iflag2=0;
	long sub_ac_seqn=0;
	char tmpstr[1024];
	char fldstr[10][100];
	char tmp_sub_tx_code[5];
	char cNote_type_bk[4],cBeg_note_no_bk[17];
	double dTemp_amt = 0.00;
	double dTmp_amt = 0.00;
	double tot_amt=0.00;
	double tot_amt1=0.00;
	long   tmp_svc_ind=0;
	int i=0;
	FILE *fp=NULL;
	char filename[248];
	struct  sub_dd_mst_c     sSub_dd_mst;
	struct  sub_dd_mst_c     gSub_dd_mst;	
	struct mdm_ac_rel_c sMdm_ac_rel;	
	struct dc_acc_rel_c sDc_acc_rel;
	struct  prdt_ac_id_c    sPrdt_ac_id;

	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));	
	memset(&sSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(&sDc_acc_rel,0x00,sizeof(struct dc_acc_rel_c));
	memset(cInput_acno1,0x00,sizeof(cInput_acno1));
	memset(cInput_acno2,0x00,sizeof(cInput_acno2));
	memset(tmpstr, 0x0, sizeof(tmpstr) );
	memset(fldstr,0x0,sizeof(fldstr));
	memset(filename,0x0,sizeof(filename));
	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_long("0440",&sub_ac_seqn);
	get_zd_double("0400",&g_pub_tx.tx_amt1);
	tot_amt1=g_pub_tx.tx_amt1;
	pub_base_old_acno( g_pub_tx.ac_no );
	strcpy(g_pub_tx.cur_no,"01");
	/* g_pub_tx.sub_ac_seqn=sub_ac_seqn; */
	pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
	if(g_pub_tx.ac_no[0]=='9' && strlen(g_pub_tx.ac_no)==13)/* 内部账 */
		g_pub_tx.ac_seqn=0;
	strcpy( g_pub_tx.ct_ind,"2" );		
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/

	memset(cNote_type_bk,0x00,sizeof(cNote_type_bk));
	memset(cBeg_note_no_bk,0x00,sizeof(cBeg_note_no_bk));
	memcpy(cNote_type_bk,g_pub_tx.note_type,sizeof(cNote_type_bk)-1);
	memcpy(cBeg_note_no_bk,g_pub_tx.beg_note_no,sizeof(cBeg_note_no_bk)-1);
	/****贷方记账*********/
	/* 遂宁财政集中户特殊处理 */
	if(sub_ac_seqn>0)
	{
		ret = Sub_dd_mst_Sel(g_pub_tx.reply,&sSub_dd_mst,"ac_no='%s' and sub_ac_seqn=%ld and ac_sts='1'",g_pub_tx.ac_no,sub_ac_seqn);
		if(ret==100){
			vtcp_log("[%s][%d] 二级户查询错误  ",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"SN08");
			goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg,"查询sub_dd_mst出错!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}else{
	        vtcp_log("[%s][%d]对应的主账户是[%s]\n",__FILE__,__LINE__,sSub_dd_mst.ac_no);
			strcpy( g_pub_tx.hst_ind,"1" );
			strcpy(g_pub_tx.add_ind,"1");/**增加**/
			tmp_svc_ind=g_pub_tx.svc_ind;
			/* by Wang Yongwei 撤销的时候用 20101201 */
			g_pub_tx.svc_ind=1201;
			memset(tmp_sub_tx_code,0,sizeof(tmp_sub_tx_code));
			memcpy(tmp_sub_tx_code,g_pub_tx.sub_tx_code,4);
			memcpy(g_pub_tx.sub_tx_code,"D312",4);

			memset(g_pub_tx.note_type,0x00,sizeof(g_pub_tx.note_type));
			memset(g_pub_tx.beg_note_no,0x00,sizeof(g_pub_tx.beg_note_no));
			ret=pub_base_sub_ac_trance(sub_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"调用存取款主控失败!");
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("子账户处理结束[%s][%d],恢复原变量\n",__FILE__,__LINE__);
			memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
			memcpy(g_pub_tx.ac_no,sSub_dd_mst.ac_no,sizeof(sSub_dd_mst.ac_no)-1);/* 为主账户记账准备数据 */
			g_pub_tx.ac_id=0;
			memcpy(g_pub_tx.note_type,cNote_type_bk,sizeof(cNote_type_bk));
			memcpy(g_pub_tx.beg_note_no,cBeg_note_no_bk,sizeof(cBeg_note_no_bk)-1);
			g_pub_tx.svc_ind=tmp_svc_ind;
			memcpy(g_pub_tx.sub_tx_code,tmp_sub_tx_code,4);
		}
	}
	
	/* 财政集中户子账户处理结束 */
		/*** 取产品代码，及帐号类别 ***/
		ret=pub_base_ac_prdt(&sPrdt_ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"取产品代码错误!");
			WRITEMSG
		    goto ErrExit;
		}
		strcpy( g_pub_tx.ac_id_type,sPrdt_ac_id.ac_id_type );
		if( sPrdt_ac_id.ac_id_type[0]=='2' )
		{
			sprintf(acErrMsg,"定期帐号不能作此交易!");
			WRITEMSG
		    strcpy(g_pub_tx.reply,"P403");
			goto ErrExit;
		}

		if( sPrdt_ac_id.ac_id_type[0]=='1' )		/*** 对公活期 ***/
		{
			strcpy(g_pub_tx.add_ind,"1"); /*** 贷方增加 ***/
			g_pub_tx.svc_ind=1001;  				/*活期存取*/
			set_zd_double("1001",g_pub_tx.tx_amt1);
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='9' )	/*** 内部帐 ***/
		{
			/* 内部帐记贷方的时候需要将add_ind置成 1 **/
			strcpy(g_pub_tx.add_ind,"1");
			g_pub_tx.svc_ind=9001;  /*内部帐存取*/
			set_zd_double("1001",g_pub_tx.tx_amt1);
		}
		else
		{
			sprintf(acErrMsg,"帐号类型错误![%s]",g_pub_tx.ac_id_type);
			WRITEMSG
		    strcpy(g_pub_tx.reply,"P410");
			goto ErrExit;
		}
		memcpy(g_pub_tx.prdt_code, sPrdt_ac_id.prdt_no, sizeof(sPrdt_ac_id.prdt_no)-1);
		strcpy(g_pub_tx.sub_tx_code,"PLKD"); 
		strcpy( g_pub_tx.hst_ind,"1" );
		vtcp_log("[%s][%d] 一贷多借 贷方记账 ac_no=%s sub_ac_seqn=%ld",__FILE__,__LINE__,g_pub_tx.ac_no,sub_ac_seqn);
		if( pub_acct_trance() )
		{
				sprintf(acErrMsg,"交易记帐处理错误!");
				WRITEMSG
				goto ErrExit;
		}
		set_zd_data("0660","2");			
		set_zd_data("0210","01");			
		set_zd_data("0670","2");			
		set_zd_double("1002",0.00);
		set_zd_double("1003",0.00);
		set_zd_double("1004",0.00);
		set_zd_double("100E",0.00);
		strcpy(g_pub_tx.sub_tx_code,"WNJZ"); /* 为了记dc_log,因为spPLKH没配dc_entry */
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
	/**借方记账开始，数据细节在文件中传到后台**/
	/* 开始处理文件内容 */
	memset(filename,0x0,sizeof(filename));
	Gl_file_first = 0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"取得的文件名为[%s]",filename);
	WRITEMSG
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf( acErrMsg," open file [%s] error ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	while (1)
	{
		/***********每次循环都重新初始化全局变量*************/
		pub_base_sysinit();
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) )
			break;
		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<6;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		/*-------说明如下
		fldstr[0] 借方账号
		fldstr[1] 借方子户序号
		fldstr[2] 借方金额
		fldstr[3] 凭证类型
		fldstr[4] 凭证号码
		fldstr[5] 摘要
		--------------------------------------------------*/
		sub_ac_seqn=0;
		pub_base_strpack(fldstr[0]);
		strcpy(g_pub_tx.ac_no,fldstr[0]);
		pub_base_old_acno(g_pub_tx.ac_no);
		
		sub_ac_seqn=atoi(fldstr[1]);
		
		if(memcmp(fldstr[3],"其他凭证",8)==0)
		{
			strcpy(g_pub_tx.note_type,"299");
		}
		else if(memcmp(fldstr[3],"转账支票",8)==0)
		{
			strcpy(g_pub_tx.note_type,"002");
		}
		else
			g_pub_tx.note_type[0]='\0';
		g_pub_tx.tx_amt1=atof( fldstr[2] );
		tot_amt+=g_pub_tx.tx_amt1;
		strcpy(g_pub_tx.brf,fldstr[5]);
		strcpy(g_pub_tx.beg_note_no,fldstr[4]);
		strcpy(g_pub_tx.end_note_no,fldstr[4]);
		strcpy(g_pub_tx.brf,fldstr[5]);
		strcpy( g_pub_tx.ct_ind,"2" );		
		strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/

		/* 遂宁财政集中户特殊处理 */
		strcpy(g_pub_tx.cur_no,"01");
		memset(&gSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
		pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
		if(g_pub_tx.ac_no[0]=='9' && strlen(g_pub_tx.ac_no)==13)/* 内部账 */
			g_pub_tx.ac_seqn=0;
		ret=pub_base_ac_prdt(&sPrdt_ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"取产品代码错误!");
			WRITEMSG
		    goto ErrExit;
		}
		if(!strcmp(g_mdm_ac_rel.draw_pwd_chg,"1") && sub_ac_seqn==0)
		{ 
			vtcp_log("%s,%d,该二级账户序号为空,请检查!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"SN11");
		    goto ErrExit;
			
		}
		if(!strcmp(g_mdm_ac_rel.draw_pwd_chg,"0") && sub_ac_seqn!=0)
		{ 
			vtcp_log("%s,%d,该二级账户序号为空,请检查!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"SN09");
		    goto ErrExit;
			
		}
		strcpy( g_pub_tx.ac_id_type,sPrdt_ac_id.ac_id_type );
		if( sPrdt_ac_id.ac_id_type[0]=='2' )
		{
			sprintf(acErrMsg,"定期帐号不能作此交易!");
			WRITEMSG
		    strcpy(g_pub_tx.reply,"P403");
			goto ErrExit;
		}
		if(!strcmp(g_mdm_ac_rel.draw_pwd_chg,"1"))
		{
			ret = Sub_dd_mst_Sel(g_pub_tx.reply,&gSub_dd_mst,"ac_no='%s' and sub_ac_seqn=%ld and ac_sts='1'",g_pub_tx.ac_no,sub_ac_seqn);
			if(ret==100){
				vtcp_log("[%s][%d] 二级户查询错误  ",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"SN08");
				goto ErrExit;
			}else if(ret){
				sprintf(acErrMsg,"查询sub_dd_mst出错!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}else{
				vtcp_log("[%s][%d]对应的主账户是[%s]\n",__FILE__,__LINE__,gSub_dd_mst.ac_no);
				memset(tmp_sub_tx_code,0,sizeof(tmp_sub_tx_code));
				memcpy(tmp_sub_tx_code,g_pub_tx.sub_tx_code,4);
				memcpy(g_pub_tx.sub_tx_code,"D311",4);

				strcpy( g_pub_tx.hst_ind,"1" );
				strcpy(g_pub_tx.add_ind,"0");/**借方减少**/
				memset(tmp_sub_tx_code,0,sizeof(tmp_sub_tx_code));
				memcpy(tmp_sub_tx_code,g_pub_tx.sub_tx_code,4);
				memcpy(g_pub_tx.sub_tx_code,"D311",4);
				tmp_svc_ind=g_pub_tx.svc_ind;
				g_pub_tx.svc_ind=1201;
				memset(g_pub_tx.note_type,0x00,sizeof(g_pub_tx.note_type));
				memset(g_pub_tx.beg_note_no,0x00,sizeof(g_pub_tx.beg_note_no));
				ret=pub_base_sub_ac_trance(sub_ac_seqn);
				if( ret )
				{
					sprintf(acErrMsg,"调用存取款主控失败!");
					WRITEMSG
					goto ErrExit;
				}
				memcpy(g_pub_tx.sub_tx_code,tmp_sub_tx_code,4);
				vtcp_log("子账户处理结束[%s][%d],恢复原变量\n",__FILE__,__LINE__);
				memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
				memcpy(g_pub_tx.ac_no,gSub_dd_mst.ac_no,sizeof(gSub_dd_mst.ac_no)-1);/* 为主账户记账准备数据 */
				g_pub_tx.ac_id=0;
				memcpy(g_pub_tx.note_type,cNote_type_bk,sizeof(cNote_type_bk));
				memcpy(g_pub_tx.beg_note_no,cBeg_note_no_bk,sizeof(cBeg_note_no_bk)-1);
				memcpy(g_pub_tx.sub_tx_code,tmp_sub_tx_code,4);
				g_pub_tx.svc_ind=tmp_svc_ind;

			}
		}
		/* 财政集中支付户子账户处理完毕 */

		strcpy(g_pub_tx.cur_no,"01");
		/*** 取产品代码，及帐号类别 ***/

		if( sPrdt_ac_id.ac_id_type[0]=='1' )		/*** 对公活期 ***/
		{
			strcpy(g_pub_tx.add_ind,"0"); /*** 借方减少 ***/
			g_pub_tx.svc_ind=1001;  				/*活期存取*/
			set_zd_double("1001",g_pub_tx.tx_amt1);
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='9' )	/*** 内部帐 ***/
		{
			strcpy(g_pub_tx.add_ind,"0");/*** 借方减少 ***/
			g_pub_tx.svc_ind=9001;  /*内部帐存取*/
			set_zd_double("1001",g_pub_tx.tx_amt1);
		}
		else
		{
			sprintf(acErrMsg,"帐号类型错误![%s]",g_pub_tx.ac_id_type);
			WRITEMSG
		    strcpy(g_pub_tx.reply,"P410");
			goto ErrExit;
		}
		memcpy(g_pub_tx.prdt_code, sPrdt_ac_id.prdt_no, sizeof(sPrdt_ac_id.prdt_no)-1);
		strcpy(g_pub_tx.sub_tx_code,"PLKD");
		strcpy( g_pub_tx.hst_ind,"1" );
		/* g_pub_tx.sub_ac_seqn=sub_ac_seqn; */
		if(memcmp(fldstr[3],"其他凭证",8)==0)
		{
			strcpy(g_pub_tx.note_type,"299");
		}
		else if(memcmp(fldstr[3],"转账支票",8)==0)
		{
			strcpy(g_pub_tx.note_type,"002");
		}
		else
			g_pub_tx.note_type[0]='\0';
		g_pub_tx.tx_amt1=atof( fldstr[2] );
		strcpy(g_pub_tx.brf,fldstr[5]);
		strcpy(g_pub_tx.beg_note_no,fldstr[4]);
		strcpy(g_pub_tx.end_note_no,fldstr[4]);
		strcpy(g_pub_tx.brf,fldstr[5]);
		vtcp_log("[%s][%d] 一贷多借 借方记账 ac_no=%s sub_ac_seqn=%ld",__FILE__,__LINE__,g_pub_tx.ac_no,sub_ac_seqn);
		if( pub_acct_trance() )
		{
				sprintf(acErrMsg,"交易记帐处理错误!");
				WRITEMSG
				goto ErrExit;
		}
		strcpy(g_pub_tx.sub_tx_code,"WNJZ"); /* 为了记dc_log,因为spPLKD没配dc_entry */
		set_zd_data("0210","01");			
		set_zd_data("0670","2");			
		set_zd_double("1002",0.00);
		set_zd_double("1003",0.00);
		set_zd_double("1004",0.00);
		set_zd_double("100E",0.00);
		set_zd_data("0660","1");	/* 文件内都按借方处理 */
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
	}
	if(pub_base_CompDblVal(tot_amt,tot_amt1))
	{
		vtcp_log("%s,%d,文件累计金额与传送金额不一致![%.2f][%.2lf]",__FILE__,__LINE__,tot_amt,tot_amt1);
		strcpy(g_pub_tx.reply,"H019");
		goto ErrExit;
	}

	fclose(fp);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"一贷多借记帐成功![%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
       if(fp != NULL)    /*add by chengbo 20100903 SN*/
       {
            fclose(fp);
            fp = NULL;
       }
	if(strlen(g_pub_tx.reply)==0 || memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		strcpy(g_pub_tx.reply,"H043");
	}
	sprintf(acErrMsg,"一贷多借记帐失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

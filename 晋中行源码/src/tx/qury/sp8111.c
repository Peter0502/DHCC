/*************************************************
* 文 件 名:  sp8111.c
* 功能描述： 划款协议登记簿查询
*
* 作    者:  
* 完成日期： 
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"

int sp8111()
{
	struct mo_prdt_prot_c vmo_prdt_prot;
	struct mo_prot_ac_rel_c vmo_prot_ac_rel;
	struct mo_prdt_prot_c f_mo_prdt_prot;
	char ac_no2[25],app_ac_no[25];
	char f_sts13[41];
	char f_acno2[25];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		char str_tmpa[9];
		char str_tmpb[9];
		char str_tmpc[9];
		char str_tmpd[9];	
		char prot_type[20];
							
		int ttlnum=0;			/**读取的总条数**/
		int i=0,Tmp_num;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0910",f_mo_prdt_prot.tx_br_no );
	get_zd_data("0240",f_mo_prdt_prot.prot_type );
	get_zd_data("0380",f_acno2);
    pub_base_old_acno(f_acno2);
	get_zd_long("0510",&f_mo_prdt_prot.main_ac_seqn );
	get_zd_data("0720",f_mo_prdt_prot.sts );
	get_zd_double("1001",&f_mo_prdt_prot.leave_amt );
	get_zd_double("1002",&f_mo_prdt_prot.turn_amt );
	get_zd_long("0290",&f_mo_prdt_prot.tx_date );
	vtcp_log("[%s][%d]签约状态是[%s]\n",__FILE__,__LINE__,f_mo_prdt_prot.sts);
	/**组成查询条件**/
	pub_base_strpack( f_mo_prdt_prot.tx_br_no );
	if( strlen(f_mo_prdt_prot.tx_br_no) )
	{
		sprintf( tmpstr," tx_br_no='%s' and",f_mo_prdt_prot.tx_br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_prdt_prot.prot_type );
	if( strlen(f_mo_prdt_prot.prot_type) )
	{
		sprintf( tmpstr," prot_type='%s' and",f_mo_prdt_prot.prot_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_acno2 );
	if( strlen(f_acno2) )
	{
		if ( pub_base_acno_acid(&f_mo_prdt_prot.main_ac_id,f_acno2) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," main_ac_id=%ld and",f_mo_prdt_prot.main_ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_prdt_prot.main_ac_seqn )
	{
		sprintf( tmpstr," main_ac_seqn=%d and",f_mo_prdt_prot.main_ac_seqn );
		strcat( wherelist,tmpstr );
	}
	/**  加一个签约状态的条件  add by wudawei 20130715**/
	if( f_mo_prdt_prot.sts )
	{
		sprintf( tmpstr," sts='%c' and",f_mo_prdt_prot.sts[0] );
		strcat( wherelist,tmpstr );
	}
	/**  add end 20130715**/
	if( f_mo_prdt_prot.leave_amt )
	{
		sprintf( tmpstr," leave_amt=%lf and",f_mo_prdt_prot.leave_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_prdt_prot.turn_amt )
	{
		sprintf( tmpstr," turn_amt=%lf and",f_mo_prdt_prot.turn_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_prdt_prot.tx_date )
	{
		sprintf( tmpstr," tx_date=%ld and",f_mo_prdt_prot.tx_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_prdt_prot" );

	/**组成查询**/
	strcat(wherelist," prot_type!='Z02' ORDER BY tx_date,trace_no") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"tx_br_no,prot_type,main_ac_id,main_ac_seqn,leave_amt,turn_amt,m3,m6,y1,y2,tx_date,trace_no,tel,sts");

	ret=Mo_prdt_prot_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		memset(&vmo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
		ret=Mo_prdt_prot_Fet_Sel( &vmo_prdt_prot, g_pub_tx.reply );
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
			fprintf( fp,"~@协议类型|主账号|序号|协议帐号|序号|$留存金额|$化转金额|3月定期|6月定期|1年定期|2年定期|@状态|登记日期|@登记机构|@柜员|流水号|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vmo_prdt_prot.main_ac_id,vmo_prdt_prot.main_ac_seqn,ac_no2 );
		if(vmo_prdt_prot.leave_amt<-2147000000) vmo_prdt_prot.leave_amt=0.0;
		if(vmo_prdt_prot.turn_amt<-2147000000) vmo_prdt_prot.turn_amt=0.0;
		if(vmo_prdt_prot.tx_date<-2147000000) vmo_prdt_prot.tx_date=0;
		if(vmo_prdt_prot.trace_no<-2147000000) vmo_prdt_prot.trace_no=0;
		pub_base_dic_show_str(f_sts13,"cancel_ind",vmo_prdt_prot.sts);
		sprintf( str_tmpa , "%6d％" , vmo_prdt_prot.m3 );
		sprintf( str_tmpb , "%6d％" , vmo_prdt_prot.m6 );
		sprintf( str_tmpc , "%6d％" , vmo_prdt_prot.y1 );
		sprintf( str_tmpd , "%6d％" , vmo_prdt_prot.y2 );				

		if( !strcmp(vmo_prdt_prot.prot_type,"TCA") )
			strcpy(prot_type,"理财套餐A");
		else if( !strcmp(vmo_prdt_prot.prot_type,"TCB") )
			strcpy(prot_type,"理财套餐B");
		else if( !strcmp(vmo_prdt_prot.prot_type,"TCC") )
			strcpy(prot_type,"理财套餐C");
		else if( !strcmp(vmo_prdt_prot.prot_type,"TCD") )
			strcpy(prot_type,"理财套餐D");
		else if( !strcmp(vmo_prdt_prot.prot_type,"TCE") )
			strcpy(prot_type,"理财套餐E");
		else if( !strcmp(vmo_prdt_prot.prot_type,"XD1") )
			strcpy(prot_type,"协定存款");
		else if( !strcmp(vmo_prdt_prot.prot_type,"YY1") )
			strcpy(prot_type,"活期转入零存整取");
		else if( !strcmp(vmo_prdt_prot.prot_type,"YY2") )
			strcpy(prot_type,"整存整取转为整存整取");
		else if( !strcmp(vmo_prdt_prot.prot_type,"YY3") )
			strcpy(prot_type,"零存整取转为整存整取");
		else if( !strcmp(vmo_prdt_prot.prot_type,"YY4") )
			strcpy(prot_type,"定期到期本金转定期");
		else if( !strcmp(vmo_prdt_prot.prot_type,"YY5") )
			strcpy(prot_type,"存本取消利息转零整");
		else if( !strcmp(vmo_prdt_prot.prot_type,"YY6") )
			strcpy(prot_type,"存本取息利息入活期");
		else if( !strcmp(vmo_prdt_prot.prot_type,"TK7") )
			strcpy(prot_type,"七天通知存款");
		/*** xxx 取协定理财帐号 ***/
		vtcp_log("prot_type=[%s]",prot_type);
		ret=Mo_prot_ac_rel_Dec_Sel( g_pub_tx.reply,"prot_type='%s' and main_ac_id=%ld and sts='%s'",vmo_prdt_prot.prot_type,vmo_prdt_prot.main_ac_id,f_mo_prdt_prot.sts);
		ERR_DEAL
		Tmp_num=0;
		while(1)
		{
			memset(&vmo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
			ret=Mo_prot_ac_rel_Fet_Sel( &vmo_prot_ac_rel, g_pub_tx.reply );
			if( ret==100 && Tmp_num==0 ) 
			{
				strcpy(app_ac_no,"");
				vmo_prot_ac_rel.app_ac_seqn=0;
				vtcp_log("a---prot_type=[%s]",prot_type);
				fprintf(fp,"%s|%s|%d|%s|%d|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%s|%ld|\n",prot_type,ac_no2,vmo_prdt_prot.main_ac_seqn,app_ac_no,vmo_prot_ac_rel.app_ac_seqn,vmo_prdt_prot.leave_amt,vmo_prdt_prot.turn_amt,str_tmpa,str_tmpb,str_tmpc,str_tmpd,f_sts13,vmo_prdt_prot.tx_date,vmo_prdt_prot.tx_br_no,vmo_prdt_prot.tel,vmo_prdt_prot.trace_no);
				break;
			}
			if( ret==100 )	break;
			ERR_DEAL
			pub_base_acid_acno( vmo_prot_ac_rel.app_ac_id,vmo_prot_ac_rel.app_ac_seqn,app_ac_no );

			vtcp_log("b---prot_type=[%s]",prot_type);
			fprintf(fp,"%s|%s|%d|%s|%d|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%s|%ld|\n",prot_type,ac_no2,vmo_prdt_prot.main_ac_seqn,app_ac_no,vmo_prot_ac_rel.app_ac_seqn,vmo_prdt_prot.leave_amt,vmo_prdt_prot.turn_amt,str_tmpa,str_tmpb,str_tmpc,str_tmpd,f_sts13,vmo_prdt_prot.tx_date,vmo_prdt_prot.tx_br_no,vmo_prdt_prot.tel,vmo_prdt_prot.trace_no);
			ttlnum++;
			Tmp_num++;
		}
	}

	ret=Mo_prdt_prot_Clo_Sel( );
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

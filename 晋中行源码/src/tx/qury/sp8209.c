/*************************************************
* 文 件 名:  sp8209.c
* 功能描述： 抵押品信息查询
*
* 作    者:  gongliangliang
* 完成日期： 2011-09-10
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
#include "ln_gage_reg_c.h"
#include "mo_po_co_c.h"
#include "pact_gaga_rel_c.h"

int sp8209()
{
	int	iFlag=0;
	struct ln_gage_reg_c vln_gage_reg,sln_gage_reg;
	struct ln_gage_reg_c f_ln_gage_reg;
	struct ln_gage_reg_c f_ln_gage_rega;
	struct pact_gaga_rel_c sPact_gaga_rel;
	struct mo_po_co_c sMo_po_co;
	char ac_no[25];
	char ac_no1[25];
	char f_in_out[5];
	char cPact_no[21]; /** add by chenchao **/
	char gage_type_name[21];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[512];
	char tmpmo[6];
	char fieldlist[1024];  /**要查询的字段列表**/
	char tablelist[128];   /**要查询的表名**/
	char br_no[6];
	char cName[50];
	int ttlnum=0;			/**读取的总条数**/
	int i=0;
	int ret=0;
	FILE *fp;
	char flag[2];
	char gage_type[3];
	long in_beg_date=0,out_beg_date=0;
	long in_end_date=0,out_end_date=0;
	double Tamt=0.00;/*总计抵制押金额*/
	memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
	memset(ac_no,0x00,sizeof(ac_no));
	memset(ac_no1,0x00,sizeof(ac_no1));
	memset(f_in_out,0x00,sizeof(f_in_out));
	memset(gage_type_name,0x00,sizeof(gage_type_name));
	memset(br_no,0x00,sizeof(br_no));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(filename,0x00,sizeof(filename));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( &vln_gage_reg,0,sizeof(vln_gage_reg));
	memset( &f_ln_gage_reg,0,sizeof(f_ln_gage_reg));
	memset( &f_ln_gage_rega,0,sizeof(f_ln_gage_rega));
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset(tmpmo,0x00,sizeof(tmpmo));
	memset(flag,0x00,sizeof(flag));
	memset(gage_type,0,sizeof(gage_type));
	memset(cName,0x00,sizeof(cName));
	
	pub_base_sysinit();
	
	get_zd_data("0860",br_no );
	vtcp_log("%s,%d 机构号=[%s]",__FILE__,__LINE__,br_no);
	get_zd_data("0630",f_ln_gage_reg.pact_no); 
	get_zd_double("0400",&f_ln_gage_reg.amt );
	get_zd_double("0410",&f_ln_gage_rega.amt );
	get_zd_long("0440",&in_beg_date );
	get_zd_long("0450",&in_end_date );
	get_zd_data("0220",gage_type);
	get_zd_long("0460",&out_beg_date );
	get_zd_long("0470",&out_end_date );	
	get_zd_data("0670",flag);
	get_zd_data("0250",cName);
	
	
	if(flag[0]=='1')
	{
		/**组成查询条件**/
		strcpy( tmpstr," sts='0' and" );
		strcat( wherelist,tmpstr );
	}
	if(flag[0]=='2')
  {
		/**组成查询条件**/
		strcpy( tmpstr," sts = '1' and" );
		strcat( wherelist,tmpstr );
	}
	if(flag[0]=='3')
  {
		/**组成查询条件**/
		strcpy( tmpstr," sts = 'X' and" );
		strcat( wherelist,tmpstr );
	}
	if(flag[0]=='4')
  {
		/**组成查询条件**/
		strcpy( tmpstr," sts = 'Y' and" );
		strcat( wherelist,tmpstr );
	}
	
		pub_base_strpack( f_ln_gage_reg.pact_no );
		if( strlen(f_ln_gage_reg.pact_no) )
		{
			sprintf( tmpstr," pact_no='%s' and",f_ln_gage_reg.pact_no );
			strcat( wherelist,tmpstr );
		}
  	
		if( f_ln_gage_reg.amt )
		{
			sprintf( tmpstr," amt>=%lf and",f_ln_gage_reg.amt );
			strcat( wherelist,tmpstr );
		}
		if( f_ln_gage_rega.amt )
		{
			sprintf( tmpstr," amt<=%lf and",f_ln_gage_rega.amt );
			strcat( wherelist,tmpstr );
		}
  	
		if( in_beg_date )
		{
			sprintf( tmpstr," in_tx_date>=%ld and",in_beg_date );
			strcat( wherelist,tmpstr );
		}
  	
		if( in_end_date )
		{
			sprintf( tmpstr," in_tx_date<=%ld and",in_end_date );
			strcat( wherelist,tmpstr );
		}
  	
		if( out_beg_date )
		{
			sprintf( tmpstr," out_tx_date>=%ld and",out_beg_date );
			strcat( wherelist,tmpstr );
		}
  	
		if( out_end_date )
		{
			sprintf( tmpstr," out_tx_date<=%ld and",out_end_date );
			strcat( wherelist,tmpstr );
		}
		
		if( strlen(gage_type) )
		{
			sprintf( tmpstr," gage_type='%s' and",gage_type);
			strcat( wherelist,tmpstr );
		}
		if(strlen(cName))   /*add by lwb 20150425*/
		{
			sprintf( tmpstr," name like '%%%%%s%%%%' and",cName);
                        strcat( wherelist,tmpstr );
		}
		strcpy( tablelist,"ln_gage_reg" );
  	
		/**组成查询**/
		strcat(wherelist," 1=1 order by pact_no") ;
  	
		vtcp_log( "WHERE [%s]",wherelist );
		strcpy( fieldlist,"pact_no,ac_id,amt,sts,trace_no");
  	
		ret=Ln_gage_reg_Dec_Sel( g_pub_tx.reply,wherelist );
		ERR_DEAL
  	
		ttlnum=0;
		while(1)
		{
			memset( &vln_gage_reg,0,sizeof(vln_gage_reg));
			ret=Ln_gage_reg_Fet_Sel( &vln_gage_reg, g_pub_tx.reply );
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
				fprintf( fp,"~@合同号/协议编号|抵质押品编号|贷 款 账 号|贷 款 户 名|$抵押品金额|@抵押品类型|@当前状态|入库日期|入库流水|出库日期|出库流水|@备注信息|\n" );
			}
			if(memcmp(sln_gage_reg.pact_no,vln_gage_reg.pact_no,sizeof(sln_gage_reg.pact_no))==0)
			{
				continue;
			}
  	
			/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */
			TRACE
			if(vln_gage_reg.ac_id>0)
			{
				pub_base_acid_acno( vln_gage_reg.ac_id,vln_gage_reg.ac_seqn,ac_no1 );
			}
			pub_base_dic_show_str(f_in_out,"ln_in_out_sts",vln_gage_reg.sts);
			if(vln_gage_reg.amt<-2147000000) vln_gage_reg.amt=0.0;
			if(vln_gage_reg.in_trace_no<-2147000000) vln_gage_reg.in_trace_no=0;
			if(vln_gage_reg.in_tx_date<-2147000000) vln_gage_reg.in_tx_date=0;
			if(vln_gage_reg.out_trace_no<-2147000000) vln_gage_reg.out_trace_no=0;
			if(vln_gage_reg.out_tx_date<-2147000000) vln_gage_reg.out_tx_date=0;
			pub_base_dic_show_str(gage_type_name,"gage_type",vln_gage_reg.gage_type);
			
			if(vln_gage_reg.gage_type[0]=='1')
			{
				memset(gage_type_name,0x00,sizeof(gage_type_name));
				strcpy(gage_type_name,"抵押品");
			}else if(vln_gage_reg.gage_type[0]=='2'){ 
				memset(gage_type_name,0x00,sizeof(gage_type_name));
				strcpy(gage_type_name,"质押品");
			}
				
			
			if(vln_gage_reg.pact_no[0]>='0'&&vln_gage_reg.pact_no[0]<='9')
			{
				if( strlen(br_no))
				{
					ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=%d",vln_gage_reg.ac_id,vln_gage_reg.ac_seqn);
					if(ret)	continue;
					if( strcmp( br_no,g_ln_mst.opn_br_no) )
					{
						vtcp_log("[%s][%d] 程序到这了！！！\n",__FILE__,__LINE__);
						continue;
					}
				}
			}else{
				if(strlen(vln_gage_reg.pact_no))
				{
					memcpy(tmpmo,vln_gage_reg.pact_no+1,5);
					vtcp_log("[%s][%d] tmpmo===[%s]\n",__FILE__,__LINE__,tmpmo);
					if( strcmp( br_no,tmpmo) )
					{
						continue;
					}
				}
			}
			ret = Pact_gaga_rel_Dec_Sel(g_pub_tx.reply," pact_no='%s' ",vln_gage_reg.pact_no);
			if(ret)
			{
				sprintf( acErrMsg, "贷款产品参数表出错!pact_no[%s]ret=[%d]" ,vln_gage_reg.pact_no ,ret);
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				strcpy( g_pub_tx.reply, "B130" );
				goto ErrExit;
			}
			while(1){
				memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
				ret = Pact_gaga_rel_Fet_Sel(&sPact_gaga_rel,g_pub_tx.reply);
				if(ret == 100)
				{
					if(iFlag==0)
					{
						sprintf( acErrMsg, "没有此合同号相关信息!pact_no[%s]ret=[%d]" ,vln_gage_reg.pact_no ,ret);
						set_zd_data(DC_GET_MSG,acErrMsg);
						WRITEMSG
						strcpy( g_pub_tx.reply, "B130" );
						goto ErrExit;
					}
					break;
				}
				if(ret)
				{
					sprintf( acErrMsg, "贷款产品参数表出错!pact_no[%s]ret=[%d]" ,vln_gage_reg.pact_no ,ret);
					set_zd_data(DC_GET_MSG,acErrMsg);
					WRITEMSG
					strcpy( g_pub_tx.reply, "B130" );
					goto ErrExit;
				}
				if(flag[0]=='3')
				{
					memset(f_in_out,0x00,sizeof(f_in_out));
					strcpy(f_in_out,"待入库");
				}else if(flag[0]=='4'){
					memset(f_in_out,0x00,sizeof(f_in_out));
					strcpy(f_in_out,"待出库");
				}
			
				fprintf( fp, "%s|%s|%s|%s|%.2lf|%s|%s|%ld|%ld|%ld|%ld|%s|\n",vln_gage_reg.pact_no,sPact_gaga_rel.gaga_id,ac_no1,vln_gage_reg.name,sPact_gaga_rel.gaga_bal,gage_type_name,f_in_out,vln_gage_reg.in_tx_date,vln_gage_reg.in_trace_no,vln_gage_reg.out_tx_date,vln_gage_reg.out_trace_no,sPact_gaga_rel.name);
				iFlag++;
			}
			Pact_gaga_rel_Clo_Sel();
			ttlnum++;
			Tamt += vln_gage_reg.amt ;
			TRACE
			memset(&sln_gage_reg,0x00,sizeof(sln_gage_reg));
			memcpy(&sln_gage_reg,&vln_gage_reg,sizeof(vln_gage_reg));
		}
		vtcp_log( "Tamt [%.2lf]",Tamt );
		ret=Ln_gage_reg_Clo_Sel( );
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

/*************************************************
* 文 件 名:  sp8603.c
* 功能描述： 债权债务查询
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "mo_jl_bal_c.h"
#include "com_branch_c.h"

	long beg_date=0, end_date=0;
	double beg_amt=0.00, end_amt=0.00;
	char nr[3];
	char zh[33];
	int zhxh;
	int ttlnum=0;			/**读取的总条数**/
	FILE *fp;

int sp8603()
{
	char filename[100];
	int ret=0;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );

	pub_base_sysinit();

	get_zd_data("0670",nr );
	vtcp_log("=======NRNR[%s]",nr);
	get_zd_data("0300",zh );
	get_zd_int("0340",&zhxh );
	get_zd_long("0440",&beg_date );
	get_zd_long("0450",&end_date );
	get_zd_double("0400",&beg_amt );
	get_zd_double("0410",&end_amt );
	get_zd_data("0910",g_pub_tx.tx_br_no);
	if( end_amt==0.00 ) end_amt=999999999.00;

	ttlnum = 0;

	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"w" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	if( nr[0]=='0' ) /**单户明细**/
	{
		ret=one_ac();
		ERR_DEAL
	}
	else  /**批量**/
	{
		ret=more_ac();
		ERR_DEAL
	}

	fclose(fp);

	if( ttlnum )
		set_zd_data( DC_FILE_SND,"1" );
	else
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
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
int one_ac()
{
	int ret=0;
	struct mo_jl_bal_c mo_jl_bal_c;
	long ksrq,zzrq,rq;
	long vvrq;
	double js;
	int first=0;

	ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s' ",
		zh );
	if( ret )
	{
		strcpy( g_pub_tx.reply,"M003" );
		goto ErrExit;
	}
TRACE
	ret=Mo_jl_bal_Dec_Sel( g_pub_tx.reply,
		"ac_id=%d and ac_seqn=%d and (beg_date<=%d and end_date>=%d) order by ac_id,ac_seqn,beg_date",
		g_mdm_ac_rel.ac_id,zhxh,end_date,beg_date );
	ERR_DEAL

	js=0;
TRACE
	while(1)
	{
		ret=Mo_jl_bal_Fet_Sel( &mo_jl_bal_c,g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

TRACE
		if( mo_jl_bal_c.beg_date<beg_date ) ksrq=beg_date;
		else ksrq=mo_jl_bal_c.beg_date;

		if( mo_jl_bal_c.end_date>end_date ) zzrq=end_date;
		else zzrq=mo_jl_bal_c.end_date;

TRACE
		while( 1 )
		{
			js+=mo_jl_bal_c.bal;

			if( first==0 )
			{
				fprintf( fp,"~@日期|$余额|$累计数|\n" );
				vvrq=ksrq;
				first=1;
			}
			fprintf( fp,"%d|%.2lf|%.2lf|\n",ksrq,mo_jl_bal_c.bal,js );
			ttlnum++;

TRACE
			pub_base_deadlineD( ksrq,1,&rq );
			ksrq=rq;
			if( ksrq>zzrq ) break;
		}
	}
	Mo_jl_bal_Clo_Sel();

	if( ttlnum )
	{
		fprintf( fp,"`%d-%d平均余额  %.2lf \n",vvrq,zzrq,js/ttlnum );
	}
	return 0;
ErrExit:
	return 1;
}
int more_ac()
{
	int ret=0;
	struct mo_jl_bal_c mo_jl_bal_c;
	struct mo_jl_bal_c omo_jl_bal_c;
	struct com_branch_c com_branch_c;
	long ksrq,zzrq,rq;
	long vvrq;
	double js;
	int first=0;
	int i=0;
	int ts=0;
	long oac_id=0;
	int oac_seqn=0;
	char jgtj[500];
	double hjs,hjbal;
	
		hjs=0.00;
		hjbal=0.00;
	ts=pub_base_CalTrueDays( beg_date,end_date )+1;
	if( ts<=0 ) ts=1;

	ret=Com_branch_Sel( g_pub_tx.reply,&com_branch_c,"br_no='%s'",
		g_pub_tx.tx_br_no );
	ERR_DEAL

	if( nr[0]=='2' )
	{
vtcp_log("^对公%d-%d平均余额大于%.2lf帐户\n",
			beg_date,end_date,beg_amt );
		fprintf(fp,
			"^对公%d-%d平均余额大于%.2lf帐户\n",
			beg_date,end_date,beg_amt );
		fprintf(fp,
			"~@机构|@帐号|@产品|$余额|@开户日期|$日均余额|@户名|\n");
	}
	else
	{
		fprintf(fp,
			"^储蓄%d-%d平均余额大于%.2lf帐户\n",
			beg_date,end_date,beg_amt );
		fprintf(fp,
			"~@机构|@帐号|@产品|$余额|@开户日期|$日均余额|@户名|\n");
	}

	if( strlen(zh) )
	{
		ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s' ", zh );
		if( ret )
		{
			strcpy( g_pub_tx.reply,"M003" );
			goto ErrExit;
		}
TRACE
		ret=Mo_jl_bal_Dec_Sel( g_pub_tx.reply,
		"ac_id=%d and ac_seqn=%d and (beg_date<=%d and end_date>=%d) order by ac_id,ac_seqn,beg_date",
			g_mdm_ac_rel.ac_id,zhxh,end_date,beg_date );
		ERR_DEAL
	}
	else 
	{
		if( com_branch_c.br_type[0]=='7' )
		{
			sprintf(jgtj,
				"opn_br_no in(select br_no from com_branch where up_br_no='%s')",
				g_pub_tx.tx_br_no );
		}
		else
		{
			sprintf(jgtj, "opn_br_no='%s'", g_pub_tx.tx_br_no );
		}

		if( nr[0]=='1' )
		{
			ret=Mo_jl_bal_Dec_Sel( g_pub_tx.reply,
			"beg_date<=%d and end_date>=%d and (ac_id in(select ac_id from dd_mst where %s and prdt_no in('101','102') ) or ac_id in(select ac_id from td_mst where %s and prdt_no in(select prdt_no from td_parm where cif_type='1') ) ) order by ac_id,ac_seqn,beg_date",
			end_date,beg_date,jgtj,jgtj );
		}
		else if( nr[0]=='2' )
		{
			ret=Mo_jl_bal_Dec_Sel( g_pub_tx.reply,
			"beg_date<=%d and end_date>=%d and (ac_id in(select ac_id from dd_mst where %s and prdt_no not in('101','102') ) or ac_id in(select ac_id from td_mst where %s and prdt_no in(select prdt_no from td_parm where cif_type!='1') ) ) order by ac_id,ac_seqn,beg_date",
			end_date,beg_date,jgtj,jgtj );
		}
		else
		{
			ret=Mo_jl_bal_Dec_Sel( g_pub_tx.reply,
			"beg_date<=%d and end_date>=%d and (ac_id in(select ac_id from dd_mst where %s and prdt_no in('101','102') ) or ac_id in(select ac_id from td_mst where %s and prdt_no in(select prdt_no from td_parm where cif_type='1') ) ) order by ac_id,ac_seqn,beg_date",
			end_date,beg_date,jgtj,jgtj );
		}
		ERR_DEAL
	}
	js=0;
	omo_jl_bal_c.ac_id=0;
	omo_jl_bal_c.ac_seqn=0;
	while(1)
	{
		ret=Mo_jl_bal_Fet_Sel( &mo_jl_bal_c,g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( mo_jl_bal_c.ac_id!=omo_jl_bal_c.ac_id || 
			mo_jl_bal_c.ac_seqn!=omo_jl_bal_c.ac_seqn )
		{
			if( omo_jl_bal_c.ac_id!=0 && js>=beg_amt*ts && js<=end_amt*ts )
			{
				ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
					"ac_id=%d and ac_seqn=%d order by note_sts",
					omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
				if( ret )
				{
					sprintf(acErrMsg,"ERRORac_id=%d and ac_seqn=%d",
						omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
					WRITEMSG
					goto NOFH;
				}
				if( omo_jl_bal_c.ac_seqn==1 )
				{
					ret=Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,
						"ac_id=%d and ac_seqn=%d",
						omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
					if( ret )
					{
						sprintf(acErrMsg,"ERRORac_id=%d and ac_seqn=%d",
							omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
						WRITEMSG
						goto NOFH;
					}
					ret=Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm,
						"prdt_no='%s'",g_dd_mst.prdt_no );
					if( ret ) strcpy( g_dd_parm.title,g_dd_mst.prdt_no );

					rq=g_dd_mst.opn_date;
					strcpy( g_mdm_ac_rel.opn_br_no,g_dd_mst.opn_br_no );
				}
				else
				{
					ret=Td_mst_Sel( g_pub_tx.reply,&g_td_mst,
						"ac_id=%d and ac_seqn=%d",
						omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
					if( ret )
					{
						sprintf(acErrMsg,"ERRORac_id=%d and ac_seqn=%d",
							omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
						WRITEMSG
						goto NOFH;
					}
					ret=Td_parm_Sel( g_pub_tx.reply,&g_td_parm,
						"prdt_no='%s'",g_td_mst.prdt_no );
					if( ret ) strcpy( g_dd_parm.title,g_td_mst.prdt_no );
					else strcpy( g_dd_parm.title,g_td_parm.title );

					rq=g_td_mst.opn_date;
					strcpy( g_mdm_ac_rel.opn_br_no,g_td_mst.opn_br_no );
					strcpy( g_dd_mst.name,g_td_mst.name );
				}
				if( nr[0]=='2' )
					fprintf( fp,"%s|%s|%s|%.2lf|%d|%.2lf|%s  |\n",
						g_mdm_ac_rel.opn_br_no,g_mdm_ac_rel.ac_no,
						g_dd_parm.title,omo_jl_bal_c.bal,
						rq,js/ts,g_dd_mst.name );
				else
					fprintf( fp,"%s|%s %d|%s|%.2lf|%d|%.2lf|%s  |\n",
						g_mdm_ac_rel.opn_br_no,g_mdm_ac_rel.ac_no,
						omo_jl_bal_c.ac_seqn,
						g_dd_parm.title,omo_jl_bal_c.bal,
						rq,js/ts,g_dd_mst.name );
				hjbal+=omo_jl_bal_c.bal;
				hjs+=js/ts;
				ttlnum++;
NOFH:			
				;
			}
			else
			{
				vtcp_log("%d [%.2lf][%.2lf] js [%.2lf]",omo_jl_bal_c.ac_id,
					beg_amt*ts,end_amt*ts,js );
			}
			js=0;
		}
			if( mo_jl_bal_c.beg_date<beg_date ) ksrq=beg_date;
			else ksrq=mo_jl_bal_c.beg_date;

			if( mo_jl_bal_c.end_date>end_date ) zzrq=end_date;
			else zzrq=mo_jl_bal_c.end_date;

			while( 1 )
			{
				js+=mo_jl_bal_c.bal;
				pub_base_deadlineD( ksrq,1,&rq );
				ksrq=rq;
				if( ksrq>zzrq ) break;
			}
		memcpy( &omo_jl_bal_c,&mo_jl_bal_c,sizeof(mo_jl_bal_c) );
	}
	Mo_jl_bal_Clo_Sel();
		if( mo_jl_bal_c.ac_id!=omo_jl_bal_c.ac_id || 
			mo_jl_bal_c.ac_seqn!=omo_jl_bal_c.ac_seqn )
		{
			if( omo_jl_bal_c.ac_id!=0 && js>=beg_amt*ts && js<=end_amt*ts )
			{
				ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
					"ac_id=%d and ac_seqn=%d order by note_sts",
					omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
				if( ret )
				{
					sprintf(acErrMsg,"ERRORac_id=%d and ac_seqn=%d",
						omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
					WRITEMSG
					goto NOFH_L;
				}
				if( omo_jl_bal_c.ac_seqn==1 )
				{
					ret=Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,
						"ac_id=%d and ac_seqn=%d",
						omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
					if( ret )
					{
						sprintf(acErrMsg,"ERRORac_id=%d and ac_seqn=%d",
							omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
						WRITEMSG
						goto NOFH_L;
					}
					ret=Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm,
						"prdt_no='%s'",g_dd_mst.prdt_no );
					if( ret ) strcpy( g_dd_parm.title,g_dd_mst.prdt_no );

					rq=g_dd_mst.opn_date;
					strcpy( g_mdm_ac_rel.opn_br_no,g_dd_mst.opn_br_no );
				}
				else
				{
					ret=Td_mst_Sel( g_pub_tx.reply,&g_td_mst,
						"ac_id=%d and ac_seqn=%d",
						omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
					if( ret )
					{
						sprintf(acErrMsg,"ERRORac_id=%d and ac_seqn=%d",
							omo_jl_bal_c.ac_id,omo_jl_bal_c.ac_seqn );
						WRITEMSG
						goto NOFH_L;
					}
					ret=Td_parm_Sel( g_pub_tx.reply,&g_td_parm,
						"prdt_no='%s'",g_td_mst.prdt_no );
					if( ret ) strcpy( g_dd_parm.title,g_td_mst.prdt_no );
					else strcpy( g_dd_parm.title,g_td_parm.title );

					rq=g_td_mst.opn_date;
					strcpy( g_mdm_ac_rel.opn_br_no,g_td_mst.opn_br_no );
					strcpy( g_dd_mst.name,g_td_mst.name );
				}
				if( nr[0]=='2' )
					fprintf( fp,"%s|%s|%s|%.2lf|%d|%.2lf|%s  |\n",
						g_mdm_ac_rel.opn_br_no,g_mdm_ac_rel.ac_no,
						g_dd_parm.title,omo_jl_bal_c.bal,
						rq,js/ts,g_dd_mst.name );
				else
					fprintf( fp,"%s|%s %d|%s|%.2lf|%d|%.2lf|%s  |\n",
						g_mdm_ac_rel.opn_br_no,g_mdm_ac_rel.ac_no,
						omo_jl_bal_c.ac_seqn,
						g_dd_parm.title,omo_jl_bal_c.bal,
						rq,js/ts,g_dd_mst.name );
				hjbal+=omo_jl_bal_c.bal;
				hjs+=js/ts;
				ttlnum++;
NOFH_L:			
				;
			}
			else
			{
				vtcp_log("%d [%.2lf][%.2lf] js [%.2lf]",omo_jl_bal_c.ac_id,
					beg_amt*ts,end_amt*ts,js );
			}
			js=0;
		}

				fprintf( fp,"%s|%s|%s|%.2lf|%d|%.2lf|%s  |\n",
					"","",
					"合     计",hjbal,
					0,hjs," " );

	return 0;
ErrExit:
	return 1;
}

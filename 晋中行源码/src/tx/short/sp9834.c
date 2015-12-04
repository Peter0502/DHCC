/*************************************************
* 文 件 名:  sp9834.c
* 功能描述： 由账号下传放款计划
*
* 作    者:  jack
* 完成日期： 2003年1月17日 
*
* 修改记录:
* 日   期 :
* 修 改 人:
* 修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include <string.h>
#define EXTERN
#include "public.h"
#include "ln_pay_pln_c.h"

int sp9834()
{
	struct ln_pay_pln_c ln_pay_pln;
    char filename[100];
    int ret;
	FILE *fp;
   
    get_zd_data("0370",g_pub_tx.ac_no);
    get_zd_int("0340",&g_pub_tx.ac_seqn);
    get_zd_data("0670",g_pub_tx.tmp_ind);

    if ( g_pub_tx.tmp_ind[0] == '1')
        goto OkExit;
	
	pub_base_AllDwnFilName( filename );

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%d] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG

	ret = Ln_pay_pln_Dec_Sel( g_pub_tx.reply,
		" ac_id=%ld and ac_seqn=%d and pay_ind='2' ",
		g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if( ret )
	{
		goto ErrExit;
	}

	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	while(1)
	{
		ret = Ln_pay_pln_Fet_Sel ( &ln_pay_pln , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		vtcp_log("%d-%d-%ld-%.2lf-",
			ln_pay_pln.ac_id,ln_pay_pln.de_ac_seqn,ln_pay_pln.beg_date,
			ln_pay_pln.ttl_amt );
			
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",
			ln_pay_pln.de_ac_id );
		if( ret ) goto ErrExit;

		pub_base_strpack(g_mdm_ac_rel.ac_no);

		fprintf(fp,"%s|%d|%ld|%.2lf|\n",
			g_mdm_ac_rel.ac_no,ln_pay_pln.de_ac_seqn,ln_pay_pln.beg_date,
			ln_pay_pln.ttl_amt );
	}
	Ln_pay_pln_Clo_Sel ();

	fclose(fp);

	set_zd_data(DC_FILE_SND,"1");
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

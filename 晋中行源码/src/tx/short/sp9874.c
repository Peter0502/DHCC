/*************************************************
* �� �� ��:  sp9874.c
* ���������� ���˺��´�����ƻ�
*
* ��    ��:  lance
* ������ڣ� 2003��3��25�� 
*
* �޸ļ�¼:
* ��   �� :
* �� �� ��:
* �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include <string.h>
#define EXTERN
#include "public.h"
#include "ln_pay_pln_c.h"

int sp9874()
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
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%d] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG

	ret = Ln_pay_pln_Dec_Sel( g_pub_tx.reply,
		" ac_id=%ld and ac_seqn=%d and pay_ind='0' ",
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

		vtcp_log("%d-%d-%ld-%.2lf-%.2lf-",
			ln_pay_pln.ac_id,ln_pay_pln.de_ac_seqn,ln_pay_pln.beg_date,
			ln_pay_pln.curr_amt,ln_pay_pln.curr_intst );
			
		strcpy(g_mdm_ac_rel.ac_no,"0");
		ln_pay_pln.de_ac_seqn=0;
		fprintf(fp,"%s|%d|%ld|%.2lf|%.2lf|\n",
			g_mdm_ac_rel.ac_no,ln_pay_pln.de_ac_seqn,ln_pay_pln.beg_date,
			ln_pay_pln.curr_amt,ln_pay_pln.curr_intst );
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

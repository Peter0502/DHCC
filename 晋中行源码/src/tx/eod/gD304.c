/***************************************************************
* �� �� ��: gD304.c
* ��������: ֧�к��������ĵ����󻮵���������
*��:-31205(֧��)
*��:31205 (��������)
**************************************************************/
#define MYRETERR if(ret){ \
        sprintf(acErrMsg,"sql erro!! [%d]",ret); \
        WRITEMSG \
		goto ErrExit; \
	}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "gl_sub_c.h"
#include "com_item_c.h"
#include "com_branch_c.h"

struct com_parm_c sCom_parm_lr;
struct com_parm_c sCom_parm_lrfp;
long kslsh,num_cnt;

gD304()
{
    int ret=0,cwnum=1;
	double c_bal=0.00, d_bal=0.00;
	char dc_ind;
	struct com_branch_c com_branch_c;
	struct com_item_c com_item_c;
	char old_amt_ind;
	double d_amt=0.00;
	double c_amt=0.00;
	double bal=0.00;
	char yefx;
	struct pub_tx v_pub_tx;
	struct gl_sub_c gl_sub_c;
	FILE *fp=NULL,*fpcw=NULL;
	char filename[64],filenamecw[64];

	memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));

    ret=sql_begin(); /*������*/
	MYRETERR

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
	set_zd_data( DC_TX_CODE, "D304");
	if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
	{
		sprintf(acErrMsg,"ȡ������ˮ�Ŵ� [%d]",g_pub_tx.trace_no);
		WRITEMSG
		goto ErrExit;
	}
	kslsh=g_pub_tx.trace_no;
	num_cnt=1;

	/* �Ƚ����������ĵĴ�Ʊ,������һ������һ������������д */
    memset(filenamecw,'\0',sizeof(filenamecw));
    sprintf(filenamecw,"%s/report/59801/gD304.txt",getenv("HOME"));
    fpcw = fopen( filenamecw,"w" ); 
    if( fpcw==NULL )
    {
        sprintf(acErrMsg," open file [%s] error ",filenamecw );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
        goto ErrExit;
    }
	memset(&sCom_parm_lr, 0x0, sizeof(struct com_parm_c));
	memset(&sCom_parm_lrfp, 0x0, sizeof(struct com_parm_c));
	memset(&com_item_c, 0x0, sizeof(com_item_c));
	ret = Com_item_Sel( g_pub_tx.reply,&com_item_c, "acc_hrt='31205' ");	/* δ�������� 31205 */
	MYRETERR

	/*ret = Com_branch_Dec_Sel( g_pub_tx.reply,
		"br_type not in('0','6') and br_no!='59801' and wrk_sts!='*' order by br_no" );mod by lwb 20150521  ���ӷ���*/
	ret = Com_branch_Dec_Sel( g_pub_tx.reply,
		"br_type not in('0','6','7') and br_no!='59801' and wrk_sts!='*' order by br_no" );
	MYRETERR

	while( 1 )
	{
		ret = Com_branch_Fet_Sel( &com_branch_c , g_pub_tx.reply );
		if( ret==100 ) break;
		MYRETERR

		pub_base_sysinit();
		strcpy( g_pub_tx.tx_br_no,com_branch_c.br_no );
		strcpy( g_pub_tx.opn_br_no,com_branch_c.br_no );
		g_pub_tx.trace_no=kslsh;
		set_zd_long( DC_TRACE_NO , g_pub_tx.trace_no);

		strcpy(g_pub_tx.brf, "�����ϻ�");

	ret=Gl_sub_Sel( g_pub_tx.reply,&gl_sub_c ,
			"br_no='%s' and cur_no='01' and acc_hrt='%s' ",
			com_branch_c.br_no,com_item_c.acc_hrt );
	if( ret==100 )
	{
		gl_sub_c.dr_bal=0.00;
		gl_sub_c.cr_bal=0.00;
	}
	else { MYRETERR }

	/* ���ܽ跽������ */
	d_amt=0;c_amt=0;
	ret = sql_sum_double("dc_log", "amt", &d_amt, 
		"tx_opn_br_no='%s' and acc_hrt='%s' and dc_ind='1' and sts='0'",
		com_branch_c.br_no,com_item_c.acc_hrt );
	MYRETERR

	ret = sql_sum_double("dc_log", "amt", &c_amt, 
		"tx_opn_br_no='%s' and acc_hrt='%s' and dc_ind='2' and sts='0'",
		com_branch_c.br_no,com_item_c.acc_hrt );
	MYRETERR
	sprintf( acErrMsg, "�跽���Ϊ:[%.2lf], �������Ϊ:[%.2lf]",d_amt,c_amt );
	WRITEMSG
	bal=gl_sub_c.cr_bal-gl_sub_c.dr_bal;
	bal=bal+c_amt-d_amt;
	if( pub_base_CompDblVal(bal,0.00)<=0 )
	{
		show_log("NOJZ [%s] [%s] [%c] [%.2lf]",
		com_branch_c.br_no,com_item_c.acc_no,yefx,bal);
		continue;
	}
	show_log("JZ   [%s] [%s] [%c] [%.2lf]",
		com_branch_c.br_no,com_item_c.acc_no,yefx,bal);
	strcpy(g_pub_tx.cur_no, "01");
	strcpy(g_pub_tx.ct_ind, "2");
	strcpy(g_pub_tx.add_ind, "1");
	g_pub_tx.tx_amt1 = -bal;

	pub_base_strpack( com_item_c.acc_no);
	strcpy( g_pub_tx.ac_no,com_item_c.acc_no);
	strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/ 
	g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/ 
	strcpy(g_pub_tx.prt_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
	g_pub_tx.trace_cnt=++num_cnt;
	set_zd_long( DC_TRACE_CNT,num_cnt);
	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "���ô�ȡ������ʧ��!!"); 
		WRITEMSG 
		goto ErrExit; 
	}
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
		MYRETERR

	/* �ǲ��� �� 31205 */
	strcpy(g_pub_tx.add_ind, "1");
	g_pub_tx.tx_amt1 =bal;
	strcpy( g_pub_tx.tx_br_no,com_branch_c.br_no );
	strcpy( g_pub_tx.opn_br_no,"59801");
	g_pub_tx.trace_cnt=++num_cnt;
	set_zd_long( DC_TRACE_CNT,num_cnt);
	ret = pub_acct_trance(); 
	if (ret != 0) 
	{
		sprintf(acErrMsg, "���ô�ȡ������ʧ��!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
		MYRETERR
		/**��ʼ��ӡ��Ʊ**/
		/**��Ӵ�Ʊ**/
    memset(filename,0x0,sizeof(filename));
    sprintf(filename,"%s/report/%s/gD304.txt",getenv("HOME"),com_branch_c.br_no);
    fp = fopen( filename,"w" ); 
    if( fp==NULL )
    {
        sprintf(acErrMsg," open file [%s] error ",filename );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
        goto ErrExit;
    }
    /**end**/
	/**��ʼ��ӡ��Ʊ**/
  char tmp_cxamt[21],vstr1[64],vstr[65];
  memset(vstr1,0x00,sizeof(vstr1));
  memset(vstr,0x00,sizeof(vstr));
  sprintf( tmp_cxamt, "%.2f" ,g_pub_tx.tx_amt1);
  pub_rpt_flttomon( tmp_cxamt,tmp_cxamt);	/* Сд��� */
  numtomoney(g_pub_tx.tx_amt1, vstr1);	/* ��д��� */
  strcat(vstr1,"       ��");
  strcat(vstr1,tmp_cxamt);
  sprintf(vstr,"%-64s",vstr1);
  vtcp_log("[%s][%d]���� vstr===[%s]\n",__FILE__,__LINE__,vstr);
	/*	fprintf(fp,"%c%c%c%c",0x1b,0x67,0x1c,0x0E);*/
		fprintf(fp, "                               ֧�������ϻ��������Ĵ�Ʊ\n");
		fprintf(fp, "                      ---------------------------------------------------------------\n");
		fprintf(fp,"\n");
		fprintf(fp, "     ������: %5s               ��������: %ld             ��ˮ��: %ld       \n",com_branch_c.br_no,g_pub_tx.tx_date,g_pub_tx.trace_no);
		fprintf(fp, "�������������Щ����������Щ����������Щ�����������������������������������������\n");
		fprintf(fp, "����  Ŀ  �ũ�31205     ����  Ŀ  ����δ��������                              ��\n");
		fprintf(fp, "�������������੤���������ة����������ة�����������������������������������������\n");
		fprintf(fp, "�����(��д)��%s��\n",vstr); 
		fprintf(fp, "�������������ة��������������������Щ����Щ�������������������������������������\n");
		fprintf(fp, "��                                �� �� �� ��       Ŀ(��):31205(֧��)        ��\n");
		fprintf(fp, "��  ���п����ѴӸõ�λ�˻���ת    �� �� ��                                    ��\n");
		fprintf(fp, "��                                �� �� �� �� �� �� Ŀ(��):31205 (��������)   ��\n");
		fprintf(fp, "��                  (�����и���)  �� ¼ ��                                    ��\n");
		fprintf(fp, "�����������������������������������ة����ة�������������������������������������\n");
		fprintf(fp,"\n\n\n");		
		fclose(fp);

		fpcw = fopen( filenamecw,"a" );
		if( fpcw==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filenamecw );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047" );
			goto ErrExit;
		}
		/*fprintf(fpcw,"%c%c%c%c",0x1b,0x67,0x1c,0x0E);*/
		fprintf(fpcw, "                               ֧�������ϻ��������Ĵ�Ʊ\n");
		fprintf(fpcw, "                      ---------------------------------------------------------------\n");
		fprintf(fpcw,"\n");
		fprintf(fpcw, "     ������: %5s               ��������: %ld             ��ˮ��: %ld       \n","59801",g_pub_tx.tx_date,g_pub_tx.trace_no);
		fprintf(fpcw, "�������������Щ����������Щ����������Щ�����������������������������������������\n");
		fprintf(fpcw, "����  Ŀ  �ũ�31205     ����  Ŀ  ����δ��������                              ��\n");
		fprintf(fpcw, "�������������੤���������ة����������ة�����������������������������������������\n");
		fprintf(fpcw, "�����(��д)��%s��\n",vstr); 
		fprintf(fpcw, "�������������ة��������������������Щ����Щ�������������������������������������\n");
		fprintf(fpcw, "��                                �� �� �� ��       Ŀ(��):31205(֧��)        ��\n");
		fprintf(fpcw, "��  ���п����ѴӸõ�λ�˻���ת    �� �� ��                                    ��\n");
		fprintf(fpcw, "��                                �� �� �� �� �� �� Ŀ(��):31205 (��������)   ��\n");
		fprintf(fpcw, "��                  (�����и���)  �� ¼ ��                                    ��\n");
		fprintf(fpcw, "�����������������������������������ة����ة�������������������������������������\n");
		fprintf(fpcw,"\n\n\n");	
    if(cwnum==3){
        fprintf(fpcw,"\f");
        cwnum=0;
    }
    fclose(fpcw);
    cwnum++;

}
	Com_branch_Clo_Sel();

OkExit:
    ret=sql_commit();   /*--�ύ����--*/
	MYRETERR
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");
    sql_rollback(); /*--����ع�--*/
	sprintf(acErrMsg,"����ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

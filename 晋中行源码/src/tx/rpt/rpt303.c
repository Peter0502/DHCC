/***************************************************************
* �� �� ��: rpt303.c
* ��������: ����תδ��������ı���   �� 311   �� 31205
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
#include "dc_log_bk_c.h"

	struct com_parm_c sCom_parm_lr;
	struct com_parm_c sCom_parm_lrfp;
int lsh=1;
rpt303()
{
    int ret=0;
	struct com_branch_c com_branch_c;
	struct dc_log_bk_c  dc_log_bk_c;
	FILE *fp=NULL;
	char filename[64];

    ret=sql_begin(); /*������*/
	MYRETERR

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	ret = Com_branch_Dec_Sel( g_pub_tx.reply,
		"br_type not in('0','6') and wrk_sts!='*' order by br_no" );
	MYRETERR

	while( 1 )
	{
		ret = Com_branch_Fet_Sel( &com_branch_c , g_pub_tx.reply );
		if( ret==100 ) break;
		MYRETERR

		memset(&dc_log_bk_c,'\0',sizeof(dc_log_bk_c));
		ret = Dc_log_bk_Sel(g_pub_tx.reply, &dc_log_bk_c, "tx_tx_br_no='%s' and trace_no=15028 and acc_hrt='31100'",com_branch_c.br_no);
		MYRETERR

	strcpy( g_pub_tx.tx_br_no,com_branch_c.br_no );
	strcpy( g_pub_tx.opn_br_no,com_branch_c.br_no );
	g_pub_tx.trace_no=15028;
	g_pub_tx.tx_date=20061231;

		/**��ʼ��ӡ��Ʊ**/
		/**��Ӵ�Ʊ**/
    memset(filename,0x00,sizeof(filename));
    sprintf(filename,"%s/report/%s/gD303.txt",getenv("HOME"),com_branch_c.br_no);
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
  sprintf( tmp_cxamt, "%.2f" ,dc_log_bk_c.amt);
  pub_rpt_flttomon( tmp_cxamt,tmp_cxamt);	/* Сд��� */
  numtomoney(dc_log_bk_c.amt, vstr1);	/* ��д��� */
  strcat(vstr1,"       ��");
  strcat(vstr1,tmp_cxamt);
  sprintf(vstr,"%-64s",vstr1);
  vtcp_log("[%s][%d]���� vstr===[%s]\n",__FILE__,__LINE__,vstr);
  
	/*	fprintf(fp,"%c%c%c%c",0x1b,0x67,0x1c,0x0E);*/
		fprintf(fp, "                               ֧�������ת��Ʊ\n");
		fprintf(fp, "                  ---------------------------------------------------\n");
		fprintf(fp,"\n");		
		fprintf(fp, "     ������: %5s               ��������: %ld            ��ˮ��: %ld       \n",com_branch_c.br_no,g_pub_tx.tx_date,g_pub_tx.trace_no);
		fprintf(fp, "�������������Щ����������Щ����������Щ�����������������������������������������\n");
		fprintf(fp, "����  Ŀ  �ũ�311       ����  Ŀ  ������������                                ��\n");
		fprintf(fp, "�������������੤���������ة����������ة�����������������������������������������\n");
		fprintf(fp, "�����(��д)��%s��\n",vstr); 
		fprintf(fp, "�������������ة��������������������Щ����Щ�������������������������������������\n");
		fprintf(fp, "��                                �� �� �� ��       Ŀ(��):311                ��\n");
		fprintf(fp, "��  ���п����ѴӸõ�λ�˻���ת    �� �� ��                                    ��\n");
		fprintf(fp, "��                                �� �� �� �� �� �� Ŀ(��):31205              ��\n");
		fprintf(fp, "��                  (�����и���)  �� ¼ ��                                    ��\n");
		fprintf(fp, "�����������������������������������ة����ة�������������������������������������\n");
		fprintf(fp,"\n\n\n");
		fclose(fp);
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

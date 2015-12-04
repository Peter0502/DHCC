/*************************************************************
* �� �� ��: rpt193.c
* �����������жһ�Ʊ�����ձ���, �ʻ��ʽ��������ϸ
*
* ��    ��: rob
* �������: 2003��09��13��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "mo_bank_acc_po_c.h"
#define _SQLTOOL_INCL
#include "sqltool.h"
struct trace_log_c	s_trace_log;
struct mo_bank_acc_po_c	s_mo_bank_acc_po;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

/* �жһ�Ʊ�����ձ��� */
rpt193()
{
	int flag=0;

	char fname[100];
	char filename[100];
	char SQL[1024];  /**��ѯ**/
	char tmpstr[100];
	char tmp_str[60];
	double sum_1=0, sum_2=0;   /* ��Ʊ�ܶ�, �ջ��ܶ� */

	memset( SQL,0,sizeof(SQL) );
	memset( &s_trace_log , 0x00 , sizeof(struct trace_log_c));
	
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
/*
   sprintf( acErrMsg, "�򿪰�!!!    " );
	  strrtrim(acErrMsg, acErrMsg); 
	  strcpy(g_pub_tx.reply,"0001");
	  WRITEMSG
	goto ErrExit ;
*/
	strcpy( filename , "BRcdhpfsrb" );	
	strcpy(vpfm, "cdhpfsrb" );
	
	/* ����: Ʊ������=111���гжһ�Ʊ */
	
	strcpy( SQL,"select A.tx_br_no _08[���׻���], A.ac_no _14[��ͬ��], B.payer_name [�жҵ�λ], B.payer_ac_no [�����ʺ�], \
	 a.amt*decode(add_ind,'1',1, -1)  _1692[������],  req_date [��Ʊ��], matr_date [������], bail_amt _1492[��֤��] \
	 from trace_log A,  mo_bank_acc_po B where A.note_type='111' and A.sts='0' and A.amt<>0.0 and A.ac_no=B.pact_no ");

	/**����**/	
	strcat(SQL," ORDER BY tx_br_no,opn_br_no,trace_no ");

	vtcp_log( "--*--SQL [%s]",SQL );
	
	page=1;
	line_file=1;
	
	/* ɾ��ԭ�ļ� */
	ret = pub_rpt_rmfile("99999" , filename , 0 );
	ERR_DEAL
	/* ���ļ� */	
	ret = pub_rpt_openfile( &fp, "99999" , filename );
	ERR_DEAL
	/* ��ӡҳ�� */
        fprintf(fp, "%30s%s�жһ�Ʊ�����ձ���\n"," ", BANK_CNAME);
        sprintf( tmp_str , "%4d��%2d��%2d��" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
        fprintf(fp,"    ��������:%s%60s    ��λ��Ԫ\n", tmp_str);
        /* ��ӡ������ */ 
/*	wprt_txt(SQL, 0, fp, 0, 0," ",'|',0, NULL ); */
	 wprt_hzcell( SQL,  0, fp, 50, "", 5, 1,35065, NULL,NULL,NULL,NULL);
	fclose(fp);

     rpt193_dd1( SQL )  ;    /*  �����ʻ��ʽ��������ϸ  */
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɳжһ�Ʊ�����ձ���ȳɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɳжһ�Ʊ�����ձ����ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*  �ʻ��ʽ��������ϸ  
*    
*   ��    ��: rob
*   �������: 2003��09��13��
*/

int rpt193_dd1_ftop(FILE * fprt, int pg )     /*  ��ҳͷ  */
{
    	page = atoi( sql_Field (0)->f_char ) ;     /* �������֧�� */
    	if (fprt != NULL) fclose(fprt);
	/* ɾ��ԭ�ļ� */
	ret = pub_rpt_rmfile(sql_Field (0)->f_char , "BRdjdcmx" , 0 );
	if (ret) goto ErrExit1 ;
	/* ���ļ� */	
	ret = pub_rpt_openfile( &fprt, sql_Field (0)->f_char , "BRdjdcmx" );
	if (ret) goto ErrExit1 ;
  sprintf( acErrMsg, "====����%d�ļ���BRdjdcmx !!!", page );
  WRITEMSG

        fprintf(fprt, "%20s%s�ʻ��ʽ��������ϸ(%d)\n", " ", BANK_CNAME, page );   
  	return 0;      
  	ErrExit1:
  sprintf( acErrMsg, "====��%d�ļ�������!!!", page );
  WRITEMSG
  	return -1; 
}

int rpt193_dd1_fnewpg(int r, int i )     /*  ��ҳ  */
{
	return (atoi( sql_Field (0)->f_char ) != page) ;     /* ����֧�б仯  */
	
}

int rpt193_dd1( char *SQL )
{
      /*----- (����) �Թ�����>=50��, ����>=30�� */
  	/* ɾ��ԭ�ļ� */
	ret = pub_rpt_rmfile("99999" , "BRdjdcmx" , 0 );
	if (ret) goto ErrExit1 ;
	/* ���ļ� */	
	ret = pub_rpt_openfile( &fp, "99999" , "BRdjdcmx" );
	if (ret) goto ErrExit1 ;
	/* ��ӡҳ�� */
        fprintf(fp, "%20s%s�ʻ��ʽ��������ϸ(����)\n"," ", BANK_CNAME);
        strcpy(SQL," select b.up_br_no [����֧��], opn_br_no [������],prdt_no [��Ʒ���], name [�ʻ���], (bal - ys_bal)/10000 [����������] \
  	   from dd_mst a  join  com_branch b on a.opn_br_no = b.br_no  \
  	   where abs(bal - ys_bal)>=500000 or (prdt_no<'109' and abs(bal - ys_bal)>=300000)  order by 1,2");
        wprt_hzcell( SQL,  0, fp, 50, "", 5, 1,85065, NULL,NULL,NULL,NULL);
        fclose(fp);
      /*----- (����֧��) �Թ�����>=10��, ����>=5��  */
       fp=NULL;  
       page=0;
       strcpy(SQL," select b.up_br_no [����֧��], opn_br_no [������],prdt_no [��Ʒ���], name [�ʻ���], (bal - ys_bal)/10000 [����������] \
  	   from dd_mst a  join  com_branch b on a.opn_br_no = b.br_no  \
  	   where abs(bal - ys_bal)>=100000 or (prdt_no<'109' and abs(bal - ys_bal)>=50000)  order by 1,2");
       wprt_hzcell( SQL,  0, fp, 50, "", 5, 1,85065, NULL, rpt193_dd1_fnewpg , rpt193_dd1_ftop ,NULL);
       fclose(fp);
  return 0;      
  ErrExit1:
  sprintf( acErrMsg, "====���ļ�������!!!" );
  WRITEMSG
  return -1;
}


/*************************************************************
* �� �� ��: gD091.c
* �������������գ����յ�����ʾ
*
* ��    ��: lance
* �������: 2003��06��17��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_holiday_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_discnt_c.h"
#include "com_sys_parm_c.h"

struct com_sys_parm_c com_sys_parm;

gD091()
{
	int ret=0;

	ret=sql_begin(); /*������*/
	MYSQLERR

	/* ��ʼ��ȫ�ֱ��� */
	pub_base_sysinit();
	TRACE

	ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	MYSQLERR

	/* ���յ��ڴ�� */
	ret=get_dqdk();
	MYERR
	
	/* �жһ�Ʊ������ʾ */
	ret=get_cddq();	
	MYERR
		
	/* ���ֵ�����ʾ */
	ret=get_txdq();
	MYERR

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ʾ�������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");/* ���޸� */
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��ʾ�������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
int get_dqdk()
{
	char wherelist[1024];
	struct com_holiday_c com_holiday;
	int first=0;
	char br_no_old[10];
	int ret=0;

	ret = pub_base_date_jjr( com_sys_parm.lst_date,&com_holiday );
	if( ret )
	{
		sprintf( wherelist,"( mtr_date=%ld or exp_mtr_date=%ld)",
			com_sys_parm.sys_date ,com_sys_parm.sys_date );
	}
	else
	{
		sprintf( wherelist,"((mtr_date>=%ld and mtr_date<=%ld) or (exp_mtr_date>=%ld and exp_mtr_date<=%ld) )",
			com_holiday.beg_date,com_sys_parm.sys_date ,com_holiday.beg_date,com_sys_parm.sys_date );
	}
	strcat( wherelist," and ac_sts in('1','2','3','4') " );

	vtcp_log("DK DQ[%s]",wherelist);

	ret = Ln_mst_Dec_Sel(g_pub_tx.reply,"%s ORDER BY opn_br_no",wherelist );
	MYSQLERR

	first=0;
	while(1)
	{
		TRACE
		ret = Ln_mst_Fet_Sel(&g_ln_mst, g_pub_tx.reply);
		if( ret==100 ) break;
		MYSQLERR

		vtcp_log("acid=[%d] ac_seqn[%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn);

		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,
				"ac_id =%ld",g_ln_mst.ac_id);
		if ( ret == 100 )
		{
			sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"W010");
			goto ErrExit;
		}
		MYSQLERR

		/* �жϽڼ����Ƿ��� */
		/*xxxx�Ȳ�Ҫ��*/

		if( strcmp(br_no_old,g_ln_mst.opn_br_no) || first==0 )
		{
			if( !first )
				strcpy ( br_no_old , g_ln_mst.opn_br_no );/* ���׻��� */
			first=1;
			pub_base_prom_inf( g_ln_mst.opn_br_no , "0" , 
		"����������� %d %s���� ���ڴ����˻���Ϣ�����������",
				g_pub_tx.tx_date, g_ln_mst.opn_br_no);			
			pub_base_prom_inf( g_ln_mst.opn_br_no , "0" , 
"�����������---------------------�������������������" 
				);
		}
				
		/* д��ʾ��Ϣ */
		pub_base_prom_inf( g_ln_mst.opn_br_no , "0" , 
			"�˺�:%13s ����:%50s" ,
				g_mdm_ac_rel.ac_no,g_ln_mst.name);
		pub_base_prom_inf( g_ln_mst.opn_br_no , "0" , 
			"    �������:%15.2lf ������:%ld" ,
				g_ln_mst.ys_bal,g_ln_mst.mtr_date);
		pub_base_prom_inf( g_ln_mst.opn_br_no , "0" , " " );

		if( strcmp(br_no_old,g_ln_mst.opn_br_no) )
		{			
			pub_base_prom_inf( br_no_old , "0" , 
"�����������---------------------�������������������" 
				);
		}
		strcpy ( br_no_old , g_ln_mst.opn_br_no );/* ���׻��� */			
	}
	Ln_mst_Clo_Sel();

	if( first )
	{
			pub_base_prom_inf( br_no_old , "0" , 
"�����������---------------------�������������������" 
				);
	}

	return 0;
ErrExit:
	return 1;
}
int get_cddq()
{
	struct S_ln_intst vln_intst[LN_INTST_TYPS];
	struct com_holiday_c com_holiday;
	struct ln_mst_c ln_mst_tmp;
  	struct mo_bank_acc_po_c mo_bank_acc_po;
   	struct mo_discnt_c mo_discnt;
	int    ret=0,num_tmp=0,chg_flag=0,fro_flg=0;
	int    ac_seqn;
	char ln_ac_no[20],dd_ac_no[20],flag[2],end_str[9],br_no_old[6];  
	char t_note_type[17],t_pn_type[11];
	
 	 	  
	strcpy( br_no_old ,"$$$$$" );	/* ��ʼ�������� */
   	 	  	 
	memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
	memset(&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
  	memset(&mo_bank_acc_po, 0x00, sizeof(struct mo_bank_acc_po_c));
	memset(&mo_discnt, 0x00, sizeof(struct mo_discnt_c));
	strcpy( br_no_old ,"$$$$$" );	/* ��ʼ�������� */

	num_tmp=0;
	fro_flg=0;	
		
	ret = Mo_bank_acc_po_Dec_Sel(g_pub_tx.reply,"matr_date=%ld and acc_po_sts='2' and acc_po_ind='0' ORDER BY tx_br_no , pact_no" , g_pub_tx.tx_date );
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		TRACE
		ret = Mo_bank_acc_po_Fet_Sel(&mo_bank_acc_po, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if (ret == 100){
			break;
		}
		num_tmp++;
		fro_flg++;
		if( strcmp ( br_no_old , g_ln_mst.opn_br_no ) && fro_flg==1 )
		{
			TRACE
			pub_base_prom_inf( mo_bank_acc_po.tx_br_no , "0" , "����������� %d %s���� �������гжһ�Ʊ��Ϣ�����������",g_pub_tx.tx_date, mo_bank_acc_po.tx_br_no);			
			pub_base_prom_inf( mo_bank_acc_po.tx_br_no , "0" , "�����������---------------------�������������������" );			

		}	
		
		/* д��ʾ��Ϣ */
		TRACE
		pub_base_prom_inf( mo_bank_acc_po.tx_br_no , "0" , 
			"Э����:%20sƱ����:%15.2lf" , 
			mo_bank_acc_po.pact_no , mo_bank_acc_po.sign_amt );		

		pub_base_prom_inf( mo_bank_acc_po.tx_br_no , "0" , 
			"    �������˺�:%24s ����������:%50s" , 
			mo_bank_acc_po.payer_ac_no , mo_bank_acc_po.payer_name );		
		pub_base_prom_inf( mo_bank_acc_po.tx_br_no , "0" , " " );

		if( strcmp ( br_no_old , mo_bank_acc_po.tx_br_no ) )
		{			
			TRACE
			if( strcmp( br_no_old ,"$$$$$" ) )
			{
				TRACE
				pub_base_prom_inf( br_no_old , "0" , "�����������---------------------------------------�������������������" );			
				num_tmp=0;
			}
			fro_flg=0;
		}
		strcpy ( br_no_old , mo_bank_acc_po.tx_br_no );/* ���׻��� */			
	}
	
	if( num_tmp )
	{
		TRACE
		pub_base_prom_inf( br_no_old , "0" , "�����������---------------------------------------�������������������" );					
	}
	TRACE			
	Mo_bank_acc_po_Clo_Sel();

	return 0;
ErrExit:
	return 1;
}
int get_txdq()
{
	struct S_ln_intst vln_intst[LN_INTST_TYPS];
	struct com_holiday_c com_holiday;
	struct ln_mst_c ln_mst_tmp;
   	struct mo_discnt_c mo_discnt;
	int    ret=0,num_tmp=0,chg_flag=0,fro_flg=0;
	int    ac_seqn;
	char ln_ac_no[20],dd_ac_no[20],flag[2],end_str[9],br_no_old[6];  
	char t_note_type[17],t_pn_type[11];
	
	memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
	memset(&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
	memset(&mo_discnt, 0x00, sizeof(struct mo_discnt_c));

	strcpy( br_no_old ,"$$$$$" );	/* ��ʼ�������� */
	num_tmp=0;
	fro_flg=0;
	
	ret = Mo_discnt_Dec_Sel(g_pub_tx.reply,"not_matr_date=%ld and sts='3' ORDER BY br_no , pn_type" , g_pub_tx.tx_date );
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		TRACE
		ret = Mo_discnt_Fet_Sel(&mo_discnt, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if (ret == 100){
			break;
		}
		num_tmp++;
		fro_flg++;
		
		if( strcmp ( br_no_old , mo_discnt.br_no ) && fro_flg==1 )
		{
			TRACE
			pub_base_prom_inf( mo_discnt.br_no ,"0" , "����������� %d %s���� ���������˻���Ϣ�����������",g_pub_tx.tx_date, mo_discnt.br_no);			
			pub_base_prom_inf( mo_discnt.br_no , "0" , "�����������------------------------------------�������������������" );			

		}	
		/* д��ʾ��Ϣ */
		TRACE
		pub_base_dic_show_str(t_pn_type,"pn_type",mo_discnt.pn_type);		/* �������� */
		pub_base_dic_show_str(t_note_type,"mo_note_type",mo_discnt.note_type);	/* Ʊ������ */	
				
		if( strlen(mo_discnt.pact_no) )
		{
			pub_base_prom_inf( mo_discnt.br_no , "0" , "����:%10s ��  ��Э��:%20s���:%15.2lf Ʊ������:%12s Ʊ�ݺ���:%16s" , t_pn_type ,  mo_discnt.pact_no , mo_discnt.tranx_bal , t_note_type , mo_discnt.pnote_no );		
		}else if( strlen(mo_discnt.tpact_no) ){
			pub_base_prom_inf( mo_discnt.br_no , "0" , "����:%10s ת����Э��:%20s���:%15.2lf Ʊ������:%12s Ʊ�ݺ���:%16s" , t_pn_type ,  mo_discnt.tpact_no , mo_discnt.tranx_bal , t_note_type , mo_discnt.pnote_no );					
		}else if( strlen(mo_discnt.rpact_no) ){
			pub_base_prom_inf( mo_discnt.br_no , "0" , "����:%10s ������Э��:%20s���:%15.2lf Ʊ������:%12s Ʊ�ݺ���:%16s" , t_pn_type ,  mo_discnt.rpact_no , mo_discnt.tranx_bal , t_note_type , mo_discnt.pnote_no );					
		}else{
			break;	
		}
		
		if( strcmp ( br_no_old , mo_discnt.br_no ) )
		{			
			TRACE
			if( strcmp( br_no_old ,"$$$$$" ) )
			{
				TRACE
				pub_base_prom_inf( br_no_old , "0" , "�����������---------------------------------------�������������������" );			
				num_tmp=0;
			}
			fro_flg=0;	
		}
		strcpy ( br_no_old , mo_discnt.br_no );/* ���׻��� */			
	}
	
	if( num_tmp )
	{
		TRACE
		pub_base_prom_inf( br_no_old , "0" , "�����������---------------------------------------�������������������" );					
	}
	TRACE			
	Mo_discnt_Clo_Sel();

	return 0;
ErrExit:
	return 1;
}

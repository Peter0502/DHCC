/***************************************************************
* �� �� ��:     pubf_ass.c
* ����������
*              2���Ǽǿ�����ϸ�ʣ�
*
*              1���Ǽ����棻
*
* ��    ��:     x
* ������ڣ�    2005��12��30��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ass_br_def_c.h"
#include "ass_gl_c.h"
#include "com_item_c.h"
#include "dc_entry_c.h"

int pubf_ass_sy( pacc_hrt,pdc_ind,pcur_no,pct_ind,popn_br_no,ptx_br_no,p_amt,p_entry )
 char * pacc_hrt;
 char * pdc_ind;
 char * pcur_no;
 char * pct_ind;
 char * popn_br_no;
 char * ptx_br_no;
 double p_amt;
 struct dc_entry_c p_entry;
{
	struct ass_br_def_c ass_br_def;
	struct pub_tx v_pub_tx;
	struct ass_gl_c ass_gl;
	struct com_item_c com_item;
	int ret=0;
	char vbrf[41];


	return(0);		/* ���˲�������ı�� */


	memset(&ass_br_def,0x00,sizeof(struct ass_br_def_c));

sprintf(acErrMsg,"����ASS [%s][%s][%.2lf]",pacc_hrt,pdc_ind,p_amt );
WRITEMSG

	ret = Com_item_Sel( g_pub_tx.reply,&com_item,"acc_hrt='%s'",pacc_hrt);
    if ( ret )
    {
		sprintf(acErrMsg,"ȡ��Ŀ������![%s]",pacc_hrt);
        WRITEMSG
        strcpy(g_pub_tx.reply,"S036");
        goto ErrExit;
    }
	if( com_item.in_ind[0]!='A' ) 
	{
		sprintf(acErrMsg,"�ǿ��˿�Ŀ");
		WRITEMSG
		sprintf(acErrMsg,"����ASS [%s][%s][%.2lf]",pacc_hrt,pdc_ind,p_amt );
		WRITEMSG		
		return 0;
	}
sprintf(acErrMsg,"����ASS [%s][%s][%.2lf]",pacc_hrt,pdc_ind,p_amt );
WRITEMSG

    /* Ӫҵ�����򿼺˻�����ת�� */
    ret = Ass_br_def_Sel(g_pub_tx.reply,&ass_br_def,"sub_br_no='%s' and \
          beg_date <= %ld and end_date >= %ld",g_pub_tx.opn_br_no,
          g_pub_tx.tx_date,g_pub_tx.tx_date);
    if ( ret )
    {
		sprintf(acErrMsg,"ȡ�����������˲�����![%s][%d][%ld]", \
				g_pub_tx.opn_br_no,ret,g_pub_tx.tx_date);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O100");
        goto ErrExit;
    }

sprintf(acErrMsg,"����ASS [%s][%s][%.2lf]",pacc_hrt,pdc_ind,p_amt );
WRITEMSG

    /* �޸Ŀ������� */
    strcpy(ass_gl.br_no,ass_br_def.br_no);
    ret = Ass_gl_Dec_Upd(g_pub_tx.reply,"br_no='%s' and ass_code='%s' ",
		ass_gl.br_no,pacc_hrt);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }


    ret = Ass_gl_Fet_Upd(&ass_gl,g_pub_tx.reply);
	if ( ret==100 )
	{
		memset( &ass_gl,0,sizeof(ass_gl) );
		strcpy( ass_gl.br_no,g_pub_tx.opn_br_no );
		strcpy( ass_gl.ass_code,pacc_hrt );
		ass_gl.in_bal=0.00;
		ass_gl.out_bal=0.00;
		ass_gl.in_intst_bal=0.00;
		ass_gl.out_intst_bal=0.00;
		ass_gl.sts[0]='1';
		if( pdc_ind[0]=='1' )
    		ass_gl.in_bal += p_amt;
		else
    		ass_gl.out_bal += p_amt;
		ret = Ass_gl_Ins( ass_gl,g_pub_tx.reply );
		if( ret )
		{
        	sprintf(acErrMsg,"Insert!! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
		}
	}
    else if ( ret )
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        Ass_gl_Clo_Upd();
        goto ErrExit;
    }
	else
	{
		if( pdc_ind[0]=='1' )
    		ass_gl.in_bal += p_amt;
		else
    		ass_gl.out_bal += p_amt;
    	ret = Ass_gl_Upd_Upd(ass_gl,g_pub_tx.reply);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	Ass_gl_Clo_Upd();
        	goto ErrExit;
    	}
	}

    Ass_gl_Clo_Upd();

    /* �Ǽǽ�����ˮ */
	memcpy( &v_pub_tx,&g_pub_tx,sizeof(v_pub_tx) );

	pub_base_strpack( com_item.acc_no );
	strcpy( g_pub_tx.ac_no,com_item.acc_no );
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;

	strcpy( g_pub_tx.cur_no,pcur_no );
	strcpy( g_pub_tx.note_type,"" );
	strcpy( g_pub_tx.beg_note_no,"" );
	strcpy( g_pub_tx.end_note_no,"" );

	strcpy( g_pub_tx.ct_ind,pct_ind );
	strcpy( g_pub_tx.add_ind,"1" );

	strcpy( g_pub_tx.opn_br_no,popn_br_no );
	strcpy( g_pub_tx.tx_br_no,ptx_br_no );
vtcp_log("ENTRYBRF[%s]",p_entry.brf);
	pub_base_strpack(p_entry.brf);
	if( strlen(p_entry.brf) ) strcpy( g_pub_tx.brf,p_entry.brf );

	g_pub_tx.tx_amt1=p_amt;
sprintf(acErrMsg,"����ASS [%s][%s][%.2lf]",pacc_hrt,pdc_ind,p_amt );
WRITEMSG

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }
	memcpy( &g_pub_tx,&v_pub_tx,sizeof(g_pub_tx) );


	return 0;
ErrExit:
	return 1;
}

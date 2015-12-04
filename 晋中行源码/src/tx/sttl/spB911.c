/*************************************************
* �� �� ��:  spB911.c
* ���������� ���������Ϣ¼�빦�ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��4��5��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int spB911()
{
	struct	mo_discnt_c	sMoDiscnt,sMoDiscnt1;	/* ���ֵǼǱ� */
	struct	mdm_ac_rel_c	sMdmAcRel;			/* �������˻���ϵ�� */
	struct	dd_mst_c	sDdMst;			/* ���ڴ�����ļ� */
  struct	dd_mst_c	s_DdMst;			/* ���ڴ�����ļ� */
	char	cOperCode[2];				/* ������ */
	char	cName[61];				/* 25������ */
	long	lPnDays=0;					/* �������� */
	double	rate_val;					/* ���� */
	int	ret;
	char    pcTmp[64];
	char	payName[51];
	char  cPn_knd[2];
    	char            cTmpAcno[21];
    	struct in_mst_c sInMst; /***add by ligl 2010/5/5 23:14:10**/
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( payName, 0x00, sizeof( payName ) );
	memset( cName, 0x00, sizeof( cName ) );
	memset( cPn_knd, 0x00, sizeof( cPn_knd ) );
	memset(cTmpAcno , 0 , sizeof(cTmpAcno));
	memset( &sInMst, 0x00, sizeof( struct in_mst_c ) );/***add by ligl 2010/5/5 23:14:10**/
  memset( &s_DdMst, 0x00, sizeof( struct dd_mst_c ) );/***add by ligl 2010/5/5 23:14:10**/

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0690", cOperCode );				/* ������ */
	get_zd_data( "0620", sMoDiscnt.pact_no );		/* ����Э���� */
	    get_zd_data("0630" , cTmpAcno);
	strcpy( sMoDiscnt.br_no, g_pub_tx.tx_br_no );	/* �������� */
	{
		/****TODO****/
		double a;
		memset(pcTmp,'\0',sizeof(pcTmp));
		get_zd_data("1002",pcTmp);
		vtcp_log("%s,%d,fd[1002]=%s\n",__FILE__,__LINE__,pcTmp);
		get_zd_double( "1002", &a );	/* ��Ʊ��� */
		vtcp_log("%s,%d,fd_db[1002]=%lf\n",__FILE__,__LINE__,a);
	}
	get_zd_double( "1002", &sMoDiscnt.par_amt );	/* ��Ʊ��� */
vtcp_log( "[%s][%d]   sMoDiscnt.par_amt[%lf] ",__FILE__,__LINE__, sMoDiscnt.par_amt );
	get_zd_double( "0840", &sMoDiscnt.pn_rate );	/* ������ */
vtcp_log( "[%s][%d]   sMoDiscnt.pn_rate[%lf] ",__FILE__,__LINE__, sMoDiscnt.pn_rate );
	sMoDiscnt.pn_date = g_pub_tx.tx_date;			/* ������ */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* ������Ϣ¼��ʱ�������� */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* ¼����ˮ�� */
	get_zd_long( "0450", &sMoDiscnt.not_matr_date );/* ��Ʊ������ */
vtcp_log( "[%s][%d]   sMoDiscnt.par_amt[%lf] ",__FILE__,__LINE__, sMoDiscnt.par_amt );
vtcp_log( "[%s][%d]   sMoDiscnt.pn_rate[%lf] ",__FILE__,__LINE__, sMoDiscnt.pn_rate );
vtcp_log( "[%s][%d] sMoDiscnt.not_matr_date[%ld] ", __FILE__,__LINE__,sMoDiscnt.not_matr_date );
	get_zd_data( "0310", sMoDiscnt.rem_ac_no );		/* ��Ʊ���ʺ� */
	get_zd_data( "0260", sMoDiscnt.remitter );		/* ��Ʊ��ȫ�� */
	get_zd_data( "0810", sMoDiscnt.payer_brno_name );/* ������ȫ�� */
	get_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* �տ����ʺ� */
	get_zd_data( "0270", sMoDiscnt.payee );			/* �տ���ȫ�� */
/* sMoDiscnt.payee_brno_name��30λ������82����50λ
	get_zd_data( "0820", sMoDiscnt.payee_brno_name );�տ��˿�����ȫ�� */

	get_zd_data( "0820", payName );/* �տ��˿�����ȫ�� */
	memcpy(sMoDiscnt.payee_brno_name, payName, sizeof(sMoDiscnt.payee_brno_name)-1);
	get_zd_data( "0900", sMoDiscnt.note_type );		/* Ʊ������ */
	get_zd_data( "0600", sMoDiscnt.pnote_no );		/* Ʊ�ݺ� */
	/***add by ligl 2010/5/12 17:07:05 dp***/
	get_zd_data( "0980", cPn_knd );		/* Ʊ�ݺ� */
	if(cPn_knd[0]=='E')
	get_zd_data( "0670", sMoDiscnt.ac_type );		/* �տ��˻����� */
	get_zd_data( "0830", sMoDiscnt.pn_ac_no );		/* �����������˺� */
	get_zd_data( "0250", cName );		/* ���������˻��� */
	get_zd_data( "0680", sMoDiscnt.local_ind );		/* ���ͬ�Ǳ�־ */
	get_zd_long( "0480", &sMoDiscnt.way_days );		/* ��;���� */
	memcpy(sMoDiscnt.pn_name,cName,sizeof(sMoDiscnt.pn_name)-1);
vtcp_log( "[%s][%d]   sMoDiscnt.par_amt[%f] ",__FILE__,__LINE__, sMoDiscnt.par_amt );
vtcp_log( "[%s][%d]   sMoDiscnt.pn_rate[%lf] ",__FILE__,__LINE__, sMoDiscnt.pn_rate );
vtcp_log( "[%s][%d] sMoDiscnt.not_matr_date[%ld] ",__FILE__,__LINE__, sMoDiscnt.not_matr_date );
vtcp_log( "[%s][%d] lock sMoDiscnt.ac_type[%s] ",__FILE__,__LINE__, sMoDiscnt.ac_type );
vtcp_log( "[%s][%d] lock sMoDiscnt.pn_name[%s] ",__FILE__,__LINE__, sMoDiscnt.pn_name );

	if( cOperCode[0] == '0' )			/* ¼�� */
	{
		/* ������ֵǼǱ����Ƿ��Ѿ����ڴ�����Э���� */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'",\
				 sMoDiscnt.pact_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "���ֵǼǱ����Ѿ����ڴ�����Э����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B111" );
			goto ErrExit;
       	}else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
			WRITEMSG
			goto ErrExit;
       	}
vtcp_log( "33333333333  sMoDiscnt.par_amt[%f] ", sMoDiscnt.par_amt );
vtcp_log( "33333333333sMoDiscnt.not_matr_date[%ld] ", sMoDiscnt.not_matr_date );

		/* ������ֵǼǱ����Ƿ��Ѿ����ڴ�Ʊ�����ͺ�Ʊ�ݺ� */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "note_type='%s' and pnote_no='%s'",\
				 sMoDiscnt.note_type, sMoDiscnt.pnote_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "���ֵǼǱ����Ѿ����ڴ�Ʊ�����ͺ�Ʊ�ݺ�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B174" );
			goto ErrExit;
       	}else if( ret != 100 )
       	{
      		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
			WRITEMSG
			goto ErrExit;
       	}
	}
	else if( cOperCode[0] == '1' || cOperCode[0] == '2' )			/* �޸�,ɾ�� */
	{
		vtcp_log( "pact_no[%s]", sMoDiscnt.pact_no );
		/* ������ֵǼǱ����Ƿ���ڴ�����Э���� */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "pact_no='%s'",\
							 sMoDiscnt.pact_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "������Э���Ų�����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B113" );
			goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
vtcp_log( "pn_type[%s]", sMoDiscnt1.pn_type );
		if( sMoDiscnt1.pn_type[0] != '1' )
		{
			sprintf( acErrMsg, "����������!pn_type[%s]", sMoDiscnt1.pn_type );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B276" );
			goto ErrExit;
		}
/* ��������Э��״̬ ���ӵ�Ʊ2010/5/5 21:26:10 */
        if (sMoDiscnt1.sts[0] != 'X') {
            sprintf(acErrMsg, "�ñ�������������������,��¼��״̬!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B131");
            goto ErrExit;
        }
		/* ��������Э��״̬ */
		if( sMoDiscnt1.sts[0] == '2' )
		{
			sprintf( acErrMsg, "������Э�������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B152" );
			goto ErrExit;
		}else if( sMoDiscnt1.sts[0] == '3' )
		{
			sprintf( acErrMsg, "�˱������Ѽ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B148" );
			goto ErrExit;
		}else if( sMoDiscnt1.sts[0] == '*' )
		{
			sprintf( acErrMsg, "�˱������ѻ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B149" );
			goto ErrExit;
		}
    /***add by ligl 2010/5/5 22:52:45 ��Ʊ���˺� ����Ʊ��**/
     memcpy(sMoDiscnt.rem_ac_no,sMoDiscnt1.rem_ac_no,sizeof(sMoDiscnt.rem_ac_no)-1);
     memcpy(sMoDiscnt.pnote_no,sMoDiscnt1.pnote_no,sizeof(sMoDiscnt.pnote_no)-1);
     memcpy(sMoDiscnt.po_knd,sMoDiscnt1.po_knd,sizeof(sMoDiscnt.po_knd)-1);
     /***end by ligl 2010/5/5 23:14:48**/
		/* ɾ��������Э���Ŷ�Ӧ�ļ�¼ */
		ret = sql_execute( "DELETE from mo_discnt WHERE pact_no='%s'",\ 
					 		sMoDiscnt.pact_no );
		if( ret ) 
		{
      		sprintf( acErrMsg, "ִ��sql_execute��![%d]", ret );
			WRITEMSG
			goto ErrExit;
      	}

		if( cOperCode[0] == '2' )		/*** ɾ�� ***/
		{
      		sprintf( acErrMsg, "ɾ���ɹ�" );
			WRITEMSG
			goto OkExit;
		}
	}
vtcp_log( "4444444444   sMoDiscnt.par_amt[%f] ", sMoDiscnt.par_amt );
vtcp_log( "4444444444 sMoDiscnt.not_matr_date[%ld] ", sMoDiscnt.not_matr_date );

	/* ȡ���ֲ�Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
	ret = pub_base_GetParm_Str( "TXCP", 1, sMoDiscnt.bkrl_code );
	if( ret )
	{
		sprintf( acErrMsg, "ȡ���ֲ�Ʒ��Ŵ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B126" );
		goto ErrExit;
	}

	/**** ����������������Ƿ���Ϲ涨  2003��8��17���ʽ�Ӫ�˲���������ʲ������ƣ����Է��  by jane *
	* ���ݴ����Ʒ���ȡ������� *
	ret = Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", sMoDiscnt.bkrl_code );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�����ֲ�Ʒ��Ų�����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B240" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Ln_parm_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	* ȡ���ʱ�Ŷ�Ӧ������ *
	ret = pub_base_getllz(g_ln_parm.rate_no,g_ln_parm.cur_no,
				g_pub_tx.tx_date,&rate_val);
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"ת��ǰ����[%lf]",rate_val);
	WRITEMSG
	
	rate_val = rate_val/1.2;* ת���������� * 
	sprintf(acErrMsg,"ת������������[%lf]",rate_val);
	WRITEMSG

	if( g_ln_parm.get_rate_type[0]=='0' ) * �������� *
	{
		* �Ƚ�������ֵ�Ƿ���ȣ�ǰһ������Ļȡֵ����һ�������ʱ��еĹ̶�����ֵ *
		if( pub_base_CompDblVal( sMoDiscnt.pn_rate, rate_val ) != 0 )
		{
			sprintf( acErrMsg, "��������������涨����ֵ����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B241" );
			goto ErrExit;
		}	
	}else		* �������ʼ���ϸ��¸����� *  
	{
		* �������ʼ�� *
		if( pub_base_CompDblVal( sMoDiscnt.pn_rate,
				rate_val*( (100+g_ln_parm.max_flt_ratio)/100.00 ) )>0 )
		{
vtcp_log("=[%d][%s]",g_ln_parm.max_flt_ratio,sMoDiscnt.bkrl_code );
			sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",
				rate_val,rate_val*((100+g_ln_parm.max_flt_ratio)/100.00)
				,g_ln_parm.max_flt_ratio );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A006" );
			goto ErrExit;
		}
sprintf(acErrMsg,"���������ȷʱ[%lf][%lf][%d] ", rate_val,
	rate_val*((100+g_ln_parm.max_flt_ratio)/100.00) ,g_ln_parm.max_flt_ratio );
WRITEMSG

		if( pub_base_CompDblVal( sMoDiscnt.pn_rate,
			rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) )<0 )
		{
			sprintf(acErrMsg,"������ʴ���ʱ[%lf][%lf][%d] ", rate_val,
	 			rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A005" );
			goto ErrExit;
		}
sprintf(acErrMsg,"���������ȷʱ[%lf][%lf][%d] ", rate_val,
	rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
WRITEMSG	
	}****/

	/* ��������������˺�Ϊ���е�,��������������˺���Ч�ԣ������ǻ��ڡ�
		�˻�״̬��������,�����������Ƿ���� */
	/* ���ݿͻ��ʺŲ�ѯ�ͻ��ʺ�ID���ʻ���� */
#if 0 /***modify by ligl 2010/5/5 23:14:10**/
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
				sMoDiscnt.pn_ac_no );
	if( ret == 0 )
	{
		/* ��������տ��˻������������������˺������Ƿ���� */
		if( sMoDiscnt.ac_type[0] != '1' )
		{
			vtcp_log( "[%s][%d] lock sMoDiscnt.ac_type[%s] ",__FILE__,__LINE__, sMoDiscnt.ac_type );
			sprintf( acErrMsg, "�տ��˻�����ӦΪ����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B140" );
			goto ErrExit;
		}

		/* ���ݿͻ��ʺ�ID���ʻ���š��������ʻ�״̬Ϊ1����
			��ѯ���ڴ�����ļ� */
		ret =Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld \
				and ac_seqn=%d and ac_sts='%s'",\
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1" );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "�˿ͻ��ʺŷǻ��ڻ��ʻ�״̬������![%s]", sMoDiscnt.pn_ac_no );
       		WRITEMSG
       		strcpy( g_pub_tx.reply, "B124" );
			goto ErrExit;
   		}else if( ret )
   		{
   			sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
           	WRITEMSG
           	goto ErrExit;
   		}
   	}
	else if( ret != 100 )
   	{
   		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
   	else
	{
		/* ��������տ��˻������������������˺������Ƿ���� */
		if( sMoDiscnt.ac_type[0] == '1' )
		{
			sprintf( acErrMsg, "�տ��˻�����ӦΪ���ػ����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B141" );
			goto ErrExit;
		}
	}
vtcp_log( "5555555555   sMoDiscnt.par_amt[%f] ", sMoDiscnt.par_amt );
vtcp_log( "5555555555 sMoDiscnt.not_matr_date[%ld] ", sMoDiscnt.not_matr_date );
#endif/***end modify by ligl 2010/5/5 23:15:16**/

   /****��ֽƱ����� modif by xyy 2010-4-29 17:13:23 NEWDP***/
   /***add by ligl 2010/5/5 23:14:10**/
    if(sMoDiscnt.po_knd[0]=='P')
    {
        /* ��������������˺�Ϊ���е�,��������������˺���Ч��:�����ǻ������˻�״̬��������,�����������Ƿ����*/
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMoDiscnt.pn_ac_no);
        if (ret) {
            sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        else {
            /* ��������տ��˻������������������˺������Ƿ���� */
            vtcp_log("[%s][%d] MoDiscnt.ac_type=[%s]\n",__FILE__,__LINE__,sMoDiscnt.ac_type);
            if (sMoDiscnt.ac_type[0] != '1') {
                sprintf(acErrMsg, "�տ��˻�����ӦΪ����!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "B140");
                goto ErrExit;
            }
            /* ���ݿͻ��ʺ�ID���ʻ���š��������ʻ�״̬Ϊ1���� ��ѯ���ڴ�����ļ� */
            ret = Dd_mst_Sel(g_pub_tx.reply, &sDdMst, "ac_id=%ld \
                    and ac_seqn=%d and ac_sts='%s'", sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1");
            if (ret == 100) {
                sprintf(acErrMsg, "�˿ͻ��ʺŷǻ��ڻ��ʻ�״̬������![%s]", sMoDiscnt.pn_ac_no);
                WRITEMSG
                strcpy(g_pub_tx.reply, "B124");
                goto ErrExit;
            }
            else if (ret) {
                sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret);
                WRITEMSG
                goto ErrExit;
            }
            if(memcmp(sMoDiscnt.pn_name , sDdMst.name ,sizeof(sMoDiscnt.pn_name)-1)!=0)
            {
                sprintf(acErrMsg, "��������![%s][%s]", sMoDiscnt.pn_name,sDdMst.name);
                WRITEMSG
                strcpy(g_pub_tx.reply, "NXD5");
                goto ErrExit;
            }
        }
    }
    else if (sMoDiscnt.po_knd[0]=='E')/***��Ʊҵ��Ҳ���������***/
    {
	Mo_discnt_Debug(&sMoDiscnt);
    		get_zd_data( "0750", sMoDiscnt.pnote_no );		/* Ʊ�ݺ� */

        if(sMoDiscnt.ac_type[0]=='1')/***��Ʊ�Ļ�Ҫ��������˻�,�����˻�ʱ***/
        {
             memset(&sMdmAcRel , 0 , sizeof(struct mdm_ac_rel_c));
             ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",cTmpAcno);
             if (ret) {
                 sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret);
                 WRITEMSG
                 goto ErrExit;
             }
             else {
                 /* ��������տ��˻������������������˺������Ƿ���� */
                 vtcp_log("[%s][%d] MoDiscnt.ac_type=[%s]\n",__FILE__,__LINE__,sMoDiscnt.ac_type);
                 
                 /* ���ݿͻ��ʺ�ID���ʻ���š��������ʻ�״̬Ϊ1���� ��ѯ���ڴ�����ļ� */
                 memset(&sDdMst , 0 , sizeof(struct dd_mst_c));
                 ret = Dd_mst_Sel(g_pub_tx.reply, &sDdMst, "ac_id=%ld \
                         and ac_seqn=%d and ac_sts='%s'", sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1");
                 if (ret) {
                     sprintf(acErrMsg, "���ʺŷǻ��ڻ��ʻ�״̬������![%s]", cTmpAcno);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "B124");
                     goto ErrExit;
                 }
             }
        }
        else
        {
             memset(&sMdmAcRel , 0 , sizeof(struct mdm_ac_rel_c));
             ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",cTmpAcno);
             if (ret) {
                 sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret);
                 WRITEMSG
                 goto ErrExit;
             }
             else {
                 /* ��������տ��˻������������������˺������Ƿ���� */
                 vtcp_log("[%s][%d] MoDiscnt.ac_type=[%s]\n",__FILE__,__LINE__,sMoDiscnt.ac_type);
                 
                 /* ���ݿͻ��ʺ�ID���ʻ���š��������ʻ�״̬Ϊ1���� ��ѯ���ڴ�����ļ� */
                 memset(&sInMst , 0 , sizeof(struct in_mst_c));
                 /*
                 ret = In_mst_Sel(g_pub_tx.reply, &sInMst, "ac_id=%ld \
                         and ac_seqn=%d and sts='%s'", sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1");
                         */
                 ret = Dd_mst_Sel(g_pub_tx.reply, &s_DdMst, "ac_id=%ld \
                         and ac_seqn=%d and ac_sts='%s'", sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1");
                 if (ret) {
                     sprintf(acErrMsg, "���ʺŷ��ڲ����ʻ�״̬������![%s]", cTmpAcno);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "B124");
                     goto ErrExit;
                 }
            }
        }
    }
    /***end by ligl 2010/5/5 23:14:10**/
	/* ����������Ϣ�����У����������� Ʊ�ݵ����� -��������+ ��;����,
		������Ϣ �� ��Ʊ��� * �������� * ����������/30��/1000���� */
	ret = pub_base_CalDays( sMoDiscnt.pn_date, sMoDiscnt.not_matr_date, "0", &lPnDays );
	if( ret )
	{
		sprintf( acErrMsg, "��������������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}

	sMoDiscnt.pn_int = sMoDiscnt.par_amt * (lPnDays + sMoDiscnt.way_days) * (sMoDiscnt.pn_rate/30/1000);
vtcp_log( "666666666   sMoDiscnt.pn_int[%f] ", sMoDiscnt.pn_int );
	if(sMoDiscnt.po_knd[0]=='E')
	get_zd_double("1001",&sMoDiscnt.pn_int);/***add by ligl 2010/5/5 23:14:10 ��ʵ��ʵ����ϢΪ׼**/
	set_zd_double( "1001", sMoDiscnt.pn_int );		/* ������Ϣ */

	/* �Ǽ����ֵǼǱ�,״̬Ϊ¼��,��������Ϊ"����" */
if(sMoDiscnt.po_knd[0]=='E')
	strcpy( sMoDiscnt.sts, "2" );				/* ��־��1¼�� ***add by ligl 2010/5/5 23:14:10 ��Ϊ���**/
	else
	strcpy( sMoDiscnt.sts, "1" );				/* ��־��1¼�� ***add by ligl 2010/5/5 23:14:10 ��Ϊ���**/
		strcpy( sMoDiscnt.pn_type, "1" );			/* ��������: 1���� */
	sMoDiscnt.tranx_bal = sMoDiscnt.par_amt;		/* �����˻���� */
	if( cOperCode[0] == '1'  )				/* �޸� */
	{
		pub_base_strpack( sMoDiscnt1.pact_no );
sprintf( acErrMsg, "__zxxxyu__sMoDiscnt.pact_no=[%s],sMoDiscnt1.pact_no=[%s]",\
		sMoDiscnt.pact_no, sMoDiscnt1.pact_no );
WRITEMSG
		if( strcmp( sMoDiscnt.pact_no, sMoDiscnt1.pact_no ) != 0 )
		{
			sprintf( acErrMsg, "����Э���Ų������޸�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B239" );
			goto ErrExit;
		}
	}

	Mo_discnt_Debug(&sMoDiscnt);
	ret = Mo_discnt_Ins( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
   	{
       	sprintf( acErrMsg, "ִ��Mo_discnt_Ins����!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}

vtcp_log( "666666666   sMoDiscnt.pn_int[] ");
	/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy( g_pub_tx.brf, "������Ϣ¼��" );
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

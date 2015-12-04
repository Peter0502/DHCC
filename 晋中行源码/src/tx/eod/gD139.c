#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
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
#include "intst_hst_c.h"
#include "com_sys_parm_c.h"
#include "com_sect_acm_c.h"


gD139()
{
	int ret=0;

	vtcp_log("��ʼ��Ϣ-----------------");

    ret=sql_begin(); /*������*/
	MYSQLERR

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();

	ret=dd_mst_cal();
	if( ret ) goto ErrExit;

	/**���˻���Ϣ*---hao---*���˺Ų���Ϣ*
	ret=sub_dd_mst_cal();
	if( ret ) goto ErrExit;
	***/

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�Թ����ڻ���Ϣ����ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G010");
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�Թ����ڻ���Ϣ����ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int dd_mst_cal()
{
	int	mouth = 0;
	struct dd_mst_c dd_mst_c;
	struct dd_parm_c dd_parm_c;
    struct intst_hst_c intst_hst_c;
    struct com_sys_parm_c com_sys_parm;
	struct com_sect_acm_c com_sect_acm_c;
	int ret=0;
	int i=0;

    memset(&dd_mst_c,0x0,sizeof(struct dd_mst_c));
    memset(&dd_parm_c,0x0,sizeof(struct dd_parm_c));

	ret=Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	MYSQLERR
	g_pub_tx.tx_date=com_sys_parm.sys_date;
	mouth=g_pub_tx.tx_date%10000/100;

	/**�Ǹ��˻�**/
    ret = Eod_Dd_mst_Dec_Upd(g_pub_tx.reply,
		"ac_type!='5' and ac_type!='7' and ac_sts in('0','1') and ac_id in(select ac_id from mo_spec_intst)");
	MYSQLERR

    while(1)
    {
        memset(&dd_mst_c,0x0,sizeof(struct dd_mst_c));

        ret = Eod_Dd_mst_Fet_Upd(&dd_mst_c, g_pub_tx.reply);
        if (ret == 100) break;
		MYSQLERR

		vtcp_log("DD_MST[%d]ac_id[%d][%d]intype[%s],mon[%d]",
			++i,dd_mst_c.ac_id,dd_mst_c.ac_seqn,dd_mst_c.intst_type,mouth);

		/*0������Ϣ��1�����汾�壻2�����ռ�Ϣ��*/
		if( dd_mst_c.intst_type[0]=='0' )
			continue;
		
		/***XXXX
		if( dd_mst_c.intst_type[0]=='1' 
			|| dd_mst_c.intst_type[0]=='2' )
			continue;
		********/

		/**���ủ**
		if( dd_mst_c.intst_type[0]=='4' && mouth!=3 && mouth!=6 
			&& mouth!=9 && mouth!=12 )
			continue;
		*********/

		/**��ủ**
		if( dd_mst_c.intst_type[0]=='5' && mouth!=12 )
			continue;
		**************/
vtcp_log("BJLX+++++++++++++++++++++++++++++[%d]",dd_mst_c.ac_id);

        memset(&dd_parm_c,0x0,sizeof(struct dd_parm_c));
        strcpy(dd_parm_c.prdt_no,dd_mst_c.prdt_no);

        ret=Dd_parm_Sel(g_pub_tx.reply,&dd_parm_c,"prdt_no='%s' ", 
				dd_parm_c.prdt_no );
        if ( ret ) vtcp_log("���ڲ�Ʒ[%s] ret[%d]",dd_parm_c.prdt_no,ret );

        /* ������� */
        if( pub_base_CalAcm(dd_mst_c.lst_date,g_pub_tx.tx_date,
            dd_parm_c.intst_term_type,dd_parm_c.acm_calc_type,
            dd_mst_c.bal,0.00,&dd_mst_c.intst_acm,
            dd_mst_c.intst_type,dd_parm_c.intst_mon,dd_parm_c.intst_day) )
        {
            sprintf(acErrMsg,"������Ϣ��������!");
            WRITEMSG
            goto ErrExit;
        }

		/******����ֶλ���begin*****/
    	ret = Com_sect_acm_Dec_Upd(g_pub_tx.reply,
			"ac_id=%d and ac_seqn=%d and sts='0'",
			dd_mst_c.ac_id,dd_mst_c.ac_seqn );
		MYSQLERR
		while(1)
		{
        	ret = Com_sect_acm_Fet_Upd(&com_sect_acm_c, g_pub_tx.reply);
			if( ret==100 ) break;
			MYSQLERR

			memset(&intst_hst_c,0,sizeof(intst_hst_c));
        	intst_hst_c.bic_acm = com_sect_acm_c.intst_acm;
        	intst_hst_c.ic_date = com_sect_acm_c.beg_date;
			intst_hst_c.intst_date=com_sect_acm_c.end_date;
        	strcpy(intst_hst_c.br_no,dd_mst_c.opn_br_no);
        	intst_hst_c.ac_id = com_sect_acm_c.ac_id;
        	intst_hst_c.ac_seqn = com_sect_acm_c.ac_seqn;
        	strcpy(intst_hst_c.prdt_no,dd_parm_c.prdt_no);
        	strcpy(intst_hst_c.cur_no,dd_parm_c.cur_no);
        	intst_hst_c.bic_bal = dd_mst_c.bal;
			/****Ҫ��Ϊ����****/
        	intst_hst_c.fct_intst = pub_base_PubDround_1(com_sect_acm_c.intst_acm*com_sect_acm_c.rate/30);
        	intst_hst_c.intst_tax = pub_base_PubDround_1(com_sect_acm_c.intst_acm*com_sect_acm_c.rate/30)*0.2;
        	intst_hst_c.aic_bal = dd_mst_c.bal + intst_hst_c.fct_intst-intst_hst_c.intst_tax;
 	       	intst_hst_c.aic_acm = dd_mst_c.intst_acm;
    	    strcpy(intst_hst_c.ac_ind,"0");
        	strcpy(intst_hst_c.pt_ind,"0");
            intst_hst_c.rate=com_sect_acm_c.rate;
        	if (pub_base_CompDblVal(intst_hst_c.fct_intst,0.00) != 0)
        	{
            	ret = Intst_hst_Ins(intst_hst_c, g_pub_tx.reply);
				MYSQLERR
            }
			com_sect_acm_c.sts[0]='1';
        	ret = Com_sect_acm_Upd_Upd( com_sect_acm_c, g_pub_tx.reply);
			MYSQLERR
		}
		Com_sect_acm_Clo_Upd();
		/******����ֶλ���over*****/
         
        memcpy(&g_dd_mst,&dd_mst_c,sizeof(struct dd_mst_c));
        memcpy(&g_dd_parm,&dd_parm_c,sizeof(struct dd_parm_c));
		memset(&intst_hst_c,0,sizeof(intst_hst_c));
        g_pub_tx.ac_id_type[0] = '1';
        intst_hst_c.bic_acm = dd_mst_c.intst_acm;

        ret = pub_base_CalIntst(&g_pub_intst);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������Ϣ����!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A032");
            goto ErrExit;
        }
        vtcp_log("��Ϣ=[%.2lf]��Ϣ˰=[%.2lf]",
					g_pub_intst.factval,g_pub_intst.val);

        dd_mst_c.intst_acm = 0.00;
 
        /* ������Ϣ���� */
        if( pub_base_CalAcm(dd_mst_c.lst_date,g_pub_tx.tx_date,
            dd_parm_c.intst_term_type,dd_parm_c.acm_calc_type,
            0.00,dd_mst_c.bal,&dd_mst_c.intst_acm,
            dd_mst_c.intst_type,dd_parm_c.intst_mon,dd_parm_c.intst_day) )
        {
            sprintf(acErrMsg,"������Ϣ��������!");
            WRITEMSG
            goto ErrExit;
        }
        sprintf(acErrMsg,"������ϢǰOK33 intst_acm=[%lf] ",dd_mst_c.intst_acm);
        WRITEMSG

        intst_hst_c.ic_date = dd_mst_c.ic_date;
		intst_hst_c.intst_date=com_sys_parm.sys_date;

  		dd_mst_c.ic_date=g_pub_tx.tx_date; 
        dd_mst_c.lst_date=g_pub_tx.tx_date;

        ret = Eod_Dd_mst_Upd_Upd(dd_mst_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"���»����ʻ����ļ�����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        
        /* д�Թ���Ϣ�Ǽǲ� */
        strcpy(intst_hst_c.br_no,dd_mst_c.opn_br_no);
        intst_hst_c.ac_id = dd_mst_c.ac_id;
        intst_hst_c.ac_seqn = dd_mst_c.ac_seqn;
        strcpy(intst_hst_c.prdt_no,dd_parm_c.prdt_no);
        strcpy(intst_hst_c.cur_no,dd_parm_c.cur_no);
        intst_hst_c.bic_bal = dd_mst_c.bal;
        intst_hst_c.fct_intst = g_pub_intst.factval;
        intst_hst_c.intst_tax = g_pub_intst.val;
        intst_hst_c.aic_bal = dd_mst_c.bal + g_pub_intst.factval;
        intst_hst_c.aic_acm = dd_mst_c.intst_acm;
        strcpy(intst_hst_c.ac_ind,"0");
        strcpy(intst_hst_c.pt_ind,"0");

        ret = pub_base_getllz(dd_parm_c.rate_no,dd_parm_c.cur_no,
                               g_pub_tx.tx_date,&intst_hst_c.rate);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"W110");
            goto ErrExit;
        }
		/*--- hao --- @20050918@ ---- ��Ϣ�������Ӹ������� ----*/
		intst_hst_c.rate=intst_hst_c.rate*(1+g_dd_mst.flt_ratio);
		/*-----------------------------------------------------*/

        if (pub_base_CompDblVal(g_pub_intst.dealval,0.00) != 0)
        {
            ret = Intst_hst_Ins(intst_hst_c, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"���Ϣ�Ǽǲ��в����¼����!![%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
        }
    }
    Eod_Dd_mst_Clo_Upd();

OkExit:
	return 0;
ErrExit:
	return 1;
}

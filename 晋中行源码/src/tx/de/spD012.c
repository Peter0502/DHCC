/***************************************************************
* �� �� ��:     spD012.c
* ����������	��Ϣ����(�����ʻ�)
*
* ��    ��:     jack
* ������ڣ�    2004��1��10��
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

spD012()
{
    double dealval;
    double val;
    double factval;
    double keepval;
    int flag;
    int year,month,day;
   
	 /*��սṹ*/
    if (init_struct())
    {
        strcpy(acErrMsg,"�ṹ��ճ���!");
        WRITEMSG
        goto ErrExit;
    }

    /*** ��ʼ��ȫ�ֱ��� ***/
    if (pub_base_sysinit())
    {
        strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
        WRITEMSG
        goto ErrExit;
    }
    
    /*ȡֵ����ֵ*/
    if (getpm_fun())
    {
        strcpy(acErrMsg,"ȡֵ����ֵ����!");
        WRITEMSG
        goto ErrExit;
    }
    
    sprintf(acErrMsg,"ac_no=[%s] ac_seqn =[%d],tx_date=[%ld]!! ",g_pub_tx.ac_no,g_pub_tx.ac_seqn,g_pub_tx.tx_date);
    WRITEMSG
    
    if (pub_base_disac_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,g_pub_tx.ac_type))
    {
        strcpy(acErrMsg,"ȡ���ļ��Ͳ�Ʒ��������!!");
        WRITEMSG
        goto ErrExit;
    }

    if (g_pub_tx.ac_id_type[0] == '2')
    { /* ���� */
        g_pub_tx.tx_amt1 = g_td_mst.bal;
		g_pub_intst.edu_ind[0] = 'Y';

		/*** ���ü�����Ϣ�����ĺ��� **** ������ȡ �������� ��������*/ 
		if( g_td_parm.td_type[0] == '2' || g_td_parm.td_type[0] == '1' || 
			g_td_parm.td_type[0] == '7' ) 
		{ 
			if( pub_base_CalAcm(g_td_mst.lst_date,
				g_pub_tx.tx_date>g_td_mst.mtr_date?g_td_mst.mtr_date:g_pub_tx.tx_date, g_td_parm.intst_term_type,g_td_parm.acm_calc_mode, g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm, g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) ) 
			{ 
				sprintf(acErrMsg,"������Ϣ��������!"); 
				WRITEMSG 
				goto ErrExit; 
			} 
		}

        sprintf(acErrMsg,"g_td_parm.intst_term_type=[%s]!!",g_td_parm.intst_term_type);
        WRITEMSG
		dealval = 0.00;
		factval = 0.00; 
		val = 0.00;
		keepval = 0.00;

        if (pub_base_CalIntstTd(&dealval,&factval,&val,&keepval,g_pub_tx.reply))
        {
            strcpy(acErrMsg,"���㶨����Ϣ����!!!");
            WRITEMSG
            goto ErrExit;
        }
    }
    else if (g_pub_tx.ac_id_type[0] == '1')
    { /* ���� */
        g_pub_tx.tx_amt1 = g_dd_mst.bal;

        sprintf(acErrMsg,"���ڼ�Ϣ!!");
        WRITEMSG

        flag = 1; /* ������Ϣ */
        year = floor(g_pub_tx.tx_date / 10000);
        month = floor((g_pub_tx.tx_date - year * 10000) / 100);
        day = g_pub_tx.tx_date - year * 10000 - month * 100;

        if ((month == 6) && (day == 30))
        {
           flag = 2; /* ��Ϣ�ռ�Ϣ */
        }

		if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date, 
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type, 
				g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm, 
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) ) 
		{ 
			sprintf(acErrMsg,"������Ϣ��������!"); 
			WRITEMSG 
			goto ErrExit; 
		}
         
        if (pub_base_CalIntstHQ(flag,&dealval,&factval,&val,&keepval,g_pub_tx.reply))
        {
            strcpy(acErrMsg,"���������Ϣ����!!!");
            WRITEMSG
            goto ErrExit;
        }
      
    }
    else
    {
       sprintf(acErrMsg,"���˻����ܲ��Ƕ��ڻ�����ʻ�,���ܼ���");
       WRITEMSG
       strcpy(g_pub_tx.reply,"");
       goto ErrExit;    
    }

    sprintf(acErrMsg,"g_pub_tx.tx_amt1=[%lf]!!",g_pub_tx.tx_amt1);
    WRITEMSG
    
    sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf]!!",dealval,factval,val);
    WRITEMSG
    
	/*** ������� ***/
    set_zd_double("BJ00",g_pub_tx.tx_amt1);
    set_zd_double("102P",dealval);
    set_zd_double("102Q",val);
    set_zd_double("102R",keepval);
    set_zd_double("102S",g_pub_tx.tx_amt1 + factval + keepval);

	sql_rollback();
	sql_begin();
    
OkExit:
	strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg," �����ʻ���Ϣ�ɹ�!! ");
    WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"�����ʻ���Ϣ���ɹ�!!");
    WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int init_struct()
{
    memset(&g_pub_tx,0x0,sizeof(struct pub_tx));
    memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
    memset(&g_td_parm,0x0,sizeof(struct td_parm_c));
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_dd_parm,0x0,sizeof(struct dd_parm_c));

    return 0;
}
int getpm_fun()
{
    char tmp[20];
    memset(tmp,0x0,sizeof(tmp));
    get_zd_data("0310",g_pub_tx.ac_no);
    get_zd_int("0340",&g_pub_tx.ac_seqn);
    get_zd_long("0440",&g_pub_tx.tx_date);
    return 0;
}

/*************************************************************
* �� �� ��: gD811.c
* ����������һ��ͨ����Ϣ����
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
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "com_eod_exec_c.h"
#include "intst_hst_c.h"

struct com_sys_parm_c com_sys_parm;
extern struct com_eod_exec_c grz_com_eod_exec;/*ȫ��,�������ӳ��򴫵ݿ�������*/

gD811()
{
	  int ret = 0;
    struct dd_mst_c dd_mst_c;
    struct dd_mst_c o_dd_mst;
    struct dd_parm_c dd_parm_c;
    struct com_branch_c com_branch_c;
	  struct S_pub_intst sum_intst;
	  long first=0;
    long days=0;
    memset(&o_dd_mst,0x0,sizeof(o_dd_mst));
    memset(&dd_mst_c,0x0,sizeof(struct dd_mst_c));
    memset(&g_pub_tx,0x0,sizeof(g_pub_tx));
    memset(&dd_parm_c,0x0,sizeof(struct dd_parm_c));
    memset(&g_pub_intst,0x0,sizeof(g_pub_intst));
    memset(&sum_intst,0x0,sizeof(sum_intst));

	show_log( "-----------begin--------------");

    ret=sql_begin(); /*������*/
	MYSQLERR

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
		if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
			sprintf(acErrMsg,"ȡ������ˮ�Ŵ� [%d]",g_pub_tx.trace_no);
			WRITEMSG
			goto ErrExit;
	}
	set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
	set_zd_long( DC_TRACE_CNT,1 );
	ret=pub_base_GetSysparm(&com_sys_parm);
	MYERR
	g_pub_tx.tx_date=com_sys_parm.sys_date;

	/* 5������� 7���˹�������� substr(spe_ind,2,1)='0'�����Ϣ��־0��δ�� */
    ret = Eod_Dd_mst_Dec_Sel(g_pub_tx.reply,
	"prdt_no='103' and ac_sts='1' and opn_br_no in(select br_no from com_branch where substr(spe_ind,2,1)='0') order by opn_br_no,prdt_no ");
	MYSQLERR

    while(1)
    {
        days=0;
        memset(&dd_mst_c,0x0,sizeof(struct dd_mst_c));
        ret = Eod_Dd_mst_Fet_Sel(&dd_mst_c, g_pub_tx.reply);
		if( ret==100 ) break;
		MYSQLERR
    pub_base_CalDays(dd_mst_c.ic_date,g_pub_tx.tx_date,"0",&days);
    if(days!=7)
    {
    	continue;
    }
		if( first==0 )
        	memcpy(&o_dd_mst,&dd_mst_c,sizeof(dd_mst_c));

		show_log( "[%d]-[%s][%d][%d]",first+1,dd_mst_c.opn_br_no,
			dd_mst_c.ac_id,dd_mst_c.ac_seqn );

		if( strcmp(dd_mst_c.opn_br_no,o_dd_mst.opn_br_no) ||
			strcmp(dd_mst_c.prdt_no,o_dd_mst.prdt_no) )
		{
			ret=sub_intst_acct(o_dd_mst,dd_parm_c,sum_intst);
			MYERR
    		memset(&sum_intst,0x0,sizeof(sum_intst));
		}
		if( strcmp(dd_mst_c.opn_br_no,o_dd_mst.opn_br_no) )
		{
			ret=Com_branch_Dec_Upd(g_pub_tx.reply,"br_no='%s'",
					o_dd_mst.opn_br_no);
			MYSQLERR
			ret=Com_branch_Fet_Upd(&com_branch_c,g_pub_tx.reply);
			MYSQLERR
			com_branch_c.spe_ind[1]='1';
			ret=Com_branch_Upd_Upd(com_branch_c,g_pub_tx.reply);
			MYSQLERR
			Com_branch_Clo_Upd();
			sql_commit();
			sql_begin();
		}
		if( first==0 || strcmp(dd_mst_c.opn_br_no,o_dd_mst.opn_br_no) ||
			strcmp(dd_mst_c.prdt_no,o_dd_mst.prdt_no) )
		{
        	strcpy(dd_parm_c.prdt_no,dd_mst_c.prdt_no);
        	ret = Dd_parm_Sel( g_pub_tx.reply,&dd_parm_c,"prdt_no='%s'", 
					dd_parm_c.prdt_no );
        	if ( ret )
        	{
            	sprintf(acErrMsg,"��ѯ���ڲ�Ʒ�����[%s]!!!",dd_mst_c.prdt_no);
            	WRITEMSG
            	goto ErrExit;
        	}
		}

		if( pub_base_CompDblVal(dd_mst_c.bal,0.00)<0 )
		{
        	memcpy(&o_dd_mst,&dd_mst_c,sizeof(dd_mst_c));
			first++;
			vtcp_log("FFFFFFFFFFFFFFFFFFFFFF");
			continue;
		}
		ret=sub_cal_intst(dd_mst_c,dd_parm_c);
		MYERR

		sum_intst.dealval+=g_pub_intst.dealval;
		sum_intst.val+=g_pub_intst.val;

        memcpy(&o_dd_mst,&dd_mst_c,sizeof(dd_mst_c));
		first++;
    }
    Eod_Dd_mst_Clo_Sel();
		
	if( first )
	{
		ret=sub_intst_acct(o_dd_mst,dd_parm_c,sum_intst);
		MYERR
	}

	set_zd_data(DC_CODE,"");
	set_zd_double("102P",0.00 );
	set_zd_double("102Q",0.00 );
    set_zd_data("102J","");
    memset(&g_pub_intst,0x0,sizeof(g_pub_intst));
	ret=sql_execute("update com_branch set spe_ind=substr(spe_ind,1,1)||'0'||substr(spe_ind,3,6) where 1=1");
	MYERR

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��˽���ڻ���Ϣ����ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��˽���ڻ���Ϣ����ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
int sub_cal_intst( dd_mst_c,dd_parm_c )
 struct dd_mst_c dd_mst_c;
 struct dd_parm_c dd_parm_c;
{
	long caldate,tmp_end_date=0;
	int ret=0,tmpday=0;
	double tmpacm=0.00,tmpbal=0.00;
	char tmptype[2];
	struct intst_hst_c	T_intst_hst;
	struct dd_mst_c tmp_dd_mst;
	double rate_val=0.00;
	double sdsl=0.00;
	double dealval=0.00;
	double val=0.00,factval=0.00,lx=0.00;
	memset(&T_intst_hst,0x00,sizeof(struct intst_hst_c));

    memset( &g_pub_intst,0x0,sizeof(g_pub_intst));
	memcpy( &g_dd_mst,&dd_mst_c,sizeof(g_dd_mst) );
	memcpy( &g_dd_parm,&dd_parm_c,sizeof(g_dd_parm) );
	strcpy( g_pub_tx.ac_no,"" );
	g_pub_tx.ac_id=g_dd_mst.ac_id ;
	g_pub_tx.ac_seqn=g_dd_mst.ac_seqn ;
	strcpy( g_pub_tx.cur_no,g_dd_parm.cur_no );
	strcpy( g_pub_tx.prdt_code,g_dd_parm.prdt_no );
	strcpy( g_pub_tx.ct_ind,"2" );

	if( grz_com_eod_exec.do_type[0]=='0' )
		pub_base_deadlineD(g_pub_tx.tx_date,1,&caldate);
	else
		caldate=g_pub_tx.tx_date;

	/***����ֶ���Ϣ*����**/
        /* ������� */
	/*---- hao ---- @20050915@ ---- ������ʱ������Ļ����ٳ� ----*/
	if( caldate==20050921 )
	{
		tmp_end_date=20060701;
		tmpbal=g_dd_mst.bal;
		tmpacm=pub_base_CalTriDays(g_dd_mst.lst_date,tmp_end_date)*floor(tmpbal);
		vtcp_log("tmpday=[%ld],tmpacm=[%lf],ac_id=[%ld],bal=[%lf]",pub_base_CalTriDays(g_dd_mst.lst_date, tmp_end_date),tmpacm,g_dd_mst.ac_id,tmpbal);
		g_dd_mst.intst_acm=g_dd_mst.intst_acm-tmpacm;
		if( pub_base_CompDblVal(g_dd_mst.intst_acm,0.00)<0 )
		{
			g_dd_mst.intst_acm=0.00;
		}
	}
	if( caldate==20050921 )
	{
		/** 20050921��ǰ��ÿ�£������Ϣ **/
		strcpy(tmptype,"1");
        if( pub_base_CalAcm(g_dd_mst.lst_date,caldate,tmptype,g_dd_parm.acm_calc_type,g_dd_mst.bal,0.00,&g_dd_mst.intst_acm,g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
        {
            sprintf(acErrMsg,"������Ϣ��������!");
            WRITEMSG
            goto ErrExit;
        }
    }
	else
	{
	/*-----------------------------------------------------------*/
        if( pub_base_CalAcm(g_dd_mst.lst_date,caldate,
            g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
            g_dd_mst.bal,0.00,&g_dd_mst.intst_acm,
            g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
        {
            sprintf(acErrMsg,"������Ϣ��������!");
            WRITEMSG
            goto ErrExit;
        }
	}

        sprintf(acErrMsg,"������Ϣǰ intst_acm=[%lf]",g_dd_mst.intst_acm);
        WRITEMSG

        g_pub_tx.ac_id_type[0] = '1';
        /**ȡ���ʱ��**/
        ret = pub_base_getllz("B00",g_dd_parm.cur_no,g_pub_tx.tx_date,&rate_val);
        if (ret != 0)
        {
                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                WRITEMSG
                goto ErrExit;
       }
       lx = rate_val * g_dd_mst.intst_acm * (1 + g_dd_mst.flt_ratio) / L360 / L100;            
       if (strcmp(g_dd_parm.tax_no,SLBH) == 0)
       {
           ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
           if (ret != 0 && ret == 1)
           {
              sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",ret);
              WRITEMSG
              goto ErrExit;
          }
       else if (ret < 0)
       {
              sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d",ret);
              WRITEMSG
              goto ErrExit;
        }
        g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
        val = lx * sdsl;
       }
       dealval= pub_base_PubDround_1(lx);
       val= pub_base_PubDround_1(val);
       factval = dealval - val;
       g_pub_intst.dealval = dealval;
    	 g_pub_intst.factval = factval;
       g_pub_intst.val = val;
       g_pub_intst.keepval = 0.00;
       /* ret = pub_base_CalIntst(&g_pub_intst);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������Ϣ����!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A032");
            goto ErrExit;
        }*/
        sprintf(acErrMsg,"��ϢΪ=[%.2lf] ��Ϣ˰=[%.2lf]",
                g_pub_intst.dealval, g_pub_intst.val);
        WRITEMSG

        /* ������Ϣ���� *�Ժ����������
        if( pub_base_CalAcm(g_dd_mst.lst_date,caldate,
            g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
            0.00,g_dd_mst.bal,&g_dd_mst.intst_acm,
            g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
        {
            sprintf(acErrMsg,"������Ϣ��������!");
            WRITEMSG
            goto ErrExit;
        }
		**************/

        sprintf(acErrMsg,"������Ϣ�� intst_acm=[%lf] ",g_dd_mst.intst_acm);
        WRITEMSG
		T_intst_hst.bic_bal=g_dd_mst.bal;
		T_intst_hst.bic_acm=g_dd_mst.intst_acm;
		T_intst_hst.fct_intst=g_pub_intst.dealval;
		T_intst_hst.intst_tax=g_pub_intst.val;
		T_intst_hst.ic_date=g_dd_mst.ic_date;

        g_dd_mst.intst_acm = 0.00;
        dd_mst_c.intst_acm = 0.00;
		strcpy( g_pub_tx.ac_wrk_ind,"0200000J1" );
		g_pub_tx.svc_ind=1012;
		strcpy( g_pub_tx.hst_ind,"1" );
		strcpy( g_pub_tx.add_ind,"1" );
		strcpy( g_pub_tx.ac_get_ind,"011" );
		strcpy( g_pub_tx.brf,"��Ϣ" );
		g_pub_tx.tx_amt1=g_pub_intst.dealval;
        g_dd_mst.ic_date = caldate;
        g_dd_mst.lst_date = g_pub_tx.tx_date ;

		ret=pub_acct_trance_jx();
		MYERR
 
		strcpy( g_pub_tx.add_ind,"0" );
		g_pub_tx.tx_amt1=g_pub_intst.val;
		strcpy( g_pub_tx.brf,"��˰" );

		ret=pub_acct_trance_jx();
		MYERR

		ret=Dd_mst_Dec_Upd( g_pub_tx.reply ,"ac_id=%d and ac_seqn=%d" ,
			g_dd_mst.ac_id,g_dd_mst.ac_seqn );
		MYERR
		ret=Dd_mst_Fet_Upd( &tmp_dd_mst , g_pub_tx.reply );
		MYERR
		ret=Dd_mst_Upd_Upd( g_dd_mst , g_pub_tx.reply  );
		Dd_mst_Clo_Upd( );
		/*--- hao --- @20050915@ ---- �Ǽǽ�Ϣ�ǼǱ�(Ҫ�������ݿ��б�����ϸ) ----*/
		strcpy(T_intst_hst.br_no,g_dd_mst.opn_br_no);
		T_intst_hst.ac_id=g_dd_mst.ac_id;
		T_intst_hst.ac_seqn=g_dd_mst.ac_seqn;
		strcpy(T_intst_hst.prdt_no,g_dd_mst.prdt_no);
		strcpy(T_intst_hst.cur_no,g_dd_parm.cur_no);
		T_intst_hst.aic_bal=g_dd_mst.bal;
		T_intst_hst.aic_acm=g_dd_mst.intst_acm;
		T_intst_hst.intst_date=caldate;
		strcpy(T_intst_hst.ac_ind,"1");
		strcpy(T_intst_hst.pt_ind,"1");
		T_intst_hst.rate=g_dd_mst.rate;
		ret=Intst_hst_Ins(T_intst_hst,g_pub_tx.reply);
		MYERR
	
	return 0;
ErrExit:
	return 1;
}
int sub_intst_acct( dd_mst_c,dd_parm_c,sum_intst )
 struct dd_mst_c dd_mst_c;
 struct dd_parm_c dd_parm_c;
 struct S_pub_intst sum_intst ;
{
	int ret=0;
	FILE *fp1=NULL;
	char filename1[248];
	char tmp_cxamt[21],vstr[60];
	memset(filename1,0x00,sizeof(filename1));
	memset(tmp_cxamt,0x00,sizeof(tmp_cxamt));
	memset(vstr,0x00,sizeof(vstr));
	vtcp_log("���ܼ���[%s][%.2lf]",dd_parm_c.prdt_no,sum_intst.dealval);

	strcpy(g_pub_tx.tx_br_no,dd_mst_c.opn_br_no);
	strcpy(g_pub_tx.opn_br_no,dd_mst_c.opn_br_no);
	strcpy(g_pub_tx.cur_no,dd_parm_c.cur_no);
    strcpy(g_pub_tx.ac_wrk_ind,"0000000");
	g_pub_tx.svc_ind=1005;
	strcpy( g_pub_tx.ct_ind,"2" );
	strcpy( g_pub_tx.add_ind,"1" );
	strcpy( g_pub_tx.prdt_code,dd_parm_c.prdt_no );
	strcpy( g_pub_tx.ac_no,"" );
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	strcpy( g_pub_tx.note_type,"" );
	strcpy( g_pub_tx.beg_note_no,"" );
	g_pub_tx.tx_amt1=sum_intst.dealval;
    strcpy(g_pub_tx.brf,"��Ϣ");
	strcpy( g_pub_tx.no_show,"0" );

	/*** ���û�Ʒ�¼�Զ��� ***
		ret=pub_ins_trace_log();
		if( ret ) goto ErrExit;
	***/
        strcpy(g_pub_tx.add_ind,"0");
        strcpy(g_pub_tx.ac_wrk_ind,"0000000");
        strcpy(g_pub_tx.hst_ind,"1");
        g_pub_tx.svc_ind=1001;
        strcpy(g_pub_tx.brf,"��Ϣ˰");
        g_pub_tx.tx_amt1 = sum_intst.val;

	/*** ���û�Ʒ�¼�Զ��� ***
		ret=pub_ins_trace_log();
		if( ret ) goto ErrExit;
	***/

        /* ���л�Ƽ��� */
				set_zd_data(DC_CODE,dd_parm_c.dc_code);
				set_zd_double("102P",sum_intst.dealval );
				set_zd_double("102Q",sum_intst.val );
        set_zd_data("102J",dd_parm_c.cur_no);
       	strcpy(g_pub_tx.sub_tx_code,"Z032"); 
        strcpy(g_pub_tx.prdt_code,dd_mst_c.prdt_no);

        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit;
        }
        
			sprintf(filename1,"%s/report/%s/gD811.txt",getenv("HOME"),g_pub_tx.opn_br_no);
			fp1 = fopen( filename1,"w" );	/*���ļ�������¼��ȷ������*/
			if( fp1==NULL )
			{
					sprintf(acErrMsg," open file [%s] error ",filename1 );
					WRITEMSG
					strcpy( g_pub_tx.reply,"S047" );
	        return 1;
			}
			memset(tmp_cxamt,0x00,sizeof(tmp_cxamt));
			memset(vstr,0x00,sizeof(vstr));
			sprintf( tmp_cxamt, "%.2f" ,sum_intst.dealval);
  		pub_rpt_flttomon( tmp_cxamt,tmp_cxamt);	/* Сд��� */
			numtomoney(sum_intst.dealval, vstr);	/* ��д��� */
			fprintf(fp1, "                            һ��ͨ��Ϣת�˽跽��Ʊ\n");
  		fprintf(fp1,"\n");
		fprintf(fp1, "  ��    ��:%s         ��������: %ld         ��ˮ��: %ld        \n",dd_mst_c.name,g_pub_tx.tx_date,g_pub_tx.trace_no);			
  		fprintf(fp1, "�������������Щ����������������������������Щ��������Щ�������������������\n");
  		fprintf(fp1, "�� �� �� �� �� %5s                      ���跽��Ŀ��52103             ��\n",g_pub_tx.opn_br_no);
  		fprintf(fp1, "�������������੤���������������������������ة��������ة�������������������\n");
  		fprintf(fp1, "�� ��д��� �� %-21s                                    ��\n",vstr);
  		fprintf(fp1, "�������������੤����������������������������������������������������������\n");
  		fprintf(fp1, "�� Сд��� �� ��%-21s                                  ��\n",tmp_cxamt);
  		fprintf(fp1, "�������������੤�����������Щ���������������������������������������������\n");
  		fprintf(fp1, "�� ��    �� ��һ��ͨ��Ϣ  ��              ����:                         ��\n");
  		fprintf(fp1, "�������������ة������������ة���������������������������������������������\n");      
     	fprintf(fp1, "                 �º�ල:               ����:            �Ƶ�:           \n"); 
  		fprintf(fp1,"\n\n\n");
			memset(tmp_cxamt,0x00,sizeof(tmp_cxamt));
			memset(vstr,0x00,sizeof(vstr));
			sprintf( tmp_cxamt, "%.2f" ,sum_intst.dealval-sum_intst.val);
  		pub_rpt_flttomon( tmp_cxamt,tmp_cxamt);	/* Сд��� */
			numtomoney(sum_intst.dealval-sum_intst.val, vstr);	/* ��д��� */
			
			fprintf(fp1, "                              һ��ͨ��Ϣת�˴�����Ʊ\n");
  		fprintf(fp1,"\n");
		fprintf(fp1, "  ��    ��:%s         ��������: %ld         ��ˮ��: %ld        \n",dd_mst_c.name,g_pub_tx.tx_date,g_pub_tx.trace_no);			
  		fprintf(fp1, "�������������Щ����������������������������Щ��������Щ�������������������\n");
  		fprintf(fp1, "�� ������� �� %5s                      ��������Ŀ��21101             ��\n",g_pub_tx.opn_br_no);
  		fprintf(fp1, "�������������੤���������������������������ة��������ة�������������������\n");
  		fprintf(fp1, "�� ��д��� �� %-21s                                    ��\n",vstr);
  		fprintf(fp1, "�������������੤����������������������������������������������������������\n");
  		fprintf(fp1, "�� Сд��� �� ��%-21s                                  ��\n",tmp_cxamt);
  		fprintf(fp1, "�������������੤�����������Щ���������������������������������������������\n");
  		fprintf(fp1, "�� ��    �� ��һ��ͨ��Ϣ  ��              ����:                         ��\n");
  		fprintf(fp1, "�������������ة������������ة���������������������������������������������\n");      
     	fprintf(fp1, "                 �º�ල:               ����:            �Ƶ�:           \n");
  		fprintf(fp1,"\n\n\n");
			memset(tmp_cxamt,0x00,sizeof(tmp_cxamt));
			memset(vstr,0x00,sizeof(vstr));
			sprintf( tmp_cxamt, "%.2f" ,sum_intst.val);
  		pub_rpt_flttomon( tmp_cxamt,tmp_cxamt);	/* Сд��� */
			numtomoney(sum_intst.val, vstr);	/* ��д��� */
			
			fprintf(fp1, "                             һ��ͨ��Ϣת�˴�����Ʊ\n");
  		fprintf(fp1,"\n");
		fprintf(fp1, "  ��    ��:%s         ��������: %ld          ��ˮ��: %ld        \n",dd_mst_c.name,g_pub_tx.tx_date,g_pub_tx.trace_no);			
  		fprintf(fp1, "�������������Щ����������������������������Щ��������Щ�������������������\n");
  		fprintf(fp1, "�� ������� �� %5s                      ��������Ŀ��26800             ��\n",g_pub_tx.opn_br_no);
  		fprintf(fp1, "�������������੤���������������������������ة��������ة�������������������\n");
  		fprintf(fp1, "�� ��д��� �� %-21s                                    ��\n",vstr);
  		fprintf(fp1, "�������������੤����������������������������������������������������������\n");
  		fprintf(fp1, "�� Сд��� �� ��%-21s                                  ��\n",tmp_cxamt);
  		fprintf(fp1, "�������������੤�����������Щ���������������������������������������������\n");
  		fprintf(fp1, "�� ��    �� ��һ��ͨ��Ϣ  ��              ����:                         ��\n");
  		fprintf(fp1, "�������������ة������������ة���������������������������������������������\n");      
     	fprintf(fp1, "                 �º�ල:               ����:            �Ƶ�:           \n");
  		fprintf(fp1,"\n\n\n");
			fclose(fp1);
  return 0;
ErrExit:
	return 1;
}

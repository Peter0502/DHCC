/*************************************************************
* �� �� ��: gD808.c
* �����������Թ����ڵ�����Ϣ����
*
* ��    ��: jack
* �������: 2003��3��10��
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
gD808()
{
	int ret=0; double dTot_amt=0; /* by Wang Yongwei 20101010 */
	char ac_no[25];
	vtcp_log("��ʼ��Ϣ-----------------");
	memset(ac_no,0x00,sizeof(ac_no));
	get_zd_data("1021",ac_no);						/*�ʺ�*/
	vtcp_log("get_zd_data,ac_no[%s]-----------------",ac_no);
	pub_base_old_acno( ac_no );           /**�Ծ��ʺŵĴ���**/
	vtcp_log("pub_base_old_acno,ac_no[%s]-----------------",ac_no);
	int	mouth = 0;
	struct dd_mst_c dd_mst_c;
	struct dd_parm_c dd_parm_c;
  struct intst_hst_c intst_hst_c;
  struct com_sys_parm_c com_sys_parm;
	struct com_sect_acm_c com_sect_acm_c;
	struct mdm_ac_rel_c mdm_ac_rel_c;
	char cIntsttype[13];
	FILE *fp;
  char filename[100];
	int i=0;
	double sum_intst=0.00;
	double sum_tax=0.00;
	long day1=0;
	long true_days=0;  /****��Ϣ����(��ʵ��������)****/
	memset(&g_pub_tx, 0, sizeof(g_pub_tx));
	memset(cIntsttype,0x0,sizeof(cIntsttype));
	memset(filename,0x0,sizeof(filename));
  memset(&dd_mst_c,0x0,sizeof(struct dd_mst_c));
  memset(&dd_parm_c,0x0,sizeof(struct dd_parm_c));
  memset(&mdm_ac_rel_c,0x0,sizeof(struct mdm_ac_rel_c));
  memset(&com_sect_acm_c,0x0,sizeof(struct com_sect_acm_c));
  pub_base_sysinit();
  memset(g_pub_tx.ac_no,0x0,sizeof(g_pub_tx.ac_no));
  strcpy(g_pub_tx.ac_no,ac_no);
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_c,"ac_no='%s'",ac_no);
	MYSQLERR
	
  vtcp_log("[%s][%d][abcd==pub_tx.ac_no=========================[%s]]\n",__FILE__,__LINE__,ac_no);
	ret =Dd_mst_Sel(g_pub_tx.reply,&dd_mst_c,"ac_id=%ld",mdm_ac_rel_c.ac_id);
	MYSQLERR
  vtcp_log("[%s][%d][abcd===========================]\n",__FILE__,__LINE__);
	if(dd_mst_c.ac_sts[0]!='1')
	{
				sprintf(acErrMsg,"�ʺ�״̬������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P109");
        goto ErrExit;
	}
	if(dd_mst_c.ac_type[0]=='5'||dd_mst_c.ac_type[0]=='7')
	{
				sprintf(acErrMsg,"�ʺ�״̬������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P109");
        goto ErrExit;
	}
	/****������Ϣ�������˽���****/
	if(dd_mst_c.intst_type[0]=='0')	{
		sprintf(acErrMsg,"���ʺŲ���Ϣ!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"T100");
		goto ErrExit;
	}

  memset(&dd_parm_c,0x0,sizeof(struct dd_parm_c));
  strcpy(dd_parm_c.prdt_no,dd_mst_c.prdt_no);
  ret=Dd_parm_Sel(g_pub_tx.reply,&dd_parm_c,"prdt_no='%s' ",dd_parm_c.prdt_no );
  if (ret)
  {
  	 vtcp_log("���ڲ�Ʒ[%s] ret[%d]",dd_parm_c.prdt_no,ret );
 	}
 	/* ������� */
	/****
 	  day1=dd_mst_c.lst_date;
	****/
	/****��Ϊ����ic_dateΪ��Ϣ��, �����������Ϣ���� modify by wanglei 20071031****/
 	  day1=dd_mst_c.ic_date;
	true_days = pub_base_CalTrueDays(day1,g_pub_tx.tx_date);
	vtcp_log("[%s][%d] true_days = [%ld]\n",__FILE__,__LINE__,true_days);
	/****end 20071031****/
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
 	ret = Com_sect_acm_Dec_Upd(g_pub_tx.reply,"ac_id=%d and ac_seqn=%d and sts='0'",dd_mst_c.ac_id,dd_mst_c.ac_seqn );
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
     	intst_hst_c.fct_intst = pub_base_PubDround_1(com_sect_acm_c.intst_acm*com_sect_acm_c.rate/36000);
     	intst_hst_c.intst_tax = 0;
		dTot_amt += intst_hst_c.fct_intst-intst_hst_c.intst_tax; /* by Wang Yongwei 20101010 */

     	intst_hst_c.aic_bal = dd_mst_c.bal + intst_hst_c.fct_intst-intst_hst_c.intst_tax;
 	   	intst_hst_c.aic_acm = dd_mst_c.intst_acm;
    	strcpy(intst_hst_c.ac_ind,"0");
     	strcpy(intst_hst_c.pt_ind,"1");  /* by Wang Yongwei 20101010 */
	/* added by liuxuan 20071012 Э��������� */
	if(dd_mst_c.ac_type[0]=='6'){
		vtcp_log("[%s][%d]Э����ȡ�ֻ�������\n",__FILE__,__LINE__);
		com_sect_acm_c.rate=dd_mst_c.rate;
	}
	/* added over */
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
  vtcp_log("��Ϣ=[%.2lf]��Ϣ˰=[%.2lf]",g_pub_intst.factval,g_pub_intst.val);
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
  /* ��Ϣ���� ������ wudawei 20131114*/
	/** intst_hst_c.intst_date=com_sys_parm.sys_date; **/
	/* add end 20131114 wudawei*/
	intst_hst_c.intst_date=g_pub_tx.tx_date; 
	vtcp_log("[%s][%d]intst_hst_c.intst_date=[%ld], com_sys_parm.sys_date=[%ld]\n",__FILE__,__LINE__,intst_hst_c.intst_date,com_sys_parm.sys_date);
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
  strcpy(intst_hst_c.pt_ind,"1");  /* by Wang Yongwei 20101010 */
  ret = pub_base_getllz(dd_parm_c.rate_no,dd_parm_c.cur_no,g_pub_tx.tx_date,&intst_hst_c.rate);
  if (ret != 0)
  {
      sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
      WRITEMSG
      strcpy(g_pub_tx.reply,"W110");
      goto ErrExit;
  }
	/* added by liuxuan 20071012 Э��������� */
	if(dd_mst_c.ac_type[0]=='6'){
		vtcp_log("[%s][%d]Э����ȡ�ֻ�������\n",__FILE__,__LINE__);
		intst_hst_c.rate=dd_mst_c.rate;
	}
	/* added over */
  strcpy(g_pub_tx.prt_ind,"0" );
  strcpy(g_pub_tx.ct_ind,"2");
  strcpy(g_pub_tx.tx_br_no,intst_hst_c.br_no);
	strcpy(g_pub_tx.opn_br_no,intst_hst_c.br_no);
	strcpy(g_pub_tx.cur_no,intst_hst_c.cur_no);
  vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,g_pub_tx.ac_no);

  g_pub_tx.ac_id = intst_hst_c.ac_id;
  g_pub_tx.ac_seqn = intst_hst_c.ac_seqn;
  strcpy(g_pub_tx.ac_get_ind,"00");
  strcpy(g_pub_tx.ac_id_type,"1");
  strcpy(g_pub_tx.add_ind,"1");
  strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
	/****************************^����ÿ������ˮ**/
  strcpy(g_pub_tx.hst_ind,"1");
  g_pub_tx.svc_ind=1005;
	strcpy(g_pub_tx.brf,"��Ϣ");
	  dTot_amt += intst_hst_c.fct_intst-intst_hst_c.intst_tax; /* by Wang Yongwei 20101010 */
  g_pub_tx.tx_amt1 = dTot_amt;
  ret=pub_acct_trance();
  if( ret )
  {
      sprintf(acErrMsg,"����ڶԹ��ʻ�����Ϣ����!!");
      WRITEMSG
      goto ErrExit;
  }
  set_zd_data("101A",g_pub_tx.cur_no);
	set_zd_data("101B","2");
	set_zd_double("1013",dTot_amt);  /* by Wang Yongwei 20101010 */
  ret = pubf_acct_proc("D099");
	if( ret ) goto ErrExit;	
  if( intst_acct(intst_hst_c,dTot_amt,intst_hst_c.intst_tax))	 /* by Wang Yongwei 20101010 */
	{
			goto ErrExit;
  }
        ret = Intst_hst_Ins(intst_hst_c, g_pub_tx.reply); /* by Wang Yongwei 20101010 */
                MYSQLERR
/*
  ret = Intst_hst_Dec_Upd(g_pub_tx.reply,"ac_ind='0' and pt_ind='0'");
	MYSQLERR
	while(1)
	{
     	ret = Intst_hst_Fet_Upd(&intst_hst_c, g_pub_tx.reply);
     	if (ret == 100) break;
			MYSQLERR
     	strcpy(intst_hst_c.pt_ind,"1");
     	ret = Intst_hst_Upd_Upd(intst_hst_c,g_pub_tx.reply);
			MYSQLERR
	}
	Intst_hst_Clo_Upd();
*/
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"w" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	 memset(cIntsttype,0x0,sizeof(cIntsttype));
	 switch(dd_mst_c.intst_type[0])
    {
    	case  '0':
    					memcpy(cIntsttype,"����Ϣ",6);
    					break;
    	case  '1':
    					memcpy(cIntsttype,"���汾��",8);
    		      break;
    	case  '2':
							memcpy(cIntsttype,"���ռ�Ϣ",8);
    		      break;    	
    	case  '3':
							memcpy(cIntsttype,"���½�Ϣ",8);
    		      break;    	
    	case  '4':
							memcpy(cIntsttype,"������Ϣ",8);
    		      break; 
    	case  '5':
    					memcpy(cIntsttype,"�����Ϣ",8);
    		      break;   	
    	default:
    					break;
    }
	 fprintf(fp,"LXQD%s|%s|%ld|%ld|%ld|%.2lf|%.2lf|%s|%.5lf|\n",ac_no,dd_mst_c.name,day1,g_pub_tx.tx_date,true_days,dTot_amt,intst_hst_c.aic_acm,cIntsttype,intst_hst_c.rate);
	 fclose(fp);
	 set_zd_data(DC_FILE_SND,"1");
	set_zd_data("101A","");
	set_zd_data("101B","");
	set_zd_double("1013",0.00);
  set_zd_data("1204","");
  set_zd_data("1205","");
  set_zd_double("1208",0.00);  
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
int intst_acct( struct intst_hst_c o_intst,double sum_intst,double sum_tax )
{
	int ret=0;

	vtcp_log("���ܼ���[%s][%.2lf-%.2lf]",o_intst.prdt_no,sum_intst,sum_tax);
/*
	strcpy(g_pub_tx.tx_br_no,o_intst.br_no);
	strcpy(g_pub_tx.opn_br_no,o_intst.br_no);
  strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
	g_pub_tx.svc_ind=1005;
	strcpy( g_pub_tx.ct_ind,"2" );
	strcpy( g_pub_tx.add_ind,"1" );
	strcpy( g_pub_tx.prdt_code,o_intst.prdt_no );
	strcpy( g_pub_tx.ac_no,"" );
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	strcpy( g_pub_tx.note_type,"" );
	strcpy( g_pub_tx.beg_note_no,"" );
	g_pub_tx.tx_amt1=sum_intst-sum_tax;
	strcpy( g_pub_tx.no_show,"0" );
	ret=pub_ins_trace_log();
	if( ret ) goto ErrExit;
	set_zd_data("101A",o_intst.cur_no);
	set_zd_data("101B","2");
	set_zd_double("1013",sum_intst-sum_tax);
  ret = pubf_acct_proc("D099");
		if( ret ) goto ErrExit;	*/
	/*-----------------------------------------------------*/	
		strcpy(g_pub_tx.tx_br_no,o_intst.br_no);
		strcpy(g_pub_tx.opn_br_no,o_intst.br_no);
		get_zd_data("0153",g_pub_tx.ac_no);
    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.ac_get_ind,"00");
    strcpy(g_pub_tx.ac_id_type,"9");
    strcpy(g_pub_tx.add_ind,"0");		/** ���Ǽ��٣��ǽ跽 **/
    strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
    strcpy(g_pub_tx.hst_ind,"1");
    g_pub_tx.svc_ind=9001;
    g_pub_tx.tx_amt1 = sum_intst;
		strcpy(g_pub_tx.brf,"�Թ���Ϣ");
    ret=pub_acct_trance();
    if( ret )
    {
        sprintf(acErrMsg,"��Ϣ֧����Ŀ���˴���!!");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("1204",g_pub_tx.cur_no);
    set_zd_data("1205",g_pub_tx.ct_ind);
    set_zd_double("1208",g_pub_tx.tx_amt1);
    ret = pubf_acct_proc("A016");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
        WRITEMSG
        goto ErrExit;
    }
	/**-------�Թ���Ϣ˰��ʱ����---------------------------**/
	if( sum_tax>0.005 || sum_tax<-0.005 )
	{
		strcpy( g_pub_tx.reply,"AT03" );
		sprintf(acErrMsg,"SQLERR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

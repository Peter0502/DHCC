#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "fn_reg_c.h"
#include "com_sys_parm_c.h"
#include "fn_parm_c.h"
#include "fn_flt_radio_parm_c.h"




int splwb()
{
	
	struct fn_reg_c sFn_reg;
	struct fn_parm_c sFn_parm;
	struct fn_flt_radio_parm_c sFn_flt_radio_parm;
	int page = 0;
	int line = 0;
	char vpfm[21];
	FILE *fp = NULL;
	int ret = 0;
	char t_str[41];
	int ttlnum = 0;
	double sum = 0.0;
	double sum2= 0.0;
	int line_file = 0;
	int g_count=0;
	int opncnt = 0;
	char filename[100]={0};
	double dTmp_amt = 0.00;
	double dFct_intst = 0.00;
	char cWherelist[100];
	
	 g_reply_int = sql_begin();      /* ������ */
    if (g_reply_int != 0)
    {
            sprintf(acErrMsg, "������ʧ��!!!");
            WRITEMSG
                    goto ErrExit;
    }
    /* ��ʼ��ȫ�ֱ��� */
    pub_base_sysinit();

	
	memset(cWherelist,0x00,sizeof(cWherelist));
	
	
	sprintf(cWherelist,"sts != '*'  order by prdt_code,pact_no,sts ",g_pub_tx.tx_date);
	
	vtcp_log( "WHERE [%s]",cWherelist );
	
	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,cWherelist);	
	
 pub_base_AllDwnFilName( filename );
 vtcp_log("filename = %s",filename);
  fp = fopen( filename,"w" );
  /*if( fp==NULL )
  {
          sprintf(acErrMsg," open file [%s] error ",filename );
          WRITEMSG
          strcpy( g_pub_tx.reply,"S047" );
          goto ErrExit;
  }*/

   while(1)
   {
   	
   	  memset(&sFn_reg,0x00,sizeof(sFn_reg));
   	  memset(&sFn_parm,0x00,sizeof(sFn_parm));
   	  memset(&sFn_flt_radio_parm,0x00,sizeof(sFn_flt_radio_parm));


   	
   		ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
   		if(ret == 100)
   			break;
   		
   		ret = Fn_flt_radio_parm_Sel(g_pub_tx.reply, &sFn_flt_radio_parm, "prdt_code = '%s'",sFn_reg.prdt_code );
   		
   		ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code = '%s' ",sFn_reg.prdt_code);
   		
   		dTmp_amt = sFn_reg.tx_amt * sFn_parm.term * sFn_parm.rate * (1 + sFn_reg.flt_radio)/365/100;
      dFct_intst = pub_base_PubDround_1(dTmp_amt);
      
      if(pub_base_CompDblVal(dFct_intst,sFn_reg.intst)!=0)
      {
      		fprintf(fp,"��Ʒ���� %s| ��Ʒ���� %s |�ͻ��˺� %s  |�Ϲ���� %lf|Ԥ��������%lf|�ϸ����� %lf| Ԥ������  %lf | ʵ������  %lf \n",sFn_parm.title,sFn_parm.prdt_code,sFn_reg.ac_no,sFn_reg.tx_amt,sFn_parm.rate,sFn_reg.flt_radio,sFn_reg.intst,dFct_intst);
      }
   		
   }	
	Fn_reg_Clo_Sel();
	fclose(fp);
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������Ʋ�Ʒ����������ϸ��ɹ�!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
printf("\n������[%d]����¼.\n",g_count);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"������Ʒ���������ϸ��ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;	
}

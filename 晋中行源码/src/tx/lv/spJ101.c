/*************************************************
* �� �� ��:  spJ101.c
* ���������� ��ȡ������
*			       �������漰��ʵʱ�������ʣ��տ��ʺ���ȡ�����ѣ�
*			       ʵʱ��ǻ�ִ���ʸ����˻��������
* ��    ��:  ligl2008
* ������ڣ� 2007-9-4 18:56
*insert into com_parm values('SXFKM',0,'�����ѿ�Ŀ','1','51108')
*insert into tx_sub_rel values('5840','XEJZ','0','0300','0370','�˺�') ����30���ֵ
*insert into tx_sub_def values('J101','��������','spJ101','0','0');
*insert into com_parm values('SSSXF',0,'TCTD������','1','15')
*insert into tx_flow_def values('5840',6,'J101','#=0000@8@#=0001@7@#$')
*insert into tx_flow_def values('5840',7,'A017','#=0000@8@#$')
*update  tx_flow_def set flow_code=8 where tx_code='5840 and flow_code='6';
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"

int spJ101()
{
	char chkcmtno[4];
	char txtime[7];	
	char acc_no[8];	
	char cErrmsg[9];
	char rcpstat[4];
	int iStep=0;  
	int ret=0;
	long iSubpackIndex=0;
  
	pub_base_sysinit();
	memset(cErrmsg,0,sizeof(cErrmsg));
	memset(chkcmtno,'\0',sizeof(chkcmtno));
	memset(acc_no,'\0',sizeof(acc_no));
	memset(txtime,'\0',sizeof(txtime));
 	/* begin NEWTCTD4 */
    	char cValues[21];
    	memset(cValues, 0 , sizeof(cValues));
    	pub_base_GetParm_Str("SSSXF",0,cValues);
	get_zd_data("0510",chkcmtno);	
	get_zd_data("0890",rcpstat);
	pub_base_strpack_all(rcpstat);
				
	/* add NEWTCTD4 */
	sprintf(chkcmtno,"%03d",atoi(chkcmtno));
	/* end NEWTCTD4 */	
	vtcp_log(" --+++====chkcmtno =[%s] [%s],[%d]",chkcmtno,rcpstat,atoi(cValues));
		
	if((!memcmp(chkcmtno,"003",3)||!memcmp(chkcmtno,"004",3)) && !memcmp(rcpstat,"00",2) && atoi(cValues)!=0)
	{
    	  	
	          get_zd_data("0370",g_pub_tx.ac_no);
	          pub_base_strpack_all(g_pub_tx.ac_no);
	          g_pub_tx.ac_seqn=1;
	          /*set_zd_data("1021",g_pub_tx.ac_no);
	          set_zd_int("1022",g_pub_tx.ac_seqn);*/
	          strcpy(g_pub_tx.add_ind,"0");	
	    	memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
		strcpy( g_pub_tx.sub_tx_code, "D003" );
		set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
		set_zd_data( "102O", "0" );
		set_zd_data( "102J", "01" );
		set_zd_data( "102K", "2" );
		set_zd_double( "102F", atoi(cValues));
		set_zd_double( "102P", 0.00 );
		set_zd_double( "102Q", 0.00 );
		strcpy(g_pub_tx.brf, "������");
		set_zd_data("102Y",g_pub_tx.brf);
		strcpy(g_pub_tx.ac_wrk_ind,"001001001");
		strcpy(g_pub_tx.hst_ind,"1");
		g_pub_tx.svc_ind=1001;
		g_pub_tx.tx_amt1=atoi(cValues);
		strcpy( g_pub_tx.tx_code, "5840" );
		ret = pub_acct_trance();
		if( ret != 0 )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}
		ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
		if ( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}
		 ret=pub_base_GetParm_Str("SXFKM",0,acc_no);
		 if( ret )
		{
			sprintf(acErrMsg,"��ȡ�����������쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O005");
			goto ErrExit;
		}
		pub_base_strpack_all(acc_no);
	        vtcp_log(" ++++++++++ acc_no =[%s] ",acc_no);	
		set_zd_data("1211","5110103");
	        set_zd_double("1218",atoi(cValues));
	        set_zd_data("121A","������");
	        set_zd_data("1215","2");
	        set_zd_data("1214","01");
		strcpy(g_pub_tx.reply,"0001" ); /*A017���� */
					
	}
	else
	{
	  	strcpy(g_pub_tx.reply,"0000" ); /* ����J099���� */
  	}
OkExit:
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("%s,%d spJ099() С������ ���� �ӽ��׽��� end! [%ld]",__FL__,iSubpackIndex);

	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("%s,%d spJ099() С������ ���� �ӽ��׽��� end! [%ld]",__FL__,iSubpackIndex);

	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

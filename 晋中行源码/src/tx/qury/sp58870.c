/***** �������ռ��Ϣ�嵥��ѯ(sp5347)��������**********/
/*****************************************************
* �� �� ��:  sp5887.c
* ���������� ͸֧��Ϣ��ѯ
* ��    ��:  wangyh
* ������ڣ� 2006��8��29�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "od_bill_c.h"
int sp5887()
{
	int ret=0,i;
	char tx_date[9];
	struct  od_bill_c       f_od_bill;
	char cPrtname[5];
	char cPrtname1[5];
	char cFilename[100];/*�ļ���*/
	FILE *fp;
	memset(tx_date,0,sizeof(tx_date));
	memset(&f_od_bill,0x00,sizeof(struct od_bill_c));
	memset(cPrtname,0,sizeof(cPrtname));
	memset(cPrtname1,0,sizeof(cPrtname1));
	vtcp_log("sp5887 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	pub_base_sysinit();
	get_zd_data("0440",tx_date);
	vtcp_log("[%s][%d]tx_date=[%s]",__FILE__,__LINE__,tx_date);	
	vtcp_log("sp5887 ��ʼ��[%s][%d]",__FILE__,__LINE__);	
	ret=Od_bill_Dec_Sel ( g_pub_tx.reply,"tx_date='%s'",tx_date);
	ERR_DEAL
	i=0;
	while(1)
	{
	  	memset(cPrtname,0,sizeof(cPrtname));
	  	memset(cPrtname1,0,sizeof(cPrtname1));
	  	ret=Od_bill_Fet_Sel (&f_od_bill,g_pub_tx.reply);
	  	if( ret==100 ) 
        	{
        		break;
        	}
		ERR_DEAL
	 	if( !i )
	  	{
			pub_base_AllDwnFilName(cFilename);
			fp = fopen(cFilename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",cFilename);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			fprintf( fp,"~��������|�ʵ����|Ƿ�����|��Ϣ����|͸֧��Ϣ|ǷϢ���|��������|����״̬|�˵�״̬|\n" );
		}
		if(f_od_bill.pay_sts[0]='Y')
		{
			memcpy(cPrtname,"�ѻ�", sizeof(cPrtname)-1);
	        }
	        else
	        {
	                memcpy(cPrtname,"δ��", sizeof(cPrtname)-1);
	        }
	       if(f_od_bill.curr_sts[0]='1')
		{
			memcpy(cPrtname1,"����", sizeof(cPrtname1)-1);
	        }
	        else
	        {
	                memcpy(cPrtname1,"����", sizeof(cPrtname1)-1);
	        } 
		fprintf( fp,"%d|%.2f|%.2f|%.2f|%.2f|%.2f|%d|%s|%s|\n",f_od_bill.tx_date,f_od_bill.amt,f_od_bill.bal,f_od_bill.intst_acm,f_od_bill.intst_amt,f_od_bill.intst_bal,f_od_bill.pay_date,cPrtname,cPrtname1);
		i++;
        }
	ret=Hv_zf_Clo_Sel();
	ERR_DEAL
	if(!i)
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
	}	
 
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}



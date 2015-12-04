/***************************************************************
* �� �� ��:     pubf_acct_chrg.c
* ����������
*				�շ����س���
*
* ��    ��:		xxx
* ������ڣ�    2005��3��29��
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
#include "svrpub_c.h"
#include "mo_def_rate_c.h"
#include "com_chrg_rate_c.h"
#include "com_def_chrg_c.h"

pubf_def_chrg(tx_code,chrg_code)
char *tx_code;
char *chrg_code;
{
	struct com_def_chrg_c def_chrg;

	RespCode=Com_def_chrg_Sel(RtCode,&def_chrg,"tx_code='%s'",tx_code);
	if ( RespCode <0 )
	{
		sprintf ( acErrMsg , "ȡ�շѶ�����[%s]",tx_code);
		WRITEMSG
		strcpy ( RtCode ,"W120" );
		return -1;
	}
	else if ( RespCode )
	{
		sprintf ( acErrMsg , "�շѶ�������޴˽��׶����[%s]",tx_code);
		WRITEMSG
		return 1;
	}

	strcpy ( chrg_code , def_chrg.chrg_rate_code );

	return 0;
}

pubf_def_chrg_proc(chrg_code,chrg_rate_no,amt_no,pamt)
char *chrg_code;
char *chrg_rate_no;
char *amt_no;
double *pamt;
{
	struct mo_def_rate_c def_rate;
	int flag;
	double bal,amt;
	int cnt; 
	int iRet=0;/*JYW*/

	flag=0;
	strcpy ( chrg_rate_no , chrg_code );
	vtcp_log("Enter pubf_def_chrg_proc");
	iRet=Mo_def_rate_Dec_Sel(RtCode,"chrg_code='%s'",chrg_code);
	vtcp_log("%s,%d find mo_def_rate =[%d]",__FILE__,__LINE__,iRet);
	if ( iRet!=0 )
	{
		sprintf ( acErrMsg , "��������������α��[%s]",chrg_code);
		WRITEMSG
		strcpy ( RtCode ,"W114" );
		return 1;
	}

	while(1)
	{
		RespCode=Mo_def_rate_Fet_Sel(&def_rate,RtCode);
		if ( RespCode < 0 )
		{
			Mo_def_rate_Clo_Sel ();
			sprintf ( acErrMsg , "ȡ����������α��[%s]",chrg_code);
			WRITEMSG
			strcpy ( RtCode ,"W115" );
			return 1;
		}
		else if ( RespCode )
		{
			break;
		}
		switch(def_rate.chrg_type)
		{
			case 0:
				vtcp_log("%s,%d �շ���������������� ",__FILE__,__LINE__);
				strcpy ( chrg_rate_no , def_rate.chrg_rate_no );
				pub_acct_dc_get_amt( amt_no,&amt );
					*pamt=amt;
				flag=1;
				break;
			case 1: /*���*/
				vtcp_log("%s,%d �շ�������������� ",__FILE__,__LINE__);
				pub_acct_dc_get_amt( amt_no,&amt );
				vtcp_log("%s,%d �շ�������������� amt=[%f]",__FILE__,__LINE__,amt);
				if ( def_rate.min_amt<=amt && def_rate.max_amt>=amt )
				{
					strcpy ( chrg_rate_no , def_rate.chrg_rate_no );
					*pamt=amt;
					flag=1;
				}
				else
					*pamt=0.0;
				break;
			default:
				Mo_def_rate_Clo_Sel ();
				strcpy ( RtCode , "W116" );
				sprintf(acErrMsg,"���������������[%d]",def_rate.chrg_type );
				WRITEMSG
				return 1;
		}
		if ( flag==1 )
		{
			break;
		}
	}
	Mo_def_rate_Clo_Sel ();

	return 0;
}

pubf_acct_chrg_proc(chrg_rate_code,dc_code,chrg_amt,amt)
char *chrg_rate_code;
char *dc_code;
double *chrg_amt;
double amt;
{
	struct com_chrg_rate_c chrg_rate;
	double tmp_amt;
	memset(&chrg_rate, 0 , sizeof(chrg_rate));
	char cQuickFlag[2];
	memset(cQuickFlag, 0 , sizeof(cQuickFlag));
	get_zd_data("122F",cQuickFlag);
	vtcp_log("%s,%d amt == %f �Ӽ���־=[%s]",__FILE__,__LINE__,amt,cQuickFlag);

	RespCode=Com_chrg_rate_Sel(RtCode,&chrg_rate,
		"chrg_rate_code='%s'",chrg_rate_code);
	if ( RespCode < 0 )
	{
		sprintf ( acErrMsg , "ȡ�������ʱ��[%s]",chrg_rate_code);
		WRITEMSG
		strcpy ( RtCode ,"W117" );
		return 1;
	}
	else if ( RespCode )
	{
		sprintf ( acErrMsg , "�������ʱ��޶�Ӧ���ʱ��[%s]",chrg_rate_code);
		WRITEMSG
		strcpy ( RtCode ,"W118" );
		return 1;
	}
	switch(chrg_rate.type[0])
	{
		case '0':
		case '1':
			*chrg_amt=pub_base_PubDround_1(chrg_rate.min_amt);
			/**chrg_amt=pub_base_PubDround_1(chrg_rate.val); */
			vtcp_log("%s,%d ����С����շ�%f",__FILE__,__LINE__,*chrg_amt);
			break;
		case '2':
			vtcp_log("%s,%d rate.val=[%f],amt ==[%f] ",__FILE__,__LINE__,chrg_rate.val,amt);
			tmp_amt=pub_base_PubDround_1(chrg_rate.val*amt);
			if ( tmp_amt <= chrg_rate.min_amt && chrg_rate.min_amt >0.005 )
			{
				tmp_amt = chrg_rate.min_amt;
			}
			if ( tmp_amt >= chrg_rate.max_amt && chrg_rate.max_amt >0.005 )
			{
				tmp_amt = chrg_rate.max_amt;
			}
			*chrg_amt=tmp_amt;
			if(cQuickFlag[0]=='1') *chrg_amt=*chrg_amt * 1.3;
			vtcp_log("%s,%d �������շ�%f",__FILE__,__LINE__,*chrg_amt);
			break;
		case '3':
			vtcp_log("%s,%d rate.val=[%f],amt ==[%f] ",__FILE__,__LINE__,chrg_rate.val,amt);
			tmp_amt=pub_base_PubDround_1(chrg_rate.min_amt);
			if ( tmp_amt <= chrg_rate.min_amt && chrg_rate.min_amt >0.005 )
			{
				tmp_amt = chrg_rate.min_amt;
			}
			if ( tmp_amt >= chrg_rate.max_amt && chrg_rate.max_amt >0.005 )
			{
				tmp_amt = chrg_rate.max_amt;
			}
			if(amt==0) *chrg_amt=0;
			else *chrg_amt=tmp_amt;
			if(cQuickFlag[0]=='1') *chrg_amt=*chrg_amt * 1.3;
			vtcp_log("%s,%d �����ݽ���շ�%f",__FILE__,__LINE__,*chrg_amt);
			break;
		default:
			strcpy ( RtCode , "W119" );
			sprintf(acErrMsg,"���������������[%s]",chrg_rate.type );
			WRITEMSG
			return 1;
	}
	*chrg_amt=pub_base_PubDround_1(*chrg_amt);/* martin ����4-5���� 2008/12/26 20:33:43*/
	strcpy ( dc_code , chrg_rate.dc_code );

	return 0;
}

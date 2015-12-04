/*****************************************************************
* �� �� ��: spH628.c
* ��������������ҵ��λ���Ϲ���
*
* ��    ��: jack
* ������ڣ�2004��3��28��
*
* �޸ļ�¼:
* ��    ��: 20070622
* �� �� ��: wanglei
* �޸�����: ��ɾ����λʱ�����������ȫ���ʻ�Ҳ�ӵ�λ��Ϣ����ɾ��
******************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "ag_qs_gd_rel_c.h"

spH628()
{
	int ret = 0;
	int Count = 0;
	char proc_flag[2];
	char tbwhere[128];

	char cUnit_no[5];
	char cUnit_name[51];
	char cOpr_attri[2];
	char cPay_type[3];
	char cAcct_no[26];
	char cQsAcct_no[26];
	char cUp_unit[5];
	char cAuto_flag[2];
	char cClr_flag[2];

	struct ag_unitinfo_c sUnitInfo;	  /*��λ��Ϣ���Ʊ�*/
	struct ag_qs_gd_rel_c sAg_qs_gd_rel; /*�����˻��͹����˻�������*/
	
	memset(proc_flag,0,sizeof(proc_flag));
	memset(tbwhere,0,sizeof(tbwhere));

	memset(&sUnitInfo,0x0,sizeof(sUnitInfo));

	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cUnit_name,0x0,sizeof(cUnit_name));
	memset(cOpr_attri,0x0,sizeof(cOpr_attri));
	memset(cPay_type,0x0,sizeof(cPay_type));
	memset(cAcct_no,0x0,sizeof(cAcct_no));
	memset(cQsAcct_no,0x0,sizeof(cQsAcct_no));
	memset( cUp_unit, 0x0, sizeof(cUp_unit) );
	memset( cAuto_flag, 0x0, sizeof(cAuto_flag) );
	memset( cClr_flag, 0x0, sizeof(cClr_flag) );

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
  
	/*-------- ȡ�������  ---------*/
	get_zd_data("0700",proc_flag);

	get_zd_data("0250",cUnit_name);
	get_zd_data("0710",cOpr_attri);
	get_zd_data("0210",cPay_type);
	get_zd_data("0300",cAcct_no);
	pub_base_old_acno( cAcct_no );
	set_zd_data("0300",cAcct_no);
	/** �˻������� add by wudawei 20130820  **/
	get_zd_data("0310",cQsAcct_no);
	pub_base_old_acno( cQsAcct_no );
	set_zd_data("0310",cQsAcct_no);
	/* add by wudawei 20130820 end */
	get_zd_data("0720",cAuto_flag);
	get_zd_data( "0670", cClr_flag );
	get_zd_data("0350",cUp_unit);

	/** ����Ƿ񱾻����Ĵ���λ **/
	if (proc_flag[0] != '0')
	{
		get_zd_data("0920",cUnit_no);
		ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sUnitInfo, \
				"unit_no = '%s'", cUnit_no );
		if ( ret )
		{
			sprintf( acErrMsg, "��ѯ��λ��Ϣ��!!!" );
			WRITEMSG
			goto ErrExit;
		}
        	
		if ( strcmp(sUnitInfo.opn_br_no, g_pub_tx.tx_br_no) != 0 )
		{
			sprintf( acErrMsg, "�Ǳ���������λ!!!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H057" );
			goto ErrExit;
		}
	}

	memset( &sUnitInfo, 0x0, sizeof(sUnitInfo) );
	/** �˻������� add by wudawei 20130820  **/
	memset( &sAg_qs_gd_rel, 0x0, sizeof(sAg_qs_gd_rel) );
	/* add by wudawei 20130820 end */
	if (proc_flag[0] == '2')	/*--- ɾ�� ---*/
	{
		get_zd_data("0920",cUnit_no);
		vtcp_log("[%s][%d]��2007��06��22�պ���ɾ����λ�����и����ʻ���Ϣ!",__FILE__,__LINE__);
		sprintf(tbwhere,"unit_no = '%s'",cUnit_no);
		ret = deldb("ag_peritemmap",tbwhere);
		if (ret != 0)
		{
			sprintf(acErrMsg,"ɾ����λ�����ʻ���Ϣ��¼��[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H007");
			goto ErrExit;
		}
		ret = deldb("ag_unitinfo",tbwhere);
		if (ret != 0)
		{
			sprintf(acErrMsg,"ɾ����λ��Ϣ��¼��[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H007");
			goto ErrExit;
		}
		/*** 20130820Ϊ����������ag_qs_gd_rel������ҵ������˻�������˻��Ĺ����� ɾ����λ��Ϣʱ ͬʱɾ��������Ϣ add by wudawei 20130820 ***/
		ret = deldb("ag_qs_gd_rel",tbwhere);
		if (ret != 0)
		{
			sprintf(acErrMsg,"ɾ�������˻�������˻��Ĺ�������Ϣ��¼��[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H007");
			goto ErrExit;
		}
		/*** add end 20130820 ***/
	}
	else if (proc_flag[0] == '1')	/*--- �޸� ---*/
	{

		get_zd_data("0920",cUnit_no);
			sprintf(acErrMsg,"�޸ĵ�λ��Ϣ�ĵ�λ���Ϊ[%s]",cUnit_no);
			WRITEMSG
		ret = Ag_unitinfo_Dec_Upd( g_pub_tx.reply , "unit_no = '%s'",cUnit_no);
		if (ret != 0)
		{
			sprintf(acErrMsg,"�޸ĵ�λ��Ϣ��1[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		ret = Ag_unitinfo_Fet_Upd( &sUnitInfo, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"�޸ĵ�λ��Ϣ��2[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		strcpy( sUnitInfo.unit_no, cUnit_no );
		strcpy( sUnitInfo.unit_name, cUnit_name );
		strcpy( sUnitInfo.bank_acct_no, cAcct_no );
		strcpy( sUnitInfo.up_unit, cUp_unit );
		strcpy( sUnitInfo.auto_flag, cAuto_flag);
		strcpy( sUnitInfo.clr_flag, cClr_flag );
		strcpy( sUnitInfo.opr_attri,cOpr_attri);
		strcpy( sUnitInfo.pay_type,cPay_type);
		
		ret = Ag_unitinfo_Upd_Upd( sUnitInfo, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"�޸ĵ�λ��Ϣ��3[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
		ret = Ag_unitinfo_Clo_Upd( );
		/** �˻������� add by wudawei 20130820  **/		
		Count = sql_count("ag_qs_gd_rel","unit_no=%s",cUnit_no);
		if(Count > 0)
		{
			ret = Ag_qs_gd_rel_Dec_Upd( g_pub_tx.reply , "unit_no = '%s'",cUnit_no);
	    if (ret != 0)
			{
				sprintf(acErrMsg,"�޸��˻�������Ϣ��1[%s]",cUnit_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"H008");
				goto ErrExit;
			}
	
			ret = Ag_qs_gd_rel_Fet_Upd( &sAg_qs_gd_rel, g_pub_tx.reply );
			if (ret != 0)
			{
				sprintf(acErrMsg,"�޸��˻�������Ϣ��2[%s]",cUnit_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"H008");
				goto ErrExit;
			}
			strcpy(sAg_qs_gd_rel.qs_ac_no, cQsAcct_no);
			strcpy(sAg_qs_gd_rel.gd_ac_no, cAcct_no);
			strcpy(sAg_qs_gd_rel.tel,  g_pub_tx.tel);
			sAg_qs_gd_rel.clr_date = g_pub_tx.tx_date;
			ret = Ag_qs_gd_rel_Upd_Upd( sAg_qs_gd_rel, g_pub_tx.reply );
			if (ret != 0)
			{
				sprintf(acErrMsg,"�޸��˻�������Ϣ��3[%s]",cUnit_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"H008");
				goto ErrExit;
			}
			ret = Ag_qs_gd_rel_Clo_Upd( );
	  }
	  else
	  {
	  	/** ���������Ϣ add by wudawei 20130820 **/
			strcpy(sAg_qs_gd_rel.unit_no, cUnit_no);
			strcpy(sAg_qs_gd_rel.qs_ac_no, cQsAcct_no);
			strcpy(sAg_qs_gd_rel.gd_ac_no, cAcct_no);
			strcpy(sAg_qs_gd_rel.tel,  g_pub_tx.tel);
			sAg_qs_gd_rel.clr_date = g_pub_tx.tx_date;
			
			ret = Ag_qs_gd_rel_Ins(sAg_qs_gd_rel,g_pub_tx.reply);
			if (ret != 0)
			{
				sprintf(acErrMsg,"��������˻��͹����˻��������[%s]",cUnit_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"H008");
				goto ErrExit;
			}
			/** add end 20130820 wudawei **/
	  }
		/*** add end 20130820 ***/
	}
	else if (proc_flag[0] == '0')
	{
		ret = pub_agent_unitno(cUnit_no);
		if (ret != 0)
		{
			sprintf(acErrMsg,"���ɵ�λ��Ŵ�!!!");
			WRITEMSG
			goto ErrExit;
		}
		strcpy(sUnitInfo.unit_no,cUnit_no);
		strcpy(sUnitInfo.unit_name,cUnit_name);
		strcpy(sUnitInfo.opr_attri,cOpr_attri);
		strcpy(sUnitInfo.pay_type,cPay_type);
		strcpy(sUnitInfo.bank_acct_no,cAcct_no);
		sUnitInfo.beg_date = 0;
		sUnitInfo.end_date = 0;
		sUnitInfo.clr_date = 0;
		strcpy( sUnitInfo.up_unit, cUp_unit );
		strcpy( sUnitInfo.auto_flag, cAuto_flag );
		strcpy( sUnitInfo.clr_flag, cClr_flag );
		sUnitInfo.tx_date = 0;
		sUnitInfo.opn_date = g_pub_tx.tx_date;
		strcpy( sUnitInfo.opn_tel, g_pub_tx.tel );
		strcpy( sUnitInfo.opn_br_no, g_pub_tx.tx_br_no );
		strcpy(sUnitInfo.unit_stat,"0");

		ret = Ag_unitinfo_Ins(sUnitInfo,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"���뵥λ��Ϣ���[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
		
		/** ���������Ϣ add by wudawei 20130820 **/
		strcpy(sAg_qs_gd_rel.unit_no, cUnit_no);
		strcpy(sAg_qs_gd_rel.qs_ac_no, cQsAcct_no);
		strcpy(sAg_qs_gd_rel.gd_ac_no, cAcct_no);
		strcpy(sAg_qs_gd_rel.tel,  g_pub_tx.tel);
		sAg_qs_gd_rel.clr_date = g_pub_tx.tx_date;
		 vtcp_log("%s,%d,unit_no=[%s]\n",__FILE__,__LINE__,sAg_qs_gd_rel.unit_no);
		  vtcp_log("%s,%d,qs_ac_no=[%s]\n",__FILE__,__LINE__,sAg_qs_gd_rel.qs_ac_no);
		   vtcp_log("%s,%d,gd_ac_no=[%s]\n",__FILE__,__LINE__,sAg_qs_gd_rel.gd_ac_no);
		    vtcp_log("%s,%d,tel=[%s]\n",__FILE__,__LINE__,sAg_qs_gd_rel.tel);
		     vtcp_log("%s,%d,clr_date=[%ld]\n",__FILE__,__LINE__,sAg_qs_gd_rel.clr_date);
		ret = Ag_qs_gd_rel_Ins(sAg_qs_gd_rel,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��������˻��͹����˻��������[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
		/** add end 20130820 wudawei **/
	}
	else
	{
		sprintf(acErrMsg,"�������ʹ�[%s]",proc_flag);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H006");
		goto ErrExit;
	}
	g_pub_tx.tx_amt1=0.00;
	g_pub_tx.ac_wrk_ind[1]='1';
/* ����ˮ��־ */
    ret = pub_ins_trace_log();
    if ( ret )
    {
        sprintf( acErrMsg, "����ˮ��־��" );
        WRITEMSG
        goto ErrExit;
    }

	/*** ������� ***/
	set_zd_data("0920",cUnit_no);
OkExit:
	ret = Ag_unitinfo_Clo_Upd( );
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ά����λ��Ϣ��Ϣ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	ret = Ag_unitinfo_Clo_Upd( );
	sprintf(acErrMsg,"ά����λ��Ϣʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

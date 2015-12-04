/*****************************************************
* �� �� ��:  sp9400.c
* ���������� ����Ҫ���͵���Ϣ���ҵ��˱ʷ�����Ϣ����ϸ  hv_zf
* ��    ��:  xyy
* ������ڣ� 2006��8��16�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "hv_define.h"
int sp9400()
{
	int ret=0;
	long lWtdate;
	char cKinbrBakcode[13];
	char cOrderno[9];
	char cCmtno[4];
	HV_FD123_AREA fd123;
	struct hv_zf_c hv_zf;

	lWtdate=0;
	memset(cOrderno,0,sizeof(cOrderno)-1);
	memset(cCmtno,0,sizeof(cCmtno)-1);
	memset(cKinbrBakcode,0,sizeof(cKinbrBakcode)-1);
	memset(&fd123, 0 , sizeof(fd123));
	memset(&hv_zf,0x00,sizeof(hv_zf));
	vtcp_log("sp9400��ʼ��[%s][%d]",__FILE__,__LINE__);

	get_zd_data("1230",(char*)&fd123);
	vtcp_log("�õ�123�ṹ�������=[%s]",(char*)&fd123);TRACE

	pub_base_sysinit();

	GetOr_br_noByBr_no(g_pub_tx.tx_br_no,cKinbrBakcode);/*���ݻ��������к�*/

	memcpy(cOrderno	,fd123.orderno		,sizeof(cOrderno)-1);
	memcpy(cCmtno  	,fd123.cmtno  		,sizeof(cCmtno)-1);
	lWtdate=g_pub_tx.tx_date;	

	ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "orderno='%s' and cmtno='%s' and or_br_no='%s' and wt_date='%ld'",cOrderno,cKinbrBakcode,cKinbrBakcode,lWtdate);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴˼�¼");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯHv_zf�����![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	

	vtcp_log("�õ�123�ṹ�������=[%s]",(char*)&fd123);TRACE

	set_zd_data("1230",(char *)&fd123);  /* �ٰ�123���ظ�ǰ̨ */


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

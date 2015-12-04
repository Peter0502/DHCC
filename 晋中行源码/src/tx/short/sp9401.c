/*****************************************************
* �� �� ��:  sp9401.c
* ���������� ��ѯ���֧�����׻�����Ϣ
* ��    ��:  xyy
* ������ڣ� 2006-9-1 22:06 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
int sp9401()
{
	int ret=0,i;
	struct lv_pkgreg_c  lv_pkgreg;
	char cKinbr[BRNO_LEN+1];
	LV_FD123_AREA fd123;
	memset(&fd123, ' ',sizeof(fd123));
	memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(cKinbr , 0 , sizeof(cKinbr));
	vtcp_log("sp9401 ��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_zd_data("0030",cKinbr);
	get_fd_data("1230",(char*)&fd123);
	vtcp_log("[%s,%d]1230=[%s]",__FILE__,__LINE__,(char*)&fd123);
	lv_pkgreg.wt_date=apatoi(fd123.wt_date,8);
	memcpy(lv_pkgreg.orderno,fd123.orderno,sizeof(lv_pkgreg.orderno)-1);
	memcpy(lv_pkgreg.or_br_no,fd123.or_br_no,sizeof(lv_pkgreg.or_br_no)-1);
	vtcp_log("+++++lv_pkgreg.or_br_no==[%s]!+++++",lv_pkgreg.or_br_no);
	zip_space(lv_pkgreg.or_br_no);
	if(strlen(lv_pkgreg.or_br_no)==0)
	{
		GetOr_br_noByBr_no(cKinbr, lv_pkgreg.or_br_no );
	}
	vtcp_log("sp9401 ��ʼ��[%s][%d]",__FILE__,__LINE__);	
	vtcp_log("[%s][%d]orderno=[%s],or_br_no=[%s]wt_date=[%d]",__FILE__,__LINE__,
		lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date);
	vtcp_log("sp9401 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	ret = lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, "orderno='%s' and or_br_no ='%s' and\
	 	wt_date=%s",lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date);
	vtcp_log("sp9401 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴˴��֧������");
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01" );
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ���֧�����׻�����Ϣ����[%d]",ret);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("sp9401 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);

	/*************��ʱ����Щ�����������Ҫ�Լ�������ӷ���ֵ��лл!*************/

	/**PKGNO***/
	memcpy(fd123.pkgno		,lv_pkgreg.pkgno,sizeof(fd123.pkgno));
	/**ҵ�����ͱ��**/
	memcpy(fd123.txnum		,lv_pkgreg.txnum,sizeof(fd123.txnum));
	/**�����**/
	memcpy(fd123.pack_id		,lv_pkgreg.packid,sizeof(fd123.pack_id));
	/**������**/
	apitoa(lv_pkgreg.pack_date	,sizeof(fd123.pack_date),fd123.pack_date);
	/**�����������к�**/
	memcpy(fd123.pay_qs_no		,lv_pkgreg.pay_qs_no,sizeof(fd123.pay_qs_no));
	/**�����к� ԭ�����к� **/
	memcpy(fd123.or_br_no		,lv_pkgreg.or_br_no,sizeof(fd123.or_br_no));
	/**�����˿�����**/
	memcpy(fd123.pay_opn_br_no	,lv_pkgreg.pay_opn_br_no,sizeof(fd123.pay_opn_br_no));
	/**�������ʺ�(ԭ�տ����ʺ�)**/
	memcpy(fd123.pay_ac_no		,lv_pkgreg.pay_ac_no,sizeof(fd123.pay_ac_no));
	/**����������(ԭ�տ�������)**/
	memcpy(fd123.pay_name		,lv_pkgreg.pay_name,sizeof(fd123.pay_name));
	/**�����˵�ַ(ԭ�տ��˵�ַ)**/
	memcpy(fd123.pay_addr		,lv_pkgreg.pay_addr,sizeof(fd123.pay_addr));
	/**���������к�**/
	memcpy(fd123.cash_qs_no		,lv_pkgreg.cash_qs_no,sizeof(fd123.cash_qs_no));
	/**�����к�(ԭ�����к�)**/
	memcpy(fd123.ac_br_no		,lv_pkgreg.ac_br_no,sizeof(fd123.ac_br_no));
	/**�տ��˿����к�**/
	memcpy(fd123.cash_opn_br_no	,lv_pkgreg.cash_opn_br_no,sizeof(fd123.cash_opn_br_no));
	/**�տ����ʺ�,ԭ�������ʺ�**/
	memcpy(fd123.cash_ac_no		,lv_pkgreg.cash_ac_no,sizeof(fd123.cash_ac_no));
	/**�տ�������,ԭ����������**/
	memcpy(fd123.cash_name		,lv_pkgreg.cash_name,sizeof(fd123.cash_name));
	/**�տ��˵�ַ**/
	memcpy(fd123.cash_addr		,lv_pkgreg.cash_addr,sizeof(fd123.cash_addr));
	/**ҵ������**/	
	memcpy(fd123.yw_type		,lv_pkgreg.lv_yw_ind,sizeof(fd123.yw_type));
	/*������� **/
	memcpy(fd123.orderno		,lv_pkgreg.orderno,sizeof(fd123.orderno));
	/**����**/
	memcpy(fd123.brf		,lv_pkgreg.filler,sizeof(fd123.brf));
	/**ԭ֧���������**/
	memcpy(fd123.o_orderno		,lv_pkgreg.o_orderno,sizeof(fd123.o_orderno));
	/**���ȼ���**/
	fd123.operlevel[0]=lv_pkgreg.operlevel[0];
	/**֧������״̬**/
	fd123.lv_sts[0]=lv_pkgreg.lv_sts[0];
	/**�����ѱ�־**/
	fd123.chrg_ind[0]=lv_pkgreg.tx_chrg_ind[0];
	/**���׽��**/
	dot_str2dbl(fd123.tx_amt,sizeof(fd123.tx_amt),&lv_pkgreg.tx_amt);
	/**�����ѽ��**/
	dot_str2dbl(fd123.chrg_amt,sizeof(fd123.chrg_amt),&lv_pkgreg.chrg_amt);
	/**���ո�**/
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
	/*������Ҫ������*/
	if(!memcmp(lv_pkgreg.cur_no,"RMB",3))
	set_zd_data("0210","01");   /* ���ҷ��� */
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

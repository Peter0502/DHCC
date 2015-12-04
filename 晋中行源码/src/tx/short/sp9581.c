/*****************************************************
* �� �� ��:  sp9581.c
* ���������� ����zjgl_mst--�ʽ����״�������ػ�����Ϣ
*  �跽���������������ڣ���Ϣ���ڣ���Ŀ�� ��Ŀ������
*  FD1001     fd1002    FD44      FD45     FD64    FD65
*  ������Ϣ���ͣ��������ʣ��跽���������������������������޶�
*   fd72          fd1005    fd56      fd67     fd25  fd1003
*  �ۼƽ��׽��޶����ڣ��޶ʼ���ڣ��޶��������,����
*  fd1004          fd1007   FD46           FD47       fd1006
* ��    ��:  LiuHuafeng 2006-10-25 17:58
* ������ڣ� 2006-10-25 17:58
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "zjgl_mst_c.h"
int sp9581()
{
	int ret=0,i;
	struct zjgl_mst_c  sZjgl_mst;
	char cKinbr[BRNO_LEN+1];
	char cAcc_no[8];
	char cCur_no[3];
	memset(cCur_no, 0 , sizeof(cCur_no));
	memset(&sZjgl_mst, 0 , sizeof(sZjgl_mst));
	memset(cKinbr , 0 , sizeof(cKinbr));
	memset(cAcc_no, 0 , sizeof(cAcc_no));

	
	vtcp_log("sp9581 ��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_zd_data("0910",cKinbr);
	get_zd_data("0640",cAcc_no);
	get_zd_data("0210",cCur_no);
	vtcp_log("%s,%d curno====[%s] ",__FILE__,__LINE__,cCur_no);
	
	/******��ȡ����������ֵ*******/
	ret = Zjgl_mst_Sel(g_pub_tx.reply,&sZjgl_mst, "opn_br_no='%s' and cur_no='%s' and acc_no ='%s' ", cKinbr,cCur_no,cAcc_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"����Ŀ�Ŀ����Ҫ�����ʽ����!!");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"���ݿ����ʧ��[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
/*  �跽���������������ڣ���Ϣ���ڣ���Ŀ�� ��Ŀ������
*  FD1001     fd1002    FD44      FD45     FD64    FD65
*  ������Ϣ���ͣ��������ʣ��跽���������������������������޶�
*   FD73          fd1005    fd56      fd57     fd25  fd1003
*  �ۼƽ��׽��޶����ڣ��޶ʼ���ڣ��޶��������,����
*  fd1004          fd1007   FD46           FD47       fd1006
*/

	/***�������޶�Ϊ����,���Ϊ��������ǰ̨ add by wanglei 20061123***/
	vtcp_log("[%s][%d] lim_amt = %.2f\n",__FILE__,__LINE__,sZjgl_mst.lim_amt);
	if(sZjgl_mst.lim_amt < 0)
	{
		sZjgl_mst.lim_amt = -sZjgl_mst.lim_amt;
	}
	vtcp_log("[%s][%d] lim_amt = %.2f\n",__FILE__,__LINE__,sZjgl_mst.lim_amt);

	set_zd_double("1001",sZjgl_mst.dr_bal);
	set_zd_double("1002",sZjgl_mst.cr_bal);
	set_zd_double("1003",sZjgl_mst.lim_amt); /* �����޶� */
	set_zd_double("1004",sZjgl_mst.lim_bal); /* �ۼƽ��׽�� */
	set_zd_double("1005",sZjgl_mst.rate); /* ���� */
	set_zd_data("100A",cCur_no);/* ���� */
	set_zd_data("100B",sZjgl_mst.lim_term); /* ���� */
	set_zd_data("0250",sZjgl_mst.name); /* ���� */
	set_zd_long("0440",sZjgl_mst.opn_date); /* �������� */
	set_zd_long("0450",sZjgl_mst.ic_date); /* ��Ϣ���� */
	set_zd_long("0460",sZjgl_mst.lim_beg_date); /* �޶ʼ���� */
	set_zd_long("0470",sZjgl_mst.lim_end_date); /* �޶�������� */
	set_zd_double("0560",sZjgl_mst.dr_intst_acm); /* �跽���� */
	set_zd_double("0570",sZjgl_mst.cr_intst_acm); /* �������� */
	set_zd_data("0650",sZjgl_mst.acc_hrt); /* ��Ŀ������ */
	set_zd_data("0730",sZjgl_mst.intst_knd); /* ��Ϣ���� */
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

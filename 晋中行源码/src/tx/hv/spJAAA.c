/*************************************************
* �� �� ��:  spJAAA.c
* ���������� ������ʴ�ӡ
* ��    ��:  ligl
* ������ڣ� 2006��8��29��
* �޸ļ�¼�� 
*   ��    ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"
spJAAA()
{
	int ret=0,i=0;
	struct hv_zf_c  hv_zf;
	struct hv_fzf_c hv_fzf;
	HV_FD123_AREA fd123;
	char cBr_no[6];/*������*/
	char cTel[5];  /*����Ա��*/
	char cTx_date[9];/*��������*/
	char cOrderno[8];/*˳���*/
	char cNextOrderno[9];/*��һ��˳���*/
	char cMinno[9]; /*��С���*/
	char cTemno[9]; /*��ʱ���*/
	char cTxstat[2];/*����״̬*/
	char cCmtno[4];/*��Ʊ����*/
	char cVtype[4];/*ƾ֤����*/
	char cVocnum[17];/*ƾ֤����*/
	char cNextVocnum[17];/*��һ��ƾ֤����*/
	char cZf_type[2];
	char wherelist[1024];  /**��ѯ����**/
	memset(&fd123, ' ',sizeof(fd123));
	memset(cBr_no     ,0, sizeof(cBr_no));
	memset(cTel     ,0, sizeof(cTel));
	memset(cTx_date   ,0, sizeof(cTx_date));	
	memset(cVtype     ,0, sizeof(cVtype));
	memset(cTxstat    ,0, sizeof(cTxstat));
	memset(cCmtno     ,0, sizeof(cCmtno));
	memset(cVocnum    ,0, sizeof(cVocnum));
	memset(cOrderno   ,'0', sizeof(cOrderno));
	memset(cTemno     ,0,sizeof(cTemno));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(cNextOrderno,0,sizeof(cNextOrderno));
	memset(cNextVocnum    ,0, sizeof(cNextVocnum));
  memset(cZf_type,0,sizeof(cZf_type));
	vtcp_log("spJAAA��ʼ��[%s][%d]",__FILE__,__LINE__);
	vtcp_log("spJAAA��ʼ��cOrderno=[%s][%s][%d]",cOrderno,__FILE__,__LINE__);
	
	/**��ʼ����**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0070",cTel);
	get_zd_data("0440",cTx_date);
	get_fd_data("0290",cTemno);
	vtcp_log("cOrderno=[%s][%s][%d]",cTemno,__FILE__,__LINE__);
	get_zd_data("0230",cCmtno);
	get_zd_data("0690",cTxstat);
	get_zd_data("0680",cZf_type);
	get_zd_data("0900",cVtype);
	get_zd_data("0600",cVocnum);
	i=strlen(cTemno);
	MEMCPY_DEBUG(cOrderno+8-i,cTemno,sizeof(cTemno)-1);
	MEMCPY_DEBUG(cNextVocnum,cVocnum,sizeof(cNextVocnum)-1);
	vtcp_log("cOrderno=[%d][%s][%d]",i,__FILE__,__LINE__);
	vtcp_log("AP MSG: ��ʼ�������ƾ֤��ӡ����![%s][%d]",__FILE__,__LINE__);
	/***********���ƾ֤*************/
	 ret=pub_com_ChkNoteUse(cVtype);/*���ƾ֤�����Ƿ����*/
	 if( ret )
	 {
		sprintf(acErrMsg,"ƾ֤����ͣ��״̬!");
		WRITEMSG
		goto ErrExit;
	 } 
	ret=pub_com_NoteUse(0,0,cVtype,cVocnum,cVocnum,cTel);/*ƾ֤���Ŵ������׹�����ʹ��ƾ֤��*/
	if( ret )
	{
		sprintf(acErrMsg,"��Աʹ��ƾ֤����!");
		WRITEMSG
		goto ErrExit;
	}   
	/**�õ���һ��ƾ֤��**/
	ret=pub_com_GetNoteMin(cVtype,cNextVocnum,cTel);
	if (ret)
	{
		MEMCPY_DEBUG(cNextVocnum,"û��ƾ֤��",sizeof(cNextVocnum)-1);
		WRITEMSG
		goto ErrExit;
	}
	/***********������*************/
	vtcp_log("cNextVocnum=[%s][%s][%d]",cNextVocnum,__FILE__,__LINE__);
	vtcp_log("AP MSG: �õ�Ҫ��ӡ�ļ�¼![%s][%d]",__FILE__,__LINE__);
	vtcp_log("cOrderno=[%s][%s][%d]",cOrderno,__FILE__,__LINE__);
if(cZf_type[0]=='0')
{		
  ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "br_no='%s' and tx_date='%s' and orderno='%s'",cBr_no,cTx_date,cOrderno);
	vtcp_log("sp9561 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴˴��֧������");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ���֧�����׻�����Ϣ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}		
	vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
	/*���´��֧������*/
	vtcp_log("���´��֧������[%s][%d]",__FILE__,__LINE__);
	ret = sql_execute("update hv_zf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s'",cBr_no,cTx_date,cOrderno);
	if (ret)
	{
		sprintf(acErrMsg,"���´��֧�����׻�����Ϣ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	/**��FD123������**/
  MEMCPY_DEBUG(fd123.cmtno,hv_zf.cmtno,sizeof(fd123.cmtno));
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	MEMCPY_DEBUG(fd123.o_cmtno,hv_zf.o_cmtno,sizeof(fd123.o_cmtno));
	MEMCPY_DEBUG(fd123.pay_qs_no,hv_zf.pay_qs_no,sizeof(fd123.pay_qs_no));
	/**�����к� ԭ�����к� */
	MEMCPY_DEBUG(fd123.or_br_no        ,hv_zf.or_br_no        ,sizeof(fd123.or_br_no        ));
	/**�������Ĵ���**/
	MEMCPY_DEBUG(fd123.sendco          ,hv_zf.sendco          ,sizeof(fd123.sendco          ));
	/**�����˿�����**/
	MEMCPY_DEBUG(fd123.pay_opn_br_no   ,hv_zf.pay_opn_br_no   ,sizeof(fd123.pay_opn_br_no   ));
	/**�������ʺ�(ԭ�տ����ʺ�)**/
	MEMCPY_DEBUG(fd123.pay_ac_no       ,hv_zf.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));
	/**����������(ԭ�տ�������)**/
	MEMCPY_DEBUG(fd123.pay_name        ,hv_zf.pay_name        ,sizeof(fd123.pay_name        ));
	/**�����˵�ַ(ԭ�տ��˵�ַ)**/
	MEMCPY_DEBUG(fd123.pay_addr        ,hv_zf.pay_addr        ,sizeof(fd123.pay_addr        ));
	/**���������к�**/
	MEMCPY_DEBUG(fd123.cash_qs_no      ,hv_zf.cash_qs_no      ,sizeof(fd123.cash_qs_no      ));
	/**�����к�(ԭ�����к�)**/
	MEMCPY_DEBUG(fd123.ac_br_no        ,hv_zf.ac_br_no        ,sizeof(fd123.ac_br_no        ));
	/**�ձ����Ĵ���**/
	MEMCPY_DEBUG(fd123.receco          ,hv_zf.receco          ,sizeof(fd123.receco          ));
	/**�տ��˿����к�**/
	MEMCPY_DEBUG(fd123.cash_opn_br_no  ,hv_zf.cash_opn_br_no  ,sizeof(fd123.cash_opn_br_no  ));
	/**�տ����ʺ�,ԭ�������ʺ�**/
	MEMCPY_DEBUG(fd123.cash_ac_no      ,hv_zf.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));
	/**�տ�������,ԭ����������**/
	MEMCPY_DEBUG(fd123.cash_name       ,hv_zf.cash_name       ,sizeof(fd123.cash_name       ));
	vtcp_log("[%s][%d]cash_name=[%s]",__FILE__,__LINE__,hv_zf.cash_name);
	/**�տ��˵�ַ**/
	MEMCPY_DEBUG(fd123.cash_addr       ,hv_zf.cash_addr       ,sizeof(fd123.cash_addr       ));
	vtcp_log("[%s][%d]cash_addr=[%s]",__FILE__,__LINE__,hv_zf.cash_addr);
	/**ҵ������**/	
	MEMCPY_DEBUG(fd123.yw_type         ,hv_zf.yw_type         ,sizeof(fd123.yw_type         ));
	vtcp_log("[%s][%d]yw_type=[%s]",__FILE__,__LINE__,hv_zf.yw_type);
	/*������� ����֧���������*/
	MEMCPY_DEBUG(fd123.orderno         ,hv_zf.orderno         ,sizeof(fd123.orderno         ));
	vtcp_log("[%s][%d]orderno=[%s]",__FILE__,__LINE__,hv_zf.orderno);
	/**����**/
	MEMCPY_DEBUG(fd123.brf             ,hv_zf.hv_brf          ,sizeof(fd123.brf             ));
	vtcp_log("[%s][%d]hv_brf=[%s]",__FILE__,__LINE__,hv_zf.hv_brf);
	/**ԭ֧���������**/
	MEMCPY_DEBUG(fd123.o_orderno       ,hv_zf.o_orderno       ,sizeof(fd123.o_orderno       ));
	fd123.operlevel[0]=hv_zf.operlevel[0];
	fd123.proc_sts[0]=hv_zf.hv_sts[0];
	set_zd_double("0390",hv_zf.tx_amt);   /* ���׽�� */
	/*���´��֧������*/
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
	vtcp_log("cNextVocnum=[%s][%s][%d]",cNextVocnum,__FILE__,__LINE__);
	set_zd_data("0961",cNextVocnum);
	}
else  
{		
  ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf, "br_no='%s' and tx_date='%s' and orderno='%s'",cBr_no,cTx_date,cOrderno);
	vtcp_log("sp9561 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴˴��֧������");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ���֧�����׻�����Ϣ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}		
	vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
	/*���´��֧������*/
	vtcp_log("���´��֧������[%s][%d]",__FILE__,__LINE__);
	ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s'",cBr_no,cTx_date,cOrderno);
	if (ret)
	{
		sprintf(acErrMsg,"���´��֧�����׻�����Ϣ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	/**��FD123������**/
  /**cmtno����**/
	MEMCPY_DEBUG(fd123.cmtno	,hv_fzf.cmtno		,sizeof(fd123.cmtno));		
	/**o_cmtno**/		
	MEMCPY_DEBUG(fd123.o_cmtno	,hv_fzf.o_cmtno	,sizeof(fd123.cmtno));		
	/**�����к� ԭ�����к� **/		
	MEMCPY_DEBUG(fd123.or_br_no	,hv_fzf.or_br_no,sizeof(fd123.or_br_no));	
	/**�����������к�**/		
	MEMCPY_DEBUG(fd123.pay_qs_no	,hv_fzf.pay_qs_no,sizeof(fd123.pay_qs_no));	
	/**�������Ĵ���**/		
	MEMCPY_DEBUG(fd123.sendco		,hv_fzf.sendco	,sizeof(fd123.sendco));
	/**���������к�**/				
	MEMCPY_DEBUG(fd123.cash_qs_no	,hv_fzf.cash_qs_no,sizeof(fd123.cash_qs_no));		
	/**�����к�(ԭ�����к�)**/	
	MEMCPY_DEBUG(fd123.ac_br_no	,hv_fzf.ac_br_no,sizeof(fd123.ac_br_no));
	/**������� ����֧���������**/			
	MEMCPY_DEBUG(fd123.orderno	,hv_fzf.orderno	,sizeof(fd123.orderno));	
	/**����**/		
	MEMCPY_DEBUG(fd123.brf		,hv_fzf.hv_brf	,sizeof(fd123.brf));	
	/**ԭ֧���������**/			
	MEMCPY_DEBUG(fd123.o_orderno	,hv_fzf.o_orderno,sizeof(fd123.o_orderno));	
	/**��֧������״̬**/		
	fd123.proc_sts[0] = hv_fzf.hv_fzf_sts[0];			
	/**������ʾ**/						
	fd123.lw_ind[0]	= hv_fzf.lw_ind[0];
	/**Ӧ��״̬**/					
	fd123.resp[0]	= hv_fzf.res_sts[0];
	/**Ӧ�����**/
	MEMCPY_DEBUG(fd123.cr_tx_num,hv_fzf.res_orderno,sizeof(hv_fzf.res_orderno)-1);
	/**Ӧ���к�**/
	MEMCPY_DEBUG(fd123.cr_br_no,hv_fzf.res_br_no,sizeof(hv_fzf.res_br_no)-1);
	vtcp_log("[%s][%d]fd123.cr_br_no=[%s]\n",__FILE__,__LINE__,fd123.cr_br_no);
	/**Ӧ������**/
	sprintf(fd123.cr_date,"%8d",hv_fzf.res_date);
	/**ԭ���׽��**/								
	set_zd_double("0390",hv_fzf.o_tx_amt);	
	/**Ӧ������**/
	if(memcmp(cCmtno,"314",sizeof(cCmtno)-1)==0 || memcmp(cCmtno,"302",sizeof(cCmtno)-1)==0 ||memcmp(cCmtno,"312",3)==0)
		MEMCPY_DEBUG(fd123.notpay_content,hv_fzf.res_content,sizeof(fd123.notpay_content));	
	else	/**��������**/
		MEMCPY_DEBUG(fd123.notpay_content,hv_fzf.req_content,sizeof(fd123.notpay_content));


	/* by liuyue 20060922*****�����ѯʱ��ԭ��������ԭ�����е�����*****/
	if(!memcmp(hv_fzf.o_or_br_no,hv_fzf.or_br_no,sizeof(hv_fzf.or_br_no)-1))/**�����о���ԭ������,����ѯ���з����Ľ���**/
	{
		fd123.tx_type[0]	 =  'A';
	}
	else 
		fd123.tx_type[0]	=   'B';
		
	/*���´��֧������*/
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
	vtcp_log("cNextVocnum=[%s][%s][%d]",cNextVocnum,__FILE__,__LINE__);
	set_zd_data("0961",cNextVocnum);
}	   
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
	
}

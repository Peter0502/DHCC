/*****************************************************
* �� �� ��:  sp9560.c
* ���������� ���ݻ�Ʊ�Ų�ѯ��Ʊ������Ϣ
* ��    ��:  ChenMing
* ������ڣ� 2006��8��8�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:  liuyue
* �޸�����:  �����˷�������  Ҳ�����˲�ѯ����
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "hv_brno_orno_c.h"
#include "draw_loan_hst_c.h"

int sp9560()
{
	int ret=0;
	struct hv_poinfo_c hv_poinfo;    
	struct hv_brno_orno_c hv_brno_orno;
	struct draw_loan_hst_c m_draw_loan_hst;
	char cFilename[1000];

	HV_FD123_AREA fd123;

	memset(&m_draw_loan_hst,0x00,sizeof(m_draw_loan_hst));
	memset(&hv_poinfo,'\0',sizeof(hv_poinfo));
	memset(&hv_brno_orno,0x00,sizeof(hv_brno_orno));
	memset(&fd123,0x00,sizeof(fd123));
	memset(cFilename, 0 , sizeof(cFilename));
	vtcp_log("sp9560 ��ʼ��[%s][%d]",__FILE__,__LINE__);

	pub_base_sysinit();

	/****��ȡFD123���е�����*****/
	get_fd_data("1230",(char *)&fd123);

	vtcp_log("[%s][%d]��ǰ̨��ȡ��ǩ���кͻ�Ʊ����!\n",__FILE__,__LINE__);	

	memcpy(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));
	memcpy(hv_poinfo.pay_br_no,fd123.pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	zip_space(hv_poinfo.pay_br_no);
	/*****����ʵ���벻��ʲô�취��,��Ϊ52�����������͵�,��ʹ����ֵҲ���Զ�
	����һ��0,û�а취����2�����汾Ӧ�õ�0��********/
	vtcp_log("hv_poinfo.len==[%d]\n",strlen(hv_poinfo.pay_br_no));

	if (hv_poinfo.pay_br_no[0]==0x00)/****���ǩ����Ϊ��,˵���Ǳ���****/
	{
		vtcp_log("[%s][%d]���ݽ��׻����Ų��ҽ����к�!\n",__FILE__,__LINE__);
		vtcp_log("++++++g_pub_tx.tx_br_no==[%s]!+++++\n",g_pub_tx.tx_br_no);
		ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'  ",g_pub_tx.tx_br_no);
		if (ret)
		{
			sprintf(acErrMsg,"���ݻ������ҽ��׻�������!\n");
			WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		memcpy(hv_poinfo.pay_br_no,hv_brno_orno.or_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	}
	vtcp_log("hv_poinfo.po_no=[%s]  [%s][%d]",hv_poinfo.po_no,__FILE__,__LINE__);
	vtcp_log("hv_poinfo.pay_br_no[%s]  [%s][%d]",hv_poinfo.pay_br_no,__FILE__,__LINE__);

	if ( strlen(hv_poinfo.po_no) == 0 ) 
	{
		sprintf(acErrMsg,"��Ʊ�Ŵ���[%d]",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"P003");
		goto ErrExit;
	}
	if ( strlen(hv_poinfo.pay_br_no) == 0 ) 
	{
		sprintf(acErrMsg,"�������к�\n");
		WRITEMSG
			strcpy(g_pub_tx.reply,"P003");
		goto ErrExit;
	}
	ret = Hv_poinfo_Sel(g_pub_tx.reply,&hv_poinfo, "po_no='%s' and pay_br_no='%s' ",hv_poinfo.po_no ,hv_poinfo.pay_br_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴˻�Ʊ��");
		WRITEMSG
		/* strcpy( g_pub_tx.reply, "MBFE" ); */
			goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ��Ʊ������Ϣ����[%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	
	vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
	set_zd_data("0030",hv_poinfo.br_no);       		/*��������*/
	set_zd_data("0670",hv_poinfo.bank_type);   		/*�б��־*/
	set_zd_data("0680",hv_poinfo.po_sts);   		/*��Ʊ״̬*/
	set_zd_data("0720",hv_poinfo.deal_mode);		/**ת������**/
	set_zd_long("0450",hv_poinfo.reg_date); 		/*�Ǽ�����*/
	set_zd_long("0460",hv_poinfo.sign_date);   		/*ǩ������*/
	set_zd_double("0390",hv_poinfo.po_bal);      	/*��Ʊ���*/
	set_zd_data("0920",hv_poinfo.op_tel);      	/*¼���Ա*/

	fd123.reqtype[0]=hv_poinfo.deal_mode[0]; 
	memcpy(fd123.or_br_no,hv_poinfo.or_br_no,sizeof(fd123.or_br_no)); 
	/*�������к�*/ 
	memcpy(fd123.pay_qs_no,hv_poinfo.pay_qs_no,sizeof(fd123.pay_qs_no)); 
	/*����������***/ 
	memcpy(fd123.pay_opn_br_no,hv_poinfo.pay_br_no,sizeof(fd123.pay_opn_br_no)); /***ǩ����*/ 
	memcpy(fd123.po_no,hv_poinfo.po_no,sizeof(fd123.po_no)); /***Ʊ�ݺ���******/ 
	fd123.po_type[0] = hv_poinfo.po_ind[0]; /***��Ʊ����***/ 
	if (!memcmp(hv_poinfo.cur_id,"RMB",3)) set_zd_data("0210","01"); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /***����***/ 
	memcpy(fd123.pay_ac_no,hv_poinfo.pay_ac_no,sizeof(fd123.pay_ac_no)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*�����ʺ�***/ 
	memcpy(fd123.pay_name,hv_poinfo.pay_name,sizeof(fd123.pay_name)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*������*/ 
	memcpy(fd123.pay_addr,hv_poinfo.pay_addr,sizeof(fd123.pay_addr)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*�����ַ*/ 
	memcpy(fd123.cash_ac_no,hv_poinfo.cash_ac_no,sizeof(fd123.cash_ac_no)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*�տ��ʺ�*/ 
	memcpy(fd123.cash_name ,hv_poinfo.cash_name,sizeof(fd123.cash_name)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*�տ���*/ 
	memcpy(fd123.cash_addr,hv_poinfo.cash_addr,sizeof(fd123.cash_addr)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /*�����ַ*/ 
	memcpy(fd123.cash_opn_br_no,hv_poinfo.cash_br_no,sizeof(fd123.cash_opn_br_no)); 
	vtcp_log(" [%s][%d] cashbrno=[%s]",__FILE__,__LINE__,hv_poinfo.cash_br_no); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /***�Ҹ��к�***/ 
	memcpy(fd123.brf,hv_poinfo.brf,sizeof(fd123.brf)); 
	vtcp_log(" [%s][%d]",__FILE__,__LINE__); /***ժҪ****/

	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]fd123==[%s]\n",(char *)&fd123);
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	set_zd_data("1230",(char *)&fd123);/**����123��**/
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	
	/*add by gong 20110803 �������� ���Ŵ�ϵͳ����*/
	
	ret=Draw_loan_hst_Sel(g_pub_tx.reply,&m_draw_loan_hst,"brf1 like 'YH________%s' ",hv_poinfo.po_no);
	if(ret){
		if(ret==100){
			set_zd_data("0830","");
			sprintf(acErrMsg,"û�ҵ���po_no=[%s]",hv_poinfo.po_no);
			WRITEMSG
		}else{
			sprintf(acErrMsg,"��ѯ�Ŵ�ϵͳ�����������[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	set_zd_data("0830",m_draw_loan_hst.draw_no);
	/*end by gong 20110803 �������� ���Ŵ�ϵͳ����*/
	OkExit:
		strcpy( g_pub_tx.reply, "0000" );
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);	
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	/* begin add by LiuHuafeng 2006-9-6 15:48 */
	{
		FILE *fpw;		
		/* д���ļ�����ȥ */
		ret=pub_base_AllDwnFilName(cFilename);
		if(ret)
		{
			vtcp_log("%s,%d �����´��ļ����� ret=%d ",__FILE__,__LINE__,ret);
		}
		fpw=fopen(cFilename,"w");
		if(fpw==NULL)
		{
			vtcp_log("%s,%d ���´��ļ����� ret=%d ",__FILE__,__LINE__,ret);
		}
		if(memcmp(fd123.cmtno,"121",3)==0)
		{
			HV_121_ADD wd_add;
			memset(&wd_add, 0 , sizeof(wd_add));
			memcpy(wd_add.spec_br_no,hv_poinfo.cash_br_no,sizeof(wd_add.spec_br_no));
			vtcp_log("%s,%d cash_br_no=[%s]",__FILE__,__LINE__,hv_poinfo.cash_br_no);
			apitoa(hv_poinfo.reg_date,sizeof(wd_add.pj_date),wd_add.pj_date);
			memcpy(wd_add.pj_num,hv_poinfo.po_no,sizeof(wd_add.pj_num));
			wd_add.hp_type[0]=hv_poinfo.po_ind[0];
			memcpy(wd_add.hp_miya,hv_poinfo.pwd,sizeof(wd_add.hp_miya));
			pub_base_full_space((char*)&wd_add,sizeof(wd_add));
			vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FILE__,__LINE__,(char *)&wd_add);
			fprintf(fpw,"%s",(char*)&wd_add);
			set_zd_data( DC_FILE_SND,"2" );
		}
		
		if(memcmp(fd123.cmtno,"724",3)==0)
		{
			double dPobal=0.0;
			HV_724_ADD wd_add;
			memset(&wd_add, 0 , sizeof(wd_add));
			dPobal=hv_poinfo.po_bal*100;

			apitoa(hv_poinfo.reg_date,sizeof(wd_add.pj_date),wd_add.pj_date);
			memcpy(wd_add.pj_num,hv_poinfo.po_no,sizeof(wd_add.pj_num));
			wd_add.hp_type[0]=hv_poinfo.po_ind[0];
			memcpy(wd_add.df_br_no,hv_poinfo.cash_br_no,sizeof(wd_add.df_br_no));
			dPobal=hv_poinfo.po_bal*100;
			dbl2str(&dPobal,0,sizeof(wd_add.cp_amt),0,wd_add.cp_amt);
			memcpy(wd_add.hp_miya,hv_poinfo.pwd,sizeof(wd_add.hp_miya));
			memcpy(wd_add.pay_opn_br_no,hv_poinfo.or_br_no,sizeof(wd_add.pay_opn_br_no));
			vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FILE__,__LINE__,(char *)&wd_add);
			pub_base_full_space((char*)&wd_add,sizeof(wd_add));
			fprintf(fpw,"%s",(char*)&wd_add);
			set_zd_data( DC_FILE_SND,"2" );
		}
		fclose(fpw);
	}
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	vtcp_log(" [%s][%d]",__FILE__,__LINE__);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


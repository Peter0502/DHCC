/*************************************************
* �� �� ��:  sp6671.c
* ���������� �ͻ�����ѯ
*
* ��    ��:  ���
* ������ڣ� 20110720
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��: 
* �޸�����:
* ˵����tx_code='6671' sub_tx_code='6671'
        insert into tx_def values ('6671', '�Ŵ��ͻ�����ѯ', '10000000000000000000000000010000000000100000000000010000000000000000000000000000000000000000000000000000000000000000000000100000', '0', '4', '0');
        insert into tx_flow_def values('6671','0','6671','#$');
        insert into tx_sub_def values ('6671', '�Ŵ��ͻ�����ѯ', 'sp6671', '0', '0');
        ���룺0280���ÿͻ���(long)��0670����Ҫ��ѯ������� '0'������'1'������0900��char����ѯ����:"0"���ڣ�"1"���ڣ�"2"��֤��"3"ί�д��
        �����0280��ͻ��ű��ֲ��� 0520��(int)������ 0390��double������ܶ� 
              1230�� �975λ ��ÿһ���˺������� �˺�1|���1|�˺�2|���2|���� �ķ�ʽ������͡�
*************************************************/
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "td_mst_c.h"
#include "in_mst_c.h"
#include "cif_basic_inf_c.h"
int sp6671(){
	int ret=0;
	char flag[2]={'a','\0'};/*������͡�*/
	char flg[2];   /*90����Ʊ�ʶ"0"���ڣ�"1"���ڣ�"2"��֤�� ��"3"ί�д��20110830*/
	int count=0;/*ͳ�ƻ��ڴ�����*/
	int tdcount=0;/*ͳ�ƶ��ڴ�����*/
	double sum=0.0;/*���˻����*/
	char msg[1000];/*�����Ŵ����˺������Ϣ*/
	struct dd_mst_c m_dd_mst;/*m_ ��ͷ��ʾMy ˽�б���*/
	struct mdm_ac_rel_c m_mdm_ac_rel;
	struct td_mst_c	 m_td_mst;
	struct in_mst_c	 m_in_mst;
	struct cif_basic_inf_c sCif_basic_inf;
	memset(msg,'\0',sizeof(msg));
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&m_dd_mst,0,sizeof(m_dd_mst));
	memset(&sCif_basic_inf,0,sizeof(sCif_basic_inf));
	memset(&m_td_mst,0,sizeof(m_td_mst));
	memset(&m_in_mst,0,sizeof(m_in_mst));
	memset(&m_mdm_ac_rel,0,sizeof(m_mdm_ac_rel));
	/*char filename[100];
	FILE *fp=NULL;
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"w" );������*/
	
	get_zd_long("0280",&m_dd_mst.cif_no);
	get_zd_data("0670",flag);
	get_zd_data("0900",flg);
	vtcp_log("[%s][%d] ǰ̨��������flg===[%s]\n",__FILE__,__LINE__,flg);
	sprintf( acErrMsg, "line 48 m_dd_mst.cif_no[%ld] flag[%s]",m_dd_mst.cif_no,flag );
	WRITEMSG
	
	/*m_dd_mst.cif_no=10128189;
	flag='0';������*/
	if ( flg[0]=='0')
	{
		ret=Dd_mst_Dec_Sel(g_pub_tx.reply,"cif_no=%ld and ac_sts='1' ",m_dd_mst.cif_no);
		if(ret){
			sprintf( acErrMsg, "sql error" );
			WRITEMSG
			set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
			goto ErrExit;
		}
		int j=0;/*����sprintf��*/
		double m_bal=0.0;/*�洢�����*/
		while(1){
			ret=Dd_mst_Fet_Sel(&m_dd_mst,g_pub_tx.reply);
			if(ret){
				if(ret==100){
					if(!count){
						set_zd_data("0130","XD00");/*��ʾû�в鵽���*/
					}					
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&m_mdm_ac_rel,"ac_id=%d ",m_dd_mst.ac_id);
			if(ret){
				if(ret==100){				
					set_zd_data("0130","XD00");/*��ʾû�в鵽��� �����ϲ��������ac_id����û��ac_no�����*/							
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			if(flag[0]=='0'){
				m_bal=m_dd_mst.bal;/*�������*/
			}
			else{
				if(flag[0]=='1'){
					m_bal=m_dd_mst.bal-m_dd_mst.hold_amt-m_dd_mst.ctl_amt;/*�������-������-Ԥ��Ȩ���*/
				}
				else{
					sprintf(acErrMsg,"flag��־����ִ��ʧ��");
					WRITEMSG
					set_zd_data("0130","ERROR[flag��־����]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			j+=sprintf(msg+j,"%s|%.2lf|",m_mdm_ac_rel.ac_no,m_bal);
			count+=1;
			sum+=m_bal;
		}
		Dd_mst_Clo_Sel();
	}else if (flg[0]=='1'){
		ret=Td_mst_Dec_Sel(g_pub_tx.reply,"cif_no=%ld and ac_sts='1' ",m_dd_mst.cif_no);
		if(ret){
			sprintf( acErrMsg, "sql error" );
			WRITEMSG
			set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
			goto ErrExit;
		}
		int j=0;/*����sprintf��*/
		double m_bal=0.0;/*�洢�����*/
		while(1){
			ret=Td_mst_Fet_Sel(&m_td_mst,g_pub_tx.reply);
			if(ret){
				if(ret==100){
					if(!tdcount){
						set_zd_data("0130","XD00");/*��ʾû�в鵽���*/
					}
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&m_mdm_ac_rel,"ac_id=%d ",m_td_mst.ac_id);
			if(ret){
				if(ret==100){				
					set_zd_data("0130","XD00");/*��ʾû�в鵽��� �����ϲ��������ac_id����û��ac_no�����*/							
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			if(flag[0]=='0'){
				m_bal=m_td_mst.bal;/*�������*/
			}
			else{
				if(flag[0]=='1'){
					m_bal=m_td_mst.bal-m_td_mst.hold_amt-m_td_mst.ctl_amt;/*�������-������-Ԥ��Ȩ���*/
				}
				else{
					sprintf(acErrMsg,"flag��־����ִ��ʧ��");
					WRITEMSG
					set_zd_data("0130","ERROR[flag��־����]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			j+=sprintf(msg+j,"%s|%.2lf|",m_mdm_ac_rel.ac_no,m_bal);
			tdcount+=1;
			sum+=m_bal;
		}
		Td_mst_Clo_Sel();
	}else if (flg[0]=='2'){
		ret=Dd_mst_Dec_Sel(g_pub_tx.reply,"cif_no=%ld and ac_sts='1' and prdt_no in ('131','142') ",m_dd_mst.cif_no);
		if(ret){
			sprintf( acErrMsg, "sql error" );
			WRITEMSG
			set_zd_data("0130","û�в��ҵ���������������");/*��ʾ����ִ��ʧ��*/
			goto ErrExit;
		}
		int j=0;/*����sprintf��*/
		double m_bal=0.0;/*�洢�����*/
		while(1){
			ret=Dd_mst_Fet_Sel(&m_dd_mst,g_pub_tx.reply);
			if(ret){
				if(ret==100){
					if(!count){
						set_zd_data("0130","XD00");/*��ʾû�в鵽���*/
					}					
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&m_mdm_ac_rel,"ac_id=%d ",m_dd_mst.ac_id);
			if(ret){
				if(ret==100){				
					set_zd_data("0130","XD00");/*��ʾû�в鵽��� �����ϲ��������ac_id����û��ac_no�����*/							
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			if(flag[0]=='0'){
				m_bal=m_dd_mst.bal;/*�������*/
			}
			else{
				if(flag[0]=='1'){
					m_bal=m_dd_mst.bal-m_dd_mst.hold_amt-m_dd_mst.ctl_amt;/*�������-������-Ԥ��Ȩ���*/
				}
				else{
					sprintf(acErrMsg,"flag��־����ִ��ʧ��");
					WRITEMSG
					set_zd_data("0130","ERROR[flag��־����]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			j+=sprintf(msg+j,"%s|%.2lf|",m_mdm_ac_rel.ac_no,m_bal);
			count+=1;
			sum+=m_bal;
		}
		Dd_mst_Clo_Sel();
	 
	}else if (flg[0]=='3'){
		vtcp_log("[%s][%d]ȡ��flagΪ3��ֵ",__FILE__,__LINE__);
		ret=Dd_mst_Dec_Sel(g_pub_tx.reply,"cif_no=%ld and ac_sts='1' and prdt_no in ('133','161','162','163','164','165','166','167','168','169','170') ",m_dd_mst.cif_no);
		if(ret){
			sprintf( acErrMsg, "sql error" );
			WRITEMSG
			set_zd_data("0130","û�в��ҵ���������������");/*��ʾ����ִ��ʧ��*/
			goto ErrExit;
		}
		int j=0;/*����sprintf��*/
		double m_bal=0.0;/*�洢�����*/
		while(1){
			ret=Dd_mst_Fet_Sel(&m_dd_mst,g_pub_tx.reply);
			if(ret){
				if(ret==100){
					if(!count){
						set_zd_data("0130","XD00");/*��ʾû�в鵽���*/
					}					
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&m_mdm_ac_rel,"ac_id=%d ",m_dd_mst.ac_id);
			if(ret){
				if(ret==100){				
					set_zd_data("0130","XD00");/*��ʾû�в鵽��� �����ϲ��������ac_id����û��ac_no�����*/							
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			if(flag[0]=='0'){
				m_bal=m_dd_mst.bal;/*�������*/
			}
			else{
				if(flag[0]=='1'){
					m_bal=m_dd_mst.bal-m_dd_mst.hold_amt-m_dd_mst.ctl_amt;/*�������-������-Ԥ��Ȩ���*/
				}
				else{
					sprintf(acErrMsg,"flag��־����ִ��ʧ��");
					WRITEMSG
					set_zd_data("0130","ERROR[flag��־����]");/*��ʾ����ִ��ʧ��*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			j+=sprintf(msg+j,"%s|%.2lf|",m_mdm_ac_rel.ac_no,m_bal);
			count+=1;
			sum+=m_bal;
		}
		Dd_mst_Clo_Sel();

	}
	if(count+tdcount){
	  set_zd_data("1230",msg);/*����1230����900��λ����˼ٶ��������������ʲ�ȥ�ж�msg�ĳ��ȣ�j��ֵ������������ɡ�*/
	  set_zd_double("0390",sum);
	  set_zd_long("0520",count+tdcount);	
	  set_zd_data("0130","0000");
  }else{
  	strcpy( g_pub_tx.reply, "XD00" );
  	set_zd_data("0130","XD00");
  	goto ErrExit;
  }
	/*fprintf(fp,"MSG[%s]��",msg);
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");������*/
	
OKExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
	

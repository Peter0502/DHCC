/*************************************************
* �� �� ��:  sp6670.c
* ���������� ���ҿͻ�����ͻ�����
*
* ��    ��:  ���
* ������ڣ� 20110720
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��: 
* �޸�����:
* ˵����tx_code='6670' sub_tx_code='6670' 
        insert into tx_def values('6670','�Ŵ���ѯ�ͻ���','00000000000000000000000000111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
        insert into tx_flow_def values('6670','0','6670','#$');
        insert into tx_sub_def  values ('6670', '�Ŵ���ѯ�ͻ���', 'sp6670', '0', '0');
        ���룺0670����֤�����ͣ�0620����֤������
        �����0130���������� "0000"�ɹ���"XD00"�޽���� "ERROR"����ʧ�ܣ�
              0270����������0280���ؿͻ��ţ�long�ͣ�
*************************************************/
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
int sp6670(){
	struct cif_id_code_rel_c cif_id_code_rel;
	struct cif_basic_inf_c cif_basic_inf;
	int ret=0;
	char dmz[21];/*����֤*/
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&cif_id_code_rel,0,sizeof(cif_id_code_rel));
	memset(&cif_basic_inf,0,sizeof(cif_basic_inf));
	memset(dmz,'\0',sizeof(dmz));
	
	get_zd_data("0670",cif_id_code_rel.id_type);/* ���֤������ */
	get_zd_data("0620",cif_id_code_rel.id_no);/* ���֤������ */
	sprintf(acErrMsg,"at line36: reply [%s] id_type[%s]  id_no[%s]",g_pub_tx.reply,cif_id_code_rel.id_type,cif_id_code_rel.id_no);
	WRITEMSG
	if(!strcmp(cif_id_code_rel.id_type,"8")){
		if(cif_id_code_rel.id_no[8]=='-'){	
		  cif_id_code_rel.id_no[8]='%';
	  }
	}/*���ǵ��Ŵ����������ݶ���10λ����11277369-X ��֮ǰ�ĺ�������Ϊ9λ ��11277369X �����һ����Ӧ*/
	sprintf(acErrMsg,"at line43: reply [%s] id_type[%s]  id_no[%s]",g_pub_tx.reply,cif_id_code_rel.id_type,cif_id_code_rel.id_no);
	WRITEMSG
				
	
	ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,"id_type='%s' and id_no like '%s' ",cif_id_code_rel.id_type,cif_id_code_rel.id_no);
	if(ret){
		if(ret==100){
			strcpy( g_pub_tx.reply, "XD00" );
			set_zd_data("0130","XD00");/*��ʾû�в鵽���*/			
			goto ErrExit;
		}
		else{
			set_zd_data("0130","ERROR");/*��ʾ����ִ��ʧ��*/
			goto ErrExit;
		}
	}
	sprintf(acErrMsg,"at line55: reply [%s] ret[%d]  cif_no[%ld]",g_pub_tx.reply,ret,cif_id_code_rel.cif_no);
	WRITEMSG
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld ",cif_id_code_rel.cif_no);
	if(ret){
		if(ret==100){
			strcpy( g_pub_tx.reply, "XD00" );
			set_zd_data("0130","XD00");	/*��ʾû�в鵽���*/
			goto ErrExit;
		}
		else{
			set_zd_data("0130","ERROR");/*��ʾ����ִ��ʧ��*/
			goto ErrExit;
		}
	}
	set_zd_long("0280",cif_basic_inf.cif_no);
	set_zd_data("0270",cif_basic_inf.name);
OKExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s] ret[%d] ",g_pub_tx.reply,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	set_zd_data("0130","0000");
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s] ret[%d] ",g_pub_tx.reply,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
		

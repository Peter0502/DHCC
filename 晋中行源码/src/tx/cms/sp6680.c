/*************************************************
* �� �� ��:  sp6680.c
* ���������� �Ŵ��õ�����Ȩ��Ϣ��ѯ
*
* ��    ��:  ���
* ������ڣ� 20110721
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��: 
* �޸�����:
* ˵����tx_code='6680' sub_tx_code='6680'
        insert into tx_def values ('6680', '�Ŵ�����Ȩ��Ϣ��ѯ', '10000000000000000000000000000100000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');
        insert into tx_flow_def values('6680','0','6680','#$');
        insert into tx_sub_def values ('6680', '�Ŵ�����Ȩ��Ϣ��ѯ', 'sp6680', '0', '0');

        ���룺0300�� �24λ char ��Ȩ���
        �����0300�� �24λ char ��Ȩ���(����)
	            0670�� 1λ char ��ǰ״̬
              0���Ǽǣ�1���ɹ���2������
*************************************************/
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
void getstr(char * a,char * b,int m,int n){
	memset(a,'\0',m);
	zip_space(b);
	memcpy(a,b,m-1);	
	memset(b, '\0', n);
}
int sp6680(){
	int ret=0;
	int count=0;
	int flag_sum=0;
	char strtmp[201];
	struct cms_rollback_c m_cms_rollback;
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&m_cms_rollback,0,sizeof(m_cms_rollback));
	memset(strtmp,'\0',sizeof(strtmp));
	get_zd_data( "0300", strtmp );		/* ת����Э���� */
	getstr(m_cms_rollback.cms_sq_no,strtmp,sizeof(m_cms_rollback.cms_sq_no),sizeof(strtmp));
	
	ret=Cms_rollback_Dec_Sel(g_pub_tx.reply,"cms_sq_no ='%s' and (substr(filler,0,1)<>'#' or filler is null )",m_cms_rollback.cms_sq_no);
	if(ret)
	{
		sprintf( acErrMsg, "sql error" );
		WRITEMSG
		set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
		goto ErrExit;
	}
	count=0;
	flag_sum=0;
	while(1){
		ret=Cms_rollback_Fet_Sel(&m_cms_rollback,g_pub_tx.reply);
		if(ret){
			if(ret==100)
			{
				if(!count)
				{
					strcpy( g_pub_tx.reply, "XD00" );
					set_zd_data("0130","XD00");/*��ʾû�в鵽���*/
					goto ErrExit;
				}
			break;
			}
			else{
				sprintf(acErrMsg,"sql error");
				WRITEMSG
				set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
				Cms_rollback_Clo_Sel();
				goto ErrExit;
			}
		}
		if(!strcmp(m_cms_rollback.sts,"2")){/*��һ��2�򷵻�2*/
			set_zd_data("0670","2");
			set_zd_data("0130","0000");
			Cms_rollback_Clo_Sel();
			goto OKExit;
		}
		if(sql_count(m_cms_rollback.beg_table,m_cms_rollback.beg_sql)){
			flag_sum+=1;
		}
		count+=1;		
	}
	if(flag_sum){
		set_zd_data("0670","0");/**�Ǽ� **/
		set_zd_data("0130","0000");
	}else{
		set_zd_data("0670","1");
		set_zd_data("0130","0000");/*��Ϊ0�򷵻�1  �ɹ�*/
	}
	Cms_rollback_Clo_Sel();
		
OKExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s] ret[%d] ",g_pub_tx.reply,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s] ret[%d] ",g_pub_tx.reply,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
	
	
	

/*********************************************************************
* �������� :  ����ӯ
* ��    �� :  2015/05/22
* ����ģ�� : 	�˻������ֻ��Ų�ѯ
* �������� :  spW223.c
* �������� :
* insert into tx_def values ('5070', '�˻������ֻ��Ų�ѯ', '10000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0','4', '0');
* insert into tx_flow_def values ('5070', 0, 'W223', '#$');
* insert into tx_sub_def values ('W223', '�˻������ֻ��Ų�ѯ', 'spW223', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "mob_acct_type_c.h"

int spW223()
{
				struct mdm_ac_rel_c     sMdm_ac_rel;
				struct dd_mst_c         sDd_mst;
				struct mob_acct_type_c  sMob_acct_type;	
				char account[32]={0};
				char temp[100]={0};
				int ret=0;
					
				memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
				memset(&sDd_mst, 0x00, sizeof(sDd_mst));
				memset(&sMob_acct_type, 0x00, sizeof(sMob_acct_type));

        get_zd_data("0300", account);   /*�˺� */
				sprintf(acErrMsg, "Ҫ��ѯ���˺�Ϊ[%s]", account);
				WRITEMSG

				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",account);
				if( ret==100 || ret==1403 )
				{
					vtcp_log("[%s][%d]Mdm_ac_rel��û�в鵽���˺�[%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"D104");
					set_zd_data("0130","û�в鵽����˺�!");	
					goto ErrExit;	
				}
				else if(ret)
				{
					vtcp_log("[%s][%d]Mdm_ac_rel_Sel error ret[%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"W011");
					set_zd_data("0130","���ҽ����˻���ϵ����");	
					goto ErrExit;
				}
				
				ret=0;
				ret = Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type," ac_no='%s' and mob_sts<>'3' ",account);
				if( ret==100 || ret==1403 )
				{
					vtcp_log("[%s],[%d] ���˻�δ��������ǩԼ���ѽ�Լ!",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"D104");
					set_zd_data("0130","���˻�δ��������ǩԼ���ѽ�Լ!");
					goto ErrExit;	
				}
				else if( ret )
				{
					vtcp_log("[%s][%d]��ѯ��Mob_acct_type_Sel����!",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"W011");
					set_zd_data("0130","��ѯ����֪ͨǩԼ����!");	
					goto ErrExit;
				}
				else
				{
					if ( strlen(sMob_acct_type.mobile) > 0)
					{
						vtcp_log("[%s],[%d]���˻���ǩԼ�ֻ���Ϊ[%s]!",__FILE__,__LINE__,sMob_acct_type.mobile);      
						set_zd_data("0380",sMob_acct_type.mobile);
					}
					else
					{
						vtcp_log("[%s],[%d]�˻�ǩԼ�ֻ���Ϊ��[%s]!",__FILE__,__LINE__,strlen(sMob_acct_type.mobile));      
						strcpy(g_pub_tx.reply,"D104");
						set_zd_data("0130","�˻�ǩԼ�ֻ���Ϊ��,����ϵ�Ƽ���!");
						goto ErrExit;	
					}
				}

OkExit:
				set_zd_data("0120","0000");
				vtcp_log("success! 5070 reply is[%s]", g_pub_tx.reply);
        return 0;
ErrExit:
				set_zd_data("0120",g_pub_tx.reply);
				vtcp_log("fail! 5070 reply is[%s]", g_pub_tx.reply);
				return 1;
}
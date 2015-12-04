/*************************************************
* �� �� ��:  pubf_com_chkGZ.c
* ���������� ���˼��
* ��  ��:    ������
* ������ڣ� 2012-2-25 
*
* ������ac_no �˺�
        ac_name ����
        gz_flag 0�ʺű���������黧��1�ʺŻ������붼���2�ʺű��������������������3�ʺŻ�����һ��������,9ǿ�ƹ���
        gz_br_no ƽ̨���� ���˻���
        gz_reason ����ԭ�� ���������أ�1���˺Ų����� 2���������� 3���˻�״̬�Ƿ� 4������״̬�Ƿ� 5�������־�Ƿ� 6��ֹ����־�Ƿ� 7������ԭ�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:Modefied by YangMeng 20140704
* �޸�����:
	�޸Ĺ��˼������
**************************************************/
#include "find_debug.h" 
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "svrpub_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "com_item_c.h"
#include "com_parm_c.h"
#include "prdt_ac_id_c.h"
#include "in_mst_c.h"
int pubf_com_CheckGZ(char *ac_no,char *ac_name,char *gz_flag,char *gz_br_no,char *gz_reason,int parm_seqn)
{
    int ret;
    char cGz_yn[2];/*�Ƿ���Ҫ���� Y�� N��*/
    char sMzCode[5] = {0};
    char sSysCode[5] = {0};
    char cTmp_ac_name[61];
    char cGz_ac_name[61];/*�����˺ŵĻ���*/
    char cOpn_br_no[BRNO_LEN+1];/*�˺ſ�������*/
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;
    struct dd_mst_c    sDd_mst;
    struct com_parm_c   sCom_parm;
    struct in_mst_c sIn_mst;
    struct com_item_c sCom_item;
    /*��ʼ������*/
    memset(cGz_yn,0x00,sizeof(cGz_yn));
    memset(cOpn_br_no,0x00,sizeof(cOpn_br_no));
    memset(cTmp_ac_name,0x00,sizeof(cTmp_ac_name));
    memset(cGz_ac_name,0x00,sizeof(cGz_ac_name));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sCom_parm,0x00,sizeof(sCom_parm));
    memset(&sIn_mst,0x00,sizeof(sIn_mst));
    memset(&sCom_item,0x00,sizeof(sCom_item));
    
    vtcp_log("[%s][%d],ac_no=[%s],ac_name=[%s],gz_flag=[%s],gz_br_no=[%s],parm_seqn=[%d]",\
    __FILE__,__LINE__,ac_no,ac_name,gz_flag,gz_br_no,parm_seqn);
    if(gz_flag[0] == '9')/*�˻�ǿ�ƹ���*/
    {
    	/*�˻�ҵ��ǿ�ƹ�ԭ���׻���*/
    	gz_reason[0] = '7';/*ǿ�ƹ���,added by YangMeng 20130312*/
    	memset(cOpn_br_no, 0x00, sizeof(cOpn_br_no));
        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
        if(ret)
        {
            sprintf(acErrMsg,"�����˺�ѡ��������");
            WRITEMSG
            return 1;
        }
    }
    else
    {
        if(strlen(ac_no) >10)/*���˺�ʱ*/
        {
            ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",ac_no);
            if(ret !=0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"L015");
                sprintf(acErrMsg,"��ѯmdm_ac_rel����");
                WRITEMSG
                return 1;
            }
            else if(ret == 100)
            {
                sprintf(acErrMsg,"�޴��˺ţ����й����ж�");
                WRITEMSG
                if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 �˺Ż�����һ�������͹���*/
                {
                    gz_reason[0] = '1';
                    ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                    if(ret)
                    {
                        sprintf(acErrMsg,"�����˺�ѡ��������");
                        WRITEMSG
                        return 1;
                    }
                    return 0;
                }
                else
                {
                    strcpy(g_pub_tx.reply,"P102");
                    sprintf(acErrMsg,"�˺Ų����ڣ����Ҳ�����");
                    WRITEMSG
                    return 1;
                }
            }
            vtcp_log("[%s][%d],��ʼ������״̬",__FILE__,__LINE__);
            memcpy(cOpn_br_no,sMdm_ac_rel.opn_br_no,sizeof(cOpn_br_no)-1);
            /*Modified by YangMeng 20131024*/
            get_zd_data("0100",sSysCode);
            get_zd_data("0270",sMzCode);
            if ((!memcmp(sSysCode,"HVPS",4) || !memcmp(sSysCode,"BEPS",4)) && !memcmp(sMzCode,"1000",4))
            {
            	if(sMdm_ac_rel.note_sts[0] == '*'){
            		vtcp_log("[%s][%d],�˻������������й����жϣ�",__FILE__,__LINE__);
            		if(gz_flag[0] == '3'||gz_flag[0] == '8'){
            			gz_reason[0] = '4'; 
            			ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
            	        if(ret)
            	        {
            	            sprintf(acErrMsg,"�����˺�ѡ��������");
            	            WRITEMSG
            	            return 1;
            	        }
            	        return 0;
            		}
            		else
            	    {
            	        strcpy(g_pub_tx.reply,"M013");
            	        sprintf(acErrMsg,"����״̬�����������Ҳ�����");
            	        WRITEMSG
            	        return 1;
            	    }
            	}else{
            		vtcp_log("[%s][%d],���������˻��⣬���������жϣ�",__FILE__,__LINE__);
            	}
            	
            }else{
            	if(sMdm_ac_rel.note_sts[0] != '0')
            	{
            	    sprintf(acErrMsg,"�˺Ž���״̬[%s]�����������й����ж�",sMdm_ac_rel.note_sts);
            	    WRITEMSG
            	    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 �˺Ż�����һ�������͹���*/
            	    {
            	        gz_reason[0] = '4';
            	        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
            	        if(ret)
            	        {
            	            sprintf(acErrMsg,"�����˺�ѡ��������");
            	            WRITEMSG
            	            return 1;
            	        }
            	        return 0;
            	    }
            	    else
            	    {
            	        strcpy(g_pub_tx.reply,"M013");
            	        sprintf(acErrMsg,"����״̬�����������Ҳ�����");
            	        WRITEMSG
            	        return 1;
            	    }
            	}
            }/*Modified by YangMeng 20131024*/
            ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id = %ld order by ac_seqn",sMdm_ac_rel.ac_id);
            if(ret !=0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"W013");
                sprintf(acErrMsg,"��ѯprdt_ac_id����");
                WRITEMSG
                return 1;
            }
            else if(ret == 100)
            {
                sprintf(acErrMsg,"prdt_ac_id�޴��˻���¼�����й����ж�");
                WRITEMSG
                if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 �˺Ż�����һ�������͹���*/
                {
                    gz_reason[0] = '1';
                    ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                    if(ret)
                    {
                        sprintf(acErrMsg,"�����˺�ѡ��������");
                        WRITEMSG
                        return 1;
                    }
                    return 0;
                }
                else
                {
                    strcpy(g_pub_tx.reply,"P102");
                    sprintf(acErrMsg,"�˺Ų����ڣ����Ҳ�����");
                    WRITEMSG
                    return 1;
                }
            }
            if(sPrdt_ac_id.ac_id_type[0] == '1')
            {
                ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn =%d",sMdm_ac_rel.ac_id,sPrdt_ac_id.ac_seqn);
                if(ret !=0 && ret != 100)
                {
                    strcpy(g_pub_tx.reply,"X003");
                    sprintf(acErrMsg,"��ѯdd_mst ����");
                    WRITEMSG
                    return 1;
                }
                else if(ret == 100)
                {
                    sprintf(acErrMsg,"�޴˻����˻������й����ж�");
                    WRITEMSG
                    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 �˺Ż�����һ�������͹���*/
                    {
                        gz_reason[0] = '1';
                        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                        if(ret)
                        {
                            sprintf(acErrMsg,"�����˺�ѡ��������");
                            WRITEMSG
                            return 1;
                        }
                        return 0;
                    }
                    else
                    {
                        strcpy(g_pub_tx.reply,"P102");
                        sprintf(acErrMsg,"�˺Ų����ڣ����Ҳ�����");
                        WRITEMSG
                        return 1;
                    }
                }
                vtcp_log("[%s][%d],��ʼ����˻�״̬�������ֹ��״̬",__FILE__,__LINE__);
                /*Modified by YangMeng 20131024*/
                if ((!memcmp(sSysCode,"HVPS",4) || !memcmp(sSysCode,"BEPS",4)) && !memcmp(sMzCode,"1000",4))
                {
                	if(sDd_mst.ac_sts[0] == '5' || sDd_mst.ac_sts[0] == '*'){
                		vtcp_log("[%s][%d],�˻������������й����жϣ�",__FILE__,__LINE__);
                		if(gz_flag[0] == '3'||gz_flag[0] == '8'){
                			gz_reason[0] = '3';
                			ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                			if(ret)
                			{
                				sprintf(acErrMsg,"�����˺�ѡ��������");
                				WRITEMSG
                				return 1;
                			}
                			return 0;
                		}else{
                			strcpy(g_pub_tx.reply,"O085");
                			sprintf(acErrMsg,"�˻�״̬�����������Ҳ�����");
                			WRITEMSG
                			return 1;
                		}
                	}else{
                		/*add by lwb 20141121 ���˺�Ϊ��֤���˺�ʱҲ����*/
                		if(!strcmp(sDd_mst.prdt_no,"131")||!strcmp(sDd_mst.prdt_no,"142"))
                		{
                		  gz_reason[0] = '7';
                        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                        if(ret)
                        {
                            sprintf(acErrMsg,"�����˺�ѡ��������");
                            WRITEMSG
                            return 1;
                        }
                        return 0;		
                		}
                		/*add end */
                		vtcp_log("[%s][%d],���������˻��⣬���������жϣ�",__FILE__,__LINE__);
                	}
                	
                }else{
                	if(sDd_mst.ac_sts[0] != '1')
                	{
                	    sprintf(acErrMsg,"�˻�״̬������ac_sts=��%s�������й����ж�",sDd_mst.ac_sts);
                	    WRITEMSG
                	    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 �˺Ż�����һ�������͹���*/
                	    {
                	        gz_reason[0] = '3';
                	        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                	        if(ret)
                	        {
                	            sprintf(acErrMsg,"�����˺�ѡ��������");
                	            WRITEMSG
                	            return 1;
                	        }
                	        return 0;
                	    }
                	    else
                	    {
                	        strcpy(g_pub_tx.reply,"O085");
                	        sprintf(acErrMsg,"�˻�״̬�����������Ҳ�����");
                	        WRITEMSG
                	        return 1;
                	    }
                	}
                }/*Modified by YangMeng 20131024*/
                if(sDd_mst.hold_sts[0] == '1')
                {
                    sprintf(acErrMsg,"�˻���ȫ������hold_sts=��%s�������й����ж�",sDd_mst.hold_sts);
                    WRITEMSG
                    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 �˺Ż�����һ�������͹���*/
                    {
                        gz_reason[0] = '5';
                        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                        if(ret)
                        {
                            sprintf(acErrMsg,"�����˺�ѡ��������");
                            WRITEMSG
                            return 1;
                        }
                        return 0;
                    }
                    else
                    {
                        strcpy(g_pub_tx.reply,"P110");
                        sprintf(acErrMsg,"�˻���ȫ�����ᣬ���Ҳ�����");
                        WRITEMSG
                        return 1;
                    }
                }
                strcpy(cTmp_ac_name,sDd_mst.name);
                
            }
            else if(sPrdt_ac_id.ac_id_type[0] == '9')
            {
                ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn =%d",sMdm_ac_rel.ac_id,sPrdt_ac_id.ac_seqn);
                if(ret !=0 && ret != 100)
                {
                    strcpy(g_pub_tx.reply,"D103");
                    sprintf(acErrMsg,"��ѯin_mst����");
                    WRITEMSG
                    return 1;
                }
                else if(ret == 100)
                {
                    sprintf(acErrMsg,"�޴��ڲ��˻������й����ж�");
                    WRITEMSG
                    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 �˺Ż�����һ�������͹���*/
                    {
                        gz_reason[0] = '1';
                        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                        if(ret)
                        {
                            sprintf(acErrMsg,"�����˺�ѡ��������");
                            WRITEMSG
                            return 1;
                        }
                        return 0;
                    }
                    else
                    {
                        strcpy(g_pub_tx.reply,"P102");
                        sprintf(acErrMsg,"�޴��ڲ��˺ţ����Ҳ�����");
                        WRITEMSG
                        return 1;
                    }
                }
                vtcp_log("[%s][%d],��ʼ����˻�״̬�������ֹ��״̬",__FILE__,__LINE__);
                if(sIn_mst.sts[0] != '1')
                {
                    sprintf(acErrMsg,"�˻�״̬������ac_sts=��%s�������й����ж�",sIn_mst.sts);
                    WRITEMSG
                    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 �˺Ż�����һ�������͹���*/
                    {
                        gz_reason[0] = '3';
                        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                        if(ret)
                        {
                            sprintf(acErrMsg,"�����˺�ѡ��������");
                            WRITEMSG
                            return 1;
                        }
                        return 0;
                    }
                    else
                    {
                        strcpy(g_pub_tx.reply,"O085");
                        sprintf(acErrMsg,"�ڲ��˺�״̬�����������Ҳ�����");
                        WRITEMSG
                        return 1;
                    }
                }
                strcpy(cTmp_ac_name,sIn_mst.name);
            }
            else
            {
                strcpy(g_pub_tx.reply,"P104");
                sprintf(acErrMsg,"���˺Ų����ڲ��˺ţ����ǻ����˺�,���й����ж�");
                WRITEMSG
                if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 �˺Ż�����һ�������͹���*/
                {
                    gz_reason[0] = '1';
                    ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                    if(ret)
                    {
                        sprintf(acErrMsg,"�����˺�ѡ��������");
                        WRITEMSG
                        return 1;
                    }
                    return 0;
                }
                else
                {
                    strcpy(g_pub_tx.reply,"P104");
                    sprintf(acErrMsg,"���˺Ų����ڲ��˺ţ����ǻ����˺ţ����Ҳ�����");
                    WRITEMSG
                    return 1;
                }
            }
        }
        else
        {
            ret = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no='%s'",ac_no);
            if(ret != 0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"D103");
                sprintf(acErrMsg,"��ѯ��Ŀ�������");
                WRITEMSG
                return 1;
            }
            else if(ret == 100)
            {
                sprintf(acErrMsg,"�޴˿�Ŀ�����й����ж�");
                WRITEMSG
                if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 �˺Ż�����һ�������͹���*/
                {
                    gz_reason[0] = '1';
                    ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                    if(ret)
                    {
                        sprintf(acErrMsg,"�����˺�ѡ��������");
                        WRITEMSG
                        return 1;
                    }
                   /* memcpy(g_pub_tx.reply, "0000", 4);*/
                    return 0;
                }
                else
                {
                    strcpy(g_pub_tx.reply,"P102");
                    sprintf(acErrMsg,"�޴˿�Ŀ�����Ҳ�����");
                    WRITEMSG
                    return 1;
                }
            }
            strcpy(cTmp_ac_name,sCom_item.acc_name);
        }
        dbc_zipspace(ac_name);
        dbc_zipspace(cTmp_ac_name);
        sprintf(acErrMsg,"��ʼ��黧�� cTmp_ac_name=[%s],ac_name=[%s]",cTmp_ac_name,ac_name);
        WRITEMSG
        /*���˱�־gz_flagΪ0��Ҫ���黧��ʱ����*/
        sprintf(acErrMsg,"��ʼ��黧�� gz_flag=[%s]",gz_flag);
        WRITEMSG
        if(gz_flag[0] != '0')
        {
            if(memcmp(cTmp_ac_name,ac_name,sizeof(cTmp_ac_name)-1) != 0)
            {
                sprintf(acErrMsg,"�������������й����ж�");
                WRITEMSG
                if(gz_flag[0] == '2'||gz_flag[0] == '3'||gz_flag[0] == '8') /*�������� Ҫ�����*/
                {
                    gz_reason[0] = '2';
                    /*�˺Ŵ��ڣ�����������������ڸ��˻��Ŀ�������***20130114**YangMeng*/
                    ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                    /* ret = gz_ac_no_sel(parm_seqn,cOpn_br_no,cOpn_br_no,ac_no,ac_name);*/
                    if(ret)
                    {
                        sprintf(acErrMsg,"�����˺�ѡ��������");
                        WRITEMSG
                        return 1;
                    }
                    sprintf(acErrMsg,"���˻���gz_br_no��%s��",gz_br_no);
                    WRITEMSG
                    return 0;
                }
                else
                {
                    strcpy(g_pub_tx.reply,"NXD5");
                    sprintf(acErrMsg,"�������������Ҳ�����");
                    WRITEMSG
                    return 1;
                }
            }
        }
        
        /*���˱�־gz_flagΪ8 ����˺Ŵ���ǿ�ƹ��˵��������� 20130322 xuhaisheng*/
        if(gz_flag[0] == '8')/*�˻�ǿ�ƹ��ˣ�����˻���ȷ ���˵������ṹ������ȷ�ҵ�ǰ̨�����Ĺ��˻�����һ��Ϊ��������*/
        {
            gz_reason[0] = '7';/*��������*/
            ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
            if(ret)
            {
                sprintf(acErrMsg,"�����˺�ѡ��������");
                WRITEMSG
                return 1;
            }
        }
    }
    sprintf(acErrMsg,"���˼�������");
    WRITEMSG
    sprintf(acErrMsg,"ac_no=[%s],ac_name[%s],gz_flag=[%s],gz_br_no=[%s],gz_reason=[%s]",ac_no,ac_name,gz_flag,gz_br_no,gz_reason);
    WRITEMSG
  
    return 0;
}
/**********************************************************
    �����˺�ѡ�� 
    parm_seqn : com_parm �������
    opn_br_no ����������
    gz_br_no : ƽ̨���͵Ĺ��˻���
    gz_ac_no :���������ع����˺�
    gz_ac_name :���������ع����˺Ż���
�޸ģ��Ȳ鿪��������Ӧ�����˺š�����˳��Ĭ��Ϊ �ȿ���������Ȼ����˻���������������ġ�
	����������˻㣬ǩ���������ȸ���ǰ��ǰ�ù��˱�־����chenggx 2013/3/15 11:38:12
**********************************************************/
int gz_ac_no_sel(int parm_seqn,char *gz_br_no,char *opn_br_no,char *gz_ac_no,char *gz_ac_name)
{
    int ret = 0;
    struct com_parm_c sCom_parm;
    struct mdm_ac_rel_c sMdm_ac_rel_gz;
    struct prdt_ac_id_c sPrdt_ac_id_gz;
    struct dd_mst_c sDd_mst_gz;
    struct com_item_c sCom_item_gz;
    struct in_mst_c sIn_mst_gz;
    memset(&sCom_parm,0x00,sizeof(sCom_parm));
    memset(&sMdm_ac_rel_gz,0x00,sizeof(sMdm_ac_rel_gz));
    memset(&sPrdt_ac_id_gz,0x00,sizeof(sPrdt_ac_id_gz));
    memset(&sDd_mst_gz,0x00,sizeof(sDd_mst_gz));
    memset(&sCom_item_gz,0x00,sizeof(sCom_item_gz));
    memset(&sIn_mst_gz,0x00,sizeof(sIn_mst_gz));
    sprintf(acErrMsg,"��ʼ���й����˺ŵ�ѡ��,opn_br_no =[%s],gz_br_no=[%s]",opn_br_no,gz_br_no);
    WRITEMSG
    ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn = %d",opn_br_no,parm_seqn);
    if(ret !=0 && ret != 100)
    {
        strcpy(g_pub_tx.reply,"O005");
        sprintf(acErrMsg,"��ѯcom_parm ����");
        WRITEMSG
        return 1;
    }
    else if(ret == 100)
    {
        sprintf(acErrMsg,"ƽ̨���͵Ļ�����û�ҵ������˺ţ�ѡ���˺ŵĿ�������");
        WRITEMSG
        ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn = %d",gz_br_no,parm_seqn);
        if(ret !=0 && ret != 100)
        {
            strcpy(g_pub_tx.reply,"O005");
            sprintf(acErrMsg,"��ѯcom_parm ����");
            WRITEMSG
            return 1;
        }
        else if(ret == 100)
        {
            sprintf(acErrMsg,"ƽ̨���͵��˺Ų����ڣ�ѡ����������");
            WRITEMSG
            ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn = %d",QS_BR_NO,parm_seqn);
            if(ret)
            {
                strcpy(g_pub_tx.reply,"O005");
                sprintf(acErrMsg,"��ѯcom_parm ����,�򲻴����������Ĺ����˺�ret=[%d]",ret);
                WRITEMSG
                return 1;
            }
        }
    }
    sprintf(acErrMsg,"��ʼ��ȡ���Ĺ����˺Ž��м��scom_parm.val��%s��",sCom_parm.val);
    WRITEMSG
    sprintf(acErrMsg,"���˻���scom_parm.parm_code��%s��",sCom_parm.parm_code);
    WRITEMSG
    strcpy(gz_ac_no,sCom_parm.val);
    strcpy(gz_br_no,sCom_parm.parm_code);
    sprintf(acErrMsg,"���˻���gz_br_no��%s��",gz_br_no);
    WRITEMSG
    /*set_zd_data("#sys77",gz_br_no);���صĹ��˻���*/
    if(strlen(gz_ac_no) >10)
    {
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_gz,"ac_no='%s'",gz_ac_no);
        if(ret !=0 && ret != 100)
        {
            strcpy(g_pub_tx.reply,"L015");
            sprintf(acErrMsg,"��ѯmdm_ac_rel����");
            WRITEMSG
            return 1;
        }
        else if(ret == 100)
        {
            strcpy(g_pub_tx.reply,"P102");
            sprintf(acErrMsg,"�����˺Ų�����");
            WRITEMSG
            return 1;
        }
        vtcp_log("[%s][%d],��ʼ����˺Ž���״̬",__FILE__,__LINE__);
        if(sMdm_ac_rel_gz.note_sts[0] != '0')
        {
            strcpy(g_pub_tx.reply,"M013");
            sprintf(acErrMsg,"�����˺Ž���״̬������");
            WRITEMSG
            return 1;
        }
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id_gz,"ac_id = %ld order by ac_seqn",sMdm_ac_rel_gz.ac_id);
        if(ret !=0 && ret != 100)
        {
            strcpy(g_pub_tx.reply,"W013");
            sprintf(acErrMsg,"��ѯprdt_ac_id����");
            WRITEMSG
            return 1;
        }
        else if(ret == 100)
        {
            strcpy(g_pub_tx.reply,"P102");
            sprintf(acErrMsg,"�����˺Ų�����");
            WRITEMSG
            return 1;
        }
        if(sPrdt_ac_id_gz.ac_id_type[0] == '1')
        {
            ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_gz,"ac_id=%ld and ac_seqn =%d",sMdm_ac_rel_gz.ac_id,sPrdt_ac_id_gz.ac_seqn);
            if(ret !=0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"X003");
                sprintf(acErrMsg,"��ѯdd_mst ����");
                WRITEMSG
                return 1;
            }
            else if(ret == 100)
            {
                strcpy(g_pub_tx.reply,"P102");
                sprintf(acErrMsg,"�����˺Ų�����");
                WRITEMSG
                return 1;
            }
            vtcp_log("[%s][%d],��ʼ����˻�״̬�������ֹ��״̬",__FILE__,__LINE__);
            if(sDd_mst_gz.ac_sts[0] != '1')
            {
                strcpy(g_pub_tx.reply,"O085");
                sprintf(acErrMsg,"�����˺�״̬������");
                WRITEMSG
                return 1;
            }
            if(sDd_mst_gz.hold_sts[0] == '1')
            {
                strcpy(g_pub_tx.reply,"P110");
                sprintf(acErrMsg,"�����˺ű�ȫ������");
                WRITEMSG
                return 1;
            }
            strcpy(gz_ac_name,sDd_mst_gz.name);
        }
        else if(sPrdt_ac_id_gz.ac_id_type[0] == '9')
        {
            ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst_gz,"ac_id=%ld and ac_seqn =%d",sMdm_ac_rel_gz.ac_id,sPrdt_ac_id_gz.ac_seqn);
            if(ret !=0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"D103");
                sprintf(acErrMsg,"��ѯin_mst����");
                WRITEMSG
                return 1;
            }
            else if(ret == 100)
            {
                strcpy(g_pub_tx.reply,"P102");
                sprintf(acErrMsg,"�����˺Ų�����");
                WRITEMSG
                return 1;
            }
            vtcp_log("[%s][%d],��ʼ����˻�״̬�������ֹ��״̬",__FILE__,__LINE__);
            if(sIn_mst_gz.sts[0] != '1')
            {
                strcpy(g_pub_tx.reply,"O085");
                sprintf(acErrMsg,"�����˺�״̬������");
                WRITEMSG
                return 1;
            }
            strcpy(gz_ac_name,sIn_mst_gz.name);
        }
        else
        {
            strcpy(g_pub_tx.reply,"P104");
            sprintf(acErrMsg,"�����˺����ʹ���");
            WRITEMSG
            return 1;
        }
    }
    else
    {
        ret = Com_item_Sel(g_pub_tx.reply,&sCom_item_gz,"acc_no='%s'",gz_ac_no);
        if(ret != 0 && ret != 100)
        {
            strcpy(g_pub_tx.reply,"D103");
            sprintf(acErrMsg,"��ѯ��Ŀ�������");
            WRITEMSG
            return 1;
        }
        else if(ret == 100)
        {
            strcpy(g_pub_tx.reply,"P102");
            sprintf(acErrMsg,"���˿�Ŀ������");
            WRITEMSG
            return 1;
        }
        strcpy(gz_ac_name,sCom_item_gz.acc_name);
    }
    return 0;
}

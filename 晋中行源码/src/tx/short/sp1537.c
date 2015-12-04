/*************************************************
* �� �� ��:    sp1537.c
* ����������   �����ִ��Э���˺���Ϣ��ѯ������������Ϣ
* ��    �ߣ�    wanghongjie 
* ʱ    �䣺    20110426
* insert into tx_def values('1537','��������Ϣ��ѯ','
10000000000010000000000010000000000000011110000000000000001001110000000000000000011000001000000000000000000000000000000000000100','0','4','0');
* insert into tx_flow_def values('1537','0','1537','#$'); 
* insert into tx_sub_def values('1537','��������Ϣ��ѯ','sp1537','0','0');
* lj...mod it .
s*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "mo_prdt_prot_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"
#include "dd_parm_c.h"
#include "mo_prot_ac_rel_c.h"

static struct mdm_ac_rel_c     g_mdm_ac_rel;
static struct dd_mst_c         sDd_mst;
static struct dd_mst_c         g_dd_mst;
static struct mo_prdt_prot_c   sMo_prdt_prot;
static struct cif_email_inf_c  g_cif_email_inf;
static struct cif_addr_inf_c   g_cif_addr_inf;
static struct dd_parm_c        g_dd_parm;
static struct mo_prot_ac_rel_c sMo_prot_ac_rel;
int sp1537()
{
    int      ret = 0, ac_seqn;
    char     netmark[6];
    char     ac_no[20], name[51];
    char     idtype[40];
    double   ac_bal=0.00, ky_bal=0.00;
    char     cif_no[20];
    double   bal=0.00;
    memset(netmark, 0x00, sizeof(netmark));
    memset(ac_no, 0x00, sizeof(ac_no));  
    memset(name, 0x00, sizeof(name));
    memset(idtype ,0x00,sizeof(idtype));
    memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
    memset(&sMo_prdt_prot, 0x00, sizeof(struct mo_prdt_prot_c));
    memset(&sDd_mst, 0x00, sizeof(struct dd_mst_c));
    memset(&g_dd_parm,0x00 , sizeof(struct dd_parm_c));
    memset(&g_dd_mst,       0x00, sizeof(struct dd_mst_c));
    memset(&sMo_prot_ac_rel,0x00, sizeof(struct mo_prot_ac_rel_c));
    
     /** ȡֵ����ֵ **/     
    get_zd_data("0100", netmark); 
    get_zd_data("0300", ac_no);  /* �ʺ� */
    pub_base_old_acno(ac_no);    /* �Ծ��ʺŵĴ��� */

		if ( memcmp(netmark,"netb",4)==0 ) /**����������,ȫ�µķ������ݺʹ���ʽ.LZY 20141120 **/
		{
				    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ac_no);
				    if(ret == 100) 
				    {
				        sprintf(acErrMsg, "���˺Ų�����!!ac_no=[%s]", ac_no);
				        WRITEMSG
				        strcpy(g_pub_tx.reply, "AT01");
								set_zd_data("0130", "���˺Ų�����!");  
				        goto ErrExit;
				    }
				    else if(ret) 
				    {
				        sprintf(acErrMsg, "Mdm_ac_rel_Sel error ret=[%d]", ret);
				        WRITEMSG
				        strcpy(g_pub_tx.reply, "D103");
				        goto ErrExit;
				    }
				    else if ( g_mdm_ac_rel.note_sts[0] !='0' )
				    {
								sprintf(acErrMsg, "���˺�״̬������!!ac_no=[%s]", ac_no);
				        WRITEMSG
				        strcpy(g_pub_tx.reply, "AT01");
								set_zd_data("0130", "���˺�״̬������!");  
				        goto ErrExit;
						}
            /*��ѯ���������ȡ���������ý���*/
            ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if(ret == 100)
            {
                sprintf(acErrMsg, "��¼������.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D104");
								set_zd_data("0130", "���˺Ų�����!");  
                goto ErrExit;
            }
            else if(ret) 
            {
                sprintf(acErrMsg, "Dd_mst_Sel error ret=[%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
            }
				    else if ( sDd_mst.ac_sts[0] !='1' )
				    {
								sprintf(acErrMsg, "���˺�״̬������!!ac_no=[%s]", ac_no);
				        WRITEMSG
				        strcpy(g_pub_tx.reply, "AT01");
								set_zd_data("0130", "���˺�״̬������!");  
				        goto ErrExit;
						}
				    else if ( sDd_mst.hold_sts[0] =='1' || sDd_mst.hold_sts[0] =='2' )
				    {
								sprintf(acErrMsg, "���˺��ѱ���ȫ�����ֻ������!!ac_no=[%s]", ac_no);
				        WRITEMSG
				        strcpy(g_pub_tx.reply, "AT01");
								set_zd_data("0130", "���˺��ѱ���ȫ�����ֻ������!");
				        goto ErrExit;
						}
						
						ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
            set_zd_data("0250", sDd_mst.name);   /*�ͻ�����*/
            set_zd_double("0400",sDd_mst.bal);   /*�˻����*/
            set_zd_double("0410",ky_bal);        /*�˻����ý��*/

        ret = Mo_prdt_prot_Sel(g_pub_tx.reply, &sMo_prdt_prot,"main_ac_id=%d and prot_type='TK7' and sts <> '*'", g_mdm_ac_rel.ac_id);
        if(ret == 100)
        {
            sMo_prdt_prot.sts[0] = 'W';
            set_zd_data("0690",sMo_prdt_prot.sts);/*�˻�״̬��δǩԼ.*/
            goto OkExit;
        }
        else if(ret)
        {
					sprintf(acErrMsg, "Mo_prdt_prot_Sel error ret=[%d]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "D103");
					goto ErrExit;
        }
        else
        {
            set_zd_data("0690",sMo_prdt_prot.sts);/**�˻�״̬ 1��ǩԼ 2����ʱ��Լ.**/        	

            ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel,"prot_type='TK7' and main_ac_id=%ld and sts='1' ",g_mdm_ac_rel.ac_id);
            if(ret)
            {
                sprintf(acErrMsg, "��ѯ����Э���˻����ձ����[%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "X013");
 								set_zd_data("0130", "��ѯ����Э���˻����ձ����!");
                goto ErrExit;
            }
            vtcp_log("�����˻�app_ac_id=��%d��",sMo_prot_ac_rel.app_ac_id);
            /*��ѯ�����˻����*/
            ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",sMo_prot_ac_rel.app_ac_id,sMo_prot_ac_rel.app_ac_seqn);
            if(ret)
            {
                sprintf(acErrMsg, "��ѯ�����������[%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "C115");
								set_zd_data("0130", "��ѯ�������˻�����!");  
                goto ErrExit;
            }
            set_zd_double("0420",g_dd_mst.bal); /**�������˻����**/
						goto OkExit;
        }
    }    
		else  /**����ǰ̨���ϴ���ʽ,д�ĺ��Ұ�.LZY 20141120**/
		{
	   /* �����ʺŲ�ѯ�˻���ź��˻�ID */
	    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ac_no);
	    if(ret == 100) 
	    {
	        sprintf(acErrMsg, "�����ڸ��ʺ�!!ac_no=[%s]", ac_no);
	        WRITEMSG
	        strcpy(g_pub_tx.reply, "AT01");
	        goto ErrExit;
	    }
	    else if(ret) 
	    {
	        sprintf(acErrMsg, "Mdm_ac_rel_Sel error ret=[%d]", ret);
	        WRITEMSG
	        strcpy(g_pub_tx.reply, "D103");
	        goto ErrExit;
	    }
	    set_zd_data("0620",  g_mdm_ac_rel.id_no);     /*֤������*/
	    /**��ѯ������Э����Ϣ..**/
	    ret = Mo_prdt_prot_Sel(g_pub_tx.reply, &sMo_prdt_prot,"main_ac_id=%d and prot_type='TK7' and sts = '1' ", g_mdm_ac_rel.ac_id);
	    if( ret==100 )
	    {
	        ret = Mo_prdt_prot_Sel(g_pub_tx.reply, &sMo_prdt_prot,"main_ac_id=%d and prot_type='TK7' and sts = '2' ", g_mdm_ac_rel.ac_id);
	        if(ret == 100)
	        {
	            /**δǩԼ..�������.���Ե�ǰ̨.**/
	            sMo_prdt_prot.sts[0] = 'W';
	            set_zd_data("0690",sMo_prdt_prot.sts);/*�˻�״̬ Э��״̬.*/
	            goto OkExit;
	        }
	        else if(ret)
	        {
	            sprintf(acErrMsg, "Mo_prdt_prot_Sel error ret=[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "D103");
	            goto ErrExit;
	        }
	        else
	        {
	            /**ȡ��������Ϣ.**/
	            /*��ѯ���������ȡ���������ý���*/
	            ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_sts='1' and ac_id=%ld",g_mdm_ac_rel.ac_id);
	            if(ret == 100)
	            {
	                sprintf(acErrMsg, "��¼������.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D104");
	                goto ErrExit;
	            } 
	            else if(ret) 
	            {
	                sprintf(acErrMsg, "Dd_mst_Sel error ret=[%d]", ret);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D103");
	                goto ErrExit;
	            }
	            set_zd_data("0250", sDd_mst.name);   /*�ͻ�����*/
	            set_zd_double("0430",sDd_mst.bal);   /*���˻����ý��*/
	            /*��ѯ�ͻ���ַ��Ϣ���ȡ��ϵ��ַ*/
	            ret=Cif_addr_inf_Sel(g_pub_tx.reply,&g_cif_addr_inf,"cif_no=%d ",sDd_mst.cif_no);
	            if(ret == 100) 
	            {
	                sprintf(acErrMsg, "��¼������.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D104");
	                goto ErrExit;
	            }
	            else if(ret)
	            {
	                sprintf(acErrMsg, "Cif_addr_inf_Sel error ret=[%d]", ret);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D103");
	                goto ErrExit;
	            }
	            set_zd_data("0820", g_cif_addr_inf.addr);/*��ϵ��ַ*/
	             
	            /*��ѯ�ͻ�������Ϣ���ȡ��ϵ�绰*/
	            ret=Cif_email_inf_Sel(g_pub_tx.reply,&g_cif_email_inf,"cif_no=%d ",sDd_mst.cif_no);
	            if(ret == 100) 
	            {
	                sprintf(acErrMsg, "��¼������.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D104");
	                goto ErrExit;
	            }
	            else if(ret)
	            {
	                sprintf(acErrMsg, "Cif_addr_inf_Sel error ret=[%d]", ret);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D103");
	                goto ErrExit;
	            }
	            set_zd_data("0830", g_cif_email_inf.email);/*�ƶ��绰*/
	            vtcp_log("[%s][%d] ������=[%17.2f]",__FILE__,__LINE__,sMo_prdt_prot.leave_amt);
	            set_zd_double("0410",sMo_prdt_prot.leave_amt);  /*������*/
	            ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel,"prot_type='TK7' and main_ac_id=%ld and sts='1'",g_mdm_ac_rel.ac_id);
	            if(ret)
	            {
	                sprintf(acErrMsg, "��ѯ����Э���˻����ձ����[%d]", ret);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "X013");
	                goto ErrExit;
	            }
	            vtcp_log("�����˻�app_ac_id=��%d��",sMo_prot_ac_rel.app_ac_id);
	            /*��ѯ�����˻����*/
	            ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",sMo_prot_ac_rel.app_ac_id,sMo_prot_ac_rel.app_ac_seqn);
	            if(ret)
	            {
	                sprintf(acErrMsg, "��ѯ�����������[%d]", ret);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "C115");
	                goto ErrExit;
	            }
	            set_zd_double("0420",g_dd_mst.bal);             /*��ת���*/
	        }
	    }
	    else if(ret)
	    {
	        sprintf(acErrMsg, "Mo_prdt_prot_Sel error ret=[%d]", ret);
	        WRITEMSG
	        strcpy(g_pub_tx.reply, "D103");
	        goto ErrExit;
	    }
	    else
	    {
	        /**ȡ��������Ϣ.**/
	        /*��ѯ���������ȡ���������ý���*/
	        ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_sts='1' and ac_id=%ld",g_mdm_ac_rel.ac_id);
	        if(ret == 100)
	        {
	            sprintf(acErrMsg, "��¼������.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "D104");
	            goto ErrExit;
	        } 
	        else if(ret) 
	        {
	            sprintf(acErrMsg, "Dd_mst_Sel error ret=[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "D103");
	            goto ErrExit;
	        }
	        set_zd_data("0250", sDd_mst.name);   /*�ͻ�����*/
	        set_zd_double("0430",sDd_mst.bal);   /*���˻����ý��*/
	        /*��ѯ�ͻ���ַ��Ϣ���ȡ��ϵ��ַ*/
	        ret=Cif_addr_inf_Sel(g_pub_tx.reply,&g_cif_addr_inf,"cif_no=%d ",sDd_mst.cif_no);
	        if(ret == 100) 
	        {
	            sprintf(acErrMsg, "��¼������.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	            WRITEMSG
	            /** ������.
	            strcpy(g_pub_tx.reply, "D104");
	            goto ErrExit;
	            **/
	        }
	        else if(ret)
	        {
	            sprintf(acErrMsg, "Cif_addr_inf_Sel error ret=[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "D103");
	            goto ErrExit;
	        }
	        set_zd_data("0820", g_cif_addr_inf.addr);/*��ϵ��ַ*/
	         
	        /*��ѯ�ͻ�������Ϣ���ȡ��ϵ�绰*/
	        ret=Cif_email_inf_Sel(g_pub_tx.reply,&g_cif_email_inf,"cif_no=%d ",sDd_mst.cif_no);
	        if(ret == 100) 
	        {
	            sprintf(acErrMsg, "��¼������.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	            WRITEMSG
	            /**������.
	            strcpy(g_pub_tx.reply, "D104");
	            goto ErrExit;
	            **/
	        }
	        else if(ret) 
	        {
	            sprintf(acErrMsg, "Cif_addr_inf_Sel error ret=[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "D103");
	            goto ErrExit;
	        }
	        set_zd_data("0830", g_cif_email_inf.email);/*�ƶ��绰*/
	        vtcp_log("[%s][%d] ������=[%17.2f]",__FILE__,__LINE__,sMo_prdt_prot.leave_amt);
	        set_zd_double("0410",sMo_prdt_prot.leave_amt);  /*������*/
	        ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel,"prot_type='TK7' and main_ac_id=%ld and sts='1'",g_mdm_ac_rel.ac_id);
	        if(ret)
	        {
	            sprintf(acErrMsg, "��ѯ����Э���˻����ձ����[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "X013");
	            goto ErrExit;
	        }
	        vtcp_log("�����˻�app_ac_id=��%d��",sMo_prot_ac_rel.app_ac_id);
	        /*��ѯ�����˻����*/
	        ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",sMo_prot_ac_rel.app_ac_id,sMo_prot_ac_rel.app_ac_seqn);
	        if(ret)
	        {
	            sprintf(acErrMsg, "��ѯ�����������[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "C115");
	            goto ErrExit;
	        }
	        set_zd_double("0420",g_dd_mst.bal);             /*��ת���*/        
	        
	    }
	    vtcp_log("[%s][%d] zyl-sts = [%s]",__FILE__,__LINE__,sMo_prdt_prot.sts);
	    /**add by jk 20130225 ǩԼ��δ��ת�����,���˻�ȡ��ʹ���˻�����51000 ��ʱ.��ʾΪ��ʱ��Լ**/
	    if(pub_base_CompDblVal(g_dd_mst.bal,0.00)<=0 && pub_base_CompDblVal(sDd_mst.bal-sDd_mst.ctl_amt-sDd_mst.hold_amt,51000.00)<0 && sMo_prdt_prot.sts[0]=='1')
	    {
	        sMo_prdt_prot.sts[0] = 'P';
	    }
	    /**
	    sts=1 ǩԼ.
	    sts=2 ��ʱ��Լ.
	    sts=* ��Լ.�����͵�ǰ̨.
	    sts=W ��Լ��δǩԼ,�����͵�ǰ̨.
	    sts=P ǩԼ״̬,��Ϊ��ʱ��Լ״̬.
	    **/
	    /**end by zyl 20110513**/
	    vtcp_log("[%s][%d] zyl-sts = [%s]",__FILE__,__LINE__,sMo_prdt_prot.sts);
	    set_zd_data("0690",sMo_prdt_prot.sts);          /*�˻�״̬ Э��״̬.*/ 
	    bal=sDd_mst.bal+g_dd_mst.bal;
	    set_zd_double("0400", bal);                     /*�����*/
		}
    
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}

/*************************************************************
* �� �� ��:  sp9303.c
* ���������a ��ͨ�ۻ�
*
*************************************************************/

#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "td_mst_c.h" 
int sp9303()
{
	      struct  dd_mst_c   g_dd_mst;
        struct mdm_ac_rel_c  g_mdm_ac_rel;
        struct td_mst_c    g_td_mst; 
        
        int ret=0;
        int i=1;    long ac_seqn;
        double sum=0.00;

        memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
        memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
        memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	      pub_base_sysinit();
       
       get_zd_data("0310",g_pub_tx.ac_no);
       get_zd_long("0340",&ac_seqn);
         vtcp_log("%s %d oooooooooooo ===[%ld] ",__FILE__,__LINE__,g_pub_tx.ac_no);
         vtcp_log("%s %d �ʻ����aaa===[%ld]",__FILE__,__LINE__,ac_seqn);
        ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
               if(ret==100){
                   sprintf(acErrMsg,"���ݿ��޴˼�¼");
                   WRITEMSG
                   goto ErrExit;
                }
                if(ret!=0){
                    sprintf(acErrMsg,"��ѯ���ʹ�ϵ���ձ����");
                    WRITEMSG
                    goto ErrExit;
                }
                
         if(ac_seqn==1)/* ���� */
         	{
            g_dd_mst.ac_id=g_mdm_ac_rel.ac_id;
            vtcp_log("%s %d xxxxxxxxxxxx ===[%ld] ",__FILE__,__LINE__,g_mdm_ac_rel.ac_id);
            vtcp_log("%s %d oooooooooooo ===[%ld] ",__FILE__,__LINE__,g_dd_mst.ac_id);
            
            /** �����ѯ **/
            /*ret=sql_count("dd_mst","ac_id=%ld   and ( hold_sts='1' or hold_sts='2')  ",g_dd_mst.ac_id);
            ret=sql_count("dd_mst","ac_id=%ld   and ( hold_sts='1' or hold_sts='2')  ",g_dd_mst.ac_id);
                 if(ret<0)
                 	{
                     sprintf(acErrMsg,"����!");
                     WRITEMSG
                     goto ErrExit;
                  }
                if(ret>0)
                	{
                     sum=0.00;
                     vtcp_log("%s %d ��������==[%.2f] ",__FILE__,__LINE__,sum); 
                     set_zd_double("0390",sum);
                     goto  OkExit;
                  }  */

             ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld  " , g_dd_mst.ac_id);
                 if(ret==100)
                 	{
                           sprintf(acErrMsg,"���ݿ��޴˼�¼");
                           WRITEMSG
                           goto  ErrExit;
                  }
                 if(ret!=0)
                 	{
                           sprintf(acErrMsg,"��ѯ�������");
                           WRITEMSG
                           goto ErrExit;
                  }
                  
                  if(g_dd_mst.hold_sts[0]=='1')
                  {
                      sprintf(acErrMsg,"���˺��ѱ���ȫ����!!! [%s]",g_dd_mst.hold_sts);
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P110");
                      goto ErrExit;
                  }
                  if(g_dd_mst.hold_sts[0]=='2')
                  {
                      sprintf(acErrMsg,"���˻�����ֻ����������״̬!!! [%s]",g_dd_mst.hold_sts);
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P111");
                      goto ErrExit;
                  }
         
             sum=g_dd_mst.bal-g_dd_mst.hold_amt;
  
                      
             vtcp_log("%s %d shishislhislhishi==[%.2f] ",__FILE__,__LINE__,sum);
             set_zd_double("0390",sum);
        }
     
      
      if(ac_seqn==2)/* ���� */
      	{
          g_td_mst.ac_id=g_mdm_ac_rel.ac_id;
          /*ret=sql_count("td_mst","ac_id=%ld   and ( hold_sts='1' or hold_sts='2')  ",g_td_mst.ac_id);
             if(ret<0)
             	{
                          sprintf(acErrMsg,"����!");
                          WRITEMSG
                          goto ErrExit;
              }
             if(ret>0)
             	{
                         sum=0.00;
                         set_zd_double("0390",sum);
                         goto  OkExit;
              }*/
          ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld  " , g_td_mst.ac_id);
            if(ret==100)
              {
                         sprintf(acErrMsg,"���ݿ��޴˼�¼");
                         WRITEMSG
                         goto  ErrExit;
              }
            if(ret!=0)
            	{
                         sprintf(acErrMsg,"��ѯ�������");
                         WRITEMSG
                         goto ErrExit;
              }
              
            if(g_td_mst.hold_sts[0]=='1')
                  {
                      sprintf(acErrMsg,"���˺��ѱ���ȫ����!!! [%s]",g_td_mst.hold_sts);
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P110");
                      goto ErrExit;
                  }
            if(g_td_mst.hold_sts[0]=='2')
                  {
                      sprintf(acErrMsg,"���˻�����ֻ����������״̬!!! [%s]",g_td_mst.hold_sts);
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P111");
                      goto ErrExit;
                  }
                  
              sum=g_td_mst.bal-g_td_mst.hold_amt;
              vtcp_log("%s %d kshishislhislhishi==[%.2f] ",__FILE__,__LINE__,sum);
              set_zd_double("0390",sum);
          }         
         

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

/*************************************************************
* �� �� ��:  sp9202.c
* ���������a �ɿۻ�����ѯ
*
*************************************************************/

#define EXTERN
#include "public.h"
#include "mo_hold_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"  
int sp9202()
{
	struct mo_hold_c   g_mo_hold;
        struct mdm_ac_rel_c  g_mdm_ac_rel;
        struct dd_mst_c    g_dd_mst;
        struct td_mst_c    g_td_mst;
        int ret=0;
        long  hold_no;
        int i=1;
        double sum=0.00;
        long ac_seqn;
        int  icnt=0;
        char min_ht[2];

        memset(&g_mo_hold,0x00,sizeof(struct mo_hold_c));
        memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	pub_base_sysinit();
       
       get_zd_long("0350",&hold_no);
       get_zd_data("0310",g_pub_tx.ac_no);
       get_zd_long("0340",&ac_seqn);
         vtcp_log("%s %d oooooooooooo ===[%ld] ",__FILE__,__LINE__,hold_no);
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
            g_mo_hold.ac_id=g_mdm_ac_rel.ac_id;
             vtcp_log("%s %d xxxxxxxxxxxx ===[%ld] ",__FILE__,__LINE__,g_mdm_ac_rel.ac_id);
        vtcp_log("%s %d oooooooooooo ===[%ld] ",__FILE__,__LINE__,hold_no);
/** del by martin 2008-11-22 15:35
        ret=sql_count("mo_hold","hold_seqn<=%ld and ( hold_typ='1' or hold_typ='2') and  ac_id=%ld ",hold_no,g_mo_hold.ac_id);
                 if(ret<0){
                            sprintf(acErrMsg,"����!");
                            WRITEMSG
                            goto ErrExit;
                          }
                 if(ret>0){
                            sum=0.00;
                            set_zd_double("0390",sum);
                            goto OkExit;
                          }

          i=1;
          while(i<=hold_no){
         vtcp_log("%s %d pppppppppppp ===[%ld] ",__FILE__,__LINE__,hold_no);
                       ret=Mo_hold_Sel(g_pub_tx.reply,&g_mo_hold,"hold_seqn<=%ld  and  ac_id=%ld   and  hold_sts='0' and  hold_seqn=%d   ",hold_no,g_mo_hold.ac_id,i );

                          if(ret==100){
                             sprintf(acErrMsg,"���ݿ��޴˼�¼");
                             WRITEMSG
                             goto ErrExit;
                          }
                         
                         if(ret!=0){
                             sprintf(acErrMsg,"��ѯ����ȼ�������!");
                             WRITEMSG
                             goto ErrExit;
                         }
                i=i+1;
                sum=sum+g_mo_hold.plan_hold_amt;
                continue;
           }
          
             vtcp_log("%s %d shishislhislhishi==[%.2f] ",__FILE__,__LINE__,sum);
           if(ac_seqn==1){
                    ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ",g_mo_hold.ac_id);
                        if(ret==100){
                           sprintf(acErrMsg,"���ݿ��޴˼�¼");
                           WRITEMSG
                           goto ErrExit;
                        }
                        if(ret!=0){
                          sprintf(acErrMsg,"��ѯ�������");
                          WRITEMSG
                          goto ErrExit;
                        } 
                 sum=g_dd_mst.bal-sum;
           }
           if(ac_seqn==0){
                  ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ", g_pub_tx.ac_id);
                      if(ret==100){
                         sprintf(acErrMsg,"���ݿ��޴˼�¼");
                         WRITEMSG
                         goto ErrExit;
                      }
                      if(ret!=0){
                         sprintf(acErrMsg,"��ѯ�������");
                         WRITEMSG
                         goto ErrExit;
                     }
                sum=g_td_mst.bal-sum;
         }
del by martin end  2008-11-22 15:36*/
	vtcp_log("����pub_acct_check_hold!i=[%d]",i);
	ret=pub_acct_check_hold( i,hold_no,&icnt,&min_ht,&sum,g_mo_hold.ac_id,ac_seqn);/*����ۻ��ⶳ���*/
	vtcp_log("pub_acct_check_hold����[%d]sum==[%lf]!",ret,sum);
	if(ret!=0)
		{
			goto ErrExit;
		}
            set_zd_double("0390",sum);

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
/***********************************
* �� �� ��:     pub_acct_check_hold
* ��������:   ��鶳��ģʽ �ۻ��ⶳ���
*             ���ڸ��µǼǲ��󣬸�������ǰ
*             �ж�֮ǰ�������ǰ�Ľ���
*
* ��    ��:    1��ac_id�ʻ�id
*              2��ac_seqn
*              3��hold_seq
*              
*              mo_ind: 0=���ᣬ1=�ⶳ
*              
* ��    ��:    1�����ض������icnt;
*              2��������󶳽�Ȩ��min_ht;
*              3�����ز��ֶ�����sum_amt;
*              
* ��־�÷���
*
* ��    ��:	    martin 	
* ������ڣ�    2008-11-21 15:32
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
************************************/
int pub_acct_check_hold(
int  mo_ind,
int hold_seqn,
int *icnt,
char *min_ht,
double *sum_amt,
long ac_id,
long ac_seqn )
{
	struct mo_hold_c t_mo_hold;
	struct dd_mst_c  g_dd_mst;
	struct td_mst_c  g_td_mst;
	
	int ret=0;
	memset(&t_mo_hold,0x00,sizeof(struct mo_hold_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	
	/*��ѯ�˻��Ƿ���ڶ����¼ end*/
if( mo_ind>0 )/* �ⶳ */
	{
	/*��ѯ�˻��Ƿ���ڶ����¼*/
		*icnt = sql_count( "mo_hold","ac_id=%ld and ac_seqn=%d and hold_sts='0' and hold_seqn<%d ", \
						ac_id,ac_seqn,hold_seqn);
		if( icnt<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}

			sprintf(acErrMsg,"[%s][%d]������*icnt=[%d]!",__FILE__,__LINE__,*icnt);
			WRITEMSG
			
		ret = sql_min_string("mo_hold","hold_typ", min_ht, sizeof(min_ht)-1,\
                         "ac_id=%ld and ac_seqn=%d and hold_sts='0'  and hold_seqn<%d ",ac_id,ac_seqn,hold_seqn);
     if( ret<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		sprintf(acErrMsg,"[%s][%d]������min_ht=[%s]!",__FILE__,__LINE__,min_ht);
			WRITEMSG

	  ret=sql_sum_double("mo_hold","plan_hold_amt",sum_amt,\
                   "ac_id=%ld and ac_seqn=%d and hold_sts='0'  and hold_seqn<%d ",ac_id,ac_seqn,hold_seqn);
		if( ret<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
			sprintf(acErrMsg,"[%s][%d]�����˽ⶳ*sum_amt=[%lf]!",__FILE__,__LINE__,*sum_amt);
			WRITEMSG
			
		ret = Mo_hold_Dec_Sel(g_pub_tx.reply, \
							"ac_id=%ld and ac_seqn=%d and hold_seqn=%d and hold_sts='0'", \
							ac_id,ac_seqn,hold_seqn);
		if( ret )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ��쳣!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O016");
			goto ErrExit;
		}

		ret = Mo_hold_Fet_Sel(&t_mo_hold,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"--------->[%ld][%d][%d]",ac_id,ac_seqn, hold_seqn );
			WRITEMSG
			sprintf(acErrMsg,"����ǼǱ��в����ڸñʼ�¼!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O019");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"����ǼǱ��в����ڸñʼ�¼!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		
		
		sprintf(acErrMsg,"����ǼǱ��д���[%d]����¼֮ǰ��t_mo_hold.hold_typ=[%s],min_ht=[%s]!",*icnt,t_mo_hold.hold_typ,min_ht);
			WRITEMSG
		/* Ȩ���ж� */
		if( t_mo_hold.hold_typ[0]=='1' )
			{
				if(min_ht[0]=='1')
					{
						sprintf(acErrMsg,"����ǼǱ����Ѵ�����ȫ����!");
			      WRITEMSG
			      strcpy (g_pub_tx.reply,"P110");
			      goto ErrExit;
					}
			}else
		if( t_mo_hold.hold_typ[0]=='2' )
			{
				if(min_ht[0]=='1')
					{
						sprintf(acErrMsg,"����ǼǱ����Ѵ�����ȫ����!");
			      WRITEMSG
			      strcpy (g_pub_tx.reply,"P110");
			      goto ErrExit;
					}else
				if(min_ht[0]=='2')
					{
						sprintf(acErrMsg,"����ǼǱ����Ѵ���ֻ����������!");
			      WRITEMSG
			      strcpy (g_pub_tx.reply,"P111");
			      goto ErrExit;
					}
			}else
		if( t_mo_hold.hold_typ[0]=='3' )
			{
				if(min_ht[0]=='1')
					{
						sprintf(acErrMsg,"����ǼǱ����Ѵ�����ȫ����!");
			      WRITEMSG
			      strcpy (g_pub_tx.reply,"P110");
			      goto ErrExit;
					}else
				if(min_ht[0]=='2')
					{
						sprintf(acErrMsg,"����ǼǱ����Ѵ���ֻ����������!");
			      WRITEMSG
			      strcpy (g_pub_tx.reply,"P111");
			      goto ErrExit;
					}else
				if(min_ht[0]=='3')
					{
					}
					*sum_amt=t_mo_hold.plan_hold_amt;
			}

			/*����ж�*/
			if(ac_seqn==1)
					{
					   ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id='%ld' ",ac_id);
			       if(ret==100){
                       sprintf(acErrMsg,"���ݿ��޴˼�¼");
                       WRITEMSG
                       goto ErrExit;
                    }
                    if(ret!=0){
                       sprintf(acErrMsg,"��ѯ�������");
                       WRITEMSG
                       goto ErrExit;
                   }
					
					   if(*sum_amt>g_dd_mst.bal)
						  {
							  sprintf(acErrMsg,"����ǼǱ��н������ʻ����!");
			          WRITEMSG
			          strcpy (g_pub_tx.reply,"P418");
			          goto ErrExit;
						  }
						  if(t_mo_hold.hold_typ[0]!='3')
					    {
						    *sum_amt=g_dd_mst.bal-*sum_amt;
					    }
						  
					}else
				 if(ac_seqn==0)
				 	{
              ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ",ac_id);
              if(ret==100){
                       sprintf(acErrMsg,"���ݿ��޴˼�¼");
                       WRITEMSG
                       goto ErrExit;
                    }
                    if(ret!=0){
                       sprintf(acErrMsg,"��ѯ�������");
                       WRITEMSG
                       goto ErrExit;
                   }
                   
              if(*sum_amt>g_dd_mst.bal)
						  {
							  sprintf(acErrMsg,"����ǼǱ��н������ʻ����!");
			          WRITEMSG
			          strcpy (g_pub_tx.reply,"D103");
			          goto ErrExit;
						  }
						  /**sum_amt=g_td_mst.bal-*sum_amt;*/
						  if(t_mo_hold.hold_typ[0]!='3')
					    {
						    *sum_amt=g_dd_mst.bal-*sum_amt;
					    }
					}
		
		Mo_hold_Clo_Sel( );
	}
		
	return 0;
ErrExit:
	return -1;
}

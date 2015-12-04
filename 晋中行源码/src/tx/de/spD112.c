/***************************************************************
* �� �� ��: spD112.c
* �����������ͻ�����ά��
* ��    ��: jack
* ������ڣ�2004��09��21��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
***************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "�޸��쳣!!" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include <stdio.h>
#include "public.h"

spD112()
{
  
  int ret=0,flag=0,ac_seqn=0;
  char tmpstr[2],ac_no[20];
  
  memset(&tmpstr,0,sizeof(tmpstr));
  memset(&ac_no,0,sizeof(ac_no));
    
  memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
  memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
  memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    
  pub_base_sysinit();     
     
  get_zd_data("1167",ac_no);
  get_zd_int("103D",&ac_seqn);
  get_zd_data("108P",tmpstr);
  
  pub_base_strpack(ac_no);
  pub_base_old_acno(ac_no);
  
  ret =Mdm_ac_rel_Sel(g_pub_tx.reply ,&g_mdm_ac_rel,"ac_no='%s'" ,ac_no);
  if(ret==100)
  
  {
   sprintf(acErrMsg,"���˺Ų�����,����!!");
   WRITEMSG
   strcpy(g_pub_tx.reply,"L161");
   goto ErrExit; 
  }else ERR_DEAL
  /*-----------��齻��-----------begin*/
  ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",\
                 g_mdm_ac_rel.ac_id,ac_seqn);
  if(ret==0)
  {
   flag=0;
   set_zd_data("0250",g_dd_mst.name);
   switch(g_dd_mst.cal_code[0])
   {
     case '1':set_zd_data("0260","����");break;
     case '2':set_zd_data("0260","��˾");break;
     case '3':set_zd_data("0260","����");break;
     case '4':set_zd_data("0260","ͬҵ");break;
     default:set_zd_data("0260","���޷���");break;
   } 
 }
  else 
   {
    ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d",\
                   g_mdm_ac_rel.ac_id,ac_seqn);
    if(ret==0)
    {
     flag=1;
     set_zd_data("0250",g_td_mst.name);
     switch(g_td_mst.cal_code[0])
     {
       case '1':set_zd_data("0260","����");break;
       case '2':set_zd_data("0260","��˾");break;
       case '3':set_zd_data("0260","����");break;
       case '4':set_zd_data("0260","ͬҵ");break;
       default:set_zd_data("0260","���޷���");break;
     }
    }
    else{ 
          ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=%d",\
                         g_mdm_ac_rel.ac_id,ac_seqn);
          if(ret==0)
          {
           flag=2;
           set_zd_data("0250",g_ln_mst.name);
           switch(g_ln_mst.cal_code[0])
           {
             case '1':set_zd_data("0260","����");break;
             case '2':set_zd_data("0260","��˾");break;
             case '3':set_zd_data("0260","����");break;
             case '4':set_zd_data("0260","ͬҵ");break;
             default:set_zd_data("0260","���޷���");break;
           }
         }
          else{ 
                sprintf(acErrMsg,"���˺Ų�����,����!!");
                strcpy(g_pub_tx.reply,"L161");
                goto ErrExit;
               }
         }
    }
 /*-----------��齻��-----------end*/
 if(strlen(tmpstr)==0) /*��ֹ�����д���ݿ�*/
    goto GoodExit;
 switch(flag)
  {
   case 0:
          ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d",\
                 g_mdm_ac_rel.ac_id,ac_seqn);
         if (ret)
          {
            sprintf(acErrMsg,"DECLARE dd_mst�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"L224");
            goto ErrExit;
          }
          
          ret=Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
          if (ret&&ret!=100)
          {
           sprintf(acErrMsg,"FETCH dd_mst�α��쳣![%s]",g_pub_tx.reply);
           WRITEMSG
           strcpy(g_pub_tx.reply,"L225");
           goto ErrExit;
          }
          strcpy(g_dd_mst.cal_code,tmpstr);
          
          ret=Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
	  if (ret)
	   {
	    sprintf(acErrMsg,"UPDATE dd_mst�α��쳣![%s]",
		   g_pub_tx.reply);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"L227");
	    goto ErrExit;
	   }
	   Dd_mst_Clo_Upd();
	   break;
   case 1:
          ret=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d",\
                             g_mdm_ac_rel.ac_id,ac_seqn);
          if (ret)
          {
            sprintf(acErrMsg,"DECLARE td_mst�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"L224");
            goto ErrExit;
          }
          
          ret=Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
          if (ret&&ret!=100)
          {
           sprintf(acErrMsg,"FETCH td_mst�α��쳣![%s]",g_pub_tx.reply);
           WRITEMSG
           strcpy(g_pub_tx.reply,"L225");
           goto ErrExit;
          }
          strcpy(g_td_mst.cal_code,tmpstr);
          
          ret=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
	  if (ret)
	   {
	    sprintf(acErrMsg,"UPDATE td_mst �α��쳣![%s]",
		   g_pub_tx.reply);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"L227");
	    goto ErrExit;
	   }
	   Td_mst_Clo_Upd();
	   break;
   case 2:
          ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d",\
                             g_mdm_ac_rel.ac_id,ac_seqn);
          if (ret)
          {
            sprintf(acErrMsg,"DECLARE ln_mst�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"L224");
            goto ErrExit;
          }
          
          ret=Ln_mst_Fet_Upd(&g_ln_mst,g_pub_tx.reply);
          if (ret&&ret!=100)
          {
           sprintf(acErrMsg,"FETCH ln_mst�α��쳣![%s]",g_pub_tx.reply);
           WRITEMSG
           strcpy(g_pub_tx.reply,"L225");
           goto ErrExit;
          }
          
          strcpy(g_ln_mst.cal_code,tmpstr);
          ret=Ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
	  if (ret)
	   {
	    sprintf(acErrMsg,"UPDATE ln_mst �α��쳣![%s]",
		   g_pub_tx.reply);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"L227");
	    goto ErrExit;
	   }
	   Ln_mst_Clo_Upd();
	   break;
   default:break;
  }
 GoodExit:
    	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
 ErrExit:
        Dd_mst_Clo_Upd();
        Td_mst_Clo_Upd();
        Ln_mst_Clo_Upd();
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

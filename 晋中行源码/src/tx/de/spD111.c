/***************************************************************
* �� �� ��: spD111.c
* �����������˺���Ϣͳ��
* ��    ��: jack
* ������ڣ�2004��03��16��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
***************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "������ļ���ѯ����!!" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include <stdio.h>
#include "public.h"

char tmp_str1[2],tmp_str2[2],tmpstr[20],tmpstr1[20];
char filename[100];
FILE *fp;   
int ttlnum=0;
double tmp_bal1,tmp_bal2;

spD111()
{   
    int ret=0;
    memset (tmp_str1, 0, sizeof(tmp_str1));
    memset (tmp_str2, 0, sizeof(tmp_str2));
    memset (filename, 0, sizeof(filename));
       
    memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
    memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
    memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
       
    /*** ��ʼ��ȫ�ֱ��� ***/
     
     pub_base_sysinit();     
     
     get_zd_data("101F",tmp_str1);
     get_zd_data("108P",tmp_str2);
     get_zd_data("0910",g_pub_tx.opn_br_no);	
     get_zd_data("1039",g_pub_tx.ac_no);
     
     pub_base_strpack(g_pub_tx.ac_no);
      
     pub_base_AllDwnFilName( filename );
     fp = fopen( filename,"w" );
     if( fp==NULL )
	   {
	    sprintf(acErrMsg," open file [%s] error ",filename );
	    WRITEMSG
	    strcpy( g_pub_tx.reply,"S047" );
	    goto ErrExit;
	   }
     /**��ʾ��ӡ����**/
     fprintf(fp,"~�ͻ�����|@����������|@�˻���|@��Ʒ����|@�ͻ�����|\n"); 
          	     
     switch(tmp_str1[0])
       {
        case '1':
        	switch(tmp_str2[0])
        	{
        	 case '1':                                    /*���˻��ڻ���ѯ*/
         	     ret=pub_trace_query(tmp_str1,tmp_str2);
         	     if(ret==0) goto GoodExit;
        	     ERR_DEAL
        	     break;
                 case '2':                               /*��˾���ڻ���ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	    if(ret==0) goto GoodExit;
        	    ERR_DEAL
                    break;
                 case '3':                               /*�������ڻ���ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	    if(ret==0) goto GoodExit;
        	    ERR_DEAL
                    break;
                 case '4':                              /*ͬҵ���ڻ���ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	    if(ret==0) goto GoodExit;
        	    ERR_DEAL
                    break;
                 case '5':	                           /*���޷�����ڻ���ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	    if(ret==0) goto GoodExit;
        	    ERR_DEAL
                    break;  
                 case '6':	                       /*ȫ�����ڻ���ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	    if(ret==0) goto GoodExit;
        	    ERR_DEAL
                    break;                    
              default:break;
               }
         case '2':                                    
         	switch(tmp_str2[0])
        	{
        	 case '1':                           /*���˶��ڻ���ѯ*/       
         	     ret=pub_trace_query(tmp_str1,tmp_str2);
         	     if(ret==0) goto GoodExit;
        	     ERR_DEAL
        	     break;
           case '2':                            /*��˾���ڻ���ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	 				  if(ret==0) goto GoodExit;
        	    			ERR_DEAL
                    break;
           case '3':                           /*�������ڻ���ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	 				  if(ret==0) goto GoodExit;
        	    			ERR_DEAL
                    break;
           case '4':                           /*ͬҵ���ڻ���ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	 			    if(ret==0) goto GoodExit;
        	    			ERR_DEAL
                    break;
           case '5':	                      /*���޷��ඨ�ڻ���ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	 			    if(ret==0) goto GoodExit;
        	    			ERR_DEAL
                    break; 
           case '6':	                      /*ȫ�����ڻ���ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	 			    if(ret==0) goto GoodExit;
        	    			ERR_DEAL
                    break;                           
           default:break;
              }
         case '3':
         	switch(tmp_str2[0])
        	{
        	 case '1':                               /*���˴����ѯ*/ 
         	     			ret=pub_trace_query(tmp_str1,tmp_str2);
         	     			if(ret==0) goto GoodExit;
        	     			ERR_DEAL
        	     			break;
           case '2':                               /*��˾�����ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	    			if(ret==0) goto GoodExit;
        	    			ERR_DEAL
                    break;
           case '3':                              /*���������ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	          if(ret==0) goto GoodExit;
        	          ERR_DEAL
                    break;
           case '4':                              /*ͬҵ�����ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	    			if(ret==0) goto GoodExit;
        	    			ERR_DEAL
                    break;
           case '5':	                        /*���޷�������ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	 			    if(ret==0) goto GoodExit;
        	    			ERR_DEAL
                    break;  
           case '6':	                       /*ȫ�������ѯ*/
                    ret=pub_trace_query(tmp_str1,tmp_str2);
         	 				  if(ret==0) goto GoodExit;
        	    			ERR_DEAL
                    break;                          
           default:break;
          }
         
       default: break;
    }
      
GoodExit:
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

int pub_trace_query(char *type_ind1, char *type_ind2)
{
  int ret=0;
  
  if (strlen(g_pub_tx.ac_no)!=0 )  
  {
     pub_base_old_acno(g_pub_tx.ac_no);
   	    	                     
     ret = Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_no='%s'" , \
           		             g_pub_tx.ac_no );
     if(ret==100)
       {
         sprintf(acErrMsg,"���˺Ų�����,����!!");
         WRITEMSG
         strcpy(g_pub_tx.reply,"L161");
         goto ErrExit;
       }else ERR_DEAL
   	    	                       
     ttlnum=0;
   	    	      
     switch (type_ind1[0])			  
     {
         case '1':if(strlen(g_pub_tx.opn_br_no)==0)
                  {
                    /*��������ѡ"���޷���"�ͻ�����di����¿�Ϊ�գ������������Ϊ��*/
                    if(type_ind2[0]!='5')         
                    {
                     sprintf(acErrMsg,"�����Ų���Ϊ��!!");
                     WRITEMSG
                     strcpy(g_pub_tx.reply,"D259");
                     goto ErrExit;	
                    }
                    else ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
                                        "ac_id=%ld and cal_code is null",\
   	    	                               g_mdm_ac_rel.ac_id);
                  }else if(type_ind2[0]=='5')
                           ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
                                          "ac_id=%ld and opn_br_no='%s'and cal_code is null",\
   	    	                                g_mdm_ac_rel.ac_id,g_pub_tx.opn_br_no);
   	    	        else  if(type_ind2[0]=='6')
   	    	                 ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
                                          "ac_id=%ld and opn_br_no='%s'",\
   	    	                                g_mdm_ac_rel.ac_id,g_pub_tx.opn_br_no);
   	    	              else ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
                                            "ac_id=%ld and opn_br_no='%s'and cal_code='%s'",\
   	    	                                  g_mdm_ac_rel.ac_id,g_pub_tx.opn_br_no,type_ind2);
                 if(ret==100)
                    {
                      sprintf(acErrMsg,"�����˺Ų�����!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"W014");
                      goto ErrExit;
                    }else ERR_DEAL
   	    	/*----------------��ֵ��ӡ--------------------*/    	 
                  strcpy(tmpstr,"�����˻�");
                  switch (g_dd_mst.cal_code[0])
                    {
                     case '1':strcpy(tmpstr1,"����"); break;
                     case '2':strcpy(tmpstr1,"��˾"); break;
                     case '3':strcpy(tmpstr1,"����"); break;
                     case '4':strcpy(tmpstr1,"ͬҵ"); break;
                     default:strcpy(tmpstr1,"���޷���"); break;	
                    }	
                  
                  fprintf(fp,"%s|%s|%s|%s|%s|\n",g_dd_mst.name,g_dd_mst.opn_br_no,g_mdm_ac_rel.ac_no,tmpstr,tmpstr1);
                  ttlnum++;
   	          break;
         case '2':if(strlen(g_pub_tx.opn_br_no)==0)
                  {
                    if(type_ind2[0]!='5')
                    {
                     sprintf(acErrMsg,"�����Ų���Ϊ��!!");
                     WRITEMSG
                     strcpy(g_pub_tx.reply,"D259");
                     goto ErrExit;	
                    }
                    else ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
                                        "ac_id=%ld and cal_code is null",g_mdm_ac_rel.ac_id);
                  }else if(type_ind2[0]=='5')
                           ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
                                          "ac_id=%ld and opn_br_no='%s'and cal_code is null",\
   	    	                           g_mdm_ac_rel.ac_id,g_pub_tx.opn_br_no);
   	    	        else  if(type_ind2[0]=='6')
   	    	                ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
                                               "ac_id=%ld and opn_br_no='%s'",\
   	    	                                g_mdm_ac_rel.ac_id,g_pub_tx.opn_br_no);
   	    	              else ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
                                                  "ac_id=%ld and opn_br_no='%s'and cal_code='%s'",\
   	    	                                   g_mdm_ac_rel.ac_id,g_pub_tx.opn_br_no,type_ind2);
   	    	         
                  if(ret==100)
                    {
                      sprintf(acErrMsg,"�����˺Ų�����!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"W016");
                      goto ErrExit;
                    }else ERR_DEAL
   	    	  /*----------------��ֵ��ӡ--------------------*/    	 
                  strcpy(tmpstr,"�����˻�");
                  switch (g_td_mst.cal_code[0])
                   {
                    case '1':strcpy(tmpstr1,"����"); break;
                    case '2':strcpy(tmpstr1,"��˾"); break;
                    case '3':strcpy(tmpstr1,"����"); break;
                    case '4':strcpy(tmpstr1,"ͬҵ"); break;
                    default:strcpy(tmpstr1,"���޷���"); break;	
                   }
                  fprintf(fp,"%s|%s|%s|%s|%s|\n",g_td_mst.name,g_td_mst.opn_br_no,g_mdm_ac_rel.ac_no,tmpstr,tmpstr1); 
                  ttlnum++;
   	          break;
         case '3':if(strlen(g_pub_tx.opn_br_no)==0)
                  {
                    if(type_ind2[0]!='5')
                    {
                     sprintf(acErrMsg,"�����Ų���Ϊ��!!");
                     WRITEMSG
                     strcpy(g_pub_tx.reply,"D259");
                     goto ErrExit;	
                    }
                    else ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, \
                                        "ac_id=%ld and cal_code is null",\
   	    	                         g_mdm_ac_rel.ac_id);
                  }else if(type_ind2[0]=='5')
                           ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, \
                                          "ac_id=%ld and opn_br_no='%s'and cal_code is null",\
   	    	                           g_mdm_ac_rel.ac_id,g_pub_tx.opn_br_no);
   	    	        else  if(type_ind2[0]=='6')
   	    	                ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, \
                                              "ac_id=%ld and opn_br_no='%s'",\
   	    	                              g_mdm_ac_rel.ac_id,g_pub_tx.opn_br_no);
   	    	              else ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, \
                                                  "ac_id=%ld and opn_br_no='%s'and cal_code='%s'",\
   	    	                                  g_mdm_ac_rel.ac_id,g_pub_tx.opn_br_no,type_ind2);
   	    	         
                  if(ret==100)
                    {
                      sprintf(acErrMsg,"�����˺Ų�����!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"W018");
                      goto ErrExit;
                    }else ERR_DEAL
                    
   	    	/*----------------��ֵ��ӡ--------------------*/    	 
                  strcpy(tmpstr,"�����˻�");
                  switch (g_ln_mst.cal_code[0])
                   {
                    case '1':strcpy(tmpstr1,"����"); break;
                    case '2':strcpy(tmpstr1,"��˾"); break;
                    case '3':strcpy(tmpstr1,"����"); break;
                    case '4':strcpy(tmpstr1,"ͬҵ"); break;
                    default: strcpy(tmpstr1,"���޷���");break;	
                   }
                 
                 fprintf(fp,"%s|%s|%s|%s|%s|\n",g_ln_mst.name,g_ln_mst.opn_br_no,g_mdm_ac_rel.ac_no,tmpstr,tmpstr1);
                 ttlnum++;
                 break;
       default:  break;        
       }
  } else 
   {
     switch (type_ind1[0])
         {
            case '1':if(strlen(g_pub_tx.opn_br_no)==0)
                      {
                      	if(type_ind2[0]!='5')
                      	{
                          sprintf(acErrMsg,"�����Ų���Ϊ��!!");
                          WRITEMSG
                          strcpy(g_pub_tx.reply,"D259");
                          goto ErrExit;	
                        }else ret=Dd_mst_Dec_Sel(g_pub_tx.reply ,"cal_code is null");
                      }else if(type_ind2[0]=='5')
		                           ret=Dd_mst_Dec_Sel(g_pub_tx.reply ,"cal_code is null and opn_br_no='%s'",\
		                                              g_pub_tx.opn_br_no); 
                            else if(type_ind2[0]=='6')   
		                               ret=Dd_mst_Dec_Sel(g_pub_tx.reply ,"opn_br_no='%s' order by cal_code",\
		                                                  g_pub_tx.opn_br_no); 
                                 else {
                                 	TRACE
                                 	ret=Dd_mst_Dec_Sel(g_pub_tx.reply ,"cal_code='%s' and opn_br_no='%s'",\
                                 	                       type_ind2,g_pub_tx.opn_br_no);
                                 	     }
                      break;
            case '2':if(strlen(g_pub_tx.opn_br_no)==0)
                      {
                      	if(type_ind2[0]!='5')
                      	{
                          sprintf(acErrMsg,"�����Ų���Ϊ��!!");
                          WRITEMSG
                          strcpy(g_pub_tx.reply,"D259");
                          goto ErrExit;	
                        }else ret=Td_mst_Dec_Sel(g_pub_tx.reply ,"cal_code is null");
                      }else if(type_ind2[0]=='5')
                              ret=Td_mst_Dec_Sel(g_pub_tx.reply ,"cal_code is null and opn_br_no='%s'",\
                                                 g_pub_tx.opn_br_no); 
                            else if(type_ind2[0]=='6')   
                                    ret=Td_mst_Dec_Sel(g_pub_tx.reply ,"opn_br_no='%s' order by cal_code",\
                                                       g_pub_tx.opn_br_no); 
                                 else ret=Td_mst_Dec_Sel(g_pub_tx.reply ,"cal_code='%s' and opn_br_no='%s'",\
                                 	                      type_ind2,g_pub_tx.opn_br_no);    
                     break;
            case '3':if(strlen(g_pub_tx.opn_br_no)==0)
                      {
                      	if(type_ind2[0]!='5')
                      	{
                          sprintf(acErrMsg,"�����Ų���Ϊ��!!");
                          WRITEMSG
                          strcpy(g_pub_tx.reply,"D259");
                          goto ErrExit;	
                        }else ret=Ln_mst_Dec_Sel(g_pub_tx.reply ,"cal_code is null");
                      }else if(type_ind2[0]=='5')
                              ret=Ln_mst_Dec_Sel(g_pub_tx.reply ,"cal_code is null and opn_br_no='%s'",\
                                                 g_pub_tx.opn_br_no); 
                            else if(type_ind2[0]=='6')   
                                  ret=Ln_mst_Dec_Sel(g_pub_tx.reply ,"opn_br_no='%s' order by cal_code",\
                                                     g_pub_tx.opn_br_no); 
                                 else ret=Ln_mst_Dec_Sel(g_pub_tx.reply ,"cal_code='%s' and opn_br_no='%s'",\
                                 	                       type_ind2,g_pub_tx.opn_br_no);    
                     break;
            default:break;
         }  
    if(ret)
     {
      sprintf(acErrMsg,"[DECLARE CURSOR]�쳣!!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"D101");
      goto ErrExit;
     }         		
    ttlnum=0;
    
    while(1)   
        {
           switch(type_ind1[0])
            {
              case '1': ret=Dd_mst_Fet_Sel(&g_dd_mst,g_pub_tx.reply);
            	          if(ret==100)
                           {
                            if(!ttlnum)
                            {
                             sprintf(acErrMsg,"���ڲ�Ʒ���ļ��в����ڸü�¼!!");
                             WRITEMSG
                             strcpy(g_pub_tx.reply,"W014");
                             goto ErrExit;
                           }
                           goto GoodExit;
                          }else ERR_DEAL
              
                        ret = Mdm_ac_rel_Sel(g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d" , \
           		                     g_dd_mst.ac_id,g_dd_mst.ac_seqn);
           		          if(ret==100)
           		          {
           		           sprintf(acErrMsg,"���˺Ų�����,����!!");
                         WRITEMSG
                         strcpy(g_pub_tx.reply,"L161");
                         continue;	
           		          }
   	    	       /*----------------��ֵ��ӡ--------------------*/
                        strcpy(tmpstr,"�����˻�");
                        switch (g_dd_mst.cal_code[0])
                        {
                         case '1':strcpy(tmpstr1,"����"); break;
                         case '2':strcpy(tmpstr1,"��˾"); break;
                         case '3':strcpy(tmpstr1,"����"); break;
                         case '4':strcpy(tmpstr1,"ͬҵ"); break;
                         default: strcpy(tmpstr1,"���޷���");break;	
                        } 
						
						            fprintf(fp,"%s|%s|%s|%s|%s|\n",g_dd_mst.name,g_dd_mst.opn_br_no,g_mdm_ac_rel.ac_no,tmpstr,tmpstr1);
                        ttlnum++;
                        break;
             case '2': ret=Td_mst_Fet_Sel(&g_td_mst,g_pub_tx.reply);
            	         if (ret==100)
                          {
                            if(!ttlnum)
                            {
                             sprintf(acErrMsg,"���ڲ�Ʒ���ļ��в����ڸü�¼!!");
                             WRITEMSG
                             strcpy(g_pub_tx.reply,"W016");
                             goto ErrExit;
                           }
                           goto GoodExit;
                          }else ERR_DEAL
                          
                        ret = Mdm_ac_rel_Sel(g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d" , \
           		                               g_td_mst.ac_id,g_td_mst.ac_seqn);
                        if(ret==100)
           		          {
           		           sprintf(acErrMsg,"���˺Ų�����,����!!");
                         WRITEMSG
                         strcpy(g_pub_tx.reply,"L161");
                         continue;	
           		          }
                      /*----------------��ֵ��ӡ--------------------*/
                       strcpy(tmpstr,"�����˻�");
                       switch (g_td_mst.cal_code[0])
                       {
                        case '1':strcpy(tmpstr1,"����"); break;
                        case '2':strcpy(tmpstr1,"��˾"); break;
                        case '3':strcpy(tmpstr1,"����"); break;
                        case '4':strcpy(tmpstr1,"ͬҵ"); break;
                        default: strcpy(tmpstr1,"���޷���");break;	
                       }
					   
         	             fprintf(fp,"%s|%s|%s|%s|%s|\n",g_td_mst.name,g_td_mst.opn_br_no,g_mdm_ac_rel.ac_no,tmpstr,tmpstr1);
                       ttlnum++;
                       break;
              case '3':ret=Ln_mst_Fet_Sel(&g_ln_mst,g_pub_tx.reply);
            	         if (ret==100)
                           {
                            if(!ttlnum)
                            {
                             sprintf(acErrMsg," �����Ʒ���ļ��в����ڸü�¼!!");
                             WRITEMSG
                             strcpy(g_pub_tx.reply,"W018");
                             goto ErrExit;
                            }
                           goto GoodExit;
                          }else ERR_DEAL
                          
                       ret = Mdm_ac_rel_Sel(g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d" , \
           		                              g_ln_mst.ac_id,g_ln_mst.ac_seqn);
                       if(ret==100)
           		         {
           		          sprintf(acErrMsg,"���˺Ų�����,����!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"L161");
                        continue;	
           		         }
                      /*----------------��ֵ��ӡ--------------------*/
                       strcpy(tmpstr,"�����˻�");
                       switch (g_ln_mst.cal_code[0])
                       {
                        case '1':strcpy(tmpstr1,"����"); break;
                        case '2':strcpy(tmpstr1,"��˾"); break;
                        case '3':strcpy(tmpstr1,"����"); break;
                        case '4':strcpy(tmpstr1,"ͬҵ"); break;
                        default: strcpy(tmpstr1,"���޷���");break;	
                       }
					   
         	             fprintf(fp,"%s|%s|%s|%s|%s|\n",g_ln_mst.name,g_ln_mst.opn_br_no,g_mdm_ac_rel.ac_no,tmpstr,tmpstr1);
                       ttlnum++;
                       break;
              default:break;
            }	
      }	
	 }
 GoodExit:
        fclose(fp);
        Dd_mst_Clo_Sel();
	      Td_mst_Clo_Sel();
	      Ln_mst_Clo_Sel();
        set_zd_data(DC_FILE_SND,"1");
	      return 0;
 ErrExit:
        Dd_mst_Clo_Sel();
        Td_mst_Clo_Sel();
        Ln_mst_Clo_Sel();
        return 1;
}                     

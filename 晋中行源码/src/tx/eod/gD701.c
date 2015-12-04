/*************************************************************
* 文 件 名: gD701.c
* 功能描述: 生成人民银行反洗钱大额可疑文件  
*
* 作    者: xxxx
* 完成日期: 2003年10月19日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}

#include <stdio.h>
#include <string.h>
#define EXTERN
#include "public.h"
#include "mas_hvpt_c.h"
#include "mas_hvcs_c.h"
#include "mas_hvcd_c.h"
#include "mas_pbto_c.h"
#include "mas_pato_c.h"

char tmp_n[61],tmp_cnt[7],datalen[13],tmp_amt[16];
int ret,hvpt_cnt,hvcd_cnt,hvcs_cnt,pato_cnt,pbto_cnt;
gD701()
{
 long qry_date=0;
 FILE *fp;
 char filename[30];
 
 struct mas_hvpt_c  g_mas_hvpt;
 struct mas_hvcd_c  g_mas_hvcd;
 struct mas_hvcs_c  g_mas_hvcs;
 struct mas_pbto_c  g_mas_pbto;
 struct mas_pato_c  g_mas_pato; 
 
 memset(filename,0,sizeof(filename)); 
 memset(&g_mas_hvpt,0x00,sizeof(struct mas_hvpt_c));
 memset(&g_mas_hvcs,0x00,sizeof(struct mas_hvcs_c));
 memset(&g_mas_hvcd,0x00,sizeof(struct mas_hvcd_c));
 memset(&g_mas_pbto,0x00,sizeof(struct mas_pbto_c));
 memset(&g_mas_pato,0x00,sizeof(struct mas_pato_c));
 hvpt_cnt=hvcd_cnt=hvcs_cnt=pato_cnt=pbto_cnt=0;
 
 ret = sql_begin();
 if ( ret )
 {
	sprintf( acErrMsg, "打开事务失败!!" );
	WRITEMSG
	goto ErrExit;
 }
 /*** 初始化全局变量 ***/
 pub_base_sysinit();
 /*生成文件名*/
 pub_base_deadlineD( g_pub_tx.tx_date, -1 ,&qry_date );
 sprintf(tmp_n,"d313247332233-%d-01-01.txt",qry_date);
 pub_base_strpack(tmp_n);
 sprintf( filename,"%s/%s",getenv("FILDIR"),tmp_n);
 
 fp=fopen( filename,"w" );

 if( fp==NULL )
	{
	  sprintf(acErrMsg," open file [%s] error ",filename );
	  WRITEMSG
	  strcpy( g_pub_tx.reply,"S047" );
	  goto ErrExit;
	}

 memset(tmp_n,0x20,sizeof(tmp_n));
 strncpy(tmp_n,"市商业银行",14);
 tmp_n[strlen(tmp_n)-1]='\0';
 
 /*写文件头-----------------begin*/
 fprintf(fp,"{\n313247332233\n%s\n%d\n",tmp_n,qry_date);
 
 init_tmp();
 hvpt_cnt=sql_count("mas_hvpt","paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
 if(hvpt_cnt<0) goto ErrExit;
 else if(hvpt_cnt!=0)
    {
	   sprintf(tmp_amt,"%d",hvpt_cnt);
	   strncpy(tmp_cnt,tmp_amt,strlen(tmp_amt));
	   sprintf(tmp_amt,"%d",hvpt_cnt*456);
	   strncpy(datalen,tmp_amt,strlen(tmp_amt));
	   fprintf(fp,"HVPT\n%s\n%s\n",tmp_cnt,datalen);
    }
 
 init_tmp();
 hvcs_cnt=sql_count("mas_hvcs","recvdate='%ld'",qry_date);
 if(hvcs_cnt<0) goto ErrExit;
 else if(hvcs_cnt!=0)
 	{
 		sprintf(tmp_amt,"%d",hvcs_cnt);
 		strncpy(tmp_cnt,tmp_amt,strlen(tmp_amt));
 		sprintf(tmp_amt,"%d",hvcs_cnt*383);
 		strncpy(datalen,tmp_amt,strlen(tmp_amt));
 		fprintf(fp,"HVCS\n%s\n%s\n",tmp_cnt,datalen);
    }
 
 init_tmp();
 hvcd_cnt=sql_count("mas_hvcd","paydate='%ld'",qry_date);
 if(hvcd_cnt<0) goto ErrExit;
 else if(hvcd_cnt!=0)
 	{
 		sprintf(tmp_amt,"%d",hvcd_cnt);
 		strncpy(tmp_cnt,tmp_amt,strlen(tmp_amt));
 		sprintf(tmp_amt,"%d",hvcd_cnt*387);
 		strncpy(datalen,tmp_amt,strlen(tmp_amt));
 		fprintf(fp,"HVCD\n%s\n%s\n",tmp_cnt,datalen);
    }
 /*
 init_tmp();
 tmp=sql_count("mas_ohvc","paydate='%ld' or recvdat
 else if(tmp!=0)e='%ld'",qry_date,qry_date);
 if(tmp<0) goto ErrExit;
 	{
    sprintf(tmp_cnt,"%d",tmp);
    sprintf(datalen,"%d",tmp*504);
    fprintf(fp,"OHVC\n%s\n%s\n",tmp_cnt,datalen);
 	}	
 */
 init_tmp();
 pbto_cnt=sql_count("mas_pbto","paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
 if(pbto_cnt<0) goto ErrExit;
 else if(pbto_cnt!=0)
     {
     	sprintf(tmp_amt,"%d",pbto_cnt);
     	strncpy(tmp_cnt,tmp_amt,strlen(tmp_amt));
     	sprintf(tmp_amt,"%d",pbto_cnt*484);
     	strncpy(datalen,tmp_amt,strlen(tmp_amt));
      fprintf(fp,"PBTO\n%s\n%s\n",tmp_cnt,datalen);
 	   }
 
 init_tmp();
 pato_cnt=sql_count("mas_pato","paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
 if(pato_cnt<0) goto ErrExit;
 else if(pato_cnt!=0)
 	{
 	 sprintf(tmp_amt,"%d",pato_cnt);
   strncpy(tmp_cnt,tmp_amt,strlen(tmp_amt));
   sprintf(tmp_amt,"%d",pato_cnt*512);
   strncpy(datalen,tmp_amt,strlen(tmp_amt));
   fprintf(fp,"PATO\n%s\n%s\n",tmp_cnt,datalen);
   }
   
 init_tmp(); 
 fprintf(fp,"%s%s%s\n}\n{\n",datalen,datalen,datalen);
 /*--------------------------end*/
 init_tmp();
 if(hvpt_cnt!=0)
 	{
 		sprintf(tmp_amt,"%d",hvpt_cnt);
    strncpy(tmp_cnt,tmp_amt,strlen(tmp_amt));
    sprintf(tmp_amt,"%d",hvpt_cnt*456);
    strncpy(datalen,tmp_amt,strlen(tmp_amt));
    fprintf(fp,"HVPT\n%s\n%s\n",tmp_cnt,datalen);
    
    ret=0;
    ret=Mas_hvpt_Dec_Sel(g_pub_tx.reply ,"paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
    if(ret)
         {
          sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
          WRITEMSG
          strcpy(g_pub_tx.reply,"D101");
          goto ErrExit;
         }   
    /*报送报文种类1数据*/
     while(1)
     {
     	ret=Mas_hvpt_Fet_Sel(&g_mas_hvpt,g_pub_tx.reply);
      if (ret==100)
          break;
      else ERR_DEAL
      init_tmp();
      strcpy(tmp_n,g_mas_hvpt.tx_amt); 
      pub_base_strpack(tmp_n);
      tmp_amt[15-strlen(tmp_n)]='\0';
      strcat(tmp_amt,tmp_n);
      fprintf(fp,"||%s||%s||%s||%s||%s||%s||%s||%s||%s||%s||%s||%s%s||%s||%s||%s||%s||%s||%s||%s||\n",\
              g_mas_hvpt.trace_no,g_mas_hvpt.payficcode,g_mas_hvpt.payfictype,g_mas_hvpt.payer_name,\
              g_mas_hvpt.payer_ac_no,g_mas_hvpt.payer_br_no,g_mas_hvpt.origrdficode,g_mas_hvpt.origrdfictype,\
              g_mas_hvpt.payee_name,g_mas_hvpt.payee_ac_no,g_mas_hvpt.payee_br_no,g_mas_hvpt.cur_no,tmp_amt,\
              g_mas_hvpt.paydate,g_mas_hvpt.recvdate,g_mas_hvpt.use,g_mas_hvpt.sttl_type,g_mas_hvpt.notecode,\
              g_mas_hvpt.txcode,g_mas_hvpt.extend);
     }
 		}
 
 init_tmp();
 if(hvcs_cnt!=0)
  	{
  	 sprintf(tmp_amt,"%d",hvcs_cnt);
     strncpy(tmp_cnt,tmp_amt,strlen(tmp_amt));
     sprintf(tmp_amt,"%d",hvcs_cnt*383);
     strncpy(datalen,tmp_amt,strlen(tmp_amt));
     fprintf(fp,"HVCS\n%s\n%s\n",tmp_cnt,datalen);
     
     ret=0;
     ret=Mas_hvcs_Dec_Sel(g_pub_tx.reply ,"recvdate='%ld'",qry_date);
     if(ret)
          {
           sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
           WRITEMSG
           strcpy(g_pub_tx.reply,"D101");
           goto ErrExit;
          } 
      
      /*报送报文种类2数据*/
      while(1)
      {
      	ret=Mas_hvcs_Fet_Sel(&g_mas_hvcs,g_pub_tx.reply);
        if (ret==100)
           break;
        else ERR_DEAL
        init_tmp();         
        strcpy(tmp_n,g_mas_hvcs.tx_amt); 
        pub_base_strpack(tmp_n);
        tmp_amt[15-strlen(tmp_n)]='\0';
        strcat(tmp_amt,tmp_n);
        fprintf(fp,"||%s||%s||%s%s||%s||%s||%s||%s||%s||%s||%s%s||%s%s||%s||%s||%s||%s||%s||\n",\
                g_mas_hvcs.trace_no,g_mas_hvcs.payer_name,g_mas_hvcs.payer_id_type,g_mas_hvcs.payer_id_no,\
                g_mas_hvcs.origrdficode,g_mas_hvcs.origrdfictype,g_mas_hvcs.payee_name,g_mas_hvcs.payee_ac_no,\
                g_mas_hvcs.payee_type,g_mas_hvcs.payee_br_no,g_mas_hvcs.payee_id_type,g_mas_hvcs.payee_id_no,\
                g_mas_hvcs.cur_no,tmp_amt,g_mas_hvcs.recvdate,g_mas_hvcs.cash_source,g_mas_hvcs.sttl_type,\
                g_mas_hvcs.txcode,g_mas_hvcs.extend);
      }
  	 }
 
 init_tmp();
 if(hvcd_cnt!=0)
		{
			sprintf(tmp_amt,"%d",hvcd_cnt);
      strncpy(tmp_cnt,tmp_amt,strlen(tmp_amt));
      sprintf(tmp_amt,"%d",hvcd_cnt*387);
      strncpy(datalen,tmp_amt,strlen(tmp_amt));
      fprintf(fp,"HVCD\n%s\n%s\n",tmp_cnt,datalen);
      
      ret=0;
      ret=Mas_hvcd_Dec_Sel(g_pub_tx.reply ,"paydate='%ld'",qry_date);
      if(ret)
           {
            sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"D101");
            goto ErrExit;
           } 
       /*报送报文种类3数据*/
       while(1) 
       {
       	 ret=Mas_hvcd_Fet_Sel(&g_mas_hvcd,g_pub_tx.reply);
         if (ret==100)
             break;
         else ERR_DEAL
         init_tmp();
         strcpy(tmp_n,g_mas_hvcd.tx_amt); 
         pub_base_strpack(tmp_n);
         tmp_amt[15-strlen(tmp_n)]='\0';
         strcat(tmp_amt,tmp_n);
         fprintf(fp,"||%s||%s||%s||%s||%s||%s||%s||%s%s||%s||%s||%s%s||%s%s||%s||%s||%s||%s||%s||\n",\
                   g_mas_hvcd.trace_no,g_mas_hvcd.payficcode,g_mas_hvcd.payfictype,g_mas_hvcd.payer_name,\
                   g_mas_hvcd.payer_ac_no,g_mas_hvcd.payer_br_no,g_mas_hvcd.payer_type,g_mas_hvcd.payer_id_type,\
                   g_mas_hvcd.payer_id_no,g_mas_hvcd.payee_name,g_mas_hvcd.payee_type,g_mas_hvcd.payee_id_type,\
                   g_mas_hvcd.payee_id_no,g_mas_hvcd.cur_no,tmp_amt,g_mas_hvcd.paydate,g_mas_hvcd.use,g_mas_hvcd.sttl_type,\
                   g_mas_hvcd.txcode,g_mas_hvcd.extend);
       }
		}
 /*init_tmp();		
 tmp=sql_count("mas_ohvc","paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
 if(tmp<0) goto ErrExit;
 	else if(tmp!=0)
 		{
 			sprintf(tmp_cnt,"%d",tmp);
 			sprintf(datalen,"%d",tmp*504);
      fprintf(fp,"OHVC\n%s\n%s\n",tmp_cnt,datalen);
      
      ret=0;
      ret=Mas_ohvc_Dec_Sel(g_pub_tx.reply ,"paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
      if(ret)
           {
            sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"D101");
            goto ErrExit;
           } 
        
      while(1)
      {
      	ret=Mas_ohvc_Fet_Sel(&g_mas_ohvc,g_pub_tx.reply);
        if (ret==100)
            break;
        else ERR_DEAL
        init_tmp();
        strcpy(tmp_n,g_mas_ohvc.tx_amt); 
        pub_base_strpack(tmp_n);
        tmp_amt[15-strlen(tmp_n)]='\0';
        strcat(tmp_amt,tmp_n);
        fprintf(fp,"||%s||%s||%s||%s||%s||%s%s||%s||%s||%s||%s||%s||%s%s||%s||%s%s||%s||%s||%s||%s||%s||%s||%s||\n",\
                g_mas_ohvc.trace_no,g_mas_ohvc.payficcode,g_mas_ohvc.payfictype,g_mas_ohvc.payer_name,g_mas_ohvc.payer_ac_no,\
                g_mas_ohvc.payer_id_type,g_mas_ohvc.payer_id_no,g_mas_ohvc.payer_br_no,g_mas_ohvc.origrdficode,g_mas_ohvc.origrdfictype,\
                g_mas_ohvc.payee_name,g_mas_ohvc.payee_ac_no,g_mas_ohvc.payee_id_type,g_mas_ohvc.payee_id_no,g_mas_ohvc.payee_br_no,\
                g_mas_ohvc.cur_no,tmp_amt,g_mas_ohvc.paydate,g_mas_ohvc.recvdate,g_mas_ohvc.use,g_mas_ohvc.sttl_type,g_mas_ohvc.notecode,\
                g_mas_ohvc.txcode,g_mas_ohvc.extend);
       }	
 		}
 */
 init_tmp();
 if(pbto_cnt!=0)
 		{
 			sprintf(tmp_amt,"%d",pbto_cnt);
      strncpy(tmp_cnt,tmp_amt,strlen(tmp_amt));
      sprintf(tmp_amt,"%d",pbto_cnt*484);
      strncpy(datalen,tmp_amt,strlen(tmp_amt));
      fprintf(fp,"PBTO\n%s\n%s\n",tmp_cnt,datalen);
      
      ret=0;
      ret=Mas_pbto_Dec_Sel(g_pub_tx.reply ,"paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
      if(ret)
           {
            sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"D101");
            goto ErrExit;
           }  
           
       /*报送报文种类5数据*/
       while(1) 
       {
       	ret=Mas_pbto_Fet_Sel(&g_mas_pbto,g_pub_tx.reply);
         if (ret==100)
             break;
         else ERR_DEAL
         init_tmp();
         strcpy(tmp_n,g_mas_pbto.tx_amt); 
         pub_base_strpack(tmp_n);
         tmp_amt[15-strlen(tmp_n)]='\0';
         strcat(tmp_amt,tmp_n);
         fprintf(fp,"||%s||%s||%s||%s||%s||%s||%s||%s||%s||%s||%s||%s||%s%s||%s%s||%s||%s||%s||%s||%s||%s||%s||\n",\
                 g_mas_pbto.trace_no,g_mas_pbto.payficcode,g_mas_pbto.payfictype,g_mas_pbto.payer_name,g_mas_pbto.payer_ac_no,\
                 g_mas_pbto.payer_br_no,g_mas_pbto.origrdficode,g_mas_pbto.origrdfictype,g_mas_pbto.payee_name,g_mas_pbto.payee_ac_no,\
                 g_mas_pbto.payee_br_no,g_mas_pbto.payee_type,g_mas_pbto.per_id_type,g_mas_pbto.per_id_no,g_mas_pbto.cur_no,tmp_amt,g_mas_pbto.paydate,\
                 g_mas_pbto.recvdate,g_mas_pbto.use,g_mas_pbto.notecode,g_mas_pbto.sttl_type,g_mas_pbto.txcode,g_mas_pbto.extend);
         }	
 			}
 
 init_tmp();
 if(pato_cnt!=0)
 		{
 			sprintf(tmp_amt,"%d",pato_cnt);
      strncpy(tmp_cnt,tmp_amt,strlen(tmp_amt));
      sprintf(tmp_amt,"%d",pato_cnt*512);
      strncpy(datalen,tmp_amt,strlen(tmp_amt));
      fprintf(fp,"PATO\n%s\n%s\n",tmp_cnt,datalen);
      
      ret=0;
      ret=Mas_pato_Dec_Sel(g_pub_tx.reply ,"paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
      if(ret)
           {
            sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"D101");
            goto ErrExit;
           } 
       /*报送报文种类6数据*/
      while(1)
      {
      	ret=Mas_pato_Fet_Sel(&g_mas_pato,g_pub_tx.reply);
         if (ret==100)
             break;
         else ERR_DEAL
        init_tmp();
        strcpy(tmp_n,g_mas_pato.tx_amt); 
        pub_base_strpack(tmp_n);
        tmp_amt[15-strlen(tmp_n)]='\0';
        strcat(tmp_amt,tmp_n);
        fprintf(fp,"||%s||%s||%s||%s||%s||%s||%s||%s%s||%s||%s||%s||%s||%s||%s||%s%s||%s%s||%s||%s||%s||%s||%s||%s||%s||\n",\
               g_mas_pato.trace_no,g_mas_pato.payficcode,g_mas_pato.payfictype,g_mas_pato.payer_name,g_mas_pato.payer_ac_no,\
               g_mas_pato.payer_br_no,g_mas_pato.payer_type,g_mas_pato.payer_id_type,g_mas_pato.payer_id_no,g_mas_pato.origrdficode,\
               g_mas_pato.origrdfictype,g_mas_pato.payee_name,g_mas_pato.payee_ac_no,g_mas_pato.payee_br_no,g_mas_pato.payee_type,\
               g_mas_pato.payee_id_type,g_mas_pato.payee_id_no,g_mas_pato.cur_no,tmp_amt,g_mas_pato.paydate,g_mas_pato.recvdate,\
               g_mas_pato.use,g_mas_pato.sttl_type,g_mas_pato.notecode,g_mas_pato.txcode,g_mas_pato.extend);
      	} 
 			}
fprintf(fp,"}\n");
fclose(fp);
pub_base_deadlineD(g_pub_tx.tx_date,-4,&qry_date );
ret=sql_execute("delete from mas_hvpt where paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
ret=sql_execute("delete from mas_hvcs where recvdate='%ld'",qry_date);
ret=sql_execute("delete from mas_hvcd where paydate='%ld'",qry_date);
ret=sql_execute("delete from mas_pato where paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
ret=sql_execute("delete from mas_pbto where paydate='%ld' or recvdate='%ld'",qry_date,qry_date);
Mas_hvpt_Clo_Sel();
Mas_hvcd_Clo_Sel();
Mas_hvcs_Clo_Sel();
Mas_pbto_Clo_Sel();
Mas_pato_Clo_Sel();
OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成反洗钱文件成功[%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"生成反洗钱文件![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
init_tmp()
{
 ret=0;
 memset(tmp_n,0x20,sizeof(tmp_n));
 tmp_n[sizeof(tmp_n)-1]='\0';
 memset(tmp_cnt,0x20,sizeof(tmp_cnt));
 tmp_cnt[sizeof(tmp_cnt)-1]='\0';
 memset(datalen,0x20,sizeof(datalen));
 datalen[sizeof(datalen)-1]='\0';
 memset(tmp_amt,0x30,sizeof(tmp_amt));
 tmp_amt[sizeof(tmp_amt)-1]='\0';
}

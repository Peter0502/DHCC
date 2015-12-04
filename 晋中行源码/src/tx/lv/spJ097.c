/****************************************************************************************
*������: spJ097.c
*����:  ���ո�ҵ���ѯ����
*
*
*��д����: 20061213
*����:   Jarod
**
****************************************************************************************/
#define ERR_DEAL if(ret){ \
	 sprintf(acErrMsg,"sql error.[%d]",ret);\
 	WRITEMSG \
 	goto ErrExit;\
}

#include <string.h>
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "lv_pldfr_c.h"
#include "lv_pldsr_c.h"
#include "lv_pldscx_c.h"
#include "lv_pldsfqugx_c.h"
#include "lv_category_c.h"

int spJ097()
{
	FILE *fp;
	char filename[100];
	int ret=0;
	int cnt=0;
	struct lv_pldfr_c sLv_pldfr;
	struct lv_pldsr_c sLv_pldsr;
	struct lv_pldscx_c sLv_pldscx;
	struct lv_pldsfqugx_c sLv_pldsfqugx;
	struct lv_category_c   lv_category;
	long wt_date=0;
	char pack_id[9];
	char order_no[9];
	char tx_type[8];
	char tx_name[2];
	char tmp[9];
	char sts[16];
	
	memset(filename,0x00,sizeof(filename));
	memset(tmp,0x00,sizeof(tmp));
	memset(&sLv_pldfr,0x00,sizeof(struct lv_pldfr_c));
	memset(&sLv_pldsr,0x00,sizeof(struct lv_pldsr_c));
	memset(&sLv_pldscx,0x00,sizeof(struct lv_pldscx_c));
	memset(&sLv_pldsfqugx,0x00,sizeof(struct lv_pldsfqugx_c));
	memset(&lv_category  , 0, sizeof(struct lv_category_c));
	memset(pack_id,0x00,sizeof(pack_id));
	memset(order_no,0x00,sizeof(order_no));
	memset(tx_type,0x00,sizeof(tx_type));
	memset(tx_name,0x00,sizeof(tx_name));
	memset(sts,0x00,sizeof(sts));

	pub_base_sysinit();

	get_zd_long("0440",&wt_date);
	get_zd_data("0450",pack_id);
	get_zd_data("0460",order_no);
	get_zd_data("0670",tx_type);
  pub_base_strpack_all(pack_id);
  pub_base_strpack_all(order_no);
/*��pack_id,order_no�Ŀո����ַ�0���*/
	memset(tmp,'0',sizeof(tmp));
	memcpy(tmp+8-strlen(pack_id),pack_id,strlen(pack_id));
	memset(pack_id,0x00,sizeof(pack_id));
	memcpy(pack_id,tmp,sizeof(pack_id)-1);

	memset(tmp,'0',sizeof(tmp));
	memcpy(tmp+8-strlen(order_no),order_no,strlen(order_no));
	memset(order_no,0x00,sizeof(order_no));
	memcpy(order_no,tmp,sizeof(order_no)-1);

	vtcp_log("������ֵΪ:wt_date=[%ld],pack_id=[%s],order_no=[%s],tx_type=[%s]   [%s]--[%d]",wt_date,pack_id,order_no,tx_type,__FILE__,__LINE__);
	/*if(memcmp(tx_type,"����",4)==0)
      {
            strcpy(tx_name, "0");     
      }
      else if(memcmp(tx_type,"����",4)==0)
      {
            strcpy(tx_name, "1");      
      }
       else if(memcmp(tx_type,"����",4)==0)
      {
            strcpy(tx_name, "2");      
      }
       else if(memcmp(tx_type,"��ͬ���",8)==0)
      {
            strcpy(tx_name, "3");      
      }
      else
      	 {
             vtcp_log("[%s][%d]û�����ֽ��׷�ʽ",__FILE__,__LINE__);
             sprintf(acErrMsg, "û�����ֽ��׷�ʽ [%s] error ",tx_type);
             WRITEMSG
             strcpy(g_pub_tx.reply, "S047");
             goto ErrExit;
      	 }*/
	switch(tx_type[0])
	{
		case '0': /**����**/
			ret=Lv_pldfr_Dec_Sel(g_pub_tx.reply," wt_date=%ld and packid='%8s' and orderno='%8s' ",wt_date,pack_id,order_no);
			ERR_DEAL
			while(1)
			{/*1 while*/
				memset(&sLv_pldfr,0x00,sizeof(struct lv_pldfr_c));
				ret=Lv_pldfr_Fet_Sel(&sLv_pldfr,g_pub_tx.reply);
				if(ret == 100) break;
				else ERR_DEAL
				
				if(memcmp(sLv_pldfr.sts,"0A",2)==0)
      {
            memcpy(sts,"���˽���",16);     
      }
      else if(memcmp(sLv_pldfr.sts,"1B",2)==0)
      {
            memcpy(sts,"���˾ܾ�",16);     
      }
       else if(memcmp(sLv_pldfr.sts,"0C",2)==0)
      {
            memcpy(sts,"���˳���",16);     
      }
       else if(memcmp(sLv_pldfr.sts,"0D",2)==0)
      {
            memcpy(sts,"�ɹ���ִ40506",16);     
      }
       else if(memcmp(sLv_pldfr.sts,"1E",2)==0)
      {
            memcpy(sts,"ʧ�ܻ�ִ40506",16);     
      }
       else if(memcmp(sLv_pldfr.sts,"0F",2)==0)
      {
            memcpy(sts,"�ɹ���ִ40503/4",16);     
      }
       else if(memcmp(sLv_pldfr.sts,"0G",2)==0)
      {
            memcpy(sts,"ʧ�ܻ�ִ40503/4",16);     
      }
       else if(memcmp(sLv_pldfr.sts,"0H",2)==0)
      {
            memcpy(sts,"���˷���",16);     
      }
       else if(memcmp(sLv_pldfr.sts,"1I",2)==0)
      {
            memcpy(sts,"���˾ܾ�",16);     
      }
       else if(memcmp(sLv_pldfr.sts,"0J",2)==0)
      {
            memcpy(sts,"��������",16);     
      }
       else if(memcmp(sLv_pldfr.sts,"1K",2)==0)
      {
            memcpy(sts,"����ʧ��",16);     
      }
      else
      	 {
             vtcp_log("[%s][%d]û������ҵ������",__FILE__,__LINE__);
             sprintf(acErrMsg, " û������ҵ������ [%s] error ",sLv_pldfr.sts);
             WRITEMSG
             strcpy(g_pub_tx.reply, "S047");
             goto ErrExit;
      	 }
				
				if(!cnt)
				{
					pub_base_AllDwnFilName( filename );
            		fp = fopen( filename,"w" );
            		if(!fp)
            		{
                		sprintf(acErrMsg," open file [%s] error ",filename );
                		WRITEMSG
                		strcpy( g_pub_tx.reply,"S047" );
                		goto ErrExit;
            		}
            		fprintf(fp,"~�����|�������|��ί������|��ϸ���|��ͬ��|����״̬|�ļ�����|\n");
				}
				fprintf(fp,"%s|%s|%ld|%s|%s|%s|%s|\n",sLv_pldfr.packid,sLv_pldfr.orderno,sLv_pldfr.wt_date,sLv_pldfr.tx_mx_no,sLv_pldfr.xyno,sts,sLv_pldfr.filename);
				cnt++;	
			}/*2 while*/	
			Lv_pldfr_Clo_Sel();
			if(!cnt)
			{
				strcpy( g_pub_tx.reply,"S049" );
				goto ErrExit;
			}
			else
			{
				fclose(fp);
				set_zd_data( DC_FILE_SND,"1" );
			}
			break;
		case '1': /**����**/
			ret=Lv_pldsr_Dec_Sel(g_pub_tx.reply," wt_date=%ld and packid='%8s' and orderno='%8s' ",wt_date,pack_id,order_no);
			ERR_DEAL
			while(1)
			{/*1 while*/
				memset(&sLv_pldsr,0x00,sizeof(struct lv_pldsr_c));
				ret=Lv_pldsr_Fet_Sel(&sLv_pldsr,g_pub_tx.reply);
				if(ret == 100) break;
				else ERR_DEAL
				
				if(memcmp(sLv_pldsr.sts,"0A",2)==0)
      {
            memcpy(sts,"���˽���",16);     
      }
      else if(memcmp(sLv_pldsr.sts,"1B",2)==0)
      {
            memcpy(sts,"���˾ܾ�",16);     
      }
       else if(memcmp(sLv_pldsr.sts,"0C",2)==0)
      {
            memcpy(sts,"���˳���",16);     
      }
       else if(memcmp(sLv_pldsr.sts,"0D",2)==0)
      {
            memcpy(sts,"�ɹ���ִ40506",16);     
      }
       else if(memcmp(sLv_pldsr.sts,"1E",2)==0)
      {
            memcpy(sts,"ʧ�ܻ�ִ40506",16);     
      }
       else if(memcmp(sLv_pldsr.sts,"0F",2)==0)
      {
            memcpy(sts,"�ɹ���ִ40503/4",16);     
      }
       else if(memcmp(sLv_pldsr.sts,"0G",2)==0)
      {
            memcpy(sts,"ʧ�ܻ�ִ40503/4",16);     
      }
       else if(memcmp(sLv_pldsr.sts,"0H",2)==0)
      {
            memcpy(sts,"���˷���",16);     
      }
       else if(memcmp(sLv_pldsr.sts,"1I",2)==0)
      {
            memcpy(sts,"���˾ܾ�",16);     
      }
       else if(memcmp(sLv_pldsr.sts,"0J",2)==0)
      {
            memcpy(sts,"��������",16);     
      }
       else if(memcmp(sLv_pldsr.sts,"1K",2)==0)
      {
            memcpy(sts,"����ʧ��",16);     
      }
      else
      	 {
             vtcp_log("[%s][%d]û������ҵ������",__FILE__,__LINE__);
             sprintf(acErrMsg, " û������ҵ������ [%s] error ",sLv_pldsr.sts);
             WRITEMSG
             strcpy(g_pub_tx.reply, "S047");
             goto ErrExit;
      	 }
				
				if(!cnt)
				{
					pub_base_AllDwnFilName( filename );
            		fp = fopen( filename,"w" );
            		if(!fp)
            		{
                		sprintf(acErrMsg," open file [%s] error ",filename );
                		WRITEMSG
                		strcpy( g_pub_tx.reply,"S047" );
                		goto ErrExit;
            		}
            		fprintf(fp,"~�����|�������|��ί������|��ϸ���|��ִ����|��ͬ��|����״̬|�ļ�����|\n");
				}
				fprintf(fp,"%s|%s|%ld|%s|%d|%s|%s|%s|\n",sLv_pldsr.packid,sLv_pldsr.orderno,sLv_pldsr.wt_date,sLv_pldsr.tx_mx_no,sLv_pldsr.re_lim_date,sLv_pldsr.xyno,sts,sLv_pldsr.filename);
				cnt++;	
			}/*2 while*/	
			Lv_pldsr_Clo_Sel();
			if(!cnt)
			{
				strcpy( g_pub_tx.reply,"S049" );
				goto ErrExit;
			}
			else
			{
				fclose(fp);
				set_zd_data( DC_FILE_SND,"1" );
			}
			break;
		case '2': /**����**/
			ret=Lv_pldscx_Dec_Sel(g_pub_tx.reply," wt_date=%ld and packid='%8s' and orderno='%8s' ",wt_date,pack_id,order_no);
			ERR_DEAL
			while(1)
			{/*1 while*/
				memset(&sLv_pldscx,0x00,sizeof(struct lv_pldscx_c));
				ret=Lv_pldscx_Fet_Sel(&sLv_pldscx,g_pub_tx.reply);
				if(ret == 100) break;
				else ERR_DEAL
				
				if(!cnt)
				{
					pub_base_AllDwnFilName( filename );
            		fp = fopen( filename,"w" );
            		if(!fp)
            		{
                		sprintf(acErrMsg," open file [%s] error ",filename );
                		WRITEMSG
                		strcpy( g_pub_tx.reply,"S047" );
                		goto ErrExit;
            		}
            		fprintf(fp,"~�����|�������|��ί������|�������к�|������|ԭ����ί������|ԭ������Ϣҵ�����|ԭ��ҵ������|���ո���������|����|\n");
				}
				/**beg  del by ChengGX 20100514 ��ˮ **
				ret = Lv_category_Sel( g_pub_tx.reply , &lv_category , " code='%s' " , sLv_pldscx.o_yw_type);
                if( ret==100 )
                    {
          	            sprintf(acErrMsg,"��ҵ������û�еǼ�![%s]",g_pub_tx.reply);
          	            goto ErrExit;
                     }
                else if( ret )
                 {
          	            sprintf(acErrMsg,"ȡҵ��������Ϣ�쳣![%s]",g_pub_tx.reply);
          	            goto ErrExit;
                  }
                end del by ChengGX 20100514 ��ˮ **/
				fprintf(fp,"%s|%s|%ld|%s|%s|%ld|%s|%s|%ld|%s|\n",sLv_pldscx.packid,sLv_pldscx.orderno,sLv_pldscx.wt_date,sLv_pldscx.br_no,sLv_pldscx.or_br_no,sLv_pldscx.o_wt_date,sLv_pldscx.o_orderno,sLv_pldscx.o_yw_type,sLv_pldscx.dsf_date,sLv_pldscx.content);	
				cnt++;	
			}/*2 while*/	
			Lv_pldscx_Clo_Sel();
			if(!cnt)
			{
				strcpy( g_pub_tx.reply,"S049" );
				goto ErrExit;
			}
			else
			{
				fclose(fp);
				set_zd_data( DC_FILE_SND,"1" );
			}
			break;
		case '3': /**��ͬ���**/
			ret=Lv_pldsfqugx_Dec_Sel(g_pub_tx.reply," wt_date=%ld and packid='%s' and orderno='%s' ",wt_date,pack_id,order_no);
			ERR_DEAL
			while(1)
			{/*1 while*/
				memset(&sLv_pldsfqugx,0x00,sizeof(struct lv_pldsfqugx_c));
				ret=Lv_pldsfqugx_Fet_Sel(&sLv_pldsfqugx,g_pub_tx.reply);
				if(ret == 100) break;
				else ERR_DEAL
				
				if(!cnt)
				{
					pub_base_AllDwnFilName( filename );
            		fp = fopen( filename,"w" );
            		if(!fp)
            		{
                		sprintf(acErrMsg," open file [%s] error ",filename );
                		WRITEMSG
                		strcpy( g_pub_tx.reply,"S047" );
                		goto ErrExit;
            		}
            		fprintf(fp,"~�����|�������|��ί������|�������к�|������|��ID|��ͬ��|��ͬ����|�����ʽ|�������к�|�����������к�|�����˿������к�|�������ʺ�|����������|�����˵�ַ|����|\n");
				}
      /*  vtcp_log("AAAAAAAA___xy_ind[%s],ch_ind[%s],xy_ind[%c],ch_ind[%c]\n",sLv_pldsfqugx.xy_ind,sLv_pldsfqugx.ch_ind,sLv_pldsfqugx.xy_ind,sLv_pldsfqugx.ch_ind); */
				fprintf(fp,"%s|%s|%ld|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",sLv_pldsfqugx.pkgno,sLv_pldsfqugx.orderno,sLv_pldsfqugx.wt_date,sLv_pldsfqugx.br_no,sLv_pldsfqugx.or_br_no,sLv_pldsfqugx.packid,sLv_pldsfqugx.xyno,sLv_pldsfqugx.xy_ind,sLv_pldsfqugx.ch_ind,sLv_pldsfqugx.pay_br_no,sLv_pldsfqugx.pay_qs_no,sLv_pldsfqugx.pay_opn_br_no,sLv_pldsfqugx.pay_ac_no,sLv_pldsfqugx.pay_name,sLv_pldsfqugx.pay_addr,sLv_pldsfqugx.content);	
				cnt++;	
			}/*2 while*/	
			Lv_pldscx_Clo_Sel();
			if(!cnt)
			{
				strcpy( g_pub_tx.reply,"S049" );
				goto ErrExit;
			}
			else
			{
				fclose(fp);
				set_zd_data( DC_FILE_SND,"1" );
			}
			break;
		default:
			sprintf(acErrMsg,"�������ʹ���!![%s]",tx_type);
			strcpy( g_pub_tx.reply,"O117" ); /*add by CGX ��ˮ 20100511**/
			WRITEMSG
			goto ErrExit;
	}
vtcp_log("��ȡ��[%d]����¼!! [%s]--[%d]",cnt,__FILE__,__LINE__);	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"OK exit!! reply is:[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail exit!! reply is:[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

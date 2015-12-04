/****************************************************************************************
*������: spJ405.c
*����:   ���ո�ҵ���ѯ����
*
*����:   2007-4-14 12:54
*����:   xyy
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
#include "lv_dsfsendlist_c.h"
#include "hv_brno_orno_c.h"

int spJ405()
{
	FILE *fp;
	char filename[100];
	int ret=0;
	int cnt=0;
	int iRepstat=0;
	long lDate=0;
	char cKinbr   [6];
	char cStat[16];
  char cOr_br_no[13];
  char cRepstat[3];
  char cTmpBuff[7];
  
	struct lv_dsfsendlist_c lv_dsfsendlist;
  struct hv_brno_orno_c sHv_brno_orno;
  
	lDate = 0;
	
	memset(filename,0x00,sizeof(filename));
	memset(&lv_dsfsendlist,0x00,sizeof(struct lv_dsfsendlist_c));
	memset(&sHv_brno_orno, 0, sizeof(struct hv_brno_orno_c));
	memset(cKinbr   , 0x00, sizeof(cKinbr));
  memset(cOr_br_no, 0x00, sizeof(cOr_br_no));
  memset(cStat    , 0x00, sizeof(cStat));
  memset(cRepstat , 0x00, sizeof(cRepstat));
  memset(cTmpBuff , 0x00, sizeof(cTmpBuff));

	pub_base_sysinit();
  
  get_zd_data("0910", cKinbr);
  get_zd_long("0440", &lDate);

  vtcp_log("[%s][%d]br_no=[%s]", __FILE__,__LINE__, cKinbr);
  vtcp_log("[%s][%d]date=[%ld]", __FILE__,__LINE__, lDate);

  ret = Hv_brno_orno_Sel(g_pub_tx.reply, &sHv_brno_orno," br_no = '%s'", cKinbr);
  if(ret)
  {
      sprintf(acErrMsg, "��ѯhv_brno_orno�����!![%d]", ret);
      strcpy(g_pub_tx.reply, "S047");
      WRITEMSG
      goto ErrExit;
  }
  memcpy(cOr_br_no, sHv_brno_orno.or_br_no, sizeof(sHv_brno_orno.or_br_no));
  vtcp_log("[%s][%d]cOr_br_no=[%s]",__FILE__,__LINE__,cOr_br_no);  

	vtcp_log("[%s][%d]������ֵΪ:wt_date=[%ld],order_no=[%s]\n",__FILE__,__LINE__,lDate,cOr_br_no);

  ret=Lv_dsfsendlist_Dec_Sel(g_pub_tx.reply," wt_date=%ld and or_br_no='%8s' order by packid ",lDate,cOr_br_no);
  ERR_DEAL
  while(1)
  {
  	memset(&lv_dsfsendlist,0x00,sizeof(struct lv_dsfsendlist_c));
  	memset(cTmpBuff , 0x00 , sizeof(cTmpBuff));
  	memset(cStat    , 0x00 , sizeof(cStat));
  	memset(cRepstat , 0x00 , sizeof(cRepstat));
  	ret=Lv_dsfsendlist_Fet_Sel(&lv_dsfsendlist,g_pub_tx.reply);
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
       		fprintf(fp,"~�����|�������|ί������|ҵ������|����״̬|ԭҵ������|Ӧ��״̬|\n");
  	}
  	
  	if(memcmp(lv_dsfsendlist.sts,"0A" ,2)==0) 
    {
        memcpy(cStat,"���˽���",8);
    }
    else if(memcmp(lv_dsfsendlist.sts,"1B" ,2)==0)  
    {
        memcpy(cStat,"���˾ܾ�",8);
    }
    else if(memcmp(lv_dsfsendlist.sts,"0C" ,2)==0)   
    {
        memcpy(cStat,"���˳���",8);
    }
    else if(memcmp(lv_dsfsendlist.sts,"0D" ,2)==0)   
    {
        memcpy(cStat,"�ɹ���ִ40506",13);
    }
    else if(memcmp(lv_dsfsendlist.sts,"1E" ,2)==0)
    {
        memcpy(cStat,"ʧ�ܻ�ִ40506",13);
    }
    else if(memcmp(lv_dsfsendlist.sts,"0F" ,2)==0)
    {
        memcpy(cStat,"�ɹ���ִ40503/4",15);
    }
    else if(memcmp(lv_dsfsendlist.sts,"0G" ,2)==0)  
    {
        memcpy(cStat,"ʧ�ܻ�ִ40503/4",13);
    }
    else if(memcmp(lv_dsfsendlist.sts,"0H" ,2)==0)   
    {
        memcpy(cStat,"���˷���",8);
    }
    else if(memcmp(lv_dsfsendlist.sts,"1I" ,2)==0)   
    {
        memcpy(cStat,"���˾ܾ�",8);
    }
    else if(memcmp(lv_dsfsendlist.sts,"0J" ,2)==0)   
    {
        memcpy(cStat,"��������",8);
    }
    else if(memcmp(lv_dsfsendlist.sts,"1K" ,2)==0)  
    { 
        memcpy(cStat,"����ʧ��",8);
    }
    else
    {
        memcpy(cStat,"״̬����",8);
    }
    
    memcpy(cRepstat,lv_dsfsendlist.repstat,sizeof(cRepstat)-1);
    iRepstat = apatoi(cRepstat,2);
    vtcp_log("[%s][%d]iRepstat=[%d]\n",__FILE__,__LINE__,iRepstat);
    switch(iRepstat)
    {
    	  case 1:
    	  	 memcpy(cTmpBuff,"�Ѿܾ�",6);
    	  	 break;
    	  case 2:
    	  	 memcpy(cTmpBuff,"��Ӧ��",6);
    	  	 break;
    	  case 3:
    	  	 memcpy(cTmpBuff,"������",6);
    	  	 break;
    	  case 4:
    	  	 memcpy(cTmpBuff,"���Ŷ�",6);
    	  	 break;
    	  case 5:
    	  	 memcpy(cTmpBuff,"������",6);
    	  	 break;
    	  case 6:
    	  	 memcpy(cTmpBuff,"�ѳ���",6);
    	  	 break;
    	  case 10:
    	  	 memcpy(cTmpBuff,"�ѻ�ִ",6);
    	  	 break;
    	  case 11:
    	  	 memcpy(cTmpBuff,"�Ѿܸ�",6);
    	  	 break;
    	  case 12:
    	  	 memcpy(cTmpBuff,"�ѳ���",6);
    	  	 break;
    	  case 13:
    	  	 memcpy(cTmpBuff,"��ֹ��",6);
    	  	 break;
    	  case 14:
    	  	 memcpy(cTmpBuff,"�ѳ�ʱ",6);
    	  	 break;
    	  default:
    	  	 memcpy(cTmpBuff,"δ�ظ�",6);
    	  	 break;
    }
  	fprintf(fp,"%s|%s|%ld|%s|%s|%s|%s|%s\n",lv_dsfsendlist.packid,lv_dsfsendlist.orderno,lv_dsfsendlist.wt_date,lv_dsfsendlist.tx_type,cStat,lv_dsfsendlist.o_tx_type,cTmpBuff);
  	cnt++;	
  }/*2 while*/	
  Lv_dsfsendlist_Clo_Sel();
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

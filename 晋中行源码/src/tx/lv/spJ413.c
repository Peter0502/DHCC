/************************************************************
** �����ܣ�ʵ�ֶ�֧Ʊ����ʷ��ѯ
** �������ƣ�spJ413.c
** ����ԭ�ͣ�dh6560023_work()
** ���������
** ���������
** ˵   ����
** ��   �ߣ�
**********************************************************/
#define ERR_DEAL if(ret){ \
           sprintf(acErrMsg,"sql error.[%d]",ret);\
           WRITEMSG \
          goto ErrExit;\
          }

#define EXTERN
#include "public.h"
#include "lv_pkgreg_c.h"
#include "lv_define.h"
#include "lv_addit_c.h"

char txnum_content[20];
char stat_content[21];
char Rcp_content[41];
char ZP_NUM[13];
char ZP_DATE[8];

spJ413()
{
     int ret=0;
     FILE *fp;
     char filename[100];
     long txdate=0;
     char brno[8];
     char acno[51];
     struct lv_pkgreg_c lv_pkgreg;
     struct lv_addit_c  lv_addit_g;
     int i=0;
     char Wlflag[2];
     char Ywtype[2];
     char Zpstat[2];
     char tempstr[512];
     char wherelist[512];
     double tmptxamt=0;
     double tmptxamt2=0;

     memset(ZP_NUM,0x00,sizeof(ZP_NUM));     
     memset(txnum_content,0x00,sizeof(txnum_content));     
     memset(Rcp_content,0x00,sizeof(Rcp_content));     
     memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
     memset(stat_content,'\0',sizeof(stat_content));
     memset(brno,'\0',sizeof(brno));     
     memset(filename,'\0',sizeof(filename));
     memset(acno,'\0',sizeof(acno));
     memset(Wlflag, '\0', sizeof(Wlflag));
     memset(Ywtype,  '\0', sizeof(Ywtype));
     memset(Zpstat,  '\0', sizeof(Zpstat));
     memset(tempstr, '\0', sizeof(tempstr));
     memset(wherelist, '\0', sizeof(wherelist));
 
     get_zd_double("0400",&tmptxamt);
     get_zd_double("0410",&tmptxamt2);
     get_zd_long("0440",&txdate);
     get_zd_data("0640",brno);
     get_zd_data("0670",Wlflag);     /**  1�����ʣ� 2���� **/
     get_zd_data("0680",Ywtype);     /** 0.ȫ�� 1.����ҵ��  2.��ִҵ�� **/
     get_zd_data("0690",Zpstat);
     get_zd_data("0300",acno);

     pub_base_sysinit();

     pub_base_strpack_all(acno);
     pub_base_strpack_all(brno);
     vtcp_log("%s,%d,txdate=[%ld],brno=[%s],Wlflag=[%s],Ywtype=[%s],Zpstat=[%s],acno=[%s]",__FILE__,__LINE__,txdate,brno,Wlflag,Ywtype,Zpstat,acno);     
 sprintf(tempstr, " (txnum='30102' or txnum='30103' or txnum='30104' or txnum='30105')  and  in_date='%ld'", txdate);
 strcat(wherelist, tempstr);

      if(brno[0]!='\0')
      {
             sprintf(tempstr," and br_no ='%s'", brno); 
             strcat(wherelist, tempstr);
      }

      if(acno[0]!='\0')
      { 
          sprintf(tempstr, " and (pay_ac_no='%s' or cash_ac_no='%s')", acno,acno);
             strcat(wherelist,tempstr);
      }

      if(tmptxamt!=0)
      { 
          sprintf(tempstr, " and tx_amt >= %.2lf", tmptxamt);
             strcat(wherelist, tempstr);
      }
      if(tmptxamt2!=0)
      { 
          sprintf(tempstr, " and tx_amt <= %.2lf", tmptxamt2);
             strcat(wherelist, tempstr);
      }

     /** dongxy �ж������ʻ������� **/
      if(Wlflag[0]=='1') /** ���� **/
      {
         sprintf(tempstr, " and lw_ind='1'");
         strcat(wherelist,tempstr);
      }else if(Wlflag[0]=='2') /** ���� **/
     {
         sprintf(tempstr, " and lw_ind='2'");
         strcat(wherelist,tempstr);
     }


      if(Ywtype[0]=='1')
      {
         sprintf(tempstr, " and pkgno='002'");
         strcat(wherelist,tempstr);
      }
      else if(Ywtype[0]=='2')
      { 
             sprintf(tempstr, " and pkgno='008'");
             strcat(wherelist, tempstr);
      }
/** ���������ӣ�������ת������ **/
     else
     {
             sprintf(tempstr, " and (pkgno='008' or pkgno = '002')");
             strcat(wherelist, tempstr);
     }

      if(Zpstat[0]!='0')
      {
            sprintf(tempstr, " and lv_sts='%s' ", Zpstat ); 
            strcat(wherelist, tempstr);
      }
      sprintf(tempstr, "  order by orderno desc");
      strcat(wherelist, tempstr);
     vtcp_log("%s,%d,wherelist=[%s]",__FILE__,__LINE__,wherelist);
     i=0;

     vtcp_log("%s,%d*************",__FILE__,__LINE__);
     ret=Lv_pkgreg_Dec_Sel(g_pub_tx.reply,"%s",wherelist);
     ERR_DEAL

     vtcp_log("%s,%d",__FILE__,__LINE__);
     while(1)
     {/*1 while*/
          memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
          memset(stat_content,'\0',sizeof(stat_content));
          memset(txnum_content,0x00,sizeof(txnum_content));     
          
	  vtcp_log("%s,%d",__FILE__,__LINE__);
          ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
          if(ret == 100) break;
          else ERR_DEAL

	  vtcp_log("%s,%d",__FILE__,__LINE__);
          if(!i)
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
               fprintf(fp,"~������|Ʊ�ݺ���|��Ʊ����|ί������|˳���|���׽��|״̬|�����к�|�����к�|�����|ҵ������|�����|��ִ״̬|\n");
          }
	  /**ȡ֧Ʊ����**/
	  if(memcmp(lv_pkgreg.txnum,"30102",5)==0 || memcmp(lv_pkgreg.txnum,"30104",5)==0){
		memset(&lv_addit_g,0,sizeof(lv_addit_g));
		ret=Lv_addit_Sel(g_pub_tx.reply,&lv_addit_g,"tx_date=%d and tagname='001' and addid='%s'",lv_pkgreg.wt_date,lv_pkgreg.addid);
		if(ret){
			sprintf(acErrMsg,"δ�ҵ�֧Ʊ����!");
			set_zd_data(DC_GET_MSG,acErrMsg);
			/**strcpy(g_pub_tx.reply,"D014");**/
			WRITEMSG;
			/*goto ErrExit;**/
		}
		memset(ZP_NUM,0,sizeof(ZP_NUM));
		memset(ZP_DATE,0,sizeof(ZP_DATE));
		memcpy(ZP_NUM,lv_addit_g.tagdata+8,12);
		memcpy(ZP_DATE,lv_addit_g.tagdata,8);
	  }
	  vtcp_log("%s,%d",__FILE__,__LINE__);
          Rtn_PAY_Txnum_Mesg(lv_pkgreg.txnum,txnum_content);
	  Rtn_PAY_Rcpstat_Mesg(lv_pkgreg.rcpstat,Rcp_content);
     
	  vtcp_log("%s,%d",__FILE__,__LINE__);
        /***��״̬����������ֵ***/
        if(!memcmp(lv_pkgreg.lv_sts, "I", 1) && memcmp(lv_pkgreg.pkgno,PKGNO_PTJJHZ,sizeof(lv_pkgreg.pkgno)-1)==0 && memcmp(lv_pkgreg.rcpstat,"00",2)!=0) 
        {
		memcpy(stat_content,"�������ܣ���ִδ����",20);
           /**if(lv_pkgreg.resp1[3] == 'I'){
              if(!memcmp(lv_pkgreg.rcpstat, "00", 2))
                  memcpy(stat_content, "�ɹ�", 4);
              else if(!memcmp(lv_pkgreg.rcpstat, "01", 2))
                  memcpy(stat_content, "�ʺŲ���", 8);
              else if(!memcmp(lv_pkgreg.rcpstat, "02", 2))
                  memcpy(stat_content, "�ʺŻ�������", 12);
              else if(!memcmp(lv_pkgreg.rcpstat, "03", 2))
                  memcpy(stat_content, "�ʺ�����֧��", 16);
              else if(!memcmp(lv_pkgreg.rcpstat, "10", 2))
                  memcpy(stat_content, "�ʺ��������", 12);
              else if(!memcmp(lv_pkgreg.rcpstat, "11", 2))
                  memcpy(stat_content, "�ʺ�״̬����", 12);
              else if(!memcmp(lv_pkgreg.rcpstat, "20", 2))
                  memcpy(stat_content, "ҵ���ѳ���", 10);
              else if(!memcmp(lv_pkgreg.rcpstat, "90", 2))
                  memcpy(stat_content, "��������", 8);
           }else{
                Rtn_PAY_Stat_Mesg(lv_pkgreg.lv_sts,stat_content);
	        }**/
     }else{
              Rtn_PAY_Stat_Mesg(lv_pkgreg.lv_sts,stat_content);
	  }
       i++;
	vtcp_log("%s,%d",__FILE__,__LINE__);
     fprintf(fp,"%s|%s|%s|%ld|%s|%.2lf|%s|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.br_no,ZP_NUM,ZP_DATE,lv_pkgreg.wt_date,lv_pkgreg.orderno,lv_pkgreg.tx_amt,stat_content,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.packid,txnum_content,lv_pkgreg.pkgno,Rcp_content);
      }
     Lv_pkgreg_Clo_Sel();
     if(!i)
     {
          strcpy( g_pub_tx.reply,"S049" );
          goto ErrExit;
     }
     else
     {
          fclose(fp);
          set_zd_data( DC_FILE_SND,"1" );
     }
vtcp_log("%s,%d,iiiiiiiiiiiiiiiiiii=[%d]",__FILE__,__LINE__,i);
OkExit:
     strcpy(g_pub_tx.reply,"0000");
     sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 0;     
ErrExit:
     sprintf(acErrMsg,"Before ERROR return: reply [%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 1;     
}
int Rtn_PAY_Stat_Mesg(char  *Stat,char *stat_content)
{
 char  TRAN_STAT[50][2][30]={
                                                "0","��Ч״̬",
                                                "1","����¼��",
                                                "2","���˸��˹���",
                                                "3","���˸��˼���",
                                                "4","���˷���",
                                                "5","���˾ܾ�",
                                                "6","��������",
                                                "7","�����Ŷ�",
                                                "8","���˳����ȴ�",
                                                "9","���˳���",
                                                "A","�����˻�ȴ�",
                                                "B","�����˻�",
                                                "C","���˳���",
                                                "D","����ֹ��",
                                                "G","�����쳣����",
                                                "H","��ʱ���ղ�����",
                                                "I","��ʽ������������",
                                                "J","ȫ��Ѻ��",
                                                "K","�ط�Ѻ��",
                                                "L","ȫ��Ѻ�͵ط�Ѻ����",
                                                "M","��Ϣ�����������",
                                                "N","��Ϣ����֧�й���",
                                                "O","�����˻صȴ�",
                                                "P","�����˻�",
                                                "Q","��ͬ��Ӧ����δ����",
                                                "R","��ͬ��Ӧ�����ѷ���",
                                                "S","�Ѳ�ͬ��Ӧ��δ����",
                                                "T","�Ѳ�ͬ��Ӧ���ѷ���",
                                                "U","�����ѳ���",
                                                "V","������ֹ��",
                                                "Z","�����쳣����",
                                                "\0"
                };
        int j=0;
        char StatMesg[21];
        memset(StatMesg,'\0',sizeof(StatMesg));
        for(j=0;j<50;j++)
        {
                if(strcmp(TRAN_STAT[j][0],Stat)==0)
                {
                        strcpy(StatMesg,TRAN_STAT[j][1]);
                        break;
                }
                if(strcmp(TRAN_STAT[j][0],"\0")==0){
                        return NULL;
               }
        }
     strcpy(stat_content,StatMesg);
        return 0;
}

int Rtn_PAY_Txnum_Mesg(char *txnum,char *txnum_content)
{
vtcp_log("%s,%d",__FILE__,__LINE__);
     char putmsg[21];
     long i=0;
     memset(putmsg,0x00,sizeof(putmsg));
     i=atoi(txnum);
vtcp_log("%s,%d,i=[%ld]",__FILE__,__LINE__,i);
      switch(i)
      {
         case 30102:
                    memcpy(putmsg, "֧Ʊ����ҵ��",12);
                    break;
         case 30103:
                    memcpy(putmsg, "ͨ�ý���ҵ��",12);
                    break;
         case 30104: 
                    memcpy(putmsg, "֧Ʊ��ִҵ��",12);
                    break;
         case 30105: 
                    memcpy(putmsg, "ͨ�û�ִҵ��",12);
                    break;  
                    
         default:   break; 
     }
     strcpy(txnum_content,putmsg); 
vtcp_log("%s,%d putmsg=[%s]",__FILE__,__LINE__,putmsg);
    return 0;
}
int Rtn_PAY_Rcpstat_Mesg(char *rcpstat,char *rcp_content)
{
	vtcp_log("[%s][%d],��ִ״̬rcpstat[%s],rcp_content[%s]\n",__FILE__,__LINE__,rcpstat,rcp_content);
	if(memcmp(rcpstat,"00",2)==0){
		strcpy(rcp_content,"�ɹ�");
	}else if(!memcmp(rcpstat,"21",2)){
		strcpy(rcp_content,"��Сд����");
	}else if(!memcmp(rcpstat,"22",2)){
		strcpy(rcp_content,"֧Ʊ�������ȫ");	
	}else if(!memcmp(rcpstat,"23",2)){
		strcpy(rcp_content,"��Ʊ��ǩ�²���");	
	}else if(!memcmp(rcpstat,"24",2)){
		strcpy(rcp_content,"֧���������");	
	}else if(!memcmp(rcpstat,"25",2)){
		strcpy(rcp_content,"��Ʊ��δ������");	
	}else if(!memcmp(rcpstat,"26",2)){
		strcpy(rcp_content,"֧ƱӰ�����");	
	}else if(!memcmp(rcpstat,"27",2)){
		strcpy(rcp_content,"��Ʊ���ʺŻ�������");	
	}else if(!memcmp(rcpstat,"28",2)){
		strcpy(rcp_content,"��Ʊ������");	
	}else if(!memcmp(rcpstat,"29",2)){
		strcpy(rcp_content,"�ظ���ʾ����");	
	}else if(!memcmp(rcpstat,"30",2)){
		strcpy(rcp_content,"�Ǳ���Ʊ��");	
	}else if(!memcmp(rcpstat,"31",2)){
		strcpy(rcp_content,"��Ʊ��������");	
	}else if(!memcmp(rcpstat,"32",2)){
		strcpy(rcp_content,"��Ʊ���ʻ��Ѷ���");	
	}else if(!memcmp(rcpstat,"33",2)){
		strcpy(rcp_content,"��Ʊ���ʻ���ֹ��");	
	}else if(!memcmp(rcpstat,"34",2)){
		strcpy(rcp_content,"ҵ���Ѿ�����ֹ��");	
	}else if(!memcmp(rcpstat,"35",2)){
		strcpy(rcp_content,"����ǩ����֤���");	
	}else if(rcpstat[0]='\0'){
		strcpy(rcp_content," ");	
	}
	return(0);
}

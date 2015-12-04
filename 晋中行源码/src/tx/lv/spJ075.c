/*****************************************************************
��������:�������Ķ෽Э���ά����1--��ѯ��2--���ӣ�3--�޸�,4--ɾ����
		 
FUNCTION For XiaoE
Create on 2005-12-10
    (Ϊ�ﵽ������Ч����������п���132)
 ****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "RBS_DEFINE.h"
#include "cn_define.h"
#include "apl_itf.h"
#include "apl_incl.h"
#include "apl_glob.h"
#include "gs_incl.h"
#include "pb_incl.h"
#include "wd_incl.h"
#include "wd_extn.h"
#include "dg_incl.h"
#include "dg_cj_incl.h"

#include "lv_define.h"


EXEC SQL INCLUDE sqlca;
EXEC SQL INCLUDE sqlda;

static struct
  {   
  	  char   optype	      ;     /**  ��������    **/ 
  	  char	 xyno	   [60];    /**	Э���  	**/
	  char   cashactno [32];    /**	�տ��ʺ�	    **/
	  char   cashname  [60];    /**	�տ��**/
	  char   flag          ;    /**  Э��״̬��־    **/
	  char   filler	   [60];    /**	��ע    **/
  } tis;
static struct 
 {
 	  char	 filename  [16];
 	  char	 flag		   ;    /**  �ɹ���־	**/
      char	 xyno	   [60];    /**	Э���  	**/
	  char   cashactno [32];    /**	�տ��ʺ�	    **/
	  char   cashname  [60];    /**	�տ��**/
	  char   stsflag       ;    /**  Э��״̬��־    **/
	  char   filler	   [60];    /**	��ע    **/
 }tos;
static struct wd_lv_dfxym_area wd_lv_dfxym;
static struct wd_hvuniontab_area wd_hvuniontab;
EXEC SQL BEGIN DECLARE SECTION;
    char    temxyno[61];
	static struct wd_lv_dfxyd_area wd_lv_dfxyd;
EXEC SQL END DECLARE SECTION;
int dh65610_Work()
{
   dfxym_Initial();
   printf("[%d]++++++++++++\n",__LINE__);
   if(it_txcom.txrsut!=TX_SUCCESS){
        gs100_error();
        return(-1);
    }
   dfxym_Verify();
   printf("[%d]++++++++++++\n",__LINE__);
   if(it_txcom.txrsut!=TX_SUCCESS){
        gs100_error();
        return(-1);
    }
   memcpy(tos.filename,"6561",sizeof(tos.filename)); 
   if(tis.optype=='1')        
   {
      	dfxym_query();
      	if(it_txcom.txrsut != TX_SUCCESS){
            gs100_error();
            return(-1);
    	}
    	tos.flag='1';
   }              
   if(tis.optype=='2') 		
   {
   	    dfxym_insert();
   	    if(it_txcom.txrsut != TX_SUCCESS){
            gs100_error();
            return(-1);
    	}
    	tos.flag='2';
   }
   
   if(tis.optype=='3')     
   {
   		dfxym_update();
   		if(it_txcom.txrsut != TX_SUCCESS){
            gs100_error();
            return(-1);
    	}
    	tos.flag='3';
   }
   if(tis.optype=='4')     
   {
   		dfxym_delete();
   		if(it_txcom.txrsut != TX_SUCCESS){
            gs100_error();
            return(-1);
    	}
    	tos.flag='4';
   }
   printf("[%s][%d]���ݴ������\n"__FILE__,__LINE__);
   dfxym_PutMsg();
   if(it_txcom.txrsut!=TX_SUCCESS){
           gs100_error();
           return(-1);
    }
    printf("+++++++++���������++++++++\n");
	return(0);
}

/**   ȫ�ֱ����ĳ�ʼ�������ݴ���	**/
dfxym_Initial()
{
    printf("[%s][%d]������ʼ��!\n",__FILE__,__LINE__);
    strcpy(it_txcom.prgnm,"6561");
    /*  ��ʼ��tis,TOTA�ṹ*/
    memset(&wd_lv_dfxym,'\0',sizeof(wd_lv_dfxym));
    memset(&tis,'\0',sizeof(tis));
    memcpy(&tis,it_tita.labtex.text,sizeof(tis));  
    memset(&tos,' ',sizeof(tos));
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    printf("[%s][%d]�������Э�����[%s]\n",__FILE__,__LINE__,wd_lv_dfxym.xyno);
    wf_updflg.upd_acmr = FLAG_OFF;
    wf_updflg.upd_xdtl = FLAG_OFF;
    wf_updflg.upd_tlrf = FLAG_OFF;
    wf_updflg.upd_vocdtl = FLAG_OFF;
    printf("[%s][%d]�������Э�����[%s]\n",__FILE__,__LINE__,wd_lv_dfxym.xyno);
	return(0);
 }
int dfxym_Verify()
{
	if(memcmp(TITA.kinbr,LV_CZ_QSBRNO,BRNO_LEN)!=0){
		printf("%s,%d,���������Ĳ��������˽���\n",__FILE__,__LINE__);
		it_txcom.txrsut=TX_REJECT;
		strncpy(it_totw.labtex.text,"���������Ĳ��������˽���!",40);
		return(-1);
	}
	return(0);
}
/*��ѯ���� */
dfxym_query()
{
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_FIND,&wd_lv_dfxym))!=DB_OK)                                                     
    {        
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]find lv_dfxym error! sqlcode=[%d]xmh=[%c] \n",__FILE__,__LINE__,it_txcom.rtncd,wd_lv_dfxym.xyno); 
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "û���������Э���![L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    }
    printf("[%s][%d]С��Э��Ų�ѯ!\n",__FILE__,__LINE__); 	
}
/*�������� */
dfxym_insert()
{
    if(tis.flag=='1')
    {
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "��һ�����ϵ�Э��û������![L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);
    }
    wd_lv_dfxym.flag[0]=tis.flag;
    
    memcpy(wd_lv_dfxym.cashactno,tis.cashactno,sizeof(wd_lv_dfxym.cashactno)-1);
    
    memcpy(wd_lv_dfxym.cashname,tis.cashname,sizeof(wd_lv_dfxym.cashname)-1);
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    memcpy(wd_lv_dfxym.brno,TITA.kinbr,sizeof(wd_lv_dfxym.brno)-1);
    memcpy(wd_lv_dfxym.lastday,it_txcom.tbsdy,sizeof(wd_lv_dfxym.lastday)-1);
    memcpy(wd_lv_dfxym.tlrno,TITA.tlrno,sizeof(wd_lv_dfxym.tlrno)-1);
    memcpy(wd_lv_dfxym.filler,tis.filler,sizeof(wd_lv_dfxym.filler)-1); 
    printf("[%s][%d] flag=[%s[[%c] and filler=[%s]\n",__FILE__,__LINE__,wd_lv_dfxym.flag,tis.flag,wd_lv_dfxym.filler);
    printf("[%s][%d]  wd_lv_dfxym=[%s]\n",__FILE__,__LINE__,&wd_lv_dfxym); 
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_FIND,&wd_lv_dfxym))==DB_OK)                                                   
    {  
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]xyno=[%s]sqlcode=[%d]\n",__FILE__,__LINE__,wd_lv_dfxym.xyno,it_txcom.rtncd);
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "lv_dfxym���Ѿ��д�Э���![L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    }
    if(it_txcom.rtncd)
    {
        if(it_txcom.rtncd==1403)
        {        
            EXEC SQL insert into lv_dfxym values(:wd_lv_dfxym); 
            printf("[%d]+++++++++++++++++++++++++++++?????????!!!!!!!!!\n",__LINE__); 
            if(sqlca.sqlcode)
            {
                printf("insert lv_dfxym sqlca.sqlcode is [%d],[%d]\n",sqlca.sqlcode,__LINE__);
                it_txcom.txrsut = TX_REJECT;
                sprintf(it_totw.labtex.text, "����lv_dfxym�����![L%d]", __LINE__);
                return(-1);
            } 
        }
        else
        {
            it_txcom.txrsut = TX_REJECT; 
            sprintf(it_totw.labtex.text, "���ݿ����![L%d]",__LINE__); 
            gs100_error();                                                    
            return(-1);
        }
    }
}
/*�޸����� */
dfxym_update()
{
    memset(temxyno,'\0',sizeof(temxyno));
    memset(&wd_lv_dfxyd,'\0',sizeof(wd_lv_dfxyd));
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    printf("[%d]++++++++++++\n",__LINE__);
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_LOCK,&wd_lv_dfxym))!= DB_OK)                                                     
    {       
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]xyno=[%s]sqlcode=[%d]\n",__FILE__,__LINE__,wd_lv_dfxym.xyno,it_txcom.rtncd);
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "lv_dfxym������ʧ��[L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    }
    if(memcmp(wd_lv_dfxym.brno,TITA.kinbr,sizeof(wd_lv_dfxym.brno)-1) )
    {
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "ֻ��¼�����޸ģ����ɸĳ�����![L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);
    }
    wd_lv_dfxym.flag[0]=tis.flag;
    
    memcpy(wd_lv_dfxym.cashactno,tis.cashactno,sizeof(wd_lv_dfxym.cashactno)-1);
    memcpy(wd_lv_dfxym.cashname,tis.cashname,sizeof(wd_lv_dfxym.cashname)-1);
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    memcpy(wd_lv_dfxym.brno,TITA.kinbr,sizeof(wd_lv_dfxym.brno)-1);
    memcpy(wd_lv_dfxym.lastday,it_txcom.tbsdy,sizeof(wd_lv_dfxym.lastday)-1);
    memcpy(wd_lv_dfxym.tlrno,TITA.tlrno,sizeof(wd_lv_dfxym.tlrno)-1);
    memcpy(wd_lv_dfxym.filler,tis.filler,sizeof(wd_lv_dfxym.filler)-1);     
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_UPDATE,&wd_lv_dfxym))!=DB_OK)                                                     
    {        
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]update lv_dfxym error sqlcode=[%d]\n",__FILE__,__LINE__,it_txcom.rtncd); 
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "lv_dfxym����ʧ��[L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    }
    if(tis.flag=='1')  
    {
        memcpy(temxyno,wd_lv_dfxym.xyno,sizeof(temxyno)-1);
        EXEC SQL DECLARE cur_dfxyd cursor for select * from lv_dfxyd  where xyno=:temxyno;
        memcpy(temxyno,wd_lv_dfxym.xyno,sizeof(temxyno)-1);
        EXEC SQL open cur_dfxyd;
        if(sqlca.sqlcode)
	    {
	    	printf("[%s][%d]Open cursor error!sqlcode=[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
	    	it_txcom.txrsut = TX_REJECT;
	    	sprintf(it_totw.labtex.text, "���α����![L%d]",__LINE__); 
	    	gs100_error();
	    	return(-1);
	    }
        while(1)
        {
            memset(&wd_lv_dfxyd,'\0',sizeof(wd_lv_dfxyd));
	        EXEC SQL FETCH cur_dfxyd
	            into :wd_lv_dfxyd;
	        if(sqlca.sqlcode==1403)
	        {
	            break;
	        }
	        if(sqlca.sqlcode!=0)
	        {
	            it_txcom.txrsut=TX_REJECT;
	            memcpy(it_totw.labtex.text,"fetch lv_dfxyd�����ݱ����!",40);
	            printf("[%s][%d]errcode=[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
	            return(-1);
	        }
		    EXEC SQL update lv_dfxyd set flag='1' where xyno=:temxyno;
            printf("[%d]+++++++++++++++++++++++!!!!!!!!![%s]\n",__LINE__,wd_lv_dfxyd.xyno);
        }
	    EXEC SQL CLOSE cur_dfxyd;
    }
}
dfxym_delete()
{
   memset(temxyno,'\0',sizeof(temxyno));
    memset(&wd_lv_dfxyd,'\0',sizeof(wd_lv_dfxyd));
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_LOCK,&wd_lv_dfxym))!=DB_OK)                                                     
    {       
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]xyno=[%s]sqlcode=[%d]\n",__FILE__,__LINE__,wd_lv_dfxym.xyno,it_txcom.rtncd);
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "lv_dfxym������ʧ��[L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    }

    if(!memcmp(wd_lv_dfxym.brno,TITA.kinbr,sizeof(wd_lv_dfxym.brno)-1) )
    {
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "ֻ��¼�����޸ģ����ɸĳ�����![L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);
    }
     
    wd_lv_dfxym.flag[0]=tis.flag;
    memcpy(wd_lv_dfxym.cashactno,tis.cashactno,sizeof(wd_lv_dfxym.cashactno)-1);
    memcpy(wd_lv_dfxym.cashname,tis.cashname,sizeof(wd_lv_dfxym.cashname)-1);
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    memcpy(wd_lv_dfxym.brno,TITA.kinbr,sizeof(wd_lv_dfxym.brno)-1);
    memcpy(wd_lv_dfxym.lastday,it_txcom.tbsdy,sizeof(wd_lv_dfxym.lastday)-1);
    memcpy(wd_lv_dfxym.tlrno,TITA.tlrno,sizeof(wd_lv_dfxym.tlrno)-1);
    memcpy(wd_lv_dfxym.filler,tis.filler,sizeof(wd_lv_dfxym.filler)-1);     
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_DELETE,&wd_lv_dfxym))!=DB_OK)                                                     
    {        
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]delete lv_dfxym error sqlcode=[%d]\n",__FILE__,__LINE__,it_txcom.rtncd); 
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "lv_dfxymɾ��ʧ��[L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    } 
    memcpy(temxyno,wd_lv_dfxym.xyno,sizeof(temxyno)-1);
    EXEC SQL DECLARE cur_dfxyd2 cursor for select * from lv_dfxyd  where xyno=temxyno;
    EXEC SQL open cur_dfxyd2;
    if(sqlca.sqlcode)
    {
    	printf("[%s][%d]Open cursor error!sqlcode=[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
    	it_txcom.txrsut = TX_REJECT;
    	sprintf(it_totw.labtex.text, "���α����![L%d]",__LINE__); 
    	gs100_error();
    	return(-1);
    }
    while(1)
    {
        memset(&wd_lv_dfxyd,'\0',sizeof(wd_lv_dfxyd));
        EXEC SQL FETCH cur_dfxyd2
            into :wd_lv_dfxyd;
        if(sqlca.sqlcode==1403)
        {
            break;
        }
        if(sqlca.sqlcode!=0)
        {
            it_txcom.txrsut=TX_REJECT;
            memcpy(it_totw.labtex.text,"��ѯ�������ݱ����!",40);
            printf("[%s][%d]errcode=[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
            return(-1);
        }
        EXEC SQL update lv_dfxyd2 set flag='1' where xyno=:temxyno;
        printf("[%d]+++++++++++++++++++++++!!!!!!!!![%s]\n",__LINE__,wd_lv_dfxyd.xyno);
    }
	EXEC SQL CLOSE cur_dfxyd2;
}
dfxym_PutMsg()
{
    /**׼����������	**/
    memcpy(tos.xyno,wd_lv_dfxym.xyno,sizeof(tos.xyno));
    printf("[%s][%d]xyno=====[%s] \n",__FILE__,__LINE__,tos.xyno);
    memcpy(tos.cashactno,wd_lv_dfxym.cashactno,sizeof(tos.cashactno));
    printf("[%s][%d]pactno=====[%s] \n",__FILE__,__LINE__,tos.cashactno); 
    memcpy(tos.cashname,wd_lv_dfxym.cashname,sizeof(tos.cashname));	  
    tos.stsflag=wd_lv_dfxym.flag[0];
    printf("[%s][%d]stsflag=====[%c]\n",__FILE__,__LINE__,tos.stsflag);  
    gs42_mvbasic();
    if(tis.optype=='1')
    {
    	TOTW.msgend = '8';	
    }
    else
    {
    	TOTW.msgend = '1';
    }
    printf("[%s][%d] \n",__FILE__,__LINE__);
    TOTW.msgtype = 'l';
    memcpy(TOTW.msgno, "6561", sizeof(TOTW.msgno));
    apitoa(TOTA_LABEL_LENGTH+sizeof(tos),sizeof(TOTW.msglng), TOTW.msglng);
    printf("[%s][%d]  \n",__FILE__,__LINE__);  
    memset(it_totw.labtex.text ,' ', sizeof(it_totw.labtex.text));  
    memcpy(it_totw.labtex.text , &tos, sizeof(tos)); 
    cs81_mvtota(); 
}


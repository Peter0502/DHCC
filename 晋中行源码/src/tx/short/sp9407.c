/*****************************************************
* �� �� ��:  sp9407.c
* ���������� ��Ʊ�Ĳ�ѯ���ӡ
* ��    ��:  xyy
* ������ڣ� 2006-10-25 19:58
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "dc_log_c.h"
#include "com_item_add_c.h"
#include "com_branch_c.h"
int sp9407()
{
	int ret=0,i;
	int iCnt=0;
	char cTxday[9];
	char cKinbr[BRNO_LEN+1];
	char cAcc_hrt[8];
	char cAcc_no[8];
	char cDc_ind[3];
	char cAccno[8];
	char cFlag[2];
	char cAdd_ind[41];
	char cBrname[31];
	char cBuff[20000];
	char cTmp[31];
	char cFilename[100];
	double dTxamt=0.00;
	double dTxamt2=0.00;
	FILE *fp;
	struct dc_log_c  dc_log;
	struct com_branch_c com_branch;
	struct com_item_add_c  com_item_add;
	
  memset(cAcc_no  , 0 ,sizeof(cAcc_no));
  memset(cAccno   , 0 ,sizeof(cAccno));
  memset(cFlag   , 0 ,sizeof(cFlag));
  memset(cTmp     , 0, sizeof(cTmp));
  memset(cDc_ind  , 0 ,sizeof(cDc_ind));
	memset(cBuff    , 0 ,sizeof(cBuff));
	memset(cBrname  , 0 ,sizeof(cBrname));
	memset(cFilename, 0 ,sizeof(cFilename));
	memset(cKinbr   , 0 ,sizeof(cKinbr));
	memset(cTxday   , 0 ,sizeof(cTxday));
	memset(cAcc_hrt , 0 ,sizeof(cAcc_hrt));
	memset(cAdd_ind , 0 ,sizeof(cAdd_ind));

	memset(&dc_log      ,0x00,sizeof(struct dc_log_c));
	memset(&com_branch  ,0x00,sizeof(struct com_branch_c));
	memset(&com_item_add,0x00,sizeof(struct com_item_add_c));
	vtcp_log("sp9407 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	
  
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	
	get_fd_data("0910",cKinbr);
	get_fd_data("0470",cTxday);
	
	ret = Com_branch_Sel(g_pub_tx.reply, &com_branch, "br_no='%s'",cKinbr);
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack( com_branch.br_name ); 
	memcpy(cBrname,com_branch.br_name,sizeof(cBrname)-1);
	vtcp_log("[%s][%d] br_name=[%s]",__FILE__,__LINE__,cBrname);
  
  ret=pub_base_AllDwnFilName(cFilename);
	if(ret)
	{
		vtcp_log("%s,%d �����´��ļ����� ret=%d ",__FILE__,__LINE__,ret);
	}
	fp=fopen(cFilename,"w");
	if(fp==NULL)
	{
		vtcp_log("%s,%d ���´��ļ����� ret=%d ",__FILE__,__LINE__,ret);
	}
  /**���￪ʼд�ļ�**/
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"~��Ŀ��",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.10s|", cTmp);/*��Ŀ��*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"�����־",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.10s|", cTmp);/*�����־*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"������",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.8s|", cTmp);/*������*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"��������",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.10s|", cTmp);/*��������*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"����",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.10s|", cTmp);/*����*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"���׽��",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.22s|", cTmp);/*���׽��*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"�ܱ���",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.8s|",cTmp);/*�ܱ���*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"��Ŀ��",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.10s| |\n", cTmp);/*��Ŀ��*/
	ret =  Com_item_add_Dec_Sel(g_pub_tx.reply,"1=1 order by acc_hrt");
	if ( ret )
	{
		sprintf(acErrMsg,"û��[%s]����[%s]�Ĵ�Ʊ��Ϣsqlcode=[%d]", cKinbr,cTxday,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply, "T060" );
		goto	ErrExit;
	}
	
	while(1)
	{
		    i=0;
		    memset(cAcc_no      ,0,sizeof(cAcc_no));
		    memset(cAcc_hrt     ,0,sizeof(cAcc_hrt));
		    memset(cAdd_ind     ,0,sizeof(cAdd_ind));
		    memset(&com_item_add,0,sizeof(com_item_add));
		    ret = Com_item_add_Fet_Sel(&com_item_add,g_pub_tx.reply);
		    if( ret == 100 ) break;
		    if ( ret )
	      {
	        	sprintf(acErrMsg,"��ѯ[%s]������[%s]��Ŀ����",cKinbr,cAcc_hrt);
	    	    WRITEMSG
	    	    strcpy( g_pub_tx.reply, "T061" );
	    	    goto	ErrExit;
	      }
	      memcpy(cAcc_no,com_item_add.acc_no,sizeof(cAcc_no)-1);
        memcpy(cAcc_hrt,com_item_add.acc_hrt,sizeof(cAcc_hrt)-1);
        cAdd_ind[0] = com_item_add.add_ind[0];
	      ret = Dc_log_Dec_Sel(g_pub_tx.reply,"tx_date='%s' and tx_opn_br_no='%s' and acc_hrt='%s' order by dc_ind",cTxday,cKinbr,cAcc_hrt);
	      while(1)
	      {
	      	
	      	iCnt=0;
	    	  memset(&dc_log,0,sizeof(dc_log));
	    	  ret = Dc_log_Fet_Sel(&dc_log,g_pub_tx.reply);
	    	  if( ret ==100 ) break;
	        if( ret )
	        {
	          sprintf(acErrMsg,"��ѯ[%s]������[%s]��Ŀ����",cKinbr,cAcc_hrt);
	          WRITEMSG
	          strcpy( g_pub_tx.reply, "T061" );
	          goto	ErrExit;
	        }
	        vtcp_log("[%s][%d]cAdd_ind=[%s]",__FILE__,__LINE__,cAdd_ind);
	        if(cAdd_ind[0]=='1')
	        {
	            if(dc_log.dc_ind[0]=='1')
	            {
	          	  memcpy(cDc_ind,"��",sizeof(cDc_ind)-1);
	            }
	            else
	            {
	          	  memcpy(cDc_ind,"��",sizeof(cDc_ind)-1);
	            }
	            iCnt = 1;
	            dTxamt = dc_log.amt;
	            /**���￪ʼд�ļ�**/
	            fprintf(fp, "%s|", cAcc_no);/*��Ŀ��*/
	            fprintf(fp, "%s|", cDc_ind);/*�����־*/
	            fprintf(fp, "%s|", cKinbr);/*������*/
              fprintf(fp, "%s|", cBrname);/*��������*/           
              fprintf(fp, "%s|", cTxday);/*����*/
              fprintf(fp, "%.2f|", dTxamt);/*���׽��*/
              fprintf(fp, "%d|",iCnt);/*�ܱ���*/
              fprintf(fp, "%s| |\n", cAcc_no);/*��Ŀ��*/
	        }
	        if(cAdd_ind[0]=='2')
	        {
	        	  vtcp_log("[%s][%d]cAcc_hrt=[%s]",__FILE__,__LINE__,cAcc_hrt);
	        	  vtcp_log("[%s][%d]cAccno=[%s]",__FILE__,__LINE__,cAccno);
	        	  if(memcmp(cAcc_hrt,cAccno,sizeof(cAccno)-1)==0)
      	      {
      	      	break;
      	      }
              vtcp_log("[%s][%d]cAcc_hrt=[%s]",__FILE__,__LINE__,cAcc_hrt);
	        	  vtcp_log("[%s][%d]cAccno=[%s]",__FILE__,__LINE__,cAccno);
	            ret = sql_sum_double ( "dc_log" , "amt" , &dTxamt , "tx_date='%s' and tx_opn_br_no='%s' and acc_hrt='%s' and dc_ind='1'", cTxday,cKinbr,cAcc_hrt );
	            if( ret )
	            {
	              sprintf(acErrMsg,"ͳ��[%s]������[%s]��Ŀ�Ľ跽�ܽ�����",cKinbr,cAcc_hrt);
	              WRITEMSG
	              strcpy( g_pub_tx.reply, "T061" );
	              goto	ErrExit;
	            }
	            iCnt = sql_count( "dc_log", "tx_date='%s' and tx_opn_br_no='%s' and acc_hrt='%s' and dc_ind='1'", cTxday,cKinbr,cAcc_hrt );
	            memcpy(cDc_ind,"��",sizeof(cDc_ind)-1);
	            cFlag[0] = dc_log.dc_ind[0];
	            vtcp_log("[%s][%d]iCnt=[%d]",__FILE__,__LINE__,iCnt);
	        	  vtcp_log("[%s][%d]dTxamt=[%f]",__FILE__,__LINE__,dTxamt);
	            if(iCnt!=0)
	            {
	                fprintf(fp, "%s|", cAcc_no);/*��Ŀ��*/
	                fprintf(fp, "%s|", cDc_ind);/*�����־*/
	                fprintf(fp, "%s|", cKinbr);/*������*/
                  fprintf(fp, "%s|", cBrname);/*��������*/           
                  fprintf(fp, "%s|", cTxday);/*����*/
                  fprintf(fp, "%.2f|", dTxamt);/*���׽��*/
                  fprintf(fp, "%d|",iCnt);/*�ܱ���*/
                  fprintf(fp, "%s| |\n", cAcc_no);/*��Ŀ��*/
              }    
	            dTxamt=0.00;
	            iCnt=0;
	            ret = sql_sum_double ( "dc_log" , "amt" , &dTxamt , "tx_date='%s' and tx_opn_br_no='%s' and acc_hrt='%s' and dc_ind='2'", cTxday,cKinbr,cAcc_hrt );
	            if( ret )
	            {
	               sprintf(acErrMsg,"ͳ��[%s]������[%s]��Ŀ�Ĵ����ܽ�����",cKinbr,cAcc_hrt);
	               WRITEMSG
	               strcpy( g_pub_tx.reply, "T061" );
	               goto	ErrExit;
	            }
	            vtcp_log("[%s][%d]iCnt=[%d]",__FILE__,__LINE__,iCnt);
	        	  vtcp_log("[%s][%d]dTxamt=[%f]",__FILE__,__LINE__,dTxamt);
	            iCnt = sql_count( "dc_log", "tx_date='%s' and tx_opn_br_no='%s' and acc_hrt='%s' and dc_ind='2'", cTxday,cKinbr,cAcc_hrt );
	            memcpy(cDc_ind,"��",sizeof(cDc_ind)-1);
	            cFlag[0] = dc_log.dc_ind[0];
	            if(iCnt!=0)
	            {
	                fprintf(fp, "%s|", cAcc_no);/*��Ŀ��*/
	                fprintf(fp, "%s|", cDc_ind);/*�����־*/
	                fprintf(fp, "%s|", cKinbr);/*������*/
                  fprintf(fp, "%s|", cBrname);/*��������*/           
                  fprintf(fp, "%s|", cTxday);/*����*/
                  fprintf(fp, "%.2f|", dTxamt);/*���׽��*/
                  fprintf(fp, "%d|",iCnt);/*�ܱ���*/
                  fprintf(fp, "%s| |\n", cAcc_no);/*��Ŀ��*/
              } 
	            vtcp_log("[%s][%d]dTxamt=[%f]",__FILE__,__LINE__,dTxamt);
	            vtcp_log("[%s][%d]iCnt=[%d]",__FILE__,__LINE__,iCnt);
	         }
	         memcpy(cAccno,cAcc_hrt,sizeof(cAccno)-1);
	         vtcp_log("[%s][%d]cAcc_hrt=[%s]",__FILE__,__LINE__,cAcc_hrt);
	         vtcp_log("[%s][%d]cAccno=[%s]",__FILE__,__LINE__,cAccno);
         }
         Dc_log_Clo_Sel();
	}
	Com_item_add_Clo_Sel();
	fclose(fp);
	set_zd_data( DC_FILE_SND,"2" );
  
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

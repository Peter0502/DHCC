/*************************************************************
* �� �� ����rpt801.c
* �����������˻�ӯҪ���µĴ���������ӡ.
*           ��ȡ�ļ� pfm/cdktj801.txt ����,�������������,select���.---�ݲ�ʹ�ô˷���.д�����������.
*           ��ӡ������ report/RPT801.txt
*           20110610 ֻ��ӡ��������͵�ͳ��ֵ,����ӡ��ϸ.
* ��    �ߣ�
* ������ڣ�20110610
* 
*
* �޸ļ�¼��
* ��    �ڣ�
* �� �� �ˣ�
* �޸����ݣ�
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( iRet ) \
	  {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "tx_def_c.h"
#include "com_sys_parm_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "ln_mst_c.h"

rpt801()
{
    int    iRet = 0;
    int    iCount = 0;
    long   lTmpDate = 0;
    double dSum = 0.0;
    char   cFilename[128];
    FILE   *fp = NULL;
    char   sTmpbuf[1024];
    char   sComm[128];
    struct com_sys_parm_c sCom_sys_parm;
    struct ln_mst_c sLn_mst;
    memset(&sCom_sys_parm, 0x00, sizeof(struct com_sys_parm_c));
    memset(&sLn_mst,0x00,sizeof(struct ln_mst_c));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    memset(sComm,0x00,sizeof(sComm));
    
    iRet=sql_begin(); /*������*/
    if( iRet )
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    /* ��ʼ��ȫ�ֱ��� */
    pub_base_sysinit();
      
    /* ȡ�����ӡ���� */
    iRet = Com_sys_parm_Sel( g_pub_tx.reply, &sCom_sys_parm, "1=1" );
    ERR_DEAL
    
    g_pub_tx.tx_date = sCom_sys_parm.lst_date;
    /**ɾ���ļ�.jk_cdktj.txt **/
    strcpy(sComm,"rm -f /home/jzht/bin/jk_cdktj.txt");
    system(sComm);
    /**�����ļ�.jk_cdktj.txt **/
    strcpy(cFilename, "/home/jzht/bin/jk_cdktj.txt" );
    if( (fp = fopen(cFilename,"ab+")) == NULL)
    {
        vtcp_log("[%s][%d]�ļ���ȡʧ��..",__FILE__,__LINE__ );
        strcpy(g_pub_tx.reply,"L206");
        goto ErrExit;
    }
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    strcpy(sTmpbuf,"       ��  ��  ��  ��  ҵ  ��  ��  ��  ��  ��  ��  ��  ͳ  ��  ��       \n");
    fputs(sTmpbuf,fp);
    /**��������**/
    vtcp_log("[%s][%d] g_pub_tx.tx_date=[%ld]",__FILE__,__LINE__,g_pub_tx.tx_date);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"����:    %8d      \n",g_pub_tx.tx_date);
    fputs(sTmpbuf,fp);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    strcpy(sTmpbuf,"========================================================================\n");
    fputs(sTmpbuf,fp);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    strcpy(sTmpbuf,"��������                                   ����               ���ϼ�\n");
    fputs(sTmpbuf,fp);
    /**ʣ�ൽ����>=1��ĵ�λ���ڴ�� 12����.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"ʣ�ൽ����>=1��ĵ�λ���ڴ��            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**jk**/
    
        /**ʣ�ൽ����<1��ĵ�λ���ڴ�� 12����.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"ʣ�ൽ����<1��ĵ�λ���ڴ��            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    /**jk**/
    /**ʣ�ൽ����>=1��Ķ��ڴ����� 12����.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date >= %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date >= %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"ʣ�ൽ����>=1��Ķ��ڴ�����            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);

    /**ʣ�ൽ����<1��Ķ��ڴ����� 12����.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"ʣ�ൽ����<1��Ķ��ڴ�����            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
       
    /**ʣ�ൽ����<30��ĵ�λ���ڴ�� 30��.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30���=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"ʣ�ൽ����<30��ĵ�λ���ڴ��            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**ʣ�ൽ����<30��Ķ��ڴ����� 30��.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30���=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"ʣ�ൽ����<30��Ķ��ڴ�����            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**���ڴ�����д���ģ�**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("dd_mst"," cif_no in(select cif_no from ln_mst where ac_sts!='*' and cif_no<20000000) and ac_type='5' and ac_sts='1' ");
    iRet = sql_sum_double("dd_mst","bal",&dSum," cif_no in(select cif_no from ln_mst where ac_sts!='*' and cif_no<20000000) and ac_type='5' and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"���ڴ�����д���ģ�                 %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**�Թ����ڴ��д���ģ�**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("dd_mst"," cif_no in(select cif_no from ln_mst where ac_sts!='*' and cif_no>50000000 and cif_no<60000000) and ac_type in('1','2','3','4') and ac_sts='1' ");
    iRet = sql_sum_double("dd_mst","bal",&dSum," cif_no in(select cif_no from ln_mst where ac_sts!='*' and cif_no>50000000 and cif_no<60000000) and ac_type in('1','2','3','4') and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"�Թ����ڴ��д���ģ�                 %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**ɽ���������ʼ������޹�˾  ���ڴ��**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("dd_mst"," cif_no='50082098' and ac_type in('1','2','3','4') and ac_sts='1' ");
    iRet = sql_sum_double("dd_mst","bal",&dSum," cif_no in(select cif_no from dd_mst where name='ɽ���������ʼ������޹�˾') and ac_type in('1','2','3','4') and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"ɽ���������ʼ������޹�˾ ���ڴ��        %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);    
    
    /**ɽ���������ʼ������޹�˾  ���ڴ�� **/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("td_mst"," cif_no in(select cif_no from td_mst where name='ɽ���������ʼ������޹�˾') and ac_sts='1' ");
    iRet = sql_sum_double("td_mst","bal",&dSum," cif_no in(select cif_no from td_mst where name='ɽ���������ʼ������޹�˾')  and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"ɽ���������ʼ������޹�˾ ���ڴ��        %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);    
      
    /**ɽ����ɭ���м������޹�˾  ���ڴ��**/
     lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("dd_mst"," cif_no='50082098' and ac_type in('1','2','3','4') and ac_sts='1' ");
    iRet = sql_sum_double("dd_mst","bal",&dSum," cif_no in(select cif_no from dd_mst where name='ɽ����ɭ���м������޹�˾') and ac_type in('1','2','3','4') and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"ɽ����ɭ���м������޹�˾ ���ڴ��        %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);   
    
    /**ɽ����ɭ���м������޹�˾  ���ڴ��**/
    
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("td_mst"," cif_no in(select cif_no from td_mst where name='ɽ����ɭ���м������޹�˾') and ac_sts='1' ");
    iRet = sql_sum_double("td_mst","bal",&dSum," cif_no in(select cif_no from td_mst where name='ɽ����ɭ���м������޹�˾')  and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"ɽ����ɭ���м������޹�˾ ���ڴ��        %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
        
    /**һ���ڵ��ڵĵ�λ����**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"һ���ڵ��ڵĵ�λ����                     %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**һ���ڵ��ڵĵ�λ������Ϣ**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    /*iSum ��Ҫ��������.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"һ���ڵ��ڵĵ�λ������Ϣ                 %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**���У�ʣ�ൽ����<30��ĵ�λ����**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30���=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"���У�ʣ�ൽ����<30��ĵ�λ����          %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**���У�ʣ�ൽ����<30��ĵ�λ������Ϣ**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30���=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    /*iSum ��Ҫ��������.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"���У�ʣ�ൽ����<30��ĵ�λ������Ϣ      %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**һ���ڵ��ڵĸ��˴���**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"һ���ڵ��ڵĸ��˴���                     %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**һ���ڵ��ڵĸ��˴�����Ϣ**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    /*iSum ��Ҫ��������.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date < %ld and ac_sts='1' and cif_no < 20000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"һ���ڵ��ڵĸ��˴�����Ϣ                 %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**���У�ʣ�ൽ����<30��ĸ��˴���**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30���=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"���У�ʣ�ൽ����<30��ĸ��˴���          %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**���У�ʣ�ൽ����<30��ĸ��˴�����Ϣ**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30���=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    /*iSum ��Ҫ��������.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date < %ld and ac_sts='1' and cif_no < 20000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"���У�ʣ�ൽ����<30��ĸ��˴�����Ϣ      %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**һ�����ϣ���һ�꣩���ڵĵ�λ����**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"һ�����ϣ���һ�꣩���ڵĵ�λ����         %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**һ�����ϣ���һ�꣩���ڵĵ�λ������Ϣ**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    /*iSum ��Ҫ��������.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"һ�����ϣ���һ�꣩���ڵĵ�λ������Ϣ     %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**һ�����ϣ���һ�꣩���ڵĸ��˴���**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date >= %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date >= %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"�������ܽ��ʧ��! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"һ�����ϣ���һ�꣩���ڵĸ��˴���         %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**һ�����ϣ���һ�꣩���ڵĸ��˴�����Ϣ**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12�º�=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date >= %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    /*iSum ��Ҫ��������.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date >= %ld and ac_sts='1' and cif_no < 20000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"һ�����ϣ���һ�꣩���ڵĸ��˴�����Ϣ     %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    

    
    fclose(fp);
    
    
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���д���������ӡ�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���д���������ӡʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}


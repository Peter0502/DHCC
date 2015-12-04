/************************************************************
* �� �� ��:   sp6345.c
* ����������  �ֽ�Ʊ�⸶
* ��    ��:    
* ������ڣ�  
* ��    ��:   xyy 
* �޸�����:   2008-03-24 00:35
insert into tx_sub_def values('6345','�ֽ�Ʊ�⸶','sp6345',0,0);
insert into tx_flow_def values('6345',0,'6345','#$');
insert into tx_def values('6345','�ֽ�Ʊ�⸶','10000000000000000000000000000001000000001000100000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','3','2')
**************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "cashier_c.h"
#include "lv_define.h"

int sp6345()
{
    int ret=0;
    char cTel[5];
    char cPono[21];
    char cTemp[21];
    long lSignday = 0;
    struct cashier_c Cashier;

    memset(cPono , 0 , sizeof(cPono));
    memset(cTel      , 0 , sizeof(cTel));
    memset(&Cashier  , 0 , sizeof(Cashier));
       
    /*** ��ʼ��ȫ�ֱ��� ***/
    pub_base_sysinit();

    /**��Ʊ����ǰ��8����**/
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    get_zd_data("0380",cTemp);
    zip_space(cTemp);
    vtcp_log("[%s][%d]cTemp==[%s]  ",__FILE__,__LINE__,cTemp);
    memcpy(cPono+20-strlen(cTemp),cTemp,strlen(cTemp));
    vtcp_log("[%s][%d]cPono==[%s]  ",__FILE__,__LINE__,cPono);

    get_zd_long("0450",&lSignday);    /*ǩ������*/
    vtcp_log("%s,%d wtday=%ld",__FILE__,__LINE__,lSignday);
    get_zd_data("0070",cTel);         /*�⸶����Ա*/  
    vtcp_log("[%s][%d]cTel=[%s]\n",__FILE__,__LINE__,cTel);
    /*****NEWBP3****/
    ret=Cashier_Dec_Upd (g_pub_tx.reply,"pono='%s' and signday =%ld ",cPono,lSignday);
    if(ret)
    {
        vtcp_log("[%s][%d]Cashier_Dec_Upd����==[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"�˱�Ʊ�Ų�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    ret=Cashier_Fet_Upd(&Cashier,g_pub_tx.reply);
    if(ret)
    {
        vtcp_log("[%s][%d]Cashier_Fet_Upd����==[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"�˱�Ʊ�Ų�����\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(Cashier.bptype[0]=='0')
    {
    	  vtcp_log("[%s][%d]Cashier_Fet_Upd����==[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"ת�˱�Ʊ���ؽ⸶\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    else if (Cashier.stat[0]=='1')
    {
    	vtcp_log("[%s][%d]Cashier_Fet_Upd����==[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"��Ʊû�жҸ�\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    else if (Cashier.stat[0]=='6')
    {
    	vtcp_log("[%s][%d]Cashier_Fet_Upd����==[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"��Ʊ����Ʊ\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    else if (Cashier.stat[0]=='4')
    {
    	vtcp_log("[%s][%d]Cashier_Fet_Upd����==[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"��Ʊ�ѹ�ʧ\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
     else if (Cashier.stat[0]=='3')
    {
    	vtcp_log("[%s][%d]Cashier_Fet_Upd����==[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"��Ʊ�ѽ⸶\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    else if (Cashier.stat[0]!='2') 
    {
    	vtcp_log("[%s][%d]Cashier_Fet_Upd����==[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"��Ʊ���ǶҸ�״̬\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***** end NEWBP3****/
    if(memcmp(Cashier.cashtlrno,cTel,sizeof(Cashier.cashtlrno)-1)!=0)
    {
    	  vtcp_log("[%s][%d]���Ǵ˲���Ա���ĶҸ�==[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"���Ǵ˲���Ա���ĶҸ�\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]׼����ʼ���±��¼\n",__FILE__,__LINE__);
 
    strcpy( Cashier.stat,"3" ); /**�ֽ�⸶**/
    strcpy( g_pub_tx.cur_no,"01" );
    g_pub_tx.add_ind[0]='0';
    g_pub_tx.tx_amt1=Cashier.avbal;
    Cashier.jqday=g_pub_tx.tx_date;
    ret=Cashier_Upd_Upd(Cashier,g_pub_tx.reply );
    if(ret)
    {
        Cashier_Debug(&Cashier);
        vtcp_log("[%s][%d]�޸ı�Ʊ��¼����[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"�޸ı�Ʊ��¼����",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Cashier_Clo_Upd();
    vtcp_log("[%s][%d]�޸ı�Ʊ��¼�ɹ�\n",__FILE__,__LINE__);
    memcpy(g_pub_tx.beg_note_no,cTemp,16);/***NEWBP3***/  /* 20110422 [12->16] */
    ret=pub_ins_trace_log();
    if(ret)
    {
        vtcp_log("[%s][%d]�Ǽǽ�����ˮ����[%d]  ",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357����**/
        goto ErrExit;
    }
    vtcp_log("[%s][%d]�Ǽǽ�����ˮ���",__FILE__,__LINE__);
    set_zd_double("0390",Cashier.avbal);
    set_zd_double("0400",Cashier.avbal);
    set_zd_data("0660","1");
    set_zd_data("0670","1");/***NEWBP3***/
    set_zd_data("0680","1");
    set_zd_data("0210","01");
    set_zd_data("0880","�ֽ�⸶");
    char cKinbr[6];
    char cAcno[20];
    memset(cKinbr,0,sizeof(cKinbr));
    memset(cAcno,0,sizeof(cAcno));
    memcpy(cKinbr,g_pub_tx.tx_br_no,5);
    g_reply_int=pub_base_GetParm_Str(cKinbr,PARM_LVGZ,cAcno);
    if(g_reply_int){
        vtcp_log("[%s][%d]�õ����˿�Ŀ����!\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"��ȡ�����ʺ�ʧ��!");
        strcpy(g_pub_tx.reply,"S101");
        goto ErrExit;
    }
    set_zd_data("0300",cAcno);

    vtcp_log("[%s][%d]~~~~~~~~~~~~����ִ�����!\n",__FILE__,__LINE__);

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

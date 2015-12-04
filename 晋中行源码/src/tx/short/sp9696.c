/*************************************************
* �� �� ��:  sp9696.c
* ���������� ����Ʊ��˳��Ų�ѯ��Ʊ��ϸ��Ϣ 
* ������ڣ� 2007��12��18��
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/
#include <string.h>
#include "public.h"
#include "cashier_c.h"
struct D37TOS_GROUP{
    char    pono[16];           /* Ʊ�ݺ��� 0 */        /* 20110422 [12->16] */
    char    brno[7];            /* ������  12 */
    char    txday[8];           /* �������� 19 */
    char    avbal[15];          /* Ʊ����[��Ʊ���] 27 */
    char    useamt[15];         /* ʵ�ʽ����� 42 */
    char    feeamt[15];         /* ������ 57 */
    char    leavebal[15];       /* ������ 72 */
    char    paybrno[12];        /* �����˿������к� 87 */
    char    payactno[32];       /* �������ʺ� 99 */
    char    payname[60];        /* ���������� 131 */
    char    paybrname[60];      /* ���������� 191 */
    char    cashactno[32];      /* �տ����ʺ� 251 */
    char    cashbrno[12];       /* ָ���Ҹ����� 283 */
    char    cashbrname[60];     /* ָ���Ҹ��������� 295 */
    char    cashname[60];       /* �տ������� 355 */
    char    stat;               /* ��ǰ״̬ 415 */
    char    regday[8];          /* �Ǽ����� 416 */
    char    signday[8];         /* ǩԼ���� 424 */
    char    cashday[8];         /* �ܸ����� 432 */
    char    jqday[8];           /* �������� 440 */
    char    endday[8];          /* �������� 448 */
    char    passwd[10];         /* ��Ѻ 456 */
    char     avbaltype;         /* 1.���2������ 466 */
    char     bptype;            /* ��Ʊ���� 467 */
    char     signtype;          /* ת����־0.�ֽ�1.ת���ʺ� 2.��Ŀ��ת 468 */
    char     cashflag;          /* ת����־0.�ֽ�1.ת���ʺ� 2.��Ŀ��ת 469 */
    char     reason[60];        /* ���� 470 */
    char     feeflag;           /* �����ѱ�־0.�ֽ�1.�ֽ�Ӽ�2.ת��3.ת�ʼӼ�4����530*/
    char     optlrno[6];        /* ¼���Ա 531 */
    char     chktlrno[6];       /* ���Ϲ�Ա 537 */
    char     passtlrno[6];      /* ��Ѻ����Ա 543 */
    char     cashtlrno[6];      /* �Ҹ�����Ա 549 */
    char     jqtlrno[6];        /* �������Ա 555 */
    char     reqno[10];         /* ������� 561 */
}tos;
int sp9696()
{
    int    ret;
    long lPono=0;
    char    cQuery[2];
    char    cHnbz[2];
    char    cPono[17];      /* 20110422 [13->17] */
    char    cTemp[21];
    struct    cashier_c    cashier;    /*��Ʊ������Ϣ��*/
    
    memset(cQuery  , 0 , sizeof(cQuery));
    memset(cHnbz   , 0 , sizeof(cHnbz));
    memset(cPono   , 0 , sizeof(cPono));
    memset(cTemp   , 0 , sizeof(cTemp));
    memset(&cashier, 0 , sizeof(struct cashier_c));
    
    get_zd_long("0440",&cashier.signday);     /*��Ʊ����*/
    get_zd_data("0300",cPono);                /*��Ʊ����*/
    get_zd_data("0200",cHnbz);                /*���ڱ�־*/
    get_zd_data("0110",cQuery);               /*������־*/
    
    vtcp_log("[%s][%d]cPono==[%s]",__FILE__,__LINE__,cPono);
    vtcp_log("[%s][%d]cHnbz==[%c]",__FILE__,__LINE__,cHnbz[0]);
    vtcp_log("[%s][%d]cQuery==[%c]",__FILE__,__LINE__,cQuery[0]);
    cashier.inflag[0]=cHnbz[0];
    
    memcpy(cTemp,"00000000000000000000",sizeof(cTemp)-1);
    vtcp_log("[%s][%d]cTemp==[%s]  ",__FILE__,__LINE__,cTemp);
    memcpy(cTemp+4,cPono,sizeof(cPono)-1);     /* 20110422 [8->4] */
    vtcp_log("[%s][%d]cTemp==[%s]  ",__FILE__,__LINE__,cTemp);
    memcpy(cashier.pono,cTemp,sizeof(cashier.pono)-1);
    
    vtcp_log("[%s][%d]cashier.pono==[%s]  ",__FILE__,__LINE__,cashier.pono);
    vtcp_log("[%s][%d]cashier.signday=[%ld]",__FILE__,__LINE__,cashier.signday);
    vtcp_log("[%s][%d]cashier.inflag=[%c] ",__FILE__,__LINE__,cashier.inflag[0]);
    if(cashier.signday==0)
    {
        cashier.signday=g_pub_tx.tx_date;
    }
    ret=Cashier_Sel(g_pub_tx.reply,&cashier,"pono='%s' and signday='%ld' ",cashier.pono,cashier.signday);
    if(ret==100)
    {
        vtcp_log("[%s][%d]������Ϣ�����޴˱�Ʊ����[%s]\n",__FILE__,__LINE__,cashier.pono);
        sprintf(acErrMsg,"������Ϣ�����޴˱�Ʊ����");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }else if(ret)
    {
        vtcp_log("[%s][%d]��ѯ������Ϣ�����[%d]\n",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"ִ��Cashier_Sel��[%d]",ret);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /*��ǰ̨��Ļ��ֵ*/
    set_zd_data("0250",  cashier.payname); /*����������*/
    vtcp_log("[%s][%d]cashier.payname=[%s]\n",__FILE__,__LINE__,cashier.payname);
    set_zd_data("0260",  cashier.cashname); /*�տ�������*/
    vtcp_log("[%s][%d]cashier.cashname=[%s]\n",__FILE__,__LINE__,cashier.cashname);
    set_zd_data("0330",  cashier.payactno);/*�������ʺ�*/
    vtcp_log("[%s][%d]cashier.payactno=[%s]\n",__FILE__,__LINE__,cashier.payactno);
    set_zd_data("0310",  cashier.cashactno);/*�տ����ʺ�*/
    vtcp_log("[%s][%d]cashier.cashactno=[%s]\n",__FILE__,__LINE__,cashier.cashactno);
    set_zd_double("0390",cashier.avbal);   /*��Ʊ���*/
    vtcp_log("[%s][%d]cashier.avbal=[%f]\n",__FILE__,__LINE__,cashier.avbal);
    set_zd_double("0400",cashier.useamt);  /*������*/
    vtcp_log("[%s][%d]cashier.useamt=[%f]\n",__FILE__,__LINE__,cashier.useamt);
    set_zd_double("0410",cashier.feeamt);  /*������*/
    vtcp_log("[%s][%d]cashier.feeamt=[%f]\n",__FILE__,__LINE__,cashier.feeamt);
    set_zd_double("0420",cashier.leavebal);/*������*/
    vtcp_log("[%s][%d]cashier.leavebal=[%f]\n",__FILE__,__LINE__,cashier.leavebal);
    set_zd_long("0440", cashier.regday);    /*�Ǽ�����*/
    vtcp_log("[%s][%d]cashier.regday=[%ld]\n",__FILE__,__LINE__,cashier.regday);
    set_zd_long("0450", cashier.signday);   /*ǩԼ����*/
    vtcp_log("[%s][%d]cashier.signday=[%ld]\n",__FILE__,__LINE__,cashier.signday);
    set_zd_long("0460", cashier.cashday);   /*�Ҹ�����*/
    vtcp_log("[%s][%d]cashier.cashday=[%ld]\n",__FILE__,__LINE__,cashier.cashday);
    set_zd_long("0470", cashier.jqday);     /*��������*/
    vtcp_log("[%s][%d]cashier.jqday=[%ld]\n",__FILE__,__LINE__,cashier.jqday);
    set_zd_data("0520",cashier.paybrno);   /*�����˿������к�*/
    vtcp_log("[%s][%d]cashier.paybrno=[%s]\n",__FILE__,__LINE__,cashier.paybrno);
    set_zd_data("0530",cashier.passwd);    /*��Ѻ*/
    vtcp_log("[%s][%d]cashier.passwd=[%s]\n",__FILE__,__LINE__,cashier.passwd);
    set_zd_data("0580",cashier.cashbrno);  /*ָ���Ҹ�����*/
    vtcp_log("[%s][%d]cashier.cashbrno=[%s]\n",__FILE__,__LINE__,cashier.cashbrno);
    set_zd_data("0590",cashier.chktlrno);  /*���˹�Ա*/
    vtcp_log("[%s][%d]cashier.chktlrno=[%s]\n",__FILE__,__LINE__,cashier.chktlrno);
    set_zd_data("0600",cashier.passtlrno); /*��Ѻ����Ա*/
    vtcp_log("[%s][%d]cashier.passtlrno=[%s]\n",__FILE__,__LINE__,cashier.passtlrno);
    set_zd_data("0610",cashier.cashtlrno); /*�Ҹ�����Ա*/
    vtcp_log("[%s][%d]cashier.cashtlrno=[%s]\n",__FILE__,__LINE__,cashier.cashtlrno);
    set_zd_data("0620",cashier.pono);      /*��Ʊ����*/
    vtcp_log("[%s][%d]cashier.pono=[%s]\n",__FILE__,__LINE__,cashier.pono);
    set_zd_data("0640",cashier.jqtlrno);   /*�������Ա*/
    vtcp_log("[%s][%d]cashier.jqtlrno=[%s]\n",__FILE__,__LINE__,cashier.jqtlrno);
    set_zd_data("0650",cashier.optlrno);   /*¼���Ա*/
    vtcp_log("[%s][%d]cashier.optlrno=[%s]\n",__FILE__,__LINE__,cashier.optlrno);
    set_zd_data("0670",cashier.avbaltype); /*�����־1����,2������*/
    vtcp_log("[%s][%d]cashier.avbaltype=[%s]\n",__FILE__,__LINE__,cashier.avbaltype);
    set_zd_data("0680",cashier.bptype);    /*��Ʊ����*/
    vtcp_log("[%s][%d]cashier.bptype=[%s]\n",__FILE__,__LINE__,cashier.bptype);
    set_zd_data("0690",cashier.ccflag);    /*ת����־*/
    vtcp_log("[%s][%d]cashier.ccflag=[%s]\n",__FILE__,__LINE__,cashier.ccflag);
    set_zd_data("0700",cashier.cashflag);  /*ת����־*/
    vtcp_log("[%s][%d]cashier.cashflag=[%s]\n",__FILE__,__LINE__,cashier.cashflag);
    set_zd_data("0740",cashier.reason);    /*����*/
    vtcp_log("[%s][%d]cashier.reason=[%s]\n",__FILE__,__LINE__,cashier.reason);
    set_zd_long("0780", cashier.endday);    /*��������*/
    vtcp_log("[%s][%d]cashier.endday=[%ld]\n",__FILE__,__LINE__,cashier.endday);
    set_zd_data("0810",cashier.paybrname); /*����������*/
    vtcp_log("[%s][%d]cashier.paybrname=[%s]\n",__FILE__,__LINE__,cashier.paybrname);
    set_zd_data("0820",cashier.cashbrname);/*ָ���Ҹ���������*/
    vtcp_log("[%s][%d]cashier.cashbrname=[%s]\n",__FILE__,__LINE__,cashier.cashbrname);
    set_zd_data("0610",cashier.ovocnum);   /*�������*/
    vtcp_log("[%s][%d]cashier.ovocnum=[%s]\n",__FILE__,__LINE__,cashier.ovocnum);
    set_zd_data("0910",cashier.brno);      /*������*/
    vtcp_log("[%s][%d]cashier.brno=[%s]\n",__FILE__,__LINE__,cashier.brno);
    set_zd_data("0930",cashier.feeflag);   /*�����ѱ�־*/
    vtcp_log("[%s][%d]cashier.feeflag=[%s]\n",__FILE__,__LINE__,cashier.feeflag);
    set_zd_data("0980",cashier.stat);      /*��ǰ״̬*/
    vtcp_log("[%s][%d]cashier.stat=[%s]\n",__FILE__,__LINE__,cashier.stat);
OkExit:
    strcpy( g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return:reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

/*******************************************************************************
*** �� �� ��:  V1.0
*** ��������: shangyongchao
*** ��    ��: 2013/5/22 15:14:25
*** ����ģ��: �����صĲ�ѯ
*** ��������: sp8508
*** ��������: ��Ʋ�Ʒ��ѯ
*** �����б�:
***         1��
***         2��

*******************************************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_parm_c.h"
#include "com_branch_c.h"
#include "fn_br_profit_c.h"
#include "fn_flt_radio_parm_c.h"
#define PAGE_LINE 40
static void fp_head(FILE *fp,struct fn_parm_c sFn_parm);
int sp8508()
{
    char cTx_code[11];
    
    memset(cTx_code,0x00,sizeof(cTx_code));
    
    pub_base_sysinit();
    
    get_zd_data("0650",cTx_code);
    
    if(strcmp(cTx_code,"0001") == 0)
    {
        if(func_0001())
        {
            vtcp_log("��ѯ��Ʋ�Ʒ������������");
            goto ErrExit;
        }
    }
    else if(strcmp(cTx_code,"0002")  == 0)
    {
        if(func_0002())
        {
            vtcp_log("��ѯ��Ʋ�Ʒ���ʧ��");
            goto ErrExit;
        }
    }
    else if(strcmp(cTx_code,"0003")  == 0)
    {
        if(func_0003())
        {
            vtcp_log("��ѯ��Ʋ�Ʒ��ϸʧ��");
            goto ErrExit;
        }
    }
    else if(strcmp(cTx_code,"0004") == 0)
    {
        if(func_0004())
        {
            vtcp_log("��ѯ�ͻ���ϸʧ��");
            goto ErrExit;
        }
    }
    else
    {
        vtcp_log("����Ĳ�ѯ��������");
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log("Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}

static int func_0001()
{
    int ret = 0;
    char cPrdt_code[21];
    char cTitle[61];
    long lBeg_date = 0;
    long lEnd_date = 0;
    long lRt_date = 0;
    char cStat[2];
    char cWherelist[1024];
    char prdt_type[10];/*��Ʒ����*/
    FILE *fp = NULL;
    char cFilename[256];
    struct fn_parm_c sFn_parm;
    char intst_type[14];/*��������*/
    char cif_type[14];/*�ͻ�����*/
    char fn_parm_sts[20];/*��Ʒ״̬����*/
    char cur_name[10];/*����*/
    char term_type[5];/*��������*/
    double n = 0.0;
    double rate = 0.0;
    double rate1 = 0.0;
    double amt = 0.0;
    double dSum_amt1 = 0.0;
    double dSum_amt2 = 0.0;
    double dSum_amt3 = 0.0;
    double dSum_amt4 = 0.0;
    double dSum_amt5 = 0.0;
    double dSum_amt6 = 0.0;
    int iRet =0;
    struct fn_flt_radio_parm_c sFn_flt_radio_parm;
    
    memset(cFilename,0x00,sizeof(cFilename));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cTitle,0x00,sizeof(cTitle));
    memset(cStat,0x00,sizeof(cStat));
    memset(cWherelist,0x00,sizeof(cWherelist));
    memset(prdt_type,0x00,sizeof(prdt_type));
    memset(intst_type,0x00,sizeof(intst_type));
    memset(cif_type,0x00,sizeof(cif_type));
    memset(fn_parm_sts,0x00,sizeof(fn_parm_sts));
    memset(cur_name,0x00,sizeof(cur_name));
    memset(term_type,0x00,sizeof(term_type));
    memset(&sFn_flt_radio_parm,0x00,sizeof(sFn_flt_radio_parm));
    
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0250",cTitle);
    get_zd_long("0440",&lBeg_date);
    get_zd_long("0450",&lEnd_date);
    get_zd_long("0460",&lRt_date);
    get_zd_data("0660",cStat);

    /* �����ļ� */
    pub_base_AllDwnFilName(cFilename);
    fp=fopen(cFilename,"w");
    if( fp==NULL )
    {
        vtcp_log(" open file [%s] error ",cFilename);
        strcpy(g_pub_tx.reply,"S066");
        goto ErrExit;
    }

    /* ���ѯ���� */
    if( strlen(cPrdt_code)>0 )
    {
        sprintf(cWherelist,"prdt_code='%s' and ",cPrdt_code);
    }
    if( strlen(cTitle)>0 )
    {
        sprintf(cWherelist+strlen(cWherelist),"title like '%%%s%%' and ",cTitle);
    }
    if( lBeg_date>0 )
    {
        sprintf(cWherelist+strlen(cWherelist),"beg_date>=%ld and ",lBeg_date);
    }
    if( lEnd_date>0 )
    {
        sprintf(cWherelist+strlen(cWherelist),"end_date>=%ld and ",lEnd_date);
    }
    if( lRt_date >0)
    {
        sprintf(cWherelist+strlen(cWherelist),"return_date=%ld and ",lRt_date);
    }
    if( strlen(cStat) >0)
    {
        sprintf(cWherelist+strlen(cWherelist),"stat='%s' and ",cStat);
    }
    vtcp_log("�õ��Ĳ�ѯ����Ϊ  [%s] ",cWherelist);
    
    /* ���� �����ѱ��� �йܷѱ��� ֧�зֺ���� �������� ��������ע����lpj20140410�޸�*/
    /*fprintf(fp,"~��Ʒ����|��Ʒ����|��Ʒ����|��������|ļ����ʼ����|ļ����������|��Ʒ��������|��Ʒ��������|�黹��������|��������|����|�ͻ�����|����|��������|���յȼ�|Ͷ�ʷ���|Ԥ������껯������|ʵ����������|�������ʱ�־|���ر������|����������|�йܷ���|֧�зֺ����|��Ʒ���ļ�����|���۽��|���ݽ��|�������|���������|��λ�����|�Ƿ���|�Ƿ����|��ط���|������|����ļ���ʽ��˻�|��λļ���ʽ��˻�|״̬|\n");
    */
     fprintf(fp,"~��Ʒ����|��Ʒ����|��Ʒ����|��������|ļ����ʼ����|ļ����������|��Ʒ��������|��Ʒ��������|�黹��������|��������|����|�ͻ�����|����|Ԥ������껯������|ʵʱ��Ȩ������|ʵ����������|���ر������|��Ʒ���ļ�����|���۽��|�������|���������|��λ�����|�Ƿ���|�Ƿ����|��ط���|����ļ���ʽ��˻�|��λļ���ʽ��˻�|״̬|\n");
     ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"%s 1=1",cWherelist);
    if( ret )
    {
        vtcp_log("declare cursor error! ret[%d] ",ret);
        strcpy( g_pub_tx.reply,"D324" );
        goto ErrExit;
    }
    while(1)
    {
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        memset(intst_type,0x00,sizeof(intst_type));
    		memset(cif_type,0x00,sizeof(cif_type));
   			memset(fn_parm_sts,0x00,sizeof(fn_parm_sts));
    		memset(cur_name,0x00,sizeof(cur_name));
    		memset(term_type,0x00,sizeof(term_type));
        
        ret=Fn_parm_Fet_Sel(&sFn_parm,g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log(" fn_parm fetch over! ");
            break;
        }
        else if( ret )
        {
            vtcp_log("fn_parm fetch error! ret[%d] ",ret);
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        /*beg add by wjl_20140324 */
        if(memcmp(sFn_parm.prdt_type,"1",1)==0)
        
        {
        		strcpy(prdt_type,"���ʽ");
        }
        else
        {
        	strcpy(prdt_type,"����ʽ");
        }
        
        /*add by lwb 20141204  ����һ����ʵʱ��Ȩ���ʵĲ���*/
        ret = Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_radio_parm,"prdt_code = '%s'",sFn_parm.prdt_code);
        if(ret &&ret != 100)
        {
        	vtcp_log("Fn_flt_radio_parm_Sel error! ret[%d] ",ret);
          strcpy( g_pub_tx.reply,"D324" );
          goto ErrExit;	
        }
        else if(ret ==0){
        
       	if(sFn_flt_radio_parm.buy_amt){
        iRet = sql_sum_double("fn_reg","tx_amt",&dSum_amt1,"prdt_code='%s' and tx_amt >=0 and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt);
        amt = sFn_flt_radio_parm.buy_amt;
        rate = sFn_flt_radio_parm.flt_radio2;
        }
        if(sFn_flt_radio_parm.buy_amt1){
        iRet = sql_sum_double("fn_reg","tx_amt",&dSum_amt2,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt,sFn_flt_radio_parm.buy_amt1);
         amt = sFn_flt_radio_parm.buy_amt1;
         rate = sFn_flt_radio_parm.flt_radio21;
        }
        if(sFn_flt_radio_parm.buy_amt2){
        iRet = sql_sum_double("fn_reg","tx_amt",&dSum_amt3,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt1,sFn_flt_radio_parm.buy_amt2);
         amt = sFn_flt_radio_parm.buy_amt2;
         rate = sFn_flt_radio_parm.flt_radio22;
        }
         if(sFn_flt_radio_parm.buy_amt3){
         iRet = sql_sum_double("fn_reg","tx_amt",&dSum_amt4,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt2,sFn_flt_radio_parm.buy_amt3);
         amt = sFn_flt_radio_parm.buy_amt3;
         rate = sFn_flt_radio_parm.flt_radio23;
         } 
         if(sFn_flt_radio_parm.buy_amt4){
         iRet = sql_sum_double("fn_reg","tx_amt",&dSum_amt5,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt3,sFn_flt_radio_parm.buy_amt4);
         amt = sFn_flt_radio_parm.buy_amt4;
         rate = sFn_flt_radio_parm.flt_radio24;
         }
         iRet = sql_sum_double("fn_reg","tx_amt",&dSum_amt6,"prdt_code='%s' and tx_amt >=%lf  and sts!='*'",sFn_parm.prdt_code,amt);


         n =  ( sFn_flt_radio_parm.flt_radio2*dSum_amt2 +sFn_flt_radio_parm.flt_radio21*dSum_amt3 +sFn_flt_radio_parm.flt_radio22*dSum_amt4+sFn_flt_radio_parm.flt_radio23*dSum_amt5 +rate*dSum_amt6)/(dSum_amt1+dSum_amt2+dSum_amt3+dSum_amt4+dSum_amt5+dSum_amt6);
         rate1 = sFn_parm.rate*(1+n);
         vtcp_log("n = %.6lf ",n);
         vtcp_log("rate1 = %.6lf ",rate1);
        }
        /*add end */
        
        /*lpj�޸� ��Ʒ����1���ʽ 2����ʽ2 ��������  1�����̶�����2������������3�Ǳ������� �ͻ����� 0.���˵�λ�Կɣ�1.���ˣ�2.�Թ� ��Ʒ״̬���Ӻ�����ʾ�� �����ѱ��� �йܷѱ��� ֧�зֺ���� ��������ע����*/
        /* �ļ���Ϣ */
        pub_base_dic_show_str(intst_type,"intst_type",sFn_parm.intst_type);
        pub_base_dic_show_str(cif_type,"cif_type",sFn_parm.cif_type);
        pub_base_dic_show_str(fn_parm_sts,"fn_parm_sts",sFn_parm.stat);
        pub_base_dic_show_str(cur_name,"cur_name",sFn_parm.cur_no);
        pub_base_dic_show_str(term_type,"term_type",sFn_parm.term_type);
       /* fprintf(fp,"%s|%s|%s|%s|%ld|%ld|%ld|%ld|%ld|%s|%ld|%s|%s|%s|%s|%s|%.6lf|%.6lf|%s|%.6lf|%.6lf|%.6lf|%.6lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%.6lf|%s|%s|%s|%s|\n",\
                sFn_parm.prdt_code,sFn_parm.title,sFn_parm.prdt_type,\
                sFn_parm.intst_type,sFn_parm.beg_date,sFn_parm.end_date,\
                sFn_parm.ic_date,sFn_parm.mtr_date,sFn_parm.return_date,\
                sFn_parm.term_type,sFn_parm.term,sFn_parm.cif_type,sFn_parm.cur_no,\
                sFn_parm.chnl_type,sFn_parm.risk_lvl,sFn_parm.invest_type,\
                sFn_parm.rate,sFn_parm.fct_rate,sFn_parm.float_rate_flag,\
                sFn_parm.capital_percent,sFn_parm.chrg_rate,sFn_parm.trust_rate,\
                sFn_parm.profit_rate,sFn_parm.min_amt,sFn_parm.sale_amt,sFn_parm.unit_amt,\
                sFn_parm.add_amt,sFn_parm.per_min_amt,sFn_parm.com_min_amt,sFn_parm.cancel_flag,\
                sFn_parm.redempt,sFn_parm.redempt_chrg,sFn_parm.quota_flag,\
                sFn_parm.per_ac_no,sFn_parm.com_ac_no,sFn_parm.stat);*/
         fprintf(fp,"%s|%s|%s|%s|%ld|%ld|%ld|%ld|%ld|%s|%ld|%s|%s|%.6lf|%.6lf|%.6lf|%.6lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%.6lf|%s|%s|%s|\n",\
                sFn_parm.prdt_code,sFn_parm.title,prdt_type,\
                intst_type,sFn_parm.beg_date,sFn_parm.end_date,\
                sFn_parm.ic_date,sFn_parm.mtr_date,sFn_parm.return_date,\
                term_type,sFn_parm.term,cif_type,cur_name,\
                sFn_parm.rate,rate1,sFn_parm.fct_rate,\
                sFn_parm.capital_percent,\
                sFn_parm.min_amt,sFn_parm.sale_amt,\               
                sFn_parm.add_amt,sFn_parm.per_min_amt,sFn_parm.com_min_amt,sFn_parm.cancel_flag,\
                sFn_parm.redempt,sFn_parm.redempt_chrg,\
                sFn_parm.per_ac_no,sFn_parm.com_ac_no,fn_parm_sts);
    }
    Fn_parm_Clo_Sel();
    fclose(fp);
    fp = NULL;
    set_zd_data(DC_FILE_SND,"2");
OkExit:
    vtcp_log("��ѯ��Ʋ�Ʒ�����ɹ�");
    strcpy(g_pub_tx.reply,"0000");
    return 0;
ErrExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    vtcp_log("��ѯ��Ʋ���Ʒ����ʧ��");
    return 1;
}
static int func_0002()
{
    int ret = 0;
    char cBr_no[6];
    char cStat[2];
    long lDate = 0;
    char cPrdt_code[21];
    char cChnl_type[6];
    
    memset(cBr_no,0x00,sizeof(cBr_no));
    memset(cStat,0x00,sizeof(cStat));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cChnl_type,0x00,sizeof(cChnl_type));
    
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0910",cBr_no);
    get_zd_data("0690",cStat);
    get_zd_long("0440",&lDate);
    get_zd_data("0920",cChnl_type);
    /*���ò�ѯļ������ĺ���*/
    vtcp_log("cBr_no[%s] cStat[%s] cChnl_type[%s]",cBr_no,cStat,cChnl_type);
    ret = qury_fn_reg_info(lDate,cBr_no,cPrdt_code,cStat,cChnl_type);
    if(ret)
    {
        vtcp_log("��ѯ��Ʋ�Ʒ���ʧ���� error! ret[%d] ",ret);
        goto ErrExit;
    }
OkExit:
    vtcp_log("��ѯ��Ʒ��Ϣ�ɹ���");
    strcpy(g_pub_tx.reply,"0000");
    return 0;
ErrExit:
    vtcp_log("��ѯ��Ʒ��Ϣʧ����");
    return 1;
}

/*���ݻ�����ѯļ��������������Ĳ����У��ֻ������Լ������� shangyongchao 2013/5/22 21:29:52*/
static int  qury_fn_reg_info(long date,char *br_no,char *prdt_code,char *stat,char *chnl_type)
{
    int iRet = 0;
    int lCnt = 0;
    long lCount = 0;/*������*/
    char cFilename[256];
    char cTerm_type[2];
    struct fn_reg_c sFn_reg_1;/*��ŵ��������� ����*/
    struct fn_reg_c sFn_reg_2;/*������е�����*/
    struct fn_parm_c sFn_parm;
    struct com_branch_c sCom_branch;
    struct fn_br_profit_c sFn_br_profit;
    FILE *fp_info = NULL;
    

    memset(cFilename,0x00,sizeof(cFilename));
    memset(cTerm_type,0x00,sizeof(cTerm_type));
    memset(&sFn_reg_1,0x00,sizeof(sFn_reg_1));
    memset(&sFn_reg_2,0x00,sizeof(sFn_reg_2));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
    vtcp_log("prdt_code[%s] g_pub_tx.tx_br_no[%s] stat[%s]",\
            prdt_code,g_pub_tx.tx_br_no,stat);
    /*������������ ���ܲ�����������Ϣ*/
    if(strcmp(g_pub_tx.tx_br_no,QS_BR_NO) != 0 && strcmp(br_no,g_pub_tx.tx_br_no) != 0)
    {
        vtcp_log("������������ ���ܲ�����������Ϣg_pub_tx.tx_br_no[%s]br_no[%s]");
        strcpy(g_pub_tx.reply,"P075");
        goto ErrExit;
    }
    /*���ļ�*/
    pub_base_AllDwnFilName(cFilename);
    fp_info=fopen(cFilename,"w");
    if( fp_info==NULL )
    {
        vtcp_log("open file [%s] error ",cFilename);
        strcpy(g_pub_tx.reply,"S066");
        goto ErrExit;
    }
    if(strlen(prdt_code) > 0) /*���ܲ�ȫ����Ϣ*/
    {
        iRet=Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",prdt_code);
        if( iRet==100 )
        {
            vtcp_log("fatal error! table no record ");
            strcpy(g_pub_tx.reply,"D104");
            goto ErrExit;
        }
        else if( iRet )
        {
            vtcp_log("database error! ret[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        /*���ݲ�ͬ�����鲻ͬ����*/
        if(strcmp(br_no,QS_BR_NO) == 0)/*�������Ĳ����иò�Ʒ��*/
        {
            /*��д���ļ�ͷ*/
            fp_head(fp_info,sFn_parm);
            /*��ͳ�Ƹ������������Ʒ���Ϲ����*/
            iRet = Com_branch_Dec_Sel(g_pub_tx.reply,"br_type !='0' order by br_no");
            if(iRet)
            {
                vtcp_log("��ѯ���������ret [%d]",iRet);
                strcpy(g_pub_tx.reply,"T087");
                goto ErrExit;
            }
            while(1)
            {
                memset(&sFn_reg_1,0x00,sizeof(sFn_reg_1));
                memset(&sCom_branch,0x00,sizeof(sCom_branch));
                iRet = Com_branch_Fet_Sel(&sCom_branch,g_pub_tx.reply);
                if(iRet != 0 && iRet != 100)
                {
                    vtcp_log("�����α����");
                    strcpy(g_pub_tx.reply,"T087");
                    goto ErrExit;
                }
                else if(iRet == 100)
                {
                   break;
                }
                iRet = my_sel_func(sCom_branch.br_no,stat,prdt_code,chnl_type,&sFn_reg_1);
                if(iRet)
                {
                    vtcp_log("��ѯfn_reg���� ");
                    goto ErrExit;
                }
                iRet = Fn_br_profit_Sel(g_pub_tx.reply,&sFn_br_profit,"prdt_code='%s' and br_no='%s' ",prdt_code,sCom_branch.br_no);
                if(iRet && iRet!=100)
                {
                	sprintf(acErrMsg,"��ѯ������������");
									WRITEMSG
									strcpy(g_pub_tx.reply,"O005");
									goto ErrExit;	
                }
                
                
                fprintf(fp_info,"     %6s%8ld%17.2lf%17.2lf%17.2lf%17.2lf%17.2lf\n",\
                        sCom_branch.br_no,sFn_reg_1.count,sFn_reg_1.tx_amt,sFn_reg_1.redempt_amt,\
                        sFn_reg_1.intst,sFn_reg_1.fct_intst,sFn_br_profit.profit_amt);
                lCnt ++;
                if(lCnt % PAGE_LINE  == 0)
                {
                    fprintf(fp_info,"\f");
                    fp_head(fp_info,sFn_parm);
                }
                sFn_reg_2.count += sFn_reg_1.count;
                sFn_reg_2.tx_amt += sFn_reg_1.tx_amt;
                sFn_reg_2.intst += sFn_reg_1.intst;
                sFn_reg_2.fct_intst += sFn_reg_1.fct_intst;
                sFn_reg_2.profit_amt += sFn_br_profit.profit_amt;/*modify by lwb*/
                sFn_reg_2.redempt_amt += sFn_reg_1.redempt_amt;
	
            }
            Com_branch_Clo_Sel();
            /*ͳ������ �����ۼӵķ�ʽ ����Ϊ�����������������*/
            /*ͳ����������ı���*/

            fprintf(fp_info,"     %6s%8ld%17.2lf%17.2lf%17.2lf%17.2lf%17.2lf\n",\
                    "�ϼ�:",sFn_reg_2.count,sFn_reg_2.tx_amt,sFn_reg_2.redempt_amt,\
                    sFn_reg_2.intst,sFn_reg_2.fct_intst,sFn_reg_2.profit_amt);
        }
        else/*�����鱾�����ò�Ʒ��*/
        {
            /*��д���ļ�ͷ*/
            fp_head(fp_info,sFn_parm);
            iRet = my_sel_func(br_no,stat,prdt_code,chnl_type,&sFn_reg_1);
            if(iRet)
            {
                vtcp_log("��ѯ��ϸ��fn_reg���� ");
                goto ErrExit;
            }
            iRet = Fn_br_profit_Sel(g_pub_tx.reply,&sFn_br_profit,"prdt_code='%s' and br_no='%s' ",prdt_code,br_no);
            if(iRet && iRet!=100)
            {
            	sprintf(acErrMsg,"��ѯ������������");
							WRITEMSG
							strcpy(g_pub_tx.reply,"O005");
							goto ErrExit;	
            }
             sFn_reg_1.profit_amt = sFn_br_profit.profit_amt;
            fprintf(fp_info,"     %6s%8ld%17.2lf%17.2lf%17.2lf%17.2lf%17.2lf\n",\
                br_no,sFn_reg_1.count,sFn_reg_1.tx_amt,sFn_reg_1.redempt_amt,sFn_reg_1.intst,\
                sFn_reg_1.fct_intst,sFn_reg_1.profit_amt);
            
        }
    }
    set_zd_data("0110","2");
OkExit:
    fclose(fp_info);
    fp_info = NULL;
    return 0;

ErrExit:
    if(fp_info != NULL)
    {
        fclose(fp_info);
        fp_info=NULL;
    }
    return 1;
}
static void fp_head(FILE *fp,struct fn_parm_c sFn_parm)
{
    /*��д���ļ�ͷ*/
    fprintf(fp,"\n\n\n\n");
    fprintf(fp,"                                 ��Ʋ�Ʒ[%s]ͳ��(%ld)\n",sFn_parm.prdt_code,g_pub_tx.tx_date);
    fprintf(fp,"    ��Ϣ��:%ld ������:%ld ��������:%ld Ԥ����������:%lf  ʵ����������:%lf\n",
    sFn_parm.ic_date,sFn_parm.mtr_date,sFn_parm.return_date,sFn_parm.rate,sFn_parm.fct_rate);
    fprintf(fp,"----------------------------------------------------------------------------------------------------------\n");
    fprintf(fp,"    %6s %8s%17s%17s %17s %17s%17s\n","������","����","�Ϲ��ܶ�","����ܶ�","�ͻ���Ԥ������","�ͻ���ʵ������","֧������");
    fprintf(fp,"----------------------------------------------------------------------------------------------------------\n");
}
/*����������ѯ fn_reg  shangyongchao 2013/5/22 21:29:22*/
static int my_sel_func(char *br_no,char *stat,char *prdt_code,char *chnl_type,struct fn_reg_c *fn_reg)
{
    int iRet = 0;
    struct fn_reg_c sFn_reg;
    memset(&sFn_reg,0x0,sizeof(sFn_reg));
    
    /*���ݲ�ͬ��״̬�鲻ͬ�Ĳ�ѯ����*/
		if(*stat == 'A' && *chnl_type == 'A')   /*add by lwb 20140821  ����״̬Ϊȫ��������*/
		{
			iRet = Fn_reg_Dec_Sel(g_pub_tx.reply,"tx_br_no='%s' and prdt_code='%s' ",\
            br_no,prdt_code);
		}
		else if(*stat == 'A' && *chnl_type != 'A')
		{
			iRet = Fn_reg_Dec_Sel(g_pub_tx.reply,"tx_br_no='%s' and prdt_code='%s' and chnl_type = '%s'",\
            br_no,prdt_code,chnl_type);
		}
		else if(*stat != 'A' && *chnl_type == 'A')
		{
			iRet = Fn_reg_Dec_Sel(g_pub_tx.reply,"tx_br_no='%s' and prdt_code='%s' and sts = '%s'",\
            br_no,prdt_code,stat);
		}
		else
		{
    iRet = Fn_reg_Dec_Sel(g_pub_tx.reply,"tx_br_no='%s' and prdt_code='%s' and sts = '%s' and chnl_type='%s'",\
            br_no,prdt_code,stat,chnl_type);
    }
    if(iRet)
    {
        vtcp_log("�����ѯ�α���� iRet[%d] ",iRet);
        strcpy(g_pub_tx.reply,"D330");
        return 1;
    }
    while(1)
    {
        memset(&sFn_reg,0x00,sizeof(sFn_reg));
        iRet = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("ȡ��ѯ�α���� iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D330");
            return 1;
        }
        else if(iRet == 100)
        {
            break;
        }
        /*���������� ָ�� �ۼӽ��*/
        fn_reg->tx_amt += sFn_reg.tx_amt;
        fn_reg->redempt_amt += sFn_reg.redempt_amt;
        fn_reg->return_capital += sFn_reg.return_capital;
        fn_reg->intst += sFn_reg.intst;
        fn_reg->fct_intst += sFn_reg.fct_intst;
        /*
        fn_reg->profit_amt += sFn_reg.profit_amt;
        */
        fn_reg->count ++;/*ͳ�Ʊ���*/
    }
    Fn_reg_Clo_Sel();
    return 0;
}

static int func_0003()
{
    int ret = 0;
    long lCnt = 0;
    char cPrdt_code[25];
    char cFlag[2];/*��ѯ��־*/
    char cSts[12];
    char cBr_no[6];
    long lDate1 = 0;
    long lDate2 = 0;
    double dAmt1 = 0.00;
    double dAmt2 = 0.00;
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;
    FILE *fp  = NULL;
    char cFilename[256];
    char cWherelist[512];
    char cTmp_str[512];
    char cTerm_type[3];
    char cCif_name[60];
    char cId_no[20];
    char cChnl_type[4];
    char cKhlx[2];/*�ͻ����͹����ٸ�*/
    
    long g_count = 0;
    double g_tx_amt = 0.00;
    double g_redempt_amt = 0.00;
    double g_intst = 0.00;
    double g_fct_intst = 0.00;
    double g_return_capital = 0.00;
    
    memset(cFilename,0x00,sizeof(cFilename));
    memset(cWherelist,0x00,sizeof(cWherelist));
    memset(cSts,0x00,sizeof(cSts));
    memset(cBr_no,0x00,sizeof(cBr_no));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(cCif_name,0x00,sizeof(cCif_name));
    memset(cId_no,0x00,sizeof(cId_no));
    memset(cChnl_type,0x00,sizeof(cChnl_type));
    memset(cKhlx,0x00,sizeof(cKhlx));/*�ͻ����͹����ٸ�*/
    
    get_zd_data("0910",cBr_no);
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0690",cFlag);
    get_zd_long("0440",&lDate1);
    get_zd_long("0450",&lDate2);
    get_zd_double("0390",&dAmt1);
    get_zd_double("0400",&dAmt2);
    get_zd_data("0920",cChnl_type);
     get_zd_data("0710",cKhlx);/*�ͻ����͹����ٸ�*/
    vtcp_log("cKhlx=%s",cKhlx);
    vtcp_log("cKhlx[%s] cBr_no[%s] cPrdt_code[%s] cFlag[%s] date1[%ld] date2[%ld] cChnl_type[%s] amt1[%lf] amt2[%lf]",\
            cKhlx,cBr_no,cPrdt_code,cFlag,lDate1,lDate2,cChnl_type,dAmt1,dAmt2);/*�ͻ����͹����ٸ�*/
        /*������������ ���ܲ�����������Ϣ*/
    if(strcmp(g_pub_tx.tx_br_no,QS_BR_NO) != 0 && strcmp(cBr_no,g_pub_tx.tx_br_no) != 0)
    {
        vtcp_log("������������ ���ܲ�����������Ϣg_pub_tx.tx_br_no[%s]br_no[%s]",g_pub_tx.tx_br_no,cBr_no);
        strcpy(g_pub_tx.reply,"P288");
        goto ErrExit;
    }
    
    pub_base_AllDwnFilName(cFilename);
    fp=fopen(cFilename,"w");
    if(fp == NULL)
    {
        vtcp_log("���ļ�ʧ��[%s]",cFilename);
        strcpy(g_pub_tx.reply,"S066");
        goto ErrExit;
    }
    /*д������ lpj20140415�޸Ľ� ״̬��ʾΪ��������,�Ϲ�����������������־ ��ע��)  �������֤������ */
   /*fprintf(fp,"~��������|������ˮ��|���׻���|Э����|��Ʒ����|�Ϲ�����|�Ϲ����|��ؽ��|���ر���|�˺�|������������|Ԥ������|��Ϣ����|��������|����|������|�����־|����ժҪ|״̬|�ۿ�����|�ۿ���ˮ��|������������|����������ˮ��|ǩԼ��Ա|�ͻ������|������֤������|������֤��|����������|\n");
    */
    fprintf(fp,"~��������|������ˮ��|���׻���|Э����|��Ʒ����|�˺�|���֤|����|�Ϲ����|��ؽ��|���ر���|������������|Ԥ������|��Ϣ����|��������|����|����ժҪ|״̬|�ۿ�����|�ۿ���ˮ��|������������|����������ˮ��|ǩԼ��Ա|�ͻ������|������֤������|������֤��|����������|\n");
    if(strlen(cPrdt_code)>0)
    {
      memset(cTmp_str ,0x00,sizeof(cTmp_str));
      sprintf(cTmp_str,"prdt_code ='%s' and ",cPrdt_code);
       strcat(cWherelist,cTmp_str);
    }
    if(strlen(cFlag) > 0)
    {
        memset(cTmp_str ,0x00,sizeof(cTmp_str));
        sprintf(cTmp_str,"  sts= '%s'  and ",cFlag);
        strcat(cWherelist,cTmp_str);
    }
    if(strlen(cKhlx) > 0)/*�ͻ����͹����ٸ�*/
    {
        memset(cTmp_str ,0x00,sizeof(cTmp_str));
        sprintf(cTmp_str," cif_type= '%s'  and ",cKhlx);
        strcat(cWherelist,cTmp_str);
    }
    if(lDate1)
    {
        memset(cTmp_str,0x00,sizeof(cTmp_str));
        sprintf(cTmp_str," tx_date >= %ld and ",lDate1);
        strcat(cWherelist,cTmp_str);
    }
    if(lDate2)
    {
        memset(cTmp_str,0x00,sizeof(cTmp_str));
        sprintf(cTmp_str,"  tx_date <= %ld and ",lDate2);
        strcat(cWherelist,cTmp_str);
    }
    if(strlen(cChnl_type) >0)
    {
    		memset(cTmp_str ,0x00,sizeof(cTmp_str));
        sprintf(cTmp_str,"  chnl_type= '%s'  and ",cChnl_type);
        strcat(cWherelist,cTmp_str);
    }
    if(dAmt1 >0.005)
    {
        memset(cTmp_str,0x00,sizeof(cTmp_str));
        sprintf(cTmp_str,"  tx_amt >= %.2lf and ",dAmt1);
        strcat(cWherelist,cTmp_str);
    }
    if(dAmt2 >0.005)
    {
        memset(cTmp_str,0x00,sizeof(cTmp_str));
        sprintf(cTmp_str,"  tx_amt <= %.2lf  and ",dAmt2);
        strcat(cWherelist,cTmp_str);
    }
    if(strcmp(cBr_no,QS_BR_NO) != 0)/*�����������ĵ��齻�׻���*/
    {
        memset(cTmp_str,0x00,sizeof(cTmp_str));
        sprintf(cTmp_str,"  tx_br_no ='%s' and ",cBr_no);
        strcat(cWherelist,cTmp_str);
    }
    strcat(cWherelist,"1=1 order by tx_date,trace_no");
    /*ѭ��ȡ��¼*/
    ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
    if(ret)
    {
        vtcp_log("������ƵǼǲ��α����");
        strcpy(g_pub_tx.reply,"D330");
        goto ErrExit;
    }
    while(1)
    {
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        memset(&sFn_reg,0x00,sizeof(sFn_reg));
        memset(cSts,0x00,sizeof(cSts));
        memset(cTerm_type,0x00,sizeof(cTerm_type));
        memset(cChnl_type,0x00,sizeof(cChnl_type));
        memset(cId_no,0x00,sizeof(cId_no));
        memset(cCif_name,0x00,sizeof(cCif_name));
        ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
        if(ret == 100)
        {
            break;
        }
        else if(ret != 100 && ret != 0)
        {
            vtcp_log("ȡ��ƵǼǲ��α����");
            strcpy(g_pub_tx.reply,"D330");
            goto ErrExit;
        }
        lCnt ++;
        ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
        if(ret)
        {
            vtcp_log("��ѯ��Ʋ�Ʒ��������");
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        
        pub_base_dic_show_str(cTerm_type,"term_type",sFn_parm.term_type);
        
        if(strcmp(cTerm_type,"��") == 0)
        {
            strcpy(cTerm_type,"��");
        }
        vtcp_log("%s,%d  sFn_reg.sts = %s",__FILE__,__LINE__,sFn_reg.sts);
        pub_base_dic_show_str(cSts,"fn_reg_sts",sFn_reg.sts); 
        vtcp_log("%s,%d  cSts = %s",__FILE__,__LINE__,cSts);
        
        pub_base_dic_show_str(cChnl_type,"chnl_type",sFn_reg.chnl_type);
        vtcp_log("%s,%d  cChnl_type = %s",__FILE__,__LINE__,cChnl_type);
        
        /*������ѯ�ͻ����������֤lpj20140416  */
        ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",sFn_reg.ac_no);
        if(ret == 100)
        {
        		vtcp_log("�������˺ű����޴�����--%s----%d",__FILE__,__LINE__);
        		strcpy(g_pub_tx.reply,"B117");
        		goto ErrExit;
        
        }
        else if(ret)
        {
        		vtcp_log("��ȡ�����˺Ŷ��ձ�ʧ��,�������Ļ�����ϵ!");
            strcpy(g_pub_tx.reply,"C114");
          	goto ErrExit;	
        }    
        vtcp_log("���֤id_no=%s",sMdm_ac_rel.id_no);   
        vtcp_log("�������id_type=%c",sMdm_ac_rel.id_type[0]);  
        if(sMdm_ac_rel.id_type[0]=='1')
        {
        		strcpy(cId_no,sMdm_ac_rel.id_no);	
        		vtcp_log("���֤Ϊ��%s",cId_no);
        }
        		ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id='%d'",sMdm_ac_rel.ac_id);
        		if(ret==100)
        		{
        				vtcp_log("���ڴ�����ļ��в����ڴ��˻���Ϣ");
            		strcpy(g_pub_tx.reply,"B114");
          			goto ErrExit;		
        		}
        		else if(ret)
        	  {
        	  		vtcp_log("��ѯ���ڲ�Ʒ���ļ�����!");
            		strcpy(g_pub_tx.reply,"W015");
          			goto ErrExit;	
        	  }
        		strcpy(cCif_name,sDd_mst.name);
        		vtcp_log("�ͻ�����Ϊ��%s",cCif_name);
        
        
        /*���ݲ�ͬ��־д��ͬ���ļ�*/
       /* fprintf(fp,"%ld|%9ld|%s|%20s|%20s|%9ld|%16.2f|%16.2f|%16.2f|%9.6f|%9.6f|%16.2f|%ld|%ld|%16.2lf|%16.2f|%s|%s|%12s|%ld|%9ld|%ld|%9ld|%s|%s|%s|%s|%s|\n",\
        sFn_reg.tx_date,sFn_reg.trace_no,sFn_reg.tx_br_no,sFn_reg.pact_no,\
        sFn_reg.prdt_code,sFn_reg.count,sFn_reg.tx_amt,sFn_reg.redempt_amt,\
        sFn_reg.return_capital,sFn_reg.flt_radio,sFn_reg.return_rate,sFn_reg.intst,\
        sFn_reg.ic_date,sFn_reg.end_date,sFn_reg.fct_intst,sFn_reg.profit_amt,\
        sFn_reg.profit_flag[0] == 'Y' ?"��":"��",sFn_reg.brf,cSts,sFn_reg.pay_date,\
        sFn_reg.pay_trace_no,sFn_reg.return_date,sFn_reg.return_trace_no,sFn_reg.tel,\
        sFn_reg.mang,sFn_reg.agent_id_type,sFn_reg.agent_id,sFn_reg.agent_name);*/
        fprintf(fp,"%ld|%9ld|%s|%20s|%20s|%s|%20s|%60s|%16.2f|%16.2f|%16.2f|%9.6f|%16.2f|%ld|%ld|%16.2lf|%s|%12s|%ld|%9ld|%ld|%9ld|%s|%s|%s|%s|%s|\n",\
		    sFn_reg.tx_date,sFn_reg.trace_no,sFn_reg.tx_br_no,sFn_reg.pact_no,\
        sFn_reg.prdt_code,sFn_reg.ac_no,cId_no,cCif_name,sFn_reg.tx_amt,sFn_reg.redempt_amt,\
        sFn_reg.return_capital,sFn_reg.return_rate,sFn_reg.intst,\
        sFn_reg.ic_date,sFn_reg.end_date,sFn_reg.fct_intst,\
        sFn_reg.brf,cSts,sFn_reg.pay_date,\
        sFn_reg.pay_trace_no,sFn_reg.return_date,sFn_reg.return_trace_no,sFn_reg.tel,\
        sFn_reg.mang,sFn_reg.agent_id_type,sFn_reg.agent_id,sFn_reg.agent_name);

        g_count ++;
        g_tx_amt+=sFn_reg.tx_amt;
        g_redempt_amt += sFn_reg.redempt_amt;
        g_return_capital += sFn_reg.return_capital;
        g_intst += sFn_reg.intst;
        g_fct_intst += sFn_reg.fct_intst;
    }
    fprintf(fp,"�ܼ�   |�ܱ���     %ld|  ���Ϲ����   %16.2f |����ؽ��  %16.2f  |�ܷ�������  %16.2f | ��Ԥ������  %16.2f |��ʵ������  %16.2f",\
           g_count,g_tx_amt,g_redempt_amt,g_return_capital,g_intst,g_fct_intst);
    Fn_reg_Clo_Sel();
    if(lCnt == 0)
    {
        vtcp_log("û�з��������ļ�¼");
        strcpy(g_pub_tx.reply,"D104");
        goto ErrExit;
    }
    set_zd_data(DC_FILE_SND,"2");
OkExit:
    fclose(fp);
    fp = NULL;
    strcpy(g_pub_tx.reply,"0000");
    return 0;
ErrExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    vtcp_log("��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣʧ��[%s]", g_pub_tx.reply);
    return 1;
}
/*�ͻ��Ϲ���Ϣ*/
static int func_0004()
{
    int ret = 0;
    char cAc_no[25];
    char cId_type[2];
    char cId_no[21];
    FILE *fp  = NULL;
    char cFilename[256];
    char cWherelist[256];
    struct mdm_ac_rel_c sMdm_ac_rel;
    
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(cWherelist,0x00,sizeof(cWherelist));
    memset(cId_type,0x00,sizeof(cId_type));
    memset(cId_no,0x00,sizeof(cId_no));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    get_zd_data("0300",cAc_no);
    get_zd_data("0670",cId_type);
    get_zd_data("0620",cId_no);
    pub_base_AllDwnFilName(cFilename);
    fp=fopen(cFilename,"w");
    if(fp == NULL)
    {
        vtcp_log("���ļ�ʧ��[%s]",cFilename);
        strcpy(g_pub_tx.reply,"S066");
        goto ErrExit;
    }
    /*д������*/
    fprintf(fp,"~�˺�|��������|������ˮ��|���׻���|Э����|��Ʒ����|�Ϲ�����|�Ϲ����|��ؽ��|���ر���|���ʸ�������|������������|Ԥ������|��Ϣ����|��������|����|������|�����־|����ժҪ|״̬|�ۿ�����|�ۿ���ˮ��|������������|����������ˮ��|ǩԼ��Ա|�ͻ������|������֤������|������֤��|����������|\n");
    /*������� ֤���� ����֤���� �������Ŀ����˺� ��ȥ���Ϲ���ϸ*/
    if(strlen(cId_no) == 0)
    {
        sprintf(cWherelist,"ac_no ='%s'",cAc_no);
        ret = my_sel_fnreg(fp,cWherelist);
        if(ret)
        {
            strcpy(g_pub_tx.reply,"P084");
            vtcp_log("�����ѯ�����˺Ź�ϵ����α����");
            goto ErrExit;
        }
    }
    else
    {
        /*ѭ��ȡ�������Ŀ����˺�*/
        if(strcmp(g_pub_tx.tx_br_no,QS_BR_NO) == 0)
        {
            ret = Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"id_type='%s' and id_no='%s'",\
                    cId_type,cId_no);
        }
        else
        {
            ret = Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"opn_br_no ='%s' and id_type='%s' and id_no='%s'",\
                    g_pub_tx.tx_br_no,cId_type,cId_no);
        }
        if(ret)
        {
            strcpy(g_pub_tx.reply,"P084");
            vtcp_log("�����ѯ�����˺Ź�ϵ����α����[%d]",ret);
            goto ErrExit;
        }
        while(1)
        {
            memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
            ret = Mdm_ac_rel_Fet_Sel(&sMdm_ac_rel,g_pub_tx.reply);
            if(ret != 0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"P088");
                vtcp_log("�����ѯ�����˺Ź�ϵ����α����[%d]",ret);
                goto ErrExit;
            }
            else if(ret == 100)
            {
                break;
            }
            sprintf(cWherelist,"ac_no ='%s'",sMdm_ac_rel.ac_no);
            ret = my_sel_fnreg(fp,cWherelist);
            if(ret)
            {
                strcpy(g_pub_tx.reply,"P084");
                vtcp_log("�����ѯ�����˺Ź�ϵ����α����");
                goto ErrExit;
            }
        }
        Mdm_ac_rel_Clo_Sel();
    }

    set_zd_data(DC_FILE_SND,"2");
OkExit:
    fclose(fp);
    fp = NULL;
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣ�ɹ�[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    vtcp_log("��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣʧ��[%s]", g_pub_tx.reply);
    return 1;
}
static int my_sel_fnreg(FILE *fp,char * cWherelist)
{
    int ret = 0;
    char cSts[12];
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    char cTerm_type[3];
    
    memset(cSts,0x00,sizeof(cSts));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
     /*ѭ��ȡ��¼������������������*/
    ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
    if(ret)
    {
        vtcp_log("������ƵǼǲ��α����");
        strcpy(g_pub_tx.reply,"D330");
        goto ErrExit;
    }
    while(1)
    {
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        memset(&sFn_reg,0x00,sizeof(sFn_reg));
        memset(cSts,0x00,sizeof(cSts));
        memset(cTerm_type,0x00,sizeof(cTerm_type));
        
        ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
        if(ret == 100)
        {
            break;
        }
        else if(ret != 100 && ret != 0)
        {
            vtcp_log("ȡ��ƵǼǲ��α����");
            strcpy(g_pub_tx.reply,"D330");
            goto ErrExit;
        }
        ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
        if(ret)
        {
            vtcp_log("��ѯ��Ʋ�Ʒ��������");
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        pub_base_dic_show_str(cTerm_type,"term_type",sFn_parm.term_type);
        if(strcmp(cTerm_type,"��") == 0)
        {
            strcpy(cTerm_type,"��");
        }
        pub_base_dic_show_str(cSts,"fn_reg_sts",sFn_reg.sts);
        fprintf(fp,"%s|%ld|%9ld|%s|%20s|%20s|%9ld|%16.2f|%16.2f|%16.2f|%9.6f|%9.6f|%16.2f|%ld|%ld|%16.2lf|%16.2f|%s|%s|%12s|%ld|%9ld|%ld|%9ld|%s|%s|%s|%s|%s|\n",\
        sFn_reg.ac_no,sFn_reg.tx_date,sFn_reg.trace_no,sFn_reg.tx_br_no,sFn_reg.pact_no,\
        sFn_reg.prdt_code,sFn_reg.count,sFn_reg.tx_amt,sFn_reg.redempt_amt,\
        sFn_reg.return_capital,sFn_reg.flt_radio,sFn_reg.return_rate,sFn_reg.intst,\
        sFn_reg.ic_date,sFn_reg.end_date,sFn_reg.fct_intst,sFn_reg.profit_amt,\
        sFn_reg.profit_flag[0] == 'Y' ?"��":"��",sFn_reg.brf,cSts,sFn_reg.pay_date,\
        sFn_reg.pay_trace_no,sFn_reg.return_date,sFn_reg.return_trace_no,sFn_reg.tel,\
        sFn_reg.mang,sFn_reg.agent_id_type,sFn_reg.agent_id,sFn_reg.agent_name);
    }
    Fn_reg_Clo_Sel();
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    return 0;
ErrExit:
    vtcp_log("��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣʧ��[%s]", g_pub_tx.reply);
    return 1;
}


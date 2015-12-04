/***************************************************************
* �� �� ��:     spJ090.c
* ����������    С����ʷ��Ϣ��ѯ
*
* �޸ļ�¼��    �������еĶ�������
* ��    ��:     20060921
* �� �� ��:     liuyue
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "lv_pkgreg_c.h"
#include "lv_define.h"
#include "find_debug.h"
#include "lv_param_c.h"  /**NEWBP4**/

char * iNum2TxType(char *ch,char *cTmpList);
char * iNum2Stat(char ch,char *cTmpList);
char * iNum2RcpStat(char *rcpstat,char *cTmpList ,char *pkgno);

spJ090()
{
    int ret = 0;
    HV_NOTPAY_AREA hv_notpayin;
    HV_FD123_AREA  fd123;
    struct lv_pkgreg_c  lv_pkgreg;
    struct hv_fzf_c hv_fzf;

    char    cLw_ind[2];      /*������־ 1 ��  2 ��*/
    char    cBr_no[6];        /*������*/
    char    cTx_date[9];      /*��������*/
    char    cZf_type[2];      /*֧����־ 0 ֧����  1 ��֧���� */
    char    cTx_type[4];      /*֧����������*/
    char    cTx_stat[2];       /*����״̬*/
    int     ttlnum = 0;       /*��ȡ��������*/
    FILE    *fp;
    char    cFilename[100];    /*�ļ���*/
    char    cWherelist[2000];
    char    cTmplist[200];
    int      iTx_date = 0;
    int        iBr_no     = 0;
    char    cPack_date[9];
    char    cStat[21];
    char     cRcp_stat[21];
    int     iCmtno;

    double Tamt=0.00;
    char     cTx_Desc[20];

    memset(cLw_ind,        0x00 ,    sizeof(cLw_ind));
    memset(cBr_no,        0x00 ,    sizeof(cBr_no ));
    memset(cTx_date,    0x00 ,    sizeof(cTx_date));
    memset(cZf_type,    0x00 ,    sizeof(cZf_type));
    memset(cTx_type,    0x00 ,    sizeof(cTx_type));
    memset(cTx_stat,    0x00 ,    sizeof(cTx_stat ));
    memset(cFilename,    0x00 ,    sizeof(cFilename));
    memset(&fd123,        0x00 ,    sizeof(fd123));
    memset(&lv_pkgreg,    0x00 ,    sizeof(struct lv_pkgreg_c));
    memset(&hv_fzf,        0x00 ,    sizeof(struct hv_fzf_c));
    memset(&hv_notpayin,0x00 ,    sizeof(hv_notpayin));
    memset(cWherelist,    0x00 ,    sizeof(cWherelist));
    memset(cTmplist,    0x00 ,    sizeof(cTmplist));
    memset(cPack_date,    0x00 ,    sizeof(cPack_date));
    memset(cStat,        0x00 ,    sizeof(cStat));
    memset(cRcp_stat,    0x00 ,    sizeof(cRcp_stat));
    memset(cTx_Desc,    0x00 ,    sizeof(cTx_Desc));

    /*** ��ʼ��ȫ�ֱ��� ***/
    pub_base_sysinit();

    get_zd_data("0670",cLw_ind);         /*������־*/     
    get_zd_data("0280",cBr_no);             /*��������*/
    get_zd_data("0440",cTx_date);         /*��������*/
    get_zd_data("0680",cZf_type);         /*֧������*/
    get_zd_data("0230",cTx_type);         /*CMT��� */
    get_zd_data("0690",cTx_stat);         /*����״̬*/

    vtcp_log("[%s][%d]������==[%s]\n",__FILE__,__LINE__,cBr_no);
    vtcp_log("[%s][%d]������־==[%s]\n",__FILE__,__LINE__,cLw_ind);
    vtcp_log("[%s][%d]��������==[%s]\n",__FILE__,__LINE__,cTx_type);
    vtcp_log("[%s][%d]��¼״̬==[%s]\n",__FILE__,__LINE__,cTx_stat);
    vtcp_log("[%s][%d]֧������==[%s]\n",__FILE__,__LINE__,cZf_type);
    vtcp_log("[%s][%d]��ѯ����==[%s]\n",__FILE__,__LINE__,cTx_date);

    vtcp_log("[%s][%d]��ʼС��Ĳ�ѯ����!\n",__FILE__,__LINE__);

    pub_base_AllDwnFilName(cFilename);
    vtcp_log("[%s][%d]cFilename===[%s]\n",__FILE__,__LINE__,cFilename);
    fp = fopen(cFilename,"rb");
    if(fp==NULL)
    {
        sprintf(acErrMsg,"���ļ�[%s]����!\n",cFilename);
        WRITEMSG
        strcpy(g_pub_tx.reply,"HV01");
    }
    fclose(fp);


    /********�������Ŀ��Բ�ѯ���еĽ���*****/

    /**��ɲ�ѯ����**/

    /**********************
     *�����׻�����      *
     *������Բ�ȫ��      *
     **********************/
    if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5))  /****���������������**/
    {
        zip_space(cBr_no);
        iBr_no    = atoi(cBr_no);
        if(iBr_no==0)
        {
            vtcp_log("[%s][%d]����������,�����뱾������!\n",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,"����������,�����뱾������!");
            strcpy(g_pub_tx.reply,"HV01");
            goto ErrExit;    
        }

        if(memcmp(g_pub_tx.tx_br_no,cBr_no,sizeof(cBr_no)-1))
        {
            vtcp_log("[%s][%d]���������Ĳ������ѯ����ҵ��!\n",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,"���������Ĳ��ܲ�ѯ���е�ҵ��!");
            strcpy(g_pub_tx.reply,"HV01");
            goto ErrExit;
        }
        sprintf(cTmplist,"br_no = '%s' and ",cBr_no);
        strcat(cWherelist,cTmplist);
    }
    else
    {
        zip_space(cBr_no);
        iBr_no = atoi(cBr_no);
        if(iBr_no!=0&&strlen(cBr_no)==BRNO_LEN)
        {
            sprintf(cTmplist,"br_no = '%s' and ",cBr_no);
            strcat(cWherelist,cTmplist);
        }
    }

    /*****************
     * ��������      *
     *������������� *
     *****************/

    zip_space(cTx_date);
    iTx_date = atoi(cTx_date);

    if(iTx_date!=0 )
    {
        if(cZf_type[0]=='0')
        /**
                sprintf(cTmplist,"(in_date='%s' or wt_date= '%s') and ",cTx_date,cTx_date);
        **/
                sprintf(cTmplist,"in_date= '%s' and ",cTx_date);
        else if(cZf_type[0]=='1')
                sprintf(cTmplist," tx_date= '%s' and  ",cTx_date);

        strcat(cWherelist,cTmplist);
    }


    /****************************************************
     *�����ı��  ��֧��Ӧ�����ر�                     *
     *ע�� :  �����ѡ��ȫ����������ʱ���֧��Ӧ����    *
     *        ������,��Ȼ����ֱ�ӽ���������302 320 �� *
     *        ֱ�Ӵ�����Դ���,�����ڴβ�������         *
     ****************************************************/


    if(memcmp(cTx_type,"888", sizeof(cTx_type)-1))/*****���ѡ��Ĳ���ȫ��***/
    {
        if(cZf_type[0]=='1')
        {
                /**����Ӧ����һ������������ͬ��¼,�������������ҳ�Ӧ���ཻ��*****/
                iCmtno =  apatoi(cTx_type,sizeof(cTx_type)-1);
                if(iCmtno%2==0)/***�����Ӧ����ķ�֧������***/
                {
                    sprintf(cTmplist,"( cmtno = '%s' or  ( cmtno = '%d' and res_sts  is not null) )  and ",cTx_type,iCmtno-1);
                }
                else 
                    sprintf(cTmplist,"cmtno = '%s' and ",cTx_type);
         }
        else if(cZf_type[0]=='0')
                sprintf(cTmplist,"pkgno = '%s' and ",cTx_type);

        strcat(cWherelist,cTmplist);
    }

    /*******************
     * �����¼״̬    *
     * ע��ƥ��ȫ��    *
     *******************/

    if(cTx_stat[0]!='E'&&cZf_type[0]=='1')/***��֧����ȫ��**/
    {
        sprintf(cTmplist,"hv_fzf_sts ='%c'  and  ",cTx_stat[0]);
        strcat(cWherelist,cTmplist);
    }
    else if(cTx_stat[0]!='E'&&cZf_type[0]=='0') /**֧����*/
    {
        sprintf(cTmplist,"lv_sts ='%c' and ",cTx_stat[0]);
        strcat(cWherelist,cTmplist);
    }



    /**************************
     *����Ƿ�֧��,���ִ�С�� *
     **************************/

    if(cZf_type[0]=='1')
    {
            sprintf(cTmplist,"otype = '%c' and ",'2');
            strcat(cWherelist,cTmplist);
    }


    /***************************
     *����������־             *
     *Ӧ�������ʲ���ı��־   *
     ***************************/
    iCmtno =  apatoi(cTx_type,sizeof(cTx_type)-1);
    if(iCmtno==888)
    {
            sprintf(cTmplist," lw_ind ='%c' ",cLw_ind[0]);
    }
    else
    {
            if(cZf_type[0]=='0')
            {
                    sprintf(cTmplist," lw_ind ='%c' ",cLw_ind[0]);
            }
            else if(cZf_type[0]=='1')/*ֻ�з�֧��ʱ����Ҫ�����***/
            {
                if(iCmtno%2==1)/*������ֱ��ȥ�Ϳ�����**/
                    sprintf(cTmplist," lw_ind ='%c' ",cLw_ind[0]);
                else if(iCmtno%2==0)
                    sprintf(cTmplist," lw_ind ='%d' ",1+(cLw_ind[0]-'0')%2);
                    
            }
    }

    strcat(cWherelist,cTmplist);

    strcat(cWherelist," and 1=1 order by br_no,orderno");
    vtcp_log("[%s][%d]cWherelist=[%s]\n",__FILE__,__LINE__,cWherelist);

    vtcp_log("[%s][%d]֧������===[%c]\n",__FILE__,__LINE__,cZf_type[0]);
    /**��ʼ��ѯ**/
    /***֧��������Ĳ�ѯ***/
    if(cZf_type[0] == '0')
    {
        vtcp_log("[%s][%d]��ѯ֧���ཻ��!\n",__FILE__,__LINE__);
        ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, cWherelist);
        if(ret)
        {
            sprintf(acErrMsg,"��ѯС��֧�����׻�����Ϣ����[%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"HV01");
            goto ErrExit;
        }
        ttlnum=0;
        Tamt=0.00;
        while(1)
        {
            memset(cStat,        0x00 ,    sizeof(cStat));
            memset(cRcp_stat,    0x00 ,    sizeof(cRcp_stat));
            memset(cTx_Desc,    0x00 ,    sizeof(cTx_Desc));

            ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
            if( ret==100 &&ttlnum>0 )
            {
                break;
            }
            else if(ret==100 &&ttlnum==0)
            {
                vtcp_log("[%s][%d]û���ҵ���Ӧ�ļ�¼!\n",__FILE__,__LINE__);
                set_zd_data(DC_GET_MSG,"�Ҳ�����Ӧ�ļ�¼!\n");
                strcpy(g_pub_tx.reply,"D104");
                goto ErrExit;
            }
            else if(ret && ret!=100)
            {
                vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!\n",__FILE__,__LINE__);
                set_zd_data(DC_GET_MSG,"���ݿ�����쳣,����Ƽ�����ϵ!\n");
                strcpy(g_pub_tx.reply,"HV01");
                goto ErrExit;
            }
            if( !ttlnum )
            {
                pub_base_AllDwnFilName(cFilename);
                fp = fopen(cFilename,"w" );
                if( fp==NULL )
                {
                    sprintf(acErrMsg," open file [%s] error ",cFilename);
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"S047" );
                    goto ErrExit;
                }
                fprintf( fp,"~PKG���|��������|������|˳���|��������|���׽��|������|������|�����  |������  |����״̬|��ִ״̬|\n" );
            }
            if(lv_pkgreg.pack_date==0)
                strcpy(cPack_date,"        ");
            else
                sprintf(cPack_date,"%08d",lv_pkgreg.pack_date);


            iNum2TxType(lv_pkgreg.pkgno,cTx_Desc);
            iNum2Stat(lv_pkgreg.lv_sts[0],cStat);
            iNum2RcpStat(lv_pkgreg.rcpstat,cRcp_stat,lv_pkgreg.pkgno);


            fprintf( fp,"%.3s|%s|%s|%s|%ld|%.2f|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.pkgno,cTx_Desc,lv_pkgreg.br_no,lv_pkgreg.orderno,lv_pkgreg.wt_date,lv_pkgreg.tx_amt,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.packid,cPack_date,cStat,cRcp_stat);
            ttlnum++;
            Tamt += lv_pkgreg.tx_amt; 
            memset(cTx_Desc,0x00,sizeof(cTx_Desc));

        }      
        fprintf( fp,"����:|%d��|����ܼ�:|%.2f|\n",ttlnum,Tamt);
        ret=Lv_pkgreg_Clo_Sel();
        if( !ttlnum )
        {
            strcpy( g_pub_tx.reply,"S049" );
            goto ErrExit;
        }
        else
        {
            fclose(fp);
            set_zd_data( DC_FILE_SND,"1");
        }
    }
    else if(cZf_type[0]=='1') /***��֧���ཻ�ײ�ѯ***/
    {
        ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, cWherelist);
        if(ret)
        {
            sprintf(acErrMsg,"��ѯС���֧�����׻�����Ϣ����[%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        ttlnum=0;
        while(1)
        {
            memset(cStat,        0x00 ,    sizeof(cStat));
            memset(cRcp_stat,    0x00 ,    sizeof(cRcp_stat));
            memset(cTx_Desc,    0x00 ,    sizeof(cTx_Desc));


            ret=Hv_fzf_Fet_Sel(&hv_fzf,g_pub_tx.reply);
            if( ret==100 &&ttlnum>0 )
            {
                break;
            }
            else if(ret==100 &&ttlnum==0)
            {
                vtcp_log("[%s][%d]û���ҵ���Ӧ�ļ�¼!\n",__FILE__,__LINE__);
                goto ErrExit;
            }
            else if(ret && ret!=100)
            {
                vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!\n",__FILE__,__LINE__);
                set_zd_data(DC_GET_MSG,"���ݿ�����쳣,����Ƽ�����ϵ!\n");
                goto ErrExit;
            }
            if( !ttlnum )
            {
                pub_base_AllDwnFilName(cFilename);
                fp = fopen(cFilename,"w" );
                if( fp==NULL )
                {
                    sprintf(acErrMsg," open file [%s] error ",cFilename);
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"S047" );
                    goto ErrExit;
                }
                fprintf( fp,"~CMT���|��������|������|˳���|��������|���׽��|��  ��  ��|��  ��  ��|״     ̬|\n");
            }

            iNum2TxType(hv_fzf.cmtno,cTx_Desc);
            iNum2Stat(hv_fzf.hv_fzf_sts[0],cStat);
            /* add by LiuHuafeng 20070505 ���Ӷ�cmt302���⴦�� */
    vtcp_log("%s,%d cmtno=[%s],lw_ind=[%c]",__FILE__,__LINE__,hv_fzf.cmtno,cLw_ind[0]);
            if(memcmp(cTx_type,"302",3)==0 && cLw_ind[0]=='2')
            {
                memcpy(cStat,"��������",8);
                memcpy(cTx_Desc,"С��鸴",8);
                fprintf( fp,"%s|%s|%s|%s|%ld|%.2f|%s|%s|%s|\n","302",cTx_Desc,hv_fzf.br_no,hv_fzf.res_orderno,hv_fzf.wt_date,hv_fzf.o_tx_amt,hv_fzf.req_br_no,hv_fzf.or_br_no,cStat);
            }
            else
            fprintf( fp,"%s|%s|%s|%s|%ld|%.2f|%s|%s|%s|\n",hv_fzf.cmtno,cTx_Desc,hv_fzf.br_no,hv_fzf.orderno,hv_fzf.wt_date,hv_fzf.o_tx_amt,hv_fzf.or_br_no,hv_fzf.ac_br_no,cStat);
            ttlnum++;
            memset(cTx_Desc,0x00,sizeof(cTx_Desc));

        }
        ret=Hv_fzf_Clo_Sel();
        if( !ttlnum )
        {
            strcpy( g_pub_tx.reply,"S049" );
            goto ErrExit;
        }
        else
        {
            fclose(fp);
            set_zd_data( DC_FILE_SND,"1");
        }

    }



OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
    sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;

}

    /**��������**/
char * iNum2TxType(char *ch,char *cTmpList)
{

    vtcp_log("[%s][%d]ch===[%s]\n",__FILE__,__LINE__,ch);

    char cName[20];
    memset(cName,0x00,sizeof(cName));

    if(!memcmp(ch,"001",3))
    {           
        strcpy(cName,"��ͨ����");
    } 
    else if(!memcmp(ch,"002",3))
    {
        strcpy(cName,"��ͨ���");
    }
    else if(!memcmp(ch,"003",3))
    {
        strcpy(cName,"ʵʱ����");
    }
    else if(!memcmp(ch,"004",3))
    {
        strcpy(cName,"ʵʱ���");
    }
    else if(!memcmp(ch,"005",3))
    {
        strcpy(cName,"���ڴ���");
    }
    else if(!memcmp(ch,"006",3))
    {
        strcpy(cName,"���ڽ��");
    }
    else if(!memcmp(ch,"007",3))
    {
        strcpy(cName,"�����˻�");
    }
    else if(!memcmp(ch,"008",3))
    {
        strcpy(cName,"�ս��ִ");
    }
    else if(!memcmp(ch,"009",3))
    {
        strcpy(cName,"ʵʱ���ǻ�ִ");
    }
    else if(!memcmp(ch,"010",3))
    {
        strcpy(cName,"ʵʱ��ǻ�ִ");
    }
    else if(!memcmp(ch,"011",3))
    {
        strcpy(cName,"���ڽ�ǻ�ִ");
    }
    else if(!memcmp(ch,"012",3))
    {
        strcpy(cName,"ͨ����Ϣ");
    }
    else if(!memcmp(ch,"013",3))
    {
        strcpy(cName,"ʵʱ��Ϣ");
    }
    else if(!memcmp(ch,"301",3))
    {
        strcpy(cName,"С���ѯ");
    }
    else if(!memcmp(ch,"302",3))
    {
        strcpy(cName,"С��鸴");
    }
    else if(!memcmp(ch,"303",3))
    {
        strcpy(cName,"���ɸ�ʽ");
    }
    else if(!memcmp(ch,"319",3))
    {
        strcpy(cName,"�����˻�");
    }
    else if(!memcmp(ch,"320",3))
    {
        strcpy(cName,"�˻�Ӧ��");
    }
    else if(!memcmp(ch,"321",3))
    {
        strcpy(cName,"���볷��");
    }
    else if(!memcmp(ch,"322",3))
    {
        strcpy(cName,"����Ӧ��");
    }
    else if(!memcmp(ch,"323",3))
    {
        strcpy(cName,"��������");
    }
    else if(!memcmp(ch,"324",3))
    {
        strcpy(cName,"����Ӧ��");
    }
    else if(!memcmp(ch,"325",3))
    {
        strcpy(cName,"����֪ͨ");
    }
    else if(!memcmp(ch,"327",3))
    {
        strcpy(cName,"ֹ������");
    }
    else if(!memcmp(ch,"328",3))
    {
        strcpy(cName,"ֹ��Ӧ��");
    }
    else
        strcpy(cName,"δ֪��������");
    MEMCPY_DEBUG(cTmpList,cName,strlen(cName));
    vtcp_log("[%s][%d]cTmplist==[%s]\n",__FILE__,__LINE__,cTmpList);
    return  cName;
}

/**����״̬**/
char * iNum2Stat(char ch,char *cTmpList)
{
    vtcp_log("[%s][%d]ch===[%c]\n",__FILE__,__LINE__,ch);
    if(ch=='0')
    {
        strcpy(cTmpList,"��Ч״̬");
    }
    else if(ch=='1')
    {
        strcpy(cTmpList,"����¼��");
    }
    else if(ch=='3')
    {
        strcpy(cTmpList,"���˸���");
    }
    else if(ch=='4')
    {
        strcpy(cTmpList,"���˷���");
    }
    else if(ch=='5')
    {
        strcpy(cTmpList,"���˾ܾ�");
    }
    else if(ch=='6')
    {
        strcpy(cTmpList,"��������");
    }
    else if(ch=='7')
    {
        strcpy(cTmpList,"�����Ŷ�");
    }
    else if(ch=='8')
    {
        strcpy(cTmpList,"���˳����ȴ�");
    }
    else if(ch=='9')
    {
        strcpy(cTmpList,"���˳���");
    }
    else if(ch=='A')
    {
        strcpy(cTmpList,"�˻س���ֹ���ȴ�");
    }
    else if(ch=='B')
    {
        strcpy(cTmpList,"�����˻�");
    }
    else if(ch=='C')
    {
        strcpy(cTmpList,"�����ѳ���");
    }
    else if(ch=='D')
    {
        strcpy(cTmpList,"������ֹ��");
    }
    else if(ch=='G')
    {
        strcpy(cTmpList,"�����쳣����");
    }
    else if(ch=='H')
    {
        strcpy(cTmpList,"��ʱ����,������");
    }
    else if(ch=='I')
    {
        strcpy(cTmpList,"��ʽ����");
    }
    else if(ch=='J')
    {
        strcpy(cTmpList,"ȫ��Ѻ��");
    }
    else if(ch=='K')
    {
        strcpy(cTmpList,"�ط�Ѻ��");
    }
    else if(ch=='L')
    {
        strcpy(cTmpList,"ȫ��Ѻ�͵ط�Ѻ����");
    }
    else if(ch=='M')
    {
        strcpy(cTmpList,"�������Ĺ���");
    }
    else if(ch=='N')
    {
        strcpy(cTmpList,"֧�й���");
    }
    else if(ch=='O')
    {
        strcpy(cTmpList,"�����˻صȴ�");
    }
    else if(ch=='P')
    {
        strcpy(cTmpList,"�����˻�");
    }
    else if(ch=='Q')
    {
        strcpy(cTmpList,"ͬ��δ����");
    }
    else if(ch=='R')
    {
        strcpy(cTmpList,"ͬ���ѷ���");
    }
    else if(ch=='S')
    {
        strcpy(cTmpList,"��ͬ��δ����");
    }
    else if(ch=='T')
    {
        strcpy(cTmpList,"��ͬ���ѷ���");
    }
    else if(ch=='U')
    {
        strcpy(cTmpList,"�����ѳ���");
    }
    else if(ch=='V')
    {
        strcpy(cTmpList,"������ֹ��");
    }
    else if(ch=='W')
    {
        strcpy(cTmpList,"�����Ѹ���");
    }
    else if(ch=='Z')
    {
        strcpy(cTmpList,"�����쳣����");
    }
    else
        strcpy(cTmpList,"δ֪״̬");

    vtcp_log("[%s][%d]cTmpList==[%s]\n",__FILE__,__LINE__,cTmpList);
    return cTmpList;

}

/***��ִ״̬*****/
char * iNum2RcpStat(char *rcpstat , char *cTmpList,char *pkgno)
{
    /**add by NEWBP4**/
    int iRet=0;
    char cTmpStat[7];
    struct lv_param_c lv_param;
    memset(cTmpStat  , 0 , sizeof(cTmpStat));
    memset(&lv_param , 0 , sizeof(lv_param));
    memcpy(cTmpStat,"TPLY",4);
    strcat(cTmpStat,rcpstat);
    vtcp_log("[%s][%d]cTmpStat=[%s]",__FILE__,__LINE__,cTmpStat);
    /**end by NEWBP4**/
    vtcp_log("liuyue[%s][%d]rcpstat==[%s]\n",__FILE__,__LINE__,rcpstat);
    if(!memcmp(pkgno,PKGNO_PTJJ,3) ||!memcmp(pkgno,PKGNO_PTJJHZ,3) ||!memcmp(pkgno,PKGNO_DQJJ,3)||!memcmp(pkgno,PKGNO_DQJJHZ,3)||!memcmp(pkgno,PKGNO_SSJJ,3) ||!memcmp(pkgno,PKGNO_SSJJHZ,3)||!memcmp(pkgno,PKGNO_SSDJHZ,3)||!memcmp(pkgno,PKGNO_SSDJ,3))
    {
          /****add by NEWBP4****/
          iRet = Lv_param_Sel(g_pub_tx.reply, &lv_param, "par_code = '%s' ", cTmpStat);
          if (iRet!=0 )
          {
                if(iRet== DB_NOTFOUND)
                {
                      strcpy(cTmpList,"δ����ִ");
                }
                else
                {
                  vtcp_log("%s,%d,���ݿ����[%d]\n",__FL__,iRet);
                  sprintf(acErrMsg,"���ݿ����");
                  return NULL;
              }
          }
          else
          {
             memcpy(cTmpList,lv_param.par_name,20);
          }
          /**end by NEWBP4**/
          /****delete by NEWBP4
            if(!memcmp(rcpstat,"00",2))
            {
                strcpy(cTmpList,"�ɹ�");
            }    
            else if(!memcmp(rcpstat,"01",2))
            {
                strcpy(cTmpList,"�ʺŲ���");
            }    
            else if(!memcmp(rcpstat,"02",2))
            {
                strcpy(cTmpList,"�ʺŻ�������");
            }    
            else if(!memcmp(rcpstat,"03",2))
            {
                strcpy(cTmpList,"�˻�����֧��");
            }    
            else if(!memcmp(rcpstat,"10",2))
            {
                strcpy(cTmpList,"�˻��������");
            }    
            else if(!memcmp(rcpstat,"11",2))
            {
                strcpy(cTmpList,"�ʺ�״̬����");
            }    
            else if(!memcmp(rcpstat,"20",2))
            {
                strcpy(cTmpList,"ҵ���ѳ���");
            }    
            else if(!memcmp(rcpstat,"90",2))
            {
                strcpy(cTmpList,"��������");
            }
            else 
                strcpy(cTmpList,"δ����ִ");    
           **** end by NEWBP4****/
    }
    else
                strcpy(cTmpList,"       ");

    vtcp_log("[%s][%d]cTmpList===[%s]\n",__FILE__,__LINE__,cTmpList);
    return cTmpList;
}


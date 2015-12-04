/***************************************************************
* 文 件 名:     spJ090.c
* 功能描述：    小额历史信息查询
*
* 修改记录：    满足银行的多重需求
* 日    期:     20060921
* 修 改 人:     liuyue
* 修改内容:
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

    char    cLw_ind[2];      /*来往标志 1 往  2 来*/
    char    cBr_no[6];        /*机构号*/
    char    cTx_date[9];      /*交易日期*/
    char    cZf_type[2];      /*支付标志 0 支付类  1 非支付类 */
    char    cTx_type[4];      /*支付交易类型*/
    char    cTx_stat[2];       /*交易状态*/
    int     ttlnum = 0;       /*读取的总条数*/
    FILE    *fp;
    char    cFilename[100];    /*文件名*/
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

    /*** 初始化全局变量 ***/
    pub_base_sysinit();

    get_zd_data("0670",cLw_ind);         /*来往标志*/     
    get_zd_data("0280",cBr_no);             /*机构号码*/
    get_zd_data("0440",cTx_date);         /*交易日期*/
    get_zd_data("0680",cZf_type);         /*支付类型*/
    get_zd_data("0230",cTx_type);         /*CMT编号 */
    get_zd_data("0690",cTx_stat);         /*交易状态*/

    vtcp_log("[%s][%d]机构号==[%s]\n",__FILE__,__LINE__,cBr_no);
    vtcp_log("[%s][%d]来往标志==[%s]\n",__FILE__,__LINE__,cLw_ind);
    vtcp_log("[%s][%d]交易类型==[%s]\n",__FILE__,__LINE__,cTx_type);
    vtcp_log("[%s][%d]记录状态==[%s]\n",__FILE__,__LINE__,cTx_stat);
    vtcp_log("[%s][%d]支付种类==[%s]\n",__FILE__,__LINE__,cZf_type);
    vtcp_log("[%s][%d]查询日期==[%s]\n",__FILE__,__LINE__,cTx_date);

    vtcp_log("[%s][%d]开始小额的查询交易!\n",__FILE__,__LINE__);

    pub_base_AllDwnFilName(cFilename);
    vtcp_log("[%s][%d]cFilename===[%s]\n",__FILE__,__LINE__,cFilename);
    fp = fopen(cFilename,"rb");
    if(fp==NULL)
    {
        sprintf(acErrMsg,"打开文件[%s]错误!\n",cFilename);
        WRITEMSG
        strcpy(g_pub_tx.reply,"HV01");
    }
    fclose(fp);


    /********清算中心可以查询它行的交易*****/

    /**组成查询条件**/

    /**********************
     *处理交易机构号      *
     *清算可以查全部      *
     **********************/
    if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5))  /****如果不是清算中心**/
    {
        zip_space(cBr_no);
        iBr_no    = atoi(cBr_no);
        if(iBr_no==0)
        {
            vtcp_log("[%s][%d]非清算中心,请输入本机构号!\n",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,"非清算中心,请输入本机构号!");
            strcpy(g_pub_tx.reply,"HV01");
            goto ErrExit;    
        }

        if(memcmp(g_pub_tx.tx_br_no,cBr_no,sizeof(cBr_no)-1))
        {
            vtcp_log("[%s][%d]非清算中心不允许查询它行业务!\n",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,"非清算中心不能查询它行的业务!");
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
     * 处理日期      *
     *日期允许输入空 *
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
     *处理报文编号  非支付应答类特别                     *
     *注意 :  这里对选择全部报文类型时候非支付应答类    *
     *        处理不好,虽然可以直接将报文类型302 320 等 *
     *        直接代入可以处理,但是在次不处理了         *
     ****************************************************/


    if(memcmp(cTx_type,"888", sizeof(cTx_type)-1))/*****如果选择的不是全部***/
    {
        if(cZf_type[0]=='1')
        {
                /**由于应答类一般是与申请类同记录,这样处理来查找出应答类交易*****/
                iCmtno =  apatoi(cTx_type,sizeof(cTx_type)-1);
                if(iCmtno%2==0)/***如果事应答类的非支付交易***/
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
     * 处理记录状态    *
     * 注意匹配全部    *
     *******************/

    if(cTx_stat[0]!='E'&&cZf_type[0]=='1')/***非支付类全部**/
    {
        sprintf(cTmplist,"hv_fzf_sts ='%c'  and  ",cTx_stat[0]);
        strcat(cWherelist,cTmplist);
    }
    else if(cTx_stat[0]!='E'&&cZf_type[0]=='0') /**支付类*/
    {
        sprintf(cTmplist,"lv_sts ='%c' and ",cTx_stat[0]);
        strcat(cWherelist,cTmplist);
    }



    /**************************
     *如果是非支付,区分大小额 *
     **************************/

    if(cZf_type[0]=='1')
    {
            sprintf(cTmplist,"otype = '%c' and ",'2');
            strcat(cWherelist,cTmplist);
    }


    /***************************
     *处理来往标志             *
     *应答类来帐不会改变标志   *
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
            else if(cZf_type[0]=='1')/*只有非支付时候需要处理的***/
            {
                if(iCmtno%2==1)/*申请类直接去就可以了**/
                    sprintf(cTmplist," lw_ind ='%c' ",cLw_ind[0]);
                else if(iCmtno%2==0)
                    sprintf(cTmplist," lw_ind ='%d' ",1+(cLw_ind[0]-'0')%2);
                    
            }
    }

    strcat(cWherelist,cTmplist);

    strcat(cWherelist," and 1=1 order by br_no,orderno");
    vtcp_log("[%s][%d]cWherelist=[%s]\n",__FILE__,__LINE__,cWherelist);

    vtcp_log("[%s][%d]支付类型===[%c]\n",__FILE__,__LINE__,cZf_type[0]);
    /**开始查询**/
    /***支付交易类的查询***/
    if(cZf_type[0] == '0')
    {
        vtcp_log("[%s][%d]查询支付类交易!\n",__FILE__,__LINE__);
        ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, cWherelist);
        if(ret)
        {
            sprintf(acErrMsg,"查询小额支付交易基本信息出错[%d]",ret);
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
                vtcp_log("[%s][%d]没有找到对应的记录!\n",__FILE__,__LINE__);
                set_zd_data(DC_GET_MSG,"找不到对应的记录!\n");
                strcpy(g_pub_tx.reply,"D104");
                goto ErrExit;
            }
            else if(ret && ret!=100)
            {
                vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!\n",__FILE__,__LINE__);
                set_zd_data(DC_GET_MSG,"数据库操作异常,请与科技科联系!\n");
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
                fprintf( fp,"~PKG编号|交易类型|交易行|顺序号|交易日期|交易金额|发起行|接受行|包序号  |包日期  |交易状态|回执状态|\n" );
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
        fprintf( fp,"共计:|%d笔|金额总计:|%.2f|\n",ttlnum,Tamt);
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
    else if(cZf_type[0]=='1') /***非支付类交易查询***/
    {
        ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, cWherelist);
        if(ret)
        {
            sprintf(acErrMsg,"查询小额非支付交易基本信息出错[%d]",ret);
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
                vtcp_log("[%s][%d]没有找到对应的记录!\n",__FILE__,__LINE__);
                goto ErrExit;
            }
            else if(ret && ret!=100)
            {
                vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!\n",__FILE__,__LINE__);
                set_zd_data(DC_GET_MSG,"数据库操作异常,请与科技科联系!\n");
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
                fprintf( fp,"~CMT编号|交易类型|交易行|顺序号|交易日期|交易金额|发  起  行|接  受  行|状     态|\n");
            }

            iNum2TxType(hv_fzf.cmtno,cTx_Desc);
            iNum2Stat(hv_fzf.hv_fzf_sts[0],cStat);
            /* add by LiuHuafeng 20070505 增加对cmt302特殊处理 */
    vtcp_log("%s,%d cmtno=[%s],lw_ind=[%c]",__FILE__,__LINE__,hv_fzf.cmtno,cLw_ind[0]);
            if(memcmp(cTx_type,"302",3)==0 && cLw_ind[0]=='2')
            {
                memcpy(cStat,"正常接收",8);
                memcpy(cTx_Desc,"小额查复",8);
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
    sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
    sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;

}

    /**交易类型**/
char * iNum2TxType(char *ch,char *cTmpList)
{

    vtcp_log("[%s][%d]ch===[%s]\n",__FILE__,__LINE__,ch);

    char cName[20];
    memset(cName,0x00,sizeof(cName));

    if(!memcmp(ch,"001",3))
    {           
        strcpy(cName,"普通贷记");
    } 
    else if(!memcmp(ch,"002",3))
    {
        strcpy(cName,"普通借记");
    }
    else if(!memcmp(ch,"003",3))
    {
        strcpy(cName,"实时贷记");
    }
    else if(!memcmp(ch,"004",3))
    {
        strcpy(cName,"实时借记");
    }
    else if(!memcmp(ch,"005",3))
    {
        strcpy(cName,"定期贷记");
    }
    else if(!memcmp(ch,"006",3))
    {
        strcpy(cName,"定期借记");
    }
    else if(!memcmp(ch,"007",3))
    {
        strcpy(cName,"贷记退汇");
    }
    else if(!memcmp(ch,"008",3))
    {
        strcpy(cName,"普借回执");
    }
    else if(!memcmp(ch,"009",3))
    {
        strcpy(cName,"实时贷记回执");
    }
    else if(!memcmp(ch,"010",3))
    {
        strcpy(cName,"实时借记回执");
    }
    else if(!memcmp(ch,"011",3))
    {
        strcpy(cName,"定期借记回执");
    }
    else if(!memcmp(ch,"012",3))
    {
        strcpy(cName,"通用信息");
    }
    else if(!memcmp(ch,"013",3))
    {
        strcpy(cName,"实时信息");
    }
    else if(!memcmp(ch,"301",3))
    {
        strcpy(cName,"小额查询");
    }
    else if(!memcmp(ch,"302",3))
    {
        strcpy(cName,"小额查复");
    }
    else if(!memcmp(ch,"303",3))
    {
        strcpy(cName,"自由格式");
    }
    else if(!memcmp(ch,"319",3))
    {
        strcpy(cName,"申请退回");
    }
    else if(!memcmp(ch,"320",3))
    {
        strcpy(cName,"退回应答");
    }
    else if(!memcmp(ch,"321",3))
    {
        strcpy(cName,"申请撤销");
    }
    else if(!memcmp(ch,"322",3))
    {
        strcpy(cName,"撤销应答");
    }
    else if(!memcmp(ch,"323",3))
    {
        strcpy(cName,"冲正申请");
    }
    else if(!memcmp(ch,"324",3))
    {
        strcpy(cName,"冲正应答");
    }
    else if(!memcmp(ch,"325",3))
    {
        strcpy(cName,"冲正通知");
    }
    else if(!memcmp(ch,"327",3))
    {
        strcpy(cName,"止付申请");
    }
    else if(!memcmp(ch,"328",3))
    {
        strcpy(cName,"止付应答");
    }
    else
        strcpy(cName,"未知交易类型");
    MEMCPY_DEBUG(cTmpList,cName,strlen(cName));
    vtcp_log("[%s][%d]cTmplist==[%s]\n",__FILE__,__LINE__,cTmpList);
    return  cName;
}

/**交易状态**/
char * iNum2Stat(char ch,char *cTmpList)
{
    vtcp_log("[%s][%d]ch===[%c]\n",__FILE__,__LINE__,ch);
    if(ch=='0')
    {
        strcpy(cTmpList,"无效状态");
    }
    else if(ch=='1')
    {
        strcpy(cTmpList,"往账录入");
    }
    else if(ch=='3')
    {
        strcpy(cTmpList,"往账复核");
    }
    else if(ch=='4')
    {
        strcpy(cTmpList,"往账发送");
    }
    else if(ch=='5')
    {
        strcpy(cTmpList,"往账拒绝");
    }
    else if(ch=='6')
    {
        strcpy(cTmpList,"往账扎差");
    }
    else if(ch=='7')
    {
        strcpy(cTmpList,"往账排队");
    }
    else if(ch=='8')
    {
        strcpy(cTmpList,"往账撤销等待");
    }
    else if(ch=='9')
    {
        strcpy(cTmpList,"往账撤销");
    }
    else if(ch=='A')
    {
        strcpy(cTmpList,"退回冲正止付等待");
    }
    else if(ch=='B')
    {
        strcpy(cTmpList,"往账退回");
    }
    else if(ch=='C')
    {
        strcpy(cTmpList,"往账已冲正");
    }
    else if(ch=='D')
    {
        strcpy(cTmpList,"往账已止付");
    }
    else if(ch=='G')
    {
        strcpy(cTmpList,"往账异常挂帐");
    }
    else if(ch=='H')
    {
        strcpy(cTmpList,"临时接收,不记帐");
    }
    else if(ch=='I')
    {
        strcpy(cTmpList,"正式接收");
    }
    else if(ch=='J')
    {
        strcpy(cTmpList,"全国押错");
    }
    else if(ch=='K')
    {
        strcpy(cTmpList,"地方押错");
    }
    else if(ch=='L')
    {
        strcpy(cTmpList,"全国押和地方押都错");
    }
    else if(ch=='M')
    {
        strcpy(cTmpList,"清算中心挂帐");
    }
    else if(ch=='N')
    {
        strcpy(cTmpList,"支行挂帐");
    }
    else if(ch=='O')
    {
        strcpy(cTmpList,"来账退回等待");
    }
    else if(ch=='P')
    {
        strcpy(cTmpList,"来账退回");
    }
    else if(ch=='Q')
    {
        strcpy(cTmpList,"同意未发送");
    }
    else if(ch=='R')
    {
        strcpy(cTmpList,"同意已发送");
    }
    else if(ch=='S')
    {
        strcpy(cTmpList,"不同意未发送");
    }
    else if(ch=='T')
    {
        strcpy(cTmpList,"不同意已发送");
    }
    else if(ch=='U')
    {
        strcpy(cTmpList,"来账已冲正");
    }
    else if(ch=='V')
    {
        strcpy(cTmpList,"来账已止付");
    }
    else if(ch=='W')
    {
        strcpy(cTmpList,"来账已付款");
    }
    else if(ch=='Z')
    {
        strcpy(cTmpList,"来账异常挂帐");
    }
    else
        strcpy(cTmpList,"未知状态");

    vtcp_log("[%s][%d]cTmpList==[%s]\n",__FILE__,__LINE__,cTmpList);
    return cTmpList;

}

/***回执状态*****/
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
                      strcpy(cTmpList,"未被回执");
                }
                else
                {
                  vtcp_log("%s,%d,数据库错误[%d]\n",__FL__,iRet);
                  sprintf(acErrMsg,"数据库错误");
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
                strcpy(cTmpList,"成功");
            }    
            else if(!memcmp(rcpstat,"01",2))
            {
                strcpy(cTmpList,"帐号不符");
            }    
            else if(!memcmp(rcpstat,"02",2))
            {
                strcpy(cTmpList,"帐号户名不符");
            }    
            else if(!memcmp(rcpstat,"03",2))
            {
                strcpy(cTmpList,"账户余额不足支付");
            }    
            else if(!memcmp(rcpstat,"10",2))
            {
                strcpy(cTmpList,"账户密码错误");
            }    
            else if(!memcmp(rcpstat,"11",2))
            {
                strcpy(cTmpList,"帐号状态错误");
            }    
            else if(!memcmp(rcpstat,"20",2))
            {
                strcpy(cTmpList,"业务已撤销");
            }    
            else if(!memcmp(rcpstat,"90",2))
            {
                strcpy(cTmpList,"其他错误");
            }
            else 
                strcpy(cTmpList,"未被回执");    
           **** end by NEWBP4****/
    }
    else
                strcpy(cTmpList,"       ");

    vtcp_log("[%s][%d]cTmpList===[%s]\n",__FILE__,__LINE__,cTmpList);
    return cTmpList;
}


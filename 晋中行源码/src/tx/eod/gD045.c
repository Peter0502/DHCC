/*************************************************************
* 文 件 名: gD045.c
* 功能描述：卡收年费处理
*
* 作    者: mike
* 完成日期: 20030801
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容: 不需要扩表 那本程序
1.采用card_reg filler1 记录POS消费次数 
  因为没有采用自然年的那种统计,只能在此记录POS一个周期内的消费次数,年费程序执行后更新成零
2.采用card_reg filler2 记录年费扣除日期 
  当第一次扣年费是取卡的怎开户日与当前日期,如果满足一年则 记录当前日期
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"
#include "com_chrg_rate_c.h"
#include "com_parm_c.h"
#include "dc_acc_rel_c.h"
#include "com_chrg_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"
#include "com_item_c.h"
#include "card_reg_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "card.h"
char    FLD124[100];
gD045(){
    int ret;
    FILE *fp, *fpend;
    char    fldstr[256][25];
    char    line[2048];
    long  lTmp=0;
    char  cAcno[25];
    char  cTmpBuff[23];
    char    str[2048], succmsg[21], fee_type[2];
    int iOpen_end_flag=0;
    double fee_recv, fee_fact, kou_fee_fact;
    struct com_sys_parm_c com_sys_parm ;
    struct mdm_ac_rel_c mdm_ac_rel ;
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    memset(cAcno   , 0 , sizeof(cAcno));
    memset(line    , 0 , sizeof(line));
    memset(succmsg , 0 , sizeof(succmsg));
    memset(fee_type, 0 , sizeof(fee_type));
    
    vtcp_log("%s %d 开始日终处理！！！", __FILE__, __LINE__);

    ret = sql_begin();    /* 打开事务 */
    if (ret) {
        sprintf(acErrMsg, "打开事务失败!!!");
        WRITEMSG
        goto ErrExit;
    }
    
    pub_base_sysinit();
/*************    
    if(gl_init_str.tel_ejfno==0)
    {
        memset(&gl_init_str , 0 , sizeof(gl_init_str));
        gl_init_str.tel_ejfno=lGet_tel_seqn(g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.reply);
        set_zd_long(DC_EJFNO,gl_init_str.tel_ejfno);
        vtcp_log("%s,%d ejfno=%ld",__FILE__,__LINE__,gl_init_str.tel_ejfno);
    }
    get_zd_long(DC_EJFNO,&lTmp);
    vtcp_log("[%s][%d] lTmp =[%ld] %d",__FILE__,__LINE__,lTmp,gl_init_str.tel_ejfno);
    
************/
    if (pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply)){
        sprintf(acErrMsg, "取主机流水号错 [%d]", g_pub_tx.trace_no);
        WRITEMSG
        goto ErrExit;
    }
    if (pub_base_GetSysparm(&com_sys_parm )){
        sprintf(acErrMsg, "取主机日期错 [%d]", __LINE__);
        WRITEMSG
        goto ErrExit;
    }
    strcpy(g_pub_tx.sub_tx_code, "D045");
    strcpy(g_pub_tx.tx_code, "D045");
    g_pub_tx.tx_date= com_sys_parm.sys_date;
    sprintf(str, "%s/yearfee/CARD_YEAR%ld.txt", getenv("HOME"), g_pub_tx.tx_date);
    vtcp_log("[%s][%d] str=[%s] ",__FILE__,__LINE__,str);
    if(NULL==(fp=fopen(str,"r")))
    {
        sprintf(acErrMsg, "%s没找到文件!!!!![%s]", str,g_pub_tx.reply);
        WRITEMSG
        goto OkExit;
    }
    sprintf(str, "%s/yearfee/CARD_YEAR%ldend.txt", getenv("HOME"), g_pub_tx.tx_date);
    if(NULL==(fpend=fopen(str,"w"))){
        strcpy(g_pub_tx.reply, "CUD5");
        sprintf(acErrMsg, "%s文件打开失败!!!!![%s]", str,g_pub_tx.reply);
        WRITEMSG
        goto ErrExit;
    }
    iOpen_end_flag=1;
    while(NULL!=fgets(line,2048,fp)){
        int i=0;

	/* add by Wang Yongwei 读取每行压缩空格后，若第一个字符为换行符则直接跳过 */
	pub_base_strpack_all(line);		
	if(line[0]=='\n')
	{
		vtcp_log("%s,%d,WYW次行非非法行!",__FILE__,__LINE__);
		continue;
	}

        while(' '==line[i])i++;
        if('#'==line[i])        continue;
        i = 0;
        
        fee_recv = 0.00;
        fee_fact = 0.00;
        
        memset(cAcno   , 0 , sizeof(cAcno));
        memset(fee_type , 0 , sizeof(fee_type));
        memset( fldstr, 0x0, sizeof(fldstr) );
        for( ; ; )
        {
            ret = pub_base_cut_str( line,fldstr[i],"|",i+1 );
            if ( ret == 100 )
            {
                break;
            }
            vtcp_log("[%s][%d] i+1= [%d] [%s] ",__FILE__,__LINE__,i+1,fldstr[i]);
            pub_base_strpack( fldstr[i] );
            switch(i+1){
                case 1: 
                        strcpy(cAcno, fldstr[i]);
                        zip_space(cAcno);
                        break;
                case 2:
                        break;
                case 3:
                        strcpy(fee_type, fldstr[i]);
                        break;
                case 4:
                        sscanf( fldstr[i], "%lf", &fee_recv );
                        break;
                case 5:
                        sscanf( fldstr[i], "%lf", &fee_fact );
                        break;
            }
            i = i + 1;
        }
        kou_fee_fact=fee_fact;
        vtcp_log("[%s][%d] 实扣金额=[%f]",__FILE__,__LINE__,kou_fee_fact);
        if(ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &mdm_ac_rel, "ac_no='%s'",cAcno)){
            sprintf(acErrMsg, "查%s账户介质表出错!!!!![%s]", mdm_ac_rel.ac_no,g_pub_tx.reply);
            WRITEMSG
            fprintf(fpend, "%s | 0.00 | 无此账户\n", line);
            continue;
        }
        memset(succmsg , 0 , sizeof(succmsg));
        ret=pub_eod_crd_fee("10", mdm_ac_rel.opn_br_no, cAcno , '2', succmsg, &kou_fee_fact);
        if(ret==0)
        {
            vtcp_log("[%s][%d] succmsg=[%s]",__FILE__,__LINE__,succmsg);
            zip_space(succmsg);
            memset(cTmpBuff , 0 , sizeof(cTmpBuff));
  
            vtcp_log("[%s][%d] kou_fee_fact=[%f]",__FILE__,__LINE__,kou_fee_fact);
            sprintf(cTmpBuff,"%.2f",kou_fee_fact);
            zip_space(cTmpBuff);
            fprintf(fpend, "%s | %s | %s\n", line, cTmpBuff, succmsg);
            memset(succmsg , 0 , sizeof(succmsg));
        }
        else
        {
            vtcp_log("[%s][%d] err!!!!!!!!!!!!!!![%d] succmsg=[%s]",__FILE__,__LINE__,ret,succmsg);
            fprintf(fpend, "%s | %.2f | %s\n", line, kou_fee_fact, succmsg);
            goto ErrExit;
        }
    }
    fclose(fp);
    fclose(fpend);
OkExit:
    sql_commit();/*--事务提交--*/
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;


ErrExit:
    fclose(fp);
    if(iOpen_end_flag==1)
      fclose(fpend);
    sql_rollback();        /*--事务回滚--*/
    sprintf(acErrMsg, "处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    if(memcmp(g_pub_tx.reply, "0000",4)==0)
        memcpy(g_pub_tx.reply,"D045",4);
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}

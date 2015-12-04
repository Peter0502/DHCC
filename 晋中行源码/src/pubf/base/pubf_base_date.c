#include "find_debug.h" 
/*************************************************
* ÎÄ ¼ş Ãû:     pub_base_date
* ¹¦ÄÜÃèÊö£º    1¡¢°üÀ¨Ä£¿épub_base_CalTriDays¼ÆËãÌìÊı(30Ìì)£»
*               2¡¢°üÀ¨Ä£¿épub_base_deadlineD¼ÆËãµ½ÆÚÈÕ(°´ÈÕ)£»
*               3¡¢°üÀ¨Ä£¿épub_base_deadlineM¼ÆËãµ½ÆÚÈÕ(°´ÔÂ)£»
*               4¡¢°üÀ¨Ä£¿épub_base_ChgJjrÈ·¶¨½Ú¼ÙÈÕÆğÊ¼ÖÕÖ¹ÈÕ;
*               5¡¢°üÀ¨Ä£¿épub_base_get_dateÈ¡µÃÏµÍ³ÈÕÆÚ;
*               6¡¢°üÀ¨Ä£¿épub_base_get_ymdÈ¡µÃÄêÔÂÈÕ; 
*               8¡¢°üÀ¨Ä£¿épub_base_chk_leapÅĞ¶ÏÊÇ·ñÈòÄê;
*               9¡¢°üÀ¨Ä£¿épub_base_day_weekÅĞ¶ÏĞÇÆÚ¼¸;
*              10¡¢°üÀ¨Ä£¿épub_base_day_xunÅĞ¶ÏÊÇÒ»Ñ®µÄµÚ¼¸Ìì;
*              11¡¢°üÀ¨Ä£¿épub_base_day_monthÅĞ¶ÏÊÇÒ»ÔÂµÄµÚ¼¸Ìì;
*              12¡¢°üÀ¨Ä£¿épub_base_day_quarterÅĞ¶ÏÊÇÒ»¼¾µÄµÚ¼¸Ìì;
*              13¡¢°üÀ¨Ä£¿épub_base_day_halfÅĞ¶ÏÊÇ°ëÄêµÄµÚ¼¸Ìì;
*              14¡¢°üÀ¨Ä£¿épub_base_day_yearÅĞ¶ÏÊÇÒ»ÄêµÄµÚ¼¸Ìì;
*              15¡¢°üÀ¨Ä£¿épub_base_week_monthÅĞ¶ÏÊÇÒ»ÔÂµÄµÚ¼¸ÖÜ;
*              16¡¢°üÀ¨Ä£¿épub_base_week_quarterÅĞ¶ÏÊÇÒ»¼¾µÄµÚ¼¸ÖÜ;
*              17¡¢°üÀ¨Ä£¿épub_base_week_halfÅĞ¶ÏÊÇ°ëÄêµÄµÚ¼¸ÖÜ;
*              18¡¢°üÀ¨Ä£¿épub_base_week_yearÅĞ¶ÏÊÇÒ»ÄêµÄµÚ¼¸ÖÜ;
*              19¡¢°üÀ¨Ä£¿épub_base_xun_monthÅĞ¶ÏÊÇÔÂµÄµÚ¼¸Ñ®;
*              20¡¢°üÀ¨Ä£¿épub_base_xun_quarterÅĞ¶ÏÊÇ¼¾¶ÈµÄµÚ¼¸Ñ®;
*              21¡¢°üÀ¨Ä£¿épub_base_xun_halfÅĞ¶ÏÊÇ°ëÄêµÄµÚ¼¸Ñ®;
*              22¡¢°üÀ¨Ä£¿épub_base_xun_yearÅĞ¶ÏÊÇÄêµÄµÚ¼¸Ñ®;
*              23¡¢°üÀ¨Ä£¿épub_base_month_quarterÅĞ¶ÏÊÇ¼¾¶ÈµÄµÚ¼¸ÔÂ;
*              24¡¢°üÀ¨Ä£¿épub_base_month_halfÅĞ¶ÏÊÇ°ëÄêµÄµÚ¼¸ÔÂ;
*              25¡¢°üÀ¨Ä£¿épub_base_month_yearÅĞ¶ÏÊÇÄêµÄµÚ¼¸ÔÂ;
*              26¡¢°üÀ¨Ä£¿épub_base_quarter_halfÅĞ¶ÏÊÇ°ëÄêµÄµÚ¼¸¼¾¶È;
*              27¡¢°üÀ¨Ä£¿épub_base_quarter_yearÅĞ¶ÏÊÇÄêµÄµÚ¼¸¼¾¶È;
*              28¡¢°üÀ¨Ä£¿épub_base_half_yearÅĞ¶ÏÊÇÇ°°ëÄê»¹ÊÇºó°ëÄê;
*              29¡¢°üÀ¨Ä£¿épub_base_date_jjrÈ¡½Ú¼ÙÈÕÎÄ¼ş;
*              30¡¢°üÀ¨Ä£¿épub_base_PubAjTs¼ÆËã°´½Ò´û¿îÌìÊı;
*              31¡¢°üÀ¨Ä£¿épub_base_PubHkqcÈ¡µÃÓ¦»¹¿îÆÚÊı;
*              32¡¢°üÀ¨Ä£¿épub_base_PubXmclÅĞ¶ÏÊÇ·ñÑ®Ä©;
*              33¡¢°üÀ¨Ä£¿épub_base_daynumSUB¼ÆËãÄ³Ò»ÈÕÆÚÒ»¶¨ÌìÊıÇ°µÄÈÕÆÚ;
*              34¡¢°üÀ¨Ä£¿épub_base_CalTrueDays¼ÆËãÌìÊı(Êµ¼ÊÌìÊı);
*              35¡¢°üÀ¨Ä£¿épub_base_CalDays¼ÆËãÌìÊı(¼ÆÏ¢ÌìÊıÀàĞÍ£¨0-Êµ¼ÊÌìÊı¡¢1-30Ìì£©);
*              36¡¢°üÀ¨Ä£¿épub_base_Get_EndAcmDayÈ¡½áÏ¢ÈÕÆÚº¯Êı
*              37¡¢°üÀ¨Ä£¿épub_base_CalMouths¼ÆËãÔÂÊı;
*              38¡¢°üÀ¨Ä£¿épub_base_daynumLAT¼ÆËãÄ³Ò»ÈÕÆÚÒ»¶¨ÌìÊıºóµÄÈÕÆÚ;
*              39. °üÀ¨Ä£¿épub_base_dayvalicheck¼ì²éÈÕÆÚºÏ·¨ĞÔ ;
*              40. °üÀ¨Ä£¿épub_base_iJumpHolidayÅĞ¶Ï´«ÈëµÄÈÕÆÚÊÇ·ñÎª½Ú¼ÙÈÕ£¬²¢µÃµ½Ò»¸ö·Ç·¨¶¨½Ú¼ÙÈÕµÄÈÕÆÚNEWBP7 ;
*              41. °üÀ¨Ä£¿épub_base_CaltureMouths¼ÆËãÔÂÊı;(ÉÏÃæµÄ37.pub_base_CalMouths¼ÆËã²»×¼È·)
* ×÷    Õß:
* ×÷    Õß:
* Íê³ÉÈÕÆÚ£º    2003Äê12ÔÂ16ÈÕ
* ĞŞ¸Ä¼ÇÂ¼£º
* ÈÕ    ÆÚ:
* ĞŞ ¸Ä ÈË:
* ĞŞ¸ÄÄÚÈİ:
*************************************************/

#include "stdio.h"
#include <time.h>
#define EXTERN
#include "public.h"
#include "com_sect_acm_c.h"
#include "com_holiday_c.h"
#include "com_parm_holiday_c.h"/**NEWBP7**/

#define L360 360
#define L30 30

int leapyear[12]={31,29,31,30,31,30,31,31,30,31,30,31};
int commyear[12]={31,28,31,30,31,30,31,31,30,31,30,31};

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_CalTriDays
 * º¯Êı¹¦ÄÜ£º   ¼ÆËãÌìÊı£¨°´30Ìì¼ÆËã£©
 * ×÷Õß/Ê±¼ä£º  
 *
 * ²ÎÊı£º   
 *     ÊäÈë: rq1,long    :ÆğÊ¼ÕûĞÍÈÕÆÚ
 *               rq2,long    :ÖÕÖ¹ÕûĞÍÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: iTs,int  :ÌìÊı
 * ×¢ÊÍ£º°´30Ìì¼ÆËã
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/

int pub_base_CalTriDays(long date1, long date2) /* ÆğÊ¼ÈÕ,  ÖÕÖ¹ÈÕ */
{
    short t_days=0,ymd1[3],ymd2[3],ymd3[3];
    int    iTs;
    long tmpts;
    long L10000=10000;
    int LONE=1;

    if (date1>=date2)
    {
        return 0;
    }

    ymd1[0]=(short)(date1/L100%L100);
    ymd1[1]=(short)(date1%L100);
    ymd1[2]=(short)(date1/L10000);  
    
    ymd2[0]=(short)(date2/L100%L100);
    ymd2[1]=(short)(date2%L100);
    ymd2[2]=(short)(date2/L10000);  
/*
    ymd3[0]=(short)(pubjx.opn_ac_date/L100%L100);
    ymd3[1]=(short)(pubjx.opn_ac_date%L100);
    ymd3[2]=(short)(pubjx.opn_ac_date/L10000);  
*/
    t_days=0;
    if (ymd1[1]>L30) ymd1[1]=L30;
    if (ymd2[1]>L30) ymd2[1]=L30;
    if (ymd3[1]>L30) ymd3[1]=L30;
    
    if (ymd2[0]==2)
    {
        if (rleapyear(ymd2[2]))            
        {
            if (ymd2[1]==29)
            {
                ymd2[1]=L30;
                if (ymd1[0]==2)
                {
                    if (rleapyear(ymd1[2]))
                    {
                        if (ymd1[1]>=28)
                            ymd1[1]++;
                    }
                    else if (ymd1[1]>=28)
                        ymd1[1]=L30;
                }
                else
                {
                    if (ymd1[1]>=29)
                        ymd1[1]=L30;
                }
            }
        }
        else if (ymd2[1]==28)
        {
            ymd2[1]=L30;
            if (ymd1[1]>=28)
                ymd1[1]=L30;
/*
            if (date1!=pubjx.opn_ac_date && ymd3[0]==2 && ymd3[1]==29)
                t_days=-LONE;
*/
        }
    }
    else if (ymd1[0]==2)
    {
        if (rleapyear(ymd1[2]) && ymd1[1]>=29)
        {
            if (ymd3[1]==L30 || ymd3[1]<29)
                t_days=-LONE;
        }
        else if (!rleapyear(ymd1[2]) && ymd1[1]>=28)
        {
            if (ymd3[1]==L30 || ymd3[1]<28)
                t_days=-2;
            else if (ymd3[1]==29)
                t_days=-LONE;
        }

    }
    iTs = (ymd2[2]-ymd1[2])*L360+(ymd2[0]-ymd1[0])*L30+(ymd2[1]-ymd1[1])+t_days;

    return (int)iTs;
}
/*
{
    short t_ts=0,ymd1[3],ymd2[3];
    int    iTs;

    if (rq1>=rq2)
    {
        return 0;
    }
    ymd1[0]=(short)(rq1/100%100);
    ymd1[1]=(short)(rq1%100);
    ymd1[2]=(short)(rq1/10000);  
    
    ymd2[0]=(short)(rq2/100%100);
    ymd2[1]=(short)(rq2%100);
    ymd2[2]=(short)(rq2/10000);  

    t_ts=0;
    if (ymd1[1]==31 && ymd2[1]!=31)
    {
        t_ts = 1;
    }
    if (ymd2[1]==31 && ymd1[1]!=31)
    {
        t_ts = -1;
    }
    if (ymd2[0]==2)
        if (!rleapyear(ymd2[2]) && ymd2[1]==28)
        {
            if (ymd1[1]>=30)
            {
                t_ts = 2;
            }
            else if (ymd1[1]==29)
            {
                t_ts = 1;
            }
        }
        else
            if (ymd2[1]==29 && ymd1[1]>=30)
            {
                t_ts+=1;
            }

    iTs = (ymd2[2]-ymd1[2])*L360+(ymd2[0]-ymd1[0])*L30+(ymd2[1]-ymd1[1])+t_ts;

    return (int)iTs;
}
*/
/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_CalTrueDays
 * º¯Êı¹¦ÄÜ£º   ¼ÆËãÌìÊı
 * ×÷Õß/Ê±¼ä£º  
 *
 * ²ÎÊı£º   
 *     ÊäÈë: begrq,long    :ÆğÊ¼ÕûĞÍÈÕÆÚ
 *             endrq,long    :ÖÕÖ¹ÕûĞÍÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: >=0   ³É¹¦
 *           <0    Ê§°Ü
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/

int pub_base_CalTrueDays(long begrq,long endrq)
{
    int i,a,leap,flag;
    int year;
    long arq=0;
    long brq=0;
    long date1=0;
    long date2=0;
    int *yn;
    if ( endrq<begrq )
    {
        flag=-1;
        date1=endrq;
        date2=begrq;
    }
    else
    {
        flag=1;
        date1=begrq;
        date2=endrq;
    }
    arq=date1/100;
    brq=date2/100;
    a=0;
vtcp_log("666666666666666666666666=[%ld]",arq);
vtcp_log("77777777777777777777777777=[%ld]",brq);
    while( arq<brq )
    {
        year=arq/100;
        if ( (year%4==0 && year%100!=0 ) || (year%400==0) )
        {
            yn=leapyear;
        }
        else
        {
            yn=commyear;
        }

        for( i=arq%100; i<=12; i++ )
        {
            a=a+*(yn+i-1);
            arq++;
            if ( i==12 ) arq=(arq/100+1)*100+01;
            if( arq>=brq ) break;
        }
    }

    if ( arq>brq )
    {vtcp_log("888888888888888888888888");
        a=a-date1%100-*(yn+i-1)+date2%100;
    }
    else
    {vtcp_log("99999999999999999999999999999");
        a=a-date1%100+date2%100;
    }

    return (a*flag);
}


/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_deadlineD
 * º¯Êı¹¦ÄÜ£º   ¼ÆËãµ½ÆÚÈÕ(°´ÈÕ£©
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: date1,long    :ÕûĞÍÈÕÆÚ
 *             num,long    :ÌìÊı
 *     Êä³ö: dqr,long *    :µ½ÆÚÈÕ
 *     ·µ»Ø: 0   ³É¹¦
 *           1   Ê§°Ü
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/

int pub_base_deadlineD(long date1,long num,long *dqr)
{
    short    y1,m1,d1,num1;
    long    flag=1;
    int *yn;

    num1=num;
    y1=date1/10000;
    m1=(date1/100)%100;
    d1=date1%100;
    num1+=d1;

    if(num1<0)
    {
        m1--;
        num1=-num1;
        flag=-1;
    }
    else if ( num1==0 )
    {
        m1--;
        flag=-1;
    }
    else
    {
        flag=1;
    }

    if (m1>12) {y1++; m1=m1-12;}      
    if (m1==0) {y1--; m1=12;}      

    while(1)
    {
        if ( (y1%4==0 && y1%100!=0 ) || (y1%400==0) )
        {
            yn=leapyear;
        }
        else
        {
            yn=commyear;
        }

        if ( num1>*(yn+m1-1) )
        {
            num1=num1-*(yn+m1-1);
            m1=m1+flag;
            if (m1>12) {y1++; m1=m1-12;}      
            if (m1==0) {y1--; m1=12;}      
        }
        else
        {
            if ( flag==-1 )
            {
                num1=*(yn+m1-1)-num1;
            }
            if ( num1==0 )
            {
                m1--;
                if (m1>12) {y1++; m1=m1-12;}      
                if (m1==0) {y1--; m1=12;}      
                continue;
            }
            break;
        }
    }

    *dqr=(long)(y1*10000+m1*100+num1);

    /*
    sprintf(acErrMsg,"¿ªÊ¼ÈÕÆÚ[%ld],ÌìÊı[%d],ÖÕÖ¹ÈÕÆÚ[%ld],",date1,num,*dqr);
    WRITEMSG
    */

    return 0;
}


/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_ChgJjr
 * º¯Êı¹¦ÄÜ£º   È·¶¨½Ú¼ÙÈÕÆğÊ¼ÖÕÖ¹ÈÕ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: 
 *     Êä³ö: 
 *     ·µ»Ø: 0   ³É¹¦
 *           1   Ê§°Ü
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_ChgJjr( int rq, int *qsrq, int *zzrq, char *xym)
{

}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_get_date
 * º¯Êı¹¦ÄÜ£º   È¡µÃÏµÍ³ÈÕÆÚ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë:
 *     Êä³ö:
 *     ·µ»Ø: 
 *     
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
void pub_base_get_date(int *year, int *month, int *day)
{
        time_t t;
        struct tm *local_time;

        t = time(0);
        local_time = localtime(&t);

        *year  = 1900 + local_time->tm_year;
        *month = local_time->tm_mon+1;
        *day   = local_time->tm_mday;

        return ;
}
/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_get_ymd(y,m,d,rq,bz)
 * º¯Êı¹¦ÄÜ£º   È¡µÃÄêÔÂÈÕ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long:ÕûĞÍÈÕÆÚ
 *           num,long       :ÔÂÊı
 *     Êä³ö: y,long   *:Äê
 *         : m,long   *:ÔÂ
 *         : d,long   *:ÈÕ
 *         : bz,long   *:±êÖ¾:  0:ÆÕÍ¨ 1:Ñ®³õ2:Ñ®Ä© 3:ÔÂ³õ4:ÔÂÄ©5:¼¾³õ
 *                              6:¼¾Ä©7:°ëÄê³õ8:°ëÄêÄ© 9:Äê³õ10:ÄêÄ©
 *     ·µ»Ø: 0   ³É¹¦
 *           1   Ê§°Ü
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/

int pub_base_get_ymd( long *y, long *m, long *d, long rq, long *bz)
{
 *y=rq/10000;
 *m=rq/100%100;

 *d=rq%100;
 *bz=0;

 if(*m>12||*m<1) {
       return 1;
      }
 if(*d==11||*d==21) *bz=1;
 if(*d==10||*d==20) *bz=2;
 if(*d==1) *bz=3;
 if(*m==1||*m==3||*m==5||*m==7||*m==8||*m==10||*m==12)
           {
                if(*d<1||*d>31) return 1;
                if(*d==1 && *m==10) *bz=5;
                if(*d==1 && *m==7) *bz=7;
                if(*d==1 && *m==1) *bz=9;
                if(*d==31) *bz=4;
                if(*d==31 && *m==3) *bz=6;
                if(*d==31 && *m==12) *bz=10;
        }
        else if(*m==4||*m==6||*m==9||*m==11)
        {
                if(*d<1||*d>30) return 1;
                if(*d==1&&*m==4) *bz=5;
                if(*d==30) *bz=4;
                if(*d==30&&*m==9) *bz=6;
                if(*d==30&&*m==6) *bz=8;
        }
        else
        {
                if(*d==11||*d==21) *bz=1;
                if(*d==10||*d==20) *bz=2;
                if(*d==1) *bz=3;
                if(*d==1&&*m==4) *bz=5;
                if(!pub_base_chk_leap(*y))
                {
                        if(*d<1||*d>29) return 1;
                        if(*d==29) *bz=4;
                }
                else
                {
                        if(*d<1||*d>28) return 1;
                        if(*d==28) *bz=4;
                }
        }
        return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_deadlineM(date1,num,dqr)
 * º¯Êı¹¦ÄÜ£º   ¼ÆËãµ½ÆÚÈÕÆÚ(°´ÔÂ)
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: date1,long     :ÕûĞÍÈÕÆÚ
 *           num,long       :ÔÂÊı
 *     Êä³ö: dqr,long *     :µ½ÆÚÈÕ
 *     ·µ»Ø: 0   ³É¹¦
 *           1   Ê§°Ü
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/

int pub_base_deadlineM( long date1, long num, long *dqr)
{
 short   y1,m1,d1,num1;
 long    date2;
 char    ss[20];

 num1=num;
 y1=date1/10000;
 m1=(date1/100)%100;
 d1=date1%100;
 num1+=m1;
 y1=y1+num1/12;
 m1=num1%12;
 if(m1==0)
        {
                m1=12;
                y1--;
        }
        else if(m1<0)
        {
                m1=m1+12;
                y1--;
        }
        if(m1==2)
        {
                if ( (y1%4==0 && y1%100!=0 ) || (y1%400==0) )
                {
                        if(d1>29) d1=29;
                }
                else
                {
                        if(d1>28) d1=28;
                }
        }
        else if ( (m1==4)||(m1==6)||(m1==9)||(m1==11) )
        {
                if(d1>30) d1=30;
        }

        *dqr=(long)(y1*10000+m1*100+d1);
        return(0);
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_chk_leapÅĞ¶ÏÊÇ·ñÈòÄê
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÊÇ·ñÈòÄê
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: y,long :Äê
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: 0:ÈóÄê 1:·ÇÈóÄê
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_chk_leap(long y)
{
        long    d,d1,d2;

        d=y%4;
        d1=y%100;
        d2=y%400;

        if(d==0&&d1!=0)
        {
                return 0;
        }
        else if(d==0&&d1==0&&d2==0)
        {
                return 0;
        }

        return 1;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_day_week(long rq)ê
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏĞÇÆÚ¼¸£¬ÅĞ¶ÏÒ»ÖÜÖĞµÄµÚ¼¸Ìì
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long:ÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: Ò»ÖÜµÄµÚ¼¸Ìì
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_day_week(long rq)
{
        long    ts;
        long    xq;

        ts=pub_base_CalTrueDays(19000101,rq);
        if(ts<0) return -1;
        ts++;
        xq=ts%7;
        if(xq==0) xq=99;

        return xq;
}
/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_day_xun
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÊÇÒ»Ñ®µÄµÚ¼¸Ìì
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: Ò»Ñ®µÄµÚ¼¸Ìì
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_day_xun(long rq)
{
        long    ts;
        long    y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        if(d>10&&d<=20)
        {
                d=d-10;
        }
        else if(d>20)
        {
                d=d-20;
        }
        if(bz==2||bz==4||bz==6||bz==8||bz==10)
        {
                d=99;
        }

        return d;
}


/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_end_xun
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÊÇ·ñÑ®Ä©
 * ×÷Õß/Ê±¼ä£º    Terry.Cui // 2005/1/16
 *
 * ²ÎÊı£º
 *     ÊäÈë: rqÈÕÆÚ
 *     Êä³ö:
 *     ·µ»Ø: 0ÊÇ        -1·ñ
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/

int pub_base_end_xun(long  sys_date)
{
     long    y1,m1,d1,t_xcrq,bz;
    int    ret;
    
        if ( pub_base_get_ymd(&y1,&m1,&d1,sys_date,&bz) ) return 1;

        if ( d1 == 10 || d1 == 20 )    return(0);
        
        /* ÅĞ¶ÏÊÇ·ñÔÂÄ©£¬Èç¹ûÊÇÔÂÄ©£¬ÔòÒ²ÊÇÑ®Ä© */
        ret = pub_base_end_month( sys_date );
        if ( !ret )        return(0);

        return(-1);
}


/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_day_month
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÊÇÒ»ÔÂµÄµÚ¼¸Ìì
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: Ò»ÔÂµÄµÚ¼¸Ìì
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_day_month(long rq)
{
        long    y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;

    return d;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_end_month
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÊÇÔÂÄ©
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: 0ÔÂÄ© £­1²»ÊÇÔÂÄ©
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_end_month(long sys_date)
{
    long    next_date;
    long    ny, nm, nd, nbz ;
        long    y,m,d,bz;
    int    ret;

    ret = pub_base_deadlineD( sys_date, 1, &next_date );
    if( ret )
    {
        sprintf( acErrMsg, "¼ÆËãÈÕÆÚ´í" );
        WRITEMSG
        return(-1);
    }    

    if ( pub_base_get_ymd ( &ny, &nm, &nd, next_date, &nbz ) )
    {
         return -1;
    }

        if( pub_base_get_ymd( &y, &m, &d, sys_date, &bz ) ) 
    {
        return -1;
    }

    if ( (m%12+1) == nm )
    {
        return(0);
    }

    return(-1);
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_end_year
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÊÇ·ñÄêÄ©
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: sys_dateÈÕÆÚ
 *     ·µ»Ø: 0³É¹¦
             ·Ç0Ê§°Ü
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_end_year(long sys_date)
{
    long    next_date;
    long    ny, nm, nd, nbz ;
        long    y,m,d,bz;
    int    ret;


vtcp_log("end_year[%d]",sys_date);
    ret = pub_base_deadlineM( sys_date, 1, &next_date );
    if( ret )
    {
        sprintf( acErrMsg, "¼ÆËãÈÕÆÚ´í" );
        WRITEMSG
        return(-1);
    }    
vtcp_log("next_year[%d]",next_date);

    if ( pub_base_get_ymd ( &ny, &nm, &nd, next_date, &nbz ) )
    {
         return -1;
    }

        if( pub_base_get_ymd( &y, &m, &d, sys_date, &bz ) ) 
    {
        return -1;
    }

    if ( y+1 == ny )
    {
vtcp_log("-----[%d][%d]",y,ny);
        return(0);
    }

    return(-1);
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_day_quarter
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÊÇÒ»¼¾µÄµÚ¼¸Ìì
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: Ò»¼¾µÄµÚ¼¸Ìì
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_day_quarter(long rq)
{
        long    ts,rq1;
        long    y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        ts=(m-1)/3;
        m=ts*3+1;
        rq1=y*10000+m*100+1;
        ts=pub_base_CalTrueDays(rq1,rq);
        if(ts<0) return -1;
        d=ts;
        if(bz==6||bz==8||bz==10)
        {
                d=99;
        }

        return d;
}
/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_day_half
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶Ï°ëÄêµÄµÚ¼¸Ìì
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: °ëÄêµÄµÚ¼¸Ìì
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_day_half(long rq)
{
        long    ts,rq1;
        long    y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        ts=(m-1)/6;
        m=ts*6+1;
        rq1=y*10000+m*100+1;
        ts=pub_base_CalTrueDays(rq1,rq);
        if(ts<0) return -1;
        d=ts;
        if(d==99) return 0;
        if(bz==8||bz==10)
        {
                d=99;
        }
        if(d>99) return 0;

        return d;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_day_year
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÄêµÄµÚ¼¸Ìì
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ÄêµÄµÚ¼¸Ìì
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_day_year(long rq)
{
 long    rq1,y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        rq1=y*10000+101;
        d=pub_base_CalTrueDays(rq1,rq);
        if(d<0) return -1;
        if(d==99) return 0;
        if(bz==10)
        {
                d=99;
        }
        if(d>99) return 0;

        return d;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_week_month
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÔÂµÄµÚ¼¸ÖÜ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ÔÂµÄµÚ¼¸ÖÜ
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_week_month(long rq)
{
 long    y,m,d,rq1,bz,xq,xq1;
 long    y1,m1,d1;

 if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
 rq1=y*10000+m*100+1;
 xq=pub_base_day_week(rq1);
 if(xq<0) return -1;
 if(xq!=1)
 {
 if(xq==99) xq=7;
 xq=7-xq+2;
 if(d<xq) return -1;
        }

        if(pub_base_deadlineM(rq1,1,&rq1)<0) return -1;
        if(pub_base_deadlineD(rq1,1,&rq1)<0) return -1;
        xq1=pub_base_day_week(rq1);
        if(xq1<0) return -1;
        if(pub_base_get_ymd(&y1,&m1,&d1,rq1,&bz)) return -1;
        if(xq1!=99)
        {
                d1=d1-xq1;
                if(d>d1) return -1;
        }

        xq1=(d-xq)/7;
        xq1=xq1+1;

        return xq1;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_week_quarter
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶Ï¼¾µÄµÚ¼¸ÖÜ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ¼¾µÄµÚ¼¸ÖÜ
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_week_quarter(long rq)
{
        long    y,m,d,rq1,bz,xq,xq1,jq,jq1;
        long    y1,m1,d1,rq2;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        m1=(m-1)/3;
        m=m1*3+1;
        rq1=y*10000+m*100+1;
        xq=pub_base_day_week(rq1);
        if(xq<0) return -1;
        jq=pub_base_CalTrueDays(rq1,rq);
        if(jq<0) return -1;
        if(xq!=1)
        {
                if(xq==99) xq=7;
                xq=7-xq+2;
                if(jq<xq) return -1;
        }

        if(pub_base_deadlineM(rq1,3,&rq2)<0) return -1;
        if(pub_base_deadlineD(rq1,1,&rq2)<0) return -1;
        xq1=pub_base_day_week(rq2);
        if(xq1<0) return -1;
        jq1=pub_base_CalTrueDays(rq1,rq2);
        if(jq1<0) return -1;
        if(xq1!=99)
        {
                jq1=jq1-xq1;
                if(jq>jq1) return -1;
        }

        xq1=(jq-xq)/7;
        xq1=xq1+1;

        return xq1;
}
/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_week_half
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶Ï°ëÄêµÄµÚ¼¸ÖÜ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: °ëÄêµÄµÚ¼¸ÖÜ
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_week_half(long rq)
{
        long    y,m,d,rq1,bz,xq,xq1,hq,hq1;
        long    y1,m1,d1,rq2;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        m1=(m-1)/6;
        m=m1*6+1;
        rq1=y*10000+m*100+1;
        xq=pub_base_day_week(rq1);
        if(xq<0) return -1;
        hq=pub_base_CalTrueDays(rq1,rq);
        if(hq<0) return -1;
        if(xq!=1)
        {
                if(xq==99) xq=7;
                xq=7-xq+2;
                if(hq<xq) return -1;
        }

        if(pub_base_deadlineM(rq1,6,&rq2)<0) return -1;
        if(pub_base_deadlineD(rq1,1,&rq2)<0) return -1;
        xq1=pub_base_day_week(rq2);
        if(xq1<0) return -1;
        hq1=pub_base_CalTrueDays(rq1,rq2);
        if(hq1<0) return -1;
        if(xq1!=99)
        {
                hq1=hq1-xq1;
                if(hq>hq1) return -1;
        }

        xq1=(hq-xq)/7;
        xq1=xq1+1;

        return xq1;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_week_year
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÄêµÄµÚ¼¸ÖÜ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ÄêµÄµÚ¼¸ÖÜ
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
 int pub_base_week_year(long rq)
{
        long    y,m,d,rq1,bz,xq,xq1,yq,yq1;
        long    y1,m1,d1,rq2;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        rq1=y*10000+101;
        rq2=y*10000+1231;
        xq=pub_base_day_week(rq1);
        if(xq<0) return -1;
        yq=pub_base_CalTrueDays(rq1,rq);
        if(yq<0) return -1;
        if(xq!=1)
        {
                if(xq==99) xq=7;
                xq=7-xq+2;
                if(yq<xq) return -1;
        }

        xq1=pub_base_day_week(rq2);
        if(xq1<0) return -1;
        yq1=pub_base_CalTrueDays(rq1,rq2);
        if(yq1<0) return -1;
        if(xq1!=99)
        {
                yq1=yq1-xq1;
                if(yq>yq1) return -1;
        }

        xq1=(yq-xq)/7;
        xq1=xq1+1;

        return xq1;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_xun_month
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÔÂµÄµÚ¼¸Ñ®
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ÔÂµÄµÚ¼¸Ñ®
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_xun_month(long rq)
{
        long    xq,y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        if(d<=10) xq=1;
        else if(10<d<=20) xq=2;
        else if(d>20) xq=3;

        return xq;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_xun_quarter
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶Ï¼¾¶ÈµÄµÚ¼¸Ñ®
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ¼¾¶ÈµÄµÚ¼¸Ñ®
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
 int pub_base_xun_quarter(long rq)
{
        long    jq,y,m,d,bz,jq1;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        jq1=pub_base_month_quarter(rq);
        if(jq1<0) return -1;
        jq=pub_base_xun_month(rq);
        jq=jq1*(m-1)+jq;

        return jq;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_xun_half
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶Ï°ëÄêµÄµÚ¼¸Ñ®
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: °ëÄêµÄµÚ¼¸Ñ®
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_xun_half(long rq)
{
        long    hq,y,m,d,bz,hq1;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        hq1=pub_base_month_half(rq);
        if(hq1<0) return -1;
        hq=pub_base_xun_month(rq);
        hq=(hq1-1)*3+hq;

        return hq;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_xun_year
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÄêµÄµÚ¼¸Ñ®
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ÄêµÄµÚ¼¸Ñ®
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_xun_year(long rq)
{
        long    yq,y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        yq=pub_base_xun_month(rq);
        yq=(m-1)*3+yq;

        return yq;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_month_quarter
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶Ï¼¾¶ÈµÄµÚ¼¸ÔÂ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ¼¾¶ÈµÄµÚ¼¸ÔÂ
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_month_quarter(long rq)
{
        long    jq,y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        jq=m%3;
        if(jq==0) jq=3;

        return jq;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_month_half
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶Ï°ëÄêµÄµÚ¼¸ÔÂ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: °ëÄêµÄµÚ¼¸ÔÂ
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_month_half(long rq)
{
        long    hq,y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        hq=m%6;
        if(hq==0) hq=6;

        return hq;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_month_year
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÄêµÄµÚ¼¸ÔÂ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ÄêµÄµÚ¼¸ÔÂ
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_month_year(long rq)
{
        long    y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;

        return m;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_quarter_half
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶Ï°ëÄêµÄµÚ¼¸¼¾¶È
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: °ëÄêµÄµÚ¼¸¸ö¼¾¶È
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_quarter_half(long rq)
{
        long    hq,y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        hq=m%6;
        hq=(hq-1)/3;
        hq=hq+1;

        return hq;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_quarter_year
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÄêµÄµÚ¼¸¼¾¶È
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ÄêµÄµÚ¼¸¸ö¼¾¶È
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_quarter_year(long rq)
{
        long    yq,y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        yq=(m-1)/3;
        yq=yq+1;

        return yq;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_half_year
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÇ°°ëÄê»¹ÊÇºó°ëÄê
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rq,long£ºÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ÄêµÄÇ°°ëÄê:1  ºó°ëÄê:2
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_half_year(long rq)
{
        long    yq,y,m,d,bz;

        if(pub_base_get_ymd(&y,&m,&d,rq,&bz)) return -1;
        yq=(m-1)/6;
        yq=yq+1;

        return yq;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_date_jjr
 * º¯Êı¹¦ÄÜ£º   È¡½Ú¼ÙÈÕÎÄ¼ş
 * ×÷Õß/Ê±¼ä£º  rob/2005/03/02
 *
 * ²ÎÊı£º
 *     ÊäÈë: 
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: 0 ³É¹¦¡¡!0 Ê§°Ü
 *           100 Î´²éµ½ÎÄ¼ş
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_date_jjr(long rq,struct com_holiday_c *com_holiday)
{
    int ret;
 
    memset(com_holiday, 0x0, sizeof(struct com_holiday_c));
    ret = Com_holiday_Sel(g_pub_tx.reply,com_holiday,
       "beg_date <= %ld and end_date >= %ld",rq,rq);
    if ( ret != 0 )
    {
        sprintf(acErrMsg,"È¡½Ú¼ÙÈÕÎÄ¼ş´íÎó!!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P221");
        return ret;
    }
    return 0;
}



/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_PubAjTs
 * º¯Êı¹¦ÄÜ£º   ¼ÆËã°´½Ò´û¿îÌìÊı
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: date1ÆğÊ¼ÈÕÆÚ
 *           date2µ½ÆÚÈÕ
 *           qsµ±Ç°ÆÚÊı
 *           zqs×ÜÆÚÊı
 *           jxr¼ÆÏ¢ÈÕ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: °´½Ò´û¿îÌìÊı
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_PubAjTs( long date1, long date2, long qs, long zqs, long jxr, char xym[])
{
 int     y1,m1,d1,i,y2,m2,d2,tmpm1,tmpm2;
 long    tmpdate1,tmpdate2,num,num1;
 char    datestr1[9],datestr2[9];

 num=0;

 datestr1[0]='\0';
 datestr2[0]='\0';
 sprintf(datestr1,"%ld",date1);
 sprintf(datestr2,"%ld",date2);
 if ( strcmp(datestr1,datestr2)>0 )
 {
                if(date1>date2){
                        strcpy(xym,"5M44");
                        return -1;
                }
        }
        y1=date1/10000;
        y2=date2/10000;
        d1=date1%100;
        d2=date2%100;
        m1=(date1/100)%100;
        m2=(date2/100)%100;
        if(y1==y2&&m1==m2)
        {
                num=d2-d1;
        }
        else if(zqs==0)
        {
                tmpm1=m1+1;
                if(tmpm1>12)
                {
                        tmpdate1=(y1+1)*10000+(tmpm1-12)*100+1;
                }
                else
                {
                        tmpdate1=y1*10000+tmpm1*100+1;
                }
                num=0;
                num=num+pub_base_CalTrueDays(date1,tmpdate1);
                tmpdate2=y2*10000+m2*100+1;
                num=num+pub_base_CalTrueDays(tmpdate2,date2);
                num1=pub_base_CalTriDays(tmpdate1,tmpdate2);
                if(num1!=0) num1=num1+1;
                num=num+num1;
        }
        else if(qs==1)
        {
                tmpm1=m1+1;
                if(tmpm1>12)
                {
                        tmpdate1=(y1+1)*10000+(tmpm1-12)*100+1;
                }
                else
                {
                        tmpdate1=y1*10000+tmpm1*100+jxr;
                }
                num=pub_base_CalTrueDays(date1,tmpdate1);
        }
        else if(qs==zqs)
        {
                tmpm2=m2-1;
                if(m2<=0)
                {
                        tmpdate2=(y2-1)*10000+(tmpm2+12)*100+jxr;
                }
                else
                {
                        tmpdate2=y2*10000+tmpm2*100+jxr;
                }
                num=pub_base_CalTrueDays(tmpdate2,date2);
                num--;
        }
        else
        {
                num=30;
        }

        return num;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_PubHkqc
 * º¯Êı¹¦ÄÜ£º   È¡µÃÓ¦»¹¿îÆÚÊı
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: khrq¿ª»§ÈÕÆÚ
         dqqcµ±Ç°ÆÚ´Î
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: Ó¦»¹¿îÆÚÊı
 *
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/

int pub_base_PubHkqc(long khrq,long dqqc)
{
        long    dqrq;
        long    rq;

        rq=khrq%100;
        if(rq>15)
        {
                dqqc++;
        }
        pub_base_deadlineM(khrq,dqqc,&dqrq);
        dqrq=dqrq/100;

        return dqrq;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_PubXmcl
 * º¯Êı¹¦ÄÜ£º   ÅĞ¶ÏÊÇ·ñÑ®Ä©
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: rqÈÕÆÚ
 *     Êä³ö: xcrqÑ®³õÈÕÆÚ
 *     ·µ»Ø: 0³É¹¦
             ·Ç0Ê§°Ü
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/

int pub_base_PubXmcl(long  rq,long  *xcrq)
{
 long    y1,m1,d1,t_xcrq,bz;

    if ( pub_base_get_ymd(&y1,&m1,&d1,rq,&bz) ) return 1;
    printf("y1=[%d] m1=[%d] d1=[%d] bz=[%d] \n",y1,m1,d1,bz);
        if ( bz!=2 && bz!=4 && bz!=6 && bz!=8 && bz!=10)
        {
                return 10;
        }
        if ( d1 <= 10 ) t_xcrq = y1*10000+m1*100+1;
        if ( d1 >= 11 && d1 <= 20 ) t_xcrq = y1*10000+m1*100+11;
        if ( d1 >= 21 && d1 <= 31 ) t_xcrq = y1*10000+m1*100+21;
    printf("t_xcrq=[%d]\n",t_xcrq);
    /*      *xcrq = chag_ymd(t_xcrq,0,0,-1);        */
    printf("xcrq=[%d]\n",xcrq);

    return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_daynumSUB
 * º¯Êı¹¦ÄÜ£º   ¼ÆËãÄ³Ò»ÈÕÆÚÒ»¶¨ÌìÊıÇ°µÄÈÕÆÚ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊı£º
 *     ÊäÈë: date1ÈÕÆÚ
             num ÌìÊı
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: ÈÕÆÚ 
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
long pub_base_daynumSUB(long  l_date,int  num)/*ÈÕÆÚ2µÈÓÚÈÕÆÚ1-ÆÚÏŞ(Ìì)*/
{
	short y1,m1,d1,num1;
	long iDate;
	num1=num;
	y1=l_date/10000;m1=(l_date/100)%100;d1=l_date%100;
	num1=d1-num1;
	while(num1<1)
	{
		if((m1==2)||(m1==4)||(m1==6)||(m1==8)||(m1==9)||(m1==11)||(m1==1)){
			m1--;
			num1=31+num1;
			if(m1==0){
				m1=12;y1=y1-1;
			}
		}else if((m1==5)||(m1==7)||(m1==10)||(m1==12)) {
			m1--;
			num1=30+num1;
		} else if(m1==3) {
			if(((y1%4==0&&y1%100!=0)||(y1%400==0))&&(m1==3)) {
				m1--;num1=29+num1;
			} else {
				m1--;num1=28+num1;
			}
		}
	}
	iDate=y1*10000+m1*100+num1;
	return(iDate);
}
/*long pub_base_daynumSUB(long date1, int num)ÈÕÆÚ2µÈÓÚÈÕÆÚ1-ÆÚÏŞ(Ìì)
{
    short   y1,m1,d1,num1;
    long    date2;
    char  str1[8];
    vtcp_log("[%s][%d] date1=[%ld]\n",__FILE__,__LINE__,date1);
        num1=num;
        y1=date1/10000;m1=(date1/100)%100;d1=date1%100;
        num1=d1-num1;
     if((m1==2)||(m1==4)||(m1==6)||(m1==8)||(m1==9)||(m1==11)||(m1==1))
        while(num1<1)
      {    m1--;
          num1=31+num1;
         if(m1==0){ m1=12;y1=y1-1;}
      }
     if((m1==5)||(m1==7)||(m1==10)||(m1==12))
      while(num1<1)
     {    m1--;num1=30+num1;
     }
    if(m1==3)
       while (num1 <1)
       {
         if ( ( ( y1%4==0 && y1%100!=0 )||(y1%400==0)) && (m1==3))
         {
             m1--; num1=29+num1;
         }
        else
         {
             m1--; num1=28+num1;
         }
       }
        date2=y1*10000+m1*100+num1;
    vtcp_log("[%s][%d] date2=[%ld]\n",__FILE__,__LINE__,date2);
        return(date2);
}*/

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_getyear()
 * º¯Êı¹¦ÄÜ£º   È¡ÈÕÆÚÖĞµÄÄê
 * ×÷Õß/Ê±¼ä£º  rob 2005/01/10
 *
 * ²ÎÊı£º
 *     ÊäÈë:  date ÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø:  year Äê
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_getyear(long date)
{   
    int year;
    
    year = floor(date / 10000);
    return year;
}
/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_getmonth()
 * º¯Êı¹¦ÄÜ£º   È¡ÈÕÆÚÖĞµÄÔÂ
 * ×÷Õß/Ê±¼ä£º  rob 2005/01/10
 *
 * ²ÎÊı£º
 *     ÊäÈë:  date ÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø:  month ÔÂ
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_getmonth(long date)
{
    int month;
    int year;
    
    year = floor(date / 10000);
    year = year * 10000;
    month = floor((date - year) / 100);   

    return month;
}


/**********************************************************************
* º¯ Êı Ãû£º  pub_base_CalDays
* º¯Êı¹¦ÄÜ£º  ¼ÆËãÌìÊı(¼ÆÏ¢ÌìÊıÀàĞÍ£¨0-Êµ¼ÊÌìÊı¡¢1-30Ìì£©)
* ×÷    Õß£º  lance
* Íê³ÉÊ±¼ä£º  2005Äê01ÔÂ14ÈÕ
*
* ²Î    Êı£º 
*     Êä  Èë£ºbeg_date   long      ÆğÊ¼ÈÕÆÚ    
*                end_date   long      ÖÕÖ¹ÈÕÆÚ 
*          type     char      ¼ÆÏ¢ÌìÊıÀàĞÍ£¨0°´Êµ¼ÊÌìÊı¡¢1°´30Ìì¼ÆËã£©
*
*     Êä  ³ö: days       long      ÌìÊı
*
*     ·µ»ØÖµ£º 0 ³É¹¦
*            ·Ç0 Ê§°Ü
*
* ĞŞ¸ÄÀúÊ·£º
*                   
********************************************************************/
int pub_base_CalDays( long beg_date , long end_date , char type[2] , long *days )
{
    vtcp_log("[%s][%d]beg_date=[%ld],end_date=[%ld],type=[%s]",__FILE__,__LINE__,beg_date,end_date,type);
    if ( type[0]=='0' )
    {
        *days = pub_base_CalTrueDays(beg_date, end_date);
    }
    else if ( type[0]=='1' )
    {
        *days = pub_base_CalTriDays(beg_date, end_date);
    }
    return 0;
}


/**********************************************************************
* º¯ Êı Ãû£º  pub_base_Get_EndAcmDay
* º¯Êı¹¦ÄÜ£º  È¡½áÏ¢ÈÕÆÚº¯Êı
* ×÷    Õß£º  lance
* Íê³ÉÊ±¼ä£º  2005Äê01ÔÂ14ÈÕ
*
* ²Î    Êı£º  
*     Êä  Èë£ºintst_mon   int      ¼ÆÏ¢ÔÂ·İ   
*                intst_day   int      ¼ÆÏ¢ÈÕ£¨È¡×Ô²úÆ·²ÎÊıÎÄ¼ş£©
*      
*     Êä  ³ö: ½áÏ¢ÈÕÆÚ È«¾Ö±äÁ¿g_pub_tx.next_intst_date
*
*     ·µ»ØÖµ£º 0 ³É¹¦
*            ·Ç0 Ê§°Ü
*
* ĞŞ¸ÄÀúÊ·£º
*                   
********************************************************************/
int pub_base_Get_EndAcmDay( int intst_mon , int intst_day )
{    
    int year;
    long EndDay;    /* ½áÏ¢ÈÕÆÚµÄÏÂÒ»Ìì */
    
    year = g_pub_tx.tx_date/10000;
    
    if ( intst_day!=99 )
        {
            g_pub_tx.next_intst_date = year*10000+intst_mon*100+intst_day;
        }
    if ( intst_day==99)
        {
            if ( intst_mon ==12 )
                {
                    EndDay = (year+1)*10000+1*100+1;
                }else{
                    EndDay = year*10000+1*100+1;
                     }
            pub_base_deadlineD( EndDay , -1 , &g_pub_tx.next_intst_date );
        }
                         
    return 0;    
    
}

        

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_CalIntstDays
 * º¯Êı¹¦ÄÜ£º   ¼ÆËãÌìÊı£
 * ×÷Õß/Ê±¼ä£º  
 *
 * ²ÎÊı£º   
 *     ÊäÈë: rq1,long    :ÆğÊ¼ÕûĞÍÈÕÆÚ
 *               rq2,long    :ÖÕÖ¹ÕûĞÍÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: iTs,int  :ÌìÊı
 * ×¢ÊÍ£º°´30Ìì¼ÆËã
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_CalIntstDays( long beg_date , long end_date , char type[2] , long *days )
{
    if ( type[0]=='0' )
    {
        *days = pub_base_CalTrueDays(beg_date, end_date);
        if (*days <0)
        {
            *days = 0;
        }
    }
    else if ( type[0]=='1' )
    {
        *days = pub_base_CalTriDays(beg_date, end_date);
    }
    return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_CalMouths
 * º¯Êı¹¦ÄÜ£º   ¼ÆËãÔÂÊı
 * ×÷Õß/Ê±¼ä£º  
 *
 * ²ÎÊı£º   
 *     ÊäÈë: rq1,long    :ÆğÊ¼ÕûĞÍÈÕÆÚ
 *             rq2,long    :ÖÕÖ¹ÕûĞÍÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: >=0   ³É¹¦
 *           <0    Ê§°Ü
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_CalMouths( long rq1, long rq2)
{
short t_ys,ymd1[3],ymd2[3],ymd3[3];
int iYs;
if (rq1>rq2) return 0;

    ymd1[0]=(short)(rq1/100%100);
    ymd1[1]=(short)(rq1%100);
    ymd1[2]=(short)(rq1/10000);

    ymd2[0]=(short)(rq2/100%100);
    ymd2[1]=(short)(rq2%100);
    ymd2[2]=(short)(rq2/10000);

    iYs = (ymd2[2]-ymd1[2])*12+(ymd2[0]-ymd1[0]);

    return (int)iYs;
}
/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_checkdate
 * º¯Êı¹¦ÄÜ£º   ¼ì²éÈÕÆÚºÏ·¨ĞÔ
 * ×÷Õß/Ê±¼ä£º  
 *
 * ²ÎÊı£º   
 *     ÊäÈë: date,long    :ÈÕÆÚ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: >=0   ³É¹¦
 *           <0    Ê§°Ü
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_checkdate( long date )
{
   char date_tmp[5] ,buf[9];
   int i,j,h ;

   i=0; j=0; h=0;
   memset(date_tmp,'\0',5);
   sprintf(buf, "%ld" ,date) ;
   if (strlen(buf)!=8) return 1;
   date_tmp[0]=buf[0] ;
   date_tmp[1]=buf[1] ;
   date_tmp[2]=buf[2] ;
   date_tmp[3]=buf[3] ;
   date_tmp[4]='\0';
   h=atoi(date_tmp) ;
   if ( h==0 ) return 1 ;

   memset(date_tmp,'\0',5);
   date_tmp[0]=buf[4] ;
   date_tmp[1]=buf[5] ;
   date_tmp[2]='\0' ;
   i=atoi(date_tmp) ;
   if ( (i<1) || (i>12) )
        return 1 ;
   memset(date_tmp,'\0',4);
   date_tmp[0]=buf[6] ;
   date_tmp[1]=buf[7] ;
   date_tmp[2]='\0' ;
   j=atoi(date_tmp);
   if ( (j<1) || (j>31) )
       return 1 ;
   if ( ( (i==2) || (i==4) || (i==6) || (i==9) || (i==11) ) && (j==31) )
       return 1 ;
   if ( (i==2) && (j==30) && (h%400) )  return 1 ;

   if ( ( ( h%4==0 && h%100!=0 ) || ( h%400==0 ) ) ){
   if ( (i==2) && (j==30) )  return 1 ;}
   else{
      if((i==2)&&(j==29)) return 1;}
   return 0 ;
}

/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_MonDays
 * º¯Êı¹¦ÄÜ£º   ¼ÆËãÔÂÌìÊı
 * ×÷Õß/Ê±¼ä£º  
 *
 * ²ÎÊı£º   
 *     ÊäÈë: ÄêÔÂ
 *     Êä³ö: ÎŞ
 *     ·µ»Ø: >0ÌìÊı <0 ´íÎó
 * ĞŞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_MonDays( long month )
{
    long y,m;

    y = month / 100;
    m = month % 100;

    switch( m )
    {
        case 1: case 3: case 5: case 8: case 10: case 12:
            return 31;
            break;
        case 4: case 6: case 7: case 9: case 11:
            return 30;
            break;
        case 2:
            if(!pub_base_chk_leap(y))
                return 29;
            else
                return 28;
            break;
        default: break;
    }

    return -1 ;
}
/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_daynumLAT                                    *
 * º¯Êı¹¦ÄÜ£º   ¼ÆËãÄ³Ò»ÈÕÆÚÒ»¶¨ÌìÊıºóµÄÈÕÆÚ                          *
 * ×÷Õß/Ê±¼ä£º  xyy 2006-10-12 10:38                                  * 
 *                                                                    *
 * ²ÎÊı£º                                                             *
 * ÊäÈë: date1ÈÕÆÚ                                                    *
         num ÌìÊı                                                     *
 * Êä³ö: ÎŞ                                                           *
 * ·µ»Ø: date2ÈÕÆÚ                                                    *
 * ĞŞ¸ÄÀúÊ·£º                                                         * 
 *                                                                    *
 *********************************************************************/

long pub_base_daynumLAT(char *date, int num)/*ÈÕÆÚ2µÈÓÚÈÕÆÚ1+ÆÚÏŞ(Ìì)*/
{
    short   y1,m1,d1,num1;
    long   date1,date2;
    char   cDate[9];
    int i=0,ret=0;
    memset(cDate,0,sizeof(cDate));

    date1 = apatoi(date,8);

    vtcp_log("[%s][%d]ÕâÀï¿ªÊ¼¼ì²éÈÕÆÚµÄÕıÈ·ĞÔ!\n",__FILE__,__LINE__);
    ret = pub_base_dayvalicheck(date1); 
    if (ret)
    {
        vtcp_log("[%s][%d]date1==[%d]\n",__FILE__,__LINE__,date1);
        return(NULL);
    }
    num1=num;
    y1=date1/10000;
    m1=(date1/100)%100;
    d1=date1%100;
    vtcp_log("[%s][%d]d1==[%ld]\n",__FILE__,__LINE__,d1);
    num1=d1+num1;
    vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
    vtcp_log("[%s][%d]num==[%ld]\n",__FILE__,__LINE__,num);
    if (num<0)
    {
        vtcp_log("[%s][%d]num1==[%d]\n",__FILE__,__LINE__,num1);
        while (num1 <0)
        {
            if (((m1==5)||(m1==7)||(m1==10)||(m1==12)) && num1!=0)
            {
                vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
                vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
                if (num1<=-30)
                {
                    i++;
                    m1--;
                    num1=num1+30;
                }
                else if ( num1 <0)
                {
                    if (-num > d1)
                    {
                        m1--;
                    }
                    num1=num1+30;
                }
                vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
                vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
            }
            if (((m1==1)||(m1==2)||(m1==4)||(m1==6)||(m1==8)||(m1==9)||(m1==11)) && num1!=0 )
            {
                vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
                vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
                if (num1<=-31)
                {
                    m1--;
                    num1=num1+31;
                    if (m1==0)
                    { 
                        m1=12;
                        y1=y1-1;
                    }
                }
                else if ( num1 <0)
                {
                    if (-num > d1)
                    {
                        m1--;
                        if (m1==0)
                        { 
                            m1=12;
                            y1=y1-1;
                        }
                    }
                    num1=num1+31;
                }
                vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
                vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
            }
            if (m1==3 && num1!=0)
            {
                vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
                vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
                if ( (( y1%4==0 && y1%100!=0 )||(y1%400==0)) && (m1==3))
                {
                    vtcp_log("[%s][%d]½ñÄêÈòÄê[%d]\n",__FILE__,__LINE__,y1);
                    if (num1<=-29)
                    {
                        m1--; 
                        num1=num1+29;
                    }
                    else if ( num1 <0)
                    {
                        if (-num > d1)
                        {
                            m1--;
                        }
                        num1=num1+29;
                    }   
                }         
                else
                {
                    if (num1<=-28)
                    {
                        m1--; 
                        num1=num1+28;
                    }
                    else if ( num1 <0)
                    {
                        if (-num > d1)
                        {
                            m1--;
                        }  
                        num1=num1+28;
                    }   
                }        
                vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
                vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
            }
        }
        vtcp_log("[%s][%d]num1=[%d]m1=[%d]\n",__FILE__,__LINE__,num1,m1);
        if (num1 == 0)
        {
            if ((m1==5)||(m1==7)||(m1==10)||(m1==12))
            {
                num1 = 30;
                m1--;
            }
            else if ((m1==1)||(m1==2)||(m1==4)||(m1==6)||(m1==8)||(m1==9)||(m1==11))
            {
                num1 = 31;
                m1--;
                if (m1==0)
                { 
                    m1=12;
                    y1=y1-1;
                }
            }
            else
            {
                if (( y1%4==0 && y1%100!=0 )||(y1%400==0))
                {
                    num1 = 29;
                    m1--;
                }
                else
                {
                    num1 = 28;
                    m1--;
                }    
            }
        }
    }
    else
    {
        while (num1 >28)
        {
            if ((m1==1)||(m1==3)||(m1==5)||(m1==7)||(m1==8)||(m1==10)||(m1==12))
            {
                vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
                if (num1>31)
                {
                    m1++;
                    num1=num1-31;
                    if (m1==13)
                    { 
                        m1=1;
                        y1=y1+1;
                    }
                }
                else
                {
                    break;
                }
                vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
            }  
            if ((m1==4)||(m1==6)||(m1==9)||(m1==11))
            {
                vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
                if (num1>30)
                {
                    m1++;
                    num1=num1-30;
                }
                else
                {
                    break;
                }
                vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
            }  
            if (m1==2)
            {
                vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
                if ( (( y1%4==0 && y1%100!=0 )||(y1%400==0)) && (m1==2))
                {
                    vtcp_log("[%s][%d]½ñÄêÈòÄê[%d]\n",__FILE__,__LINE__,y1);
                    if (num1>29)
                    {
                        m1++; 
                        num1=num1-29;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if (num1>28)
                    {
                        m1++; 
                        num1=num1-28;
                    }
                    else
                    {
                        break;
                    }
                }
                vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
            }
        }
    }
    date2=y1*10000+m1*100+num1;
    sprintf(cDate,"%8d",date2);
    vtcp_log("[%s][%d]cDate=[%s]\n",__FILE__,__LINE__,cDate);
    vtcp_log("[%s][%d]lDate=[%ld]\n",__FILE__,__LINE__,date2);
    return(date2);
}


/**********************************************************************
 * º¯ Êı Ãû£º   pub_base_dayvalicheck                                 *
 * º¯Êı¹¦ÄÜ£º   ¼ì²éÈÕÆÚºÏ·¨ĞÔ                                        *
 * ×÷Õß/Ê±¼ä£º  gyf 2006-10-12 10:38                                  * 
 *                                                                    *
 * ²ÎÊı£º                                                             *
 * ÊäÈë: idateÈÕÆÚ                                                    *
                                                                      *
 * Êä³ö: ÎŞ                                                           *
 * ·µ»Ø: 0-Õı³£ 1-²»ºÏ·¨                                              *
 * ĞŞ¸ÄÀúÊ·£º                                                         * 
 *                                                                    *
 *********************************************************************/
int pub_base_dayvalicheck(long idate)
{
    int iyear = 0;
    int imonth = 0;
    int iday = 0;

    iyear  = (int)(idate/10000);
    imonth = (idate / 100)%100 ;
    iday   = idate%100;

    vtcp_log("[%s][%d]ÈÕÆÚºÏ·¨ĞÔ¼ì²é,´«ÈëÈÕÆÚ=[%ld]",__FILE__,__LINE__,idate);

    /** ¼ì²éÄê **/
    if( iyear < 1000 )
    {
            sprintf(acErrMsg,"[%s][%d]ÈÕÆÚÄê·İ´íÎó!sqlcode=[%d]",__FILE__,__LINE__,iyear);
            WRITEMSG
            return(1);
    }

    /** ¼ì²éÔÂ **/
    if( imonth < 1 || imonth > 12 )
    {
            sprintf(acErrMsg,"[%s][%d]ÈÕÆÚÔÂ·İ´íÎó!sqlcode=[%d]",__FILE__,__LINE__,imonth);
            WRITEMSG
            return(1);
    }
    
    /** ¼ì²éÈÕ **/
    switch( imonth )
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if(iday<1 || iday>31)
            {
                    sprintf(acErrMsg,"[%s][%d]ÈÕÆÚÌìÊı´íÎó!sqlcode=[%d]",__FILE__,__LINE__,iday);
                    WRITEMSG
                    return(1);
            }
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            if(iday<1 || iday>30)
            {
                    sprintf(acErrMsg,"[%s][%d]ÈÕÆÚÌìÊı´íÎó!sqlcode=[%d]",__FILE__,__LINE__,iday);
                    WRITEMSG
                    return(1);
            }
            break;
        case 2:
            if(iday<1 || iday>29)
            {
                    sprintf(acErrMsg,"[%s][%d]ÈÕÆÚÌìÊı´íÎó!sqlcode=[%d]",__FILE__,__LINE__,iday);
                    WRITEMSG
                    return(1);
            }
            /** ·ÇÈòÄê2ÔÂÓĞ29ÌìÊÇ´íÎóµÄ **/
            if( !((iyear%4==0 && iyear%100!=0) || iyear%400==0) && iday==29 )    
            {
                    sprintf(acErrMsg,"[%s][%d]ÈÕÆÚÌìÊı´íÎó!sqlcode=[%d]",__FILE__,__LINE__,iday);
                    WRITEMSG
                    return(1);
            }
            break;
        default:
            sprintf(acErrMsg,"[%s][%d]ÈÕÆÚÔÂ·İ´íÎó!sqlcode=[%d]",__FILE__,__LINE__,imonth);
            WRITEMSG
            return(1);
    }

    return(0);
}
/*****************************************************************
 * º¯ÊıÃû: pub_base_iJumpHoliday                                 *
 * ¹¦  ÄÜ: ÅĞ¶Ï´«ÈëµÄÈÕÆÚÊÇ·ñÎª½Ú¼ÙÈÕ£¬µÃµ½Ò»¸ö·Ç·¨¶¨¼ÙÈÕµÄ¹¤×÷ÈÕ*
 * ×÷  Õß: xyy 2008-4-17 11:17                                   * 
 * Ëµ  Ã÷: ·¨¶¨½Ú¼ÙÈÕ°üÀ¨ĞÇÆÚÁùĞÇÆÚÈÕºÍ¹úÎñÔºÏÂ·¢µÄ½Ú¼ÙÈÕ        *
 * ²Î  Êı:                                                       *
 *     ÊäÈë: date1ÈÕÆÚ                                           *
 *     Êä³ö: ÎŞ                                                  *
 * ·µ  »Ø: date1ÈÕÆÚ                                             *
 * ĞŞ  ¸Ä:                                                       * 
 *                                                               *
 *****************************************************************/
int pub_base_iJumpHoliday(long *date1)/***NEWBP7***/
{
    int iRet=0;
    int i = 0;
    long lDate1=0;
    long lDate2=1;
    char cTx_date[9];
    char cTmp_date[9];
    struct com_parm_holiday_c com_parm_holiday;
    vtcp_log("[%s][%d]This process is test holiday[%ld]\n",__FILE__,__LINE__,*date1);
    for (;;)
    {
        iRet=Com_parm_holiday_Dec_Sel(g_pub_tx.reply,"beg_date<='%ld' and end_date>='%ld' and type='1' ",*date1,*date1);
        if(iRet)
        {
            vtcp_log("%s,%d,ÓÎ±êÉùÃ÷³ö´í[%d]\n",__FILE__,__LINE__,iRet);
            strcpy(acErrMsg,"ÓÎ±êÉùÃ÷³ö´í!!");WRITEMSG
            return(-1);
        }
        while(1)
        {
            memset(&com_parm_holiday , 0 , sizeof(com_parm_holiday));
            
            iRet = Com_parm_holiday_Fet_Sel(&com_parm_holiday,g_pub_tx.reply);
            if(iRet!=0 && iRet!=100)
            {
                vtcp_log("%s,%d,Êı¾İ¿â´íÎó[%d]\n",__FILE__,__LINE__,iRet);
                strcpy(acErrMsg,"Êı¾İ¿â´íÎó!!");WRITEMSG
                return(-1);
            }
            else if (iRet==100)
            {
            	  vtcp_log("[%s][%d]²»´æÔÚÕâÑùµÄÇø¼ä\n",__FILE__,__LINE__);
            	  if(i==0)
            	  {
                    lDate1 = 1;
                }
                break;
            }
            else
            {   
                memset(cTx_date  , 0 , sizeof(cTx_date));
                memset(cTmp_date , 0 , sizeof(cTmp_date));
                apitoa(com_parm_holiday.end_date,8,cTx_date);
                vtcp_log("[%s][%d]´Ë´Îµ½ÆÚÈÕÎª[%s]\n",__FILE__,__LINE__,cTx_date);
                memcpy(cTmp_date,pub_base_daynumLAT(cTx_date,1),sizeof(cTmp_date)-1);;
                vtcp_log("[%s][%d]µ½ÆÚÈÕµÄÏÂÒ»ÌìÎª[%s]\n",__FILE__,__LINE__,cTmp_date);
                lDate1 = apatoi(cTmp_date,8);
            }
            i++;
         }
         Com_parm_holiday_Clo_Sel();
         vtcp_log("[%s][%d]note:[%ld][%ld]\n",__FILE__,__LINE__,lDate1,lDate2);
         if(lDate1==lDate2)
         {
             break;
         }
         else if(lDate1 > lDate2)
         {
             lDate2 = lDate1;
             *date1 = lDate1;
         }
         else
         {
             if(lDate1 ==1)
             {
                  vtcp_log("[%s][%d]Ë³ÀûÍË³ö[%ld][%ld]\n",__FILE__,__LINE__,lDate1,lDate2);
                  break;
             }
             else
             {
                  vtcp_log("[%s][%d]¼ÆËãÈÕÆÚ´í[%ld][%ld]\n",__FILE__,__LINE__,lDate1,lDate2);
                  sprintf( acErrMsg, "¼ÆËãÈÕÆÚ´í" );
                  WRITEMSG
                  return(-1);
             }
        }
    }
    vtcp_log("[%s][%d]µ½ÆÚÈÕÎª[%ld]\n",__FILE__,__LINE__,*date1);
    return 0;
}

/**********************************************************************
* º¯ Êı Ãû£º   pub_base_CaltureMouths
* º¯Êı¹¦ÄÜ£º   ¼ÆËã×¼È·ÔÂÊı
* ×÷Õß/Ê±¼ä£º  yaoyuchao  20111103
*
* ²ÎÊı£º   
*     ÊäÈë: date1,long	:ÆğÊ¼ÕûĞÍÈÕÆÚ
*		        date2,long	:ÖÕÖ¹ÕûĞÍÈÕÆÚ
*
*     Êä³ö: ÔÂÊıiYs
*
*     ·µ»Ø: >=0   ³É¹¦
*           <0    Ê§°Ü
* ĞŞ¸ÄÀúÊ·£º
*
********************************************************************/
int pub_base_CaltureMouths( long date1, long date2)
{
	int ret1=0;
	int ret2=0;  
	int tail=0;	  /****ÓÃÓÚ¼ÆËãÂú²»ÂúÕûÔÂ£¬ÂúÎª0£¬²»ÂúÎª-1***/
	short t_ys,ymd1[3],ymd2[3],ymd3[3];
	long iYs;
	if (date1>date2) return 0;

	ymd1[0]=(short)(date1/100%100);
	ymd1[1]=(short)(date1%100);
	ymd1[2]=(short)(date1/10000);

	ymd2[0]=(short)(date2/100%100);
	ymd2[1]=(short)(date2%100);
	ymd2[2]=(short)(date2/10000);
	
	ret1=pub_base_end_month(date1);
	ret2=pub_base_end_month(date2);
	if((ret1==0 && ret2==0) || (ret1==-1 && ret2==0))
		{
			tail=0;     
		}
	if((ret1==0 && ret2==-1) && (ymd2[1]>=ymd1[1]))
		{			
			tail=0;
		}
	if((ret1==0 && ret2==-1) && (ymd2[1]<ymd1[1]))
		{
				tail=-1;
		}
	if((ret1==-1 && ret2==-1) && (ymd2[1]>=ymd1[1]))	
		{
			tail=0;
		}
	if((ret1==-1 && ret2==-1) && (ymd2[1]<ymd1[1]))
		{
			tail=-1;
		}
	iYs = (ymd2[2]-ymd1[2])*12+(ymd2[0]-ymd1[0])+tail;
	return (long)iYs;
}

/* create by lanzhisheng - 20050906 - HongDong  */
/* MIS使用的综合业务的资产负债表和损益表的数据  */
drop index mis_iosreportdatatmp_idx;
drop index mis_iosreportdatatmp_idx2;
drop table mis_iosreportdatatmp;
create table mis_iosreportdatatmp
(
        brno         varchar2(7) NOT NULL,      /* 行号     */
        pigday       varchar2(8) NOT NULL,      /* 归档日期 */
        flag         number(2,0) NOT NULL,      /* 报表类别标志 1-资产负债表
                                                   2-损益表 3-月报表*/
        reprownum    number(4,0) NOT NULL,      /* 行次     */
        name         varchar2(60) NOT NULL,     /* 栏目名称 */
        tdbal        varchar(18),               /* 本期余额 */
        ydbal        varchar(18),               /* 上期余额 */
        tmbal        number(20,2),              /* 本期发生额 */
        tdmbal       number(20,2),              /* 本期借方发生额 */
        tcmbal       number(20,2),              /* 本期贷方发生额 */
        cdcnt        number(8)   ,              /*{借方现金笔数}   */
        cdamt        number(16,2),              /*{借方现金金额}   */
        cccnt        number(8)   ,              /*{贷方现金笔数}   */
        ccamt        number(16,2),              /*{贷方现金金额}   */
        mdcnt        number(8)   ,              /*{借方转帐笔数}   */
        mdamt        number(16,2),              /*{借方转帐金额}   */
        mccnt        number(8)   ,              /*{贷方转帐笔数}   */
        mcamt        number(16,2),              /*{贷方转帐金额}   */
        fillter      varchar(10)                /* 备注 */
)tablespace users;
create unique index mis_iosreportdatatmp_idx on mis_iosreportdatatmp(brno,pigday,flag,reprownum) tablespace indx;
create index mis_iosreportdatatmp_idx2 on mis_iosreportdatatmp(brno,pigday,flag,name) tablespace indx;


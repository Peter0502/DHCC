/**************************************************/
/* TABLE NAME:bplosthis 本票挂失信息表            */
/* Description  : Postal Order Saving Master File */
/* Table index  : pono,txday,                     */
/* create by Wei XiaoJian: 2006-11-9              */
/***************************************************/
drop index  bplosthis_idx;
drop table bplosthis;
create table bplosthis(
    lostday           number(8),    /***挂失日期***/
    pono              varchar2(20), /***本票号码****/
    lostcnt           number(6),    /***挂失次数****/
    signday           number(8),    /***出票日期***/
    unlostday         number(8),    /***挂失/解挂日期***/
    lostbrno	      Varchar2(7),  /***挂失交易机构***/
    unlostbrno	      Varchar2(7),  /***解挂失交易机构***/
    inflag            varchar2(1),  /*本票行别0.本行本票1.他行本票 */
    bptype            varchar2(1),  /*本票类别0.转帐本票1.现金本票 */
    paybrno           Varchar2(12), /**签发行行号***/
    paybrname         varchar2(60), /** 签发行行名**/
    cashbrno          varchar2(12), /** 兑付行行号**/
    cashbrname        varchar2(60), /** 兑付行行名**/
    avbal             Number(16,2), /***票面金额***/
    feeflag           varchar2(1),  /** 手续费标志 0-收 1-不收 */
    feeamt            number(16,2), /** 手续费金额 */
    stat              Varchar2(1),  /***帐户状态0--挂失1--解挂失***/
    losttlrno         Varchar2(6),  /***挂失操作员***/
    unlosttlrno       Varchar2(6),  /***解挂操作员***/
    filler            Varchar2(60)  /***备注---可以放挂失人联系方式****/
)tablespace users;
create unique index bplosthis_idx on bplosthis(pono,lostday,lostcnt) tablespace indx;

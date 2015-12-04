/******************************************************************************/
/* Table Name:  LV_NEWXYH 新多方协议号表                                         */
/* Location:    On region server                                              */
/* Index:       xyno,payactno                                                  */
/* Create for   XiaoE                        2005/12/08                          */
/******************************************************************************/
drop index lv_newxyh_idx;
drop table lv_newxyh;
CREATE TABLE lv_newxyh(
    xyno         varchar2(60),    /* 多方协议号,付款检查用 */
    pay_ac_no    varchar2(32),    /* 付款帐号<本行>    */
    pay_name     varchar2(60),    /* 付款户名<本行> */
    pay_no       varchar2(12),    /* 付款人开户行*/
    cashxyno     varchar2(60),    /* 收款人协议号,不作检查 */
    cash_ac_no   varchar2(32),    /* 收款人帐号 */
    cash_name    varchar2(60),    /* 收款人户名 */
    cash_no      varchar2(12),    /* 收款人行号 */
    yw_type      varchar2(5),     /* 业务种类 */
    ywlist       varchar2(200),   /* 明细业务种类列表**/
    limitamt     number(17,2),    /* 付款限额 */
    br_no        varchar2(7),     /* 录入机构 */
    last_date    number(8),       /* 最近修改日期 */
    tel          varchar2(6),     /* 最后修改操作员号 */
    flag         varchar2(1),     /* 标志*/
    filler       varchar2(60)     /* 备注 */
)tablespace users;
CREATE UNIQUE INDEX lv_newxyh_idx on lv_newxyh(xyno,pay_ac_no) tablespace indx;

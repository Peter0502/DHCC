/**    CMT552小额收费通知报文登记簿 **/
drop index hv_chargemx_idx;
drop table hv_chargemx;
create table hv_chargemx(
    br_no           Varchar2(7)    ,/**  机构号（接收行号）                  **/
    in_date         Number(8)      ,/**  交易（进入系统）日期                **/
    dtlid           Number(3)      ,/**  明细序号                            **/
    chgtype         Varchar(1)     ,/**  收费类型 1支付类 2单边 3信息类      **/
    cmtno           Varchar2(3)    ,/**  Cmt编号                             **/
    srflag          Varchar2(1)    ,/**  收费明细之收发标志0 发送 1接收      **/
    /** timespan        Varchar2(40)   , 计费时间段                          **/
    lowlimit        Varchar2(6)    ,/**  收费明细之时间下限                  **/
    upplimit        Varchar2(6)    ,/**  收费明细之时间上限                  **/
    mnum            Varchar2(10)   ,/**  收费明细之记录笔数                  **/
    price           Varchar2(6)    ,/**  收费明细之计费单价单位              **/
    brate           Varchar2(6)    ,/**  收费明细之基础费率单位：            **/
    pecrate         Varchar2(6)    ,/**  收费明细之时段收费百分比            **/
    txamt           Varchar2(15)   ,/**  收费明细之计费金额                  **/
    prtflag         Varchar2(1)     /**  收费明细之打印标志0 未打印 1已打印  **/
)tablespace users;
create unique index hv_chargemx_idx on hv_chargemx(in_date,dtlid)tablespace indx;

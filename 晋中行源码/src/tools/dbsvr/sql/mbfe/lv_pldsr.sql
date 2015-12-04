/**    批量代收业务接收登记簿 **/
drop index lv_pldsr_idx;
drop table lv_pldsr;
create table lv_pldsr(
    packid           varchar2(8)  ,     /** 包序号　                **/
    orderno          varchar2(8)  ,     /** 交易序号　              **/
    wt_date          number(9)    ,     /** 包委托日期　            **/
    br_no            varchar2(12) ,     /** 本交易处理行的行号　    **/
    or_br_no         varchar2(12) ,     /** 发起行                  **/
    dsf_date         number(9)    ,     /** 代收付中心工作日        **/
    re_lim_date      number(2)    ,     /** 回执期限（天数）        **/
    cash_br_no       varchar2(12 ) ,    /** 收款行行号              **/
    cash_opn_br_no   varchar2(12 ) ,    /** 收款人开户行行号        **/
    cash_ac_no       varchar2(32 ) ,    /** 收款人账户              **/
    cash_name        varchar2(60 ) ,    /** 收款人名称              **/
    cash_addr        varchar2(60 ) ,    /** 收款人地址              **/
    yw_type          varchar2(5  ) ,    /** 业务种类                **/
    totamt           number(13,2)  ,    /** 本条收款总金额          **/
    pay_qs_no        varchar2(12 ) ,    /** 付款人清算行行号        **/
    pay_num          number(8  )   ,    /** 付款人数目              **/
    tx_mx_no         varchar2(8  ) ,    /** 明细序号,全0时为总信息  **/
    pay_br_no        varchar2(12 ) ,    /** 付款行行号              **/
    pay_opn_br_no    varchar2(12 ) ,    /** 付款人开户行行号        **/
    pay_ac_no        varchar2(32 ) ,    /** 付款人账号              **/
    pay_name         varchar2(60 ) ,    /** 付款人名称              **/
    pay_addr         varchar2(60 ) ,    /** 付款人地址              **/
    tx_amt           number(13,2)  ,    /** 金额                    **/
    xyno             varchar2(60 ) ,    /** 合同（协议）号          **/
    content          varchar2(60 ) ,    /** 附言                    **/ 
    filename         varchar2(256) ,    /** 生成的报盘文件　        **/ 
    sts              varchar2(2)   ,    /** 交易状态                **/
    beg_sts          varchar2(2)   ,    /** 初始交易状态            **/    
    operlevel        varchar2(1)   ,    /** 优先级                  **/    
    /** 40503　回执 **/
    repstat          varchar(2  )  ,    /** 回执状态                **/
    succamt          number(13,2)  ,    /** 成功收款总金额          **/
    succnum          number(8  )   ,    /** 成功笔数                **/
    re_content       varchar(60 )  ,    /** 附言                    **/
    /** 40506　回执 **/
    re_dsf_date      number(9)     ,    /** 代收付中心工作日        **/
    repstat2         varchar2(2 )  ,    /** 业务回执状态            **/
    re_content2      varchar2(60)       /** 附言                    **/
)tablespace users;
create unique index lv_pldsr_idx on lv_pldsr(packid,orderno,wt_date,tx_mx_no)  tablespace indx;

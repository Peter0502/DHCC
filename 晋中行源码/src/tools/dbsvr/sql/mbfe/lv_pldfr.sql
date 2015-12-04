/**    批量代付业务接收登记簿 **/
drop index lv_pldfr_idx;
drop table lv_pldfr;
create table lv_pldfr(
    packid         varchar2(8) ,  /**包序号　            **/
    orderno        varchar2(8) ,  /**交易序号　          **/
    wt_date        number(9)   ,  /**包委托日期　        **/
    br_no          varchar2(12) , /*本交易处理行的行号   **/
    or_br_no       varchar2(12) , /**发起行              **/
    dsf_date       number(9)    , /**代收付中心工作日    **/
    xyno           varchar2(60) , /**合同（协议）号      **/
    pay_br_no      varchar2(12) , /**付款行行号          **/
    pay_opn_br_no  varchar2(12) , /**付款人开户行行号    **/
    pay_ac_no      varchar2(32) , /**付款人账号          **/
    pay_name       varchar2(60) , /**付款人名称          **/
    pay_addr       varchar2(60) , /**付款人地址          **/                        
    yw_type        varchar2(5)  , /**业务种类            **/
    totamt         number(13,2) , /**本条付款总金额      **/                                                                
    cash_qs_no     varchar2(12) , /**收款人清算行行号    **/
    cash_num       number(8)    , /**收款人数目          **/
    tx_mx_no       varchar2(8)  , /**明细序号,全0时为总信息**/
    cash_br_no     varchar2(12) , /**收款行行号          **/
    cash_opn_br_no varchar2(12) , /**收款人开户行行号    **/
    cash_ac_no     varchar2(32) , /**收款人账号          **/
    cash_name      varchar2(60) , /**收款人名称          **/
    cash_addr      varchar2(60) , /**收款人地址          **/
    tx_amt         number(13,2) , /**金额                **/
    content        varchar2(60) , /**附言                **/
    sts            varchar2(2)  , /** 交易状态           **/
    beg_sts        varchar2(2)  , /** 初始交易状态       **/
    operlevel      varchar2(1)  , /** 优先级             **/
    filename       varchar2(256), /** 生成的报盘文件　   **/
    /** 40503　回执 **/
    repstat        varchar2(2)  , /** 回执状态           **/
    succamt        number(13,2) , /** 成功付款总金额     **/
    succnum        number(8)  , /** 成功笔数           **/
    re_content     varchar2(60) , /** 附言               **/
    /** 40506　回执 **/
    re_dsf_date    number(9)    , /** 代收付中心工作日   **/
    repstat2       varchar2(2)  , /** 业务回执状态       **/
    re_content2    varchar2(60)   /** 附言               **/
)tablespace users;
create unique index lv_pldfr_idx on lv_pldfr(packid,orderno,wt_date,tx_mx_no)tablespace indx;

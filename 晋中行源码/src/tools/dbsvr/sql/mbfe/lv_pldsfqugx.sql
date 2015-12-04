/**    批量签约关系变更通知登记簿 **/
drop index lv_pldsfqugx_idx;
drop table lv_pldsfqugx;
create table lv_pldsfqugx(
    pkgno         varchar2(3)   ,     /** 包序号　           **/
    orderno       varchar2(8)   ,     /** 交易序号　         **/
    wt_date       number(9)     ,     /** 包委托日期　       **/
    br_no         varchar2(12)  ,     /** 本交易处理行的行号 **/
    or_br_no      varchar2(12)  ,     /** 发起行             **/
    packid        varchar2(8)   ,     /** 包id               **/
    xyno          varchar2(60 ) ,     /** 合同（协议）号     **/
    xy_ind        varchar2(1  ) ,     /** 合同（协议）类型   **/
    ch_ind        varchar2(1  ) ,     /** 变更方式           **/
    pay_br_no     varchar2(12 ) ,     /** 付款行行号         **/
    pay_qs_no     varchar2(12 ) ,     /** 付款清算行行号     **/
    pay_opn_br_no varchar2(12 ) ,     /** 付款人开户行行号   **/
    pay_ac_no     varchar2(32 ) ,     /** 付款人账号         **/
    pay_name      varchar2(60 ) ,     /** 付款人名称         **/
    pay_addr      varchar2(60 ) ,     /** 付款人地址         **/
    content       varchar2(60 ) ,     /** 附言               **/
    sts           varchar2(2)     ,   /** 交易状态           **/
    beg_sts       varchar2(2)     ,   /** 初始交易状态       **/
    operlevel     varchar2(1)     ,   /** 优先级             **/
    /** 40506　回执 **/                                      
    re_dsf_date   number(9) ,         /** 代收付中心工作日   **/
    repstat       varchar2(2 ) ,      /** 业务回执状态       **/
    re_content2   varchar2(60)        /** 附言               **/
)tablespace users;
create unique index lv_pldsfqugx_idx on lv_pldsfqugx(orderno,wt_date,or_br_no) tablespace indx;    

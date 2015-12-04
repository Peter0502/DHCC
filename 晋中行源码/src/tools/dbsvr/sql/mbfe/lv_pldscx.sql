/**    批量代收撤消登记簿 **/
drop index lv_pldscx_idx;
drop table lv_pldscx;
create table lv_pldscx(
    packid           varchar2(8)  ,     /** 包序号　          **/
    orderno          varchar2(8)  ,     /** 交易序号　        **/
    wt_date          number(9)    ,     /** 包委托日期　      **/
    br_no            varchar2(12) ,     /** 本交易处理行的行号 */
    or_br_no         varchar2(12) ,     /** 发起行            **/
    o_wt_date        number(9 )   ,     /** 原交易委托日期    **/
    o_orderno        varchar2(8 ) ,     /** 原交易信息业务序号**/
    o_yw_type        varchar2(5 ) ,     /** 原包业务类型      **/
    dsf_date         number(9 )   ,     /** 代收付中心日期    **/
    content          varchar2(60) ,     /** 附言              **/
    sts              varchar2(2)  ,     /** 交易状态          **/
    beg_sts          varchar2(2)  ,     /** 初始交易状态      **/
    operlevel        varchar2(1)  ,     /** 优先级            **/
    /** 40506　回执 **/
    re_dsf_date      number(9 )   ,     /** 代收付中心工作日  **/
    repstat2         varchar2(2 )   ,     /** 业务回执状态      **/
    re_content2      varchar2(60)       /** 附言              **/
)tablespace users;
create unique index lv_pldscx_idx on lv_pldscx(packid,orderno,wt_date)tablespace indx;

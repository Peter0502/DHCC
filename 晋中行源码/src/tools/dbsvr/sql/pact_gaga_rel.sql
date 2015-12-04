create table pact_gaga_rel (
    pact_no varchar2(20),        /**合同号      **/
    gaga_id varchar2(20),        /**抵质押品编号**/
    name varchar(60),  		/** 物品描述 **/
    gaga_bal decimal(17,2),      /**所抵押金额  **/
    gaga_val decimal(17,2),      /**所抵押价值  **/
    stsvar varchar2(1),          /**状态  X信贷发起入库Y-信贷发起提示出库1-正常关联，2-关联取消      **/
    pact_type varchar2(1),       /**D:贷款 C:承兑 **/
    io_ind varchar2(4),          /** 120为本行定期存单 其他为外行存单    **/
    bill_no varchar2(20),         /** 本行质押存单账号  **/
    filler varchar2(30)          /**            **/
)tablespace users;
create index  pact_gaga_rel_idx on pact_gaga_rel (pact_no,gaga_id) tablespace indx;

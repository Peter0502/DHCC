drop table mo_bank_po_fee;
drop index mobankpofeeidx;
CREATE TABLE mo_bank_po_fee (
pact_no varchar2(20),   /** 承兑汇票协议编号 **/
num number(9),          /** 序号(与协议编号组成唯一) **/
po_amt          decimal(16,2),  /** 票面金额 **/
fee_ratio       decimal(8,5),   /** 收费比例 **/
fee_amt         decimal(16,2),  /** 收费金额 **/
prm_amt         decimal(16,2),  /** 承诺费 **/
sts             varchar2(1)     /** 该票状态 X:信贷登记  0:已签发 **/
)tablespace users;
create unique index mobankpofeeidx on mo_bank_po_fee(pact_no,num) tablespace indx;

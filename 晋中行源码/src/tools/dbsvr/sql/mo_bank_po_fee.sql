drop table mo_bank_po_fee;
drop index mobankpofeeidx;
CREATE TABLE mo_bank_po_fee (
pact_no varchar2(20),   /** �жһ�ƱЭ���� **/
num number(9),          /** ���(��Э�������Ψһ) **/
po_amt          decimal(16,2),  /** Ʊ���� **/
fee_ratio       decimal(8,5),   /** �շѱ��� **/
fee_amt         decimal(16,2),  /** �շѽ�� **/
prm_amt         decimal(16,2),  /** ��ŵ�� **/
sts             varchar2(1)     /** ��Ʊ״̬ X:�Ŵ��Ǽ�  0:��ǩ�� **/
)tablespace users;
create unique index mobankpofeeidx on mo_bank_po_fee(pact_no,num) tablespace indx;

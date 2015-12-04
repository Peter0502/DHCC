drop  table mo_bank_po;
create table mo_bank_po(
	sign_br_no varchar2(5),
	sign_date number(9),
	receive_date number(9),
	cash_date number(9),
	pact_no varchar2(20),
	pact_no_num number(6),
	po_no varchar2(30),/**为电票改为30**/
	po_amt number(16,2),
	at_term_ind varchar2(1),
	po_sts varchar2(1),
	po_ind varchar2(1)
)tablespace users;

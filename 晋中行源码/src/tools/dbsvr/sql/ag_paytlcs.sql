drop  table ag_paytlcs;
create table ag_paytlcs(
	unit_no varchar2(4),
	bank_tlcs varchar2(12),
	batch_no varchar2(10),
	unit_tlcs varchar2(10),
	pre_no varchar2(12),
	bank_acct_no varchar2(25),
	trn_date varchar2(8),
	trn_time varchar2(6),
	br_no varchar2(7),
	tel_no varchar2(4),
	trn_amt number(12,2),
	trcode varchar2(4),
	item_no varchar2(20),
	pay_month varchar2(6),
	pay_way varchar2(1),
	ckmk_flag varchar2(1),
	bill_flag varchar2(1),
	bill_no varchar2(10),
	stat varchar2(1) 
)tablespace users;

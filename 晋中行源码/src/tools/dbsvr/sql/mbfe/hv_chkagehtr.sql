drop index hv_chkagehtr_idx;
drop  table hv_chkagehtr;
create table hv_chkagehtr(
	orderno varchar2(8),
	pay_qs_no varchar2(12),
	tx_date number(9),
	br_no varchar2(7),
	cmtno varchar2(3),
	tx_time number(9),
	cur_no varchar2(3),
	tx_amt number(15,2),
	or_br_no varchar2(12),
	pay_no varchar2(12),
	pay_ac_no varchar2(32),
	pay_name varchar2(60),
	cash_qs_ac_no varchar2(12),
	acbr_no varchar2(12),
	cash_no varchar2(12),
	cash_ac_no varchar2(32),
	cash_name varchar2(60),
	yw_type varchar2(2),
	sendco varchar2(4),
	receco varchar2(4),
	operlevel varchar2(1),
	brf1 varchar2(30),
	brf2 varchar2(30) 
)tablespace users;
create	 unique index hv_chkagehtr_idx on hv_chkagehtr( orderno,or_br_no,tx_date)tablespace indx;

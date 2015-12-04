drop index hv_sendlist_idx;
drop  table hv_sendlist;
create table hv_sendlist(
	br_no varchar2(7),
	tel varchar2(6),
	trace_no number(9),
	ref_no varchar2(20),
	cmtno varchar2(3),
	cmt_title varchar2(30),
	orderno varchar2(8),
	or_br_no varchar2(12),
	ac_br_no varchar2(12),
	pay_ac_no varchar2(32),
	pay_name varchar2(60),
	cash_ac_no varchar2(32),
	cash_name varchar2(60),
	tx_amt number(15,2),
	resp_flg varchar2(1),
	resp_no varchar2(8),
	resp_title varchar2(60),
	brf varchar2(30) 
)tablespace users;
create	 unique index hv_sendlist_idx on hv_sendlist( ref_no)tablespace indx;

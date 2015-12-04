drop index hv_chkerror_idx;
drop  table hv_chkerror;
create table hv_chkerror(
	br_no varchar2(7),
	cmtno varchar2(3),
	orderno varchar2(8),
	pay_qs_no varchar2(12),
	or_br_no varchar2(12),
	tx_date number(9),
	tx_time number(9),
	tx_amt number(15,2),
	tx_amt_ccb number(15,2),
	tx_amt_ccpc number(15,2),
	o_sts varchar2(1),
	sts varchar2(1),
	chk_flg varchar2(1),
	title varchar2(30),
	prt_flag varchar2(1),
	brf varchar2(60) 
)tablespace users;
create	 unique index hv_chkerror_idx on hv_chkerror( orderno,or_br_no,tx_date)tablespace indx;

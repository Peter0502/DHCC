drop index hv_errormr_idx;
drop  table hv_errormr;
create table hv_errormr(
	br_no varchar2(7),
	cmtno varchar2(3),
	orderno varchar2(8),
	pay_qs_no varchar2(12),
	tx_date number(9),
	tx_time number(9),
	tx_amt_ccb number(15,2),
	tx_amt_ccpc number(15,2),
	sts varchar2(1),
	flg varchar2(1),
	brf varchar2(60) 
)tablespace users;
create	 unique index hv_errormr_idx on hv_errormr( orderno,pay_qs_no)tablespace indx;

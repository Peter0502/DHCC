drop index hv_chkmrdtl_idx;
drop  table hv_chkmrdtl;
create table hv_chkmrdtl(
	tx_date number(9),
	cmtno varchar2(6),
	s_cnt number(9),
	s_amt number(22,2),
	r_cnt number(9),
	r_amt number(22,2),
	brf varchar2(30) 
)tablespace users;
create	 unique index hv_chkmrdtl_idx on hv_chkmrdtl( tx_date,cmtno)tablespace indx;

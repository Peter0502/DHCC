drop index pos_fee_mst_1;
drop  table pos_fee_mst;
create table pos_fee_mst(
	area_code varchar2(4),
	tx_date number(9),
	tx_cnt number(9),
	tx_amt number(16,2),
	dr_amt number(16,2),
	cr_amt number(16,2),
	n_cr_amt number(16,2),
	adjust_dr_amt number(16,2),
	adjust_cr_amt number(16,2),
	succ_num number(9),
	mater_amt number(16,2),
	clr_amt number(16,2),
	settle_date number(9),
	deal_sign varchar2(1),
	deal_msg varchar2(40) 
)tablespace users;
create	 index pos_fee_mst_1 on pos_fee_mst( area_code,tx_date,clr_amt)tablespace indx;

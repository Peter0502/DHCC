drop index pos_fee_deal_1;
drop  table pos_fee_deal;
create table pos_fee_deal(
	fname varchar2(10),
	pos_date number(9),
	deal_flag varchar2(1),
	req_cnt number(9) 
)tablespace users;
create	 unique index pos_fee_deal_1 on pos_fee_deal( fname,pos_date)tablespace indx;

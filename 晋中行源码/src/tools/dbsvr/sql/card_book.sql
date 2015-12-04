drop  table card_book;
create table card_book(
	mdm_code varchar2(4),
	beg_no varchar2(20),
	end_no varchar2(20),
	cnt number(9),
	tx_date number(9),
	br_no varchar2(5),
	tel varchar2(4) 
)tablespace users;

drop  table tel_control;
create table tel_control(
	br_no varchar2(5),
	tel varchar2(4),
	tx_date number(9),
	sts varchar2(1),
	msg varchar2(100) 
)tablespace users;

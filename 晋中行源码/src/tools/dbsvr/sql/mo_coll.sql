drop  table mo_coll;
create table mo_coll(
	ac_no varchar2(19),
	name varchar2(50),
	pick_date number(9),
	pick_br_no varchar2(5),
	pick_tel varchar2(4),
	ret_date number(9),
	ret_br_no varchar2(5),
	sts varchar2(1),
	wrk_date number(9),
	trance_no number(9),
	ret_tel varchar2(4) 
)tablespace users;

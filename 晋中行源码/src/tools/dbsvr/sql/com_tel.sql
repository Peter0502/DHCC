drop index com_tel_1;
drop  table com_tel;
create table com_tel(
	tel varchar2(4),
	br_no varchar2(5),
	name varchar2(8),
	pwd varchar2(6),
	lvl varchar2(1),
	csts varchar2(1),
	pwd_date number(9),
	beg_menu varchar2(4),
	chk_ind varchar2(1),
	tx_type varchar2(10) 
)tablespace users;
create	 unique index com_tel_1 on com_tel( tel)tablespace indx;

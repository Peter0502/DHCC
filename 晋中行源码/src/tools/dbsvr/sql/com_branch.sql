drop index com_branch_1;
drop  table com_branch;
create table com_branch(
	br_no varchar2(5),
	br_name varchar2(30),
	wrk_sts varchar2(10),
	br_type varchar2(1),
	ipaddr varchar2(15),
	ind varchar2(1),
	city_no number(6),
	spe_ind varchar2(10),
	up_br_no varchar2(5),
	tx_type number(6),
	br_tele varchar2(20) 
)tablespace users;
create	 unique index com_branch_1 on com_branch( br_no)tablespace indx;

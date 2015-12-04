drop  table dc_branch;
create table dc_branch(
	br_no varchar2(5),
	br_name varchar2(20),
	cash_ac_ind varchar2(1),
	br_lvl varchar2(1),
	up_br_no varchar2(5) 
)tablespace users;

drop index com_cur_no_code_1;
drop  table com_cur_no_code;
create table com_cur_no_code(
	cur_no varchar2(2),
	cur_no_name varchar2(20),
	cur_no_code varchar2(5),
	use_ind varchar2(1) 
)tablespace users;
create	 unique index com_cur_no_code_1 on com_cur_no_code( cur_no)tablespace indx;

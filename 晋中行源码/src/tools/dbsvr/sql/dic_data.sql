drop index dic_data_1;
drop  table dic_data;
create table dic_data(
	pr_code varchar2(15),
	pr_seq varchar2(8),
	content varchar2(40) 
)tablespace users;
create	 unique index dic_data_1 on dic_data( pr_code,pr_seq)tablespace indx;

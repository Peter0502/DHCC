drop index mbfe_sys_ctl_idx;
drop  table mbfe_sys_ctl;
create table mbfe_sys_ctl(
	par_code varchar2(2),
	par_name varchar2(40),
	par_value varchar2(60),
	par_desc varchar2(100) 
)tablespace users;
create	 unique index mbfe_sys_ctl_idx on mbfe_sys_ctl( par_code)tablespace indx;

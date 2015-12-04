drop index mbfe_errcode_idx;
drop  table mbfe_errcode;
create table mbfe_errcode(
	errcode varchar2(8),
	errdesc varchar2(60) 
)tablespace users;
create	 unique index mbfe_errcode_idx on mbfe_errcode( errcode)tablespace indx;

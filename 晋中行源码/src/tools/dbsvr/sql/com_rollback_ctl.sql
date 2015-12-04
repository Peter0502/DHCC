drop index com_rollback_ctl_1;
drop  table com_rollback_ctl;
create table com_rollback_ctl(
	sub_tx_code varchar2(4),
	reg_ind varchar2(200),
	memo varchar2(10) 
)tablespace users;
create	 unique index com_rollback_ctl_1 on com_rollback_ctl( sub_tx_code)tablespace indx;

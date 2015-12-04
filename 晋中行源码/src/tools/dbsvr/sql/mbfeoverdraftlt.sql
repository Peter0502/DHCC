drop index mbfeoverdraftlt_idx;
drop  table mbfeoverdraftlt;
create table mbfeoverdraftlt(
	row_id number(9),
	consigndate number(9),
	accrualtime varchar2(6),
	overdraftamnt number(22,2),
	rate number(6,4),
	accrualamnt number(22,2) 
)tablespace users;
create	 unique index mbfeoverdraftlt_idx on mbfeoverdraftlt( consigndate,accrualtime)tablespace indx;

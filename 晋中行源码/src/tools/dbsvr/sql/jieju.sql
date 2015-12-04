drop  table jieju;
create table jieju(
	ac_id   number(9),
	o_pact_no varchar2(20),
	n_pact_no varchar2(20)
)tablespace users;
create	 unique index jieju_1 on jieju(ac_id) tablespace indx;

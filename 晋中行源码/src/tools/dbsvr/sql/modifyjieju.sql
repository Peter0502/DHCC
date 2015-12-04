drop  table modifyjieju;
create table modifyjieju(
	ac_no   varchar2(20),
	ac_id   number(9),
	o_pact_no varchar2(20),
	n_pact_no varchar2(20)
)tablespace users;
create	 unique index modifyjieju_1 on modifyjieju(ac_no) tablespace indx;

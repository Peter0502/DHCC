drop index mbfeeisbanksnd_idx;
drop  table mbfeeisbanksnd;
create table mbfeeisbanksnd(
	bnkno varchar2(6),
	eissiteno varchar2(4),
	cnapsflg varchar2(1),
	cnapsbnkno varchar2(12),
	bnksstat varchar2(1),
	bnkcity varchar2(4),
	bnklname varchar2(60),
	bnkssname varchar2(20),
	bnkaddr varchar2(60),
	bnkpostcode varchar2(6),
	bnktel varchar2(30),
	bkemail varchar2(30),
	opertype varchar2(1),
	opereffdate number(9),
	bnkrmrk varchar2(60) 
)tablespace users;
create	 unique index mbfeeisbanksnd_idx on mbfeeisbanksnd( bnkno,opertype,opereffdate)tablespace indx;

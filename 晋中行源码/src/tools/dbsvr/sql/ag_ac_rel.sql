drop index ag_ac_rel_1;
drop table ag_ac_rel;
create table ag_ac_rel
(
	main_ac_id	Number(9),	/*���˺�AC_ID						*/
	app_ac_id	Number(9),	/*���˺�AC_ID		                                */
	sts		varchar2(1),	/*״̬			                                */
	filler1		varchar2(20),	/*����1			                                */
	filler2		varchar2(20)	/*����2		                                        */
)tablespace users;
create unique index ag_ac_rel_1 on ag_ac_rel( main_ac_id) tablespace indx;


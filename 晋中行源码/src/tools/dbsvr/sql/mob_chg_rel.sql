drop index mob_chg_rel_1;
drop table mob_chg_rel;
create table mob_chg_rel
(
	main_ac_id	Number(9),	/*���˺�AC_ID						*/
	app_ac_id	Number(9),	/*���˺�AC_ID		                                */
	sts		varchar2(1),	/*״̬			                                */
	filler1		varchar2(20),	/*����1			                                */
	filler2		varchar2(20)	/*����2		                                        */
)tablespace users;
create index mob_chg_rel_1 on mob_chg_rel( main_ac_id,app_ac_id);


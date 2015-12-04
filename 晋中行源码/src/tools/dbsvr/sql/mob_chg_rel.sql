drop index mob_chg_rel_1;
drop table mob_chg_rel;
create table mob_chg_rel
(
	main_ac_id	Number(9),	/*主账号AC_ID						*/
	app_ac_id	Number(9),	/*从账号AC_ID		                                */
	sts		varchar2(1),	/*状态			                                */
	filler1		varchar2(20),	/*备用1			                                */
	filler2		varchar2(20)	/*备用2		                                        */
)tablespace users;
create index mob_chg_rel_1 on mob_chg_rel( main_ac_id,app_ac_id);


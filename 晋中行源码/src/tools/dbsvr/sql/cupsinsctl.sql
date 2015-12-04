/***银联卡交易流水控制表***/
drop index  cupsinsctl_idx;
drop table cupsinsctl;
create table cupsinsctl(
	trseq 		number(6),	/*参数序号*/
	brtype		varchar2(1),	/*0-发卡方 1-受理方 */
	lstinsday	varchar2(8),	/*最后一次插入日 */
	name1		varchar2(11),	/*文件名1-11位 */
	name2		varchar2(1),	/*文件名12位 */
	name3		varchar2(10),	/*文件名12位后的取值 */
	tabname		varchar2(20)	/*该文件对应插入哪个表 */
)tablespace users;
create unique index cupsinsctl_idx on cupsinsctl(trseq) tablespace indx;

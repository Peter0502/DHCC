drop index lv_c503r_idx;	/**业务权限控制表**/
drop table lv_c503r;
create table lv_c503r(
	bakcode        varchar2(12),	/**参与者或CCPC代码**/
	sendcnt        number(6),	/**发起权限数目**/
	sendgrant      varchar2(2000),	/**发起权限**/
	recvcnt        number(6),	/**接收权限数目**/
	recvgrant      varchar2(2000),	/**接收权限**/
	upd_date       varchar2(8),	/**更新日期**/
	upd_node       varchar2(4)	/**更新结点**/
)tablespace users;
create unique index lv_c503r_idx on lv_c503r(bakcode);


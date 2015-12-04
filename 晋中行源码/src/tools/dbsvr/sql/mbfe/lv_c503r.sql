     /**CMT503 业务权限控制通知登记簿 **/
drop index lv_c503r_idx;
drop table lv_c503r;
create table lv_c503r
(
	bakcode   varchar2(12),     /**代码  **/
	sendcnt   number(6),        /**发送数目 >=0表示状态正常  **/
	sendgrant varchar2(2000),   /**发送  **/
	recvcnt   number(6),        /**接收数目  **/
	recvgrant varchar2(2000),   /**接收  **/
	upd_date  varchar2(8),      /**更新日期  **/
	upd_node  varchar2(4),       /**更新节点  **/
)tablespace users;
create unique index lv_c503r_idx on lv_c503r(bakcode)tablespace indx;

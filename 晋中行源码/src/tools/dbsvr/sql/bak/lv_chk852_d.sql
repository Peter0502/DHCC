      /**	小额包明细核对申请报文明细请求清单登记簿**/
drop index lv_chk852_d_idx;
drop table lv_chk852_d;
create table lv_chk852_d(
	lw_flg        varchar2(1),  /* 来往帐务表识 1往帐2来帐 */
	pkgno         varchar2(3),  /* 包类型号 */
	pay_qs_no     varchar2(12), /* 发起清算行 */
	pack_date     number(8),  /* 委托日期    **/
	packid        varchar2(8),  /* 包id */
	filler      varchar2(60)  /* 备注            **/
)tablespace users;
create unique index lv_chk852_d_idx on lv_chk852_d(pay_qs_no,packid,pack_date)tablespace indx;


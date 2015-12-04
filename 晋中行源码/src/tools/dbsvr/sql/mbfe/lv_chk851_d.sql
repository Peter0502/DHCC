      /**	小额明细核对应答报文登记簿**/
drop index lv_chk851_d_idx;
drop table lv_chk851_d;
create table lv_chk851_d(
	lw_flg        varchar2(1),  /* 来往帐务表识 1往帐2来帐 */
	zc_jd_lx      varchar2(1),  /* 扎差节点类型 */
	zc_date       number(8),  /* 轧差日期    **/
	zc_cc         varchar2(2),  /* 轧差场次 */
	pkgno         varchar2(3),  /* 包类型号 */
	cnt           number(8)  ,  /* 本类包数目 */
	pay_qs_no     varchar2(12), /* 发起清算行 */
	pack_date     number(8),  /* 委托日期    **/
	packid        varchar2(8),  /* 包id */
	dtlcnt        number(8),    /* 明细业务总笔数 */
	tot_amt       number(24,2), /* 包总金额*/
	qs_date       number(8),  /* 清算日期 */
	pack_sts      varchar2(2),  /* ccpc包状态 */
	dhcc_sts      varchar2(2),  /* dhcc包状态00 表示要重新发送  其他和ccpc标识一致*/
	filler        varchar2(60)  /* 备注            **/
)tablespace users;
create unique index lv_chk851_d_idx on lv_chk851_d(pack_date,pay_qs_no,packid,lw_flg)tablespace indx;

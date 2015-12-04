     /**小额轧差净额清算通知接收登记簿 **/
drop index lv_c254r_idx;                                 
drop index lv_c254r_idx2;
drop table lv_c254r;
create table lv_c254r(
	cmtno			Varchar2(3)	, /** Cmt编号			**/
	in_date			number(8)	, /** 进入系统日期		**/
	wt_date			number(8)	, /** 委托日期			**/
	dtlid			Number(3)	, /** 明细序号			**/
	o_pkgno			Varchar2(3)	, /** 原包类型号		**/
	o_wtdate		number(8)	, /** 原包委托日期		**/
	o_pay_qs_no		Varchar2(12)	, /** 原包发起清算行	**/
	zc_type			Varchar2(1)	, /** 轧差类型			**/
	zc_node			Varchar2(1)	, /** 轧差节点类型		**/
	zc_date			number(8)	, /** 轧差日期			**/
	zc_id			Varchar2(8)	, /** 轧差场次			**/
	crdb			Varchar2(1)	, /** 借贷标志			**/
	zc_amt			Number(17,2)	, /** 轧差金额			**/
	o_packid		Varchar2(8)	, /** 原包序号			**/
	qs_date			number(8)	, /** 清算日期			**/
	qs_sts			Varchar2(1)	, /** 清算处理状态		**/
	tx_time			Varchar2(14)	, /** 交易时间			**/
	brno			Varchar2(7)	, /** 交易机构			**/
	tel			Varchar2(6)	, /** 接收操作员号	    **/
	tr_ac_no		Varchar2(6)	, /** 接收流水号		**/
	sts			Varchar2(1)	, /** 交易状态			**/
	brprt_flg		Varchar2(64)	, /** 支行打印标志		**/
	qsprt_flg		Varchar2(64)	, /** 清算中心打印标志	**/
	filler			Varchar2(60)  /** 备注            	**/
)tablespace users;
create unique index lv_c254r_idx on lv_c254r(wt_date,o_packid,dtlid)tablespace indx;
create index lv_c254r_idx2 on lv_c254r(in_date,brno)tablespace indx;

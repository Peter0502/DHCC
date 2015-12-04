/**	CMT554小额对外返还通知报文登记簿 **/
drop index lv_c554r_idx;
drop index lv_c554r_idx2;
drop table lv_c554r;
create table lv_c554r(
	cmtno      	Varchar2(3),/**  Cmt编号	**/
	in_date		number(8),/**  进入系统日期**/
	wt_date		number(8),/**  委托日期	**/
	brno            Varchar2(7) ,/**  接收行号  **/
	tlrno           Varchar2(6) ,/**  操作员    **/
	sendco		Varchar2(4) ,/**  发起者编号**/
	acbrno		Varchar2(12),/**  接收者编号**/
	iftype		Varchar2(1),/**  通知类型 1 付款通知 2 收款通知 3 付款回执 4 收款回执 **/
	dtlid		Number(3)   ,/**  明细序号	**/
	payno		Varchar2(12),/**  付款行行号	**/
	paymon		Varchar2(6), /**  计费月份	**/
	sdate		Varchar2(8), /**  计费起始日期	**/
	edate		Varchar2(8), /**  计费中止日期	**/
	aorm		Varchar2(1),/**  自动/手工扣费 0自动 1 手工**/
	rebakno		Varchar2(12),/**  返还行号**/
	rebamt		Number(17,2),/**  返还总金额**/
	rebrate		Varchar2(6), /**  直接参与者返还比率 **/
	rebnum		Varchar2(6),/**  返还记录数**/
	seqno		Number(6)   ,/**  返还记录序号	**/
	pkgno		Varchar2(3),/**  汇费返还清单之报文类型号	**/
	totnum		Varchar2(8) ,/**  汇费返还清单之总笔数	**/
	brate		Varchar2(6),/**  汇费返还清单之返还基础费率单位：分/笔**/
	price		Varchar2(6),/**  汇费返还清单之计费单价单位：分/笔 	**/
	txamt		Varchar2(15)  /**  汇费返还清单之计费金额  单位：分		**/
)tablespace users;
create unique index lv_c554r_idx on lv_c554r(in_date,dtlid,seqno)tablespace indx;

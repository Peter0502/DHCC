/***银联卡错帐处理交易流水***/
drop index  cupserr_idx;
drop table cupserr;
create table cupserr(
	brtype 		varchar2(1),	/* 0-发卡方 1-受理方 */
	errflg		varchar2(1),	/* 差错交易标志 */
	filename	varchar2(30),	/* 银联明细文件名称 */
	txday		varchar2(8),	/* 交易日期 */
	brno		varchar2(8),    /* 交易机构或开户机构 */
	dlbrno		varchar2(32),	/* 代理机构标识码 32*/
	fsbrno		varchar2(32),	/* 发送机构标识码 33*/
	trseq 		varchar2(11),	/* 系统跟踪号 11*/
	txtime		varchar2(10),	/* 交易传输时间 7*/
	cardno		varchar2(19),	/* 主帐号 2*/
	txamt		number(16,2),	/* 交易金额 4*/
	fee		number(16,2),	/* 持卡人交易手续费 28*/
	msgtype		varchar2(4),	/* 报文类型 */
	
	
	
	txcode		varchar2(6),	/* 交易类型码 3*/
	mertype		varchar2(4),	/* 商户类型 18*/
	atmno		  varchar2(8),	/* 受卡机终端标识码 41*/
	wssrno		varchar2(12),	/* 检索参考号 37*/
	inputcd		varchar2(2),	/* 服务点条件码 25*/
	authcd		varchar2(6),	/* 授权应答码 38*/
	isbkno		varchar2(11),	/* 接收机构标识码 100*/
	opnbrno		varchar2(11),	/* 发卡机构标识码 100*/
	otrseq		varchar2(6),	/* 原始交易的系统跟踪号 90.2*/
	respcd		varchar2(2),	/* 交易返回码 39*/
	inputmd		varchar2(3),	/* 服务点输入方式 22*/
	insxfee		number(16,2),	/* 应收手续费 */
	outsxfee	number(16,2),	/* 应付手续费 */
	zjfee		  number(16,2),	/* 转接服务费 */
	infee		  number(16,2),	/* 应收费用 */
	outfee		number(16,2),	/* 应付费用 */
	errreasn	varchar2(4),	/* 差错原因 */
	cardseq		varchar2(3),	/* 卡片序列号 23*/
	tmreadfg	varchar2(1),	/* 终端读取能力 60.2.2*/
	iccardfg	varchar2(1),	/* IC卡条件代码 60.2.3*/
	otxtime		varchar2(10),	/* 原始交易时间*/
	oqstime		varchar2(10),	/* 原始交易清算日期15*/
	otxday		varchar2(10),	/* 原始交易日期 90.3*/
	otxamt		number(16,2),	/* 原始交易金额 4*/
	eciflag		varchar2(2),	/* ECI标志 */
	outbrno		varchar2(11),	/* 接收/转出机构标识码 100*/
	outcardno	varchar2(19),	/* 转出卡号 102 */
	inbrno		varchar2(11),	/* 转入机构标识码 100*/
	incardno	varchar2(19),	/* 转入卡号 103*/
	areaflg		varchar2(1),	/* 交易地域标志 
0－非跨境一般交易，1－跨境一般交易，2－同城转账、3－异地转账，4－跨境转账**/
	tmp		varchar2(300)	/* 保留使用 */
)tablespace users;
create unique index cupserr_idx on cupserr(brtype,trseq,txtime) tablespace indx;

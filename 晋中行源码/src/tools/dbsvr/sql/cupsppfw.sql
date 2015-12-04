/***银联卡品牌服务费交易流水***/
drop index  cupsppfw_idx;
drop table cupsppfw;
create table cupsppfw(
	brtype 		varchar2(1),	/* 0-发卡方 1-受理方 */
	filename	varchar2(30),	/* 银联明细文件名称 */
	txday		  varchar2(8),	/* 交易日期 */
	brno		  varchar2(8),  /* 交易机构或开户机构 */
	firstbr		varchar2(11),	/* 受理/发卡方一级分行代码 */
	cardtype	varchar2(1),	/* 是否银联标准卡 */
	cdtype		varchar2(1),	/* 借贷记卡标志 */
	jyqd		  varchar2(2),	/* 交易渠道 60.2.5*/
	cardjz		varchar2(1),	/* 卡介质 */
	outtxfg		varchar2(1),	/* 跨境交易标志 */
	mertype		varchar2(4),	/* 商户类型 18*/
	secbr		  varchar2(11),	/* 受理/发卡方二级分行代码 */
	dlbrno		varchar2(32),	/* 代理机构标识码 32*/
	fsbrno		varchar2(32),	/* 发送机构标识码 33*/
	opnbrno		varchar2(11),	/* 发卡机构标识码 */
	isbkno		varchar2(11),	/* 接收机构标识码 100*/
	sdbrno		varchar2(11),	/* 收单机构代码 */
	msgtype		varchar2(4),	/* 报文类型 */
	txcode		varchar2(6),	/* 交易类型码 3*/
	inputcd		varchar2(2),	/* 服务点条件码 25*/
	trseq 		varchar2(11),	/* 系统跟踪号 11*/
	txtime		varchar2(10),	/* 交易传输时间 7*/
	cardno		varchar2(19),	/* 主帐号 2*/
	outcardno	varchar2(19),	/* 转出卡号 102*/
	incardno	varchar2(19),	/* 转入卡号 103*/
	otxmsg		varchar2(42),	/* 原始交易信息 90*/
	atmno		  varchar2(8),	/* 受卡机终端标识码 41*/
	acbkflg		varchar2(15),	/* 受卡方标识码 42*/
	wssrno		varchar2(12),	/* 检索参考号 37*/
	txamt		  number(16,2),	/* 交易金额 4*/
	slfee		  number(16,2),	/* 受理方手续费 */
	pinpfee		number(16,2),	/* 品牌服务费 */
	jamt		  number(16,2),	/* 净金额*/
	tmp1		  varchar2(12),	/* 保留使用 */
	tmp2		  varchar2(100)	/* 保留使用 */
)tablespace users;
create unique index cupsppfw_idx on cupsppfw(brtype,trseq,txtime) tablespace indx;

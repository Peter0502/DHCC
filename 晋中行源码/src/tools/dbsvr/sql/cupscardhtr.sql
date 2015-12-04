/***银联卡交易流水***/
drop index  cupscardhtr_idx;
drop table cupscardhtr;
create table cupscardhtr(
	brtype 		varchar2(1),	/* 0-发卡方(转入方) 1-受理方(转出方) */
	filename	varchar2(30),	/* 银联明细文件名称 */
	txday		varchar2(8),	/* 交易日期*/
	brno		varchar2(8),	/* 交易机构或开户机构 */
	dlbrno		varchar2(32),	/* 代理机构标识码 32*/
	fsbrno		varchar2(32),	/* 发送机构标识码 33*/
	trseq 		varchar2(11),	/* 系统跟踪号 11*/
	txtime		varchar2(10),	/* 交易传输时间 7*/
	cardno		varchar2(19),	/* 主帐号 2*/
	txamt		number(16,2),	/* 交易金额 4*/
	dsamt		number(16,2),	/* 部分代收时的承兑金额 95*/
	fee		number(16,2),	/* 持卡人交易手续费 28*/
	msgtype		varchar2(4),	/* 报文类型 */
	txcode		varchar2(6),	/* 交易类型码 3*/
	mertype		varchar2(4),	/* 商户类型 18*/
	atmno		varchar2(8),	/* 受卡机终端标识码 41*/
	acbkflg		varchar2(15),	/* 受卡方标识码 42*/
	acbkaddr	varchar2(40),	/* 受卡方名称地址 43*/
	wssrno		varchar2(12),	/* 检索参考号 37*/
	inputcd		varchar2(2),	/* 服务点条件码 25*/
	authcd		varchar2(6),	/* 授权应答码 38*/
	isbkno		varchar2(11),	/* 接收机构标识码 100*/
	otrseq		varchar2(6),	/* 原始交易的系统跟踪号 90.2*/
	respcd		varchar2(2),	/* 交易返回码 39*/
	inputmd		varchar2(3),	/* 服务点输入方式 22*/
	infee		number(16,2),	/* 应收手续费 */
	outfee		number(16,2),	/* 应付手续费 */
	zjfee		number(16,2),	/* 转接服务费 */
	dsflag		varchar2(1),	/* 单双转换标志 */
	cardseq		varchar2(3),	/* 卡片序列号 23*/
	tmreadfg	varchar2(1),	/* 终端读取能力 60.2.2*/
	iccardfg	varchar2(1),	/* IC卡条件代码 60.2.3*/
	otxtime		varchar2(10),	/* 原始交易日期时间 90.3*/
	opnbrno		varchar2(11),	/* 发卡机构标识码 */
	outtxfg		varchar2(1),	/* 境外交易标志 */
	jyqd		varchar2(2),	/* 交易渠道 60.2.5*/
	eciflag		varchar2(2),	/* ECI标志 */
	curcd 		varchar2(3),	/* 交易货币代码 49*/
	dltxday		varchar2(4),	/* 受卡方所在地日期 13*/
	dltxtime	varchar2(6),	/* 受卡方所在地时间 12*/
	qsday		varchar2(4),	/* 清算日期 15*/
	sqresn		varchar2(3),     /* 代授权原因 000:正常代授权 001:异常代授权 */
	certtype	varchar2(2),	/* 证件种类 61.1*/
	certnum		varchar2(20),	/* 证件编号 61.2*/
	dldsflg		varchar2(1),	/* 受卡方单双标志 0-单信息 1-双信息 */
	clzxseq		varchar2(9),	/* 处理中心流水号 */
	opncdbk		varchar2(11),	/* 发卡银行代码 */
	feeflg		varchar2(1),	/* 收费标志  0-无需收取代授权费用 1-需要收取代授权费用 */
	tzxeflg		varchar2(1),	/* 是否高于通知限额标志 0-不高于代授权通知限额 1-高于代授权通知限额 */
	djyobj		varchar2(30),	/* 代校验项目 用于通知发卡方中心所做的代校验项目， 每一位的取值代表一个代授权项目，第1位至第12位依次表示：
			 代授权校验PVV
			 代授权校验CVV
			 代授权校验CVV2
			 代授权校验ARQC
			 代授权校验卡号长度
			 代授权校验卡号校验位
			 代授权校验卡有效期
			 代授权校验pin重试次数
			 代授权校验ARPC
			 止付名单检查
			 VIP文件检查
			 交易限额检查
			 每一位的取值有两种：
			 Y-中心已按照协议的要求做了该项代授权
			 N-协议未要求做该项代授权
			 其它位保留使用，以缺省值填充。 */
	dsqfee		number(16,2),	/* 代授权费用 */
	outbrno		varchar2(11),	/* 转出机构标识码 100*/
	outcardno	varchar2(19),	/* 转出卡号 102*/
	inbrno		varchar2(11),	/* 转入机构标识码 100*/
	incardno	varchar2(19),	/* 转入卡号 103*/
	cityfg		varchar2(1),	/* 同城异地标志 */
	tmp		varchar2(300)	/* 保留使用 */
)tablespace users;
create unique index cupscardhtr_idx on cupscardhtr(brtype,trseq,txtime,txcode) tablespace indx;

 
/**股金分户帐：in_stok.sql*/
drop table in_stok;
create table in_stok (
stokno				varchar2(18),			/*股东证号	                                          */
ostokno				varchar2(18),			/*旧股东号	                                            */
name					varchar2(60),			/*股东名称	                                              */
stcktype			varchar2(3),			/*股东类别	第一位：1：国有，2：法人，3：个体工商，4：职工，5：个人，6：其它 
 第二位：1：普通股，2：优先股  第三位：1：保息股，2：普通股     */
idtype				varchar2(2),			/*证件类型	                                              */
idno					varchar2(20),			/*身份证/企业代码	                                        */
addr					varchar2(60),			/*股东地址	                                              */
stokquant			number(16,2),			/*股数	                                              */
stokamt				number(16,2),			/*每股价格	                                            */
bal						number(16,2),			/*认购金额	                                                */
stat					varchar2(1)	,			/*状态	0：正常，1：退股销户，2：转让销户，3：继承销户      */
losssts				varchar2(1)	,			/*挂失标志	0：正常，1：挂失                              */
holdsts				varchar2(1)	,			/*冻结标志	0：正常，1：全部冻结，2：部分冻结，3：只进不出*/
ctlamt				number(16,2),			/*控制金额	                                            */
txcode				varchar2(2)	,			/*交易代码	目前可不填                                    */
txtype				varchar2(1),			/*交易种类	目前可不填                                    */
bookbal				number(16,2),			/*存折余额	                                            */
nowcnt				number(4,0)	,			/*当前笔数	                                              */
bookcnt				number(4,0)	,			/*存折行数	                                              */
unprtcnt			number(4,0)	,			/*未登折笔数	                                          */
pactno				varchar2(19),			/*转出账号	开户所需资金帐号                            */
tactno				varchar2(19),			/*活期账号	接收红利帐号                                */
opndate				varchar2(8)	,			/*开户时间	                                              */
txdate				varchar2(8)	,			/*交易时间	                                              */
drawpwdflag		varchar2(8)	,			/*密印标志	0：密码，1：印鉴，2：全有，3：全无        */
pwd						varchar2(8)	,			/*密码	                                                      */
notenum				varchar2(12),			/*凭证号	                                              */
txbrno				varchar2(7)	,			/*机构号	                                                */
tel						varchar2(6)	,			/*操作员	                                                    */
cifno					varchar2(10)			/*客户号	   */
) tablespace users;
create unique index in_stok_idx on in_stok(stokno) tablespace indx;

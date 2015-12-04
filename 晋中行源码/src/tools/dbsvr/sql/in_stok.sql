 
/**�ɽ�ֻ��ʣ�in_stok.sql*/
drop table in_stok;
create table in_stok (
stokno				varchar2(18),			/*�ɶ�֤��	                                          */
ostokno				varchar2(18),			/*�ɹɶ���	                                            */
name					varchar2(60),			/*�ɶ�����	                                              */
stcktype			varchar2(3),			/*�ɶ����	��һλ��1�����У�2�����ˣ�3�����幤�̣�4��ְ����5�����ˣ�6������ 
 �ڶ�λ��1����ͨ�ɣ�2�����ȹ�  ����λ��1����Ϣ�ɣ�2����ͨ��     */
idtype				varchar2(2),			/*֤������	                                              */
idno					varchar2(20),			/*���֤/��ҵ����	                                        */
addr					varchar2(60),			/*�ɶ���ַ	                                              */
stokquant			number(16,2),			/*����	                                              */
stokamt				number(16,2),			/*ÿ�ɼ۸�	                                            */
bal						number(16,2),			/*�Ϲ����	                                                */
stat					varchar2(1)	,			/*״̬	0��������1���˹�������2��ת��������3���̳�����      */
losssts				varchar2(1)	,			/*��ʧ��־	0��������1����ʧ                              */
holdsts				varchar2(1)	,			/*�����־	0��������1��ȫ�����ᣬ2�����ֶ��ᣬ3��ֻ������*/
ctlamt				number(16,2),			/*���ƽ��	                                            */
txcode				varchar2(2)	,			/*���״���	Ŀǰ�ɲ���                                    */
txtype				varchar2(1),			/*��������	Ŀǰ�ɲ���                                    */
bookbal				number(16,2),			/*�������	                                            */
nowcnt				number(4,0)	,			/*��ǰ����	                                              */
bookcnt				number(4,0)	,			/*��������	                                              */
unprtcnt			number(4,0)	,			/*δ���۱���	                                          */
pactno				varchar2(19),			/*ת���˺�	���������ʽ��ʺ�                            */
tactno				varchar2(19),			/*�����˺�	���պ����ʺ�                                */
opndate				varchar2(8)	,			/*����ʱ��	                                              */
txdate				varchar2(8)	,			/*����ʱ��	                                              */
drawpwdflag		varchar2(8)	,			/*��ӡ��־	0�����룬1��ӡ����2��ȫ�У�3��ȫ��        */
pwd						varchar2(8)	,			/*����	                                                      */
notenum				varchar2(12),			/*ƾ֤��	                                              */
txbrno				varchar2(7)	,			/*������	                                                */
tel						varchar2(6)	,			/*����Ա	                                                    */
cifno					varchar2(10)			/*�ͻ���	   */
) tablespace users;
create unique index in_stok_idx on in_stok(stokno) tablespace indx;

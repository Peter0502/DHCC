
/**�ɽ���ϸ�ʣ�in_stok_hst.sql*/
drop table in_stok_hst;
create table   in_stok_hst (    
stokno				varchar2(18),			/*�ɶ�֤��	                          */
seqno					number(6,0),			/*���	ÿ�����±�ţ����йɶ��ʻ�ͳһ���*/
dcind					varchar2(1),			/*�����־	1���裬2����                  */
txcode				varchar2(2),			/*���״���	Ŀǰ�ɲ���                  */
txtype				varchar2(1),			/*��������	Ŀǰ�ɲ���                  */
txamt					number(16,2),			/*������	                              */
txdate				varchar2(8),			/*��������	                            */
txtime				varchar2(8),			/*����ʱ��	                            */
bookcnt				number(2,0),			/*��������	�������ڴ�������            */
stcktype			varchar2(3),			/*�ɶ����	��һλ��1�����У�2�����ˣ�3�����幤�̣�4��ְ����5�����ˣ�6������  �ڶ�λ��1����ͨ�ɣ�2�����ȹ� ����λ��1����Ϣ�ɣ�2����ͨ��                                      */
name					varchar2(60),			/*�ɶ�����	                            */
brf						varchar2(20),			/*ժҪ	                                  */
bookbrf				varchar2(20),			/*����ժҪ	                          */
bal						number(16,2),			/*��ǰ���	                              */
txbrno				varchar2(7),			/*������	                              */
tel						varchar2(6)				/*����Ա	       */
) tablespace users;
create unique index in_stok_hst_idx on in_stok_hst(stokno,seqno,txdate) tablespace indx;  

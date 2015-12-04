/**�ɽ�δ������ϸ��mdm_stok_unprt_hst.sql*/
drop  table mdm_stok_unprt_hst;
create table mdm_stok_unprt_hst(
stokno						varchar2(18),						/*�ɶ�֤��	                                    */
unprtno						number(4,0)	,						/*δ�������	���ɶ�֤Ŀǰ����δ���۵���Ϣͳһ���*/
txday							varchar2(8)	,						/*��������	                                        */
txbrno						varchar2(7)	,						/*������	                                        */
tel								varchar2(6)	,						/*����Ա��	                                          */
dcind							varchar2(1)	,						/*�����־	1���裬2����                            */
txamt							number(16,2),						/*������	                                        */
bookbrf						varchar2(20),						/*����ժҪ	                                    */
nowcnt						number(4,0)	,						/*����λ��	�Ǻ�����ʱ�����Ҫ����ֶ����ʹ��    */
flag							varchar2(1)							/*�ɽ𡢺�����־	0���ɽ�1������    */
) tablespace users;
create unique index mdm_stok_unprt_hst_idx on mdm_stok_unprt_hst(stokno,unprtno,flag) tablespace indx;              

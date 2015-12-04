/* 3.1.29�����Ǽǲ� 	cassreg*/
drop index  cassreg_idx; 
drop table cassreg;
create table cassreg(
    br_no	Varchar2(5)	,/*��������	*/
    ac_id 	number(9)	,/*�����ʻ�	*/
    name	Varchar2(60),	/*������λ����	*/
    pact_no	Varchar2(20)	,/*��ݺ�	*/
    bw_ac_id	number(9)	,/* ������λ�ͻ��� */
    in_tx_date	number(8)	,/*��������	*/
    out_tx_date	number(8)	,/*�����������	*/
    amt		Number(16,2)	,/*�������	*/
    bal		Number(16,2)	,/*��������ʱ���*/
    tel  	Varchar2(4)	,/*����Ա	*/
    flag  	Varchar2(1)	,/* 0����  1��� */
    in_trace_no	number(6),	/* ����ʱ��ˮ�� */
    out_trace_no	number(6)	/* ���ʱ��ˮ�� */
)tablespace users;
create index cassreg_idx on cassreg(pact_no) tablespace indx;

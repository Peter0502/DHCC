drop table draw_loan_limit;
drop index draw_loan_limit_idx;
create table draw_loan_limit(
pact_no 	varchar2(20),	/**�����ݺ�                    **/                         	
ac_no 		varchar2(19),	/**����˺�           		 **/
ac_id 		number(9),	/**����˻�id         		 **/
save_amt	decimal(16,2),	/**�ſ���           		 **/
trust_amt    	decimal(16,2),	/**ί��֧�����       		 **/
self_amt     	decimal(16,2),	/**����֧�����       		 **/
trust_draw_amt	decimal(16,2),	/**��֧ȡί��֧�����		 **/
self_draw_amt	decimal(16,2),	/**��֧ȡ����֧�����		 **/
sts          	varchar2(1),	/**״̬	X:�Ŵ��������
				1.���������� 2.�ѷ��� *.֧ȡ��� **/
br_no        	varchar2(5),	/**�������� 		         **/
tel          	varchar2(6),	/**������Ա 		         **/
tx_date      	number(9),	/**ǩ������ 		         **/
brf1         	varchar2(60),	/**��ע1              		 **/
brf2         	varchar2(60)	/**��ע2              		 **/
)tablespace users;
create unique index draw_loan_limit_idx on draw_loan_limit (pact_no) tablespace indx;

	/**С��ϵͳ���Ʊ�**/
drop table lv_sysctl;
create table lv_sysctl(
   	tx_date     number(8) , /*��������*/ 
	stat        varchar2(1) , /*����״̬*/ 
	next_date   number(8) , /*��һ����������*/
	chgnode     varchar2(4) , /*����ڵ�*/  
	curstat     varchar2(8) , /*Ӫҵ״̬*/  
	chgtime     varchar2(14), /*���ʱ��*/ 
	message	    varchar2(60), /*��������*/  
	filler	    varchar2(100) /*��ע*/  
)tablespace users;

/****��ӡ��ҳ�ʸ���***/
drop index print_full_page_idx;
drop table print_full_page;      
create table print_full_page(    
	ac_id		number(9),		/*�ͻ�ID*/
	hst_cnt		number(9),		/*��ǰ��С��δ��ӡ��ϸ�ı�־��*/
	pagecnt		number(9),		/*�Ѿ���ӡ����ҳ�ʵ�ҳ��*/
	lst_bal		number(16,2),	/*��ҳ���һ�ʵ����*/
	trace_no	number(9),		/*��ǰ��Сδ��ӡ�ļ�¼����ˮ��*/
	tx_date		number(9),		/*��ǰ��Сδ��ӡ�ļ�¼������*/
	tx_time		number(9),		/*��ǰ��Сδ��ӡ�ļ�¼��ʱ��*/
	filler1		varchar(10),	/*�����ֶ�1*/
	filler2		varchar(20),	/*�����ֶ�2*/
	filler3		varchar(30)		/*�����ֶ�3*/
)tablespace users;
create unique index print_full_page_idx on print_full_page(ac_id) tablespace indx;

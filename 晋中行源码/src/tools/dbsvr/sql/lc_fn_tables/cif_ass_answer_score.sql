drop table cif_ass_answer_score;
create table cif_ass_answer_score
(
	qt_code		varchar2(3),	/*������			*/
    qt_count    number(2),  /*��Ŀ����*/
	qt_num		number(2),	/*��Ŀ���	                */
	answer_num	varchar2(1),	/*�𰸱��	A\B\C\D\E       */
	score		number(4)	/*��ֵ		                */
)tablespace users;
create Unique index cif_ass_answer_score_1 on cif_ass_answer_score(qt_code,qt_num,answer_num)
tablespace indx;

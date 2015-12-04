drop table cif_ass_answer_score;
create table cif_ass_answer_score
(
	qt_code		varchar2(3),	/*试题编号			*/
    qt_count    number(2),  /*题目数量*/
	qt_num		number(2),	/*题目编号	                */
	answer_num	varchar2(1),	/*答案编号	A\B\C\D\E       */
	score		number(4)	/*分值		                */
)tablespace users;
create Unique index cif_ass_answer_score_1 on cif_ass_answer_score(qt_code,qt_num,answer_num)
tablespace indx;

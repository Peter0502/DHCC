drop table ass_score_lvl_rel;
create table ass_score_lvl_rel
(
qt_code		varchar2(3),	/*试题编号						*/	
min_score	number(4),	/*小分值		                                */
max_score	number(4),	/*大分值		                                */
ass_lvl		varchar2(1)	/*风险承受等级		1：小于等于20分评级结果保守型   */
				/*			2：21分--45分评级结果稳健型     */
				/*			3：46分--70分评级结果平衡型     */
				/*			4：71分--85分评级结果成长型     */
				/*			5：86分--100分评级结果进取型    */
)tablespace users;
create unique index ass_score_lvl_rel_1 on ass_score_lvl_rel(qt_code,min_score,max_score) tablespace indx;

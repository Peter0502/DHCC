#ifndef __cif_ass_answer_score_CH__
#define __cif_ass_answer_score_CH__
struct cif_ass_answer_score_c{
        char      rowid[20];
        char      qt_code[4];
        long      qt_count;
        long      qt_num;
        char      answer_num[2];
        long      score;
};
#endif 

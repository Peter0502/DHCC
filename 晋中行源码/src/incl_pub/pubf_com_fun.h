/** com目录下 函数声明部分 **/

/** pubf_com_note.c **/
int pub_com_CalNoteCnt(char *note_type,char *beg_note_no,char *end_note_no );
int pub_com_NoteActSts( char *who,char *note_act, char *note_sts,char *p_note_sts );
/*int pub_com_NoteApart(char *note_act,struct note_mst_c note_mst);
int pub_com_NoteApartMerg( struct note_mst_c note_mst );
*/
int pub_com_NoteMerg( struct note_mst_c note_mst );
int pub_com_CalNoteNo(char *note_type,char *note_no,char *re_note_no,int  cnt);
int pub_com_RegTelNote( char *tel,char *br_no, char *pNoteAct,char *note_type,char *beg_no,char *end_no );
int pub_com_ChkNoteMin( char *note_type,char *beg_note_no,char *tel);
int pub_com_ChkNoteHead(char *note_type,char *note_no,char *tel);
int pub_com_RegNoteMin(char *br_no,char *note_type,char *note_no,char *tel,char *in_out);
int pub_com_NoteMstSts(char *note_type,char *beg_note_no,char *end_note_no,char *tel,char *sts);
int pub_com_RegNoteHst( char *note_act,struct note_mst_c note_mst,char *tw_br_no,char *tw_tel );
int pub_com_ChkNoteUse( char *note_type );
int pub_com_ChkBoeNote(char *note_type,char *beg_note_no,char *end_note_no);
int pub_com_GetNoteMin( char *note_type,char *note_no,char *tel);
int pub_com_GetNoteHst(	long tx_date, char *note_type, char *beg_note_no, \
			char *end_note_no, char *tw_br_no, char *tw_tel, \
			char *note_act, struct note_mst_hst_c *note_mst_hst );
int pub_com_ChkPile( char *note_type, char *beg_note_no, char *end_note_no);

/** pubf_com_cheq.c **/
int pub_com_ChkCheqHead(long ac_id, int ac_seqn, char *note_type,char *cheq_no);
int pub_com_ChkAcCheq(long ac_id, int ac_seqn, char *note_type,char *cheq_beg_no,char *cheq_end_no,char *book_sts);
int pub_com_RegCheq( struct note_mst_c note_mst, int ac_id, long ac_seqn, long tx_date, char *note_act);

int pub_com_RegCheqHst( char *note_act,struct note_cheq_mst_c cheq_mst,char *tw_br_no,char *tw_tel );
int pub_com_ChkCheqSell(long ac_id,int ac_seqn, char *note_type,char *beg_note_no,char *end_note_no);
int pub_com_CalPileCnt(struct note_parm_c note_parm,char *beg_note_no,char *end_note_no);
int pub_com_CalAcGrp(long ac_id,int ac_seqn, char *note_type);
int pub_com_CalAcGrp(long ac_id,int ac_seqn, char *note_type);

/** pubf_com_noteUse.c **/
int pub_com_NoteClsNo( char *note_type,char *beg_no,char *end_no,char *tel );
int pub_com_CheqClsNo( long ac_id,int ac_seqn, char *note_type,char *cheq_beg_no,char *cheq_end_no );
int pub_com_NoteUse(long ac_id, int ac_seqn, char *note_type, char *beg_no, char *end_no, char *tel);

/** pubf_com_noteUndo.c **/
int pub_com_NoteUndo( char *note_type,long trace_no, long trace_cnt, long tx_date, char *hst_flag );
int pub_com_CheqUndoNo( long trace_no, long trace_cnt, long tx_date, char *hst_flag );
int pub_com_NoteUndoNo( long trace_no, long trace_cnt, long tx_date, char *hst_flag );

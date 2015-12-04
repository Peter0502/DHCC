drop index seqctl_idx;
dbaccess << --
!
{ ÐòºÅ¿ØÖÆµ² }
database dhccb;
drop table seqctl;
create table seqctl
(
	br_no       char(5),{ »ú¹¹ºÅ }
	ac_ind      char(2),{ ÐòºÅÀàÐÍ,99-´ó¶îÖ§¸¶½»Ò×ÍùÕÊÐòºÅ£¬98-´ó¶îÖ§¸¶½»Ò×²Î¿¼ºÅ }
	seqno       integer	{ ÐòºÅË³ÐòºÅ }
);
close database;
!

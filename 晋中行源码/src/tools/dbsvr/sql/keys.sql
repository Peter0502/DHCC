drop index keys_idx;
drop table keys;
create table keys(
	keytype number(4),
	keyvalue varchar(16)
)tablespace users;
create unique index keys_idx on keys(keytype);

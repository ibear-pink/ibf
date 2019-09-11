--################################# iMDB ################################################
drop table bbb1;
create table bbb1(
f1 	varchar(15),
f2 	numeric(9,2),
f3 	date,
primary key(f1,f2)
);
create index in1 on bbb1(f1);

drop table bbb2;
create table bbb2(
aa 	varchar(15),
bb 	date,
primary key(aa)
);
create index in2 on bbb2(aa);

drop table bbb3;
CREATE TABLE BBB(
F1      VARCHAR(15),
F2      NUMERIC(9),
F3      DATE,
PRIMARY KEY(F1)
);
CREATE INDEX IN1 ON BBB(F1);

--################################# Mysql ################################################
drop table bbb1;
create table bbb1(
f1 	varchar(15),
f2 	numeric(9,2),
f3 	datetime,
primary key(f1,f2)
)TYPE=INNODB;

drop table bbb2;
create table bbb2(
aa 	varchar(15),
bb 	datetime,
primary key(aa)
)TYPE=INNODB;

drop table bbb;
create table bbb(
f1      varchar(15),
f2      numeric(9),
f3      datetime,
primary key(f1)
)TYPE=INNODB;

--################################# Oracle Лђеп Altibase ################################################
drop table bbb1;
create table bbb1(
f1 	varchar(15),
f2 	numeric(9,2),
f3 	date,
primary key(f1,f2)
);

drop table bbb2;
create table bbb2(
aa 	varchar(15),
bb 	date,
primary key(aa)
);

drop table bbb3;
CREATE TABLE BBB(
F1      VARCHAR(15),
F2      NUMERIC(9),
F3      DATE,
PRIMARY KEY(F1)
);

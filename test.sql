begin;
create table trj_test(id serial, traj trajectory);
insert into trj_test(traj) values('
SRID:4326 NVOLUME:1024 NLOCATION:3 HASACCURACY:1 HASALTITUDE:1 HASBEARING:1 HASSPEED:1 LOCATIONS:0.1 0.1 0 2011-11-03 11:12:11 0 0 40, 0.2 0.2 0 2011-11-04 22:20:40 0 0 20, 0.3 0.3 0 2011-11-04 23:10:10 0 0 30');
--insert into trj_test(traj) values('
--SRID:4326 NLOCATION:2 HASACCURACY:-1 HASALTITUDE:-1 HASBEARING:-1 HASSPEED:-1 LOCATIONS:112.1 36.1 0 123456.1 0 0 0');
select trj_get_srid( traj ) from trj_test;
select trj_get_hasaltitude(traj) from trj_test;
select trj_get_location_n(traj) from trj_test;
select trj_get_starttime(traj) from trj_test;
select trj_get_endtime(traj) from trj_test;
select trj_get_extent(traj) from trj_test;


end;

insert into trj_test(traj) values(trj_trajectory_makeEmpty(1, 100, 0, 0, 0, 1)) ;
select * from trj_test;
drop table trj_test;


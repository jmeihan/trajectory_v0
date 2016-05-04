dropdb trajectorydb 
createdb -T template1 trajectorydb
psql -d trajectorydb -c "create extension postgis"
cp -rf ../trajectory ~/software/postgresql-9.3.2/contrib/
make -C ~/software/postgresql-9.3.2/contrib/trajectory
sudo make -C ~/software/postgresql-9.3.2/contrib/trajectory/ install

psql trajectorydb -f /usr/share/postgresql/contrib/trajectory.sql

ALTER USER 'root'@'%' IDENTIFIED BY 'MYSQL_ROOT_PASSWORD';
create user if not exists 'MYSQL_U'@'%' identified by 'MYSQL_P';
create database if not exists app;
grant all privileges on *.* to 'MYSQL_U'@'%';
flush privileges;

all:
	sed -i '1i create database if not exists app;' 02-mariadb-dump.sql
	sed -i '2i use app;' 02-mariadb-dump.sql
	sed -i 's/AUTO_INCREMENT=.* /AUTO_INCREMENT=1 /g' 02-mariadb-dump.sql

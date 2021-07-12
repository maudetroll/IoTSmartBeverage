#!/bin/sh
while [ 1 ]; do
    if [ ! -f already-executed ]; then
        echo MYSQL_SA
        echo MYSQL_SP
        echo MYSQL_ROOT_PASSWORD
        echo MYSQL_U
        echo MYSQL_P
        echo starting to fill the database
        mysql -h mysql -u root < /usr/share/work/01-user-and-permissions.sql
        mysql -h mysql -u root -pMYSQL_ROOT_PASSWORD < /usr/share/work/01-user-and-permissions.sql
        mysql -h mysql -u MYSQL_U -pMYSQL_P < /usr/share/work/02-mariadb-dump.sql  &&
        mysql -h mysql -u MYSQL_U -pMYSQL_P < /usr/share/work/03-data.sql && touch already-executed
        sleep 5
    fi
# This can be seen as a daemon that tries to fill our database
sleep 60
done

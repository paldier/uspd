#!/bin/bash

echo "preparation script"

pwd

mkdir -p /etc/bbfdm/
cp -r ./test/files/etc/* /etc/
cp -r ./schemas/ubus/* /usr/share/rpcd/schemas
cp ./gitlab-ci/iopsys-supervisord.conf /etc/supervisor/conf.d/

ls /etc/config/
ls /usr/share/rpcd/schemas/
ls /etc/supervisor/conf.d/

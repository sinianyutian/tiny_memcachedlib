#! /bin/bash
# create memcached service
memcached -m 16 -p 11211 -u memcache -d
memcached -m 16 -p 11212 -u memcache -d
memcached -m 16 -p 11213 -u memcache -d
memcached -m 16 -p 11214 -u memcache -d
memcached -m 16 -p 11215 -u memcache -d

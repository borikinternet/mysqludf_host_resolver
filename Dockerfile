FROM mariadb:latest

ADD cmake-build-release/lib_mysqludf_host_resolver.so /usr/lib/mysql/plugin/
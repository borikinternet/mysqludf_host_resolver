USE mysql;

DROP FUNCTION IF EXISTS lib_mysqludf_host_resolver;
CREATE FUNCTION lib_mysqludf_host_resolver RETURNS STRING SONAME 'lib_mysqludf_host_resolver.so';

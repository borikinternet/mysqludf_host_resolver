USE mysql;

CREATE OR REPLACE FUNCTION host_resolver RETURNS STRING SONAME 'lib_mysqludf_host_resolver.so';

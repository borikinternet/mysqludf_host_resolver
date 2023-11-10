/*
 * Skeleton libary for MySQL.
 * A set of MySQL user defined functions (UDF) to [DESCRIPTION]
 *
 * Copyright (C) [YYYY YOUR NAME <YOU@EXAMPLE.COM>]
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/param.h>
#include "lib_msqludf_host_resolver.h"

/* For Windows, define PACKAGE_STRING in the VS project */
#ifndef __WIN__

#include "config.h"

#endif

/* These must be right or mysqld will not find the symbol! */
#ifdef  __cplusplus
extern "C" {
#endif
DLLEXP my_bool host_resolver_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

/*DLLEXP void host_resolver_deinit(UDF_INIT *initid);*/
/* For functions that return STRING or DECIMAL */
DLLEXP char *
host_resolver(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null,
              char *error);

/* For functions that return REAL */
/* DLLEXP double lib_mysqludf_skeleton_info(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error); */
/* For functions that return INTEGER */
/* DLLEXP longlong lib_mysqludf_skeleton_info(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error); */

/* If you are doing an Aggregate function you'll need these too */
/* DLLEXP void lib_mysqludf_skeleton_info_clear( UDF_INIT* initid, char* is_null, char* is_error ); */
/* DLLEXP void lib_mysqludf_skeleton_info_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* is_error ); */

#ifdef  __cplusplus
}
#endif

#define MAX_RESOLVER_RESULT_LEN 512
#define MAX_FQDN_LEN            255

/*
 * Output the library version.
 * lib_mysqludf_skeleton_info()
 */

my_bool host_resolver_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  if (!args->arg_count) {
    strcpy(message, "Wrong arguments to host_resolver: function accept at least one string");
    return 1;
  }
  int i;
  for (i = 0; i < args->arg_count; ++i)
    if (args->arg_type[i] != STRING_RESULT) RETURN_ERR(
      "Wrong arguments to host_resolver: function accept only strings");
  initid->maybe_null = true;
  initid->max_length = MAX_RESOLVER_RESULT_LEN;
  initid->const_item = false;
  initid->ptr = NULL;
  return 0;
}

/*
void host_resolver_deinit(UDF_INIT *initid) {
  free(initid->ptr);
}
*/

/* For functions that return REAL */
/* double lib_mysqludf_skeleton_info(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) */
/* For functions that return INTEGER */
/* longlong lib_mysqludf_skeleton_info(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) */

/* For functions that return STRING or DECIMAL */
char *host_resolver(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null,
                    char *error) {
  struct addrinfo hints, *addr_info_res = NULL;
  char _result[MAX_RESOLVER_RESULT_LEN];

  bzero(&hints, sizeof hints);
  bzero(_result, sizeof _result);

  *length = 0;
  *is_null = true;
  *error = 0;

  if (!args || args->arg_count == 0) {
    *error = 1;
    return NULL;
  }

  int i;
  for (i = 0; i < args->arg_count; ++i) {

    if (args->lengths[i] > MAX_FQDN_LEN)
      continue;

    char cur_host_name[MAX_FQDN_LEN + 1];
    bzero(cur_host_name, sizeof cur_host_name);
    memcpy(cur_host_name, args->args[i], args->lengths[i]);

    int z;
    if ((z = getaddrinfo(cur_host_name, NULL, &hints, &addr_info_res))) {
      // some error occurs
      // printf("lib_mysql_host_resolver error: '%s' while resolving name '%s'\n", gai_strerror(z), cur_host_name);
      freeaddrinfo(addr_info_res);
      addr_info_res = NULL;
      continue;
    }

    struct addrinfo *cur_addr;
    for (cur_addr = addr_info_res; cur_addr; cur_addr = cur_addr->ai_next) {

      char addr_str[INET6_ADDRSTRLEN];
      inet_ntop(cur_addr->ai_family,
                cur_addr->ai_family == AF_INET ? (void *) &((struct sockaddr_in *) cur_addr->ai_addr)->sin_addr
                                               : (void *) &((struct sockaddr_in6 *) cur_addr->ai_addr)->sin6_addr,
                addr_str, sizeof addr_str);

      if (strstr(_result, addr_str))
        continue;

      if (*length + strnlen(addr_str, sizeof addr_str) > initid->max_length)
        break;

      if (*length) {
        *length += snprintf(_result + *length, MIN(sizeof addr_str + 1, sizeof _result - *length), " %s", addr_str);
      } else {
        *length = strnlen(addr_str, sizeof addr_str);
        strncpy(_result, addr_str, sizeof addr_str);
      }
      *is_null = false;
    }
    freeaddrinfo(addr_info_res);
    addr_info_res = NULL;
  }

  *length = snprintf(result, initid->max_length, "%s", _result);
  return result;
}

#ifdef TESTING

#include <check.h>

START_TEST(test_1)
  {
    UDF_INIT initid;
    UDF_ARGS args;
    char message[256];
    bzero(message, 256);
    bzero(&initid, sizeof initid);
    bzero(&args, sizeof args);
    args.arg_count = 1;
    args.lengths = malloc(sizeof *args.lengths);
    args.lengths[0] = sizeof "www.ya.ru";
    args.args = malloc(sizeof(char *));
    args.args[0] = "www.ya.ru";
    args.arg_type = malloc(sizeof *args.arg_type);
    args.arg_type[0] = STRING_RESULT;
    args.maybe_null = malloc(sizeof *args.maybe_null);
    args.maybe_null[0] = 0;
    my_bool res1 = host_resolver_init(&initid, &args, message);
    ck_assert(res1 == 0);

    char *result = malloc(initid.max_length);
    unsigned long length = 0;
    char is_null = 0, error = 0;

    char *res2 = host_resolver(&initid, &args, result, &length, &is_null, &error);
    ck_assert(res2 == result);
    ck_assert(is_null == false);
    ck_assert(error == 0);

    free(args.lengths);
    free(args.arg_type);
    free(args.maybe_null);
    free(args.args);
    free(result);
  }
END_TEST

Suite *udf_suite() {
  Suite *s;
  TCase *tc_core;

  s = suite_create("udf");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_1);
  suite_add_tcase(s, tc_core);

  return s;
}

int main() {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = udf_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
/*
  {
    UDF_INIT initid;
    UDF_ARGS args;
    char message[256];
    bzero(message, 256);
    bzero(&initid, sizeof initid);
    bzero(&args, sizeof args);
    args.arg_count = 1;
    args.lengths = malloc(sizeof *args.lengths);
    args.lengths[0] = sizeof "www.ya.ru";
    args.args = malloc(sizeof(char *));
    args.args[0] = "www.ya.ru";
    args.arg_type = malloc(sizeof *args.arg_type);
    args.arg_type[0] = STRING_RESULT;
    args.maybe_null = malloc(sizeof *args.maybe_null);
    args.maybe_null[0] = 0;
    my_bool res1 = host_resolver_init(&initid, &args, message);
    ck_assert(res1 == 0);

    char *result = malloc(initid.max_length);
    unsigned long length = 0;
    char is_null = 0, error = 0;

    char *res2 = host_resolver(&initid, &args, result, &length, &is_null, &error);
    ck_assert(res2 == result);
    ck_assert(is_null == false);
    ck_assert(error == 0);

    free(args.lengths);
    free(args.arg_type);
    free(args.maybe_null);
    free(result);
  }
*/
}

#endif // TESTING

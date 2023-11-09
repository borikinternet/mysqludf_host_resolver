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
#include "mysqludf.h"

/* For Windows, define PACKAGE_STRING in the VS project */
#ifndef __WIN__

#include "config.h"

#endif

/* These must be right or mysqld will not find the symbol! */
#ifdef  __cplusplus
extern "C" {
#endif
DLLEXP my_bool host_resolver_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

DLLEXP void host_resolver_deinit(UDF_INIT *initid);
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

#define MAX_RESOLVER_RESULT_LEN 4096
#define MAX_FQDN_LEN            255

/*
 * Output the library version.
 * lib_mysqludf_skeleton_info()
 */

my_bool host_resolver_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  if (!args->arg_count || args->arg_type[0] != STRING_RESULT) {
    strcpy(message, "Wrong arguments to host_resolver: function accept at least one string");
    return 1;
  }
  initid->maybe_null = true;
  initid->max_length = MAX_RESOLVER_RESULT_LEN;
  initid->const_item = false;
  return 0;
}

void host_resolver_deinit(UDF_INIT *initid) {
}

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
    goto error;
  }

  int i;
  for (i = 0; i < args->arg_count; ++i) {

    unsigned int len = args->lengths[i];
    if (len > MAX_FQDN_LEN || args->arg_type[i] != STRING_RESULT)
      continue;

    char cur_host_name[MAX_FQDN_LEN + 1];
    bzero(cur_host_name, sizeof cur_host_name);
    memcpy(cur_host_name, args->args[i], len);

    int z;
    if ((z = getaddrinfo(cur_host_name, NULL, &hints, &addr_info_res))) {
      // some error occurs
      printf("lib_mysql_host_resolver error: '%s' while resolving name '%s'\n", gai_strerror(z), cur_host_name);
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

      if (*length + strnlen(addr_str, sizeof addr_str) > MAX_RESOLVER_RESULT_LEN)
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
  goto end;

  error:
  *is_null = true;

  end:
  *length = snprintf(result, MAX_RESOLVER_RESULT_LEN, "%s", _result);
  return result;
}

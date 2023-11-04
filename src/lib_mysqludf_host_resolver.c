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
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "mysqludf.h"

/* For Windows, define PACKAGE_STRING in the VS project */
#ifndef __WIN__

#include "config.h"

#endif

/* These must be right or mysqld will not find the symbol! */
#ifdef  __cplusplus
extern "C" {
#endif
DLLEXP int lib_mysqludf_host_resolver_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

DLLEXP void lib_mysqludf_host_resolver_deinit(UDF_INIT *initid);
/* For functions that return STRING or DECIMAL */
DLLEXP char *
lib_mysqludf_host_resolver(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null,
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

#define MAX_RESOLVER_RESULT_LEN 65535

/*
 * Output the library version.
 * lib_mysqludf_skeleton_info()
 */

int lib_mysqludf_host_resolver_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  initid->maybe_null = true;
  initid->max_length = MAX_RESOLVER_RESULT_LEN;
  return 0;
}

void lib_mysqludf_host_resolver_deinit(UDF_INIT *initid) {
}

/* For functions that return REAL */
/* double lib_mysqludf_skeleton_info(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) */
/* For functions that return INTEGER */
/* longlong lib_mysqludf_skeleton_info(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) */

/* For functions that return STRING or DECIMAL */
char *lib_mysqludf_host_resolver(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null,
                                 char *error) {
  struct addrinfo hints, *addr_info_res = NULL;
  int i = 0;
  hints.ai_family = AF_INET;

  result = NULL;
  *length = 0;

  if (args->arg_count == 0)
    goto end;

  for (i = 0; i < args->arg_count; ++i) {
    if (getaddrinfo(args->args[i], NULL, &hints, &addr_info_res))
      // some error occurs
      continue;
    struct addrinfo *cur_addr = addr_info_res;
    while (cur_addr) {
      char addr_str[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, cur_addr->ai_addr, addr_str, sizeof addr_str);
      unsigned long old_len = *length;
      if (old_len) {
        *length += strnlen(addr_str, sizeof addr_str);
        result = realloc(result, *length + 1); // for space between old and additional new values
        *length = snprintf(result + old_len, *length - old_len, " %s", addr_str);
      } else {
        *length += strnlen(addr_str, sizeof addr_str);
        result = malloc(*length);
        strncpy(result, addr_str, *length);
      }
      cur_addr = cur_addr->ai_next;
    }
    freeaddrinfo(addr_info_res);
  }

  end:
  return result;
}

/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *     Copyright 2012 Couchbase, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#include "internal.h"

LIBCOUCHBASE_API
void libcouchbase_set_timeout(libcouchbase_t instance, libcouchbase_uint32_t usec)
{
    instance->timeout.usec = usec;
}

LIBCOUCHBASE_API
libcouchbase_uint32_t libcouchbase_get_timeout(libcouchbase_t instance)
{
    return instance->timeout.usec;
}

static void libcouchbase_server_timeout_handler(libcouchbase_socket_t sock,
                                         short which,
                                         void *arg)
{
    libcouchbase_server_t *server = arg;

    libcouchbase_purge_single_server(server, LIBCOUCHBASE_ETIMEDOUT);
    libcouchbase_update_server_timer(server);
    libcouchbase_maybe_breakout(server->instance);

    (void)sock;
    (void)which;
}

void libcouchbase_update_server_timer(libcouchbase_server_t *server)
{
    libcouchbase_t instance = server->instance;

    if (server->timer) {
        instance->io->delete_timer(instance->io, server->timer);
    }
    instance->io->update_timer(instance->io, server->timer,
                               instance->timeout.usec, server,
                               libcouchbase_server_timeout_handler);
}


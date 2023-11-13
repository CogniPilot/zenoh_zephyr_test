//
// Copyright (c) 2022 ZettaScale Technology
//
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
// which is available at https://www.apache.org/licenses/LICENSE-2.0.
//
// SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
//
// Contributors:
//   ZettaScale Zenoh Team, <zenoh@zettascale.tech>

#include <stdio.h>
#include <stdlib.h>
#include <zenoh-pico.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#define MODE "peer"
#define CONNECT "udp/192.0.2.1:7447@iface=eth0"

//#define MODE "client"
//#define CONNECT ""

#define KEYEXPR "demo/example/**"

LOG_MODULE_REGISTER(main, CONFIG_CEREBRI_LOG_LEVEL);

K_THREAD_STACK_DEFINE(zenoh_work_q_stack_area, 1000000);

struct k_work_q g_zenoh_work_q;

void zenoh_workqueues_entry_point(void)
{
    k_work_queue_init(&g_zenoh_work_q);
    struct k_work_queue_config zenoh_work_q_cfg = {
        .name = "zenoh_work_q",
        .no_yield = true
    };
    k_work_queue_start(
        &g_zenoh_work_q,
        zenoh_work_q_stack_area,
        K_THREAD_STACK_SIZEOF(zenoh_work_q_stack_area),
        4,
        &zenoh_work_q_cfg);
};

struct shell_work_context {
    struct k_work work;
    char connect[64];
    char mode[10];
} g_ctx;

static void data_handler(const z_sample_t *sample, void *arg) {
    z_owned_str_t keystr = z_keyexpr_to_string(sample->keyexpr);
    LOG_WRN(" >> [Subscriber handler] Received ('%s': '%.*s')\n", z_loan(keystr), (int)sample->payload.len,
           sample->payload.start);
    z_drop(z_move(keystr));
}

void zenoh_work_handler(struct k_work* item) {
    struct shell_work_context * ctx = CONTAINER_OF(
            item, struct shell_work_context, work);

    LOG_WRN("mode: %s", ctx->mode);
    LOG_WRN("connect: %s", ctx->connect);

    // Initialize Zenoh Session and other parameters
    z_owned_config_t config = z_config_default();
    zp_config_insert(z_loan(config), Z_CONFIG_MODE_KEY, z_string_make(ctx->mode));
    if (strcmp(ctx->connect, "") != 0) {
        zp_config_insert(z_loan(config), Z_CONFIG_CONNECT_KEY, z_string_make(CONNECT));
    }

    // Open Zenoh session
    LOG_WRN("Opening Zenoh Session...");
    z_owned_session_t s = z_open(z_move(config));
    if (!z_check(s)) {
        LOG_WRN("Unable to open session!\n");
	return;
    }
    LOG_WRN("OK\n");

    // Start the receive and the session lease loop for zenoh-pico
    zp_start_read_task(z_loan(s), NULL);
    zp_start_lease_task(z_loan(s), NULL);

    LOG_WRN("Declaring Subscriber on '%s'...", KEYEXPR);
    z_owned_closure_sample_t callback = z_closure(data_handler);
    z_owned_subscriber_t sub = z_declare_subscriber(z_loan(s), z_keyexpr(KEYEXPR), z_move(callback), NULL);
    if (!z_check(sub)) {
        LOG_WRN("Unable to declare subscriber.\n");
	return;
    }
    LOG_WRN("OK!\n");

    while (1) {
        k_msleep(5000);
    }

    LOG_WRN("Closing Zenoh Session...");
    z_undeclare_subscriber(z_move(sub));

    // Stop the receive and the session lease loop for zenoh-pico
    zp_stop_read_task(z_loan(s));
    zp_stop_lease_task(z_loan(s));

    z_close(z_move(s));
    LOG_WRN("OK!\n");
    return;
}

int zenoh_shell_handler(const struct shell * sh, size_t argc, char ** argv)
{
    k_work_init(&g_ctx.work, zenoh_work_handler);
    strcpy(g_ctx.mode, argv[1]);
    strcpy(g_ctx.connect, argv[2]);
    k_work_submit_to_queue(&g_zenoh_work_q, &g_ctx.work);
    return 0;
}

K_THREAD_DEFINE(zenoh_workqueue, 1024,
    zenoh_workqueues_entry_point, NULL, NULL, NULL,
    4, 0, 0);

SHELL_CMD_ARG_REGISTER(zenoh, NULL, "connect zenoh", zenoh_shell_handler, 3, 0);

// vi: ts=4 sw=4 et

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <uv.h>
#include <sol-mainloop.h>

#define DBG_UV_LOOP_HIJACK(s)

enum MainloopState {
	MAINLOOP_HIJACKING_STARTED,
	MAINLOOP_HIJACKED,
	MAINLOOP_RELEASED
};

static enum MainloopState mainloopState = MAINLOOP_RELEASED;
static void (*old_sigusr2)(int) = NULL;
static uv_timer_t uv_timer;
static struct sol_mainloop_source *uv_loop_source = NULL;
static struct sol_fd *uv_loop_fd = NULL;

static void uv_timer_callback() {}

static void undo_signal_setup() {
	DBG_UV_LOOP_HIJACK(printf("undo_signal_setup: Entering\n"));
	signal(SIGUSR2, old_sigusr2);
	uv_timer_stop(&uv_timer);
}

static void sigusr2_callback(int the_signal) {
	DBG_UV_LOOP_HIJACK(printf("sigusr2_callback: Removing signal setup and starting the loop\n"));
	undo_signal_setup();
	mainloopState = MAINLOOP_HIJACKED;
	sol_run();
	DBG_UV_LOOP_HIJACK(printf("sigusr2_callback: main loop has quit, returning\n"));
}

static bool uv_loop_source_check (void *data) {
	uv_loop_t *loop = (uv_loop_t *)data;
    uv_update_time (loop);

	DBG_UV_LOOP_HIJACK(printf("uv_loop_source_check: Returning %s\n", uv_loop_alive (loop) ? "true" : "false"));
    return uv_loop_alive (loop);
}

static bool uv_loop_source_get_next_timeout(void *data, struct timespec *timeout) {
    /* Otherwise, check the timeout. If the timeout is 0, that means we're
     * ready to go. Otherwise, keep sleeping until the timeout happens again. */
    int t = uv_backend_timeout ((uv_loop_t *)data);

	DBG_UV_LOOP_HIJACK(printf("uv_loop_source_get_next_timeout: t = %d\n", t));

	timeout->tv_sec = (int)(t / 1000);
	timeout->tv_nsec = (t % 1000) * 1000000;

	return ( t >= 0 );
}

static void uv_loop_source_dispatch (void *data) {
	DBG_UV_LOOP_HIJACK(printf("uv_loop_source_dispatch: Running one uv loop iteration\n"));
    uv_run((uv_loop_t *)data, UV_RUN_NOWAIT);
}

static const struct sol_mainloop_source_type uv_loop_source_funcs = {
	SOL_MAINLOOP_SOURCE_TYPE_API_VERSION,
    NULL,
	uv_loop_source_get_next_timeout,
	uv_loop_source_check,
    uv_loop_source_dispatch,
	NULL,
};

static bool uv_loop_fd_changed(void *data, int fd, uint32_t active_flags) {
	DBG_UV_LOOP_HIJACK(printf("uv_loop_fd_changed: Running one uv loop iteration\n"));
	uv_run((uv_loop_t *)data, UV_RUN_NOWAIT);
	return true;
}

static void hijack_main_loop() {
	DBG_UV_LOOP_HIJACK(printf("hijack_main_loop: Entering\n"));
	if (mainloopState != MAINLOOP_RELEASED) {
		return;
	}

	// The actual hijacking starts here, inspired by node-gtk. The plan:
	// 1. Attach a source to the glib main loop
	// 2. Attach a repeating timeout to the uv main loop to keep it from exiting until the signal arrives
	// 3. Send ourselves a SIGUSR2 which we handle outside of node, and wherein we start the main loop

	mainloopState = MAINLOOP_HIJACKING_STARTED;

	// We allocate a main loop and a source only once. After that, we simple start/stop the loop.
	if (!uv_loop_source) {
		DBG_UV_LOOP_HIJACK(printf("hijack_main_loop: Allocating loop-related variables\n"));
		uv_loop_t *uv_loop = uv_default_loop();
		uv_loop_source = sol_mainloop_source_new(&uv_loop_source_funcs, (const void *)uv_loop);
		uv_loop_fd = sol_fd_add(uv_backend_fd(uv_loop),
			SOL_FD_FLAGS_IN | SOL_FD_FLAGS_OUT | SOL_FD_FLAGS_ERR,
			uv_loop_fd_changed, (const void *)uv_loop);
		uv_timer_init(uv_loop, &uv_timer);
	}

	DBG_UV_LOOP_HIJACK(printf("hijack_main_loop: Preparing for SIGUSR2\n"));
	uv_timer_start(&uv_timer, uv_timer_callback, 10000, 10000);
	old_sigusr2 = signal(SIGUSR2, sigusr2_callback);
	pid_t the_pid = getpid();

	if (!fork()) {
		sleep(1);
		DBG_UV_LOOP_HIJACK(printf("hijack_main_loop: child process triggering SIGUSR2\n"));
		kill(the_pid, SIGUSR2);
		_exit(1);
	}
}

static void release_main_loop() {
	DBG_UV_LOOP_HIJACK(printf("release_main_loop: Entering\n"));
	if (mainloopState == MAINLOOP_RELEASED) {
		return;
	}

	// hijack_main_loop() was called, but the SIGUSR2 has not yet arrived
	if (mainloopState == MAINLOOP_HIJACKING_STARTED) {
		DBG_UV_LOOP_HIJACK(printf("release_main_loop: SIGUSR2 has not yet arrived, so undoing setup\n"));
		undo_signal_setup();
	}

	DBG_UV_LOOP_HIJACK(printf("release_main_loop: quitting main loop\n"));
	sol_quit();

	mainloopState = MAINLOOP_RELEASED;
}

static int hijack_refcount = 0;

void hijack_ref() {
	DBG_UV_LOOP_HIJACK(printf("hijack_ref: Entering\n"));
	if (hijack_refcount == 0) {
		hijack_refcount++;
		DBG_UV_LOOP_HIJACK(printf("hijack_ref: hijacking main loop\n"));
		hijack_main_loop();
	}
}

void hijack_unref() {
	DBG_UV_LOOP_HIJACK(printf("hijack_unref: Entering\n"));
	hijack_refcount--;
	if (hijack_refcount == 0) {
		DBG_UV_LOOP_HIJACK(printf("hijack_unref: releasing main loop\n"));
		release_main_loop();
	}
}

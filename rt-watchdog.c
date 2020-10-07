/* rt-watchdog.c
 *
 * general userspace watchdog program.  Launches two threads: the first
 * a RT99 thread that runs periodically;  the second, a SCHED_OTHER thread
 * that is the canary.  When the canary stops singing, the watchdog takes
 * a pre-defined action based on command-line arguments -- it either kills
 * all the realtime processes with a specified signal, or executes a
 * specified program to remedy the situation.  In addition to taking some
 * action, it also dumps messages to syslog.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright 2006 by IBM, All Rights Reserved
 */

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/resource.h>

extern int optind;
extern char *optarg;

int fake = 0;

/* here is the song the canary sings */
volatile int note;

/* program status */
volatile int done;

/* default watchdog interval in seconds */
#define DEFAULT_WD_INTERVAL 3

int verbose = 0;

struct pid_list {
	int pid;
	struct pid_list *next;
};

struct killing_spree_args {
	int signal;
	int interval;
	int nokill_size;
	struct pid_list *pid_list;
};

struct procinfo {
	int pid;
	int tid;
	int killed;
	unsigned long cputime;
	char cmdline[512];
	struct procinfo *next;
};

void wdlog(int level, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	if (verbose) {
		
		vprintf(fmt, ap);
		fflush(stdout);
	}
	if (level != LOG_DEBUG)
		vsyslog(level, fmt, ap);
	va_end(ap);
}

int write_to_file(char *filename, char *format, ...) {
	va_list ap;
	int ret;
	FILE * f = fopen(filename, "w");
	if (!f) {
		perror("fopen");
		return -1;
	}
	va_start(ap, format);
	ret = vfprintf(f, format, ap);
	va_end(ap);
	fclose(f);
	if (ret < 0) {
		perror("vfprintf");
	}
	return ret;
}

/* helper functions for dealing with /proc */
int progexe(int pid, char *name, int size) {
	char path[32];
	sprintf(path, "/proc/%d/exe", pid);
	return readlink(path, name, size);
}

void ulongsleep(unsigned long t) {
	if (t >= 1000000) {
		sleep(t / 1000000);
		usleep(t % 1000000);
	} else {
		usleep(t);
	}
}

/* here is the canary */
void *canary(void* arg)
{
	int interval = (int)arg*100000;
	wdlog(LOG_DEBUG, "Canary is starting...\n");
	if (setpriority(PRIO_PROCESS, getpid(), -20)) {
		perror("nice: canary may not function as designed");
	}

	while (!done){
		wdlog(LOG_DEBUG, ".");
		note = 0xa;
		ulongsleep(interval);
	}
	wdlog(LOG_DEBUG, "Canary is done\n");
	return 0;
}

void fill_procinfo(struct procinfo *p) {
	unsigned long utime, stime;
	char fname[128];
	sprintf(fname, "/proc/%d/task/%d/stat", p->pid, p->tid);
	p->killed = 0;
	FILE *f = fopen(fname, "r");
	if (!f) {
		perror("fopen");
		return;
	}
	fscanf(f, "%*d %*s %*c %*d %*d %*d %*d %*d "
			"%*u %*u %*u %*u %*u %lu %lu", &utime, &stime);
	fclose(f);
	if (p->cputime) {
		p->cputime = utime + stime - p->cputime;
	} else {
		p->cputime = utime + stime - p->cputime;
		sprintf(fname, "/proc/%d/task/%d/cmdline", p->pid, p->tid);
		f = fopen(fname, "r");
		if (!f) {
			perror("fopen");
			return;
		}
		int len = fread(p->cmdline, 1, sizeof(p->cmdline), f);
		fclose(f);
		int i;
		for (i=0; i<len-1; i++) {
			if (p->cmdline[i] == 0)
				p->cmdline[i] = ' ';
		}
	}
}

struct procinfo * get_realtime_info(struct procinfo *prev) {
	struct procinfo *info = NULL, *pi = NULL;
	DIR *dir = opendir("/proc");
	if (!dir) {
		perror("opendir");
		wdlog(LOG_EMERG, "try alt-SysRq-e or alt-SysRq-i to restore system control\n");
		return NULL;
	}
	if (!prev) {
		struct dirent *d;
		while ((d = readdir(dir)) != NULL) {
			int pid;
			if ((pid = strtol(d->d_name, NULL, 10)) > 1) {
				char tasksdir[128];
				sprintf(tasksdir, "/proc/%d/task", pid);
				DIR *tasks = opendir(tasksdir);
				struct dirent *dd;
				if (!tasks) {
					wdlog(LOG_DEBUG, "failed to open dir %s\n", tasksdir);
					continue;
				}
				while ((dd = readdir(tasks)) != NULL) {
					int tid;
					if ((tid = strtol(dd->d_name, NULL, 10)) > 1) {
						char exe[64];
						if (progexe(pid, exe, sizeof(exe)) > 0 &&
								sched_getscheduler(pid) != SCHED_OTHER) {
							if (!info) {
								info = pi = malloc(sizeof(struct procinfo));
							} else {
								pi->next = malloc(sizeof(struct procinfo));
								pi = pi->next;
							}
							pi->next = NULL;
							pi->pid = pid;
							pi->tid = tid;
							fill_procinfo(pi);
						}
					}
				}
				closedir(tasks);
			}
		}
		closedir(dir);
	} else {
		info = pi = prev;
		while (pi) {
			fill_procinfo(pi);
			/* while we are looping catch the max time */
			if (pi->cputime > info->cputime) {
				info = pi;
			}
			pi = pi->next;
		}
	}
	return info;
}

struct procinfo *sort_procinfo(struct procinfo *head, int count) {
	struct procinfo *c, *b = NULL, *mp, *m, *t = NULL, *ret = NULL;
	int sorted = 0;

	if (!count || !head)
		return NULL;
	while (sorted < count && head) {
		/* find the highest in p */
		m = head; mp = NULL;
		c = head;
		while (c) {
			if (c->cputime > m->cputime) {
				m = c;
				mp = b;
			}
			b = c;
			c = c->next;
		}
		/* delete the highest from the head list */
		if (m == head) {
			head = head->next;
		} else {
			mp->next = m->next;
		}
		/* append the highest to the tail */
		if (t) {
			t->next = m;
			t = m;
		} else {
			ret = t = m;
		}
		t->next = NULL;
		sorted++;
	}
	/* append the rest of head to the tail */
	t->next = head;
	
	return ret;
}

void log_and_kill(struct procinfo *p, int sig) {
	wdlog(LOG_EMERG, "killing % 6d %s\n", p->pid, p->cmdline);
	p->killed = !(kill(p->pid, sig));
}

void free_procinfo_list(struct procinfo *p) {
	struct procinfo *t;
	while (p) {
		t = p->next;
		free(p);
		p = t;
	}
}

/* kill all those naughty RT procs */
void killing_spree(void *arg) {
	struct killing_spree_args *args = (struct killing_spree_args *)arg;
	int self = getpid();
	struct pid_list *pl, *nokill = args->pid_list;
	int interval = args->interval * 1000000;

	if (done)
		return;
		/* get list of real time processes with cpu usage */
	struct procinfo *rt_tasks = get_realtime_info(NULL);
	/* wait a while */
	ulongsleep(interval);
	if (done) {
		wdlog(LOG_NOTICE, "killing spree interrupted (rt-watchdog quitting)\n");
		return;
	}
	if (note) {
		wdlog(LOG_EMERG, "killing spree interrupted (canary awoke after %d seconds)\n",
				args->interval);
		return;
	}
	/* get the new list of real time processes with cpu usage */
	struct procinfo *hog = get_realtime_info(rt_tasks);
	if (done) {
		wdlog(LOG_NOTICE, "killing spree interrupted (rt-watchdog quitting)\n");
		return;
	}
	if (note) {
		wdlog(LOG_EMERG, "killing spree interrupted (canary awoke after %d seconds)\n",
				2 * args->interval);
		return;
	}
	/* kill something */
	if (!hog) {
		wdlog(LOG_DEBUG, "no real-time tasks found... what happened here?\n");
		return;
	}
	if (hog->pid != self && hog->pid != 1) {
		pl = nokill;
		while (pl) {
			if (pl->pid == hog->pid) {
				hog = NULL;
				break;
			}
			pl = pl->next;
		}
		if (hog) {
			log_and_kill(hog, args->signal);
			return;
		}
	}
	rt_tasks = sort_procinfo(rt_tasks, args->nokill_size + 2);
	hog = rt_tasks;
	while (hog) {
		if (hog->killed || hog->pid == self || hog->pid == 1) {
			hog = hog->next;
			continue;
		}
		pl = nokill;
		while (pl) {
			if (pl->pid == rt_tasks->pid)
				break;
			pl = pl->next;
		}
		if (pl) {
			hog = hog->next;
			continue;
		}
		log_and_kill(hog, args->signal);
		break;
	}
	free_procinfo_list(rt_tasks);
}

void spawn_process(void *arg) {
	char **argv = (char **)arg;
	int pid = fork();
	if (pid < 0) {
		perror("spawn_process: fork");
		wdlog(LOG_EMERG, "rt-watchdog failed to fork, sacrificing current process to exec\n");
	}
	if (pid <= 0) {
		if (execvp(argv[0], argv) < 0) {
			perror("spawn_process: execvp");
			wdlog(LOG_EMERG, "rt-watchdog failed to exec process -- system is probably unusable now\n");
		}
	} else {
		waitpid(pid, NULL, 0);
	}
}

void reboot_now(void *arg) {
	arg = NULL;
	wdlog(LOG_EMERG, "rt-watchdog rebooting system!");
	sync();
	reboot(LINUX_REBOOT_CMD_RESTART);
	wdlog(LOG_EMERG, "rt-watchdog failed to reboot machine -- system is probably unusable now\n");
}

void watchdog(int work_time, int interval, void (*rampage)(void *), void *arg)
{
	struct timeval start, now;
	struct sched_param param;
	
	interval *= 1000000;

	if (!fake) {
	/* We're the rt-watchdog, so set our priority right */
	param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	if (sched_setscheduler(0, SCHED_FIFO, &param) < 0) {
		perror("sched_setscheduler");
	}
	} // fake

	wdlog(LOG_DEBUG, "Watchdog starting...\n");

	gettimeofday(&start, 0);
	now = start;
	note = 0xa;

	/* Watch the system */
	while (!done && (work_time < 0 || (now.tv_sec - start.tv_sec) < work_time)) {
		ulongsleep(interval);
		if (note == 0) {
			wdlog(LOG_DEBUG, "The canary is starving! taking action...\n");
			rampage(arg);
		} else {
			wdlog(LOG_DEBUG, "canary is still alive\n");
		}
		note = 0;
		gettimeofday(&now, 0);
	}

	wdlog(LOG_DEBUG, "Watchdog finished -- work's done!\n");
	done = 1;
}


void quit(int signal) {
	wdlog(LOG_EMERG, "caught signal %d -- quitting\n", signal);
	done = 1;
}

char _pid_file[] = "/var/run/rt-watchdog.pid";
int read_pid_file(int expected) {
	struct stat sb;
	if (stat(_pid_file, &sb) < 0) {
		if (expected)
			printf("no pid file found\n");
		return -1;
	}
	FILE *pf = fopen(_pid_file, "r");
	int daemon;
	if (!pf) {
		perror("fopen");
		return -1;
	}
	if (fscanf(pf, "%d\n", &daemon) < 1) {
		if (expected)
			printf("pid file is corrupted\n");
		fclose(pf);
		return -1;
	}
	fclose(pf);
	return daemon;
}
int write_pid_file(int pid) {
	return write_to_file(_pid_file, "%d\n", pid);
}
void delete_pid_file(void) {
	unlink(_pid_file);
}

void daemonize() {
	if (read_pid_file(0) > 0) {
		printf("pid file found - cowardly refusing to start\n");
		exit(1);
	}
	int child = fork();
	if (child > 0) {
		signal(SIGHUP, SIG_IGN);
		if (write_pid_file(child) < 0) {
			printf("failed to create pid file\n");
			kill(child, SIGKILL);
			waitpid(child, NULL, 0);
			exit(1);
		}
		exit(0);
	} else if (child < 0) {
		perror("fork");
		return;
	}
	close(0);
	close(1);
	close(2);
	int tty = open("/dev/tty", O_RDWR);
	if (tty < 0) {
		perror("open");
		return;
	}
	if (ioctl(tty, TIOCNOTTY) < 0) {
		perror("ioctl");
		return;
	}
	close(tty);
}

int kill_daemon(void) {
	int daemon;
	
	daemon = read_pid_file(1);
	if (daemon > 0)
		kill(daemon, SIGTERM);
	delete_pid_file();
	exit(0);
}

void usage(char *prog, int ret) {
	printf("%s [-t sec] [-d] [-v] [-i msec] [-n p] [-s signal_num]\n", prog);
	printf("%s [-t sec] [-d] [-v] [-i msec] -r\n", prog);
	printf("%s [-t sec] [-d] [-v] [-i msec] -e prog args...\n", prog);
	printf("%s -k\n", prog);
	printf("%s -h\n", prog);
	printf("\t-t seconds to run (default: -1 for infinite)\n");
	printf("\t-d debug mode -- don't daemonize (default: off)\n");
	printf("\t-v verbose mode (default: off)\n");
	printf("\t-i watchdog interval (in seconds [1:1000] default: %d)\n", DEFAULT_WD_INTERVAL);
	printf("\t-n do not kill process with pid p (use cautiously)\n");
	printf("\t\t-n may be specified more than once\n");
	printf("\t-k kill running daemon\n");
	printf("\t-h display this message\n");
	printf("choose one of the following actions to take when the canary is starved\n");
	printf("\t-e execute action to take when canary is starved\n");
	printf("\t\tprog will be called with args (use -a -- if args have '-')\n");
	printf("\t-r reboot the machine when canary is starved\n");
	printf("\t-s signal number to kill processes with when canary is starved\n");
	printf("\t\t-s %d is the default action when canary is starved\n", SIGKILL);
	exit(ret);
}

int main(int argc, char* argv[])
{
	pthread_t canary_pid;
	int interval = DEFAULT_WD_INTERVAL;
	int work_time = -1, daemon = 1, c;
	struct pid_list *nokill = NULL, *pl = NULL;
	struct killing_spree_args killing_spree_args = { SIGKILL, DEFAULT_WD_INTERVAL, 0, NULL };
	void (*action)(void*) = killing_spree;
	void *action_args = &killing_spree_args;

	while ((c = getopt(argc, argv, "fers:i:kn:hdt:v")) >= 0) {
		switch (c) {
			case 'd':
				daemon = 0;
				break;
			case 'e':
				action = spawn_process;
				break;
			case 'f':
				fake = 1;
				break;
			case 'h':
				usage(argv[0], 0);
				break;
			case 'i':
				interval = strtol(optarg, NULL, 0);
				if (interval > 1000 || interval < 1) {
					printf("pick an interval in [1,1000)\n");
					exit(1);
				}
				break;
			case 'k':
				return kill_daemon();
				break;
			case 'n':
				if (!pl) {
					nokill = pl = malloc(sizeof(struct pid_list));
				} else {
					pl->next = malloc(sizeof(struct pid_list));
					pl = pl->next;
				}
				pl->pid = strtol(optarg, NULL, 0);
				pl->next = NULL;
				killing_spree_args.nokill_size++;
				break;
			case 'r':
				action = reboot_now;
				break;
			case 's':
				action = killing_spree;
				killing_spree_args.signal = strtol(optarg, NULL, 0);
				action_args = &killing_spree_args;
				break;
			case 't':
				work_time = strtol(optarg, NULL, 0);
				break;
			case 'v':
				verbose = 1;
				daemon = 0;
				break;
			default:
				usage(argv[0], 1);
				break;
		}
	}

	if (getuid() != 0) {
		printf("%s must run as root -- refusing to start\n", argv[0]);
		return 1;
	}

	if (daemon) {
		verbose = 0;
		daemonize();
	} else {
		if (read_pid_file(0) > 0) {
			printf("pid file found -- cowardly refusing to start\n");
			return 1;
		}
		write_pid_file(getpid());
	}

	if (action == spawn_process) {
		action_args = &argv[optind];
		wdlog(LOG_DEBUG, "action is: %s", argv[optind]);
		for (c=optind+1; c<argc; c++) {
			wdlog(LOG_DEBUG, " %s", argv[c]);
		}
		wdlog(LOG_DEBUG, "\n");
	} else if (action == killing_spree) {
		killing_spree_args.interval = interval;
		killing_spree_args.pid_list = nokill;
		wdlog(LOG_DEBUG, "action is kill -%d for runaway RT procs", killing_spree_args.signal);
		pl = nokill;
		if (pl) {
			wdlog(LOG_DEBUG, " except pids");
		}
		while (pl) {
			wdlog(LOG_DEBUG, " %d", pl->pid);
			pl = pl->next;
		}
		wdlog(LOG_DEBUG, "\n");
	} else if (action == reboot_now) {
		wdlog(LOG_DEBUG, "action is reboot\n");
	}

	signal(SIGTERM, quit);
	signal(SIGINT, quit);
	signal(SIGQUIT, quit);

	openlog("rt-watchdog", LOG_CONS|LOG_NDELAY, LOG_DAEMON);

	pthread_attr_t attr;
	struct sched_param param;

	param.sched_priority = sched_get_priority_min(SCHED_OTHER);

	pthread_attr_init(&attr);
	pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedparam(&attr, &param);
	pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

	/* uncover the birdcage */
	wdlog(LOG_DEBUG, "wake up, canary!\n");
	if (pthread_create(&canary_pid, &attr, canary, (void *)interval)) {
		perror("pthread_create failed");
	}
	pthread_attr_destroy(&attr);

	/* watch the bird */
	watchdog(work_time, interval, action, action_args);

	pthread_join(canary_pid, 0);

	while (nokill) {
		pl = nokill;
		nokill = nokill->next;
		free(pl);
	}

	closelog();
	delete_pid_file();

	return 0;
}

/****************************************************
  a  netlink message includes: nlmsghdr, cn_msg and process event
	PROC_CN_MCAST_LISTEN open monitor,
	PROC_CN_MCAST_IGNORE close monitor
*****************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#include <linux/cn_proc.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int g_fd = -1;

enum MY_RET_CODE 
{
	RET_SUCCESS = 0,
	RET_FAIL
};

#define deal_err(f) do \
{\
	printf("%s err: %s\n", f, strerror(errno)); \
	return RET_FAIL; \
}while(0);

int cnnt()
{
	int fd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
	if (fd < 0)
	{
		deal_err("socket");
	}

	struct sockaddr_nl sa_nl;
	sa_nl.nl_family = AF_NETLINK;
	sa_nl.nl_pad = 0;
	sa_nl.nl_groups = CN_IDX_PROC;
	sa_nl.nl_pid = 0;

	if (bind(fd, (struct sockaddr*)&sa_nl, sizeof(sa_nl)) < 0)
	{
		close(fd);
		deal_err("bind");
	}

	return fd;
}

int change_pro_mode(enum proc_cn_mcast_op mode)
{
	int fd = g_fd;
	struct __attribute__((aligned(NLMSG_ALIGNTO)))
	{
		struct nlmsghdr nl_hdr;
		struct __attribute__((__packed__))
		{
			struct cn_msg cn;
			enum proc_cn_mcast_op mode;
		}
	}mon_msg;

	memset(&mon_msg, 0, sizeof(mon_msg));
	mon_msg.nl_hdr.nlmsg_len = sizeof(mon_msg);
	mon_msg.nl_hdr.nlmsg_pid = getpid();
	mon_msg.nl_hdr.nlmsg_type = NLMSG_DONE;
	
	mon_msg.cn.id.idx = CN_IDX_PROC;
	mon_msg.cn.id.val = CN_VAL_PROC;
	mon_msg.cn.len = sizeof(mode);
	mon_msg.mode = mode;

	if (send(fd, &mon_msg, sizeof(mon_msg), 0) < 0 )
	{
		close(fd);
		deal_err("send");
	}
	return RET_SUCCESS;
}

void do_proc_ev(const struct proc_event* p)
{
	switch (p->what)
	{
		case PROC_EVENT_EXIT:
			printf("PROC_EVENT_EXIT: tid:%d, pid:%d, exit_code:%d\n\n",
					p->event_data.exit.process_pid,
					p->event_data.exit.process_tgid,
					p->event_data.exit.exit_code);
			break;
		case PROC_EVENT_FORK:
			printf("PROC_EVENT_FORK: parent tid:%d, pid:%d, child tid:%d, pid:%d\n\n",
					p->event_data.fork.parent_tgid,
					p->event_data.fork.parent_pid,
					p->event_data.fork.child_pid,
					p->event_data.fork.child_tgid);
			break;
		case PROC_EVENT_EXEC:
			 printf("PROC_EVENT_FORK\n\n");
			break;
		default:
			printf("Unkonwn PROC_EVENT: %d\n\n", p->what);
			break;
	}
}

int do_mon(int fd)
{
	struct __attribute__((aligned(NLMSG_ALIGNTO)))
	{
		struct nlmsghdr nl_hdr;
		struct __attribute__((__packed__))
		{
			struct cn_msg cn;
			struct proc_event pro_ev;
		}
	}mon_rsp;
	
	int rc = 0;
	while(1)
	{
		rc = recv(fd, &mon_rsp, sizeof(mon_rsp), 0);
		if (rc <= 0)
		{
			printf("%s err: %s\n", "recv", strerror(errno));
			break;
		}
		do_proc_ev(&mon_rsp.pro_ev);
	}

	return RET_SUCCESS;
}

void sig_handler(int signo)
{
	change_pro_mode(PROC_CN_MCAST_IGNORE);
	printf("sig handler\n");
	close(g_fd);
	exit(0);
}

int main()
{
	struct sigaction sa; 
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_RESETHAND;
	sa.sa_handler = sig_handler;
	sigaction(SIGINT, &sa, NULL);

	g_fd = cnnt();	
	if (RET_FAIL != g_fd)
	{
		change_pro_mode( PROC_CN_MCAST_LISTEN);
		do_mon(g_fd);
		close(g_fd);
	}
	return 0;
}


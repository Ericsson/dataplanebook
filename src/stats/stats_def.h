#define EP_MAX_SESSIONS (1000)

struct ep_session_stats
{
	uint64_t bytes;
	uint64_t pkts;
};

struct ep_stats
{
	struct ep_session_stats sessions[EP_MAX_SESSIONS];
	uint64_t total_bytes;
	uint64_t total_pkts;
};

static struct ep_stats stats;

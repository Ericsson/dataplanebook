#define EP_MAX_SESSIONS (1000)

struct ep_session_stats
{
	uint64_t bytes;
	uint64_t pkts;
};

struct ep_stats
{
	rte_seqcount_t sc; /* protecting all counters */
	struct ep_session_stats sessions[EP_MAX_SESSIONS];
	uint64_t total_bytes;
	uint64_t total_pkts;
} __rte_cache_aligned;

static struct ep_stats lcore_stats[RTE_LCORE_MAX];

void
ep_stats_update(uint16_t session_id, uint64_t pkt_size)
{
	unsigned int lcore_id;
	struct ep_stats *stats;
	struct ep_session_stats *session_stats;

	lcore_id = rte_lcore_id();

	stats = &lcore_stats[lcore_id];
	session_stats = &stats->sessions[session_id];

	rte_seqcount_begin_write(&stats->sc);

	stats->total_pkts++;
	stats->total_bytes += pkt_size;

	session_stats->pkts++;
	session_stats->bytes += pkt_size;

	rte_seqcount_end_write(&stats->sc);
}

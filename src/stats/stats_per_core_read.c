#define EP_STATS_GET(field)						\
	({								\
		unsigned int lcore_id;					\
		uint64_t value = 0;					\
									\
		for (lcore_id = 0; lcore_id < RTE_LCORE_MAX; lcore_id++) { \
			struct ep_stats *stats = &lcore_stats[lcore_id]; \
									\
			value += __atomic_load_n(&(field),		\
						 __ATOMIC_RELAXED);	\
		}							\
									\
		return value;						\
        })


#define EP_SESSION_STATS_GET(session_id, field) \
	EP_STATS_GET(sessions[session_id].field)

void
ep_stats_session_get(uint32_t session_id,
		     struct ep_session_stats *session_stats)
{
	*session_stats = (struct ep_session_stats) {
		.bytes = EP_SESSION_STATS_GET(session_id, bytes),
		.pkts = EP_SESSION_STATS_GET(session_id, pkts)
	};
}

void
ep_stats_get(struct ep_stats *stats)
{
	uint32_t session_id;

	for (session_id = 0; session_id < RTE_LCORE_MAX; session_id++) {
		struct ep_session_stats *session_stats =
			&stats->session[session_id];

		ep_stats_session_get(session_id, session_stats);
	}

	stats->bytes = EP_STATS_GET(bytes);
	stats->pkts = EP_STATS_GET(pkts);
}

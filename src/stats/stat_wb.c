#define STAT_WB_MAX_SIZE (1024)
#define STAT_WB_SOFT_THRESHOLD (STAT_WB_MAX_SIZE / 2)
#define STAT_WB_MAX_HINTS (64)

struct stat_wb_add64_op
{
	uint64_t *counter;
	uint64_t operand;
};

struct stat_wb {
	uint16_t hints;
	uint16_t num_add64_ops;
	struct stat_wb_add64_op add64_ops[STAT_WB_MAX_SIZE];
} __rte_cache_aligned;

static struct stat_wb lcore_wbs[RTE_MAX_LCORE];

static rte_spinlock_t stat_lock __rte_cache_aligned;

static struct stat_wb *
lcore_wb(void)
{
	unsigned int lcore_id;
	struct stat_wb *wb;

	lcore_id = rte_lcore_id();
	wb = &lcore_wbs[lcore_id];

	return wb;
}

static void
flush(struct stat_wb *wb, bool force)
{
	uint16_t i;

	if (force || wb->num_add64_ops == STAT_WB_MAX_SIZE)
		rte_spinlock_lock(&stat_lock);
	else if (!rte_spinlock_trylock(&stat_lock))
		return;

	for (i = 0; i < wb->num_add64_ops; i++) {
		struct stat_wb_add64_op *op = &wb->add64_ops[i];

		*op->counter += op->operand;
	}

	rte_spinlock_unlock(&stat_lock);

	wb->num_add64_ops = 0;
	wb->hints = 0;
}

static void
try_flush(struct stat_wb *wb)
{
	flush(wb, false);
}

static void
force_flush(struct stat_wb *wb)
{
	flush(wb, true);
}

static void
schedule_add64(struct stat_wb *wb, uint64_t *counter,
	       uint64_t operand)
{
	wb->add64_ops[wb->num_add64_ops] =
	    (struct stat_wb_add64_op) {
		.counter = counter,
		.operand = operand
	};

	wb->num_add64_ops++;
}

void
stat_wb_schedule_add64(uint64_t *counter, uint64_t operand)
{
	struct stat_wb *wb = lcore_wb();

	if (wb->num_add64_ops == STAT_WB_MAX_SIZE)
		force_flush(wb);
	else if (wb->num_add64_ops > STAT_WB_SOFT_THRESHOLD)
		try_flush(wb);

	schedule_add64(wb, counter, operand);
}

void
stat_wb_hint_flush(void)
{
	struct stat_wb *wb = lcore_wb();

	if (wb->hints == STAT_WB_MAX_HINTS)
		try_flush(wb);
	else
		wb->hints++;
}

void
stat_wb_try_flush(void)
{
	struct stat_wb *wb = lcore_wb();

	try_flush(wb);
}

void
stat_wb_flush(void)
{
	struct stat_wb *wb = lcore_wb();

	force_flush(wb);
}

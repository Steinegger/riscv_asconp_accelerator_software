#ifndef BENCHMARK_CONFIG_H_
#define BENCHMARK_CONFIG_H_

#ifndef MULTIROUND_CONF
	#define MULTIROUND_CONF 1
#endif

#ifndef LITTLE_ENDIAN_CONF
	#define LITTLE_ENDIAN_CONF 1
#endif

#ifndef SPIKE
	#ifndef RISCY_HIGH_PERFORMANCE
		#define RISCY_HIGH_PERFORMANCE 1
	#endif
#else
	#undef RISCY_HIGH_PERFORMANCE
#endif

#ifndef PERFORMANCE_TEST
	#define PERFORMANCE_TEST 1
#endif

// Settings for performance and performance hashing evaluation
#ifndef MIN_ASSOCIATED_DATA_LENGTH
	#define MIN_ASSOCIATED_DATA_LENGTH	0
#endif
#ifndef STEP_ASSOCIATED_DATA
	#define STEP_ASSOCIATED_DATA				64
#endif
#ifndef MAX_ASSOCIATED_DATA_LENGTH
	#define MAX_ASSOCIATED_DATA_LENGTH	0
#endif

#ifndef MIN_MESSAGE_LENGTH
	#define MIN_MESSAGE_LENGTH					0
#endif
#ifndef STEP_MESSAGE
	#define STEP_MESSAGE								64
#endif
#ifndef MAX_MESSAGE_LENGTH
	#define MAX_MESSAGE_LENGTH					64
#endif

#define ALIVE_MESSAGE_INTERVAL      5000

#define SEED 213423

#endif // BENCHMARK_CONFIG_H_
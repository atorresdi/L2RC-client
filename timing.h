/* timing.h Definitions for the time module */

#if !defined(TIMING_H)

#define TIMING_H

#define CLOCK_MAX_VAL     822733124     /* this value depends on the plattform */

/* Timeouts and periods definition */

#define TIME_BASE		10000000UL 	/* 10 ms */

/* Timeouts and periods quantity */

#define NUM_PERIOD		4
#define NUM_TIMEOUT		4

/* Periods */

#define AUX_PERIOD_NUM			0
#define AUX_PERIOD_VAL			1000				/* 1 sec */

/* Timeouts */

#define AUX_TIMEOUT_NUM			0
#define AUX_TIMEOUT_VAL			500		/* 10 secs */

#define HLC_TO_NUM              1
#define HLC_TO_VAL              1000

/* SW flags */

#define TM_PERIOD_ACTIVE		0x01U
#define TM_PERIOD_END			0x02U

/* Tipos de datos */

struct Tm_Period{
	unsigned char flags;
	unsigned int counter,
                 period;
};

struct Tm_Control{
	unsigned char num_period;
	struct Tm_Period *period;
	
	unsigned char num_timeout;
	unsigned int *timeout;
};

/* Rutinas */ 

void Tm_Define(struct Tm_Control *tcp,
               unsigned char num_period,
               struct Tm_Period *pp,
               unsigned char num_timeout,
               unsigned int *tp);

void Tm_Process(struct Tm_Control *tcp);

void Tm_Start_Period(struct Tm_Control *tcp,
                     unsigned char num_period,
                     unsigned int period);

char Tm_Period_Complete(const struct Tm_Control *tcp,
                        unsigned char num_period);

void Tm_Clean_Period(struct Tm_Control *tcp,
                     unsigned char num_period);

void Tm_End_Period(struct Tm_Control *tcp,
                   unsigned char num_period);
												
void Tm_Start_Timeout(struct Tm_Control *tcp,
                      unsigned char num_timeout,
                      unsigned int timeout);
											 
char Tm_Timeout_Complete(const struct Tm_Control *tcp,
                         unsigned char num_timeout);
										 
char Handle_Timer (void);
							
#endif

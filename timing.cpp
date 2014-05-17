/* Time_Module.c Implementation of the time module */

#include "timing.h"
#include <time.h>

/* Local variables */
static struct timespec tm_prev_ts;    /* previous measurement */
static struct timespec tm_curr_ts;    /* current measurement */

void Tm_Define(struct Tm_Control *tcp,
               unsigned char num_period,
               struct Tm_Period *pp,
               unsigned char num_timeout,
               unsigned int *tp)
		{
			unsigned char n;
			
			tcp->num_period = num_period;
			tcp->period = pp;
			
			for(n = num_period; n; --n, ++pp)
				pp->flags = 0;
			
			tcp->num_timeout = num_timeout;
			tcp->timeout = tp;
			
			for(n = num_timeout; n; --n, tp++)
				*tp = 0;			
			
			/* HW setup */
            clock_gettime(CLOCK_MONOTONIC_RAW, &tm_prev_ts);
            clock_gettime(CLOCK_MONOTONIC_RAW, &tm_curr_ts);
        }
		
void Tm_Process(struct Tm_Control *tcp)
		{
			unsigned char n;
			struct Tm_Period *pp;
			unsigned int *tp;
			
			for (n = tcp->num_period, pp = tcp->period; n ; --n, ++pp)
				{
					if (((pp->flags) & TM_PERIOD_ACTIVE))
						{
						--(pp->counter);
						
						if (!(pp->counter))
							{
								pp->flags |= TM_PERIOD_END;
								pp->counter = pp->period;
							};
						};
					};
						
			for (n = tcp->num_timeout, tp = tcp->timeout; n; --n, tp++)
				{
					if(*tp)
						--(*tp);
				}		
        }
		
void Tm_Start_Period(struct Tm_Control *tcp,
                     unsigned char num_period,
                     unsigned int period)
		{
			struct Tm_Period *pp = (tcp->period) + num_period;
			
			if (num_period >= (tcp->num_period))
				return;
			
			pp->flags |= TM_PERIOD_ACTIVE;
			pp->period = pp->counter = period;
        }
		
char Tm_Period_Complete(const struct Tm_Control *tcp,
                        unsigned char num_period)
		{
			return (tcp->period[num_period].flags & TM_PERIOD_END);
        }
		
void Tm_Clean_Period(struct Tm_Control *tcp,
                     unsigned char num_period)
		{
			tcp->period[num_period].flags &= ~(TM_PERIOD_END);
        }
		
void Tm_End_Period(struct Tm_Control *tcp,
                   unsigned char num_period)
		{
			tcp->period[num_period].flags &= ~(TM_PERIOD_ACTIVE);
        }
		
void Tm_Start_Timeout(struct Tm_Control *tcp,
                      unsigned char num_timeout,
                      unsigned int timeout)
		{
			tcp->timeout[num_timeout] = timeout;
        }
		
char Tm_Timeout_Complete(const struct Tm_Control *tcp,
                         unsigned char num_timeout)
		{
			return(!(tcp->timeout[num_timeout]));
        }
		
char Handle_Timer (void)
		{
            clock_gettime(CLOCK_MONOTONIC_RAW, &tm_curr_ts);

            if (tm_curr_ts.tv_nsec < tm_prev_ts.tv_nsec)
            {
                if ( (unsigned long int)(tm_curr_ts.tv_nsec + (CLOCK_MAX_VAL - tm_prev_ts.tv_nsec)) >=  TIME_BASE)
                {
                    clock_gettime(CLOCK_MONOTONIC_RAW, &tm_prev_ts);
                    return 1;
                };
            };

            if ( (unsigned long int)(tm_curr_ts.tv_nsec - tm_prev_ts.tv_nsec) >=  TIME_BASE)
            {
                clock_gettime(CLOCK_MONOTONIC_RAW, &tm_prev_ts);
                return 1;
            };

            return 0;
        }

#include <stdio.h>
#include <math.h>
#include "lcgrand.h"  /* Header file for random-number generator. */

#define ENCODE_RATE 15800
#define STORAGE_RATE 1600
#define ENCODE_SIZE 20 /* Limit on encoder queue length. */
#define ALPHA 0.1
#define BUSY 1  /* Mnemonics for server's being busy */
#define IDLE 0  /* and idle. */
#define END 28800
#define MEAN_INTERARRIVAL 0.008
#define MEAN_SIZEOF_FRAME 200

int   next_event_type , encoder_status , numbers_in_encoder_queue , storage_status , numbers_in_storage_queue , num_events;
int frame_counter , discard_frame ,total_frame;
float sim_time, time_arrival_in_encoder[ENCODE_SIZE + 1] , time_next_event[4];//1 is arrival at encode , 2 is departure to storage , 3 is start storage
float frame_size_in_encoder , size_of_storage_frame , storage_queue_frame_size[100000000] , time_last_event , storage_busy_time;

//FILE  *infile, *outfile;

void  initialize(void);
void  timing(void);
void  arrive_at_encoder(void);
void  depart_to_storage_queue(void);
void  report(void);
void  update_time_avg_stats(void);
float expon(float mean);
void top_frame_delete(void);
void bottom_frame_delete(void);
void storage(void);


main()  /* Main function. */
{
    /* Specify the number of events for the timing function. */

    num_events = 3;

    /* Initialize the simulation. */

    initialize();

    /* Run the simulation while more delays are still needed. */

    while (sim_time < END)
    {
        /* Determine the next event. */
        timing();
        //printf("first = %f \t second = %f \t third = %f \n",time_next_event[1],time_next_event[2],time_next_event[3]);
        //printf("numbers_in_encoder_queue = %d \t",numbers_in_encoder_queue);
        //printf("%d",encoder_status);

        /* Update time-average statistical accumulators. */
        int i;
        update_time_avg_stats();
        /* Invoke the appropriate event function. */

        switch (next_event_type)
        {
            case 1:
                arrive_at_encoder();
                break;
            case 2:
                depart_to_storage_queue();
                break;
            case 3:
                storage();
                break;
        }
    }

    printf("B = %d\ntotal frame = %d \ndiscard frame = %d\nfraction of discard frame = %f\nstorage utilization = %f\n" ,ENCODE_SIZE , total_frame , discard_frame , (float)discard_frame / (float)total_frame , storage_busy_time / sim_time);


    return 0;
}


void initialize(void)  /* Initialization function. */
{
    /* Initialize the simulation clock. */

    sim_time = 0.0;

    /* Initialize the state variables. */

    encoder_status = IDLE;
    storage_status = IDLE;
    numbers_in_encoder_queue = 0;
    numbers_in_storage_queue = 0;
    time_last_event = 0.0;
    frame_counter = 0;
    discard_frame = 0;
    total_frame = 0;

    /* Initialize the statistical counters. */



    /* Initialize event list.  Since no customers are present, the departure
       (service completion) event is eliminated from consideration. */

    time_next_event[1] = 0;
    time_next_event[2] = 1.0e+30;
    time_next_event[3] = 1.0e+30;
}


void timing(void)  /* Timing function. */
{
    int   i;
    float min_time_next_event = 1.0e+29;

    next_event_type = 0;

    /* Determine the event type of the next event to occur. */

    for (i = 1; i <= num_events; i++)
    {
        if (time_next_event[i] < min_time_next_event)
        {
            min_time_next_event = time_next_event[i];
            next_event_type     = i;
        }
    }
    /* Check to see whether the event list is empty. */

    if (next_event_type == 0)
    {

        /* The event list is empty, so stop the simulation. */

        //fprintf(outfile, "\nEvent list empty at time %f", sim_time);
        exit(1);
    }

    /* The event list is not empty, so advance the simulation clock. */

    sim_time = min_time_next_event;
}


void arrive_at_encoder(void)  /* Arrival event function. */
{
    total_frame++;

    /* Schedule next arrival. */

    time_next_event[1] = sim_time + expon(MEAN_INTERARRIVAL);

    /* Check to see whether server is busy. */
    if (encoder_status == BUSY)
    {

        /* Server is busy, so increment number of customers in queue. */

        numbers_in_encoder_queue++;

        /* Check to see whether an overflow condition exists. */

        if (numbers_in_encoder_queue > ENCODE_SIZE && frame_counter == 0) // 0 is top_frame and 1 is bottom_frame
        {
            top_frame_delete();
            numbers_in_encoder_queue--;
            discard_frame = discard_frame + 2;
            frame_counter++;
            total_frame++;
        }
        else if(numbers_in_encoder_queue > ENCODE_SIZE && frame_counter == 1)
        {
            bottom_frame_delete();
            numbers_in_encoder_queue--;
            discard_frame = discard_frame + 2;
        }
        else
        {
            time_arrival_in_encoder[numbers_in_encoder_queue] = sim_time;
        }
        /* There is still room in the queue, so store the time of arrival of the
           arriving customer at the (new) end of time_arrival. */


    }

    else
    {

        /* Server is idle, so arriving customer has a delay of zero.  (The
           following two statements are for program clarity and do not affect
           the results of the simulation.) */

        /* Increment the number of customers delayed, and make server busy. */

        encoder_status = BUSY;

        /* Schedule a departure (service completion). */

        frame_size_in_encoder = expon(MEAN_SIZEOF_FRAME);
        time_next_event[2] = sim_time + frame_size_in_encoder / ENCODE_RATE;
    }
    frame_counter = (frame_counter + 1) % 2 ;
}


void depart_to_storage_queue(void)  /* Departure event function. */
{
    int   i;

    /* Check to see whether the queue is empty. */

    if (numbers_in_encoder_queue == 0)
    {

        /* The queue is empty so make the server idle and eliminate the
           departure (service completion) event from consideration. */
        numbers_in_storage_queue++;
        storage_queue_frame_size[numbers_in_storage_queue] = frame_size_in_encoder;
        encoder_status = IDLE;
        time_next_event[2] = 1.0e+30;
    }
    else
    {

        /* The queue is nonempty, so decrement the number of customers in
           queue. */

        numbers_in_encoder_queue--;

        /* Compute the delay of the customer who is beginning service and update
           the total delay accumulator. */

        /*delay            = sim_time - time_arrival[1];
        total_of_delays += delay;*/

        /* Increment the number of customers delayed, and schedule departure. */

        //++num_custs_delayed;
        frame_size_in_encoder = expon(MEAN_SIZEOF_FRAME);
        time_next_event[2] = sim_time + frame_size_in_encoder / ENCODE_RATE;

        /* Move each customer in queue (if any) up one place. */

        for (i = 1; i <= numbers_in_encoder_queue; i++)
        {
            time_arrival_in_encoder[i] = time_arrival_in_encoder[i + 1];
        }
        numbers_in_storage_queue++;
        storage_queue_frame_size[numbers_in_storage_queue] = frame_size_in_encoder;
        if(numbers_in_storage_queue > 1 && storage_status == IDLE)
        {
            time_next_event[3] = sim_time;
        }
    }
}
void storage()
{
    int i;
    if(numbers_in_storage_queue > 1)
    {
        storage_status = BUSY;
        size_of_storage_frame = ALPHA * (storage_queue_frame_size[1] + storage_queue_frame_size[2]);
        numbers_in_storage_queue = numbers_in_storage_queue - 2;
        time_next_event[3] = sim_time + size_of_storage_frame / STORAGE_RATE;
        for(i = 1 ; i <= numbers_in_storage_queue ; i++)
        {
            storage_queue_frame_size[i] = storage_queue_frame_size[i + 2];
        }
    }
    else
    {
        storage_status = IDLE;
        time_next_event[3] = 1.0e+30;
    }
}
void top_frame_delete()
{
    time_next_event[1] = sim_time + expon(MEAN_INTERARRIVAL) * 2;
}
void bottom_frame_delete()
{
    numbers_in_encoder_queue--;
}



void update_time_avg_stats(void)  /* Update area accumulators for time-average
                                     statistics. */
{
    float time_since_last_event;

    /* Compute time since last event, and update last-event-time marker. */

    time_since_last_event = sim_time - time_last_event;
    time_last_event       = sim_time;

    /* Update area under number-in-queue function. */

    //area_num_in_q      += num_in_q * time_since_last_event;

    /* Update area under server-busy indicator function. */

    storage_busy_time += storage_status * time_since_last_event;
}


float expon(float mean)  /* Exponential variate generation function. */
{
    /* Return an exponential random variate with mean "mean". */

    return -mean * log(lcgrand(1));
}


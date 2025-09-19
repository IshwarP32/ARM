/**
 * @file producer_consumer_example.c
 * @brief Producer-Consumer Example using Queues and Semaphores
 * @author Example Implementation
 * @date 2024
 * 
 * This example demonstrates inter-task communication using message queues
 * and semaphores for synchronization.
 */

#include "rtos_config.h"
#include "task_manager.h"
#include "queue_manager.h"
#include "scheduler.h"

/* Example data structure */
typedef struct {
    uint32_t sequence_number;
    uint32_t data_value;
    uint32_t timestamp;
} data_packet_t;

/* Global variables */
static uint32_t producer_sequence = 0;
static uint32_t consumer_count = 0;

/* Task function prototypes */
void producer_task(void);
void consumer_task(void);
void monitor_task(void);

/* Semaphore IDs */
#define MUTEX_SEMAPHORE     0
#define COUNT_SEMAPHORE     1

/**
 * @brief Initialize producer-consumer example
 */
void producer_consumer_example_init(void)
{
    /* Create message queue for data packets */
    queue_create(QUEUE_1, 8);  /* Queue with 8 slots */
    
    /* Create semaphores */
    semaphore_create(MUTEX_SEMAPHORE, 1, 1);       /* Mutex semaphore */
    semaphore_create(COUNT_SEMAPHORE, 0, 10);      /* Counting semaphore */
    
    /* Create tasks */
    task_create(producer_task, "Producer", PRIORITY_MEDIUM, 512);
    task_create(consumer_task, "Consumer", PRIORITY_MEDIUM, 512);
    task_create(monitor_task, "Monitor", PRIORITY_LOW, 256);
    
    DEBUG_PRINT("Producer-Consumer Example initialized\n");
}

/**
 * @brief Producer task - generates data packets
 */
void producer_task(void)
{
    data_packet_t packet;
    
    while(1)
    {
        /* Prepare data packet */
        packet.sequence_number = ++producer_sequence;
        packet.data_value = producer_sequence * 100 + (producer_sequence % 50);
        packet.timestamp = timer_get_uptime_ms();
        
        /* Try to send packet to queue */
        if(queue_send(QUEUE_1, &packet, 100) == QUEUE_SUCCESS)
        {
            DEBUG_PRINT("Producer: Sent packet #%ld (value: %ld)\n", 
                       packet.sequence_number, packet.data_value);
            
            /* Signal counting semaphore */
            semaphore_give(COUNT_SEMAPHORE);
        }
        else
        {
            DEBUG_PRINT("Producer: Queue full, dropping packet #%ld\n", 
                       packet.sequence_number);
        }
        
        /* Produce at different rates based on sequence number */
        if(producer_sequence % 3 == 0)
        {
            task_delay(150);  /* Slow production every 3rd packet */
        }
        else
        {
            task_delay(75);   /* Normal production rate */
        }
    }
}

/**
 * @brief Consumer task - processes data packets
 */
void consumer_task(void)
{
    data_packet_t packet;
    uint32_t processing_time;
    
    while(1)
    {
        /* Wait for data to be available */
        if(semaphore_take(COUNT_SEMAPHORE, 1000) == RTOS_SUCCESS)
        {
            /* Take mutex before accessing shared resources */
            if(semaphore_take(MUTEX_SEMAPHORE, 100) == RTOS_SUCCESS)
            {
                /* Receive packet from queue */
                if(queue_receive(QUEUE_1, &packet, 0) == QUEUE_SUCCESS)
                {
                    consumer_count++;
                    
                    /* Simulate processing time based on data value */
                    processing_time = 20 + (packet.data_value % 30);
                    
                    DEBUG_PRINT("Consumer: Processing packet #%ld (value: %ld, delay: %ld ms)\n",
                               packet.sequence_number, packet.data_value, processing_time);
                    
                    /* Release mutex */
                    semaphore_give(MUTEX_SEMAPHORE);
                    
                    /* Simulate processing */
                    task_delay(processing_time);
                    
                    DEBUG_PRINT("Consumer: Completed packet #%ld\n", packet.sequence_number);
                }
                else
                {
                    /* Release mutex on error */
                    semaphore_give(MUTEX_SEMAPHORE);
                    DEBUG_PRINT("Consumer: Error receiving packet\n");
                }
            }
            else
            {
                DEBUG_PRINT("Consumer: Mutex timeout\n");
            }
        }
        else
        {
            DEBUG_PRINT("Consumer: No data available (timeout)\n");
        }
    }
}

/**
 * @brief Monitor task - displays system statistics
 */
void monitor_task(void)
{
    uint32_t last_producer_count = 0;
    uint32_t last_consumer_count = 0;
    
    while(1)
    {
        /* Take mutex for safe reading */
        if(semaphore_take(MUTEX_SEMAPHORE, 500) == RTOS_SUCCESS)
        {
            DEBUG_PRINT("=== Producer-Consumer Statistics ===\n");
            DEBUG_PRINT("Produced: %ld packets (+%ld)\n", 
                       producer_sequence, producer_sequence - last_producer_count);
            DEBUG_PRINT("Consumed: %ld packets (+%ld)\n", 
                       consumer_count, consumer_count - last_consumer_count);
            DEBUG_PRINT("Queue Count: %ld\n", queue_get_count(QUEUE_1));
            DEBUG_PRINT("Queue Space: %ld\n", queue_get_space(QUEUE_1));
            DEBUG_PRINT("Count Semaphore: %d\n", semaphore_get_count(COUNT_SEMAPHORE));
            
            /* Calculate throughput */
            uint32_t producer_rate = producer_sequence - last_producer_count;
            uint32_t consumer_rate = consumer_count - last_consumer_count;
            
            DEBUG_PRINT("Production Rate: %ld packets/5s\n", producer_rate);
            DEBUG_PRINT("Consumption Rate: %ld packets/5s\n", consumer_rate);
            
            if(producer_rate > consumer_rate)
            {
                DEBUG_PRINT("Status: Queue filling up\n");
            }
            else if(producer_rate < consumer_rate)
            {
                DEBUG_PRINT("Status: Queue draining\n");
            }
            else
            {
                DEBUG_PRINT("Status: Balanced\n");
            }
            
            /* Update counters */
            last_producer_count = producer_sequence;
            last_consumer_count = consumer_count;
            
            /* Release mutex */
            semaphore_give(MUTEX_SEMAPHORE);
        }
        else
        {
            DEBUG_PRINT("Monitor: Mutex timeout\n");
        }
        
        /* Monitor every 5 seconds */
        task_delay(5000);
    }
}
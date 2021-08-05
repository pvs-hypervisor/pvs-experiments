/* 
 * This script is used for academic and scientific purposes only.
 * 
 * It uses codes from
 *     https://qnaplus.com/implement-periodic-timer-linux/
 * and
 *     https://stackoverflow.com/questions/3756323/how-to-get-the-current-time-in-milliseconds-from-c-in-linux/17083824 
 * 
 * The codes have been adapted for the purposes of the scientific
 * experiments it must support. Use the following command to compile it:
 * 
 *     $ gcc -O3 -o script script.c -lpthread
 * 
 */

#include <stdint.h>
#include <string.h>
#include <sys/timerfd.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>

char * iface;
long long iface_intvl;

long long global_rxp, global_txp, global_rxb, global_txb;
char rx_packets [256], tx_packets [256], rx_bytes [256], tx_bytes [256];

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL);
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}

#define MAX_TIMER_COUNT 1000


typedef void (*time_handler)(size_t timer_id, void * user_data);

int initialize();
size_t start_timer(unsigned int interval, time_handler handler, void * user_data);
void stop_timer(size_t timer_id);
void finalize();

struct timer_node
{
    int                 fd;
    time_handler        callback;
    void *              user_data;
    unsigned int        interval;
    struct timer_node * next;
};

static void * _timer_thread(void * data);
static pthread_t g_thread_id;
static struct timer_node *g_head = NULL;

int initialize()
{
    if(pthread_create(&g_thread_id, NULL, _timer_thread, NULL))
    {
        /*Thread creation failed*/
        return 0;
    }

    return 1;
}

size_t start_timer(unsigned int interval, time_handler handler, void * user_data)
{
    struct timer_node * new_node = NULL;
    struct itimerspec new_value;

    new_node = (struct timer_node *)malloc(sizeof(struct timer_node));

    if(new_node == NULL) return 0;

    new_node->callback  = handler;
    new_node->user_data = user_data;
    new_node->interval  = interval;

    new_node->fd = timerfd_create(CLOCK_REALTIME, 0);

    if (new_node->fd == -1)
    {
        free(new_node);
        return 0;
    }

    new_value.it_value.tv_sec = interval / 1000;
    new_value.it_value.tv_nsec = (interval % 1000)* 1000000;

    new_value.it_interval.tv_sec= interval / 1000;
    new_value.it_interval.tv_nsec = (interval %1000) * 1000000;

    timerfd_settime(new_node->fd, 0, &new_value, NULL);

    /*Inserting the timer node into the list*/
    new_node->next = g_head;
    g_head = new_node;

    return (size_t)new_node;
}

void stop_timer(size_t timer_id)
{
    struct timer_node * tmp = NULL;
    struct timer_node * node = (struct timer_node *)timer_id;

    if (node == NULL) return;

    if(node == g_head)
    {
        g_head = g_head->next;
    } else {

      tmp = g_head;

      while(tmp && tmp->next != node) tmp = tmp->next;

      if(tmp)
      {
          /*tmp->next can not be NULL here*/
          tmp->next = tmp->next->next;
          close(node->fd);
          free(node);
      }
    }
}

void finalize()
{
    while(g_head) stop_timer((size_t)g_head);

    pthread_cancel(g_thread_id);
    pthread_join(g_thread_id, NULL);
}

struct timer_node * _get_timer_from_fd(int fd)
{
    struct timer_node * tmp = g_head;

    while(tmp)
    {
        if(tmp->fd == fd) return tmp;

        tmp = tmp->next;
    }
    return NULL;
}

void * _timer_thread(void * data)
{
    struct pollfd ufds[MAX_TIMER_COUNT] = {{0}};
    int iMaxCount = 0;
    struct timer_node * tmp = NULL;
    int read_fds = 0, i, s;
    uint64_t exp;

    while(1)
    {
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_testcancel();
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

        iMaxCount = 0;
        tmp = g_head;

        memset(ufds, 0, sizeof(struct pollfd)*MAX_TIMER_COUNT);
        while(tmp)
        {
            ufds[iMaxCount].fd = tmp->fd;
            ufds[iMaxCount].events = POLLIN;
            iMaxCount++;

            tmp = tmp->next;
        }
        read_fds = poll(ufds, iMaxCount, 100);

        if (read_fds <= 0) continue;

        for (i = 0; i < iMaxCount; i++)
        {
            if (ufds[i].revents & POLLIN)
            {
                s = read(ufds[i].fd, &exp, sizeof(uint64_t));

                if (s != sizeof(uint64_t)) continue;

                tmp = _get_timer_from_fd(ufds[i].fd);

                if(tmp && tmp->callback) tmp->callback((size_t)tmp, tmp->user_data);
            }
        }
    }

    return NULL;
}

void init_stats()
{
    sprintf (rx_packets, "/sys/class/net/%s/statistics/rx_packets", iface);
    sprintf (tx_packets, "/sys/class/net/%s/statistics/tx_packets", iface);
    sprintf (rx_bytes, "/sys/class/net/%s/statistics/rx_bytes", iface);
    sprintf (tx_bytes, "/sys/class/net/%s/statistics/tx_bytes", iface);
    
    FILE *fp = fopen (rx_packets, "r");
    if (fp == NULL) {
        printf ("file %s not found or cannot be opened.\n", rx_packets);
        exit (1);
    }
    fscanf (fp, "%lld", &global_rxp);
    fclose (fp);

    fp = fopen (tx_packets, "r");
    if (fp == NULL) {
        printf ("file %s not found or cannot be opened.\n", tx_packets);
        exit (1);
    }
    fscanf (fp, "%lld", &global_txp);
    fclose (fp);

    fp = fopen (rx_bytes, "r");
    if (fp == NULL) {
        printf ("file %s not found or cannot be opened.\n", rx_bytes);
        exit (1);
    }
    fscanf (fp, "%lld", &global_rxb);
    fclose (fp);

    fp = fopen (tx_bytes, "r");
    if (fp == NULL) {
        printf ("file %s not found or cannot be opened.\n", tx_bytes);
        exit (1);
    }
    fscanf (fp, "%lld", &global_txb);
    fclose (fp);
}

void collect_stats(size_t timer_id, void * user_data)
{
    long long rxp, txp, rxb, txb;

    FILE *fp = fopen (rx_packets, "r");
    fscanf (fp, "%lld", &rxp);
    fclose (fp);
    fp = fopen (tx_packets, "r");
    fscanf (fp, "%lld", &txp);
    fclose (fp);
    fp = fopen (rx_bytes, "r");
    fscanf (fp, "%lld", &rxb);
    fclose (fp);
    fp = fopen (tx_bytes, "r");
    fscanf (fp, "%lld", &txb);
    fclose (fp);

    printf("%llu TX %s: %lld pkts/s %lld bps RX %s: %lld pkts/s %lld bps \n", current_timestamp(),
           iface, (txp - global_txp)*1000/iface_intvl, (txb - global_txb)*1000/iface_intvl,
           iface, (rxp - global_rxp)*1000/iface_intvl, (rxb - global_rxb)*1000/iface_intvl);
    // fflush (stdout);
    
    global_txp = txp;
    global_txb = txb;
    global_rxp = rxp;
    global_rxb = rxb;
}

void main(int argc, char **argv)
{
    size_t timer_stats;

    if (argc != 3) {
        printf ("usage: %s [network-interface] [interval(miliseconds)]\n\n", argv[0]);
        printf ("   e.g. %s eth0 50\n\n", argv[0]);
        printf ("   shows packets-per-second. interval MUST BE multiple of 50.\n");
        exit (1);
    }
    
    iface = strdup(argv[1]);
    iface_intvl = atoi(argv[2]);
    
    initialize();
    
    init_stats();

    timer_stats = start_timer(iface_intvl, collect_stats, NULL);

    while (1) sleep(1);

    stop_timer(timer_stats);

    finalize();
}

#include <linux/ktime.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/skbuff.h>

#include <linux/if_ether.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>

#include <net/sock.h>
#include <net/ip.h>
#include <net/icmp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yuri Jaschek");
MODULE_DESCRIPTION("@p4drv: p4 driver");

#define printp4(...) printk(KERN_INFO "@p4drv: " __VA_ARGS__);

#define INTERFACE_NAME1 "eth2"
#define INTERFACE_NAME2 "eth3"

#define __GET_KT_TS 1
#define __GET_NET_TS 0
#define __IGNORE_SSH 0

// network interface card resources
static struct packet_type proto_listener1;
static struct net_device *dev1;
static struct socket *sock1;

#ifdef INTERFACE_NAME2
static struct packet_type proto_listener2;
static struct net_device *dev2;
static struct socket *sock2;
#endif

#if (__GET_NET_TS)
static ktime_t curr;
static ktime_t prev;
#endif

#if (__GET_KT_TS)
static ktime_t curr_kt;
static ktime_t prev_kt;
#endif

#if (__IGNORE_SSH)
static u16 ssh_port;
#endif

static u16 iperf_port;

static void
free_dev_and_sock(struct net_device **dev,
                  struct socket **sock)
{
    if(*dev) {
        dev_put(*dev);
        *dev = NULL;
    }
    if(*sock) {
        sock_release(*sock);
        *sock = NULL;
    }
}

static int
get_dev_and_sock(struct net_device **dev,
                 struct socket **sock,
                 const char *interface_name)
{
    *dev = dev_get_by_name(&init_net, interface_name); /// Freed with dev_put()
    if(*dev == NULL) {
        printp4("could not find interface %s\n", interface_name);
        return -1;
    }
    sock_create(PF_PACKET, SOCK_RAW, IPPROTO_RAW, sock); /// Freed with sock_release()
    if(*sock == NULL) {
        printp4("could not create a socket\n");
        return -1;
    }
    return 0 ;
}

int p4drv_recv(struct sk_buff *skb, struct net_device *dev,
               struct packet_type *pt, struct net_device *orig_dev)
{
    int rc = 0;
 
    // struct ethhdr *eth_pkt;
    struct tcphdr *tcp_pkt;
    struct iphdr *ip_pkt;
    // struct udphdr *udp_pkt;
    struct icmphdr *icmp_pkt;
    // u16 frag;
    u32 saddr, daddr;
    u16 sport, dport;
    u8 type;

    // We will only report latency stats from packets originated
    // by iperf3. It sends UDP data blocks in 8K blocks. These blocks
    // are fragmented to the NIC MTU. It means that, for each block,
    // the board will send five 1500+x bytes long packets, plus one
    // 820+x bytes long packet.

    ip_pkt = ip_hdr(skb);
    if (ip_pkt) {
       if (ip_pkt->protocol == IPPROTO_UDP && ip_is_fragment(ip_pkt)) {
         // we likely have an iperf packet. do our job
         saddr = htonl(ip_pkt->saddr);
         daddr = htonl(ip_pkt->daddr);
#if (__GET_NET_TS)
         curr = skb->tstamp;
         printp4("iperf3_udp (%s) skb_ts %llu len %u gap %llu == (id %u) %x > %x",
                 dev ? dev->name : "", curr, skb->len, curr - prev, 
                 ip_pkt->id, saddr, daddr);
         prev = curr;
#endif

#if (__GET_KT_TS)
         curr_kt = ktime_get();
         printp4("iperf3_udp (%s) ktime_ts %llu len %u gap %llu == (id %u) %x > %x",
                 dev ? dev->name : "", curr_kt, skb->len, curr_kt - prev_kt, 
                 ip_pkt->id, saddr, daddr);
         prev_kt = curr_kt;
#endif
       } else if (ip_pkt->protocol == IPPROTO_TCP && (tcp_pkt = tcp_hdr(skb)) &&
                 (tcp_pkt->source == iperf_port || tcp_pkt->dest == iperf_port)) {
         // we most certainly have an iperf packet. do our job
         saddr = htonl(ip_pkt->saddr);
         daddr = htonl(ip_pkt->daddr);
         sport = htons(tcp_pkt->source);
         dport = htons(tcp_pkt->dest);
#if (__GET_NET_TS)
         curr = skb->tstamp;
         printp4("iperf3_tcp (%s) skb_ts %llu len %u gap %llu == (id %u) %x(%u) > %x(%u)",
                 dev ? dev->name : "", curr, skb->len, curr - prev, ip_pkt->id, 
                 saddr, sport, daddr, dport);
         prev = curr;
#endif

#if (__GET_KT_TS)
         curr_kt = ktime_get();
         printp4("iperf3_tcp (%s) ktime_ts %llu len %u gap %llu == (id %u) %x(%u) > %x(%u)",
                 dev ? dev->name : "", curr_kt, skb->len, curr_kt - prev_kt,
                 ip_pkt->id, saddr, sport, daddr, dport);
         prev_kt = curr_kt;
#endif

       } else if (ip_pkt->protocol == IPPROTO_ICMP) {
         // we might be testing with icmp
         icmp_pkt = icmp_hdr(skb);
         type = icmp_pkt ? icmp_pkt->type : 255; // broken, dunno why
         saddr = htonl(ip_pkt->saddr);
         daddr = htonl(ip_pkt->daddr);
#if (__GET_NET_TS)
         curr = skb->tstamp;
         printp4("iperf3_icmp (%s) skb_ts %llu len %u gap %llu == (id %u) %x > %x (type:%u)",
                 dev ? dev->name : "", curr, skb->len, curr - prev, ip_pkt->id, saddr, daddr, type);
         prev = curr;
#endif

#if (__GET_KT_TS)
         curr_kt = ktime_get();
         printp4("iperf3_icmp (%s) ktime_ts %llu len %u gap %llu == (id %u) %x > %x (type:%u)",
                 dev ? dev->name : "", curr_kt, skb->len, curr_kt - prev_kt, ip_pkt->id, saddr, daddr, type);
         prev_kt = curr_kt;
#endif

       } 
    }
    
#if (__IGNORE_SSH)
end_processing:
#endif
    kfree_skb(skb);
    return rc;
}

static int __init nl_p4_init(void)
{
    int err;

    // Get kernel networking stack structures
    err = get_dev_and_sock(&dev1, &sock1, INTERFACE_NAME1);
    if (err) {
        free_dev_and_sock(&dev1, &sock1);
        return -1;
    }

#ifdef INTERFACE_NAME2
    err = get_dev_and_sock(&dev2, &sock2, INTERFACE_NAME2);
    if (err) {
        free_dev_and_sock(&dev1, &sock1);
        free_dev_and_sock(&dev2, &sock2);
        return -1;
    }
#endif

    proto_listener1.type = htons(ETH_P_ALL);
    proto_listener1.dev = dev1; /// NULL is a wildcard, listen on all interfaces
    proto_listener1.func = p4drv_recv;

#ifdef INTERFACE_NAME2
    proto_listener2.type = htons(ETH_P_ALL);
    proto_listener2.dev = dev2; /// NULL is a wildcard, listen on all interfaces
    proto_listener2.func = p4drv_recv;
#endif

#if (__GET_NET_TS)
    net_enable_timestamp();
#endif
    
    // avoid calling htons everytime
#if (__IGNORE_SSH)
    ssh_port = htons(22);
#endif

    iperf_port = htons(5201);
    
    /** Packet sockets are used to receive or send raw packets at the device
     * driver (OSI Layer 2) level. They allow the user to implement
     * protocol modules in user space on top of the physical layer.
     * 
     * Add a protocol handler to the networking stack.
     * The passed packet_type is linked into kernel lists and may not be freed until 
     * it has been removed from the kernel lists.
     */
    dev_add_pack (&proto_listener1);
#ifdef INTERFACE_NAME2
    dev_add_pack (&proto_listener2);
#endif

#if (__GET_NET_TS)
    curr = prev = 0;
#endif

#if (__GET_KT_TS)
    curr_kt = prev_kt = 0;
#endif
    
    printp4("nl_p4_init success");
    printp4("if_name: %s loaded\n", INTERFACE_NAME1);
#ifdef INTERFACE_NAME2
    printp4("if_name: %s loaded\n", INTERFACE_NAME2);
#endif
    return 0;
}

static void __exit nl_p4_exit(void) {
    free_dev_and_sock(&dev1, &sock1);
#ifdef INTERFACE_NAME2
    free_dev_and_sock(&dev2, &sock2);
#endif
    dev_remove_pack(&proto_listener1);
#ifdef INTERFACE_NAME2
    dev_remove_pack(&proto_listener2);
#endif

#if (__GET_NET_TS)
    net_disable_timestamp();
#endif

    printp4("nl_p4_exit success!\n");
}

module_init(nl_p4_init);
module_exit(nl_p4_exit);

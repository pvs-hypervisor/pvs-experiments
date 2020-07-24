## Impact of Virtual Switch Magament

### Environment settings

This folder contains datasets generated during one of our experiments to measure the impact of virtual switch management on top of PvS. We deployed a l2 switch on top of PvS, assigned to VLAN ID 1, and started three virtual machines: VM1, VM2, and VM3, each connected in bridge mode to a different network interface in the host machine (eth0, eth1, and eth2, respectively). These interfaces were, in turn, connected to PvS through a Centec v330 OpenFlow switch, which we used to convert from 10/100/1000 BaseT twisted pair to SFP+ 10G. We also configured the Open vSwitch in the Centec v330 using OpenFlow to add/remove a VLAN tag in packets going back and forth the virtual machines and the virtual l2 switch. Logically, each virtual machine was connected to a different virtual port of the l2 switch. We used toos/send_raw_ip_packets.c to send raw IP packets from VM1, each packet having 1,484 bytes and targeted to a host having hw ether addr 08:22:22:22:22:08. We disabled any other app using the internet in the VMs, and disabled IPv6 using the following command:

    1. sysctl -w net.ipv6.conf.all.disable_ipv6=1

The match-action rules configured in the Centec v330 OpenFlow switch were:

    1. ovs-ofctl add-flow br0 "in_port=1, actions=push_vlan:0x8100,mod_vlan_vid:1,output:49" -O openflow13
    2. ovs-ofctl add-flow br0 "in_port=50,actions=pop_vlan,output:2" -O openflow13
    3. ovs-ofctl add-flow br0 "in_port=51,actions=pop_vlan,output:3" -O openflow13

### Experiment procedures

At instant t = 0, VM1 generates a 450 Mbps packet flow, which is steered by the virtual l2 switch to VM2. Then, at instant t = 10 seconds, we issued a P4Runtime message request to update the match-action table of the virtual l2 switch so that the flow were rerouted to VM3. Finally, at instant t = 20 seconds, we issued a second P4Runtime message request to update the match-action table again, steering traffic back to VM2.

### Data files

The folder dataset/ contains the set of measurements made in the network interfaces of the host machine, using the tools/collect_iface_pkt_ctr_bw_stats.sh script. We use a rate of four data collections per second (one every 250 ms). We used the script tools/parse_pkt_ctr_bw_stats.sh to parse the measurements, and generate the input to the gnuplot script, located in the plot/ dir. To generate each parsed file, we used the following routine:

    1. for i in $(seq 0 2); do
    2.     ../../tools/parse_pkt_ctr_bw_stats.sh bw_eth${i}.txt 1581132882 1581132917 4 > bw_eth${i}.txt.parsed ;
    3. done


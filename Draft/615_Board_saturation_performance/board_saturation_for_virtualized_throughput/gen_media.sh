g++ -O2 -o media media.cpp

./media firewall.txt --scale > firewall_media.txt
./media router.txt --scale > router_media.txt
./media l2.txt --scale > l2_media.txt
./media pvs_total.txt > pvs_total_media.txt

rm media

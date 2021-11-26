#!/bin/bash

# samba
# IP固定，ネーム指定
# vnc接続
# git clone

# スーパーユーザー出なかったら出る
if [ "`whoami`" != "root" ]; then
  echo "Require root privilege"
  echo "Type sudo su"
  exit 1
fi



# 要確認
echo "hdmi強制設定"
sed -i s/'#hdmi_force_hotplug=1'/'hdmi_force_hotplug=1'/ /boot/config.txt
# sed -i s/'hdmi_force_hotplug=1'/'#hdmi_force_hotplug=1'/ /boot/config.txt
echo "hdmi強制完了"

# apt-get update --allow-releaseinfo-change

# IP固定
echo "IP固定設定"
echo "初期化"
sed -i '/interface wlan0/d' /etc/dhcpcd.conf
sed -i '/static ip_address=/d' /etc/dhcpcd.conf
sed -i '/static routers=/d' /etc/dhcpcd.conf
sed -i '/static domain_name_servers/d' /etc/dhcpcd.conf


IPaddress=`hostname -I`
echo $IPaddress
arr=( `echo ${IPaddress} | tr -s '.' ' '` )
# 192.168.11.32
# 0  .1  .2 .3
echo "interface wlan0" >> /etc/dhcpcd.conf
echo "static ip_address=${arr[0]}.${arr[1]}.${arr[2]}.${arr[3]}/24" >> /etc/dhcpcd.conf
echo "static routers=${arr[0]}.${arr[1]}.${arr[2]}.1" >> /etc/dhcpcd.conf
echo "static domain_name_servers=${arr[0]}.${arr[1]}.${arr[2]}.1" >> /etc/dhcpcd.conf

# dhcpcd　再起動
service dhcpcd reload

echo "IP固定完了"



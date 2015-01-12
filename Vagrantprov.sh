#!/bin/sh

apt-get update
apt-get install -y git cmake g++ valgrind

echo "192.168.33.10 korra" | sudo tee -a /etc/hosts
echo "192.168.33.11 katara" | sudo tee -a /etc/hosts
echo "192.168.33.12 azula" | sudo tee -a /etc/hosts

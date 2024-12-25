#!/bin/bash

cp sources.list /etc/apt
sudo apt-get update
cd nachos-project-master
chmod +x install_building_tools.sh
./install_building_tools.sh 
cd -
chmod +x build_nachos.sh                                                                                                                                                                                                                                                          chmod +x build_test.sh
chmod +x coff2noff.sh
./coff2noff.sh


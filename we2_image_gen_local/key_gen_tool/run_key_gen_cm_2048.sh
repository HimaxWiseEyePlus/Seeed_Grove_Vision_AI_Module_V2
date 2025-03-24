#!/bin/bash
# to execute key generation and then copy keys to secureboot_tool/cert/key folder
./key_generation cm_keys_2048.cfg

# move ICV related keys and CM OTP to ICV folder
mv ICV*.pem ICV/
mv ICV*.pwd ICV/
mv *ICV.txt ICV/
# move CM_*.* ICV/
mv CM_otp.* ICV/

# Copy keys to secureboot_tool's key folder
cp ICV/ICV*.pem ../secureboot_tool/cert/key
cp ICV/ICV*.pwd ../secureboot_tool/cert/key
cp ICV/*ICV.txt ../secureboot_tool/cert/key

# COPY keys to key cert tool
cp ICV/ICVRoT.* create_key_cert_tool/input
cp ICV/ICVSBContentPub.* create_key_cert_tool/input

# invoke create_key_cert_tool
cd create_key_cert_tool/
./create_key_cert ICVSBKey1_2LvCert.cfg 2048
cd ..

# move ICV related keys, key cert to sign tool key folder
cp create_key_cert_tool/output/ICVSBKey1.crt ICV/
cp ICV/ICVSBKey1.crt ../secureboot_tool/cert
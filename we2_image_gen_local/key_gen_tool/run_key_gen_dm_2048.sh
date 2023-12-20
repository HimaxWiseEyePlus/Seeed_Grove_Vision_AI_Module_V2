#!/bin/bash
# to execute key generation and then copy keys to secureboot_tool/cert/key folder
./key_generation dm_keys_2048.cfg

# move OEM related keys and CM OTP to OEM folder
mv OEM*.pem OEM/
mv OEM*.pwd OEM/
mv KCE.txt OEM/
mv KCP.txt OEM/
# move DM_*.* OEM/
mv DM_otp.* OEM/

# Copy keys to secureboot_tool's key folder
cp OEM/OEM*.pem ../secureboot_tool/cert/key
cp OEM/OEM*.pwd ../secureboot_tool/cert/key
cp OEM/KCE.txt ../secureboot_tool/cert/key
cp OEM/KCP.txt ../secureboot_tool/cert/key

# COPY keys to key cert tool
cp OEM/OEMRoT.* create_key_cert_tool/input
cp OEM/OEMSBContentPub.* create_key_cert_tool/input

# invoke create_key_cert_tool
cd create_key_cert_tool/
./create_key_cert OEMSBKey1_2LvCert.cfg 2048
cd ..

# move OEM related keys, key cert to sign tool key folder
cp create_key_cert_tool/output/OEMSBKey1.crt OEM/
cp OEM/OEMSBKey1.crt ../secureboot_tool/cert
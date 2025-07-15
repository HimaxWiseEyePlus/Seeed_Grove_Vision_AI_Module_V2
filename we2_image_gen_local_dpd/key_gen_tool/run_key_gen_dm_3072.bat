::to execute key generation and then copy keys to secureboot_tool\cert\key folder
key_generation.exe dm_keys_3072.cfg

:: move OEM related keys and CM OTP to OEM folder
MOVE OEM*.pem OEM\
MOVE OEM*.pwd OEM\
MOVE KCE.txt OEM\
MOVE KCP.txt OEM\
::MOVE DM_*.* OEM\
MOVE DM_otp.*.* OEM\
MOVE prim_key_hash.* OEM\
MOVE zero_bits_in_hash.* OEM\

:: Copy keys to secureboot_tool's key folder
COPY OEM\OEM*.pem ..\secureboot_tool\cert\key
COPY OEM\OEM*.pwd ..\secureboot_tool\cert\key
COPY OEM\KCE.txt ..\secureboot_tool\cert\key
COPY OEM\KCP.txt ..\secureboot_tool\cert\key

::COPY keys to key cert tool
COPY OEM\OEMRoT.* create_key_cert_tool\input
COPY OEM\OEMSBContentPub.* create_key_cert_tool\input

::invoke create_key_cert_tool
CD create_key_cert_tool\
create_key_cert.exe OEMSBKey1_2LvCert.cfg 3072
CD ..

::move OEM related keys, key cert to sign tool key folder
COPY create_key_cert_tool\output\OEMSBKey1.crt OEM\
COPY OEM\OEMSBKey1.crt ..\secureboot_tool\cert
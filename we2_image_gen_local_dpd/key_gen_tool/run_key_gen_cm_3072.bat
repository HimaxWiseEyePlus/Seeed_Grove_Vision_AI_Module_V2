::to execute key generation and then copy keys to secureboot_tool\cert\key folder
key_generation.exe cm_keys_3072.cfg

:: move ICV related keys and CM OTP to ICV folder
MOVE ICV*.pem ICV\
MOVE ICV*.pwd ICV\
MOVE *ICV.txt ICV\
::MOVE CM_*.* ICV\
MOVE CM_otp.*.* ICV\
MOVE HUK.* ICV\
MOVE prim_key_hash.* ICV\
MOVE zero_bits_in_hash.* ICV\

:: Copy keys to secureboot_tool's key folder
COPY ICV\ICV*.pem ..\secureboot_tool\cert\key
COPY ICV\ICV*.pwd ..\secureboot_tool\cert\key
COPY ICV\*ICV.txt ..\secureboot_tool\cert\key

::COPY keys to key cert tool
COPY ICV\ICVRoT.* create_key_cert_tool\input
COPY ICV\ICVSBContentPub.* create_key_cert_tool\input

::invoke create_key_cert_tool
CD create_key_cert_tool\
create_key_cert.exe ICVSBKey1_2LvCert.cfg 3072
CD ..

::move ICV related keys, key cert to sign tool key folder
COPY create_key_cert_tool\output\ICVSBKey1.crt ICV\
COPY ICV\ICVSBKey1.crt ..\secureboot_tool\cert
# WW500 Error Messages
#### CGP 7/4/25

While running thsi software a variety of error are reported. Unfortunately many of them originate in 
library files which I don't have source for.

For the moment I am just going to collate these:

This when reading the RoI Output registers.
```
Motion detected:
[hx_drv_i2cm_write_restart_read]-L:837, Fail:-61
[hx_drv_i2cm_write_restart_read]-L:837, Fail:-61
[hx_drv_i2cm_write_restart_read]-L:837, Fail:-61
[hx_drv_i2cm_write_restart_read]-L:837, Fail:-61
[hx_drv_i2cm_write_restart_read]-L:837, Fail:-64
[hx_drv_i2cm_write_restart_read]-L:837, Fail:-61
```

This happens when reading the two frame counter registers 0x0005 & 0x0006:
Note error 2 is 'HX_CIS_ERROR_I2C'
```
APP_MSG_MAINEVENT_VISIONALGO_STARTDONE
[hx_drv_i2cm_write_restart_read]-L:837, Fail:-64
hx_drv_cis_setRegTable() of 0x0005 fail. err 2
```

As above
```
APP_MSG_MAINEVENT_VISIONALGO_STARTDONE
[hx_drv_i2cm_write_restart_read]-L:837, Fail:-64
hx_drv_cis_setRegTable() of 0x0006 fail. err 2
```

### Noise on SCL?

These seemed to largely go away when I attach a scope probe to the SCL signal, so
I guess there is noise on this line which is causing trouble. Add RC network on the next PCB revision!
	
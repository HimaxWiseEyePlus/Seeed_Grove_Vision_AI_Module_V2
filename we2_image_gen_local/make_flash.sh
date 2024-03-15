WEWaitInput() {
   if [ -f "EPII_CM55M_APP_S/ok" ] && [ -f "EPII_CM55M_APP_S/obj_epii_evb_icv30_bdv10/gnu_epii_evb_WLCSP65/EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf" ]; then
      return 0
   fi

   return 1
}

WEDelete() {
   if [ -f "EPII_CM55M_APP_S/ok" ]; then
      rm -fr "EPII_CM55M_APP_S/ok"
   fi

   if [ -f "EPII_CM55M_APP_S/obj_epii_evb_icv30_bdv10/gnu_epii_evb_WLCSP65/EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf" ]; then
      rm -fr "EPII_CM55M_APP_S/obj_epii_evb_icv30_bdv10/gnu_epii_evb_WLCSP65/EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf"
   fi

   return 0
}

WEMakeImage () {
   echo "make image..."
   cp "EPII_CM55M_APP_S/obj_epii_evb_icv30_bdv10/gnu_epii_evb_WLCSP65/EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf" \
   "we2_image_gen_local/input_case1_secboot" && \
   cd we2_image_gen_local && \
   ./we2_local_image_gen project_case1_blp_wlcsp.json && \
   return 0

   return 1
}

WEMake () {
   WEWaitInput && WEMakeImage && WEDelete || echo "waiting..."
}

while [ 1 == 1 ];
do
   WEMake
   sleep 1
done

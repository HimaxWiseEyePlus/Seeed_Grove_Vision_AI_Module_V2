/*
 * Copyright (c) 2022 Edge Impulse Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS
 * IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language
 * governing permissions and limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AT_COMMAND_SET_H
#define AT_COMMAND_SET_H

/* Edge Impulse CLi is supporting AT Command set version up to currently locked major/minor.
 * If you upgrading major/minor here without upgrading CLI tools, they won't work with the changed firmware.
 */
#define AT_COMMAND_VERSION "1.8.0"

/*************************************************************************************************/
/* mandatory commands required by Edge Impulse CLI Tools*/
#define AT_CLEARCONFIG               "CLEARCONFIG"
#define AT_CLEARCONFIG_HELP_TEXT     "Clears complete config and resets system"
#define AT_CONFIG                    "CONFIG"
#define AT_CONFIG_HELP_TEXT          "Lists complete config"
#define AT_DEVICEID                  "DEVICEID"
#define AT_DEVICEID_ARGS             "DEVICEID"
#define AT_DEVICEID_HELP_TEXT        "Sets the device ID"
#define AT_SAMPLESETTINGS            "SAMPLESETTINGS"
#define AT_SAMPLESETTINGS_ARGS       "LABEL,INTERVAL_MS,LENGTH_MS,[HMAC_KEY]"
#define AT_SAMPLESETTINGS_HELP_TEXT  "Lists or sets current sampling settings"
#define AT_UPLOADSETTINGS            "UPLOADSETTINGS"
#define AT_UPLOADSETTINGS_ARGS       "APIKEY,PATH"
#define AT_UPLOADSETTINGS_HELP_TEXT  "Lists or sets current upload settings"
#define AT_UPLOADHOST                "UPLOADHOST"
#define AT_UPLOADHOST_ARGS           "HOST"
#define AT_UPLOADHOST_HELP_TEXT      "Sets upload host"
#define AT_MGMTSETTINGS              "MGMTSETTINGS"
#define AT_MGMTSETTINGS_ARGS         "URL"
#define AT_MGMTSETTINGS_HELP_TEXT    "Lists or sets current management settings"
#define AT_READFILE                  "READFILE"
#define AT_READFILE_ARGS             "FILENAME,[USEMAXRATE]"
#define AT_READFILE_HELP_TEXT        "Read a specific file (as base64)"
#define AT_READBUFFER                "READBUFFER"
#define AT_READBUFFER_ARGS           "START,LENGTH,[USEMAXRATE]"
#define AT_READBUFFER_HELP_TEXT      "Read from the temporary buffer (as base64)"
#define AT_UNLINKFILE                "UNLINKFILE"
#define AT_UNLINKFILE_ARGS           "FILE"
#define AT_UNLINKFILE_HELP_TEXT      "Unlink a specific file"
#define AT_SAMPLESTART               "SAMPLESTART"
#define AT_SAMPLESTART_ARGS          "SENSOR_NAME"
#define AT_SAMPLESTART_HELP_TEXT     "Start sampling"
#define AT_RUNIMPULSE                "RUNIMPULSE"
#define AT_RUNIMPULSE_HELP_TEXT      "Run the impulse"
#define AT_RUNIMPULSEDEBUG           "RUNIMPULSEDEBUG"
#define AT_RUNIMPULSEDEBUG_ARGS      "USEMAXRATE"
#define AT_RUNIMPULSEDEBUG_HELP_TEXT "Run the impulse with additional debug output or live preview"
#define AT_RUNIMPULSECONT            "RUNIMPULSECONT"
#define AT_RUNIMPULSECONT_HELP_TEXT  "Run the impulse continuously"
#define AT_RUNIMPULSESTATIC          "RUNIMPULSESTATIC"
#define AT_RUNIMPULSESTATIC_ARGS     "DEBUG,LENGTH"
#define AT_RUNIMPULSESTATIC_HELP_TEXT "Run the impulse on static data (base64 encoded)"
/*************************************************************************************************/
/* platform specific commands */
#define AT_WIFI                     "WIFI"
#define AT_WIFI_ARGS                "SSID,PASSWORD,SECURITY"
#define AT_WIFI_HELP_TEXT           "Lists or sets WiFi credentials"
#define AT_SCANWIFI                 "SCANWIFI"
#define AT_SCANWIFI_HELP_TEXT       "Scans for WiFi networks"
#define AT_SNAPSHOT                 "SNAPSHOT"
#define AT_SNAPSHOT_ARGS            "WIDTH,HEIGHT,[USEMAXRATE]"
#define AT_SNAPSHOT_HELP_TEXT       "Take a snapshot"
#define AT_SNAPSHOTSTREAM           "SNAPSHOTSTREAM"
#define AT_SNAPSHOTSTREAM_ARGS      "WIDTH,HEIGHT,[USEMAXRATE]"
#define AT_SNAPSHOTSTREAM_HELP_TEXT "Take a stream of snapshot stream"

/*************************************************************************************************/
/* optional commands (not required by Edge Impulse CLI Tools) */
#define AT_CLEARFILES           "CLEARFILES"
#define AT_CLEARFILES_HELP_TEXT "Clears all files from the file system, this does not clear config"
#define AT_DEVICEINFO           "DEVICEINFO"
#define AT_DEVICEINFO_HELP_TEXT "Lists device information"
#define AT_SENSORS              "SENSORS"
#define AT_SENSORS_HELP_TEXT    "Lists sensors"
#define AT_RESET                "RESET"
#define AT_RESET_HELP_TEXT      "Reset the system"
#define AT_LISTFILES            "LISTFILES"
#define AT_LISTFILES_HELP_TEXT  "Lists all files on the device"
#define AT_READRAW              "READRAW"
#define AT_READRAW_ARS          "START,LENGTH"
#define AT_READRAW_HELP_TEXT    "Read raw from flash"
#define AT_BOOTMODE             "BOOTMODE"
#define AT_BOOTMODE_HELP_TEXT   "Jump to bootloader"
#define AT_INFO                 "INFO"
#define AT_INFO_HELP_TEXT       "Prints details about compiled firmware and ML model"

/*************************************************************************************************/
/* HELP is not necessary as it is built-in into ATServer and
   any custom implementation is ignored. For documentation purposes only */
#define AT_HELP           "HELP"
#define AT_HELP_HELP_TEXT "Lists all commands"

/*************************************************************************************************/
/* Common AT commands handlers */

bool at_info(void);

#endif /* AT_COMMAND_SET_H */

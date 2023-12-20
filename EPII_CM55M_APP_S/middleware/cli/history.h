/**
 ********************************************************************************************
 *  @file      history.h
 *  @details   This file contains all the data and functions prototypes for the history.
 *  @author    himax/300212
 *  @version   V1.0.0
 *  @date      1-March-2013
 *  @copyright (C) COPYRIGHT, Himax Technologies, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
#ifndef __HISTORY_H__
#define __HISTORY_H__

const char* history_previous(void);
const char* history_next(void);
void history_init(void);
void history_add(const char *line);
void history_edit(const char *line);
void history_forget_editing(void);

#endif /* __HISTORY_H__ */

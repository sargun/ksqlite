/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mutex_wrapper.h
 * Author: sdhillon
 *
 * Created on May 1, 2016, 4:16 AM
 */

#include "sqlite3.h"

#ifndef MUTEX_WRAPPER_H
#define MUTEX_WRAPPER_H
sqlite3_mutex *sqlite3_mutex_alloc(int);
void sqlite3_mutex_free(sqlite3_mutex*);
void sqlite3_mutex_enter(sqlite3_mutex*);
int sqlite3_mutex_try(sqlite3_mutex*);
void sqlite3_mutex_leave(sqlite3_mutex*);
#endif /* MUTEX_WRAPPER_H */


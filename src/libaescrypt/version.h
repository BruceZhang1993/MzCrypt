/*
 * version.h
 *
 * Copyright (C) 2007-2010
 *
 * This software is licensed as "freeware."  Permission to distribute
 * this software in source and binary forms is hereby granted without a
 * fee.  THIS SOFTWARE IS PROVIDED 'AS IS' AND WITHOUT ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * THE AUTHOR SHALL NOT BE HELD LIABLE FOR ANY DAMAGES RESULTING FROM
 * THE USE OF THIS SOFTWARE, EITHER DIRECTLY OR INDIRECTLY, INCLUDING,
 * BUT NOT LIMITED TO, LOSS OF DATA OR DATA BEING RENDERED INACCURATE.
 *
 */

#ifndef __AESCRYPT_VERSION_H__
#define __AESCRYPT_VERSION_H__

#if !defined(CONFIG_MZ_PASSBLOB)
#define LPROG_NAME       L"aescrypt"
#define PROG_NAME        "aescrypt"
#define LPROG_VERSION    L"3.08"
#define PROG_VERSION     "3.08"
#define LPROG_DATE       L"October 17, 2010"
#define PROG_DATE        "October 17, 2010"
#else
#define LPROG_NAME       L"aescrypt"
#define PROG_NAME        "recovery"
#define LPROG_VERSION    L"3.08"
#define PROG_VERSION     "1.00"
#define LPROG_DATE       L"October 17, 2010"
#define PROG_DATE        "October 17, 2010"
#endif

#endif // __AESCRYPT_VERSION_H__

/*
 * Copyright (c) 2015-2016 Swift-tech.Inc. All rights reserved.
 *
 * Swift-tech retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Swift-tech., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Swift-tech all copies of this computer program.  If you
 * are licensed by Swift-tech, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Swift-tech.
 *
 * This computer program contains trade secrets owned by Swift-tech.
 * and, unless unauthorized by Swift-tech in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * SWIFT-TECH EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */
 /*
  * File   : SwiftHikSDK.h
  * Author : TonyHuang
  * Date   : 2016.07
  */

#ifndef _SWIFTHIKSDK_H_
#define _SWIFTHIKSDK_H_

#define SW_SDK_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

int sw_hik_sdk_init( void );
void sw_hik_realplay_start( void );
void sw_hik_realplay_stop( void );


#ifdef __cplusplus
}
#endif /* ENDIF __cplusplus */

#endif /* ENDIF __SWIFTHIKSDK_H_ */


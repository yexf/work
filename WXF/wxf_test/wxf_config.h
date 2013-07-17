/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_config.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/22
**
** Description:≤‚ ‘≈‰÷√Œƒº˛
**************************************************************************************/ 
#ifndef _wxf_config_h_
#define _wxf_config_h_
#define WXF_TEST
#include <wxf_utils.h>
typedef enum wxf_test_type_em
{
	EM_TEST_NONE = 0,
	/*THE START*/
	EM_UTILS,
	
	/*THE END*/
	EM_TEST_CONUT,
}wxf_tt;
const wxf_tsf_ent tsf_ent[EM_TEST_CONUT] = {
	{EM_TEST_NONE,"wxf_test help",NULL},
	{EM_UTILS,"wxf_utils",wxf_utils_test},
};

#endif /*_wxf_config_h_*/

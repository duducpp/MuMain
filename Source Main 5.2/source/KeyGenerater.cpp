// KeyGenerater.cpp: implementation of the CKeyGenerater class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KeyGenerater.h"

CKeyGenerater g_KeyGenerater;

static int KEY_GENERATE_FILTER[MAX_KEY_GENERATER_FILTER][4] = {
  { 321, 37'531'879, 8'734, 32 }, // 0
  { 873, 64'374'332, 3'546, 87 },
  { 537, 24'798'765, 5'798, 32 },
  { 654, 32'498'765, 3'573, 73 },
  { 546, 98'465'432, 6'459, 12 }, // 4
  { 987, 24'654'876, 5'616, 54 },
  { 357, 34'599'876, 8'764, 98 },
  { 665, 78'641'332, 6'547, 54 },
  { 813, 85'132'165, 8'421, 98 },
  { 454, 57'684'216, 6'875, 45 }
};

CKeyGenerater::CKeyGenerater() {}

CKeyGenerater::~CKeyGenerater() {}

DWORD CKeyGenerater::GenerateKeyValue( DWORD dwKeyValue )
{
  DWORD dwRegenerateKeyValue = 0;
  BYTE  btNumericValue       = 0;

  btNumericValue = dwKeyValue % MAX_KEY_GENERATER_FILTER;

  dwRegenerateKeyValue = dwKeyValue * KEY_GENERATE_FILTER[btNumericValue][0] + KEY_GENERATE_FILTER[btNumericValue][1] -
                         KEY_GENERATE_FILTER[btNumericValue][2] / KEY_GENERATE_FILTER[btNumericValue][3];
  return dwRegenerateKeyValue;
}

bool CKeyGenerater::CheckKeyValue( DWORD* dwOldKeyValue, DWORD dwReceiveKeyValue )
{
  DWORD dwGeneratedKeyValue = 0;

  dwGeneratedKeyValue = GenerateKeyValue( *dwOldKeyValue );
  if ( dwReceiveKeyValue == dwGeneratedKeyValue ) {
    *dwOldKeyValue = dwGeneratedKeyValue;
    return true;
  }

  return false;
}

/** @file
    File:       IccMpeBasic.cpp

    Contains:   Implementation of Basic Multi Processing Elements

    Version:    V1

    Copyright:  � see ICC Software License
*/

/*
 * The ICC Software License, Version 0.2
 *
 *
 * Copyright (c) 2003-2012 The International Color Consortium. All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. In the absence of prior written permission, the names "ICC" and "The
 *    International Color Consortium" must not be used to imply that the
 *    ICC organization endorses or promotes products derived from this
 *    software.
 *
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNATIONAL COLOR CONSORTIUM OR
 * ITS CONTRIBUTING MEMBERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the The International Color Consortium. 
 *
 *
 * Membership in the ICC is encouraged when this software is used for
 * commercial purposes. 
 *
 *  
 * For more information on The International Color Consortium, please
 * see <http://www.color.org/>.
 *  
 * 
 */

////////////////////////////////////////////////////////////////////// 
// HISTORY:
//
// -Initial implementation by Max Derhak 1-30-2006
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning( disable: 4786) //disable warning in <list.h>
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "IccMpeBasic.h"
#include "IccIO.h"
#include <map>
#include "IccUtil.h"
#include "IccCAM.h"

#ifdef USEREFICCMAXNAMESPACE
namespace refIccMAX {
#endif

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::CIccFormulaCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccFormulaCurveSegment::CIccFormulaCurveSegment(icFloatNumber start, icFloatNumber end)
{
  m_nReserved = 0;
  m_nReserved2 = 0;
  m_startPoint = start;
  m_endPoint = end;

  m_nFunctionType = 0;
  m_nShortcutType = 0;
  m_nParameters = 0;
  m_params = NULL;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::CIccFormulaCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccFormulaCurveSegment::CIccFormulaCurveSegment(const CIccFormulaCurveSegment &seg)
{
  m_nReserved = seg.m_nReserved;
  m_nReserved2 = seg.m_nReserved2;
  m_startPoint = seg.m_startPoint;
  m_endPoint = seg.m_endPoint;

  m_nFunctionType = seg.m_nFunctionType;
  m_nShortcutType = seg.m_nShortcutType;
  m_nParameters = seg.m_nParameters;

  if (seg.m_params) {
    m_params = (icFloatNumber*)malloc(m_nParameters*sizeof(icFloatNumber));
    memcpy(m_params, seg.m_params, m_nParameters*sizeof(icFloatNumber));
  }
  else
    m_params = NULL;
}

/**
 ******************************************************************************
 * Name: &CIccFormulaCurveSegment::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccFormulaCurveSegment &CIccFormulaCurveSegment::operator=(const CIccFormulaCurveSegment &seg)
{
  if (m_params)
    free(m_params);

  m_nReserved = seg.m_nReserved;
  m_nReserved2 = seg.m_nReserved2;
  m_startPoint = seg.m_startPoint;
  m_endPoint = seg.m_endPoint;

  m_nFunctionType = seg.m_nFunctionType;
  m_nShortcutType = seg.m_nShortcutType;
  m_nParameters = seg.m_nParameters;
  if (seg.m_params) {
    m_params = (icFloatNumber*)malloc(m_nParameters*sizeof(icFloatNumber));
    memcpy(m_params, seg.m_params, m_nParameters*sizeof(icFloatNumber));
  }
  else
    m_params = NULL;

  return (*this);
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::~CIccFormulaCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccFormulaCurveSegment::~CIccFormulaCurveSegment()
{
  if (m_params) {
    free(m_params);
  }
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccFormulaCurveSegment::Describe(std::string &sDescription)
{
  icChar buf[128];

  sDescription += "Segment [";
  if (m_startPoint==icMinFloat32Number)
    sDescription += "-Infinity, ";
  else {
  sprintf(buf, "%.8f, ", m_startPoint);
  sDescription += buf;
  }
  if (m_endPoint==icMaxFloat32Number)
    sDescription += "+Infinity";
  else {
  sprintf(buf, "%.8f", m_endPoint);
  sDescription += buf;
  }
  sprintf(buf, "]\r\nFunctionType: %04Xh\r\n", m_nFunctionType);
  sDescription += buf;

  switch(m_nFunctionType) {
  case 0x0000:
    if (m_params[1]==0.0 && m_params[2]==0.0)
      sprintf(buf, "Y = %.8f\r\n\r\n", m_params[3]);
    else if (m_params[0]==1.0 && m_params[1]==1.0 && m_params[2]==0.0 && m_params[3]==0.0)
      sprintf(buf, "Y = X\r\n\r\n");
    else if (m_params[0]==1.0 && m_params[2]==0.0)
      sprintf(buf, "Y = %.8f * X + %.8f\r\n\r\n", 
              m_params[1], m_params[3]);
    else
      sprintf(buf, "Y = (%.8f * X + %.8f)^%.4f + %.8f\r\n\r\n", 
              m_params[1], m_params[2], m_params[0], m_params[3]);
    sDescription += buf;
    return;

  case 0x0001:
    sprintf(buf, "Y = %.8f * log (%.8f * (X ^ %.8f)  + %.8f) + %.8f\r\n\r\n",
            m_params[1], m_params[2], m_params[0], m_params[3], m_params[4]);
    sDescription += buf;
    return;

  case 0x0002:
    sprintf(buf, "Y = %.8f * (%.8f ^ (%.8f * X + %.8f)) + %.8f\r\n\r\n",
            m_params[0], m_params[1], m_params[2], m_params[3], m_params[4]);
    sDescription += buf;
    return;

  case 0x0003:
    if (m_params[1]==0.0 && m_params[2]==0.0)
      sprintf(buf, "Y = %.8f\r\n\r\n", m_params[3]);
    else if (m_params[0]==1.0 && m_params[1]==1.0 && m_params[2]==1.0 && m_params[3]==0.0 && m_params[4]==0.0)
      sprintf(buf, "Y = X\r\n\r\n");
    else if (m_params[0]==1.0 && m_params[1]==1.0 && m_params[3]==0.0)
      sprintf(buf, "Y = %.8f * X + %.8f\r\n\r\n", 
              m_params[2], m_params[3]);
    else if (m_params[0]==1.0 && m_params[2]==1.0 && m_params[3]==0.0)
      sprintf(buf, "Y = %.8f * X + %.8f\r\n\r\n", 
      m_params[1], m_params[3]);
    else
      sprintf(buf, "Y = %8f * (%.8f * X + %.8f)^%.4f + %.8f\r\n\r\n", 
              m_params[1], m_params[2], m_params[3], m_params[0], m_params[4]);
    sDescription += buf;
    return;

  default:
    int i;
    sprintf(buf, "Unknown Function with %d parameters:\r\n\r\n", m_nParameters);
    sDescription += buf;

    for (i=0; i<m_nParameters; i++) {
      sprintf(buf, "Param[%d] = %.8lf\r\n\r\n", i, m_params[i]);
      sDescription += buf;
    }
  }
}


/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::SetFunction
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccFormulaCurveSegment::SetFunction(icUInt16Number functionType, icUInt8Number num_parameters, icFloatNumber *parameters)
{
  if (m_params)
    free(m_params);

  if (num_parameters) {
    m_params = (icFloatNumber*)malloc(num_parameters * sizeof(icFloatNumber));
    memcpy(m_params, parameters, num_parameters * sizeof(icFloatNumber));
  }
  else
    m_params = NULL;

  m_nFunctionType = functionType;
  m_nParameters = num_parameters;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccFormulaCurveSegment::Read(icUInt32Number size, CIccIO *pIO)
{
  icCurveSegSignature sig;

  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&m_nFunctionType))
    return false;

  if (!pIO->Read16(&m_nReserved2))
    return false;

  if (m_params) {
    free(m_params);
  }

  switch(m_nFunctionType) {
    case 0x0000:
      m_nParameters = 4;
      break;

    case 0x0001:
    case 0x0002:
    case 0x0003:
      m_nParameters = 5;
      break;

    default:
      return false;
  }

  if (m_nParameters) {

    m_params = (icFloatNumber*)malloc(m_nParameters * sizeof(icFloatNumber));
    if (!m_params)
      return false;

    if (pIO->ReadFloat32Float(m_params, m_nParameters)!= m_nParameters) {
      return false;
    }
  }
  else
    m_params = NULL;

  return true;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccFormulaCurveSegment::Write(CIccIO *pIO)
{
  icCurveSegSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nFunctionType))
    return false;

  if (!pIO->Write16(&m_nReserved2))
    return false;

  switch(m_nFunctionType) {
    case 0x0000:
      if (m_nParameters!=4)
        return false;
      break;

    case 0x0001:
    case 0x0002:
    case 0x0003:
      if (m_nParameters!=5)
        return false;
      break;
  }

  if (m_nParameters) {
    if (pIO->WriteFloat32Float(m_params, m_nParameters)!=m_nParameters)
      return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccFormulaCurveSegment::Begin(CIccCurveSegment *pPrevSeg = NULL)
{
  switch (m_nFunctionType) {
  case 0x0000:
    if (!m_params || m_nParameters<4)
      return false;

    if (m_params[0] == (icFloatNumber)1.0) { //don't apply gamma
      if (m_params[2] == (icFloatNumber)0.0 && m_params[3] == (icFloatNumber)0.0)
        m_nShortcutType = 1;
      else if (m_params[2] == 0.0)
        m_nShortcutType = 2;
      else if (m_params[3] == 0.0)
        m_nShortcutType = 3;
      else
        m_nShortcutType = 4;
    }
    else {
      m_nShortcutType = 0;
    }

    return true;

  case 0x0001:
    if (!m_params || m_nParameters<5)
      return false;

    return true;

  case 0x0002:
  case 0x0003:
    if (!m_params || m_nParameters<5)
      return false;
    
    return true;

  default:
    return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icFloatNumber CIccFormulaCurveSegment::Apply(icFloatNumber v) const
{
  switch (m_nFunctionType) {
  case 0x0000:
    //Y = (a * X + b) ^ g  + c        : g a b c
    switch (m_nShortcutType) {
    case 0:
    default:
      return (icFloatNumber)(pow(m_params[1] * v + m_params[2], m_params[0]) + m_params[3]);
    case 1:
      return (m_params[1] * v);
    case 2:
      return (m_params[1] * v + m_params[3]);
    case 3:
      return (m_params[1] * v + m_params[2]);
    case 4:
      return (m_params[1] * v + m_params[2] + m_params[3]);
    }

  case 0x0001:
    // Y = a * log (b * X^g + c) + d  : g a b c d
    return (icFloatNumber)(m_params[1] * log10(m_params[2] * pow(v, m_params[0]) + m_params[3]) + m_params[4]);

  case 0x0002:
    //Y = a * b^(c*X+d) + e           : a b c d e
    return (icFloatNumber)(m_params[0] * pow(m_params[1], m_params[2] * v + m_params[3]) + m_params[4]);

  case 0x0003:
    //Y = a * (b * X + c) ^ g  + d        : g a b c d
    return (icFloatNumber)(m_params[1] * pow(m_params[2] * v + m_params[3], m_params[0]) + m_params[4]);
  }

  //Shouldn't get here!
  return v;
}

/**
 ******************************************************************************
 * Name: CIccFormulaCurveSegment::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccFormulaCurveSegment::Validate(std::string sigPath, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  CIccInfo Info;
  std::string sSigPathName = Info.GetSigPathName(sigPath);

  icValidateStatus rv = icValidateOK;
  if (m_nReserved || m_nReserved2) {
    sReport += icMsgValidateWarning;
    sReport += sSigPathName;
    sReport += " formula curve has non zero reserved data.\r\n";
    rv = icMaxStatus(rv, icValidateWarning);
  }

  switch (m_nFunctionType) {
  case 0x0000:
    if (!m_params || m_nParameters<4) {
      sReport += icMsgValidateCriticalError;
      sReport += sSigPathName;
      sReport += " formula curve has Invalid formulaCurveSegment parameters.\r\n";
      rv = icMaxStatus(rv, icValidateCriticalError);
    }
    else if (m_nParameters > 4) {
      sReport += icMsgValidateWarning;
      sReport += sSigPathName;
      sReport += " formula curve has too many formulaCurveSegment parameters.\r\n";
      rv = icMaxStatus(rv, icValidateWarning);
    }
    break;

  case 0x0001:
    if (!m_params || m_nParameters<5) {
      sReport += icMsgValidateCriticalError;
      sReport += sSigPathName;
      sReport += " formula curve has Invalid formulaCurveSegment parameters.\r\n";
      rv = icMaxStatus(rv, icValidateCriticalError);
    }
    else if (m_nParameters > 5) {
      sReport += icMsgValidateWarning;
      sReport += sSigPathName;
      sReport += " formula curve has too many formulaCurveSegment parameters.\r\n";
      rv = icMaxStatus(rv, icValidateWarning);
    }
    break;

  case 0x0002:
    if (!m_params || m_nParameters<5) {
      sReport += icMsgValidateCriticalError;
      sReport += sSigPathName;
      sReport += " formula curve has Invalid formulaCurveSegment parameters.\r\n";
      rv = icMaxStatus(rv, icValidateCriticalError);
    }
    else if (m_nParameters > 5) {
      sReport += icMsgValidateWarning;
      sReport += sSigPathName;
      sReport += " formula curve has too many formulaCurveSegment parameters.\r\n";
      rv = icMaxStatus(rv, icValidateWarning);
    }
    break;

  case 0x0003:
    if (!m_params || m_nParameters<5) {
      sReport += icMsgValidateCriticalError;
      sReport += sSigPathName;
      sReport += " formula curve has Invalid formulaCurveSegment parameters.\r\n";
      rv = icMaxStatus(rv, icValidateCriticalError);
    }
    else if (m_nParameters > 5) {
      sReport += icMsgValidateWarning;
      sReport += sSigPathName;
      sReport += " formula curve has too many formulaCurveSegment parameters.\r\n";
      rv = icMaxStatus(rv, icValidateWarning);
    }
    break;

  default:
    {
      icChar buf[128];
      sReport += icMsgValidateCriticalError;
      sReport += sSigPathName;
      sprintf(buf, " formula curve uses unknown formulaCurveSegment function type %d\r\n", m_nFunctionType);
      sReport += buf;
      rv = icMaxStatus(rv, icValidateCriticalError);
    }
  }

  return rv;
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::CIccSampledCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSampledCurveSegment::CIccSampledCurveSegment(icFloatNumber start, icFloatNumber end)
{
  m_nReserved = 0;
  m_startPoint = start;
  m_endPoint = end;
  m_nCount = 0;
  m_pSamples = 0;
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::CIccSampledCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSampledCurveSegment::CIccSampledCurveSegment(const CIccSampledCurveSegment &curve)
{
  m_nReserved = curve.m_nReserved;
  m_startPoint = curve.m_startPoint;
  m_endPoint = curve.m_endPoint;
  m_nCount = curve.m_nCount;

  if (m_nCount) {
    m_pSamples = (icFloatNumber*)malloc(m_nCount * sizeof(icFloatNumber));
    if (m_pSamples)
      memcpy(m_pSamples, curve.m_pSamples, m_nCount * sizeof(icFloatNumber));
    else
      m_nCount = 0;
  }
  else{
    m_pSamples = NULL;
  }
}

/**
 ******************************************************************************
 * Name: &CIccSampledCurveSegment::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSampledCurveSegment &CIccSampledCurveSegment::operator=(const CIccSampledCurveSegment &curve)
{
  if (m_pSamples)
    free(m_pSamples);

  m_nReserved = curve.m_nReserved;
  m_startPoint = curve.m_startPoint;
  m_endPoint = curve.m_endPoint;
  m_nCount = curve.m_nCount;

  if (m_nCount) {
    m_pSamples = (icFloatNumber*)malloc(m_nCount * sizeof(icFloatNumber));
    if (m_pSamples)
      memcpy(m_pSamples, curve.m_pSamples, m_nCount * sizeof(icFloatNumber));
    else
      m_nCount = 0;
  }
  else {
    m_pSamples = NULL;
  }
  return (*this);
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::~CIccSampledCurveSegment
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSampledCurveSegment::~CIccSampledCurveSegment()
{
  if (m_pSamples)
    free(m_pSamples);
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::SetSize
 * 
 * Purpose: 
 *  Sets size of sampled lookup table.  Previous data (if exists) is lost.
 * 
 * Args: 
 *  nCount = number of elements in lut (must be >= 2).  Note: the m_pSample[0] is
 *    initialized from the the previous segment.  It is not saved as part
 *    of Write(), or loaded as part of Read().  It will be initialized during
 *    the call to Begin(),  The actual count of elements written to the file
 *    will be nCount-1
 *  bZeroAlloc = flag to decide if memory should be set to zero.
 * 
 * Return: 
 *  true if allocation successful.
 ******************************************************************************/
bool CIccSampledCurveSegment::SetSize(icUInt32Number nCount, bool bZeroAlloc/*=true*/)
{
  if (!nCount) {
    if (m_pSamples)
      free(m_pSamples);
    m_pSamples = NULL;
    m_nCount = nCount;
    return true;
  }

  if (m_pSamples) {
    free(m_pSamples);
  }

  if (bZeroAlloc)
    m_pSamples = (icFloatNumber*)calloc(nCount, sizeof(icFloatNumber));
  else
    m_pSamples = (icFloatNumber*)malloc(nCount * sizeof(icFloatNumber));

  if (m_pSamples)
    m_nCount = nCount;
  else
    m_nCount = 0;

  return (m_pSamples != NULL);
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccSampledCurveSegment::Describe(std::string &sDescription)
{
  icChar buf[128];

  if (m_nCount<2) {
    sDescription += "Empty Segment [";
    if (m_startPoint==icMinFloat32Number)
      sDescription += "-Infinity, ";
    else {
    sprintf(buf, "%.8f, ", m_startPoint);
    sDescription += buf;
    }
    if (m_endPoint==icMaxFloat32Number)
      sDescription += "+Infinity";
    else {
    sprintf(buf, "%.8f", m_endPoint);
    sDescription += buf;
    }

    sprintf(buf, "]\r\n");
    sDescription += buf;
  }
  else {
    sDescription += "Sampled Segment [";
    if (m_startPoint==icMinFloat32Number)
      sDescription += "-Infinity, ";
    else {
    sprintf(buf, "%.8f, ", m_startPoint);
    sDescription += buf;
    }
    if (m_endPoint==icMaxFloat32Number)
      sDescription += "+Infinity";
    else {
    sprintf(buf, "%.8f", m_endPoint);
    sDescription += buf;
    }
    sprintf(buf, "]\r\n");
    sDescription += buf;
    sDescription += "IN  OUT\r\n";

    icUInt32Number i;

    icFloatNumber range = m_endPoint - m_startPoint;
    icFloatNumber last = (icFloatNumber)(m_nCount-1);

    for (i=1; i<m_nCount; i++) {
      sprintf(buf, "%.8f %.8f\r\n", m_startPoint + (icFloatNumber)i*range/last, m_pSamples[i]);
      sDescription += buf;
    }
  }
  sDescription += "\r\n";
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSampledCurveSegment::Read(icUInt32Number size, CIccIO *pIO)
{
  icCurveSegSignature sig;

  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt32Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read32(&m_nCount))
    return false;

  if (m_nCount > size - headerSize || m_nCount * sizeof(icFloat32Number) > size - headerSize)
    return false;

  //Reserve room for first point who's value comes from previous segment
  m_nCount++;

  if (!SetSize(m_nCount, false))
    return false;

  if (m_nCount) {
    if (pIO->ReadFloat32Float(m_pSamples+1, m_nCount-1)!=(icInt32Number)(m_nCount-1))
      return false;
  }

  //Initialize first point with zero.  Properly initialized during Begin()
  m_pSamples[0] = 0;

  return true;
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSampledCurveSegment::Write(CIccIO *pIO)
{
  icCurveSegSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  icUInt32Number nCount;

  if (m_nCount)
    nCount = m_nCount -1;
  else
    nCount = 0;

  if (!pIO->Write32(&nCount))
    return false;

  //First point in samples is ONLY for interpolation (not saved)
  if (nCount) {
    if (pIO->WriteFloat32Float(m_pSamples+1, nCount)!=(icInt32Number)nCount)
      return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSampledCurveSegment::Begin(CIccCurveSegment *pPrevSeg = NULL)
{
  if (m_nCount<2)
    return false;

  m_range = m_endPoint - m_startPoint;
  m_last = (icFloatNumber)(m_nCount - 1);

  if (m_endPoint-m_startPoint == 0.0)
    return false;

  if (!pPrevSeg)
    return false;

  //Set up interpolation from Application of last segment
  m_pSamples[0] = pPrevSeg->Apply(m_startPoint);

  return true;
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icFloatNumber CIccSampledCurveSegment::Apply(icFloatNumber v) const
{
  if (v<m_startPoint)
    v=m_startPoint;
  else if (v>m_endPoint)
    v=m_endPoint;

  icFloatNumber pos = (v-m_startPoint)/m_range * m_last;
  icUInt32Number index = (icUInt32Number) pos;
  icFloatNumber remainder = pos - (icFloatNumber)index;

  if (remainder==0.0)
    return m_pSamples[index];

  return (icFloatNumber)((1.0-remainder)*m_pSamples[index] + remainder*m_pSamples[index+1]);
}

/**
 ******************************************************************************
 * Name: CIccSampledCurveSegment::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccSampledCurveSegment::Validate(std::string sigPath, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  CIccInfo Info;
  std::string sSigPathName = Info.GetSigPathName(sigPath);

  icValidateStatus rv = icValidateOK;
  if (m_nReserved) {
    sReport += icMsgValidateWarning;
    sReport += sSigPathName;
    sReport += " sampled curve has non zero reserved data.\r\n";
    rv = icValidateWarning;
  }

  if (m_nCount<1) {
    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " sampled curve has too few sample points.\r\n";
    rv = icMaxStatus(rv, icValidateCriticalError);
  }
  else if (m_endPoint-m_startPoint == 0.0) {
    sReport += icMsgValidateWarning;
    sReport += sSigPathName;
    sReport += " sampled curve has a range of zero.\r\n";
    rv = icMaxStatus(rv, icValidateWarning);
  }

  return rv;
}



/**
******************************************************************************
* Name: CIccSingleSampledCurve::CIccSingleSampledCurve
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccSingleSampledCurve::CIccSingleSampledCurve(icFloatNumber first, icFloatNumber last)
{
  m_nReserved = 0;
  m_nCount = 0;
  m_pSamples = 0;

  m_storageType = icValueTypeFloat32;
  m_extensionType = icClipSingleSampledCurve;

  if (first < last) {
    m_firstEntry = first;
    m_lastEntry = last;
  }
  else {
    m_firstEntry = last;
    m_lastEntry = first;
  }

  m_loIntercept = 0;
  m_loSlope = 0;
  m_hiIntercept = 1.0;
  m_hiSlope = 0;
}

/**
******************************************************************************
* Name: CIccSingleSampledCurve::CIccSingleSampledCurve
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccSingleSampledCurve::CIccSingleSampledCurve(const CIccSingleSampledCurve &curve)
{
  m_nReserved = curve.m_nReserved;
  m_nCount = curve.m_nCount;

  m_storageType = curve.m_storageType;
  m_extensionType = curve.m_extensionType;

  if (m_nCount) {
    m_pSamples = (icFloatNumber*)malloc(m_nCount * sizeof(icFloatNumber));
    if (m_pSamples)
      memcpy(m_pSamples, curve.m_pSamples, m_nCount * sizeof(icFloatNumber));
    else
      m_nCount = 0;
  }
  else{
    m_pSamples = NULL;
  }

  m_firstEntry = curve.m_firstEntry;
  m_lastEntry = curve.m_lastEntry;

  m_loIntercept = curve.m_loIntercept;
  m_loSlope = curve.m_loSlope;
  m_hiIntercept = curve.m_hiIntercept;
  m_hiSlope = curve.m_hiSlope;
}

/**
******************************************************************************
* Name: &CIccSingleSampledCurve::operator=
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccSingleSampledCurve &CIccSingleSampledCurve::operator=(const CIccSingleSampledCurve &curve)
{
  if (m_pSamples)
    free(m_pSamples);

  m_nReserved = curve.m_nReserved;
  m_nCount = curve.m_nCount;

  m_storageType = curve.m_storageType;
  m_extensionType = curve.m_extensionType;

  if (m_nCount) {
    m_pSamples = (icFloatNumber*)malloc(m_nCount * sizeof(icFloatNumber));
    if (m_pSamples)
      memcpy(m_pSamples, curve.m_pSamples, m_nCount * sizeof(icFloatNumber));
    else
      m_nCount = 0;
  }
  else {
    m_pSamples = NULL;
  }

  m_firstEntry = curve.m_firstEntry;
  m_lastEntry = curve.m_lastEntry;

  m_loIntercept = curve.m_loIntercept;
  m_loSlope = curve.m_loSlope;
  m_hiIntercept = curve.m_hiIntercept;
  m_hiSlope = curve.m_hiSlope;
  return (*this);
}

/**
******************************************************************************
* Name: CIccSingleSampledCurve::~CIccSingleSampledCurve
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccSingleSampledCurve::~CIccSingleSampledCurve()
{
  if (m_pSamples)
    free(m_pSamples);
}

/**
******************************************************************************
* Name: CIccSingleSampledCurve::SetRange
* 
* Purpose: 
*  Sets range of sampled lookup table.
* 
* Args: 
*  first = first entry input position
*  last = last entry input position
******************************************************************************/
void CIccSingleSampledCurve::SetRange(icFloatNumber first/* =0.0f */, icFloatNumber last/* =1.0f */)
{
  if (first < last) {
    m_firstEntry = first;
    m_lastEntry = last;
  }
  else {
    m_firstEntry = last;
    m_lastEntry = first;
  }
}


/**
******************************************************************************
* Name: CIccSingleSampledCurve::SetExtensionType
*
* Purpose:
*  Sets extension type of the single sampled curve
*
* Args:
*  nExtensionType is type of extension to use
*
* Return:
*  true if valid extension type, false otherwise
******************************************************************************/
bool CIccSingleSampledCurve::SetExtensionType(icUInt16Number nExtensionType)
{
  m_extensionType = nExtensionType;

  switch (nExtensionType) {
    case icClipSingleSampledCurve:
    case icExtendSingleSampledCurve:
      return true;
  }
  return false;
}

/**
******************************************************************************
* Name: CIccSingleSampledCurve::SetStorageType
*
* Purpose:
*  Sets storate type of the data stored in the single sampled curve
*
* Args:
*  nStorageType is type of data to use
*
* Return:
*  true if valid storage type, false otherwise
******************************************************************************/
bool CIccSingleSampledCurve::SetStorageType(icUInt16Number nStorateType)
{
  m_storageType = nStorateType;

  switch (nStorateType) {
    case icValueTypeUInt8:
    case icValueTypeUInt16:
    case icValueTypeFloat16:
    case icValueTypeFloat32:
      return true;
  }
  return false;
}


/**
******************************************************************************
* Name: CIccSingleSampledCurve::SetSize
* 
* Purpose: 
*  Sets size of sampled lookup table.  Previous data (if exists) is lost.
* 
* Args: 
*  nCount = number of elements in lut (must be >= 2).  
*  bZeroAlloc = flag to decide if memory should be set to zero.
* 
* Return: 
*  true if allocation successful.
******************************************************************************/
bool CIccSingleSampledCurve::SetSize(icUInt32Number nCount, bool bZeroAlloc/*=true*/)
{
  if (!nCount) {
    if (m_pSamples)
      free(m_pSamples);
    m_pSamples = NULL;
    m_nCount = nCount;
    return true;
  }

  if (m_pSamples) {
    free(m_pSamples);
  }

  if (bZeroAlloc)
    m_pSamples = (icFloatNumber*)calloc(nCount, sizeof(icFloatNumber));
  else
    m_pSamples = (icFloatNumber*)malloc(nCount * sizeof(icFloatNumber));

  if (m_pSamples)
    m_nCount = nCount;
  else
    m_nCount = 0;

  return (m_pSamples != NULL);
}

/**
******************************************************************************
* Name: CIccSingleSampledCurve::Describe
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
void CIccSingleSampledCurve::Describe(std::string &sDescription)
{
  icChar buf[128];

  if (m_nCount<2) {
    sDescription += "Empty Single Sampled Curve [";
    sprintf(buf, "%.8f, ", m_firstEntry);
    sDescription += buf;

    sprintf(buf, "%.8f", m_lastEntry);
    sDescription += buf;

    sprintf(buf, "]\r\n");
    sDescription += buf;
  }
  else {
    sDescription += "Single Sampled Curve [";

    sprintf(buf, "%.8f, ", m_firstEntry);
    sDescription += buf;

    sprintf(buf, "%.8f", m_lastEntry);
    sDescription += buf;

    sprintf(buf, "]\r\n");
    sDescription += buf;

    switch(m_storageType) {
      case icValueTypeUInt8:
        sDescription +="UInt8Number Array\n";
        break;
      case icValueTypeUInt16:
        sDescription +="UInt16Number Array\n";
        break;
      case icValueTypeFloat16:
        sDescription +="Float16Number Array\n";
        break;
      case icValueTypeFloat32:
        sDescription +="Float32Number Array\n";
        break;
      default:
        sDescription +="Unknown Data Type\n";
        break;
    }

    switch(m_extensionType) {
      case icClipSingleSampledCurve:
        sDescription += "Clip End Points\n";
        break;
      case icExtendSingleSampledCurve:
        sDescription += "Extend End Points\n";
        break;
      default:
        sDescription += "Unknown extension handling\n";
        break;
    }
    sDescription += "IN  OUT\r\n";

    icUInt32Number i;

    icFloatNumber range = m_lastEntry - m_firstEntry;
    icFloatNumber last = (icFloatNumber)(m_nCount-1);

    for (i=0; i<m_nCount; i++) {
      sprintf(buf, "%.8f %.8f\r\n", m_firstEntry + (icFloatNumber)i*range/last, m_pSamples[i]);
      sDescription += buf;
    }
  }
  sDescription += "\r\n";
}

/**
******************************************************************************
* Name: CIccSingleSampledCurve::Read
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
bool CIccSingleSampledCurve::Read(icUInt32Number size, CIccIO *pIO)
{
  icCurveSegSignature sig;

  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt32Number) +
    sizeof(icFloat32Number) +
    sizeof(icFloat32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig) || sig!=GetType())
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read32(&m_nCount))
    return false;

  if (!pIO->ReadFloat32Float(&m_firstEntry))
    return false;

  if (!pIO->ReadFloat32Float(&m_lastEntry))
    return false;

  if (!pIO->Read16(&m_extensionType))
    return false;

  if (!pIO->Read16(&m_storageType))
    return false;

  if (!SetSize(m_nCount, false))
    return false;

  if (m_nCount > size - headerSize)
    return false;

  if (m_nCount) {
    switch(m_storageType) {
      case icValueTypeUInt8:
        if (m_nCount * sizeof(icUInt8Number) > size - headerSize)
          return false;

        if (pIO->ReadUInt8Float(m_pSamples, m_nCount)!=(icInt32Number)(m_nCount))
          return false;
        break;
      case icValueTypeUInt16:
        if (m_nCount * 2 > size - headerSize)
          return false;

        if (pIO->ReadUInt16Float(m_pSamples, m_nCount)!=(icInt32Number)(m_nCount))
          return false;
        break;
      case icValueTypeFloat16:
        if (m_nCount * 2 > size - headerSize)
          return false;

        if (pIO->ReadFloat16Float(m_pSamples, m_nCount)!=(icInt32Number)(m_nCount))
          return false;
        break;
      case icValueTypeFloat32:
        if (m_nCount * sizeof(icFloat32Number) > size - headerSize)
          return false;

        if (pIO->ReadFloat32Float(m_pSamples, m_nCount)!=(icInt32Number)(m_nCount))
          return false;
        break;
      default:
        return false;
    }
  }

  return true;
}

/**
******************************************************************************
* Name: CIccSingleSampledCurve::Write
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
bool CIccSingleSampledCurve::Write(CIccIO *pIO)
{
  icCurveElemSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write32(&m_nCount))
    return false;

  if (!pIO->WriteFloat32Float(&m_firstEntry))
    return false;

  if (!pIO->WriteFloat32Float(&m_lastEntry))
    return false;

  if (!pIO->Write16(&m_extensionType))
    return false;

  if (!pIO->Write16(&m_storageType))
    return false;

  if (m_nCount) {

    switch(m_storageType) {
      case icValueTypeUInt8:
        if (pIO->WriteUInt8Float(m_pSamples, m_nCount)!=(icInt32Number)(m_nCount))
          return false;
        break;
      case icValueTypeUInt16:
        if (pIO->WriteUInt16Float(m_pSamples, m_nCount)!=(icInt32Number)(m_nCount))
          return false;
        break;
      case icValueTypeFloat16:
        if (pIO->WriteFloat16Float(m_pSamples, m_nCount)!=(icInt32Number)(m_nCount))
          return false;
        break;
      case icValueTypeFloat32:
        if (pIO->WriteFloat32Float(m_pSamples, m_nCount)!=(icInt32Number)(m_nCount))
          return false;
        break;
      default:
        return false;
    }
  }

  return true;
}

/**
******************************************************************************
* Name: CIccSingleSampledCurve::Begin
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
bool CIccSingleSampledCurve::Begin()
{
  if (m_nCount<2)
    return false;

  m_range = m_lastEntry - m_firstEntry;
  m_last = (icFloatNumber)(m_nCount - 1);
  icFloatNumber stepSize = m_range / m_last;

  if (m_range == 0.0)
    return false;

  switch(m_extensionType) {
    case icClipSingleSampledCurve:
      m_loSlope=0.0;
      m_loIntercept = m_pSamples[0];

      m_hiSlope=0.0;
      m_hiIntercept = m_pSamples[m_nCount-1];
      break;

    case icExtendSingleSampledCurve:
      m_loSlope = (m_pSamples[1]-m_pSamples[0])/stepSize;
      m_loIntercept = m_pSamples[0] - m_loSlope*m_firstEntry;

      m_hiSlope = (m_pSamples[m_nCount-1] - m_pSamples[m_nCount-2])/stepSize;
      m_hiIntercept = m_pSamples[m_nCount-1] - m_hiSlope*m_lastEntry;
      break;

    default:
      return false;
  }

  return true;
}

/**
******************************************************************************
* Name: CIccSingleSampledCurve::Apply
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
icFloatNumber CIccSingleSampledCurve::Apply(icFloatNumber v) const
{
  if (v<m_firstEntry) {
    return m_loSlope * v + m_loIntercept;;
  }
  else if (v>m_lastEntry) {
    return m_hiSlope * v + m_hiIntercept;
  }

  icFloatNumber pos = (v-m_firstEntry)/m_range * m_last;
  icUInt32Number index = (icUInt32Number) pos;
  icFloatNumber remainder = pos - (icFloatNumber)index;

  if (remainder==0.0)
    return m_pSamples[index];

  return (icFloatNumber)((1.0-remainder)*m_pSamples[index] + remainder*m_pSamples[index+1]);
}

/**
******************************************************************************
* Name: CIccSingleSampledCurve::Validate
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
icValidateStatus CIccSingleSampledCurve::Validate(std::string sigPath, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  CIccInfo Info;
  std::string sSigPathName = Info.GetSigPathName(sigPath);

  icValidateStatus rv = icValidateOK;
  if (m_nReserved) {
    sReport += icMsgValidateWarning;
    sReport += sSigPathName;
    sReport += " single sampled curve has non zero reserved data.\r\n";
    rv = icValidateWarning;
  }

  if (m_extensionType > icMaxSingleSampledCurveType) {
    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " single sampled curve has unknown extension type\r\n";
    rv = icMaxStatus(rv, icValidateCriticalError);
  }

  if (m_storageType > icMaxValueType) {
    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " single sampled curve uses unknown value type\r\n";
    rv = icMaxStatus(rv, icValidateCriticalError);
  }

  if (m_nCount<2) {
    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " single sampled curve has too few sample points.\r\n";
    rv = icMaxStatus(rv, icValidateCriticalError);
  }

  else if (m_lastEntry-m_firstEntry <= 0.0) {
    sReport += icMsgValidateWarning;
    sReport += sSigPathName;
    sReport += " single sampled curve has an invalid sample range.\r\n";
    rv = icMaxStatus(rv, icValidateWarning);
  }

  return rv;
}

/**
 ******************************************************************************
 * Name: CIccCurveSegment::Create
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccCurveSegment* CIccCurveSegment::Create(icCurveSegSignature sig, icFloatNumber start, icFloatNumber end)
{
  switch(sig) {
  case icSigFormulaCurveSeg:
    return new CIccFormulaCurveSegment(start, end);
  case icSigSampledCurveSeg:
    return new CIccSampledCurveSegment(start, end);
  default:
    return NULL;
  }

}

/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::CIccSegmentedCurve
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSegmentedCurve::CIccSegmentedCurve()
{
  m_list = new CIccCurveSegmentList();
  m_nReserved1 = 0;
  m_nReserved2 = 0;

}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::CIccSegmentedCurve
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSegmentedCurve::CIccSegmentedCurve(const CIccSegmentedCurve &curve)
{
  CIccCurveSegmentList::iterator i;

  m_list = new CIccCurveSegmentList();

  for (i=curve.m_list->begin(); i!=curve.m_list->end(); i++) {
    m_list->push_back((*i)->NewCopy());
  }
  m_nReserved1 = curve.m_nReserved1;
  m_nReserved2 = curve.m_nReserved2;
}


/**
 ******************************************************************************
 * Name: &CIccSegmentedCurve::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSegmentedCurve &CIccSegmentedCurve::operator=(const CIccSegmentedCurve &curve)
{
  Reset();

  CIccCurveSegmentList::iterator i;

  for (i=curve.m_list->begin(); i!=curve.m_list->end(); i++) {
    m_list->push_back((*i)->NewCopy());
  }
  m_nReserved1 = curve.m_nReserved1;
  m_nReserved2 = curve.m_nReserved2;

  return (*this);
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::~CIccSegmentedCurve
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccSegmentedCurve::~CIccSegmentedCurve()
{
  Reset();
  delete m_list;
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccSegmentedCurve::Describe(std::string &sDescription)
{
  CIccCurveSegmentList::iterator i;

  sDescription += "BEGIN_CURVE\r\n";
  for (i=m_list->begin(); i!=m_list->end(); i++) {
    (*i)->Describe(sDescription);
  }
}



/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSegmentedCurve::Read(icUInt32Number size, CIccIO *pIO)
{
  icCurveElemSignature sig;

  icUInt32Number startPos = pIO->Tell();
  
  icUInt32Number headerSize = sizeof(icCurveElemSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (sig!=GetType())
    return false;

  if (!pIO->Read32(&m_nReserved1))
    return false;

  icUInt16Number nSegments;

  if (!pIO->Read16(&nSegments))
    return false;

  if (!pIO->Read16(&m_nReserved2))
    return false;

  Reset();

  icUInt32Number pos = pIO->Tell();
  icCurveSegSignature segSig;
  CIccCurveSegment *pSeg;

  if (nSegments==1) {
    if (sizeof(segSig) > size - headerSize)
      return false;

    if (!pIO->Read32(&segSig))
      return false;
    pSeg = CIccCurveSegment::Create(segSig, icMinFloat32Number, icMaxFloat32Number);
    if (!pSeg)
      return false;

    pIO->Seek(pos, icSeekSet);

    if (!pSeg->Read(size-(pos-startPos), pIO)) {
      delete pSeg;
      return false;
    }

    m_list->push_back(pSeg);
  }
  else if (nSegments) {
    if (nSegments > size - headerSize)
      return false;

    icFloatNumber *breakpoints=(icFloatNumber*)calloc(nSegments-1, sizeof(icFloatNumber));

    if (!breakpoints)
      return false;

    if (pIO->ReadFloat32Float(breakpoints, nSegments-1)!=nSegments-1) {
      free(breakpoints);
      return false;
    }

    int i;
    for (i=0; i<nSegments; i++) {
      pos = pIO->Tell();
      if (!pIO->Read32(&segSig)) {
        free(breakpoints);
        return false;
      }
      if (pIO->Seek(pos, icSeekSet)!=(icInt32Number)pos)
        return false;;

      if (!i)
        pSeg = CIccCurveSegment::Create(segSig, icMinFloat32Number, breakpoints[i]);
      else if (i==nSegments-1) 
        pSeg = CIccCurveSegment::Create(segSig, breakpoints[i-1], icMaxFloat32Number);
      else
        pSeg = CIccCurveSegment::Create(segSig, breakpoints[i-1], breakpoints[i]);
      
      if (!pSeg) {
        free(breakpoints);
        return false;
      }

      if (!pSeg->Read(size-(pos-startPos), pIO)) {
        delete pSeg;
        free(breakpoints);
        return false;
      }

      m_list->push_back(pSeg);
    }

    free(breakpoints);
  }

  return true;
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSegmentedCurve::Write(CIccIO *pIO)
{
  icCurveElemSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved1))
    return false;

  icUInt16Number nSegments = (icUInt16Number)m_list->size();

  if (!pIO->Write16(&nSegments))
    return false;

  if (!pIO->Write16(&m_nReserved2))
    return false;

  CIccCurveSegmentList::iterator i;
  if (nSegments>1) {
    icFloatNumber breakpoint;
    
    i=m_list->begin();
    for (i++; i!=m_list->end(); i++) {
      breakpoint = (*i)->StartPoint();
      if (!pIO->WriteFloat32Float(&breakpoint))
        return false;
    }
  }
  for (i=m_list->begin(); i!=m_list->end(); i++) {
    if (!(*i)->Write(pIO))
      return false;
  }
  
  return true;
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Reset
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccSegmentedCurve::Reset()
{
  CIccCurveSegmentList::iterator i;

  for (i=m_list->begin(); i!=m_list->end(); i++) {
    delete (*i);
  }
  m_list->clear();
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Insert
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSegmentedCurve::Insert(CIccCurveSegment *pCurveSegment)
{
  CIccCurveSegmentList::reverse_iterator last = m_list->rbegin();

  if (last!=m_list->rend()) {
    if (pCurveSegment->StartPoint() == (*last)->EndPoint()) {
      m_list->push_back(pCurveSegment);
      return true;
    }
  }
  else {
    m_list->push_back(pCurveSegment);
    return true;
  }

  return false;
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccSegmentedCurve::Begin()
{
  if (m_list->size()==0)
    return false;

 CIccCurveSegmentList::iterator i;
 CIccCurveSegment *pLast = NULL;

  for (i=m_list->begin(); i!=m_list->end(); i++) {
    if (!(*i)->Begin(pLast))
      return false;
    pLast = *i;
  }

  return true;
}


/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icFloatNumber CIccSegmentedCurve::Apply(icFloatNumber v) const
{
 CIccCurveSegmentList::iterator i;

  for (i=m_list->begin(); i!=m_list->end(); i++) {
    if (v <= (*i)->EndPoint())
      return (*i)->Apply(v);
  }
  return v;
}

/**
 ******************************************************************************
 * Name: CIccSegmentedCurve::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccSegmentedCurve::Validate(std::string sigPath, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  CIccInfo Info;
  std::string sSigPathName = Info.GetSigPathName(sigPath);

  icValidateStatus rv = icValidateOK;
  if (m_nReserved1 || m_nReserved2) {
    sReport += icMsgValidateWarning;
    sReport += sSigPathName;
    sReport += " Segmented curve has non zero reserved data.\r\n";
    rv = icValidateWarning;
  }

  if (m_list->size()==0) {
    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " Has Empty CurveSegment!\r\n";
    return icMaxStatus(rv, icValidateCriticalError);
  }

  CIccCurveSegmentList::iterator i;

  for (i=m_list->begin(); i!=m_list->end(); i++) {
    rv = icMaxStatus(rv, (*i)->Validate(sigPath+icGetSigPath(GetType()), sReport, pMPE));
  }

  return rv;
}


/**
 ******************************************************************************
 * Name: CIccCurveSetCurve::Create
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccCurveSetCurve* CIccCurveSetCurve::Create(icCurveElemSignature sig)
{
  switch (sig) {
  case icSigSegmentedCurve:
    return new CIccSegmentedCurve();
  case icSigSingleSampledCurve:
    return new CIccSingleSampledCurve();
  default:
    return NULL;
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::CIccMpeCurveSet
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCurveSet::CIccMpeCurveSet(int nSize/*=0*/)
{
  m_nReserved = 0;
  if (nSize) {
    m_nInputChannels = m_nOutputChannels = nSize;
    m_curve = (icCurveSetCurvePtr*)calloc(nSize, sizeof(icCurveSetCurvePtr));
    m_position = (icPositionNumber*)calloc(nSize, sizeof(icPositionNumber));
  }
  else {
    m_nInputChannels = m_nOutputChannels = 0;
    m_curve = NULL;
    m_position = NULL;
  }
}

typedef std::map<icCurveSetCurvePtr, icCurveSetCurvePtr> icCurveMap;

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::CIccMpeCurveSet
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCurveSet::CIccMpeCurveSet(const CIccMpeCurveSet &curveSet)
{
  m_nReserved = curveSet.m_nReserved;

  if (curveSet.m_nInputChannels) {
    int i;

    m_nInputChannels = m_nOutputChannels = curveSet.m_nInputChannels;
    m_curve = (icCurveSetCurvePtr*)calloc(m_nInputChannels, sizeof(icCurveSetCurvePtr));
    m_position = (icPositionNumber*)calloc(m_nInputChannels, sizeof(icPositionNumber));

    icCurveMap map;
    for (i=0; i<m_nInputChannels; i++) {
      CIccCurveSetCurve *ptr = curveSet.m_curve[i];
      if (ptr) {
        if (!map[ptr]) {
          m_curve[i] = ptr->NewCopy();
          map[ptr] = m_curve[i];
        }
        else
          m_curve[i] = map[ptr];
      }
    }
  }
  else {
    m_nInputChannels = m_nOutputChannels = 0;
    m_curve = NULL;
  }
}

/**
 ******************************************************************************
 * Name: &CIccMpeCurveSet::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCurveSet &CIccMpeCurveSet::operator=(const CIccMpeCurveSet &curveSet)
{
  m_nReserved = m_nReserved;

  if (m_curve) {
    free(m_curve);
  }

  if (curveSet.m_nInputChannels) {
    int i;

    m_nInputChannels = m_nOutputChannels = curveSet.m_nInputChannels;
    m_curve = (icCurveSetCurvePtr*)calloc(m_nInputChannels, sizeof(icCurveSetCurvePtr));
    m_position = (icPositionNumber*)calloc(m_nInputChannels, sizeof(icPositionNumber));

    icCurveMap map;
    for (i=0; i<m_nInputChannels; i++) {
      CIccCurveSetCurve *ptr = curveSet.m_curve[i];
      if (ptr) {
        if (!map[ptr]) {
          m_curve[i] = ptr->NewCopy();
          map[ptr] = m_curve[i];
        }
        else
          m_curve[i] = map[ptr];
      }
    }
  }
  else {
    m_nInputChannels = m_nOutputChannels = 0;
    m_curve = NULL;
  }

  return *this;
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::~CIccMpeCurveSet
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCurveSet::~CIccMpeCurveSet()
{
  SetSize(0);
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::SetSize
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCurveSet::SetSize(int nNewSize)
{
  if (m_curve) {
    icCurveMap map;
    int i;

    for (i=0; i<m_nInputChannels; i++) {
      if (!map[m_curve[i]]) {
        map[m_curve[i]] = (CIccCurveSetCurve*)1;
        delete m_curve[i];
      }
    }
    free(m_curve);
  }
  if (m_position) {
    free(m_position);
  }

  if (nNewSize) {
    m_curve = (icCurveSetCurvePtr*)calloc(nNewSize, sizeof(icCurveSetCurvePtr));

    if (!m_curve) {
      m_position = NULL;
      m_nInputChannels = m_nOutputChannels = 0;
      return false;
    }

    m_position = (icPositionNumber*)calloc(nNewSize, sizeof(icPositionNumber));

    if (!m_position) {
      free(m_curve);
      m_curve = NULL;

      m_nInputChannels = m_nOutputChannels = 0;
      return false;
    }
    m_nInputChannels = m_nOutputChannels = nNewSize;
  }
  else {
    m_curve = NULL;
    m_nInputChannels = m_nOutputChannels = 0;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::SetCurve
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCurveSet::SetCurve(int nIndex, icCurveSetCurvePtr newCurve)
{
  if (nIndex<0 || nIndex>m_nInputChannels)
    return false;

  if (m_curve) {
    int i;

    for (i=0; i<m_nInputChannels; i++)
      if (i!=nIndex && m_curve[i]==m_curve[nIndex])
        break;

    if (i==m_nInputChannels && m_curve[nIndex]) {
      delete m_curve[nIndex];
    }
  }
  m_curve[nIndex] = newCurve;
  
  return true;
}


/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCurveSet::Describe(std::string &sDescription)
{
  if (m_curve) {
    icChar buf[81];
    int i;

    sprintf(buf, "BEGIN_CURVE_SET %d\r\n", m_nInputChannels);
    sDescription += buf;

    for (i=0; i<m_nInputChannels; i++) {
      sprintf(buf, "Curve %d of %d\r\n", i+1, m_nInputChannels);
      sDescription += buf;
      if (m_curve[i]) {
        m_curve[i]->Describe(sDescription);
      }
    }
    sDescription += "END_CURVE_SET\r\n";
  }
}

typedef std::map<icUInt32Number, CIccCurveSetCurve*> icCurveOffsetMap;
typedef std::map<CIccCurveSetCurve*, icPositionNumber> icCurvePtrMap;

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCurveSet::Read(icUInt32Number size, CIccIO *pIO)
{
  icElemTypeSignature sig;

  icUInt32Number startPos = pIO->Tell();
  
  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  icUInt16Number nInputChannels, nOutputChannels;

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&nInputChannels))
    return false;

  if (!pIO->Read16(&nOutputChannels))
    return false;

  if (nInputChannels != nOutputChannels)
    return false;

  if (nInputChannels > size - headerSize || nInputChannels * 2*sizeof(icUInt32Number) > size - headerSize)
    return false;

  if (!SetSize(nInputChannels))
    return false;

  if (m_curve) {
    int i;

    if (headerSize + m_nInputChannels*2*sizeof(icUInt32Number) > size)
      return false;

    for (i=0; i<m_nInputChannels; i++) {
      if (!pIO->Read32(&m_position[i].offset)) {
        return false;
      }
      if (!pIO->Read32(&m_position[i].size)) {
        return false;
      }
    }

    icCurveOffsetMap map;
    icCurveElemSignature curveSig;
    for (i=0; i<m_nInputChannels; i++) {
      if (!map[m_position[i].offset]) {
        icUInt32Number pos;
        if (!m_position[i].offset || !m_position[i].size) {
          return false;
        }

        pos = startPos + m_position[i].offset;
        if (pIO->Seek(pos, icSeekSet)!=(icInt32Number)pos) {
          return false;
        }
        
        if (!pIO->Read32(&curveSig)) {
          return false;
        }
        m_curve[i] = CIccCurveSetCurve::Create(curveSig);

        if (!m_curve[i]) {
          return false;
        }

        if (pIO->Seek(pos, icSeekSet)!=(icInt32Number)pos) {
          return false;
        }
      
        if (!m_curve[i]->Read(m_position[i].size, pIO)) {
          return false;
        }

        map[m_position[i].offset] = m_curve[i];
      }
      else {
        m_curve[i] = map[m_position[i].offset];
      }
    }
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCurveSet::Write(CIccIO *pIO)
{
  icElemTypeSignature sig = GetType();

  if (!pIO)
    return false;

  icUInt32Number elemStart = pIO->Tell();

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nInputChannels))
    return false;

  if (!pIO->Write16(&m_nInputChannels))
    return false;

  if (m_curve && m_nInputChannels) {
    int i;
    icCurvePtrMap map;
    icUInt32Number start, end;
    icUInt32Number zeros[2] = { 0, 0};
    icPositionNumber position;

    icUInt32Number startTable = pIO->Tell();

    //First write empty position table
    for (i=0; i<m_nInputChannels; i++) {
      if (pIO->Write32(&zeros[0], 2)!=2)
        return false;
    }

    //Now Write curves
    for (i=0; i<m_nInputChannels; i++) {
      if (m_curve[i]) {
        if (map.find(m_curve[i])==map.end()) {
          start = pIO->Tell();
          m_curve[i]->Write(pIO);
          end = pIO->Tell();
          pIO->Align32();
          position.offset = start - elemStart;
          position.size = end - start;
          map[m_curve[i]] = position;
        }
        m_position[i] = map[m_curve[i]];
      }
    }
    end = pIO->Tell();
    
    //Back fill position table
    pIO->Seek(startTable, icSeekSet);
    for (i=0; i<m_nInputChannels; i++) {
      if (!pIO->Write32(&m_position[i].offset))
        return false;
      if (!pIO->Write32(&m_position[i].size))
        return false;
    }

    pIO->Seek(end, icSeekSet);
  }
  
  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCurveSet::Begin(icElemInterp nInterp, CIccTagMultiProcessElement *pMPE)
{
  if (!m_curve)
    return false;

  int i;
  for (i=0; i<m_nInputChannels; i++) {
    if (!m_curve[i] || !m_curve[i]->Begin())
      return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCurveSet::Apply(CIccApplyMpe *pApply, icFloatNumber *pDestPixel, const icFloatNumber *pSrcPixel) const
{
  int i;
  for (i=0; i<m_nInputChannels; i++) {
    *pDestPixel++ = m_curve[i]->Apply(*pSrcPixel++);
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCurveSet::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccMpeCurveSet::Validate(std::string sigPath, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  std::string mpeSigPath = sigPath + icGetSigPath(GetType());
  icValidateStatus rv = CIccMultiProcessElement::Validate(sigPath, sReport, pMPE);

  bool empty=false;
  if (m_curve) {
    int i;
    for (i=0; !empty && i<m_nInputChannels; i++) {
      if (!m_curve[i]) {
        empty = true;
      }
      else {
        rv = icMaxStatus(rv, m_curve[i]->Validate(mpeSigPath, sReport, pMPE));
      }
    }
  }
  else
    empty = true;
  
  if (empty) {
    CIccInfo Info;
    std::string sSigPathName = Info.GetSigPathName(mpeSigPath);

    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " -  Has Empty Curve Element(s)!\r\n";
    return icValidateCriticalError;
  }

  return rv;
}

/**
 ******************************************************************************
 * Name: CIccMpeTintArray::CIccMpeTintArray
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeTintArray::CIccMpeTintArray(int nVectorSize/*=0*/)
{
  m_nReserved = 0;
  m_nInputChannels = 1;
  m_nOutputChannels = nVectorSize;
  m_Array = NULL;
}

typedef std::map<icCurveSetCurvePtr, icCurveSetCurvePtr> icCurveMap;

/**
 ******************************************************************************
 * Name: CIccMpeTintArray::CIccMpeTintArray
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeTintArray::CIccMpeTintArray(const CIccMpeTintArray &tintArray)
{
  m_nReserved = tintArray.m_nReserved;

  m_nInputChannels = tintArray.m_nInputChannels;
  m_nOutputChannels = tintArray.m_nOutputChannels;

  if (tintArray.m_Array)
    m_Array = (CIccTagNumArray*)tintArray.m_Array->NewCopy();

}

/**
 ******************************************************************************
 * Name: &CIccMpeTintArray::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeTintArray &CIccMpeTintArray::operator=(const CIccMpeTintArray &tintArray)
{
  m_nReserved = m_nReserved;

  if (m_Array) {
    delete m_Array;
  }

  m_nInputChannels = tintArray.m_nInputChannels;
  m_nOutputChannels = tintArray.m_nOutputChannels;

  if (tintArray.m_Array)
    m_Array = (CIccTagNumArray*)tintArray.m_Array->NewCopy();

  return *this;
}

/**
 ******************************************************************************
 * Name: CIccMpeTintArray::~CIccMpeTintArray
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeTintArray::~CIccMpeTintArray()
{
  if (m_Array)
    delete m_Array;
}

/**
 ******************************************************************************
 * Name: CIccMpeTintArray::SetVectorSize
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeTintArray::SetVectorSize(int nVectorSize)
{
  m_nOutputChannels = nVectorSize;
}

/**
 ******************************************************************************
 * Name: CIccMpeTintArray::SetCurve
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeTintArray::SetArray(CIccTagNumArray *pArray)
{
  if (m_Array)
    delete m_Array;

  m_Array = pArray;
}


/**
 ******************************************************************************
 * Name: CIccMpeTintArray::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeTintArray::Describe(std::string &sDescription)
{
  if (m_Array) {
    icChar buf[81];

    sprintf(buf, "BEGIN_TINT_ARRAY %d\r\n", m_nOutputChannels);
    sDescription += buf;

    m_Array->Describe(sDescription);

    sDescription += "END_TINT_ARRAY\r\n";
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeTintArray::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeTintArray::Read(icUInt32Number size, CIccIO *pIO)
{
  if (m_Array)
    delete m_Array;
  m_Array = NULL;

  icElemTypeSignature sig;

  icUInt32Number startPos = pIO->Tell();
  
  icUInt32Number headerSize = sizeof(icElemTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number);

  if (headerSize +sizeof(icTagTypeSignature) > size)
    return false;

  if (!pIO) {
    return false;
  }

  icUInt16Number nInputChannels, nOutputChannels;

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&nInputChannels))
    return false;

  if (!pIO->Read16(&nOutputChannels))
    return false;

  if (nInputChannels != 1 || !nOutputChannels)
    return false;

  m_nInputChannels = nInputChannels;
  m_nOutputChannels = nOutputChannels;

  icUInt32Number arrayPos = pIO->Tell();

  icTagTypeSignature tagType;
  if (!pIO->Read32(&tagType))
    return false;

  CIccTag *pTag = CIccTag::Create(tagType);
  if (!pTag)
    return false;

  if (!pTag->IsNumArrayType()) {
    delete pTag;
  }

  m_Array = (CIccTagNumArray*)pTag;
  pIO->Seek(arrayPos, icSeekSet);
  if (!m_Array->Read(size-headerSize, pIO)) {
    return false;
  }

  icUInt32Number nVals = m_Array->GetNumValues();
  if (nVals/m_nOutputChannels <2 || (nVals%m_nOutputChannels) != 0) {
    return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeTintArray::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeTintArray::Write(CIccIO *pIO)
{
  if (!m_Array)
    return false;

  icElemTypeSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nInputChannels))
    return false;

  if (!pIO->Write16(&m_nOutputChannels))
    return false;

  return m_Array->Write(pIO);
}

/**
 ******************************************************************************
 * Name: CIccMpeTintArray::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeTintArray::Begin(icElemInterp nInterp, CIccTagMultiProcessElement *pMPE)
{
  if (!m_Array)
    return false;

  icUInt32Number nVals = m_Array->GetNumValues();

  if (nVals/m_nOutputChannels<2 || nVals % m_nOutputChannels != 0)
    return false;

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeTintArray::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeTintArray::Apply(CIccApplyMpe *pApply, icFloatNumber *pDestPixel, const icFloatNumber *pSrcPixel) const
{
  if (m_Array) {
    m_Array->Interpolate(pDestPixel, *pSrcPixel, m_nOutputChannels);
  }
  else
    memset(pDestPixel, 0, m_nOutputChannels*sizeof(icFloatNumber));
}

/**
 ******************************************************************************
 * Name: CIccMpeTintArray::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccMpeTintArray::Validate(std::string sigPath, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  std::string mpeSigPath = sigPath + icGetSigPath(GetType());
  icValidateStatus rv = CIccMultiProcessElement::Validate(sigPath, sReport, pMPE);

  if (m_nInputChannels!=1) {
    CIccInfo Info;
    std::string sSigPathName = Info.GetSigPathName(mpeSigPath);

    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " -  Bad number of input channels!\r\n";
    return icValidateCriticalError;
  }

  if (!m_nOutputChannels) {
    CIccInfo Info;
    std::string sSigPathName = Info.GetSigPathName(mpeSigPath);

    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " -  Bad number of output channels!\r\n";
    return icValidateCriticalError;
  }

  if (!m_Array) {
    CIccInfo Info;
    std::string sSigPathName = Info.GetSigPathName(mpeSigPath);

    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " -  Has no tint values(s)!\r\n";
    return icValidateCriticalError;
  }
  else {
    icUInt32Number nVals = m_Array->GetNumValues();
    bool bBad = false;
    
    /*
     * NOTE: nVals is unsigned as is m_nOutputChannels
     * so the result will never be < 0
     */
    /*
    if (nVals/m_nOutputChannels<0) {
      CIccInfo Info;
      std::string sSigPathName = Info.GetSigPathName(mpeSigPath);

      sReport += icMsgValidateCriticalError;
      sReport += sSigPathName;
      sReport += " -  Needs two or more tint steps!\r\n";
      bBad = true;
    }
    */
    if ((nVals % m_nOutputChannels)!=0) {
      CIccInfo Info;
      std::string sSigPathName = Info.GetSigPathName(mpeSigPath);

      sReport += icMsgValidateCriticalError;
      sReport += sSigPathName;
      sReport += " -  Array size must be multiple of output channels!\r\n";
      bBad = true;
    }

    if (bBad) {
      return icValidateCriticalError;
    }
  }

  return rv;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::CIccMpeMatrix
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeMatrix::CIccMpeMatrix()
{
  m_nReserved = 0;
  m_nInputChannels = m_nOutputChannels = 0;
  m_size = 0;
  m_pMatrix = NULL;
  m_pConstants = NULL;
}


/**
 ******************************************************************************
 * Name: CIccMpeMatrix::CIccMpeMatrix
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeMatrix::CIccMpeMatrix(const CIccMpeMatrix &matrix)
{
  m_nReserved = matrix.m_nReserved;

  m_nInputChannels = matrix.m_nInputChannels;
  m_nOutputChannels = matrix.m_nOutputChannels;

  m_size = matrix.m_size;
  if(matrix.m_pMatrix) {
    int num = m_size * sizeof(icFloatNumber);
    m_pMatrix = (icFloatNumber*)malloc(num);
    memcpy(m_pMatrix, matrix.m_pMatrix, num);
  }
  else
    m_pMatrix = NULL;

  if (matrix.m_pConstants) {
    int num = m_nOutputChannels*sizeof(icFloatNumber);
    m_pConstants = (icFloatNumber*)malloc(num);
    memcpy(m_pConstants, matrix.m_pConstants, num);
  }
  else
    m_pConstants = NULL;

  m_bApplyConstants = true;
}

/**
 ******************************************************************************
 * Name: &CIccMpeMatrix::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeMatrix &CIccMpeMatrix::operator=(const CIccMpeMatrix &matrix)
{
  m_nReserved = matrix.m_nReserved;

  m_nInputChannels = matrix.m_nInputChannels;
  m_nOutputChannels = matrix.m_nOutputChannels;

  if (m_pMatrix)
    free(m_pMatrix);

  m_size = matrix.m_size;
  if (matrix.m_pMatrix) {
    int num = m_size * sizeof(icFloatNumber);
    m_pMatrix = (icFloatNumber*)malloc(num);
    memcpy(m_pMatrix, matrix.m_pMatrix, num);
  }
  else
    m_pMatrix = NULL;

  if (m_pConstants)
    free(m_pConstants);

  if (matrix.m_pConstants) {
    int num = m_nOutputChannels*sizeof(icFloatNumber);
    m_pConstants = (icFloatNumber*)malloc(num);
    memcpy(m_pConstants, matrix.m_pConstants, num);
  }
  else
    m_pConstants = NULL;

  m_bApplyConstants = matrix.m_bApplyConstants;

  return *this;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::~CIccMpeMatrix
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeMatrix::~CIccMpeMatrix()
{
  if (m_pMatrix)
    free(m_pMatrix);

  if (m_pConstants)
    free(m_pConstants);
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::SetSize
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeMatrix::SetSize(icUInt16Number nInputChannels, icUInt16Number nOutputChannels, bool bUseConstants)
{
  if (m_pMatrix) {
    free(m_pMatrix);
    m_pMatrix = NULL;
  }
  if (m_pConstants) {
    free(m_pConstants);
    m_pConstants = NULL;
  }

  m_size = (icUInt32Number)nInputChannels * nOutputChannels;

  if (m_size) {
    m_pMatrix = (icFloatNumber*)calloc(m_size, sizeof(icFloatNumber));
    if (!m_pMatrix)
      return false;
  }

  if (bUseConstants) {
    m_pConstants = (icFloatNumber*)calloc(nOutputChannels, sizeof(icFloatNumber));

    if (!m_pConstants)
      return false;
  }

  m_nInputChannels = nInputChannels;
  m_nOutputChannels = nOutputChannels;

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeMatrix::Describe(std::string &sDescription)
{
  icChar buf[81];
  int i, j;
  icFloatNumber *data = m_pMatrix;

  sprintf(buf, "BEGIN_ELEM_MATRIX %d %d\r\n", m_nInputChannels, m_nOutputChannels);
  sDescription += buf;

  for (j=0; j<m_nOutputChannels; j++) {
    if (data) {
      for (i=0; i<m_nInputChannels; i++) {
        if (i)
          sDescription += " ";
        sprintf(buf, "%12.8lf", data[i]);
        sDescription += buf;
      }
      if (m_pConstants) {
        sprintf(buf, "  +  %12.8lf\r\n", m_pConstants[j]);
        sDescription += buf;
      }
      data += i;
    }
    else {
      sprintf(buf, "ZeroRow  +  %12.8lf\r\n", m_pConstants[j]);
      sDescription += buf;
    }
  }
  sDescription += "END_ELEM_MATRIX\r\n";
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeMatrix::Read(icUInt32Number size, CIccIO *pIO)
{
  icElemTypeSignature sig;
  
  icUInt32Number headerSize = sizeof(icElemTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number);

  if (headerSize > size)
    return false;

  icUInt32Number dataSize = size - headerSize;

  if (!pIO) {
    return false;
  }

  icUInt16Number nInputChannels, nOutputChannels;

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&nInputChannels))
    return false;

  if (!pIO->Read16(&nOutputChannels))
    return false;

  if (dataSize >= (icUInt32Number)nInputChannels * nOutputChannels * sizeof(icFloatNumber) &&
      dataSize < ((icUInt32Number)nInputChannels+1) * nOutputChannels * sizeof(icFloatNumber)) {
    //Matrix with no constants
    if (!SetSize(nInputChannels, nOutputChannels, false))
      return false;

    if (!m_pMatrix)
      return false;

    //Read Matrix data
    if (pIO->ReadFloat32Float(m_pMatrix, m_size) != (icInt32Number)m_size)
      return false;
  }
  else if (dataSize < (icUInt32Number)nInputChannels * nOutputChannels *sizeof(icFloatNumber) &&
           dataSize >= (icUInt32Number)nOutputChannels * sizeof(icFloatNumber)) {
    //Constants with no matrix
    if (!SetSize(0, nOutputChannels))
      return false;

    m_nInputChannels = nInputChannels;

    //Read Constant data
    if (pIO->ReadFloat32Float(m_pConstants, m_nOutputChannels)!=m_nOutputChannels)
      return false;
  }
  else {
    if ((icUInt32Number)nInputChannels*nOutputChannels > dataSize ||
        ((icUInt32Number)nInputChannels*nOutputChannels + nOutputChannels) > dataSize ||
        ((icUInt32Number)nInputChannels*nOutputChannels + nOutputChannels) * sizeof(icFloat32Number) > dataSize)
      return false;

    //Matrix with constants
    if (!SetSize(nInputChannels, nOutputChannels))
      return false;

    if (!m_pMatrix) 
      return false;

    if ((m_size + nOutputChannels)*sizeof(icFloat32Number) > dataSize)
      return false;

    //Read Matrix data
    if (pIO->ReadFloat32Float(m_pMatrix, m_size)!=(icInt32Number)m_size)
      return false;

    //Read Constant data
    if (pIO->ReadFloat32Float(m_pConstants, m_nOutputChannels)!=m_nOutputChannels)
      return false;
  }
  
  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeMatrix::Write(CIccIO *pIO)
{
  icElemTypeSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nInputChannels))
    return false;

  if (!pIO->Write16(&m_nOutputChannels))
    return false;

  if (m_pMatrix) {
    if (pIO->WriteFloat32Float(m_pMatrix, m_size)!=(icInt32Number)m_size)
      return false;
  }

  //Write Constant data
  if (m_pConstants) {
    if (pIO->WriteFloat32Float(m_pConstants, m_nOutputChannels)!=m_nOutputChannels)
      return false;
  }
  else {
    //Write zero constants because spec says it they have to be there
    icFloat32Number zero = 0;
    int i;
    for (i = 0; i < m_nOutputChannels; i++) {
      if (!pIO->WriteFloat32Float(&zero, 1))
        return false;
    }
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeMatrix::Begin(icElemInterp nInterp, CIccTagMultiProcessElement *pMPE)
{
  m_bApplyConstants = false;
  if (m_pConstants) {
    int i;
    for (i = 0; i < m_nOutputChannels; i++) {
      if (icNotZero(m_pConstants[i])) {
        m_bApplyConstants = true;
        break;
      }
    }
  }

  if (m_nInputChannels==3 && m_nOutputChannels==3)
    m_type = ic3x3Matrix;
  else if (m_nInputChannels==3 && m_nOutputChannels==4)
    m_type = ic3x4Matrix;
  else if (m_nInputChannels==4 && m_nOutputChannels==3)
    m_type = ic4x3Matrix;
  else if (m_nInputChannels==4 && m_nOutputChannels==4)
    m_type = ic4x4Matrix;
  else
    m_type = icOtherMatrix;

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeMatrix::Apply(CIccApplyMpe *pApply, icFloatNumber *dstPixel, const icFloatNumber *srcPixel) const
{
  icFloatNumber *data = m_pMatrix;
  if (data) {
    if (m_bApplyConstants) {
      switch (m_type) {
        case ic3x3Matrix:
          *dstPixel++ = data[ 0]*srcPixel[0] + data[ 1]*srcPixel[1] + data[ 2]*srcPixel[2] + m_pConstants[0];
          *dstPixel++ = data[ 3]*srcPixel[0] + data[ 4]*srcPixel[1] + data[ 5]*srcPixel[2] + m_pConstants[1];
          *dstPixel   = data[ 6]*srcPixel[0] + data[ 7]*srcPixel[1] + data[ 8]*srcPixel[2] + m_pConstants[2];
          break;

        case ic3x4Matrix:
          *dstPixel++ = data[ 0]*srcPixel[0] + data[ 1]*srcPixel[1] + data[ 2]*srcPixel[2] + m_pConstants[0];
          *dstPixel++ = data[ 3]*srcPixel[0] + data[ 4]*srcPixel[1] + data[ 5]*srcPixel[2] + m_pConstants[1];
          *dstPixel++ = data[ 6]*srcPixel[0] + data[ 7]*srcPixel[1] + data[ 8]*srcPixel[2] + m_pConstants[2];
          *dstPixel   = data[ 9]*srcPixel[0] + data[10]*srcPixel[1] + data[11]*srcPixel[2] + m_pConstants[3];
          break;

        case ic4x3Matrix:
          *dstPixel++ = data[ 0]*srcPixel[0] + data[ 1]*srcPixel[1] + data[ 2]*srcPixel[2] + data[ 3]*srcPixel[3] + m_pConstants[0];
          *dstPixel++ = data[ 4]*srcPixel[0] + data[ 5]*srcPixel[1] + data[ 6]*srcPixel[2] + data[ 7]*srcPixel[3] + m_pConstants[1];
          *dstPixel   = data[ 8]*srcPixel[0] + data[ 9]*srcPixel[1] + data[10]*srcPixel[2] + data[11]*srcPixel[3] + m_pConstants[2];
          break;

        case ic4x4Matrix:
          *dstPixel++ = data[ 0]*srcPixel[0] + data[ 1]*srcPixel[1] + data[ 2]*srcPixel[2] + data[ 3]*srcPixel[3] + m_pConstants[0];
          *dstPixel++ = data[ 4]*srcPixel[0] + data[ 5]*srcPixel[1] + data[ 6]*srcPixel[2] + data[ 7]*srcPixel[3] + m_pConstants[1];
          *dstPixel++ = data[ 8]*srcPixel[0] + data[ 9]*srcPixel[1] + data[10]*srcPixel[2] + data[11]*srcPixel[3] + m_pConstants[2];
          *dstPixel   = data[12]*srcPixel[0] + data[13]*srcPixel[1] + data[14]*srcPixel[2] + data[15]*srcPixel[3] + m_pConstants[3];
          break;
      
        case icOtherMatrix:
        default:
          {
            int i, j;

            for (j=0; j<m_nOutputChannels; j++) {
              *dstPixel = m_pConstants[j];

              for (i=0; i<m_nInputChannels; i++) {
                *dstPixel += data[i]*srcPixel[i];
              }

              data += i;
              dstPixel++;
            }
          }
          break;
      }
    }
    else {
      switch (m_type) {
        case ic3x3Matrix:
          *dstPixel++ = data[0] * srcPixel[0] + data[1] * srcPixel[1] + data[2] * srcPixel[2];
          *dstPixel++ = data[3] * srcPixel[0] + data[4] * srcPixel[1] + data[5] * srcPixel[2];
          *dstPixel = data[6] * srcPixel[0] + data[7] * srcPixel[1] + data[8] * srcPixel[2];
          break;

        case ic3x4Matrix:
          *dstPixel++ = data[0] * srcPixel[0] + data[1] * srcPixel[1] + data[2] * srcPixel[2];
          *dstPixel++ = data[3] * srcPixel[0] + data[4] * srcPixel[1] + data[5] * srcPixel[2];
          *dstPixel++ = data[6] * srcPixel[0] + data[7] * srcPixel[1] + data[8] * srcPixel[2];
          *dstPixel = data[9] * srcPixel[0] + data[10] * srcPixel[1] + data[11] * srcPixel[2];
          break;

        case ic4x3Matrix:
          *dstPixel++ = data[0] * srcPixel[0] + data[1] * srcPixel[1] + data[2] * srcPixel[2] + data[3] * srcPixel[3];
          *dstPixel++ = data[4] * srcPixel[0] + data[5] * srcPixel[1] + data[6] * srcPixel[2] + data[7] * srcPixel[3];
          *dstPixel = data[8] * srcPixel[0] + data[9] * srcPixel[1] + data[10] * srcPixel[2] + data[11] * srcPixel[3];
          break;

        case ic4x4Matrix:
          *dstPixel++ = data[0] * srcPixel[0] + data[1] * srcPixel[1] + data[2] * srcPixel[2] + data[3] * srcPixel[3];
          *dstPixel++ = data[4] * srcPixel[0] + data[5] * srcPixel[1] + data[6] * srcPixel[2] + data[7] * srcPixel[3];
          *dstPixel++ = data[8] * srcPixel[0] + data[9] * srcPixel[1] + data[10] * srcPixel[2] + data[11] * srcPixel[3];
          *dstPixel = data[12] * srcPixel[0] + data[13] * srcPixel[1] + data[14] * srcPixel[2] + data[15] * srcPixel[3];
          break;

        case icOtherMatrix:
        default:
          {
            int i, j;

            for (j = 0; j < m_nOutputChannels; j++) {
              *dstPixel = 0.0f;

              for (i = 0; i < m_nInputChannels; i++) {
                *dstPixel += data[i] * srcPixel[i];
              }

              data += i;
              dstPixel++;
            }
          }
          break;
      }
    }
  }
  else if (m_bApplyConstants) {
    memcpy(dstPixel, m_pConstants, m_nOutputChannels*sizeof(icFloatNumber));
  }
  else {
    memset(dstPixel, 0, m_nOutputChannels * sizeof(icFloatNumber));
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeMatrix::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccMpeMatrix::Validate(std::string sigPath, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  std::string mpeSigPath = sigPath + icGetSigPath(GetType());
  icValidateStatus rv = CIccMultiProcessElement::Validate(sigPath, sReport, pMPE);
  
  if (!m_pConstants) {
    CIccInfo Info;
    std::string sSigPathName = Info.GetSigPathName(mpeSigPath);

    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " - Has Empty Matrix Constant data!\r\n";
    return icValidateCriticalError;
  }

  return rv;
}


static icFloatNumber NoClip(icFloatNumber v)
{
  return v;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::CIccMpeCLUT
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCLUT::CIccMpeCLUT()
{
  m_pCLUT = NULL;
  m_nInputChannels = 0;
  m_nOutputChannels = 0;

  m_nReserved = 0;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::CIccMpeCLUT
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCLUT::CIccMpeCLUT(const CIccMpeCLUT &clut)
{
  if (clut.m_pCLUT)
    m_pCLUT = new CIccCLUT(*clut.m_pCLUT);
  else
    m_pCLUT = NULL;

  m_nReserved = clut.m_nReserved;
  m_nInputChannels = clut.m_nInputChannels;
  m_nOutputChannels = clut.m_nOutputChannels;
}

/**
 ******************************************************************************
 * Name: &CIccMpeCLUT::operator=
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCLUT &CIccMpeCLUT::operator=(const CIccMpeCLUT &clut)
{
  if (m_pCLUT)
    delete m_pCLUT;

  if (clut.m_pCLUT)
    m_pCLUT = new CIccCLUT(*clut.m_pCLUT);
  else
    m_pCLUT = NULL;

  m_nReserved = clut.m_nReserved;
  m_nInputChannels = clut.m_nInputChannels;
  m_nOutputChannels = clut.m_nOutputChannels;

  return *this;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::~CIccMpeCLUT
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
CIccMpeCLUT::~CIccMpeCLUT()
{
  if (m_pCLUT)
    delete m_pCLUT;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::SetCLUT
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCLUT::SetCLUT(CIccCLUT *pCLUT)
{
  if (m_pCLUT)
    delete m_pCLUT;

  m_pCLUT = pCLUT;
  if (pCLUT) {
    pCLUT->SetClipFunc(NoClip);
    m_nInputChannels = pCLUT->GetInputDim();
    m_nOutputChannels = pCLUT->GetOutputChannels();
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Describe
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCLUT::Describe(std::string &sDescription)
{
  if (m_pCLUT) {
    m_pCLUT->DumpLut(sDescription, "ELEM_CLUT", icSigUnknownData, icSigUnknownData);
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Read
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCLUT::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;
  
  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number) + 
    16 * sizeof(icUInt8Number);

  if (headerSize > size)
    return false;

  icUInt32Number dataSize = size - headerSize;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&m_nInputChannels))
    return false;

  if (!pIO->Read16(&m_nOutputChannels))
    return false;

  icUInt8Number gridPoints[16];

  if (pIO->Read8(gridPoints, 16)!=16) {
    return false;
  }

  icUInt32Number nPoints = (icUInt32Number)m_nInputChannels * m_nOutputChannels;

  if (m_nInputChannels > 16 || nPoints > dataSize || nPoints * sizeof (icFloat32Number) > dataSize)
    return false;

  m_pCLUT = new CIccCLUT((icUInt8Number)m_nInputChannels, (icUInt16Number)m_nOutputChannels, 4);

  if (!m_pCLUT)
    return false;

  m_pCLUT->SetClipFunc(NoClip);

  m_pCLUT->Init(gridPoints);

  icFloatNumber *pData = m_pCLUT->GetData(0);

  if (!pData)
    return false;

  nPoints = m_pCLUT->NumPoints()*m_nOutputChannels;

  if (pIO->ReadFloat32Float(pData,nPoints)!= nPoints)
    return false;
  
  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Write
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCLUT::Write(CIccIO *pIO)
{
  icElemTypeSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nInputChannels))
    return false;

  if (!pIO->Write16(&m_nOutputChannels))
    return false;

  if (m_pCLUT) {
    icUInt8Number gridPoints[16];
    int i;

    for (i=0; i<16; i++)
      gridPoints[i] = m_pCLUT->GridPoint(i);

    if (pIO->Write8(gridPoints, 16)!=16)
      return false;

    icFloatNumber *pData = m_pCLUT->GetData(0);
    icInt32Number nPoints = m_pCLUT->NumPoints()*m_nOutputChannels;

    if (pIO->WriteFloat32Float(pData, nPoints) != nPoints) 
      return false;
  }

  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Begin
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
bool CIccMpeCLUT::Begin(icElemInterp nInterp, CIccTagMultiProcessElement *pMPE)
{
  if (!m_pCLUT)
    return false;

  m_pCLUT->Begin();

  switch (m_nInputChannels) {
  case 1:
    m_interpType = ic1dInterp;
    break;
  case 2:
    m_interpType = ic2dInterp;
    break;
  case 3:
    if (nInterp==icElemInterpTetra)
      m_interpType = ic3dInterpTetra;
    else
      m_interpType = ic3dInterp;
    break;
  case 4:
    m_interpType = ic4dInterp;
    break;
  case 5:
    m_interpType = ic5dInterp;
    break;
  case 6:
    m_interpType = ic6dInterp;
    break;
  default:
    m_interpType = icNdInterp;
    break;
  }
  return true;
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Apply
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
void CIccMpeCLUT::Apply(CIccApplyMpe *pApply, icFloatNumber *dstPixel, const icFloatNumber *srcPixel) const
{
  const CIccCLUT *pCLUT = m_pCLUT;

  switch(m_interpType) {
  case ic1dInterp:
    pCLUT->Interp1d(dstPixel, srcPixel);
    break;
  case ic2dInterp:
    pCLUT->Interp2d(dstPixel, srcPixel);
    break;
  case ic3dInterpTetra:
    pCLUT->Interp3dTetra(dstPixel, srcPixel);
    break;
  case ic3dInterp:
    pCLUT->Interp3d(dstPixel, srcPixel);
    break;
  case ic4dInterp:
    pCLUT->Interp4d(dstPixel, srcPixel);
    break;
  case ic5dInterp:
    pCLUT->Interp5d(dstPixel, srcPixel);
    break;
  case ic6dInterp:
    pCLUT->Interp6d(dstPixel, srcPixel);
    break;
  case icNdInterp:
    pCLUT->InterpND(dstPixel, srcPixel);
    break;
  }
}

/**
 ******************************************************************************
 * Name: CIccMpeCLUT::Validate
 * 
 * Purpose: 
 * 
 * Args: 
 * 
 * Return: 
 ******************************************************************************/
icValidateStatus CIccMpeCLUT::Validate(std::string sigPath, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  std::string mpeSigPath = sigPath + icGetSigPath(GetType());
  icValidateStatus rv = CIccMultiProcessElement::Validate(sigPath, sReport, pMPE);

  if (!m_pCLUT) {
    CIccInfo Info;
    std::string sSigPathName = Info.GetSigPathName(mpeSigPath);

    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " - Has No CLUT!\r\n";
    return icValidateCriticalError;
  }

  return rv;
}

/**
******************************************************************************
* Name: CIccMpeExtCLUT::CIccMpeExtCLUT
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeExtCLUT::CIccMpeExtCLUT()
{
  m_pCLUT = NULL;
  m_nInputChannels = 0;
  m_nOutputChannels = 0;

  m_nReserved = 0;
  m_nReserved2 = 0;

  m_storageType = icValueTypeFloat32;
}

/**
******************************************************************************
* Name: CIccMpeExtCLUT::CIccMpeExtCLUT
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeExtCLUT::CIccMpeExtCLUT(const CIccMpeExtCLUT &clut) : CIccMpeCLUT(clut)
{
  m_nReserved2 = clut.m_nReserved2;
  m_storageType = clut.m_storageType;
}

/**
******************************************************************************
* Name: &CIccMpeExtCLUT::operator=
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
CIccMpeExtCLUT &CIccMpeExtCLUT::operator=(const CIccMpeExtCLUT &clut)
{
  CIccMpeCLUT::operator=(clut);

  m_nReserved2 = clut.m_nReserved2;
  m_storageType = clut.m_storageType;

  return *this;
}

/**
******************************************************************************
* Name: CIccMpeExtCLUT::Describe
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
void CIccMpeExtCLUT::Describe(std::string &sDescription)
{
  if (m_pCLUT) {
    char desc[256];
    sprintf(desc, "EXT_ELEM_CLUT(%d)", m_storageType);

    m_pCLUT->DumpLut(sDescription, desc, icSigUnknownData, icSigUnknownData);
  }
}

/**
******************************************************************************
* Name: CIccMpeExtCLUT::SetStorageType
*
* Purpose:
*  Sets storage type of the data stored in the CLUT
*
* Args:
*  nStorageType is type of data to use
*
* Return:
*  true if valid storage type, false otherwise
******************************************************************************/
bool CIccMpeExtCLUT::SetStorageType(icUInt16Number nStorateType)
{
  m_storageType = nStorateType;

  switch (nStorateType) {
  case icValueTypeUInt8:
  case icValueTypeUInt16:
  case icValueTypeFloat16:
  case icValueTypeFloat32:
    return true;
  }
  return false;
}


/**
******************************************************************************
* Name: CIccMpeExtCLUT::Read
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
bool CIccMpeExtCLUT::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number) + 
    sizeof(icUInt16Number) +
    sizeof(icUInt16Number) +
    16 * sizeof(icUInt8Number);

  if (headerSize > size)
    return false;

  icUInt32Number dataSize = size - headerSize;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&m_nInputChannels))
    return false;

  if (!pIO->Read16(&m_nOutputChannels))
    return false;

  if (!pIO->Read16(&m_storageType))
    return false;

  if (!pIO->Read16(&m_nReserved2))
    return false;

  icUInt8Number gridPoints[16];

  if (pIO->Read8(gridPoints, 16)!=16) {
    return false;
  }

  icUInt32Number nPoints = (icUInt32Number)m_nInputChannels * m_nOutputChannels;

  if (m_nInputChannels > 16 || nPoints > dataSize)
    return false;
  
  m_pCLUT = new CIccCLUT((icUInt8Number)m_nInputChannels, (icUInt16Number)m_nOutputChannels, 4);

  if (!m_pCLUT)
    return false;

  m_pCLUT->SetClipFunc(NoClip);

  if (!m_pCLUT->Init(gridPoints, dataSize, icGetStorageTypeBytes(m_storageType)))
    return false;

  icFloatNumber *pData = m_pCLUT->GetData(0);

  if (!pData)
    return false;

  nPoints = m_pCLUT->NumPoints()*m_nOutputChannels;

  if (nPoints > dataSize)
    return false;

  switch(m_storageType) {
    case icValueTypeUInt8:
      if (pIO->ReadUInt8Float(pData,nPoints)!= nPoints)
        return false;
      break;

    case icValueTypeUInt16:
      if (nPoints * 2 > dataSize)
        return false;

      if (pIO->ReadUInt16Float(pData,nPoints)!= nPoints)
        return false;
      break;

    case icValueTypeFloat16:
      if (nPoints * 2 > dataSize)
        return false;

      if (pIO->ReadFloat16Float(pData,nPoints)!= nPoints)
        return false;
      break;

    case icValueTypeFloat32:
      if (pIO->ReadFloat32Float(pData,nPoints)!= nPoints)
        return false;
      break;

    default:
      return false;
  }
  return true;
}

/**
******************************************************************************
* Name: CIccMpeExtCLUT::Write
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
bool CIccMpeExtCLUT::Write(CIccIO *pIO)
{
  icElemTypeSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nInputChannels))
    return false;

  if (!pIO->Write16(&m_nOutputChannels))
    return false;

  if (!pIO->Write16(&m_storageType))
    return false;

  if (!pIO->Write16(&m_nReserved2))
    return false;

  if (m_pCLUT) {
    icUInt8Number gridPoints[16];
    int i;

    for (i=0; i<16; i++)
      gridPoints[i] = m_pCLUT->GridPoint(i);

    if (pIO->Write8(gridPoints, 16)!=16)
      return false;

    icFloatNumber *pData = m_pCLUT->GetData(0);
    icInt32Number nPoints = m_pCLUT->NumPoints()*m_nOutputChannels;

    switch(m_storageType) {
    case icValueTypeUInt8:
      if (pIO->WriteUInt8Float(pData,nPoints)!= nPoints)
        return false;
      break;

    case icValueTypeUInt16:
      if (pIO->WriteUInt16Float(pData,nPoints)!= nPoints)
        return false;
      break;

    case icValueTypeFloat16:
      if (pIO->WriteFloat16Float(pData,nPoints)!= nPoints)
        return false;
      break;

    case icValueTypeFloat32:
      if (pIO->WriteFloat32Float(pData,nPoints)!= nPoints)
        return false;
      break;

    default:
      return false;
    }
  }

  return true;
}


/**
******************************************************************************
* Name: CIccMpeExtCLUT::Validate
* 
* Purpose: 
* 
* Args: 
* 
* Return: 
******************************************************************************/
icValidateStatus CIccMpeExtCLUT::Validate(std::string sigPath, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  std::string mpeSigPath = sigPath + icGetSigPath(GetType());
  icValidateStatus rv = CIccMpeCLUT::Validate(sigPath, sReport, pMPE);

  if (m_storageType>icMaxValueType) {
    CIccInfo Info;
    std::string sSigPathName = Info.GetSigPathName(mpeSigPath);

    sReport += icMsgValidateCriticalError;
    sReport += sSigPathName;
    sReport += " - Invalid value type!\r\n";
    return icValidateCriticalError;
  }

  return rv;
}


CIccMpeCAM::CIccMpeCAM()
{
  m_pCAM = NULL;
  m_nInputChannels = 3;
  m_nOutputChannels = 3;
  m_nReserved = 0;
}

CIccMpeCAM::~CIccMpeCAM()
{
  if (m_pCAM)
    delete m_pCAM;
}

bool CIccMpeCAM::Read(icUInt32Number size, CIccIO *pIO)
{
  icTagTypeSignature sig;

  icUInt32Number headerSize = sizeof(icTagTypeSignature) + 
    sizeof(icUInt32Number) + 
    sizeof(icUInt16Number)*2 + 
    8 * sizeof(icFloat32Number);

  if (headerSize > size)
    return false;

  if (!pIO) {
    return false;
  }

  if (!pIO->Read32(&sig))
    return false;

  if (sig!=GetType())
    return false;

  if (!pIO->Read32(&m_nReserved))
    return false;

  if (!pIO->Read16(&m_nInputChannels))
    return false;

  if (!pIO->Read16(&m_nOutputChannels))
    return false;

  icFloatNumber param[8];

  if (pIO->ReadFloat32Float(param, 8)!=8)
    return false;

  if (m_pCAM)
    delete m_pCAM;

  m_pCAM = new CIccCamConverter;

  m_pCAM->SetParameter_WhitePoint(&param[0]);
  m_pCAM->SetParameter_La(param[3]);
  m_pCAM->SetParameter_Yb(param[4]);
  m_pCAM->SetParameter_C(param[5]);
  m_pCAM->SetParameter_Nc(param[6]);
  m_pCAM->SetParameter_F(param[7]);

  return true;
}

bool CIccMpeCAM::Write(CIccIO *pIO)
{
  if (!m_pCAM)
    return false;

  icElemTypeSignature sig = GetType();

  if (!pIO)
    return false;

  if (!pIO->Write32(&sig))
    return false;

  if (!pIO->Write32(&m_nReserved))
    return false;

  if (!pIO->Write16(&m_nInputChannels))
    return false;

  if (!pIO->Write16(&m_nOutputChannels))
    return false;


  icFloatNumber param[8];

  m_pCAM->GetParameter_WhitePoint(&param[0]); //sets param[0], param[1], and param[2]
  param[3] = m_pCAM->GetParameter_La();
  param[4] = m_pCAM->GetParameter_Yb();
  param[5] = m_pCAM->GetParameter_C();
  param[6] = m_pCAM->GetParameter_Nc();
  param[7] = m_pCAM->GetParameter_F();

  if (pIO->WriteFloat32Float(&param, 8)!=8)
    return false;

  return true;
}

bool CIccMpeCAM::Begin(icElemInterp nInterp, CIccTagMultiProcessElement *pMPE)
{
  if (m_pCAM) {
    return true;
  }
  return false;
}

void CIccMpeCAM::SetCAM(CIccCamConverter *pCAM)
{
  if (m_pCAM)
    delete m_pCAM;
  m_pCAM = pCAM;
}

void CIccMpeCAM::Describe(std::string &sDescription)
{
  sDescription += "Begin ";
  sDescription += GetXformName();
  sDescription += "\n";

  if (m_pCAM) {
    char line[256];

    icFloatNumber xyz[3];
    m_pCAM->GetParameter_WhitePoint(xyz);
    sprintf(line, "WhitePoint (X=%f, Y=%f, Z=%f)\n", xyz[0], xyz[1], xyz[2]);
    sDescription += line;

    sprintf(line, "Luminance(La)=%f cd/m^2\n", m_pCAM->GetParameter_La());
    sDescription += line;

    sprintf(line, "Background Luminance(Yb)=%f cd/m^2\n", m_pCAM->GetParameter_Yb());
    sDescription += line;

    sprintf(line, "Impact Surround(C)=%f\n", m_pCAM->GetParameter_C());
    sDescription += line;

    sprintf(line, "Chromatic Induction Factor(Nc)=%f\n", m_pCAM->GetParameter_Nc());
    sDescription += line;

    sprintf(line, "Adaptation Factor(F)=%f\n", m_pCAM->GetParameter_F());
    sDescription += line;

  }
  sDescription += "End";
  sDescription += GetXformName();
  sDescription += "\n";
}


icValidateStatus CIccMpeCAM::Validate(std::string sigPath, std::string &sReport, const CIccTagMultiProcessElement* pMPE/*=NULL*/) const
{
  icValidateStatus rv = icValidateOK;

  if (m_nInputChannels!=3) {
    sReport += icMsgValidateCriticalError;
    sReport += "CAM Element Must have 3 input channels\n";
    rv = icMaxStatus(icValidateCriticalError, rv);
  }
  if (m_nOutputChannels!=3) {
    sReport += icMsgValidateCriticalError;
    sReport += "CAM Element Must have 3 output channels";
    return icMaxStatus(icValidateCriticalError, rv);
  }
  if (!m_pCAM) {
    sReport += icMsgValidateCriticalError;
    sReport += "Invalid CAM";
    return icMaxStatus(icValidateCriticalError, rv);
  }

  return rv;
}

CIccMpeJabToXYZ::CIccMpeJabToXYZ() : CIccMpeCAM()
{
}

CIccMpeJabToXYZ::CIccMpeJabToXYZ(const CIccMpeJabToXYZ &cam)
{
  if (cam.m_pCAM) {
    m_pCAM = new CIccCamConverter();
    icFloatNumber xyz[3];
    cam.m_pCAM->GetParameter_WhitePoint(xyz);
    m_pCAM->SetParameter_WhitePoint(xyz);
    m_pCAM->SetParameter_La(cam.m_pCAM->GetParameter_La());
    m_pCAM->SetParameter_Yb(cam.m_pCAM->GetParameter_Yb());
    m_pCAM->SetParameter_C(cam.m_pCAM->GetParameter_C());
    m_pCAM->SetParameter_Nc(cam.m_pCAM->GetParameter_Nc());
    m_pCAM->SetParameter_F(cam.m_pCAM->GetParameter_F());
  }
  else
    m_pCAM = NULL;
}

CIccMpeJabToXYZ &CIccMpeJabToXYZ::operator=(const CIccMpeJabToXYZ &cam)
{
  if (cam.m_pCAM) {
    if (m_pCAM)
      delete m_pCAM;

    m_pCAM = new CIccCamConverter();
    icFloatNumber xyz[3];
    
    cam.m_pCAM->GetParameter_WhitePoint(xyz);
    m_pCAM->SetParameter_WhitePoint(xyz);
    m_pCAM->SetParameter_La(cam.m_pCAM->GetParameter_La());
    m_pCAM->SetParameter_Yb(cam.m_pCAM->GetParameter_Yb());
    m_pCAM->SetParameter_C(cam.m_pCAM->GetParameter_C());
    m_pCAM->SetParameter_Nc(cam.m_pCAM->GetParameter_Nc());
    m_pCAM->SetParameter_F(cam.m_pCAM->GetParameter_F());
  }
  else
    m_pCAM = NULL;

  return *this;
}

CIccMpeJabToXYZ::~CIccMpeJabToXYZ()
{
}

void CIccMpeJabToXYZ::Apply(CIccApplyMpe *pApply, icFloatNumber *dstPixel, const icFloatNumber *srcPixel) const
{
  if (m_pCAM)
    m_pCAM->JabToXYZ(srcPixel, dstPixel, 1);
}

CIccMpeXYZToJab::CIccMpeXYZToJab() : CIccMpeCAM()
{
}

CIccMpeXYZToJab::CIccMpeXYZToJab(const CIccMpeXYZToJab &cam)
{
  if (cam.m_pCAM) {
    m_pCAM = new CIccCamConverter();
    icFloatNumber xyz[3];

    cam.m_pCAM->GetParameter_WhitePoint(xyz);
    m_pCAM->SetParameter_WhitePoint(xyz);
    m_pCAM->SetParameter_La(cam.m_pCAM->GetParameter_La());
    m_pCAM->SetParameter_Yb(cam.m_pCAM->GetParameter_Yb());
    m_pCAM->SetParameter_C(cam.m_pCAM->GetParameter_C());
    m_pCAM->SetParameter_Nc(cam.m_pCAM->GetParameter_Nc());
    m_pCAM->SetParameter_F(cam.m_pCAM->GetParameter_F());
  }
  else
    m_pCAM = NULL;
}

CIccMpeXYZToJab &CIccMpeXYZToJab::operator=(const CIccMpeXYZToJab &cam)
{
  if (cam.m_pCAM) {
    if (m_pCAM)
      delete m_pCAM;

    m_pCAM = new CIccCamConverter();
    icFloatNumber xyz[3];

    cam.m_pCAM->GetParameter_WhitePoint(xyz);
    m_pCAM->SetParameter_WhitePoint(xyz);
    m_pCAM->SetParameter_La(cam.m_pCAM->GetParameter_La());
    m_pCAM->SetParameter_Yb(cam.m_pCAM->GetParameter_Yb());
    m_pCAM->SetParameter_C(cam.m_pCAM->GetParameter_C());
    m_pCAM->SetParameter_Nc(cam.m_pCAM->GetParameter_Nc());
    m_pCAM->SetParameter_F(cam.m_pCAM->GetParameter_F());
  }
  else
    m_pCAM = NULL;

  return *this;
}

CIccMpeXYZToJab::~CIccMpeXYZToJab()
{
}

void CIccMpeXYZToJab::Apply(CIccApplyMpe *pApply, icFloatNumber *dstPixel, const icFloatNumber *srcPixel) const
{
  if (m_pCAM)
    m_pCAM->XYZToJab(srcPixel, dstPixel, 1);
}


#ifdef USEREFICCMAXNAMESPACE
} //namespace refIccMAX
#endif

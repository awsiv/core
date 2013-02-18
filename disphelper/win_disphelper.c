/* This file is the compacted single file version of the DispHelper COM helper library.
 * DispHelper allows you to call COM objects with an extremely simple printf style syntax.
 * DispHelper can be used from C++ or even plain C. It works with most Windows compilers
 * including Dev-CPP, Visual C++ and LCC-WIN32. Including DispHelper in your project
 * couldn't be simpler as it is available in a compacted single file version (this file).
 *
 * Included with DispHelper are over 20 samples that demonstrate using COM objects
 * including ADO, CDO, Outlook, Eudora, Excel, Word, Internet Explorer, MSHTML,
 * PocketSoap, Word Perfect, MS Agent, SAPI, MSXML, WIA, dexplorer and WMI.
 *
 * DispHelper is free open source software provided under the BSD license.
 *
 * Find out more, browse the readable version of the source code
 * and download DispHelper at:
 * http://sourceforge.net/projects/disphelper/
 * http://disphelper.sourceforge.net/
 */  
    
/* To use DispHelper in your project, include this file(disphelper.c) and the
 * header (disphelper.h). For Visual C++, Borland C++ and LCC-Win32 import
 * libraries are included via pragma directives. For other compilers you may
 * need to add ole32, oleaut32 and uuid. To do this in Dev-CPP add
 * "-lole32 -loleaut32 -luuid" to the linker box under Project->Project Options->Parameters.
 */ 
    
/* If you are using Dev-CPP and get errors when compiling this file:
 * Make sure this file is set to compile as C and not C++ under
 * Project->Project Options->Files.
 */ 
    
#include "platform.h"
#include "compiler.h"
    
#define DISPHELPER_INTERNAL_BUILD
#include <win_disphelper.h>
#include <math.h>
#include <assert.h>
    
/* ----- convert.h ----- */ 
    










/* ----- dh_create.c ----- */ 
    
                              void **ppv) 
{
    
    
    
    
    
        return DH_EXIT(E_INVALIDARG, szProgId);
    
        
    
    else
        
    
        hr = CoGetClassObject(&clsid, dwClsContext, pServerInfo, &IID_IClassFactory, (void **) &pCf);
    
        hr = pCf->lpVtbl->CreateInstance(pCf, NULL, riid, ppv);
    
        pCf->lpVtbl->Release(pCf);
    



                         void **ppv) 
{
    
    
    
        return DH_EXIT(E_INVALIDARG, szProgId);
    
        
    {
        
            
        {
            
        
        
        else
            
        {
            
            
            
                hr = ppf->lpVtbl->Load(ppf, szPathName, 0);
            
                hr = ppf->lpVtbl->QueryInterface(ppf, riid, ppv);
            
                ppf->lpVtbl->Release(ppf);
        
    
    
    else
        
    {
        
        
        
            
        
        else
            
        
            hr = GetActiveObject(&clsid, NULL, &pUnk);
        
            hr = pUnk->lpVtbl->QueryInterface(pUnk, riid, ppv);
        
            pUnk->lpVtbl->Release(pUnk);
    
    



{
    
    
    
    
        DH_EXIT(dhCreateObjectEx
                (szProgId, &IID_IDispatch,
                 
                 

{
    
    
        DH_EXIT(dhGetObjectEx
                (szPathName, szProgId, &IID_IDispatch, 
                 (void **) ppDisp), szProgId);

{
    
    
    
    
    
    
    



{
    
    
    
    
    
    
    



{
    
    
    
    
    
    
    



{
    
    
    
    
    
    
    



{
    
    
    
    
    
    
    



/* ----- dh_core.c ----- */ 
    

                         VARIANT * pArgs) 
{
    
    
    
    
    
    
    
    
        return DH_EXIT(E_INVALIDARG, szMember);
    
    
        return DH_EXITEX(hr, TRUE, szMember, szMember, NULL, 0);
    
        VariantInit(pvResult);
    
    
    
        
    {
        
        
    
    
        pDisp->lpVtbl->Invoke(pDisp, dispID, &IID_NULL, LOCALE_USER_DEFAULT, (WORD) invokeType, &dp, pvResult, &excep,
                              &uiArgErr);
    



{
    
    



{
    
    



{
    
    



{
    
    
    
    
    
        return DH_EXIT(E_INVALIDARG, szMember);
    
        szIdentifier++;
    
        
    {
    
        returnType = VT_I4;
        break;
    
        returnType = VT_UI4;
        break;
    
        returnType = VT_R8;
        break;
    
        returnType = VT_BOOL;
        break;
    
        returnType = VT_EMPTY;
        break;
    
        returnType = VT_BSTR;
        break;
    
        returnType = VT_BSTR;
        break;
    
        returnType = VT_BSTR;
        break;
    
        returnType = VT_BSTR;
        break;
    
        returnType = VT_DISPATCH;
        break;
    
        returnType = VT_UNKNOWN;
        break;
    
        returnType = VT_DATE;
        break;
    
        returnType = VT_DATE;
        break;
    
        returnType = VT_DATE;
        break;
    
        returnType = VT_DATE;
        break;
        
#ifndef _WIN64
    case L'p':
        returnType = VT_I4;
        break;
        
#else
    case L'p':
        returnType = VT_I8;
        break;
        
#endif
    default:
        
        
    
    
    
        return DH_EXIT(hr, szMember);
    
        
    {
    
        
        
    
        
        
    
        
        
    
        
        
    
        
        
    
        
        
    
        
        
    
        
        
        
    
        
            
        {
            
        
        
        else
            
        {
            
            
        
        
    
        
        
            hr = E_NOINTERFACE;
        
    
        
        
            hr = E_NOINTERFACE;
        
    
        
        
    
        
        
    
        
        
    
        
        
    
        
#ifndef _WIN64
            *((LPVOID *) pResult) = (LPVOID) V_I4(&vtResult);
        
#else
            *((LPVOID *) pResult) = (LPVOID) V_I8(&vtResult);
        
#endif
            break;
    
    



/* ----- dh_invoke.c ----- */ 



                                LPOLESTR szMember, va_list * marker);


                     va_list * marker) 
{
    
    
    
    
    
    
        return DH_EXIT(E_INVALIDARG, szMember);
    
    do
        
    {
        
            return DH_EXIT(E_INVALIDARG, szMember);
    
    
    
    
    
        
    {
        
        
    
    



{
    
    
    
    
    
        (*lpszMember)++;
    
    
    
        return DH_EXIT(NOERROR, *lpszMember);
    
    
    do
        
    {
        
        
            InternalInvokeV(DISPATCH_METHOD | DISPATCH_PROPERTYGET, VT_DISPATCH, 
        
            hr = E_NOINTERFACE;
        
        
            break;
        
        
    
    
    
    



                                  LPOLESTR szMember, va_list * marker) 
{
    
    
    
    
    
    
    
        
    {
        
        
            
        {
            
                VariantClear(&vtArgs[iArg]);
        
        
            
        {
            
            
                VariantClear(pvResult);
        
    
    



                                      UINT * pcArgs, va_list * marker) 
{
    
    
    
    
    
        
    {
        
            
        {
            
            
        
        
        else if (*szMember == L'%')
            
        {
            
                
            {
                
                
            
            
            
            {
                hr = E_INVALIDARG;
                break;
            }
            
            
            
                break;
        
        
    
    
    
        
    {
        
            
        {
            
                VariantClear(&pArgs[iArg]);
        
    
    



{
    
    
    
        chIdentifier = (dh_g_bIsUnicodeMode ? L'S' : L's');
    
        
    {
    
        
        
        
    
        
        
        
    
        
        
        
    
        
        
        
    
        
        
    
        
        
        
    
        
        
        
    
        
    {
        
        
        
        
            hr = E_OUTOFMEMORY;
        
        
    
    
        
        
        
        
    
        
        
        
    
        
        
        
    
        
        
        
    
        
        

        
    
        
        
        
    
        
        

        
    
        
#ifndef _WIN64
            V_VT(pvArg) = VT_I4;
        
        
#else
            V_VT(pvArg) = VT_I8;
        
        
#endif
            break;
    
        
        
        
    
    



/* ----- dh_enum.c ----- */ 
    
{
    
    
    
    
    
    
    
        return DH_EXIT(E_INVALIDARG, szMember);
    
        
    {
        
        
            return DH_EXIT(hr, szMember);
    
    
    else
        
    {
        
    
    
        pDispObj->lpVtbl->Invoke(pDispObj, DISPID_NEWENUM, &IID_NULL, LOCALE_USER_DEFAULT,
                                 
    
        pDispObj->lpVtbl->Release(pDispObj);
    
        return DH_EXITEX(hr, TRUE, L"_NewEnum", szMember, &excep, 0);
    
        
    
    else if (vtResult.vt == VT_UNKNOWN)
        
    
    else
        
    
    



{
    
    
        return DH_EXIT(E_INVALIDARG, L"Enumerator");
    



{
    
    
    
    
        return DH_EXIT(E_INVALIDARG, L"Enumerator");
    
    
        
    {
        
            
        {
            
        
        
        else
            
        {
            
            
                *ppDisp = vtResult.pdispVal;
            
            else
                VariantClear(&vtResult);
        
    
    



{
    
    
    
    
    
    
    



/* ----- convert.c ----- */ 








#ifndef _WIN64
static const DATE VARIANT_TIMET_MAX = 50424.13480;


#else
static const time_t TIMET_VARIANT_OVERFLOW = 253402300800;


#endif
    
{
    
    
        return E_INVALIDARG;
    
    
        return E_INVALIDARG;
    
    
        *pDate = floor(*pDate) + (floor(*pDate) - *pDate);
    



{
    
    
        return E_INVALIDARG;
    
        date = ceil(date) + (ceil(date) - date);
    
        return E_INVALIDARG;
    
    
    



{
    
    

    
        return E_INVALIDARG;
    
        return hr;
    



{
    

    
        return E_INVALIDARG;
    
        return HRESULT_FROM_WIN32(GetLastError());
    



{
    

    
        return E_INVALIDARG;
    
#ifndef _WIN64
        if (date < VARIANT_TIMET_DAY0 || date > VARIANT_TIMET_MAX)
        return E_INVALIDARG;
    
#else
        if (date < VARIANT_TIMET_DAY0)
        return E_INVALIDARG;
    
#endif
        
    
        return E_FAIL;
    



{
    
    

    
        return E_INVALIDARG;
    
        return E_FAIL;
    
    
        return E_FAIL;
    
    
    
    
        return E_FAIL;
    
    
#ifdef _WIN64
        if (timeT >= TIMET_VARIANT_OVERFLOW)
        return E_INVALIDARG;
    
#endif
        *pDate = (DATE) (timeT / (double) TIMET_ONE_DAY) + VARIANT_TIMET_DAY0;
    



{
    

    
        return E_INVALIDARG;
    
    {
        *lpBstrOut = NULL;
        return NOERROR;
    }
    
    
        return HRESULT_FROM_WIN32(GetLastError());
    
    
        return E_OUTOFMEMORY;
    
        
    {
        
        
    
    



{
    

    
        return E_INVALIDARG;
    
    {
        *lpszOut = NULL;
        return NOERROR;
    }
    
    
        return HRESULT_FROM_WIN32(GetLastError());
    
    
        return E_OUTOFMEMORY;
    
        
    {
        
        
    
    



/* ----- dh_exceptions.c ----- */ 
    
#ifndef DISPHELPER_NO_EXCEPTIONS





# define SetStackCount(nStackCount)   TlsSetValue(f_TlsIdxStackCount, (LPVOID) (nStackCount))
# define SetExceptionPtr(pException)  TlsSetValue(f_TlsIdxException, pException);
# define GetStackCount()       (UINT) TlsGetValue(f_TlsIdxStackCount)
# define GetExceptionPtr()            TlsGetValue(f_TlsIdxException)
# define CheckTlsInitialized()        if (f_lngTlsInitEnd != 0) InitializeTlsIndexes();

{
    
    
    do
        
    {
        
            break;
    
    
    



{
    
        
    {
        
        
        
    
    
    else
        
    {
        
            Sleep(5);
    



{
    
    

                       
{
    
    
    
        
    {
        

        
            
        {
            
            
                return hr;
            
        
        
        else if (pException->bOld)
            
        {
            
            
            
            
        
        
            
        {
            
            
            
            
            
                hlprStringCchCopyW(pException->szMember, ARRAYSIZE(pException->szMember), szMember);
            
                
            {
                
                    pExcepInfo->pfnDeferredFillIn(pExcepInfo);
                
                
                
                
                
            
        
        
            
        {
            
            
            
                hlprStringCchCopyW(pException->szCompleteMember, ARRAYSIZE(pException->szCompleteMember),
                                   szCompleteMember);
            
                
            
                
        
    
    
    else if (hr == DISP_E_EXCEPTION && pExcepInfo)
        
    {
        
        
        
    
    



{
    
    
    
    



{
    
    
    
# 	define DESCRIPTION_LENGTH 255
        
        return E_INVALIDARG;
    
        
    {
        
        
            
        {
            
                
            {
                
                
            
            
        
    
    
    
        
    {
        
            
        
        else
            
    
    
        
    {
        
        
            
        {
            
                
            {
            
                
                
            
            
                
                            L"Object doesn't support this property or method: '%s'", pException->szMember);
                
            
                
                    
                {
                    
                                L"Type mismatch: '%s'. Argument Index: %d", pException->szMember,
                                pException->iArgError);
                    
                
            
                
            {
                
# ifndef UNICODE
                    CHAR szDescription[DESCRIPTION_LENGTH];
                
# else
                    LPWSTR szDescription = pException->szDescription;
                
# endif
                    cch =
                    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                
                    wcscpy(pException->szDescription, L"Unknown runtime error");
                
# ifndef UNICODE
                    else
                    MultiByteToWideChar(CP_ACP, 0, szDescription, -1, pException->szDescription, DESCRIPTION_LENGTH);
                
# endif
            }
            
        
    
    
        
    {
        
            cch = wcslen(pException->szDescription);
        
            
        
        else if (cch >= 1 && pException->szDescription[cch - 1] == L'\n')
            
    
    
        
    {
        
            
        {
            
                        L"Member:\t  %s\r\nFunction:\t  %s\t\t\r\nError In:\t  %s\r\nError:\t  %s\r\nCode:\t  %x\r\nSource:\t  %s",
                        
                        
        
        
        else
            
        {
            
                        L"Member:   %s\r\nFunction: %s\r\nError In: %s\r\nError:    %s\r\nCode:     %x\r\nSource:   %s",
                        
                        
        
        
    
    



{
    
    
    
        
    



{
    
        return E_INVALIDARG;
    
    
    



{
    
    
        g_ExceptionOptions.bDisableRecordExceptions = FALSE;
    



{
    
        return E_INVALIDARG;
    
    
    
    
    
    



{
    
        return E_INVALIDARG;
    
    
    
    
    
    



{
    

    
    
    
        
    {
        
        
        
        
        
    



#endif
    
/* ----- dh_init.c ----- */ 
    
{
    
    
        return CoInitialize(NULL);
    



{
    
#ifndef DISPHELPER_NO_EXCEPTIONS
        dhCleanupThreadException();
    
#endif
        if (bUninitializeCOM)
        CoUninitialize();



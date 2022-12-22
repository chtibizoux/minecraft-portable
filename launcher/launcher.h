#pragma once

#include "resource.h"

#include <regex>
#include <CommCtrl.h>
#include <shellapi.h>
#ifdef CONSOLE
#include <uxtheme.h>
#endif
#ifdef JNI
#include <jni.h>
#endif
#ifdef EMBED
#include <prims/jni.cpp>
#endif
#ifdef DOWNLOAD
#include <shlobj.h>

class DownloadStatus : public IBindStatusCallback
{
public:
    STDMETHOD(OnStartBinding)(/* [in] */ DWORD dwReserved,/* [in] */ IBinding __RPC_FAR* pib)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(GetPriority)(/* [out] */ LONG __RPC_FAR* pnPriority)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(OnLowResource)(/* [in] */ DWORD reserved)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(OnProgress)(/* [in] */ ULONG ulProgress,/* [in] */ ULONG ulProgressMax,/* [in] */ ULONG ulStatusCode,/* [in] */ LPCWSTR wszStatusText);

    STDMETHOD(OnStopBinding)(/* [in] */ HRESULT hresult,/* [unique][in] */ LPCWSTR szError)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(GetBindInfo)(/* [out] */ DWORD __RPC_FAR* grfBINDF,/* [unique][out][in] */ BINDINFO __RPC_FAR* pbindinfo)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(OnDataAvailable)(/* [in] */ DWORD grfBSCF,/* [in] */ DWORD dwSize,/* [in] */ FORMATETC __RPC_FAR* pformatetc,/* [in] */ STGMEDIUM __RPC_FAR* pstgmed)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(OnObjectAvailable)(/* [in] */ REFIID riid,/* [iid_is][in] */ IUnknown __RPC_FAR* punk)
    {
        return E_NOTIMPL;
    }

    // IUnknown methods.  Note that IE never calls any of these methods, since
    // the caller owns the IBindStatusCallback interface, so the methods all
    // return zero/E_NOTIMPL.

    STDMETHOD_(ULONG, AddRef)()
    {
        return 0;
    }

    STDMETHOD_(ULONG, Release)()
    {
        return 0;
    }

    STDMETHOD(QueryInterface)(/* [in] */ REFIID riid,/* [iid_is][out] */ void __RPC_FAR* __RPC_FAR* ppvObject)
    {
        return E_NOTIMPL;
    }
};

//bool Unzip2Folder(BSTR lpZipFile, BSTR lpFolder)
//{
//	IShellDispatch* pISD;
//
//	Folder* pZippedFile = 0L;
//	Folder* pDestination = 0L;
//
//	long FilesCount = 0;
//	IDispatch* pItem = 0L;
//	FolderItems* pFilesInside = 0L;
//
//	VARIANT Options, OutFolder, InZipFile, Item;
//	CoInitialize(NULL);
//	__try {
//		if (CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void**)&pISD) != S_OK)
//			return 1;
//
//		InZipFile.vt = VT_BSTR;
//		InZipFile.bstrVal = lpZipFile;
//		pISD->NameSpace(InZipFile, &pZippedFile);
//		if (!pZippedFile)
//		{
//			pISD->Release();
//			return 1;
//		}
//
//		OutFolder.vt = VT_BSTR;
//		OutFolder.bstrVal = lpFolder;
//		pISD->NameSpace(OutFolder, &pDestination);
//		if (!pDestination)
//		{
//			pZippedFile->Release();
//			pISD->Release();
//			return 1;
//		}
//
//		pZippedFile->Items(&pFilesInside);
//		if (!pFilesInside)
//		{
//			pDestination->Release();
//			pZippedFile->Release();
//			pISD->Release();
//			return 1;
//		}
//
//		pFilesInside->get_Count(&FilesCount);
//		if (FilesCount < 1)
//		{
//			pFilesInside->Release();
//			pDestination->Release();
//			pZippedFile->Release();
//			pISD->Release();
//			return 0;
//		}
//
//		pFilesInside->QueryInterface(IID_IDispatch, (void**)&pItem);
//
//		Item.vt = VT_DISPATCH;
//		Item.pdispVal = pItem;
//
//		Options.vt = VT_I4;
//		Options.lVal = 1024 | 512 | 16 | 4;
//
//		bool retval = pDestination->CopyHere(Item, Options) != S_OK;
//
//		pItem->Release(); pItem = 0L;
//		pFilesInside->Release(); pFilesInside = 0L;
//		pDestination->Release(); pDestination = 0L;
//		pZippedFile->Release(); pZippedFile = 0L;
//		pISD->Release(); pISD = 0L;
//
//		return retval;
//
//	}
//	__finally
//	{
//		CoUninitialize();
//	}
//}
void UnZipFile(BSTR source, BSTR dest)
{
	HRESULT hResult = S_FALSE;
	IShellDispatch* pIShellDispatch = NULL;
	Folder* pToFolder = NULL;
	VARIANT variantDir, variantFile, variantOpt;

	CoInitialize(NULL);

	hResult = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellDispatch, (void**)&pIShellDispatch);
	if (SUCCEEDED(hResult))
	{
		VariantInit(&variantDir);
		variantDir.vt = VT_BSTR;
		variantDir.bstrVal = dest;
		hResult = pIShellDispatch->NameSpace(variantDir, &pToFolder);

		if (SUCCEEDED(hResult))
		{
			Folder* pFromFolder = NULL;
			VariantInit(&variantFile);
			variantFile.vt = VT_BSTR;
			variantFile.bstrVal = source;
			pIShellDispatch->NameSpace(variantFile, &pFromFolder);

			FolderItems* fi = NULL;
			pFromFolder->Items(&fi);

			VariantInit(&variantOpt);
			variantOpt.vt = VT_I4;
			variantOpt.lVal = FOF_NO_UI;

			VARIANT newV;
			VariantInit(&newV);
			newV.vt = VT_DISPATCH;
			newV.pdispVal = fi;
			hResult = pToFolder->CopyHere(newV, variantOpt);
			Sleep(1000);
			pFromFolder->Release();
			pToFolder->Release();
		}
		pIShellDispatch->Release();
	}

	CoUninitialize();
}
#endif
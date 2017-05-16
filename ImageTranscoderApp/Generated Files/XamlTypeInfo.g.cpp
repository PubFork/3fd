﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------
#include "pch.h"
#include "XamlTypeInfo.g.h"

#include "App.xaml.h"
#include "MainPage.xaml.h"
#include "XamlBindingInfo.g.hpp"
#include "App.g.hpp"
#include "MainPage.g.hpp"

template<typename T>
::Platform::Object^ ActivateType()
{
    return ref new T;
}

template<typename TInstance, typename TItem>
void CollectionAdd(::Platform::Object^ instance, ::Platform::Object^ item)
{
    safe_cast<TInstance^>(instance)->Append((TItem)item);
}

template<typename TInstance, typename TKey, typename TItem>
void DictionaryAdd(::Platform::Object^ instance, ::Platform::Object^ key, ::Platform::Object^ item)
{
    safe_cast<TInstance^>(instance)->Insert((TKey)key, (TItem)item);
}

template<typename T>
::Platform::Object^ FromStringConverter(::XamlTypeInfo::InfoProvider::XamlUserType^ userType, ::Platform::String^ input)
{
    return ref new ::Platform::Box<T>((T)userType->CreateEnumUIntFromString(input));
}

template<typename TDeclaringType>
::Platform::Object^ GetReferenceTypeMember_InputImages(::Platform::Object^ instance)
{
    return safe_cast<TDeclaringType^>(instance)->InputImages;
}

template<typename TDeclaringType>
::Platform::Object^ GetReferenceTypeMember_Thumbnail(::Platform::Object^ instance)
{
    return safe_cast<TDeclaringType^>(instance)->Thumbnail;
}

template<typename TDeclaringType>
::Platform::Object^ GetReferenceTypeMember_Description(::Platform::Object^ instance)
{
    return safe_cast<TDeclaringType^>(instance)->Description;
}

template<typename TDeclaringType>
::Platform::Object^ GetReferenceTypeMember_File(::Platform::Object^ instance)
{
    return safe_cast<TDeclaringType^>(instance)->File;
}

template<typename TDeclaringType, typename TValue>
void SetReferenceTypeMember_Thumbnail(::Platform::Object^ instance, ::Platform::Object^ value)
{
    safe_cast<TDeclaringType^>(instance)->Thumbnail = safe_cast<TValue^>(value);
}

template<typename TDeclaringType, typename TValue>
void SetReferenceTypeMember_File(::Platform::Object^ instance, ::Platform::Object^ value)
{
    safe_cast<TDeclaringType^>(instance)->File = safe_cast<TValue^>(value);
}

struct TypeInfo
{
    PCWSTR  typeName;
    PCWSTR contentPropertyName;
    ::Platform::Object^ (*activator)();
    void (*collectionAdd)(::Platform::Object^, ::Platform::Object^);
    void (*dictionaryAdd)(::Platform::Object^, ::Platform::Object^, ::Platform::Object^);
    ::Platform::Object^ (*fromStringConverter)(::XamlTypeInfo::InfoProvider::XamlUserType^, ::Platform::String^);
    int     baseTypeIndex;
    int     firstMemberIndex;
    int     firstEnumValueIndex;
    ::Windows::UI::Xaml::Interop::TypeKind kindofType;
    bool    isLocalType;
    bool    isSystemType;
    bool    isReturnTypeStub;
    bool    isBindable;
};

TypeInfo TypeInfos[] = 
{
    //   0
    L"Object", L"",
    nullptr, nullptr, nullptr, nullptr,
    -1,
    0, 0, ::Windows::UI::Xaml::Interop::TypeKind::Metadata,
    false, true,  false, false,
    //   1
    L"String", L"",
    nullptr, nullptr, nullptr, nullptr,
    -1,
    0, 0, ::Windows::UI::Xaml::Interop::TypeKind::Metadata,
    false, true,  false, false,
    //   2
    L"ImageTranscoderApp.MainPage", L"",
    &ActivateType<::ImageTranscoderApp::MainPage>, nullptr, nullptr, nullptr,
    4, // Windows.UI.Xaml.Controls.Page
    0, 0, ::Windows::UI::Xaml::Interop::TypeKind::Custom,
    true,  false, false, false,
    //   3
    L"Windows.Storage.StorageFile", L"",
    nullptr, nullptr, nullptr, nullptr,
    0, // Object
    1, 0, ::Windows::UI::Xaml::Interop::TypeKind::Metadata,
    false, false, true,  false,
    //   4
    L"Windows.UI.Xaml.Controls.Page", L"",
    nullptr, nullptr, nullptr, nullptr,
    -1,
    1, 0, ::Windows::UI::Xaml::Interop::TypeKind::Metadata,
    false, true,  false, false,
    //   5
    L"ImageTranscoderApp.FileListItem", L"",
    nullptr, nullptr, nullptr, nullptr,
    0, // Object
    1, 0, ::Windows::UI::Xaml::Interop::TypeKind::Custom,
    true,  false, false, false,
    //   6
    L"Windows.UI.Xaml.Controls.UserControl", L"",
    nullptr, nullptr, nullptr, nullptr,
    -1,
    4, 0, ::Windows::UI::Xaml::Interop::TypeKind::Metadata,
    false, true,  false, false,
    //   7
    L"Windows.UI.Xaml.Media.Imaging.BitmapImage", L"",
    nullptr, nullptr, nullptr, nullptr,
    -1,
    4, 0, ::Windows::UI::Xaml::Interop::TypeKind::Metadata,
    false, true,  false, false,
    //   8
    L"Windows.Foundation.Collections.IObservableVector`1<ImageTranscoderApp.FileListItem>", L"",
    nullptr, &CollectionAdd<::Windows::Foundation::Collections::IObservableVector<::ImageTranscoderApp::FileListItem^>, ::ImageTranscoderApp::FileListItem^>, nullptr, nullptr,
    -1,
    4, 0, ::Windows::UI::Xaml::Interop::TypeKind::Metadata,
    false, false, true,  false,
    //  Last type here is for padding
    L"", L"",
    nullptr, nullptr, nullptr, nullptr,
    -1, 
    4, 0,::Windows::UI::Xaml::Interop::TypeKind::Custom,
    false, false, false, false,
};

UINT TypeInfoLookup[] = { 
      0,   //   0
      0,   //   1
      0,   //   2
      0,   //   3
      0,   //   4
      0,   //   5
      0,   //   6
      2,   //   7
      2,   //   8
      2,   //   9
      2,   //  10
      2,   //  11
      2,   //  12
      2,   //  13
      2,   //  14
      2,   //  15
      2,   //  16
      2,   //  17
      2,   //  18
      2,   //  19
      2,   //  20
      2,   //  21
      2,   //  22
      2,   //  23
      2,   //  24
      2,   //  25
      2,   //  26
      2,   //  27
      4,   //  28
      4,   //  29
      5,   //  30
      5,   //  31
      6,   //  32
      6,   //  33
      6,   //  34
      6,   //  35
      6,   //  36
      7,   //  37
      7,   //  38
      7,   //  39
      7,   //  40
      7,   //  41
      8,   //  42
      8,   //  43
      8,   //  44
      8,   //  45
      8,   //  46
      8,   //  47
      8,   //  48
      8,   //  49
      8,   //  50
      8,   //  51
      8,   //  52
      8,   //  53
      8,   //  54
      8,   //  55
      8,   //  56
      8,   //  57
      8,   //  58
      8,   //  59
      8,   //  60
      8,   //  61
      8,   //  62
      8,   //  63
      8,   //  64
      8,   //  65
      8,   //  66
      8,   //  67
      8,   //  68
      8,   //  69
      8,   //  70
      8,   //  71
      8,   //  72
      8,   //  73
      8,   //  74
      8,   //  75
      8,   //  76
      8,   //  77
      8,   //  78
      8,   //  79
      8,   //  80
      8,   //  81
      8,   //  82
      8,   //  83
      9,   //  84
};

struct MemberInfo 
{
    PCWSTR shortName;
    ::Platform::Object^ (*getter)(::Platform::Object^);
    void (*setter)(::Platform::Object^, ::Platform::Object^);
    int typeIndex;
    int targetTypeIndex;
    bool isReadOnly;
    bool isDependencyProperty;
    bool isAttachable;
};

MemberInfo MemberInfos[] = 
{
    //   0 - ImageTranscoderApp.MainPage.InputImages
    L"InputImages",
    &GetReferenceTypeMember_InputImages<::ImageTranscoderApp::MainPage>,
    nullptr,
    8, // Windows.Foundation.Collections.IObservableVector`1<ImageTranscoderApp.FileListItem>
    -1,
    true,  false, false,
    //   1 - ImageTranscoderApp.FileListItem.Thumbnail
    L"Thumbnail",
    &GetReferenceTypeMember_Thumbnail<::ImageTranscoderApp::FileListItem>,
    &SetReferenceTypeMember_Thumbnail<::ImageTranscoderApp::FileListItem, ::Windows::UI::Xaml::Media::Imaging::BitmapImage>,
    7, // Windows.UI.Xaml.Media.Imaging.BitmapImage
    -1,
    false, false, false,
    //   2 - ImageTranscoderApp.FileListItem.Description
    L"Description",
    &GetReferenceTypeMember_Description<::ImageTranscoderApp::FileListItem>,
    nullptr,
    1, // String
    -1,
    true,  false, false,
    //   3 - ImageTranscoderApp.FileListItem.File
    L"File",
    &GetReferenceTypeMember_File<::ImageTranscoderApp::FileListItem>,
    &SetReferenceTypeMember_File<::ImageTranscoderApp::FileListItem, ::Windows::Storage::StorageFile>,
    3, // Windows.Storage.StorageFile
    -1,
    false, false, false,
};

PCWSTR GetShortName(PCWSTR longName)
{
    PCWSTR separator = wcsrchr(longName, '.');
    return separator != nullptr ? separator + 1: longName;
}

TypeInfo* GetTypeInfo(::Platform::String^ typeName)
{
    int typeNameLength = typeName->Length();
    if (typeNameLength < _countof(TypeInfoLookup) - 1)
    {
        for (UINT i = TypeInfoLookup[typeNameLength]; i < TypeInfoLookup[typeNameLength+1]; i++)
        {
            if (typeName == ::Platform::StringReference(TypeInfos[i].typeName))
            {
                return &TypeInfos[i];
            }
        }
    }
    return nullptr;
}

MemberInfo* GetMemberInfo(::Platform::String^ longMemberName)
{
    for (int lastDotIndex = longMemberName->Length(); lastDotIndex >= 0; lastDotIndex--)
    {
        if (longMemberName->Data()[lastDotIndex] == '.')
        {
            TypeInfo* pTypeInfo = GetTypeInfo(ref new ::Platform::String(longMemberName->Data(), lastDotIndex));
            TypeInfo* pNextTypeInfo = pTypeInfo + 1;
            if (pTypeInfo)
            {
                PCWSTR shortMemberName = GetShortName(longMemberName->Data());
                for (int i = pTypeInfo->firstMemberIndex; i < pNextTypeInfo->firstMemberIndex; i++)
                {
                    if (wcscmp(shortMemberName, MemberInfos[i].shortName) == 0)
                    {
                        return &MemberInfos[i];
                    }
                }
            }
            break;
        }
    }
    return nullptr;
}

::Platform::Collections::Vector<::Windows::UI::Xaml::Markup::IXamlMetadataProvider^>^ ::XamlTypeInfo::InfoProvider::XamlTypeInfoProvider::OtherProviders::get()
{
    if(_otherProviders == nullptr)
    {
        auto otherProviders = ref new ::Platform::Collections::Vector<::Windows::UI::Xaml::Markup::IXamlMetadataProvider^>();
        _otherProviders = otherProviders;
    }
    return _otherProviders;
}

::Windows::UI::Xaml::Markup::IXamlType^ ::XamlTypeInfo::InfoProvider::XamlTypeInfoProvider::CreateXamlType(::Platform::String^ typeName)
{
    TypeInfo* pTypeInfo = GetTypeInfo(typeName);
    TypeInfo* pNextTypeInfo = pTypeInfo + 1;
    if (pTypeInfo == nullptr || pNextTypeInfo == nullptr)
    {
        return nullptr;
    }
    else if (pTypeInfo->isSystemType)
    {
        return ref new ::XamlTypeInfo::InfoProvider::XamlSystemBaseType(typeName);
    }
    else
    {
        ::XamlTypeInfo::InfoProvider::XamlUserType^ userType = ref new ::XamlTypeInfo::InfoProvider::XamlUserType(
            this, 
            ::Platform::StringReference(pTypeInfo->typeName), 
            this->GetXamlTypeByName(::Platform::StringReference(pTypeInfo->baseTypeIndex >= 0 ? TypeInfos[pTypeInfo->baseTypeIndex].typeName : L"")));
        userType->KindOfType = pTypeInfo->kindofType;
        userType->Activator = pTypeInfo->activator;
        userType->CollectionAdd = pTypeInfo->collectionAdd;
        userType->DictionaryAdd = pTypeInfo->dictionaryAdd;
        userType->FromStringConverter = pTypeInfo->fromStringConverter;
        userType->ContentPropertyName = ::Platform::StringReference(pTypeInfo->contentPropertyName);
        userType->IsLocalType = pTypeInfo->isLocalType;
        userType->IsReturnTypeStub = pTypeInfo->isReturnTypeStub;
        userType->IsBindable = pTypeInfo->isBindable;
        int nextMemberIndex = pTypeInfo->firstMemberIndex;
        for (int i=pTypeInfo->firstMemberIndex; i < pNextTypeInfo->firstMemberIndex; i++)
        {
            userType->AddMemberName(::Platform::StringReference(MemberInfos[i].shortName));
            nextMemberIndex++;
        }
        return userType;
    }
}

::Windows::UI::Xaml::Markup::IXamlMember^ ::XamlTypeInfo::InfoProvider::XamlTypeInfoProvider::CreateXamlMember(::Platform::String^ longMemberName)
{
    ::XamlTypeInfo::InfoProvider::XamlMember^ xamlMember = nullptr;
    MemberInfo* pMemberInfo = GetMemberInfo(longMemberName);
    if (pMemberInfo != nullptr)
    {
        xamlMember = ref new ::XamlTypeInfo::InfoProvider::XamlMember(
            this,
            ::Platform::StringReference(pMemberInfo->shortName),
            ::Platform::StringReference(TypeInfos[pMemberInfo->typeIndex].typeName));
        xamlMember->Getter = pMemberInfo->getter;
        xamlMember->Setter = pMemberInfo->setter;
        xamlMember->TargetTypeName = pMemberInfo->targetTypeIndex >= 0 ? ::Platform::StringReference(TypeInfos[pMemberInfo->targetTypeIndex].typeName) : L"";
        xamlMember->IsReadOnly = pMemberInfo->isReadOnly;
        xamlMember->IsDependencyProperty = pMemberInfo->isDependencyProperty;
        xamlMember->IsAttachable = pMemberInfo->isAttachable;
    }
    return xamlMember;
}


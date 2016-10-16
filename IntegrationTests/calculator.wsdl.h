﻿
// File generated by Wsutil Compiler version 1.0098 
// This file defines C/C++ functions, callbacks, types that correspond to operations,
// and types specified in WSDL and XSD files processed by Wsutil compiler. The generated 
// type definitions, function and callback declarations can be used with various 
// Web Services APIs either in applications that send and receive requests to and 
// from a running web service, or in the implementation of web services, or in both. 
//
// If Wsutil has generated this file from a WSDL file, the file may contain up to three 
// sets of C/C++ definitions. The first set consists of function declarations that can be 
// used in calling operations on a running web service from client applications. 
// The second set consists of callback declarations that can be used in the 
// implementation of web services.  
// The third set contains definitions C/C++ structures that are used for serialization of 
// C/C++ types to Schema types defined in the WSDL file.
//
// The set of function declarations for web services clients may contain the 
// following declaration: 
//
//    HRESULT WINAPI DefaultBinding_ICalculator_Add(
//       _In_ WS_SERVICE_PROXY* _serviceProxy,
//       _In_ int a, 
//       _In_ int b, 
//       _Out_ int* result, 
//       _In_ WS_HEAP* _heap,
//       _In_opt_ WS_CALL_PROPERTY* _callProperties,
//       _In_ const ULONG _callPropertyCount,
//       _In_opt_ const WS_ASYNC_CONTEXT*_asyncContext,
//       _In_opt_ WS_ERROR* _error);
//
// This function can be called from an application code to request ICalculator web 
// service to perform operation Add using two integers a and b as input data.
// Once the web service processes this request and sends the result back,
// the result is returned to the caller of this function in the "result" parameter.
// A WS_SERVICE_PROXY must be created using WsCreateServiceProxy() function
// before this proxy function can be invoked.
//
// The set of callbacks for implementation of web services may contain the following 
// definition of a callback: 
//
//    typedef HRESULT (CALLBACK* DefaultBinding_ICalculator_AddCallback) (
//       _In_ const WS_OPERATION_CONTEXT* _context,
//       _In_ int a, 
//       _In_ int b, 
//       _Out_ int* result, 
//       _In_ const WS_ASYNC_CONTEXT* _asyncContext,
//       _In_ WS_ERROR* _error); 
//
//    typedef struct DefaultBinding_ICalculatorFunctionTable 
//    {
//         DefaultBinding_ICalculator_AddCallback DefaultBinding_ICalculator_Add;
//    } DefaultBinding_ICalculatorFunctionTable;
//
// This definition can be used to implement a function that defines the Add operation 
// as a part of an ICalculator web service. Once this callback is implemented and 
// the web service is deployed and running, if a client applications attempts to 
// execute the operation Add, the implementation of this callback is called and 
// it receives two integers a and b as input data and it returns a third integer 
// that is the result of the operation. A WS_SERVICE_ENDPOINT must be filled out, 
// and its contract field set to instance of such a function table.  
// WsCreateServiceHost may then be used to register the endpoint as a service.
// 
// The set with definitions of C/C++ structures may contain a definition of the following 
// structure:
//    typedef struct ContactInfo 
//    {
//			WCHAR*  name;
//			WCHAR*  email;
//			__int64 contactID;
//	  } ContactInfo;
//
// This structure corresponds to the following complex type defined in the WSDL file:
//    <s:complexType name="ContactInfo">
//      <s:sequence>
//        <s:element minOccurs="1" maxOccurs="1" name="name" type="s:string" />
//        <s:element minOccurs="1" maxOccurs="1" name="email" type="s:string" />
//        <s:element minOccurs="1" maxOccurs="1" name="contactID" type="s:long" />
//      </s:sequence>
//    </s:complexType>
//
// For more information about content of this file, please see the documentation for
// Wsutil compiler.
// If Wsutil has generated this file from an XSD file, the file contains definition of 
// C/C++ structures types that correspond to types defined in the XSD file. 
// For example, if the following XSD complexType is defined in the XSD file
//
//      <xsd:complexType name="AddRequest">
//          <xsd:sequence>
//              <xsd:element minOccurs="0" name="a" type="xsd:int" />
//              <xsd:element minOccurs="0" name="b" type="xsd:int" />
//          </xsd:sequence>
//      </xsd:complexType>
// 
// this file contains the following definitions of the structure 
//      // AddRequest (xsd:complexType)
//      // <AddRequest xmlns='http://tempuri.org'>
//      // WS_STRUCT_DESCRIPTION* = &calculator_xsd.globalTypes.AddRequest
//      struct AddRequest
//      {
//          int a;
//          int b;
//      };
//
// For more information on how to use the C/C++ types generated in this file to read or write elements of XML documents that conform to 
// this XSD, please see the documentation for 
// WsReadType() and WsWriteType() functions.
// 
// If Wsutil generates this file from a WSDL file and policy processing is enabled using
// policy switch, the file may contain declaration of functions that can be used 
// for creating a proxy to a web service or an endpoint of a web service. 
// For example, the file may contain declaration of the following function:
//
//    HRESULT DefaultBinding_ICalculator_CreateServiceProxy(
//    _In_ WS_HTTP_SSL_BINDING_TEMPLATE* templateValue,
//    _In_reads_opt_(proxyPropertyCount) const WS_PROXY_PROPERTY* proxyProperties,
//    _In_ const ULONG proxyPropertyCount,
//    _Outptr_ WS_SERVICE_PROXY** proxy,
//    _In_opt_ WS_ERROR* error);
//
// This function can be called from application code to create a service
// proxy object. A service proxy must be created before any request can be sent 
// to the web service.
//
// Also, the file may contain declaration of the following function:
//
//    HRESULT DefaultBinding_ICalculator_CreateServiceEndpoint(
//    _In_ WS_HTTP_SSL_BINDING_TEMPLATE* templateValue,
//    _In_opt_ WS_STRING address,
//    _In_opt_ struct DefaultBinding_ICalculatorFunctionTable* functionTable,
//    _In_opt_ WS_SERVICE_SECURITY_CALLBACK authorizationCallback,
//    _In_reads_opt_(endpointPropertyCount) WS_SERVICE_ENDPOINT_PROPERTY* endpointProperties,
//    _In_ const ULONG endpointPropertyCount,
//    _In_ WS_HEAP* heap,
//    _Outptr_ WS_SERVICE_ENDPOINT** serviceEndpoint,
//    _In_opt_ WS_ERROR* error);
//
// This function can be used to create a Service Host object, which has to be 
// created before a web services starts receiving requests from its clients.
//
// For more information about content of this file, please see the documentation for
// Wsutil compiler.
#if _MSC_VER > 1000 
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

// The following client functions were generated:

//     CalcBindingHeaderAuthSSL_Add
//     CalcBindingHeaderAuthSSL_Multiply
//     CalcBindingHeaderAuthSSL_CloseService
//     CalcBindingSSL_Add
//     CalcBindingSSL_Multiply
//     CalcBindingSSL_CloseService
//     CalcBindingUnsecure_Add
//     CalcBindingUnsecure_Multiply
//     CalcBindingUnsecure_CloseService

// The following server function tables were generated:

//     struct CalcBindingHeaderAuthSSLFunctionTable
//     struct CalcBindingSSLFunctionTable
//     struct CalcBindingUnsecureFunctionTable

// The following types were generated:

//     struct EmptyRequest;
//     struct MultipurposeSingleIntegerResponse;
//     struct BinaryOpRequest;
//     struct OperationResponse;
//     struct EmptyRequest;
//     struct MultipurposeSingleIntegerResponse;
//     struct BinaryOpRequest;
//     struct OperationResponse;
//     struct BinaryOpRequest;
//     struct OperationResponse;

// the following policy helpers were generated:

//  CalcBindingHeaderAuthSSL_CreateServiceEndpoint;
//  CalcBindingSSL_CreateServiceEndpoint;
//  CalcBindingUnsecure_CreateServiceEndpoint;

// The following header files must be included in this order before this one

// #include <WebServices.h>

////////////////////////////////////////////////
// C structure definitions for generated types
////////////////////////////////////////////////

typedef struct EmptyRequest EmptyRequest;

// typeDescription: calculator_wsdl.globalTypes.MultipurposeSingleIntegerResponse
typedef struct MultipurposeSingleIntegerResponse 
{
    __int64 result;
} MultipurposeSingleIntegerResponse;

// typeDescription: calculator_wsdl.globalTypes.BinaryOpRequest
typedef struct BinaryOpRequest 
{
    double first;
    double second;
} BinaryOpRequest;

// typeDescription: calculator_wsdl.globalTypes.OperationResponse
typedef struct OperationResponse 
{
    double result;
} OperationResponse;

////////////////////////////////////////////////
// Policy helper routines
////////////////////////////////////////////////

//  WS_CHANNEL_PROPERTY_ENCODING = WS_ENCODING_XML_BINARY_1,
//  WS_CHANNEL_PROPERTY_ADDRESSING_VERSION = WS_ADDRESSING_VERSION_TRANSPORT,
//  WS_ENVELOPE_VERSION = WS_ENVELOPE_VERSION_SOAP_1_2,
//  WS_PROTECTION_LEVEL = WS_PROTECTION_LEVEL_SIGN_AND_ENCRYPT,
//  WS_SSL_TRANSPORT_SECURITY_BINDING_POLICY_DESCRIPTION
//  WS_HTTP_HEADER_AUTH_SECURITY_BINDING_POLICY_DESCRIPTION
//  security binding Property: WS_HTTP_HEADER_AUTH_SCHEME_NEGOTIATE,
// client channel type: WS_CHANNEL_TYPE_REQUEST, service endpoint channel type: WS_CHANNEL_TYPE_REPLY

struct CalcBindingHeaderAuthSSLFunctionTable;
HRESULT CalcBindingHeaderAuthSSL_CreateServiceEndpoint(
    _In_opt_ WS_HTTP_SSL_HEADER_AUTH_BINDING_TEMPLATE* templateValue,
    _In_opt_ CONST WS_STRING* address,
    _In_opt_ struct CalcBindingHeaderAuthSSLFunctionTable* functionTable,
    _In_opt_ WS_SERVICE_SECURITY_CALLBACK authorizationCallback,
    _In_reads_opt_(endpointPropertyCount) WS_SERVICE_ENDPOINT_PROPERTY* endpointProperties,
    _In_ const ULONG endpointPropertyCount,
    _In_ WS_HEAP* heap,
    _Outptr_ WS_SERVICE_ENDPOINT** serviceEndpoint,
    _In_opt_ WS_ERROR* error);

////////////////////////////////////////////////
// Service functions definitions
////////////////////////////////////////////////

typedef HRESULT (CALLBACK* CalcBindingHeaderAuthSSL_AddCallback) (
    _In_ const WS_OPERATION_CONTEXT* _context,
    _In_ double first, 
    _In_ double second, 
    _Out_ double* result, 
    _In_ const WS_ASYNC_CONTEXT* _asyncContext,
    _In_ WS_ERROR* _error);

typedef HRESULT (CALLBACK* CalcBindingHeaderAuthSSL_MultiplyCallback) (
    _In_ const WS_OPERATION_CONTEXT* _context,
    _In_ double first, 
    _In_ double second, 
    _Out_ double* result, 
    _In_ const WS_ASYNC_CONTEXT* _asyncContext,
    _In_ WS_ERROR* _error);

typedef HRESULT (CALLBACK* CalcBindingHeaderAuthSSL_CloseServiceCallback) (
    _In_ const WS_OPERATION_CONTEXT* _context,
    _Out_ __int64* result, 
    _In_ const WS_ASYNC_CONTEXT* _asyncContext,
    _In_ WS_ERROR* _error);

// binding: CalcBindingHeaderAuthSSL
typedef struct CalcBindingHeaderAuthSSLFunctionTable 
{
    CalcBindingHeaderAuthSSL_AddCallback CalcBindingHeaderAuthSSL_Add;
    CalcBindingHeaderAuthSSL_MultiplyCallback CalcBindingHeaderAuthSSL_Multiply;
    CalcBindingHeaderAuthSSL_CloseServiceCallback CalcBindingHeaderAuthSSL_CloseService;
} CalcBindingHeaderAuthSSLFunctionTable;

////////////////////////////////////////////////
// Policy helper routines
////////////////////////////////////////////////

//  WS_CHANNEL_PROPERTY_ENCODING = WS_ENCODING_XML_UTF8,
//  WS_CHANNEL_PROPERTY_ADDRESSING_VERSION = WS_ADDRESSING_VERSION_TRANSPORT,
//  WS_ENVELOPE_VERSION = WS_ENVELOPE_VERSION_SOAP_1_2,
//  WS_PROTECTION_LEVEL = WS_PROTECTION_LEVEL_SIGN_AND_ENCRYPT,
//  WS_SSL_TRANSPORT_SECURITY_BINDING_POLICY_DESCRIPTION
// client channel type: WS_CHANNEL_TYPE_REQUEST, service endpoint channel type: WS_CHANNEL_TYPE_REPLY

struct CalcBindingSSLFunctionTable;
HRESULT CalcBindingSSL_CreateServiceEndpoint(
    _In_opt_ WS_HTTP_SSL_BINDING_TEMPLATE* templateValue,
    _In_opt_ CONST WS_STRING* address,
    _In_opt_ struct CalcBindingSSLFunctionTable* functionTable,
    _In_opt_ WS_SERVICE_SECURITY_CALLBACK authorizationCallback,
    _In_reads_opt_(endpointPropertyCount) WS_SERVICE_ENDPOINT_PROPERTY* endpointProperties,
    _In_ const ULONG endpointPropertyCount,
    _In_ WS_HEAP* heap,
    _Outptr_ WS_SERVICE_ENDPOINT** serviceEndpoint,
    _In_opt_ WS_ERROR* error);

////////////////////////////////////////////////
// Service functions definitions
////////////////////////////////////////////////

typedef HRESULT (CALLBACK* CalcBindingSSL_AddCallback) (
    _In_ const WS_OPERATION_CONTEXT* _context,
    _In_ double first, 
    _In_ double second, 
    _Out_ double* result, 
    _In_ const WS_ASYNC_CONTEXT* _asyncContext,
    _In_ WS_ERROR* _error);

typedef HRESULT (CALLBACK* CalcBindingSSL_MultiplyCallback) (
    _In_ const WS_OPERATION_CONTEXT* _context,
    _In_ double first, 
    _In_ double second, 
    _Out_ double* result, 
    _In_ const WS_ASYNC_CONTEXT* _asyncContext,
    _In_ WS_ERROR* _error);

typedef HRESULT (CALLBACK* CalcBindingSSL_CloseServiceCallback) (
    _In_ const WS_OPERATION_CONTEXT* _context,
    _Out_ __int64* result, 
    _In_ const WS_ASYNC_CONTEXT* _asyncContext,
    _In_ WS_ERROR* _error);

// binding: CalcBindingSSL
typedef struct CalcBindingSSLFunctionTable 
{
    CalcBindingSSL_AddCallback CalcBindingSSL_Add;
    CalcBindingSSL_MultiplyCallback CalcBindingSSL_Multiply;
    CalcBindingSSL_CloseServiceCallback CalcBindingSSL_CloseService;
} CalcBindingSSLFunctionTable;

////////////////////////////////////////////////
// Policy helper routines
////////////////////////////////////////////////

//  WS_CHANNEL_PROPERTY_ENCODING = WS_ENCODING_XML_UTF8,
//  WS_CHANNEL_PROPERTY_ADDRESSING_VERSION = WS_ADDRESSING_VERSION_TRANSPORT,
//  WS_ENVELOPE_VERSION = WS_ENVELOPE_VERSION_SOAP_1_2,
// client channel type: WS_CHANNEL_TYPE_REQUEST, service endpoint channel type: WS_CHANNEL_TYPE_REPLY

struct CalcBindingUnsecureFunctionTable;
HRESULT CalcBindingUnsecure_CreateServiceEndpoint(
    _In_opt_ WS_HTTP_BINDING_TEMPLATE* templateValue,
    _In_opt_ CONST WS_STRING* address,
    _In_opt_ struct CalcBindingUnsecureFunctionTable* functionTable,
    _In_opt_ WS_SERVICE_SECURITY_CALLBACK authorizationCallback,
    _In_reads_opt_(endpointPropertyCount) WS_SERVICE_ENDPOINT_PROPERTY* endpointProperties,
    _In_ const ULONG endpointPropertyCount,
    _In_ WS_HEAP* heap,
    _Outptr_ WS_SERVICE_ENDPOINT** serviceEndpoint,
    _In_opt_ WS_ERROR* error);

////////////////////////////////////////////////
// Service functions definitions
////////////////////////////////////////////////

typedef HRESULT (CALLBACK* CalcBindingUnsecure_AddCallback) (
    _In_ const WS_OPERATION_CONTEXT* _context,
    _In_ double first, 
    _In_ double second, 
    _Out_ double* result, 
    _In_ const WS_ASYNC_CONTEXT* _asyncContext,
    _In_ WS_ERROR* _error);

typedef HRESULT (CALLBACK* CalcBindingUnsecure_MultiplyCallback) (
    _In_ const WS_OPERATION_CONTEXT* _context,
    _In_ double first, 
    _In_ double second, 
    _Out_ double* result, 
    _In_ const WS_ASYNC_CONTEXT* _asyncContext,
    _In_ WS_ERROR* _error);

typedef HRESULT (CALLBACK* CalcBindingUnsecure_CloseServiceCallback) (
    _In_ const WS_OPERATION_CONTEXT* _context,
    _Out_ __int64* result, 
    _In_ const WS_ASYNC_CONTEXT* _asyncContext,
    _In_ WS_ERROR* _error);

// binding: CalcBindingUnsecure
typedef struct CalcBindingUnsecureFunctionTable 
{
    CalcBindingUnsecure_AddCallback CalcBindingUnsecure_Add;
    CalcBindingUnsecure_MultiplyCallback CalcBindingUnsecure_Multiply;
    CalcBindingUnsecure_CloseServiceCallback CalcBindingUnsecure_CloseService;
} CalcBindingUnsecureFunctionTable;

////////////////////////////////////////////////
// Global web service descriptions.
////////////////////////////////////////////////

typedef struct _calculator_wsdl
{
    struct // globalTypes
    {
        // xml type: EmptyRequest ("http://calculator.example.org/")
        // c type: EmptyRequest
        // WS_TYPE: WS_STRUCT_TYPE
        // typeDescription: calculator_wsdl.globalTypes.EmptyRequest
        WS_STRUCT_DESCRIPTION EmptyRequest;
        
        // xml type: MultipurposeSingleIntegerResponse ("http://calculator.example.org/")
        // c type: MultipurposeSingleIntegerResponse
        // WS_TYPE: WS_STRUCT_TYPE
        // typeDescription: calculator_wsdl.globalTypes.MultipurposeSingleIntegerResponse
        WS_STRUCT_DESCRIPTION MultipurposeSingleIntegerResponse;
        
        // xml type: BinaryOpRequest ("http://calculator.example.org/")
        // c type: BinaryOpRequest
        // WS_TYPE: WS_STRUCT_TYPE
        // typeDescription: calculator_wsdl.globalTypes.BinaryOpRequest
        WS_STRUCT_DESCRIPTION BinaryOpRequest;
        
        // xml type: OperationResponse ("http://calculator.example.org/")
        // c type: OperationResponse
        // WS_TYPE: WS_STRUCT_TYPE
        // typeDescription: calculator_wsdl.globalTypes.OperationResponse
        WS_STRUCT_DESCRIPTION OperationResponse;
        
    } globalTypes;
    struct // globalElements
    {
        // xml element: CloseServiceRequest ("http://calculator.example.org/")
        // c type: EmptyRequest
        // elementDescription: calculator_wsdl.globalElements.CloseServiceRequest
        WS_ELEMENT_DESCRIPTION CloseServiceRequest;
        
        // xml element: CloseServiceResponse ("http://calculator.example.org/")
        // c type: MultipurposeSingleIntegerResponse
        // elementDescription: calculator_wsdl.globalElements.CloseServiceResponse
        WS_ELEMENT_DESCRIPTION CloseServiceResponse;
        
        // xml element: AdditionRequest ("http://calculator.example.org/")
        // c type: BinaryOpRequest
        // elementDescription: calculator_wsdl.globalElements.AdditionRequest
        WS_ELEMENT_DESCRIPTION AdditionRequest;
        
        // xml element: AdditionResponse ("http://calculator.example.org/")
        // c type: OperationResponse
        // elementDescription: calculator_wsdl.globalElements.AdditionResponse
        WS_ELEMENT_DESCRIPTION AdditionResponse;
        
        // xml element: MultiplicationRequest ("http://calculator.example.org/")
        // c type: BinaryOpRequest
        // elementDescription: calculator_wsdl.globalElements.MultiplicationRequest
        WS_ELEMENT_DESCRIPTION MultiplicationRequest;
        
        // xml element: MultiplicationResponse ("http://calculator.example.org/")
        // c type: OperationResponse
        // elementDescription: calculator_wsdl.globalElements.MultiplicationResponse
        WS_ELEMENT_DESCRIPTION MultiplicationResponse;
        
    } globalElements;
    struct // messages
    {
        // message: AdditionRequestMessage
        // c type: BinaryOpRequest
        // action: "http://calculator.example.org/Add"
        // messageDescription: calculator_wsdl.messages.AdditionRequestMessage
        WS_MESSAGE_DESCRIPTION AdditionRequestMessage;
        
        // message: AdditionResponseMessage
        // c type: OperationResponse
        // action: ""
        // messageDescription: calculator_wsdl.messages.AdditionResponseMessage
        WS_MESSAGE_DESCRIPTION AdditionResponseMessage;
        
        // message: MultiplicationRequestMessage
        // c type: BinaryOpRequest
        // action: "http://calculator.example.org/Multiply"
        // messageDescription: calculator_wsdl.messages.MultiplicationRequestMessage
        WS_MESSAGE_DESCRIPTION MultiplicationRequestMessage;
        
        // message: MultiplicationResponseMessage
        // c type: OperationResponse
        // action: ""
        // messageDescription: calculator_wsdl.messages.MultiplicationResponseMessage
        WS_MESSAGE_DESCRIPTION MultiplicationResponseMessage;
        
        // message: CloseServiceRequestMessage
        // c type: EmptyRequest
        // action: "http://calculator.example.org/Close"
        // messageDescription: calculator_wsdl.messages.CloseServiceRequestMessage
        WS_MESSAGE_DESCRIPTION CloseServiceRequestMessage;
        
        // message: CloseServiceResponseMessage
        // c type: MultipurposeSingleIntegerResponse
        // action: ""
        // messageDescription: calculator_wsdl.messages.CloseServiceResponseMessage
        WS_MESSAGE_DESCRIPTION CloseServiceResponseMessage;
        
    } messages;
    struct // contracts
    {
        // binding: CalcBindingHeaderAuthSSL
        // portType: CalculatorInterface
        // operation: CalcBindingHeaderAuthSSL_Add
        //     input message: AdditionRequestMessage
        //     output message: AdditionResponseMessage
        // operation: CalcBindingHeaderAuthSSL_Multiply
        //     input message: MultiplicationRequestMessage
        //     output message: MultiplicationResponseMessage
        // operation: CalcBindingHeaderAuthSSL_CloseService
        //     input message: CloseServiceRequestMessage
        //     output message: CloseServiceResponseMessage
        // contractDescription: calculator_wsdl.contracts.CalcBindingHeaderAuthSSL
        WS_CONTRACT_DESCRIPTION CalcBindingHeaderAuthSSL;
        
        // binding: CalcBindingSSL
        // portType: CalculatorInterface
        // operation: CalcBindingSSL_Add
        //     input message: AdditionRequestMessage
        //     output message: AdditionResponseMessage
        // operation: CalcBindingSSL_Multiply
        //     input message: MultiplicationRequestMessage
        //     output message: MultiplicationResponseMessage
        // operation: CalcBindingSSL_CloseService
        //     input message: CloseServiceRequestMessage
        //     output message: CloseServiceResponseMessage
        // contractDescription: calculator_wsdl.contracts.CalcBindingSSL
        WS_CONTRACT_DESCRIPTION CalcBindingSSL;
        
        // binding: CalcBindingUnsecure
        // portType: CalculatorInterface
        // operation: CalcBindingUnsecure_Add
        //     input message: AdditionRequestMessage
        //     output message: AdditionResponseMessage
        // operation: CalcBindingUnsecure_Multiply
        //     input message: MultiplicationRequestMessage
        //     output message: MultiplicationResponseMessage
        // operation: CalcBindingUnsecure_CloseService
        //     input message: CloseServiceRequestMessage
        //     output message: CloseServiceResponseMessage
        // contractDescription: calculator_wsdl.contracts.CalcBindingUnsecure
        WS_CONTRACT_DESCRIPTION CalcBindingUnsecure;
        
    } contracts;
    struct // policies
    {
        // policy for binding: CalcBindingHeaderAuthSSL
        // port name: CalculatorEndpointHeaderAuthSSL, binding: CalcBindingHeaderAuthSSL, namespace: http://calculator.example.org/
        WS_HTTP_SSL_HEADER_AUTH_POLICY_DESCRIPTION CalcBindingHeaderAuthSSL;
        // policy for binding: CalcBindingSSL
        // port name: CalculatorEndpointSSL, binding: CalcBindingSSL, namespace: http://calculator.example.org/
        WS_HTTP_SSL_POLICY_DESCRIPTION CalcBindingSSL;
        // policy for binding: CalcBindingUnsecure
        // port name: CalculatorEndpointUnsecure, binding: CalcBindingUnsecure, namespace: http://calculator.example.org/
        WS_HTTP_POLICY_DESCRIPTION CalcBindingUnsecure;
    } policies;
} _calculator_wsdl;

extern const _calculator_wsdl calculator_wsdl;

#ifdef __cplusplus
}
#endif


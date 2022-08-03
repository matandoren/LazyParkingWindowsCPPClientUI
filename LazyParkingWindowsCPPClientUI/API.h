#pragma once

#ifdef LAZYPARKINGWINDOWSCPPCLIENTBL_EXPORTS
#define LAZYPARKINGBL_API __declspec(dllexport)
#else
#define LAZYPARKINGBL_API __declspec(dllimport)
#endif

#include <string>

typedef int MODEL_DESCRIPTOR;
typedef int CALLBACK_DESCRIPTOR;

#define LAZYPARKINGBLAPI_INVALID_DESCRIPTOR 0

enum eStatusCode
{
	eStatusCode_Success,
	eStatusCode_WSAStartupException, // a runtime exception was generated by the underlying OS implementaion during the execution of the call when starting up WSA
	eStatusCode_SocketConnectionException, // a runtime exception was generated by the underlying OS implementaion during the execution of the call when trying to connect to the server
	eStatusCode_SocketReadException, // a runtime exception was generated by the underlying OS implementaion during the execution of the call when trying to read from a socket
	eStatusCode_SocketWriteException, // a runtime exception was generated by the underlying OS implementaion during the execution of the call when trying to write to a socket
	eStatusCode_InvalidDescriptor, // the function was supplied with an invalid descriptor argument or possibly a valid descriptor but not in the context of the call
	eStatusCode_BadArgumentsOrModelInImproperState
};

// initializes a Model object; you must call this function and use the output parameter o_ModelDescriptor must be used as an argument for all the rest of the functions of the API
extern "C" LAZYPARKINGBL_API eStatusCode CreateModel(MODEL_DESCRIPTOR& o_ModelDescriptor);

// deallocates the resources associated with a Model object; should be called whenever you are done with a Model object
extern "C" LAZYPARKINGBL_API eStatusCode CleanupModel(MODEL_DESCRIPTOR i_Descriptor);

// this function can be called only while the model is NOT logged in; i_numberOfParkingSpots must be greater than 0
extern "C" LAZYPARKINGBL_API eStatusCode SetNumberOfParkingSpots(MODEL_DESCRIPTOR i_Descriptor, int i_numberOfParkingSpots);

// this function can be called only while the model is NOT logged in
extern "C" LAZYPARKINGBL_API eStatusCode SetIP(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_IP);

// this function can be called only while the model is NOT logged in
extern "C" LAZYPARKINGBL_API eStatusCode SetPort(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Port);

// asynchronous function; the result is provided via a callback sent to AddLoginReplyEventHandler; this function can be called only while the model is NOT logged in
extern "C" LAZYPARKINGBL_API eStatusCode Login(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Username, const std::string& i_Password);

// asynchronous method; the result is provided via a callback sent to AddLogoutReplyEventHandler; this function can be called only while the model is logged in
extern "C" LAZYPARKINGBL_API eStatusCode Logout(MODEL_DESCRIPTOR i_Descriptor);

// asynchronous method; the result is provided via a callback sent to AddOpenGateReplyEventHandler; this function can be called only while the model is logged in
extern "C" LAZYPARKINGBL_API eStatusCode OpenGate(MODEL_DESCRIPTOR i_Descriptor);

// asynchronous method; the result is provided via a callback sent to AddCancelReservationReplyEventHandler; this function can be called only while the model is logged in; 
// i_numberOfParkingSpots must be greater than 0
extern "C" LAZYPARKINGBL_API eStatusCode CancelReservation(MODEL_DESCRIPTOR i_Descriptor, int i_ParkingSpotID);

// asynchronous method; the result is provided via a callback sent to AddChangePasswordReplyEventHandler; this function can be called only while the model is logged in;
// both username and password must not be empty
extern "C" LAZYPARKINGBL_API eStatusCode ChangePassword(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Username, const std::string& i_Password);

// asynchronous method; the result is provided via a callback sent to AddDeleteDriverReplyEventHandler; this function can be called only while the model is logged in
extern "C" LAZYPARKINGBL_API eStatusCode DeleteDriver(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Username);

// asynchronous method; the result is provided via a callback sent to AddAddDriverReplyEventHandler; this function can be called only while the model is logged in;
// i_Name must contain first name and last name, both at least 2 characters long and their combined length must not exceed 20 characters, the names must contain only letters;
// i_Password must not be empty
// i_ExpirationDate must be non-negative
extern "C" LAZYPARKINGBL_API eStatusCode AddDriver(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Name, const std::string& i_Password, time_t i_ExpirationDate/* 0 for no expiration */);

// asynchronous method; the result is provided via a callback sent to AddUpdateDriverExpirationReplyEventHandler; this function can be called only while the model is logged in
extern "C" LAZYPARKINGBL_API eStatusCode UpdateDriverExpiration(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Username, time_t i_ExpirationDate/* 0 for no expiration */);

// asynchronous method; the result is provided via a callback sent to AddReserveParkingSpotReplyEventHandler; this function can be called only while the model is logged in
extern "C" LAZYPARKINGBL_API eStatusCode ReserveParkingSpot(MODEL_DESCRIPTOR i_Descriptor, int i_ParkingSpotID, const std::string& i_ReservedFor, time_t i_ExpirationDate/* 0 for no expiration */);

// asynchronous method; the result is provided via a callback sent to AddRequestExpiredUsersReplyEventHandler; this function can be called only while the model is logged in
extern "C" LAZYPARKINGBL_API eStatusCode RequestExpiredUsers(MODEL_DESCRIPTOR i_Descriptor);

// first call RequestExpiredUsers; after a callback sent to AddRequestExpiredUsersReplyEventHandler signals that the expired users list is prepared, use this function to get expired users one by one; 
// this function can be called only while the model is logged in
extern "C" LAZYPARKINGBL_API eStatusCode GetNextExpiredUser(MODEL_DESCRIPTOR i_Descriptor, std::string& o_Username, time_t& o_ExpirationDate);

// this function can be called only while the model is logged in
extern "C" LAZYPARKINGBL_API eStatusCode GetParkingSpotStatus(MODEL_DESCRIPTOR i_Descriptor, int i_ID, bool& o_IsOccupied, bool& o_IsReserved, std::string& o_ReservedFor);

// in the following functions i_OptionalData is used so that the client code could provide a reference to any object to be stored along with the callback;
// this reference will be provided to the callback when invoked;
// the client could use this to provide method wrappers along with a reference to the object that would activate the wrapped method;
// for example: wrapperFunction(void* i_OptionalData){SomeClass *sc = static_cast<SomeClass*>(i_OptionalData); sc->WrappedMethod();}
// if the callback has no need for optional data, pass nullptr;
// the following functions have callback descriptor output parameters that are needed in order to identify the callbacks that you want to remove when calling RemoveXXXXXEventHandler
extern "C" LAZYPARKINGBL_API eStatusCode AddLoginReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */, int/* i_PermissionLevel */, bool/* i_UsernameNotFound */, bool/* i_WrongPW */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode AddLogoutReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode AddOpenGateReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */, bool/* i_IsSuccessful */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode AddCancelReservationReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */, bool/* i_NoPermission */, bool/* i_IDNotFound */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode AddChangePasswordReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */, bool/* i_NoPermission */, bool/* i_UsernameNotFound */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode AddDeleteDriverReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */, bool/* i_NoPermission */, bool/* i_UsernameNotFound */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode AddAddDriverReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */, bool/* i_NoPermission */, bool/* i_DateExpired */, const std::string&/* i_Username */, int/* i_CardKey */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode AddUpdateDriverExpirationReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */, bool/* i_NoPermission */, bool/* i_UsernameNotFound */, bool/* i_DateExpired */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode AddReserveParkingSpotReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */, bool/* i_NoPermission */, bool/* i_IDNotFound */, bool/* i_DateExpired */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode AddRequestExpiredUsersReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */, int/* i_NumberOfExpiredUsers */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode AddParkingStatusUpdateEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor,
	void(*i_Callback)(void*/* i_OptionalData */, int/* i_ParkingSpotID */, bool/* i_IsOccupied */, bool/* i_IsReserved */, const std::string&/* i_ReservedFor */), void* i_OptionalData, CALLBACK_DESCRIPTOR & o_CallbackDescriptor);


extern "C" LAZYPARKINGBL_API eStatusCode RemoveLoginReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode RemoveLogoutReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode RemoveOpenGateReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode RemoveCancelReservationReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode RemoveChangePasswordReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode RemoveDeleteDriverReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode RemoveAddDriverReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode RemoveUpdateDriverExpirationReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode RemoveReserveParkingSpotReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode RemoveRequestExpiredUsersReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);

extern "C" LAZYPARKINGBL_API eStatusCode RemoveParkingStatusUpdateEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor);
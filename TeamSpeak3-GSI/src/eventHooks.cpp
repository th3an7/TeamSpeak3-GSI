#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

#include <teamspeak/public_errors.h>
#include <teamspeak/public_errors_rare.h>
#include <teamspeak/public_definitions.h>
#include <teamspeak/public_rare_definitions.h>
#include <teamspeak/clientlib_publicdefinitions.h>
#include <ts3_functions.h>

#include "plugin_exports.hpp"
#include "eventHooks.hpp"

#define PREPARE_JSON_FOR_AURORA(x) \
rapidjson::Pointer("/provider/name").Set(x, "TeamSpeak"); \
rapidjson::Pointer("/provider/appid").Set(x, -1);

#define JSON_ADD_VAL(documentName,subName,valName) rapidjson::Pointer("/data/"#subName"/"#valName).Set(documentName, valName);

void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber) {
	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	JSON_ADD_VAL(json, onConnectStatusChangeEvent, serverConnectionHandlerID);
	JSON_ADD_VAL(json, onConnectStatusChangeEvent, newStatus);
	JSON_ADD_VAL(json, onConnectStatusChangeEvent, errorNumber);

	sendJSON_to_Aurora(json);
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage) {
	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	JSON_ADD_VAL(json, onClientMoveEvent, serverConnectionHandlerID);
	JSON_ADD_VAL(json, onClientMoveEvent, clientID);
	JSON_ADD_VAL(json, onClientMoveEvent, oldChannelID);
	JSON_ADD_VAL(json, onClientMoveEvent, newChannelID);
	JSON_ADD_VAL(json, onClientMoveEvent, visibility);
	JSON_ADD_VAL(json, onClientMoveEvent, moveMessage);

	sendJSON_to_Aurora(json);
}

void ts3plugin_onClientKickFromChannelEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage) {
	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	JSON_ADD_VAL(json, onClientKickFromChannelEvent, serverConnectionHandlerID);
	JSON_ADD_VAL(json, onClientKickFromChannelEvent, clientID);
	JSON_ADD_VAL(json, onClientKickFromChannelEvent, oldChannelID);
	JSON_ADD_VAL(json, onClientKickFromChannelEvent, newChannelID);
	JSON_ADD_VAL(json, onClientKickFromChannelEvent, visibility);
	JSON_ADD_VAL(json, onClientKickFromChannelEvent, kickerID);
	JSON_ADD_VAL(json, onClientKickFromChannelEvent, kickerName);
	JSON_ADD_VAL(json, onClientKickFromChannelEvent, kickerUniqueIdentifier);
	JSON_ADD_VAL(json, onClientKickFromChannelEvent, kickMessage);

	sendJSON_to_Aurora(json);
}

void ts3plugin_onClientKickFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage) {
	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	JSON_ADD_VAL(json, onClientKickFromServerEvent, serverConnectionHandlerID);
	JSON_ADD_VAL(json, onClientKickFromServerEvent, clientID);
	JSON_ADD_VAL(json, onClientKickFromServerEvent, oldChannelID);
	JSON_ADD_VAL(json, onClientKickFromServerEvent, newChannelID);
	JSON_ADD_VAL(json, onClientKickFromServerEvent, visibility);
	JSON_ADD_VAL(json, onClientKickFromServerEvent, kickerID);
	JSON_ADD_VAL(json, onClientKickFromServerEvent, kickerName);
	JSON_ADD_VAL(json, onClientKickFromServerEvent, kickerUniqueIdentifier);
	JSON_ADD_VAL(json, onClientKickFromServerEvent, kickMessage);

	sendJSON_to_Aurora(json);
}

int ts3plugin_onClientPokeEvent(uint64 serverConnectionHandlerID, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored) {
	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	JSON_ADD_VAL(json, onClientPokeEvent, serverConnectionHandlerID);
	JSON_ADD_VAL(json, onClientPokeEvent, fromClientID);
	JSON_ADD_VAL(json, onClientPokeEvent, pokerName);
	JSON_ADD_VAL(json, onClientPokeEvent, pokerUniqueIdentity);
	JSON_ADD_VAL(json, onClientPokeEvent, message);
	JSON_ADD_VAL(json, onClientPokeEvent, ffIgnored);

	sendJSON_to_Aurora(json);

	return 0;  /* 0 = handle normally, 1 = client will ignore the poke */
}

int ts3plugin_onTextMessageEvent(uint64 serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored) {
	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	JSON_ADD_VAL(json, onTextMessageEvent, serverConnectionHandlerID);
	JSON_ADD_VAL(json, onTextMessageEvent, toID);
	JSON_ADD_VAL(json, onTextMessageEvent, fromName);
	JSON_ADD_VAL(json, onTextMessageEvent, fromUniqueIdentifier);
	JSON_ADD_VAL(json, onTextMessageEvent, message);
	JSON_ADD_VAL(json, onTextMessageEvent, ffIgnored);

	sendJSON_to_Aurora(json);

	return 0;
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
	char name[512];

	if (ts3Functions.getClientDisplayName(serverConnectionHandlerID, clientID, name, 512) == ERROR_ok) {
		rapidjson::Document json;
		PREPARE_JSON_FOR_AURORA(json);

		JSON_ADD_VAL(json, onTalkStatusChangeEvent, serverConnectionHandlerID);
		JSON_ADD_VAL(json, onTalkStatusChangeEvent, status);
		JSON_ADD_VAL(json, onTalkStatusChangeEvent, isReceivedWhisper);
		JSON_ADD_VAL(json, onTalkStatusChangeEvent, clientID);
		JSON_ADD_VAL(json, onTalkStatusChangeEvent, name);

		sendJSON_to_Aurora(json);
	}
}

void ts3plugin_onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char* oldValue, const char* newValue) {
	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	JSON_ADD_VAL(json, onClientSelfVariableUpdateEvent, serverConnectionHandlerID);
	JSON_ADD_VAL(json, onClientSelfVariableUpdateEvent, flag);
	JSON_ADD_VAL(json, onClientSelfVariableUpdateEvent, oldValue);
	JSON_ADD_VAL(json, onClientSelfVariableUpdateEvent, newValue);

	sendJSON_to_Aurora(json);
}

#include <stdio.h>

#include <teamspeak/public_errors.h>
#include <teamspeak/public_errors_rare.h>
#include <teamspeak/public_definitions.h>
#include <teamspeak/public_rare_definitions.h>
#include <teamspeak/clientlib_publicdefinitions.h>
#include <ts3_functions.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "plugin_exports.hpp"


#ifdef _WIN32
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Normaliz.lib")
#endif

#pragma comment(lib, "libcurl_a.lib")

#define CURL_STATICLIB
#include <curl/curl.h>

static TS3Functions ts3Functions;

void safe_strcpy(char* dest, size_t destSize, char const* src) {
	for (size_t currentIndex = 0; currentIndex < destSize; currentIndex++) {
		if (!src[currentIndex]) {
			break;
		}
		dest[currentIndex] = src[currentIndex];
	}
}

#define PLUGIN_API_VERSION 23

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128

#define PREPARE_JSON_FOR_AURORA(x) \
x["provider"]["name"] = "TeamSpeak"; \
x["provider"]["appid"] = -1;

static char* pluginID = nullptr;

/* Unique name identifying this plugin */
const char* ts3plugin_name() {
	//Removed UTF16 to UTF8 conversion because this name is fully valid ASCII alphanum and ASCII alphanum is fully valid UTF8
	return "Aurora GSI Integration";
}

/* Plugin version */
const char* ts3plugin_version() {
	return "0.1";
}

/* Plugin API version. Must be the same as the clients API major version, else the plugin fails to load. */
int ts3plugin_apiVersion() {
	return PLUGIN_API_VERSION;
}

/* Plugin author */
const char* ts3plugin_author() {
	return "Th3 Ant 'n SnakePin";
}

/* Plugin description */
const char* ts3plugin_description() {
	return "Display TeamSpeak3 events as RGB effects in Project Aurora";
}

/* Set TeamSpeak 3 callback functions */
void ts3plugin_setFunctionPointers(const TS3Functions funcs) {
	ts3Functions = funcs;
}

int ts3plugin_init() {
	char appPath[PATH_BUFSIZE];
	char resourcesPath[PATH_BUFSIZE];
	char configPath[PATH_BUFSIZE];
	char pluginPath[PATH_BUFSIZE];

	/* Your plugin init code here */
	printf("PLUGIN: init\n");

	// Init CURL
	curl_global_init(CURL_GLOBAL_ALL);

	/* Example on how to query application, resources and configuration paths from client */
	/* Note: Console client returns empty string for app and resources path */
	ts3Functions.getAppPath(appPath, PATH_BUFSIZE);
	ts3Functions.getResourcesPath(resourcesPath, PATH_BUFSIZE);
	ts3Functions.getConfigPath(configPath, PATH_BUFSIZE);
	ts3Functions.getPluginPath(pluginPath, PATH_BUFSIZE, pluginID);

	printf("PLUGIN: App path: %s\nResources path: %s\nConfig path: %s\nPlugin path: %s\n", appPath, resourcesPath, configPath, pluginPath);

	return 0;  /* 0 = success, 1 = failure, -2 = failure but client will not show a "failed to load" warning */
	/* -2 is a very special case and should only be used if a plugin displays a dialog (e.g. overlay) asking the user to disable
	 * the plugin again, avoiding the show another dialog by the client telling the user the plugin failed to load.
	 * For normal case, if a plugin really failed to load because of an error, the correct return value is 1. */
}

void ts3plugin_shutdown() {
	/* Your plugin cleanup code here */
	printf("PLUGIN: shutdown\n");

	// CURL Cleanup
	curl_global_cleanup();

	/*
	 * Note:
	 * If your plugin implements a settings dialog, it must be closed and deleted here, else the
	 * TeamSpeak client will most likely crash (DLL removed but dialog from DLL code still open).
	 */

	 /* Free pluginID if we registered it */
	if (pluginID != nullptr) {
		delete[] pluginID;
		pluginID = nullptr;
	}
}

/****************************** Optional functions ********************************/
/*
 * Following functions are optional, if not needed you don't need to implement them.
 */

void ts3plugin_registerPluginID(const char* id) {
	const size_t sz = strlen(id) + 1;
	pluginID = new char[sz];
	safe_strcpy(pluginID, sz, id);  /* The id buffer will invalidate after exiting this function */
	printf("PLUGIN: registerPluginID: %s\n", pluginID);
}

int sendJSON_to_Aurora(rapidjson::Document& json) {
	CURL* curlHandle;
	CURLcode curlResult;

	/* get a curl handle */
	curlHandle = curl_easy_init();
	if (curlHandle) {
		struct curl_slist* headerstruct = curl_slist_append(NULL, "Content-Type: application/json");
		curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headerstruct);
		curl_easy_setopt(curlHandle, CURLOPT_URL, "http://localhost:9088");


		rapidjson::StringBuffer buffer; rapidjson::Writer<rapidjson::StringBuffer> writer(buffer); json.Accept(writer);
		curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, buffer.GetString());

		curlResult = curl_easy_perform(curlHandle);

		// If sending request fails, print the error message
		if (curlResult != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curlResult));
		}

		curl_easy_cleanup(curlHandle);
	}

	return 0;
}

void ts3plugin_onConnectStatusChangeEvent(uint64 severConnectionHandlerID, int newStatus, unsigned int errorNumber) {
	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	if (newStatus == STATUS_CONNECTING) {
		printf("PLUGIN onConnectStatusChangeEvent: Connecting - status 1\n");
		json["state"]["connected"] = 1;
	}
	else if (newStatus == STATUS_CONNECTED) {
		printf("PLUGIN onConnectStatusChangeEvent: Connected - status 2\n");
		json["state"]["connected"] = 2;
	}
	else if (newStatus == STATUS_CONNECTION_ESTABLISHING) {
		printf("PLUGIN onConnectStatusChangeEvent: Establishing connection - status 3\n");
		json["state"]["connected"] = 3;
	}
	else if (newStatus == STATUS_CONNECTION_ESTABLISHED) {
		printf("PLUGIN onConnectStatusChangeEvent: Established connection - status 4\n");
		json["state"]["connected"] = 4;
	}

	sendJSON_to_Aurora(json);
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage) {
	printf("PLUGIN onClientMoveEvent: User moved\n");

	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	json["state"]["moved"] = true;

	sendJSON_to_Aurora(json);
}

void ts3plugin_onClientKickFromChannelEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage) {
	printf("PLUGIN onClientKickFromChannelEvent: User kicked from the channel\n");

	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	json["state"]["kicked"] = 1;

	sendJSON_to_Aurora(json);
}

void ts3plugin_onClientKickFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage) {
	printf("PLUGIN onClientKickFromServerEvent: User kicked from the server\n");

	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	json["state"]["kicked"] = 2;

	sendJSON_to_Aurora(json);
}

int ts3plugin_onClientPokeEvent(uint64 serverConnectionHandlerID, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored) {
	printf("PLUGIN onClientPokeEvent: Received PM message\n");

	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	json["state"]["text"] = 0;

	sendJSON_to_Aurora(json);

	return 0;  /* 0 = handle normally, 1 = client will ignore the poke */
}

int ts3plugin_onTextMessageEvent(uint64 serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored) {
	printf("PLUGIN: onTextMessageEvent: Received text message\n");

	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	json["state"]["text"] = 1;

	sendJSON_to_Aurora(json);

	return 0;
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
	/* Demonstrate usage of getClientDisplayName */
	char name[512];

	if (ts3Functions.getClientDisplayName(serverConnectionHandlerID, clientID, name, 512) == ERROR_ok) {
		rapidjson::Document json;
		PREPARE_JSON_FOR_AURORA(json);

		if (status == STATUS_TALKING) {
			printf("--> %s starts talking\n", name);
			json["state"]["talking"] = true;

		}
		else {
			printf("--> %s stops talking\n", name);
			json["state"]["talking"] = false;
		}

		sendJSON_to_Aurora(json);
	}
}

void ts3plugin_onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char* oldValue, const char* newValue) {
	rapidjson::Document json;
	PREPARE_JSON_FOR_AURORA(json);

	if (flag == CLIENT_OUTPUT_MUTED) {
		printf("PLUGIN: onClientSelfVariableUpdateEvent: Client output muted: %s\n", newValue);
		if (strcmp(newValue, "1") == 0) {
			json["state"]["outputMuted"] = true;
		}
		else {
			json["state"]["outputMuted"] = false;
		}
	}
	else if (flag == CLIENT_INPUT_MUTED) {
		printf("PLUGIN: onClientSelfVariableUpdateEvent: Client input muted: %s\n", newValue);
		if (strcmp(newValue, "1") == 0) {
			json["state"]["inputMuted"] = true;
		}
		else
		{
			json["state"]["inputMuted"] = false;
		}
	}

	sendJSON_to_Aurora(json);
}

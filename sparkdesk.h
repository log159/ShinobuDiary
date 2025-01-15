#pragma once
#include "./sparkdesk_src/include/sparkchain.h"
#include "./sparkdesk_src/include/sc_llm.h"

#include <iostream>
#include <string>
#include <atomic>
#include <Windows.h>
#include <regex>
using namespace SparkChain;
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define RESET "\033[0m"

// async status tag
static std::atomic_bool finish(false);
// result cache

class SparkCallbacks : public LLMCallbacks {

	void onLLMResult(LLMResult* result, void* usrContext) {
		static std::string final_result = "";

		int status = result->getStatus();
		printf(GREEN "%d:%s:%s " "usrContext:%d\n" RESET, status, result->getRole(), result->getContent(), *(int*)usrContext);
		final_result += std::string(result->getContent());
		if (status == 2) {
			printf(GREEN "tokens:%d + %d = %d "  "usrContext:%d\n" RESET, result->getCompletionTokens(), result->getPromptTokens(), result->getTotalTokens(), *(int*)usrContext);
			finish = true;
		}
	}
	void onLLMEvent(LLMEvent* event, void* usrContext) {
		printf(YELLOW "onLLMEventCB\n  eventID:%d eventMsg:%s "  "usrContext:%d\n" RESET, event->getEventID(), event->getEventMsg(), *(int*)usrContext);
	}
	void onLLMError(LLMError* error, void* usrContext) {
		printf(RED "onLLMErrorCB\n errCode:%d errMsg:%s "  "usrContext:%d\n" RESET, error->getErrCode(), error->getErrMsg(), *(int*)usrContext);
		finish = true;
	}
};

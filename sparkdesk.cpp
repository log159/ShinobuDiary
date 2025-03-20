#include "sparkdesk.h"
int initSDK()
{
	// 全局初始化
	SparkChainConfig* config = SparkChainConfig::builder();
	config->appID("")       // 你的appid
		->apiKey("")        // 你的apikey
		->apiSecret("");	// 你的apisecret
	int ret = SparkChain::init(config);
	printf(RED "\ninit SparkChain result:%d" RESET, ret);
	return ret;
}

void syncLLMTest()
{
	std::cout << "\n######### 同步调用 #########" << std::endl;
	// 配置大模型参数
	LLMConfig* llmConfig = LLMConfig::builder();
	llmConfig->domain("generalv3.5");
	llmConfig->url("ws(s)://spark-api.xf-yun.com/v3.5/chat");

	Memory* window_memory = Memory::WindowMemory(5);
	LLM* syncllm = LLM::create(llmConfig, window_memory);

	// Memory* token_memory = Memory::TokenMemory(500);
	// LLM *syncllm = LLM::create(llmConfig,token_memory);

	int i = 0;
	const char* input = "介绍一下讯飞星火公司";
	while (i++ < 2)
	{
		// 同步请求
		LLMSyncOutput* result = syncllm->run(input);
		if (result->getErrCode() != 0)
		{
			printf(RED "\nsyncOutput: %d:%s\n\n" RESET, result->getErrCode(), result->getErrMsg());
			continue;
		}
		else
		{
			printf(GREEN "\nsyncOutput: %s:%s\n" RESET, result->getRole(), result->getContent());
		}
		input = "那日语呢？";
	}
	// 垃圾回收
	if (syncllm != nullptr)
	{
		LLM::destroy(syncllm);
	}
}

void asyncLLMTest()
{
	std::cout << "\n######### 异步调用 #########" << std::endl;
	// 配置大模型参数
	LLMConfig* llmConfig = LLMConfig::builder();
	llmConfig->domain("generalv3.5");
	llmConfig->url("ws(s)://spark-api.xf-yun.com/v3.5/chat");
	Memory* window_memory = Memory::WindowMemory(5);
	LLM* asyncllm = LLM::create(llmConfig, window_memory);

	// Memory* token_memory = Memory::TokenMemory(500);
	// LLM *asyncllm = LLM::create(llmConfig,token_memory);
	if (asyncllm == nullptr)
	{
		printf(RED "\nLLMTest fail, please setLLMConfig before" RESET);
		return;
	}
	// 注册监听回调
	SparkCallbacks* cbs = new SparkCallbacks();
	asyncllm->registerLLMCallbacks(cbs);

	// 异步请求
	int i = 0;
	const char* input = "你好用英语怎么说？";
	while (i++ < 2)
	{
		finish = false;
		int usrContext = 1;
		int ret = asyncllm->arun(input, &usrContext);
		if (ret != 0)
		{
			printf(RED "\narun failed: %d\n\n" RESET, ret);
			finish = true;
			continue;
		}

		int times = 0;
		while (!finish)
		{ // 等待结果返回退出
			Sleep(1000);
			if (times++ > 10) // 等待十秒如果没有最终结果返回退出
				break;
		}
		input = "那日语呢？";
	}
	// 垃圾回收
	if (asyncllm != nullptr)
	{
		LLM::destroy(asyncllm);
	}
	if (cbs != nullptr)
		delete cbs;
}

void uninitSDK()
{
	// 全局逆初始化
	SparkChain::unInit();
}

void send()
{
	system("chcp 65001");
	std::cout << "\n######### llm Demo #########" << std::endl;
	// 全局初始化
	int ret = initSDK();
	if (ret != 0)
	{
		std::cout << "initSDK failed:" << ret << std::endl;
	}
	// 同步调用和异步调用选择一个执行
	syncLLMTest(); // 同步调用
	//asyncLLMTest(); // 异步调用
	// 退出
	uninitSDK();
}

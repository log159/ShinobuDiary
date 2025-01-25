#include "llmhome.h"

namespace Su {
    void SparkDesk(Su::Kasb* kasb, char* const buf, size_t bufsize, const char* question) {

        SparkChainConfig* config = SparkChainConfig::builder();
        config->appID(kasb->appid)          // 你的appid
            ->apiKey(kasb->key)             // 你的apikey
            ->apiSecret(kasb->secret);      // 你的apisecret
        int ret = SparkChain::init(config);
        if (ret != 0)
        {
            std::cout << "initSDK failed:" << ret << std::endl;
        }
        printf(u8"\ninit SparkChain result:%d" RESET, ret);
        std::cout << u8"\n######### 同步调用 #########" << std::endl;
        // 配置大模型参数
        SparkChain::LLMConfig* llmConfig = SparkChain::LLMConfig::builder();
        llmConfig->domain("generalv3.5");
        llmConfig->url("ws(s)://spark-api.xf-yun.com/v3.5/chat");

        Memory* window_memory = Memory::WindowMemory(5);
        SparkChain::LLM* syncllm = SparkChain::LLM::create(llmConfig, window_memory);

        // Memory* token_memory = Memory::TokenMemory(500);
        // LLM *syncllm = LLM::create(llmConfig,token_memory);

        int i = 0;
        const char* input = question;
        while (i++ < 1)
        {
            // 同步请求
            LLMSyncOutput* result = syncllm->run(input);
            if (result->getErrCode() != 0)
            {
                //printf( RESET, result->getErrCode(), result->getErrMsg());
                snprintf(buf, bufsize, u8"\nsyncOutput: %d:%s\n\n", result->getErrCode(), result->getErrMsg());
                continue;
            }
            else
            {
                //printf(u8"\nsyncOutput: %s:%s\n" RESET, result->getRole(), result->getContent());
                snprintf(buf, bufsize, u8"\nsyncOutput: %s:%s\n", result->getRole(), result->getContent());
            }
        }
        // 垃圾回收
        if (syncllm != nullptr)
        {
            SparkChain::LLM::destroy(syncllm);
        }
    }
}

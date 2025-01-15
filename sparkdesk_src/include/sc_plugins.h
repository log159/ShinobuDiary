#ifndef SC_PLUGINS_API_H
#define SC_PLUGINS_API_H

#include "sc_plugins.h"
#include "sc_type.h"

#include <string>
#include <list>
#include <memory>

namespace SparkChain {

    class  BaseModel {
        ;
    };

    class  BasePlugin {
    public:
        __declspec(dllexport) BasePlugin(std::string name);
        __declspec(dllexport) BasePlugin(std::string name, std::string description, std::string prompt, bool isDirect, BaseModel* schema);
        __declspec(dllexport) virtual ~BasePlugin();

        std::string run(std::string input);
    public:
        inline bool isDirect() const {
            return mIsDirect;
        }
        inline std::string getName() const {
            return mName;
        }
        inline std::string getPrompt() const {
            return mPrompt;
        }
        inline std::string getDescription() const {
            return mDescription;
        }
        inline BaseModel* getSchema() const {
            return mArgsSchema;
        }
        inline bool valid() {
            if (mName.empty())
                return false;

            if (mDescription.empty())
                return false;

            if (mPrompt.empty())
                return false;

            return true;
        }
    private:
        std::string mName;
        std::string mDescription;
        std::string mPrompt;

        bool mIsDirect;

        BaseModel* mArgsSchema;
    };

    class Plugins {
    public:
        Plugins();
        virtual ~Plugins();

        __declspec(dllexport) Plugins* append(BasePlugin* plugin);
        __declspec(dllexport) BasePlugin* getPlugin(std::string name);
        std::list<std::shared_ptr<BasePlugin>> getPlugins() {
            return mPlugins;
        }
    private:
        std::list<std::shared_ptr<BasePlugin>> mPlugins;
    };

    // class PluginsKit {
    // public:
    //     Plugins();
    //     virtual ~Plugins();
    // };

    // class PluginsMgr {
    // public:
    //     PluginsMgr(const char * appid);
    //     virtual ~PluginsMgr();
    //     static Plugins* loadPlugins();
    // };

}

#endif  //SC_PLUGINS_API_H

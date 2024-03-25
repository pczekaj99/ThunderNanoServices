/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 Metrological
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 #include "Module.h"
 #include <interfaces/IStressTest.h>

namespace WPEFramework {
namespace Plugin {
    class StressTest : public PluginHost::IPlugin, public PluginHost::JSONRPC {
        private:
            void SendMessage();
            string CreateMessage();
        
        public:
            StressTest(const StressTest&) = delete;
            StressTest& operator=(const StressTest&) = delete;

            StressTest()
            : _messageSize(0)
            , _sendingInterval(0)
            , _config()
            , _service(nullptr)
            , _minRangeMessageSize(1)
            , _maxRaneMessageSize(10000)
            , _minRangeSendingInterval(1)
            , _maxRangeSendingInterval(30)
            , _job(*this)
            , _implementation(nullptr)
            {
            }

            BEGIN_INTERFACE_MAP(StressTest)
            INTERFACE_ENTRY(PluginHost::IPlugin)
            INTERFACE_ENTRY(PluginHost::IDispatcher)
            INTERFACE_AGGREGATE(Exchange::IStressTest, _implementation)
            END_INTERFACE_MAP

            const string Initialize(PluginHost::IShell* service) override;
            void Deinitialize(PluginHost::IShell* service) override;
            string Information() const override;

            void Dispatch() {
                SendMessage();
            }

        private:
            class Config : public Core::JSON::Container {
                public:
                    Config(const Config&);
                    Config& operator=(const Config&);

                    Config()
                        : Core::JSON::Container()
                        , MessageSize()
                        , SendingInterval()
                        , NumberOfCycles()
                    {
                        Add(_T("messagesize"), &MessageSize);
                        Add(_T("sendinginterval"), &SendingInterval);
                        Add(_T("numberofcycles"), &NumberOfCycles);
                    }

                    ~Config() {}

                public:
                    Core::JSON::DecUInt32 MessageSize;
                    Core::JSON::DecUInt32 SendingInterval;
                    Core::JSON::DecUInt8 NumberOfCycles;
                };

            Config _config;
            Core::WorkerPool::JobType<StressTest&> _job;
            PluginHost::IShell* _service;
            Exchange::IStressTest* _implementation;

            // min and max ranges is for random generator
            uint32_t _minRangeMessageSize;
            uint32_t _maxRaneMessageSize;
            uint8_t _minRangeSendingInterval;
            uint8_t _maxRangeSendingInterval;

            uint32_t _messageSize;
            uint32_t _sendingInterval;
            uint8_t _numberOfCycles;
            uint32_t _connectionId;
    };

} // namespace Plugin
} // namespace WPEframework
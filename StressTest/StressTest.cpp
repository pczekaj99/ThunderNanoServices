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

#include <random>
#include "StressTest.h"

namespace WPEFramework {
namespace Plugin {

    const string Initialize(PluginHost::IShell* service) {
        ASSERT (_service == nullptr);

        std::random_device randomGenerator;
        std::mt19937 rng(randomGenerator());

        _config.FromString(service->ConfigLine());

        if(config.MessageSize.IsSet() == true && config.MessageSize.Value() != 0) {
            _messageSize = config.MessageSize.Value();
        }
        else {
            std::uniform_int_distribution<std::mt19937::result_type> rangeMessageSize(_minRangeMessageSize, _maxRaneMessageSize);
            _messageSize = rangeMessageSize(rng);
        }

        if(config.SendingInterval.IsSet() == true && config.MessageSize.Value() != 0) {
            _sendingInterval = config.SendingInterval.Value();
        }
        else {
            std::uniform_int_distribution<std::mt19937::result_type> rangeSendingInterval(_minRangeSendingInterval, _maxRangeSendingInterval);
            _sendingInterval = rangeSendingInterval(rng);
        }

        if(config.NumberOfCycles.IsSet() == true) {
            _numberOfCycles = config.NumberOfCycles.Value();
        }

        for(int i = 0; i < _numberOfCycles; i++) {
            Core::WorkerPool::Instance().Schedule((Core::Time::Now() + sendingInterval), _job);
        }
    }

    void Deinitialize(PluginHost::IShell* service) {
        ASSERT(service != nullptr);

        Core::IWorkerPool::Instance().Revoke(_job);
    }


    void SendMessage() {
        string newMessage = CreateMessage();
        TRACE(Trace::Information, newMessage);
    }

    string CreateMessage() {
        string message;
        for(int i = 0; i < _messageSize; i++) {
            message += 'X';
        }
    }

} // namespace Plugin
} // namespace WPEframework
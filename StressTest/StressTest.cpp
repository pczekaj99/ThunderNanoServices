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

    namespace {

        static Metadata<StressTest> metadata(
            // Version
            1, 0, 0,
            // Preconditions
            {},
            // Terminations
            {},
            // Controls
            {}
        );
    }

    const string StressTest::Initialize(PluginHost::IShell* service) {
        //ASSERT (service == nullptr);

        TRACE(Trace::Information, (_T("Innit starting")));
        _service = service;
        _service->AddRef();

        _implementation = _service->Root<Exchange::IStressTest>(_connectionId, 2000, _T("StressTestImplementation"));

        TRACE(Trace::Information, (_T("Connected")));

        std::random_device randomGenerator;
        std::mt19937 rng(randomGenerator());

        _config.FromString(_service->ConfigLine());

        if(_config.MessageSize.IsSet() == true && _config.MessageSize.Value() != 0) {
            _messageSize = _config.MessageSize.Value();
        }
        else {
            std::uniform_int_distribution<std::mt19937::result_type> rangeMessageSize(_minRangeMessageSize, _maxRaneMessageSize);
            _messageSize = rangeMessageSize(rng);
        }

        if(_config.SendingInterval.IsSet() == true && _config.MessageSize.Value() != 0) {
            _sendingInterval = _config.SendingInterval.Value();
        }
        else {
            std::uniform_int_distribution<std::mt19937::result_type> rangeSendingInterval(_minRangeSendingInterval, _maxRangeSendingInterval);
            _sendingInterval = rangeSendingInterval(rng);
        }

        if(_config.NumberOfCycles.IsSet() == true) {
            _numberOfCycles = _config.NumberOfCycles.Value();
        }

        TRACE(Trace::Information, (_T("Config done")));

        for(int i = 0; i < _numberOfCycles; i++) {
            //Core::IWorkerPool::Instance().Schedule((Core::Time::Now() + _sendingInterval), _job);
            _job.Submit();
            _job.Reschedule(Core::Time::Now() + _sendingInterval);
        }

        return _T("End");
    }

    void StressTest::Deinitialize(PluginHost::IShell* service) {
        ASSERT(service != nullptr);

        _job.Revoke();
    }

    string StressTest::Information() const
    {
        return string();
    }

    void StressTest::SendMessage() {
        string newMessage = CreateMessage();
        TRACE(Trace::Information, (_T("[%s]"), newMessage.c_str()));
    }

    string StressTest::CreateMessage() {
        string message;
        for(int i = 0; i < _messageSize; i++) {
            message += 'X';
        }
        
        return message;
    }

} // namespace Plugin
} // namespace WPEframework
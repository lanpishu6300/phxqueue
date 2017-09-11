/*
Tencent is pleased to support the open source community by making PhxQueue available.
Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.
Licensed under the BSD 3-Clause License (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

<https://opensource.org/licenses/BSD-3-Clause>

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
*/



/* scheduler_client.cpp

 Generated by phxrpc_pb2client from scheduler.proto

*/

#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>

#include "phxqueue/comm.h"
#include "phxrpc/rpc.h"

#include "scheduler_client_uthread.h"
#include "phxrpc_scheduler_stub.h"


using namespace std;


static phxrpc::ClientConfig global_schedulerclientuthread_config_;
static phxrpc::ClientMonitorPtr global_schedulerclientuthread_monitor_;


bool SchedulerClientUThread::Init(const char *config_file) {
    return global_schedulerclientuthread_config_.Read(config_file);
}

const char *SchedulerClientUThread::GetPackageName() {
    const char *ret = global_schedulerclientuthread_config_.GetPackageName();
    if (strlen(ret) == 0) {
        ret = "phxqueue_phxrpc.scheduler";
    }
    return ret;
}

SchedulerClientUThread::SchedulerClientUThread(phxrpc::UThreadEpollScheduler *uthread_scheduler) {
    uthread_scheduler_ = uthread_scheduler;
    static mutex monitor_mutex;
    if (!global_schedulerclientuthread_monitor_.get()) {
        monitor_mutex.lock();
        if (!global_schedulerclientuthread_monitor_.get()) {
            global_schedulerclientuthread_monitor_ = phxrpc::MonitorFactory::GetFactory()
                ->CreateClientMonitor(GetPackageName());
        }
        global_schedulerclientuthread_config_.SetClientMonitor(global_schedulerclientuthread_monitor_);
        monitor_mutex.unlock();
    }
}

SchedulerClientUThread::~SchedulerClientUThread() {}

int SchedulerClientUThread::PHXEcho(const google::protobuf::StringValue &req,
                                    google::protobuf::StringValue *resp) {
    const phxrpc::Endpoint_t *ep = global_schedulerclientuthread_config_.GetRandom();

    if (uthread_scheduler_ != nullptr && ep != nullptr) {
        phxrpc::UThreadTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(uthread_scheduler_, &socket, ep->ip, ep->port,
                global_schedulerclientuthread_config_.GetConnectTimeoutMS(),
                *(global_schedulerclientuthread_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_schedulerclientuthread_config_.GetSocketTimeoutMS());

            SchedulerStub stub(socket, *(global_schedulerclientuthread_monitor_.get()));
            return stub.PHXEcho(req, resp);
        }
    }

    return -1;
}

int SchedulerClientUThread::PhxBatchEcho(const google::protobuf::StringValue &req,
                                         google::protobuf::StringValue *resp) {
    int ret = -1;
    size_t echo_server_count = 2;
    uthread_begin;
    for (size_t i = 0; i < echo_server_count; i++) {
        uthread_t [=, &uthread_s, &ret](void *) {
            const phxrpc::Endpoint_t *ep = global_schedulerclientuthread_config_.GetByIndex(i);
            if (ep != nullptr) {
                phxrpc::UThreadTcpStream socket;
                if (phxrpc::PhxrpcTcpUtils::Open(&uthread_s, &socket, ep->ip, ep->port,
                        global_schedulerclientuthread_config_.GetConnectTimeoutMS(),
                        *(global_schedulerclientuthread_monitor_.get()))) {
                    socket.SetTimeout(global_schedulerclientuthread_config_.GetSocketTimeoutMS());
                    SchedulerStub stub(socket, *(global_schedulerclientuthread_monitor_.get()));
                    int this_ret = stub.PHXEcho(req, resp);
                    if (this_ret == 0) {
                        ret = this_ret;
                        uthread_s.Close();
                    }
                }
            }
        };
    }
    uthread_end;
    return ret;
}

int SchedulerClientUThread::GetAddrScale(const phxqueue::comm::proto::GetAddrScaleRequest &req,
                                         phxqueue::comm::proto::GetAddrScaleResponse *resp) {
    const phxrpc::Endpoint_t *ep = global_schedulerclientuthread_config_.GetRandom();

    if (uthread_scheduler_ != nullptr && ep != nullptr) {
        phxrpc::UThreadTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(uthread_scheduler_, &socket, ep->ip, ep->port,
                    global_schedulerclientuthread_config_.GetConnectTimeoutMS(),
                    *(global_schedulerclientuthread_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_schedulerclientuthread_config_.GetSocketTimeoutMS());

            SchedulerStub stub(socket, *(global_schedulerclientuthread_monitor_.get()));
            return stub.GetAddrScale(req, resp);
        }
    }

    return -1;
}

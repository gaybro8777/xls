// Copyright 2020 The XLS Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "xls/jit/serial_proc_runtime.h"

#include "absl/status/status.h"
#include "xls/common/status/status_macros.h"
#include "xls/ir/proc.h"
#include "xls/jit/jit_channel_queue.h"
#include "xls/jit/proc_jit.h"

namespace xls {

absl::StatusOr<std::unique_ptr<SerialProcRuntime>> SerialProcRuntime::Create(
    Package* package) {
  auto runtime = absl::WrapUnique(new SerialProcRuntime(std::move(package)));
  XLS_RETURN_IF_ERROR(runtime->Init());
  return runtime;
}

SerialProcRuntime::SerialProcRuntime(Package* package) : package_(package) {}

absl::Status SerialProcRuntime::Init() {
  // Create a ProcJit and continuation for each proc.
  for (const std::unique_ptr<Proc>& proc : package_->procs()) {
    XLS_ASSIGN_OR_RETURN(std::unique_ptr<OrcJit> orc_jit, OrcJit::Create());
    if (queue_mgr_ == nullptr) {
      XLS_ASSIGN_OR_RETURN(queue_mgr_, JitChannelQueueManager::CreateThreadSafe(
                                           package_, orc_jit.get()));
      jit_runtime_ = std::make_unique<JitRuntime>(orc_jit->GetDataLayout(),
                                                  &orc_jit->GetTypeConverter());
    }

    XLS_ASSIGN_OR_RETURN(
        proc_jits_[proc.get()],
        ProcJit::Create(proc.get(), queue_mgr_.get(), std::move(orc_jit)));
    continuations_[proc.get()] = proc_jits_.at(proc.get())->NewContinuation();
  }

  // Enqueue initial values into channels.
  for (Channel* channel : package_->channels()) {
    for (const Value& value : channel->initial_values()) {
      XLS_RETURN_IF_ERROR(EnqueueValueToChannel(channel, value));
    }
  }
  return absl::OkStatus();
}

absl::Status SerialProcRuntime::Tick(bool print_traces) {
  bool progress_made = true;
  absl::flat_hash_set<Proc*> completed_procs;
  // In round-robin fashion, run each proc until every proc has either completed
  // their tick or are blocked on receives.
  while (progress_made) {
    progress_made = false;
    for (const std::unique_ptr<Proc>& proc : package_->procs()) {
      if (completed_procs.contains(proc.get())) {
        continue;
      }
      XLS_ASSIGN_OR_RETURN(
          TickResult result,
          proc_jits_.at(proc.get())->Tick(*continuations_.at(proc.get())));
      progress_made = progress_made || result.progress_made;
      if (result.tick_complete) {
        completed_procs.insert(proc.get());
      }
    }
  }
  return absl::OkStatus();
}

absl::Status SerialProcRuntime::EnqueueValueToChannel(Channel* channel,
                                                      const Value& value) {
  return queue_mgr()->GetQueue(channel).Enqueue(value);
}

absl::Status SerialProcRuntime::EnqueueBufferToChannel(
    Channel* channel, absl::Span<uint8_t const> buffer) {
  JitChannelQueue& queue = queue_mgr()->GetJitQueue(channel);
  queue.EnqueueRaw(buffer.data());
  return absl::OkStatus();
}

absl::StatusOr<std::optional<Value>> SerialProcRuntime::DequeueValueFromChannel(
    Channel* channel) {
  return queue_mgr()->GetQueue(channel).Dequeue();
}

absl::StatusOr<bool> SerialProcRuntime::DequeueBufferFromChannel(
    Channel* channel, absl::Span<uint8_t> buffer) {
  return queue_mgr()->GetJitQueue(channel).DequeueRaw(buffer.data());
}

absl::StatusOr<std::vector<Value>>
SerialProcRuntime::SerialProcRuntime::ProcState(Proc* proc) const {
  return continuations_.at(proc)->GetState();
}

void SerialProcRuntime::ResetState() {
  for (const std::unique_ptr<Proc>& proc : package_->procs()) {
    continuations_.at(proc.get()) =
        proc_jits_.at(proc.get())->NewContinuation();
  }
}

}  // namespace xls

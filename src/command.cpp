#include "command.h"

/* Copyright (c) 2018, Jack Deeth github@jackdeeth.org.uk
// All rights reserved
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those
// of the authors and should not be interpreted as representing official policies,
// either expressed or implied, of the FreeBSD Project.
*/

PPL::Command::Command(XPLMCommandRef ref,
                      std::function<Outcome(XPLMCommandRef, Phase)> cb,
                      bool run_before_sim)
    : callback(cb), ref_(ref), before_(run_before_sim ? 1 : 0) {
  XPLMRegisterCommandHandler(ref_, scb, before_, this);
}

PPL::Command::Command(std::string cmd_to_replace,
                      std::function<Outcome(XPLMCommandRef, Phase)> callback,
                      bool run_before)
    : Command(XPLMFindCommand(cmd_to_replace.c_str()), callback, run_before) {}

PPL::Command::Command(std::string new_cmd,
                      std::string description,
                      std::function<Outcome(XPLMCommandRef, Phase)> callback,
                      bool run_before)
    : Command(XPLMCreateCommand(new_cmd.c_str(), description.c_str()),
              callback,
              run_before) {}

PPL::Command::~Command() {
  XPLMUnregisterCommandHandler(ref_, scb, before_, this);
}

PPL::Command::Phase PPL::Command::phase() const { return phase_; }

int PPL::Command::scb(XPLMCommandRef ref, XPLMCommandPhase phase, void *vp) {
  auto cmd = reinterpret_cast<Command *>(vp);
  if (phase == xplm_CommandBegin) cmd->phase_ = Phase::Begin;
  if (phase == xplm_CommandContinue) cmd->phase_ = Phase::Continue;
  if (phase == xplm_CommandEnd) cmd->phase_ = Phase::End;
  auto outcome = cmd->callback(ref, cmd->phase_);
  return outcome == Outcome::Halt ? 1 : 0;
}

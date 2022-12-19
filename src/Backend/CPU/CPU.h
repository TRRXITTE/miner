// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <atomic>

#include "Backend/IBackend.h"
#include "Types/JobSubmit.h"

class CPU : virtual public IBackend
{
  public:
    CPU(
        const std::shared_ptr<HardwareConfig> &hardwareConfig,
        const std::function<void(const JobSubmit &jobSubmit)> &submitHashCallback);

    virtual void start(const Job &job, const uint32_t initialNonce);

    virtual void stop();

    virtual void setNewJob(const Job &job, const uint32_t initialNonce);

    virtual std::vector<PerformanceStats> getPerformanceStats();

  private:

    void hash(const uint32_t threadNumber);

    /* Current job to be working on */
    Job m_currentJob;

    /* Nonce to begin hashing at */
    uint32_t m_nonce;

    /* Should we stop the worker funcs */
    std::atomic<bool> m_shouldStop = false;

    /* Threads to launch, whether CPU/GPU is enabled, etc */
    std::shared_ptr<HardwareConfig> m_hardwareConfig;

    /* Worker threads */
    std::vector<std::thread> m_threads;

    /* A bool for each thread indicating if they should swap to a new job */
    std::vector<bool> m_newJobAvailable;

    /* Used to submit a hash back to the miner manager */
    const std::function<void(const JobSubmit &jobSubmit)> m_submitHash;
};

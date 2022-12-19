// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

////////////////////////////////////
#include "MinerManager/HashManager.h"
////////////////////////////////////

#include <iostream>
#include <sstream>

#include "Utilities/ColouredMsg.h"

HashManager::HashManager(
    const std::shared_ptr<PoolCommunication> pool):
    m_pool(pool)
{
}

bool isHashValidForTarget(
    const uint8_t *hash,
    const uint64_t target)
{
    return *reinterpret_cast<const uint64_t *>(hash + 24) < target;
}

void HashManager::incrementHashesPerformed(
    const uint32_t hashesPerformed,
    const std::string &device)
{
    if (m_totalHashes == 0)
    {
        m_effectiveStartTime = std::chrono::high_resolution_clock::now();
    }

    m_hashProducers[device].totalHashes += hashesPerformed;

    m_totalHashes += hashesPerformed;
}

void HashManager::submitValidHash(const JobSubmit &jobSubmit)
{
    m_submittedHashes++;
    m_pool->submitShare(jobSubmit.hash, jobSubmit.jobID, jobSubmit.nonce);
}

void HashManager::submitHash(const JobSubmit &jobSubmit)
{
    incrementHashesPerformed(1, jobSubmit.hardwareIdentifier);

    if (isHashValidForTarget(jobSubmit.hash, jobSubmit.target))
    {
        submitValidHash(jobSubmit);
    }
}

void HashManager::shareAccepted()
{
    /* Sometimes the pool randomly sends us a share accepted message... even
       when we haven't submitted any shares. Why? Who knows! */
    if (m_totalHashes == 0 || m_submittedHashes == 0)
    {
        return;
    }

    m_acceptedHashes++;

    m_pool->printPool();

    std::cout << SuccessMsg("Share accepted by pool!");

    std::stringstream stream;

    uint64_t accepted = m_acceptedHashes;
    const uint64_t sent = m_submittedHashes;

    /* Pools sometimes send double accepted messages */
    if (accepted > sent)
    {
        return;
    }

    stream << " [" << accepted << " / " << sent << "]" << std::endl;

    std::cout << InformationMsg(stream.str());
}

void HashManager::printStats()
{
    const auto elapsedTime = std::chrono::high_resolution_clock::now() - m_effectiveStartTime;

    /* Calculating in milliseconds for more accuracy */
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();

    for (const auto &[device, hashes] : m_hashProducers)
    {
        m_pool->printPool();

        std::cout << WhiteMsg(device, 20);

        if (milliseconds != 0 && hashes.totalHashes != 0)
        {
            const double hashratePerSecond = (1000 * static_cast<double>(hashes.totalHashes) / milliseconds);

            std::cout << std::fixed << std::setprecision(2) << "| "
                      << WhiteMsg(hashratePerSecond) << WhiteMsg(" H/s") << std::endl;
        }
        else
        {
            std::cout << WhiteMsg("N/A") << std::endl;
        }
    }

    if (m_hashProducers.size() > 1)
    {
        m_pool->printPool();

        std::cout << WhiteMsg("Total Hashrate", 20);

        if (milliseconds != 0 && m_totalHashes != 0)
        {
            const double hashratePerSecond = (1000 * static_cast<double>(m_totalHashes) / milliseconds);

            std::cout << std::fixed << std::setprecision(2) << "| "
                      << WhiteMsg(hashratePerSecond) << WhiteMsg(" H/s") << std::endl;
        }
        else
        {
            std::cout << WhiteMsg("N/A") << std::endl;
        }
    }

    double submitPercentage = 0;

    if (m_acceptedHashes != 0 && m_submittedHashes != 0)
    {
        submitPercentage = 100 * (static_cast<double>(m_acceptedHashes) / m_submittedHashes);

        if (submitPercentage > 100)
        {
            submitPercentage = 100;
        }
    }

    m_pool->printPool();

    std::cout << WhiteMsg("Accepted Shares", 20)
              << std::fixed << std::setprecision(2)
              << "| "
              << WhiteMsg(submitPercentage) << WhiteMsg("%") << std::endl;
}

void HashManager::start()
{
    if (m_paused)
    {
        const auto pauseDuration = std::chrono::high_resolution_clock::now() - m_pauseTime;
        m_effectiveStartTime += pauseDuration;
    }

    m_paused = false;
}

void HashManager::pause()
{
    m_paused = true;
    m_pauseTime = std::chrono::high_resolution_clock::now();
}

void HashManager::resetShareCount()
{
    m_submittedHashes = 0;
    m_acceptedHashes = 0;
}

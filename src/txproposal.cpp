///////////////////////////////////////////////////////////////////////////////
//
// CoinSocket
//
// txproposal.h
//
// Copyright (c) 2015 Eric Lombrozo
//
// All Rights Reserved.
//

#include "txproposal.h"

#include <stdutils/uchar_vector.h>

#include <CoinCore/hash.h>

#include <mutex>

using namespace CoinSocket;
using namespace std;

static mutex g_mutex;
static txproposals_t g_txProposals;

void TxProposal::setHash() const
{
    // TODO: Better serialization resistant to malleability
    uchar_vector serialized;
    uint64_t v;

    for (auto c: username_)     { serialized.push_back((unsigned char)c); }
    serialized.push_back(0x00);

    for (auto c: account_)      { serialized.push_back((unsigned char)c); }
    serialized.push_back(0x00);

    for (auto& txout: txouts_)
    {
        for (auto c: txout->sending_label()) { serialized.push_back((unsigned char)c); }
        serialized.push_back(0x00);

        serialized += txout->script();

        v = txout->value();
        serialized.push_back((unsigned char)((v >> 56) & 0xff));
        serialized.push_back((unsigned char)((v >> 48) & 0xff));
        serialized.push_back((unsigned char)((v >> 40) & 0xff));
        serialized.push_back((unsigned char)((v >> 32) & 0xff));
        serialized.push_back((unsigned char)((v >> 24) & 0xff));
        serialized.push_back((unsigned char)((v >> 16) & 0xff));
        serialized.push_back((unsigned char)((v >> 8) & 0xff));
        serialized.push_back((unsigned char)(v & 0xff));
    }

    v = fee_;
    serialized.push_back((unsigned char)((v >> 56) & 0xff));
    serialized.push_back((unsigned char)((v >> 48) & 0xff));
    serialized.push_back((unsigned char)((v >> 40) & 0xff));
    serialized.push_back((unsigned char)((v >> 32) & 0xff));
    serialized.push_back((unsigned char)((v >> 24) & 0xff));
    serialized.push_back((unsigned char)((v >> 16) & 0xff));
    serialized.push_back((unsigned char)((v >> 8) & 0xff));
    serialized.push_back((unsigned char)(v & 0xff));

    v = timestamp_;
    serialized.push_back((unsigned char)((v >> 56) & 0xff));
    serialized.push_back((unsigned char)((v >> 48) & 0xff));
    serialized.push_back((unsigned char)((v >> 40) & 0xff));
    serialized.push_back((unsigned char)((v >> 32) & 0xff));
    serialized.push_back((unsigned char)((v >> 24) & 0xff));
    serialized.push_back((unsigned char)((v >> 16) & 0xff));
    serialized.push_back((unsigned char)((v >> 8) & 0xff));
    serialized.push_back((unsigned char)(v & 0xff));
        
    hash_ = sha256_2(serialized);
}

void CoinSocket::addTxProposal(std::shared_ptr<TxProposal> txProposal)
{
    lock_guard<mutex> lock(g_mutex);
 
    auto it = g_txProposals.find(txProposal->hash());
    if (it != g_txProposals.end()) return;

    g_txProposals[txProposal->hash()] = txProposal;
}

std::shared_ptr<TxProposal> CoinSocket::getTxProposal(const bytes_t& hash)
{
    lock_guard<mutex> lock(g_mutex);

    auto it = g_txProposals.find(hash);
    if (it == g_txProposals.end()) throw runtime_error("Transaction proposal not found.");

    return it->second;
}

void CoinSocket::eraseTxProposal(const bytes_t& hash)
{
    lock_guard<mutex> lock(g_mutex);

    auto it = g_txProposals.find(hash);
    if (it == g_txProposals.end()) throw runtime_error("Transaction proposal not found.");

    g_txProposals.erase(hash);
}

void CoinSocket::clearTxProposals()
{
    lock_guard<mutex> lock(g_mutex);

    g_txProposals.clear();
}

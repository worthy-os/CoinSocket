///////////////////////////////////////////////////////////////////////////////
//
// CoinSocket
//
// jsonobjects.cpp
//
// Copyright (c) 2014 Eric Lombrozo
//
// All Rights Reserved.
//

#include "jsonobjects.h"
#include <CoinDB/Schema.h>
#include <CoinDB/SigningRequest.h>
#include <CoinDB/SynchedVault.h>
#include <stdutils/uchar_vector.h>

using namespace CoinDB;
using namespace json_spirit;

Object getSyncStatusObject(const SynchedVault& synchedVault)
{
    Object result;
    result.push_back(Pair("status", SynchedVault::getStatusString(synchedVault.getStatus())));
    result.push_back(Pair("syncheight", (uint64_t)synchedVault.getSyncHeight()));
    result.push_back(Pair("synchash", uchar_vector(synchedVault.getSyncHash()).getHex()));
    result.push_back(Pair("bestheight", (uint64_t)synchedVault.getBestHeight()));
    result.push_back(Pair("besthash", uchar_vector(synchedVault.getBestHash()).getHex()));
    return result;
}

Object getBlockHeaderObject(BlockHeader* header)
{
    Object result;
    result.push_back(Pair("hash", uchar_vector(header->hash()).getHex()));
    result.push_back(Pair("height", (uint64_t)header->height()));
    result.push_back(Pair("version", (uint64_t)header->version()));
    result.push_back(Pair("prevhash", uchar_vector(header->prevhash()).getHex()));
    result.push_back(Pair("merkleroot", uchar_vector(header->merkleroot()).getHex()));
    result.push_back(Pair("timestamp", (uint64_t)header->timestamp()));
    result.push_back(Pair("bits", (uint64_t)header->bits()));
    result.push_back(Pair("nonce", (uint64_t)header->nonce()));
    return result;
}

Object getKeychainObject(Keychain* keychain)
{
    Object result;
    result.push_back(Pair("id", (uint64_t)keychain->id()));
    result.push_back(Pair("name", keychain->name()));
    result.push_back(Pair("depth", (int)keychain->depth()));
    result.push_back(Pair("parentfp", (uint64_t)keychain->parent_fp()));
    result.push_back(Pair("childnum", (uint64_t)keychain->child_num()));
    result.push_back(Pair("pubkey", uchar_vector(keychain->pubkey()).getHex()));
    result.push_back(Pair("hash", uchar_vector(keychain->hash()).getHex()));
    return result;
}

Object getAccountInfoObject(const AccountInfo& accountInfo)
{
    Object result;
    result.push_back(Pair("id", (uint64_t)accountInfo.id()));
    result.push_back(Pair("name", accountInfo.name()));
    result.push_back(Pair("minsigs", (int)accountInfo.minsigs()));
    result.push_back(Pair("keychains", Array(accountInfo.keychain_names().begin(), accountInfo.keychain_names().end())));
    result.push_back(Pair("unusedpoolsize", (uint64_t)accountInfo.unused_pool_size()));
    result.push_back(Pair("timecreated", (uint64_t)accountInfo.time_created()));
    result.push_back(Pair("bins", Array(accountInfo.bin_names().begin(), accountInfo.bin_names().end())));
    return result;
}

Object getTxViewObject(const TxView& txview)
{
    bytes_t hash = txview.status == CoinDB::Tx::UNSIGNED
        ? txview.unsigned_hash : txview.hash;

    Object result;
    result.push_back(Pair("id", (uint64_t)txview.id));
    result.push_back(Pair("hash", uchar_vector(hash).getHex()));
    result.push_back(Pair("status", CoinDB::Tx::getStatusString(txview.status, true)));
    result.push_back(Pair("version", (uint64_t)txview.version));
    result.push_back(Pair("locktime", (uint64_t)txview.locktime));
    result.push_back(Pair("timestamp", (uint64_t)txview.timestamp));
    result.push_back(Pair("height", (uint64_t)txview.height));
    return result;
}

Object getSigningRequestObject(const SigningRequest& req)
{
    std::vector<std::string> keychain_names;
    std::vector<std::string> keychain_hashes;
    for (auto& keychain_pair: req.keychain_info())
    {
        keychain_names.push_back(keychain_pair.first);
        keychain_hashes.push_back(uchar_vector(keychain_pair.second).getHex());
    }
    std::string hash = uchar_vector(req.hash()).getHex();
    std::string rawtx = uchar_vector(req.rawtx()).getHex();

    Object result;
    result.push_back(Pair("hash", hash));
    result.push_back(Pair("sigsneeded", (uint64_t)req.sigs_needed()));
    result.push_back(Pair("keychains", Array(keychain_names.begin(), keychain_names.end())));
    result.push_back(Pair("keychainhashes", Array(keychain_hashes.begin(), keychain_hashes.end())));
    result.push_back(Pair("rawtx", rawtx));
    return result;
}

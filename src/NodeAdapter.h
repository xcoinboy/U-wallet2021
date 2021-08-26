// Copyright (c) 2011-2015 The Cryptonote developers
// Copyright (c) 2016-2020 The UrbanCash developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <QObject>
#include <QThread>

#include <INode.h>
#include <IWalletLegacy.h>
#include "CryptoNoteWrapper.h"

namespace CryptoNote {

class Currency;

}

namespace Logging {
  class LoggerManager;
}

namespace WalletGui {

class InProcessNodeInitializer;

class NodeAdapter : public QObject, public INodeCallback {
  Q_OBJECT
  Q_DISABLE_COPY(NodeAdapter)

public:
  static NodeAdapter& instance();

  quintptr getPeerCount();
  std::string convertPaymentId(const QString& _payment_id_string) const;
  QString extractPaymentId(const std::string& _extra) const;
  CryptoNote::IWalletLegacy* createWallet() const;

  bool init();
  void deinit();
  quint64 getLastKnownBlockHeight() const;
  quint64 getLastLocalBlockHeight() const;
  QDateTime getLastLocalBlockTimestamp() const;
  quint64 getDifficulty();
  quint64 getTxCount();
  quint64 getTxPoolSize();
  quint64 getAltBlocksCount();
  quint64 getConnectionsCount();
  quint64 getOutgoingConnectionsCount();
  quint64 getIncomingConnectionsCount();
  quint64 getWhitePeerlistSize();
  quint64 getGreyPeerlistSize();
  quint64 getMinimalFee() const;
  quint64 getNodeFeeAmount() const;
  QString getNodeFeeAddress() const;
  uint8_t getCurrentBlockMajorVersion();
  quint64 getAlreadyGeneratedCoins();
  std::vector<CryptoNote::p2pConnection> getConnections();
  CryptoNote::BlockHeaderInfo getLastLocalBlockHeaderInfo();
  void peerCountUpdated(Node& _node, size_t _count) Q_DECL_OVERRIDE;
  void localBlockchainUpdated(Node& _node, uint64_t _height) Q_DECL_OVERRIDE;
  void lastKnownBlockHeightUpdated(Node& _node, uint64_t _height) Q_DECL_OVERRIDE;
  void connectionStatusUpdated(bool _connected) Q_DECL_OVERRIDE;
  bool isOffline();

private:
  Node* m_node;
  QThread m_nodeInitializerThread;
  InProcessNodeInitializer* m_nodeInitializer;

  NodeAdapter();
  ~NodeAdapter();

  bool initInProcessNode();
  CryptoNote::CoreConfig makeCoreConfig() const;
  CryptoNote::NetNodeConfig makeNetNodeConfig() const;

Q_SIGNALS:
  void localBlockchainUpdatedSignal(quint64 _height);
  void lastKnownBlockHeightUpdatedSignal(quint64 _height);
  void nodeInitCompletedSignal();
  void peerCountUpdatedSignal(quintptr _count);
  void initNodeSignal(Node** _node, const CryptoNote::Currency* currency, INodeCallback* _callback, Logging::LoggerManager* _loggerManager,
    const CryptoNote::CoreConfig& _coreConfig, const CryptoNote::NetNodeConfig& _netNodeConfig);
  void deinitNodeSignal(Node** _node);
  void connectionFailedSignal();
  void connectionStatusUpdatedSignal(bool _connected);
};

}

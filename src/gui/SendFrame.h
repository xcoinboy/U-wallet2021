// Copyright (c) 2011-2016 The Cryptonote developers
// Copyright (c) 2015-2016 XDN developers
// Copyright (c) 2016-2021 The Urbancash developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <QFrame>

#include <IWallet.h>
#include <IWalletLegacy.h>
#include "SendGlassFrame.h"

namespace Ui {
  class SendFrame;
}

namespace WalletGui {

class TransferFrame;

class SendFrame : public QFrame {
  Q_OBJECT
  Q_DISABLE_COPY(SendFrame)

public:
  SendFrame(QWidget* _parent);
  ~SendFrame();

  void setAddress(const QString& _address);
  Q_SLOT void parsePaymentRequest(QString _request);
  Q_SLOT void sendOutputs(QList<CryptoNote::TransactionOutputInformation> _selectedOutputs);

private:
  QScopedPointer<Ui::SendFrame> m_ui;
  QList<TransferFrame*> m_transfers;
  SendGlassFrame* m_glassFrame;

  QString m_nodeFeeAddress;
  quint64 m_nodeFee = 0;
  quint64 m_flatRateNodeFee = 0;
  quint64 m_totalAmount = 0;
  quint64 m_unmixableBalance = 0;
  quint64 m_selectedOutputsAmount = 0;
  QList<CryptoNote::TransactionOutputInformation> m_selectedOutputs;

  void sendTransactionCompleted(CryptoNote::TransactionId _id, bool _error, const QString& _error_text);
  void walletActualBalanceUpdated(quint64 _balance);
  void walletSynchronized(int _error, const QString& _error_text);
  void walletSynchronizationInProgress(quint64 _current, quint64 _total);
  void insertPaymentID(QString _paymentid);
  static bool isValidPaymentId(const QByteArray& _paymentIdString);
  void onNodeFeeAddressFound(const QString& _address, const quint64 _feeAmount);
  double getMinimalFee();
  quint64 getFee();
  void calculateNodeFee();
  void recalculateAmountsSendOutputs();
  void reset();
  bool confirmZeroMixin();

  Q_SLOT void addRecipientClicked();
  Q_SLOT void clearAllClicked();
  Q_SLOT void mixinValueChanged(int _value);
  Q_SLOT void priorityValueChanged(int _value);
  Q_SLOT void feeValueChanged(double _value);
  Q_SLOT void donateValueChanged(double _value);
  Q_SLOT void amountValueChanged();
  Q_SLOT void sendClicked();
  Q_SLOT void sendAllClicked();
  Q_SLOT void openUriClicked();
  Q_SLOT void generatePaymentIdClicked();
  Q_SLOT void advancedClicked(bool _show);
  Q_SLOT void dontRelayToggled(bool _dont);
  Q_SLOT void donateToggled(bool _donate);
  Q_SLOT void feeOverrideToggled(bool _override);

Q_SIGNALS:
  void uriOpenSignal();

};

}

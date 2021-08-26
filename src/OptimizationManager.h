// Copyright (c) 2015-2017, The Bytecoin developers
// Copyright (c) 2018, The Urbancash developers
//
// This file is part of Urbancashvanets.
//
// Urbancashvanets is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Urbancashvanets is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Urbancashvanets.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <QObject>

#include "CryptoNoteWrapper.h"
#include "CurrencyAdapter.h"
#include "WalletAdapter.h"

namespace WalletGui {

class OptimizationManager : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(OptimizationManager)

public:
  OptimizationManager(QObject *_parent);
  ~OptimizationManager();

  void checkOptimization();

  Q_SLOT void walletOpened();
  Q_SLOT void walletClosed();
  Q_SLOT void synchronizationProgressUpdated();
  Q_SLOT void synchronizationCompleted();

protected:
  virtual void timerEvent(QTimerEvent* _event);

private:
  int m_checkTimerId;
  int m_optimizationTimerId;
  quint64 m_currentOptimizationInterval;
  bool m_isSynchronized;

  void optimize();
  void ensureStarted();
  void ensureStopped();

};

}

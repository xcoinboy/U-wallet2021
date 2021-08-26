// Copyright (c) 2011-2016 The Cryptonote developers
// Copyright (c) 2015-2016 XDN developers
// Copyright (c) 2016-2021 Urbancash developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <QSortFilterProxyModel>

namespace WalletGui {

class VisibleOutputsModel : public QSortFilterProxyModel {
  Q_OBJECT

public:
  VisibleOutputsModel();
  ~VisibleOutputsModel();

protected:
  bool filterAcceptsColumn(int _sourceColumn, const QModelIndex& _sourceParent) const Q_DECL_OVERRIDE;
};

}

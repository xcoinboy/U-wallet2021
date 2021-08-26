// Copyright (c) 2015-2017, The Bytecoin developers
// Copyright (c) 2017-2021, The Urbancash developers
//
// This file is part of Urbancash.
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

#include <QDialog>

namespace Ui {
class QRCodeDialog;
}

namespace WalletGui {

class QRCodeDialog : public QDialog {
  Q_OBJECT
  Q_DISABLE_COPY(QRCodeDialog)

public:
  QRCodeDialog(const QString& _title, const QString& _text, QWidget* _parent);
  ~QRCodeDialog();

private:
  QScopedPointer<Ui::QRCodeDialog> m_ui;

  Q_SLOT void saveQRcodeToFile();
};

}

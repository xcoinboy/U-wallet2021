// Copyright (c) 2011-2015 The Cryptonote developers
// Copyright (c) 2016-2020 The UrbanCash developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <QApplication>
#include <QCommandLineParser>
#include <QLocale>
#include <QTranslator>
#include <QLockFile>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QSplashScreen>
#include <QStyleFactory>
#include <QSettings>
#include <QTextCodec>

#include "CommandLineParser.h"
#include "CurrencyAdapter.h"
#include "LoggerAdapter.h"
#include "NodeAdapter.h"
#include "Settings.h"
#include "SignalHandler.h"
#include "WalletAdapter.h"
#include "gui/MainWindow.h"
#include "Update.h"
#include "PaymentServer.h"
#include "TranslatorManager.h"
#include "LogFileWatcher.h"

#define DEBUG 1

using namespace WalletGui;

const QRegularExpression LOG_SPLASH_REG_EXP("\\] ");

QSplashScreen* splash(nullptr);

inline void newLogString(const QString& _string) {
  QRegularExpressionMatch match = LOG_SPLASH_REG_EXP.match(_string);
  if (match.hasMatch()) {
    QString message = _string.mid(match.capturedEnd());
    splash->showMessage(message, Qt::AlignLeft | Qt::AlignBottom, Qt::white);
  }
}

int main(int argc, char* argv[]) {

  QApplication app(argc, argv);
  app.setApplicationName(CurrencyAdapter::instance().getCurrencyName() + "wallet");
  app.setApplicationVersion(Settings::instance().getVersion());
  app.setQuitOnLastWindowClosed(false);

#ifndef Q_OS_MAC
  QApplication::setStyle(QStyleFactory::create("Fusion"));
#endif

  CommandLineParser cmdLineParser(nullptr);
  Settings::instance().setCommandLineParser(&cmdLineParser);
  bool cmdLineParseResult = cmdLineParser.process(app.arguments());
  Settings::instance().load();

  //Translator must be created before the application's widgets.
  TranslatorManager* tmanager = TranslatorManager::instance();
  Q_UNUSED(tmanager)

  setlocale(LC_ALL, "");

  QFile File1(":/qdarkstyle/style.qss");
  File1.open(QFile::ReadOnly);
  QString StyleSheet1 = QLatin1String(File1.readAll());

  QFile File2(":/skin/dark.qss");
  File2.open(QFile::ReadOnly);
  QString StyleSheet2 = QLatin1String(File2.readAll());

  // fix font sizes for MacOS
  const char MAC_FIX_STYLE_SHEET[] = "QWidget{font-size:12px}";
#ifdef Q_OS_MAC
  qApp->setStyleSheet(MAC_FIX_STYLE_SHEET + StyleSheet1 + StyleSheet2);
#else
  qApp->setStyleSheet(StyleSheet1 + StyleSheet2);
#endif

  if (PaymentServer::ipcSendCommandLine())
  exit(0);

  PaymentServer* paymentServer = new PaymentServer(&app);

#ifdef Q_OS_WIN
  if(!cmdLineParseResult) {
    QMessageBox::critical(nullptr, QObject::tr("Error"), cmdLineParser.getErrorText());
    return app.exec();
  } else if (cmdLineParser.hasHelpOption()) {
    QMessageBox::information(nullptr, QObject::tr("Help"), cmdLineParser.getHelpText());
    return app.exec();
  }

  //Create registry entries for URL execution
  QSettings urbancashKey("HKEY_CLASSES_ROOT\\urbancash", QSettings::NativeFormat);
  urbancashKey.setValue(".", "Urbancash Wallet");
  urbancashKey.setValue("URL Protocol", "");
  QSettings urbancashOpenKey("HKEY_CLASSES_ROOT\\urbancash\\shell\\open\\command", QSettings::NativeFormat);
  urbancashOpenKey.setValue(".", "\"" + QCoreApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");
#endif

#if defined(Q_OS_LINUX)
  QStringList args;
  QProcess exec;

  //as root
  args << "-c" << "printf '[Desktop Entry]\\nName = Urbancash URL Handler\\nGenericName = Urbancash\\nComment = Handle URL Sheme urbancash://\\nExec = " + QCoreApplication::applicationFilePath() + " %%u\\nTerminal = false\\nType = Application\\nMimeType = x-scheme-handler/urbancash;\\nIcon = Urbancash-Wallet' | tee /usr/share/applications/urbancash-handler.desktop";
  exec.start("/bin/sh", args);
  exec.waitForFinished();

  args.clear();
  args << "-c" << "update-desktop-database";
  exec.start("/bin/sh", args);
  exec.waitForFinished();
#endif

  LoggerAdapter::instance().init();

  QString dataDirPath = Settings::instance().getDataDir().absolutePath();

  if (!QDir().exists(dataDirPath)) {
    QDir().mkpath(dataDirPath);
  }

  QLockFile lockFile(Settings::instance().getDataDir().absoluteFilePath(QApplication::applicationName() + ".lock"));
  if (!lockFile.tryLock()) {
    QMessageBox::warning(nullptr, QObject::tr("Fail"), QObject::tr("%1 wallet already running or cannot create lock file %2. Check your permissions.").arg(CurrencyAdapter::instance().getCurrencyDisplayName()).arg(Settings::instance().getDataDir().absoluteFilePath(QApplication::applicationName() + ".lock")));
    return 0;
  }

  SignalHandler::instance().init();
  QObject::connect(&SignalHandler::instance(), &SignalHandler::quitSignal, &app, &QApplication::quit);

  if (splash == nullptr) {
    splash = new QSplashScreen(QPixmap(":images/splash"), Qt::X11BypassWindowManagerHint);
  }

  if (!splash->isVisible()) {
    splash->show();
  }

  splash->showMessage(QObject::tr("Loading blockchain..."), Qt::AlignLeft | Qt::AlignBottom, Qt::white);

  LogFileWatcher* logWatcher(nullptr);
  if (logWatcher == nullptr) {
    logWatcher = new LogFileWatcher(Settings::instance().getDataDir().absoluteFilePath(QCoreApplication::applicationName() + ".log"), &app);
    QObject::connect(logWatcher, &LogFileWatcher::newLogStringSignal, &app, &newLogString);
  }

  app.processEvents();
  qRegisterMetaType<CryptoNote::TransactionId>("CryptoNote::TransactionId");
  qRegisterMetaType<QList<CryptoNote::TransactionOutputInformation>>("QList<CryptoNote::TransactionOutputInformation>");
  qRegisterMetaType<quintptr>("quintptr");
  if (!NodeAdapter::instance().init()) {
    return 0;
  }

  splash->finish(&MainWindow::instance());

  if (logWatcher != nullptr) {
    logWatcher->deleteLater();
    logWatcher = nullptr;
  }

  splash->deleteLater();
  splash = nullptr;

  Updater *d = new Updater();
  d->checkForUpdate();

  MainWindow::instance().show();
  WalletAdapter::instance().open("");

  QTimer::singleShot(1000, paymentServer, SLOT(uiReady()));
  QObject::connect(paymentServer, &PaymentServer::receivedURI, &MainWindow::instance(), &MainWindow::handlePaymentRequest, Qt::QueuedConnection);

  QObject::connect(QApplication::instance(), &QApplication::aboutToQuit, []() {
    MainWindow::instance().quit();
    if (WalletAdapter::instance().isOpen()) {
      WalletAdapter::instance().close();
    }

    NodeAdapter::instance().deinit();
  });

  return app.exec();
}

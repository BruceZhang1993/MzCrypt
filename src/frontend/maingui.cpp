#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <QtGui>

#include "maingui.h"
#include "cryptoworker.h"


// XXX Qt 4.8 bug: locale->quoteString("foo")
//                 inserts wrongly encoded quote characters
//                 into rich text QString
#if HAVE_QT_QUOTESTRING
#  define QUOTE_STRING(x) locale->quoteString(x)
#  define QUOTE_STRING_LITERAL(x) locale->quoteString(x)
#else
#  define QUOTE_STRING(x) "\"" + x + "\""
#  define QUOTE_STRING_LITERAL(x) "\"" x "\""
#endif


Main_GUI::Main_GUI()
{
  create_layout();
  create_connections();
  create_actions();
  create_menus();
  create_status_bar();

  read_settings();

  setUnifiedTitleAndToolBarOnMac(true);
}


// overloading

void
Main_GUI::closeEvent(QCloseEvent* event)
{
  write_settings();
  event->accept();
}


void
Main_GUI::about()
{
  QMessageBox::about(this,
                     tr("About MzCrypt-Gui"),
                     tr("<p>This is <b>MzCrypt-Gui</b> version 1.0.3 MeizuMX3 Edition"
                        " based on the <b>TTFautohint frontend</b>.<br>"
                        "Please refer to the <a href='http://www.freetype.org/GPL.TXT'>GNU"
                        " GPLv2</a> for terms of redistribution.</p>"
                        "<p>Visit <a href='http://www.freetype.org/ttfautohint/'>http://www.freetype.org/ttfautohint/</a>"
                        " for more information about TTFautohint, and"
                        " <a href='http://www.meizume.com/'>MeizuMe</a> for support about this tool.</p>"));
}


void
Main_GUI::browse_input()
{
  QString file = QFileDialog::getOpenFileName(
                   this,
                   tr("Open Input File"),
                   QDir::homePath(),
                   "");
  if (!file.isEmpty())
    input_line->setText(QDir::toNativeSeparators(file));
}


void
Main_GUI::browse_output()
{
  QString file = QFileDialog::getSaveFileName(
                   this,
                   tr("Open Output File"),
                   QDir::homePath(),
                   "");

  if (!file.isEmpty())
    output_line->setText(QDir::toNativeSeparators(file));
}


void
Main_GUI::check_run()
{
  if (input_line->text().isEmpty() || output_line->text().isEmpty())
    run_button->setEnabled(false);
  else
    run_button->setEnabled(true);

  if (!input_line->text().isEmpty())
  {
    if (input_line->text().toUpper().endsWith(".ZIP"))
    {
      command_box->setCurrentIndex(0);
    }
    if (input_line->text().toUpper().endsWith(".BIN"))
    {
      command_box->setCurrentIndex(1);
    }
  }
}


void
Main_GUI::absolute_input()
{
  QString input_name = QDir::fromNativeSeparators(input_line->text());
  if (!input_name.isEmpty()
      && QDir::isRelativePath(input_name))
  {
    QDir cur_path(QDir::currentPath() + "/" + input_name);
    input_line->setText(QDir::toNativeSeparators(cur_path.absolutePath()));
  }
}


void
Main_GUI::absolute_output()
{
  QString output_name = QDir::fromNativeSeparators(output_line->text());
  if (!output_name.isEmpty()
      && QDir::isRelativePath(output_name))
  {
    QDir cur_path(QDir::currentPath() + "/" + output_name);
    output_line->setText(QDir::toNativeSeparators(cur_path.absolutePath()));
  }
}


int
Main_GUI::check_filenames(const QString& input_name,
                          const QString& output_name)
{
  if (!QFile::exists(input_name))
  {
    QMessageBox::warning(
      this,
      "MzCrypt-Gui",
      tr("The file %1 cannot be found.")
         .arg(QUOTE_STRING(QDir::toNativeSeparators(input_name))),
      QMessageBox::Ok,
      QMessageBox::Ok);
    return 0;
  }

  if (input_name == output_name)
  {
    QMessageBox::warning(
      this,
      "MzCrypt-Gui",
      tr("Input and output file names must be different."),
      QMessageBox::Ok,
      QMessageBox::Ok);
    return 0;
  }

  if (QFile::exists(output_name))
  {
    int ret = QMessageBox::warning(
                this,
                "MzCrypt-Gui",
                tr("The file %1 already exists.\n"
                   "Overwrite?")
                   .arg(QUOTE_STRING(QDir::toNativeSeparators(output_name))),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No);
    if (ret == QMessageBox::No)
      return 0;
  }

  return 1;
}




void
Main_GUI::run()
{
  statusBar()->clearMessage();

  QString input_name = QDir::fromNativeSeparators(input_line->text());
  QString output_name = QDir::fromNativeSeparators(output_line->text());
  if (!check_filenames(input_name, output_name))
    return;

  QFileInfo fileinfo(input_name);

  progress = new QProgressDialog(QString("Processing %1").arg(QDir::toNativeSeparators(input_name)),
                                 QString(),
                                 0, fileinfo.size());

  QThreadPool *threadPool = QThreadPool::globalInstance();
  CryptoWorker *work = new CryptoWorker(QDir::toNativeSeparators(input_name),
                                        QDir::toNativeSeparators(output_name),
                                        command_box->currentIndex());
  work->setAutoDelete(true);
  threadPool->start(work);

  QObject::connect(work, SIGNAL(signalWorkFinished(int,const QString&)),
                   this, SLOT(slotWorkFinished(int,const QString&)));
  QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
  timer.start(500);

  // blocks
  progress->exec();
}


void
Main_GUI::slotWorkFinished(int rc,const QString& message)
{
  progress->reset();
  progress->deleteLater();
  if (rc != 0) {
    QMessageBox::warning(this,
      "MzCrypt-Gui",
      tr("Failed processing! Error code is %1 (%2).\n")
                         .arg(rc)
                         .arg(message),
      QMessageBox::Ok,
      QMessageBox::Ok);
    statusBar()->showMessage(QString("Error has occured."));
  }
  else
  {
    statusBar()->showMessage(QString("Successfully converted."));
  }
}


void
Main_GUI::slotTimer()
{
    if (progress != 0 && QThreadPool::globalInstance()->activeThreadCount() > 0)
    {
        QFileInfo fileinfo(output_line->text());
        progress->setValue(fileinfo.size());
    }
}


void
Main_GUI::create_layout()
{
  QString desc;
  //
  // file stuff
  //
  QCompleter* completer = new QCompleter(this);
  QFileSystemModel* model = new QFileSystemModel(completer);
  model->setRootPath(QDir::rootPath());
  completer->setModel(model);

  QLabel* input_label = new QLabel(tr("&Input File:"));
  // enforce rich text to get nice word wrapping
  desc = tr("<b></b>The input file, either a .BIN update you want to decrypt,"
            " or a .ZIP update you want to encrypt.");
  input_line = new Line_Edit;
  input_button = new QPushButton(tr("Browse..."));
  input_label->setBuddy(input_line);
  input_line->setCompleter(completer);
  input_label->setToolTip(desc);
  input_line->setToolTip(desc);
  input_button->setToolTip(desc);

  QLabel* output_label = new QLabel(tr("&Output File:"));
  desc = tr("<b></b>The output file, either encrypting or decrypting from input file.");
  output_line = new Line_Edit;
  output_button = new QPushButton(tr("Browse..."));
  output_label->setBuddy(output_line);
  output_line->setCompleter(completer);
  output_label->setToolTip(desc);
  output_line->setToolTip(desc);
  output_button->setToolTip(desc);


  // layout
  QGridLayout* file_layout = new QGridLayout;

  file_layout->addWidget(input_label, 0, 0, Qt::AlignRight);
  file_layout->addWidget(input_line, 0, 1);
  file_layout->addWidget(input_button, 0, 2);

  file_layout->setRowStretch(1, 1);

  file_layout->addWidget(output_label, 2, 0, Qt::AlignRight);
  file_layout->addWidget(output_line, 2, 1);
  file_layout->addWidget(output_button, 2, 2);

  //
  // command control
  //
  QLabel* command_label = new QLabel(tr("&Command:"));
  command_box = new QComboBox;
  command_label->setBuddy(command_box);
  command_label->setToolTip(
    tr("<b></b>This sets which way the conversion takes place."
       "Normally automatically detected."));
  command_box->insertItem(0, tr("Encrypt (zip to bin)"));
  command_box->insertItem(1, tr("Decrypt (bin to zip)"));
  command_box->setCurrentIndex(0);



  //
  // running
  //
  run_button = new QPushButton("    "
                               + tr("&Run")
                               + "    "); // make label wider
  run_button->setEnabled(false);

  //
  // the whole gui
  //
  QGridLayout* gui_layout = new QGridLayout;
  QFrame* hline = new QFrame;
  hline->setFrameShape(QFrame::HLine);
  int row = 0; // this counter simplifies inserting new items

  gui_layout->setRowMinimumHeight(row, 10); // XXX urgh, pixels...
  gui_layout->setRowStretch(row++, 1);

  gui_layout->addLayout(file_layout, row, 0, row, -1);
  gui_layout->setRowStretch(row++, 1);

  gui_layout->setRowMinimumHeight(row, 10); // XXX urgh, pixels...
  gui_layout->setRowStretch(row++, 1);

  gui_layout->addWidget(command_label, row, 0, Qt::AlignRight);
  gui_layout->addWidget(command_box, row++, 1, Qt::AlignLeft);

  gui_layout->addWidget(hline, row, 0, row, -1);
  gui_layout->setRowStretch(row++, 1);

  gui_layout->setRowMinimumHeight(row, 30); // XXX urgh, pixels...
  gui_layout->setRowStretch(row++, 1);

  gui_layout->addWidget(run_button, row++, 1, Qt::AlignRight);

  // create dummy widget to register layout
  QWidget* main_widget = new QWidget;
  main_widget->setLayout(gui_layout);
  setCentralWidget(main_widget);
  setWindowTitle("MzCrypt-Gui 1.0.3 MeizuMX3 Edition");
}


void
Main_GUI::create_connections()
{
  connect(input_button, SIGNAL(clicked()), this,
          SLOT(browse_input()));
  connect(output_button, SIGNAL(clicked()), this,
          SLOT(browse_output()));

  connect(input_line, SIGNAL(textChanged(QString)), this,
          SLOT(check_run()));
  connect(output_line, SIGNAL(textChanged(QString)), this,
          SLOT(check_run()));

  connect(input_line, SIGNAL(editingFinished()), this,
          SLOT(absolute_input()));
  connect(output_line, SIGNAL(editingFinished()), this,
          SLOT(absolute_output()));

  connect(run_button, SIGNAL(clicked()), this,
          SLOT(run()));
}


void
Main_GUI::create_actions()
{
  exit_act = new QAction(tr("E&xit"), this);
  exit_act->setShortcuts(QKeySequence::Quit);
  connect(exit_act, SIGNAL(triggered()), this, SLOT(close()));

  about_act = new QAction(tr("&About"), this);
  connect(about_act, SIGNAL(triggered()), this, SLOT(about()));

  about_Qt_act = new QAction(tr("About &Qt"), this);
  connect(about_Qt_act, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}


void
Main_GUI::create_menus()
{
  file_menu = menuBar()->addMenu(tr("&File"));
  file_menu->addAction(exit_act);

  help_menu = menuBar()->addMenu(tr("&Help"));
  help_menu->addAction(about_act);
  help_menu->addAction(about_Qt_act);
}


void
Main_GUI::create_status_bar()
{
  statusBar()->showMessage("");
}


void
Main_GUI::read_settings()
{
  QSettings settings;
}


void
Main_GUI::write_settings()
{
  QSettings settings;
}

// end of maingui.cpp

// lineedit.cpp

// Copyright (C) 2012 by Werner Lemberg.
//
// This file is part of the ttfautohint library, and may only be used,
// modified, and distributed under the terms given in `COPYING'.  By
// continuing to use, modify, or distribute this file you indicate that you
// have read `COPYING' and understand and accept it fully.
//
// The file `COPYING' mentioned in the previous paragraph is distributed
// with the ttfautohint library.


// Derived class `Line_Edit' is QLineEdit which accepts drag and drop.

#include "lineedit.h"

Line_Edit::Line_Edit(QWidget* parent)
: QLineEdit(parent)
{
  // empty
}

 
void
Line_Edit::dragEnterEvent(QDragEnterEvent* event)
{
  QList<QUrl> url_list;

  if (event->mimeData()->hasUrls())
  {
    url_list = event->mimeData()->urls();

    // if just text was dropped, url_list is empty
    if (url_list.size())
    {
      event->acceptProposedAction();
    }
  }
}


void
Line_Edit::dropEvent(QDropEvent* event)
{
  QList<QUrl> url_list;
  QString file_name;
  QFileInfo info;

  if (event->mimeData()->hasUrls())
  {
    url_list = event->mimeData()->urls();

    // if just text was dropped, url_list is empty
    if (url_list.size())
    {
      file_name = url_list[0].toLocalFile();

      // check whether `file_name' is valid
      info.setFile(file_name);
      if (info.isFile())
        setText(QDir::toNativeSeparators(file_name));
    }
  }

  event->acceptProposedAction();
}

// end of lineedit.cpp

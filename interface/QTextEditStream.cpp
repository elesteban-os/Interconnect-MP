#include "QTextEditStream.h"

QTextEditStream::QTextEditStream(QTextEdit* textEdit)
    : edit(textEdit) {}

int QTextEditStream::overflow(int c) {
    if (!edit) return c;

    if (c == '\n') {
        QString line = buffer;
        buffer.clear();

        QMetaObject::invokeMethod(edit, [this, line]() {
            edit->append(line);
        }, Qt::QueuedConnection);
    } else {
        buffer += QChar(c);
    }

    return c;
}


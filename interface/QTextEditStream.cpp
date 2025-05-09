#include "QTextEditStream.h"

QTextEditStream::QTextEditStream(QTextEdit* textEdit)
    : edit(textEdit) {}

int QTextEditStream::overflow(int c) {
    if (c == '\n') {
        edit->append(buffer);
        buffer.clear();
    } else {
        buffer += QChar(c);
    }
    return c;
}

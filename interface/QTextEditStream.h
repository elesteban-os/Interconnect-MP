#ifndef QTEXTEDITSTREAM_H
#define QTEXTEDITSTREAM_H

#include <streambuf>
#include <QString>
#include <QTextEdit>

class QTextEditStream : public std::streambuf {
public:
    explicit QTextEditStream(QTextEdit* textEdit);

protected:
    int overflow(int c) override;

private:
    QTextEdit* edit;
    QString buffer;
};

#endif // QTEXTEDITSTREAM_H


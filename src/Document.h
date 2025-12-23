#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <QSizeF>
#include <QString>
#include <memory>

namespace Poppler {
class Document;
};

class Document {
public:
  Document();
  ~Document();

  // load pdf
  // returns true on success, false ow
  bool load(const QString &filePath);

  // check if loaded
  bool isLoaded() const;

  // get page count
  int pageCount() const;

  QSizeF pageSize(int pageNumber) const;

  QString title() const;

  QString errorString() const;

  Poppler::Document *popplerDocument() const;

  static double pointsToMM(double points);

  QSizeF pageSizeMM(int pageNumber) const;

  static QString detectPaperSize(const QSizeF &sizeMM);

private:
  std::unique_ptr<Poppler::Document> m_document;
  QString m_errorString;
  QString m_filePath;
};

#endif // DOCUMENT_H_

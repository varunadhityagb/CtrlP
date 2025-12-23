#include "Document.h"
#include <QFileInfo>
#include <QtMath>
#include <poppler/qt6/poppler-qt6.h>

Document::Document() : m_document(nullptr), m_errorString("") {}

Document::~Document() {}

bool Document::load(const QString &filePath) {

  QFileInfo fileInfo(filePath);
  if (!fileInfo.exists() || !fileInfo.isFile()) {
    m_errorString = "File does not exist";
    m_document.reset();
    return false;
  }

  auto doc = Poppler::Document::load(filePath);

  if (!doc) {
    m_errorString = "Failed to load PDF";
    m_document.reset();
    return false;
  }

  m_document = std::move(doc);
  m_filePath = filePath;
  m_errorString.clear();

  return true;
}

bool Document::isLoaded() const { return m_document != nullptr; }

int Document::pageCount() const {
  if (!isLoaded())
    return 0;
  return m_document->numPages();
}

QSizeF Document::pageSize(int pageNumber) const {
  if (!isLoaded())
    return QSizeF(0, 0);
  if (pageNumber < 0 || pageNumber >= pageCount())
    return QSizeF(0, 0);

  auto page = m_document->page(pageNumber);
  if (!page)
    return QSizeF(0, 0);

  return page->pageSizeF();
}

QString Document::title() const {
  if (!isLoaded())
    return QString();
  return m_document->info("Title");
}

QString Document::errorString() const { return m_errorString; }

Poppler::Document *Document::popplerDocument() const {
  return m_document.get();
}

double Document::pointsToMM(double points) { return points * 25.4 / 72; }

QSizeF Document::pageSizeMM(int pageNumber) const {
  QSizeF sizePoints = pageSize(pageNumber);
  return QSizeF(pointsToMM(sizePoints.width()),
                pointsToMM(sizePoints.height()));
}

QString Document::detectPaperSize(const QSizeF &sizeMM) {
  double w = sizeMM.width();
  double h = sizeMM.height();

  auto matches = [](double a, double b, double tolerance = 2.0) {
    return qAbs(a - b) < tolerance;
  };

  if (matches(w, 210) && matches(h, 297))
    return "A4";
  if (matches(w, 216) && matches(h, 279))
    return "Letter";
  if (matches(w, 216) && matches(h, 356))
    return "Legal";
  if (matches(w, 297) && matches(h, 420))
    return "A3";

  if (matches(w, 297) && matches(h, 210))
    return "A4 (Landscape)";
  if (matches(w, 279) && matches(h, 216))
    return "Letter (Landscape)";
  if (matches(w, 356) && matches(h, 216))
    return "Legal (Landscape)";
  if (matches(w, 420) && matches(h, 297))
    return "A3 (Landscape)";

  return "Custom";
}

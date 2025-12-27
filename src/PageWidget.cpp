#include "PageWidget.h"
#include "PrintSettings.h"
#include <QFont>
#include <QPainter>
#include <QPen>
#include <qnamespace.h>
#include <qpixmap.h>

PageWidget::PageWidget(QWidget *parent)
    : QWidget(parent), m_printSettings(nullptr), m_dpi(150.0), m_pageNumber(0) {
  setStyleSheet("background-color: black;");
}

void PageWidget::setPagePixmap(const QPixmap &pixmap) {
  m_pagePixmap = pixmap;
  setFixedSize(pixmap.size());
  update();
}

void PageWidget::setPrintSettings(const PrintSettings *settings) {
  m_printSettings = settings;
  update();
}

void PageWidget::setDPI(double dpi) {
  m_dpi = dpi;
  update();
}

void PageWidget::setPageNumber(int pageNum) {
  m_pageNumber = pageNum;
  update();
}

QSize PageWidget::sizeHint() const { return m_pagePixmap.size(); }

void PageWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);

  if (!m_pagePixmap.isNull())
    painter.drawPixmap(0, 0, m_pagePixmap);

  if (m_printSettings) {
    drawMargins(&painter);
    drawDuplexIndicator(&painter);
  }
}

void PageWidget::drawMargins(QPainter *painter) {
  if (m_pagePixmap.isNull())
    return;

  double topPx = mmToPixels(m_printSettings->margins.top);
  double bottomPx = mmToPixels(m_printSettings->margins.bottom);
  double leftPx = mmToPixels(m_printSettings->margins.left);
  double rightPx = mmToPixels(m_printSettings->margins.right);

  int pageWidth = m_pagePixmap.width();
  int pageHeight = m_pagePixmap.height();

  QPen pen(QColor(255, 0, 0, 100));
  pen.setStyle(Qt::DashLine);
  pen.setWidth(2);
  painter->setPen(pen);

  QRectF marginRect(leftPx, topPx, pageWidth - leftPx - rightPx,
                    pageHeight - topPx - bottomPx);

  painter->drawRect(marginRect);

  pen.setStyle(Qt::SolidLine);
  pen.setWidth(1);
  painter->setPen(pen);

  int markerSize = 10;

  painter->drawLine(0, topPx, markerSize, topPx);
  painter->drawLine(leftPx, 0, leftPx, markerSize);

  painter->drawLine(pageWidth - markerSize, topPx, pageWidth, topPx);
  painter->drawLine(pageWidth - rightPx, 0, pageWidth - rightPx, markerSize);

  painter->drawLine(0, pageHeight - bottomPx, markerSize,
                    pageHeight - bottomPx);
  painter->drawLine(leftPx, pageHeight - markerSize, leftPx, pageHeight);

  painter->drawLine(pageWidth - markerSize, pageHeight - bottomPx, pageWidth,
                    pageHeight - bottomPx);
  painter->drawLine(pageWidth - rightPx, pageHeight - markerSize,
                    pageWidth - rightPx, pageHeight);
}

void PageWidget::drawDuplexIndicator(QPainter *painter) {
  if (m_printSettings->duplexMode == PrintSettings::Simplex)
    return;

  if (m_pagePixmap.isNull())
    return;

  int x = 10;
  int y = m_pagePixmap.height() - 30;

  painter->fillRect(x - 5, y - 5, 100, 25, QColor(0, 0, 0, 100));

  painter->setPen(Qt::white);
  QFont font = painter->font();
  font.setPointSize(10);
  painter->setFont(font);

  QString text;
  if (m_printSettings->duplexMode == PrintSettings::DuplexLongEdge) {
    if (m_pageNumber % 2 == 0) {
      text = "↓ Flip ↓";
    } else {
      text = "↑ Flip ↑";
    }
  } else {
    if (m_pageNumber % 2 == 0) {
      text = "→ Flip →";
    } else {
      text = "← Flip ←";
    }
  }

  painter->drawText(x, y, text);
}

double PageWidget::mmToPixels(double mm) const { return (mm / 25.4) * m_dpi; }

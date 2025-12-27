#ifndef PAGEWIDGET_H_
#define PAGEWIDGET_H_

#include "PrintSettings.h"
#include <QPixmap>
#include <QWidget>

class PageWidget : public QWidget {
  Q_OBJECT

public:
  PageWidget(QWidget *parent = nullptr);

  void setPagePixmap(const QPixmap &pixmap);
  void setPrintSettings(const PrintSettings *settings);
  void setDPI(double dpi);
  void setPageNumber(int pageNum);

  QSize sizeHint() const override;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  void drawMargins(QPainter *painter);
  void drawDuplexIndicator(QPainter *painter);

  double mmToPixels(double mm) const;

  QPixmap m_pagePixmap;
  const PrintSettings *m_printSettings;
  double m_dpi;
  int m_pageNumber;
};

#endif // PAGEWIDGET_H_

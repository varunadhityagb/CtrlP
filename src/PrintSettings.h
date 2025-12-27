#ifndef PRINTSETTINGS_H_
#define PRINTSETTINGS_H_

#include <QString>

struct PrintSettings {
  enum ScaleMode { FitToPage, ActualSize, CustomPercent };

  enum DuplexMode { Simplex, DuplexLongEdge, DuplexShortEdge };

  struct Margins {
    double top;
    double bottom;
    double right;
    double left;

    Margins(double t = 10.0, double b = 10.0, double l = 10.0, double r = 10.0)
        : top(t), bottom(b), left(l), right(r) {}
  };

  ScaleMode scaleMode;
  int customPercent;
  Margins margins;
  DuplexMode duplexMode;
  bool colorMode;

  bool printAllPages;
  int fromPage;
  int toPage;

  PrintSettings()
      : scaleMode(FitToPage), customPercent(100),
        margins(10.0, 10.0, 10.0, 10.0), duplexMode(Simplex), colorMode(true),
        printAllPages(true), fromPage(1), toPage(1) {}

  static Margins marginPresetNone() { return Margins(0, 0, 0, 0); }
  static Margins marginPresetMinimal() { return Margins(5, 5, 5, 5); }
  static Margins marginPresetNormal() { return Margins(10, 10, 10, 10); }
  static Margins marginPresetComfortable() { return Margins(15, 15, 15, 15); }
  static Margins marginPresetWide() { return Margins(20, 20, 25, 25); }

  QString marginPresetName() const {
    if (margins.top == 0 && margins.left == 0)
      return "None";
    if (margins.top == 5 && margins.left == 5)
      return "Minimal";
    if (margins.top == 10 && margins.left == 10)
      return "Normal";
    if (margins.top == 15 && margins.left == 15)
      return "Comfortable";
    if (margins.top == 20 && margins.left == 25)
      return "Wide";
    return "Custom";
  }

  QString duplexModeName() const {
    switch (duplexMode) {
    case Simplex:
      return "One-sided";
    case DuplexLongEdge:
      return "Long Edge";
    case DuplexShortEdge:
      return "Short Edge";
    default:
      return "Unknown";
    }
  }

  QString scaleModeName() const {
    switch (scaleMode) {
    case FitToPage:
      return "Fit to Page";
    case ActualSize:
      return "Actual Size";
    case CustomPercent:
      return QString("%1%").arg(customPercent);
    default:
      return "Unknown";
    }
  }
};

#endif // PRINTSETTINGS_H_

#ifndef OVERLAYITEM_H
#define OVERLAYITEM_H
#include <QString>

class OverlayItem
{
private:
    int m_x, m_y;
    QString m_text;
public:
    OverlayItem(int, int, QString);
    int GetX() const;
    int GetY() const;
    QString getText() const;
};

#endif // OVERLAYITEM_H

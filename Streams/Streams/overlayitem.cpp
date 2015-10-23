#include "overlayitem.h"

OverlayItem::OverlayItem(int x, int y, QString text)
{
    m_x = x;
    m_y = y;
    m_text = text;
}

int OverlayItem::GetX() const
{
    return m_x;
}

int OverlayItem::GetY() const
{
    return m_y;
}
QString OverlayItem::getText() const
{
    return m_text;
}

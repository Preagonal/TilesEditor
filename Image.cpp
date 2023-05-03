#include <QPainter>
#include "Image.h"

namespace TilesEditor
{
    Image::Image(const QString& assetName, QPixmap pixmap):
        Resource(assetName)
    {
        m_pixmap = pixmap;
    }

    void Image::replace(const QString& fileName)
    {
        QPixmap pixmap;

        if (pixmap.load(fileName))
        {
            m_pixmap = pixmap;
        }
    }

    void Image::draw(QPainter* painter, double x, double y)
    {
        painter->drawPixmap((int)x, (int)y, this->pixmap());
    }

    Image* Image::load(const QString& assetName, const QString& fileName)
    {
        QPixmap pixmap;

        if (pixmap.load(fileName))
        {
            return new Image(assetName, pixmap);
        }
        return nullptr;
    }
};


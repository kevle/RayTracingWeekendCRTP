#pragma once

#include "HostImageRGB.hpp"
#include "LoadImage.hpp"
#include "Vector.hpp"

#include <QDebug>
#include <QQuickImageProvider>
#include <QUrlQuery>
#include <iostream>

template <typename Renderer>
class RenderProvider : public QQuickImageProvider
{
    const Renderer& renderer;

public:
    RenderProvider(const Renderer& renderer)
        : QQuickImageProvider(QQuickImageProvider::Pixmap)
        , renderer(renderer)
    {
    }

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override
    {
        if (requestedSize.width() <= 0 || requestedSize.height() <= 0)
        {
            return QImage(QSize(1, 1), QImage::Format::Format_Mono);
        }
        QSize renderSize = requestedSize;

        const auto& width = renderSize.width();
        const auto& height = renderSize.height();

        std::cout << "Rendering " << renderSize.width() << "x" << renderSize.height() << std::endl;

        if (size)
            *size = renderSize;
        QImage image(renderSize, QImage::Format::Format_RGB32);

        QUrlQuery url(id);
        int rows = url.queryItemValue("rows").toInt();
        int cols = url.queryItemValue("cols").toInt();
        int row = url.queryItemValue("row").toInt();
        int col = url.queryItemValue("col").toInt();
        int tileWidth = url.queryItemValue("width").toInt();
        int tileHeight = url.queryItemValue("height").toInt();

        int totalWidth = cols * tileWidth;
        int totalHeight = rows * tileHeight;
        int widthOffset = col * tileWidth;
        int heightOffset = row * tileHeight;

        qDebug() << "Id: " << id;
        qDebug() << "Requested size: " << requestedSize;
        image.fill(QColor(id).rgba());

        renderer([&image, height](int i, int j, const Vector3f& c)
            { image.setPixelColor(i, height - 1 - j, qRgb(c.x() * 255, c.y() * 255, c.z() * 255)); },
            totalWidth, totalHeight, width, height, widthOffset, heightOffset);
        //
        // image.save(QString("render_%1_%2.png").arg(renderSize.width()).arg(renderSize.height()));

        return image;
    }

    QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override
    {
        return QPixmap::fromImage(requestImage(id, size, requestedSize));
    }
};

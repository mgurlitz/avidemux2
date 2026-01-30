#ifndef ADM_CLICKABLE_LABEL_H
#define ADM_CLICKABLE_LABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>

/**
 * \class ADM_clickableLabel
 * \brief A QLabel that emits a signal when clicked/dragged with the relative position
 *        and displays a position indicator line
 */
class ADM_clickableLabel : public QLabel
{
    Q_OBJECT

public:
    ADM_clickableLabel(QWidget *parent = nullptr) : QLabel(parent), m_positionRatio(-1.0), m_dragging(false)
    {
        setCursor(Qt::PointingHandCursor);
        setMouseTracking(false);
    }
    ~ADM_clickableLabel() {}

    // Set the current position indicator (0.0 to 1.0, or negative to hide)
    void setPositionRatio(double ratio)
    {
        m_positionRatio = ratio;
        update(); // Trigger repaint
    }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            m_dragging = true;
            emitPosition(event);
        }
        QLabel::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (m_dragging && (event->buttons() & Qt::LeftButton))
        {
            emitPosition(event);
        }
        QLabel::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            m_dragging = false;
        }
        QLabel::mouseReleaseEvent(event);
    }

    void paintEvent(QPaintEvent *event) override
    {
        // First draw the label content (the waveform pixmap)
        QLabel::paintEvent(event);

        // Then draw the position indicator line on top
        if (m_positionRatio >= 0.0 && m_positionRatio <= 1.0)
        {
            int xPos = (int)(m_positionRatio * width());
            QPainter painter(this);
            painter.setPen(QPen(QColor(255, 255, 0), 2)); // Bright yellow, 2px wide
            painter.drawLine(xPos, 0, xPos, height());
        }
    }

private:
    double m_positionRatio;
    bool m_dragging;

    void emitPosition(QMouseEvent *event)
    {
        int x = 0;
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
        x = event->x();
#else
        x = event->position().x();
#endif
        // Clamp to widget bounds
        if (x < 0) x = 0;
        if (x > width()) x = width();
        emit clicked(x, width());
    }

signals:
    void clicked(int position, int totalWidth);
};

#endif // ADM_CLICKABLE_LABEL_H

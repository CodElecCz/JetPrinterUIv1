#ifndef LIGHTWIDGET_H
#define LIGHTWIDGET_H

#include <QPainter>
#include <QWidget>
#include <QObject>
#include <QTimer>

#define LIGHTWIDGET_OFFSET  3
#define LIGHTWIDGET_SIZE_V  15
#define LIGHTWIDGET_SIZE_H  160
#define LIGHTWIDGET_BLINK   250 //ms

class LightWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
    Q_PROPERTY(QString text READ getText WRITE setText)
    Q_PROPERTY(QColor color READ getColor WRITE setColor)

public:
    LightWidget(const QColor &color, QString text = "", QWidget *parent = 0)
        : QWidget(parent), m_color(color), m_on(true), m_text(text)
    {
        this->setFixedSize(LIGHTWIDGET_SIZE_H, LIGHTWIDGET_SIZE_V);

        connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    }
    ~LightWidget() { timer.stop(); }

    bool isOn() const { return m_on; }
    void setOn(bool on)
    {
        if (on == m_on)
            return;

        m_on = on;
        update();
    }

    QColor getColor() const { return m_color; }
    void setColor(QColor color)
    {
        if (color == m_color)
            return;

        m_color = color;
        update();
    }

    QString getText() const { return m_text; }
    void setText(QString text)
    {
        if (0==text.compare(m_text))
            return;

        m_text = text;
        update();
    }

    void blink(int time = 1500)
    {
        m_time = time;
        timer.setInterval(LIGHTWIDGET_BLINK);
        timer.start();
    }

public slots:
    void turnOff() { setOn(false); }
    void turnOn() { setOn(true); }

private slots:
    void updateTime()
    {
        m_time -= LIGHTWIDGET_BLINK;
        if(m_time<=0)
        {
            timer.stop();
            setOn(true);
        }
        else
        {
            setOn(!isOn());
        }
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        paint(m_on? m_color:Qt::white);
    }

    void paint(QColor color)
    {
        QPainter painter(this);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(color);
        painter.drawEllipse(LIGHTWIDGET_OFFSET, LIGHTWIDGET_OFFSET, LIGHTWIDGET_SIZE_V-2*LIGHTWIDGET_OFFSET, LIGHTWIDGET_SIZE_V-2*LIGHTWIDGET_OFFSET);
        painter.drawText(LIGHTWIDGET_SIZE_V, LIGHTWIDGET_SIZE_V-LIGHTWIDGET_OFFSET, m_text);
    }

private:
    QColor m_color;
    bool m_on;
    int m_time;
    QTimer timer;
    QString m_text;
};

#endif // LIGHTWIDGET_H

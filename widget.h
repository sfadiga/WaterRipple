#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QTime>

static const int RIPPLE_RAD = 3;
static const int DELAY = 30; // delay is desired animation FPS

namespace Ui {
class Widget;
}

//!
//! \brief The Widget class - this widget implements the Water Ripple effect, the idea
//! and some of the code were copied from: http://agilerepose.weebly.com/water-ripple.html
//! I kept some of the original coments on the code...
//!
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    // QWidget interface
protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_pushButton_clicked();
    void update();

private:

    // -------------------------------------------------------
    // Drop something in the water at location: dx, dy
    // -------------------------------------------------------
    void dropAt(int dx, int dy);

    // -------------------------------------------------------
    // Create the next frame of the ripple effect
    // -------------------------------------------------------
    void processFrame();

private:
    Ui::Widget *ui;

    //!
    //! \brief _timer - the event timer to control animation loop
    //!
    QTimer* _timer;

    //!
    //! \brief _clock - used to control the random drop event
    //!
    QTime* _clock;

    int _width;
    int _height;
    int _half_width;
    int _half_height;
    int _size;       // space for 2 images (old and new), +2 to cover ripple radius <= 3

    int _last_index;
    int _current_index; // +2 from above size calc +1 more to get to 2nd image
    int _map_index;

    int *_ripple_map;
    int *_last_map;

    QImage* _texture;
    QImage* _ripple;

};

#endif // WIDGET_H

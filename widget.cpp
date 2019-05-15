#include "widget.h"
#include "ui_widget.h"

#include <QFileDialog>
#include <QImageReader>
#include <QRandomGenerator>
#include <QMouseEvent>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    _width(0),
    _height(0),
    _half_width(0),
    _half_height(0),
    _size(0),
    _last_index(0),
    _current_index(0),
    _map_index(0),
    _ripple_map(nullptr),
    _last_map(nullptr),
    _texture(nullptr),
    _ripple(nullptr)
{
    ui->setupUi(this);

    this->setWindowTitle("Water Ripple");
    ui->label->setText("");

    _timer = new QTimer();
    _timer->setInterval(DELAY);

    _clock = new QTime();

    connect(_timer, &QTimer::timeout, this, &Widget::update);
}

Widget::~Widget()
{
    delete ui;
    _timer->stop();
    delete[] _ripple_map;
    delete[] _last_map;
    delete _texture;
    delete _ripple;
    delete _timer;
    delete _clock;
}

void Widget::update()
{
    // randomly create drops on the screen
    if(_clock->elapsed() % 100 == 0)
        dropAt(QRandomGenerator::global()->bounded(_width), QRandomGenerator::global()->bounded(_height));

    // draw the pixel map on the frame buffer (paint the image on the label at the gui)
    ui->label->setPixmap(QPixmap::fromImage(*_ripple));

    // calculates the next pixel map
    processFrame();
}

void Widget::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"),
                                                    "/",
                                                    tr("Image Files (*.png *.jpg *.bmp)"),
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);
    if(fileName.isNull() || fileName.isEmpty())
        return;

    // reset ui events
    ui->label->removeEventFilter(this);
    ui->label->setMouseTracking(false);
    _timer->stop();

    QImageReader ir;
    ir.setFileName(fileName);

    delete _texture;
    _texture = new QImage(ir.read());

    _width = _texture->width();
    _height = _texture->height();
    _half_width = _width >> 1;
    _half_height = _height >> 1;
    // space for 2 images (old and new), +2 to cover ripple radius <= 3
    _size = _width * (_height + 2) * 2;
    _last_index = _width;
    // +2 from above size calc +1 more to get to 2nd image
    _current_index = _width * (_height + 3);

    delete _ripple_map;
    _ripple_map = new int[_size] ();

    delete _last_map;
    _last_map = new int[_size] ();

    for (int i = 0; i < _size; i++)
    {
        _ripple_map[i]   = 0;
        _last_map[i] = 0;
    }

    delete _ripple;
    _ripple = new QImage(*_texture);

    ui->label->installEventFilter(this);
    ui->label->setMouseTracking(true);

    _timer->start();
    _clock->restart();
}

void Widget::dropAt(int dx, int dy)
{
    // Make certain dx and dy are integers
    // Shifting left 0 is slightly faster than parseInt and math.* (or used to be)
    dx <<= 0;
    dy <<= 0;

    // Our ripple effect area is actually a square, not a circle
    for (int j = dy - RIPPLE_RAD ; j < dy + RIPPLE_RAD ; j++)
    {
        for (int k = dx - RIPPLE_RAD ; k < dx + RIPPLE_RAD ; k++)
        {
            int i = _last_index + (j * _width) + k;
            if(i < _size && i >= 0)
                _ripple_map[i] += 512;
        }
    }
}

void Widget::processFrame()
{
    int i = 0;
    int a = 0;
    int b = 0;
    int data = 0;
    int old_data = 0;

    // Store indexes - old and new may be misleading/confusing
    //               - current and next is slightly more accurate
    //               - previous and current may also help in thinking
    i = _last_index;
    _last_index = _current_index;
    _current_index = i;

    // Initialize the looping values - each will be incremented
    i = 0;
    _map_index = _last_index;

    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            // Use rippleMap to set data value, mapIdx = oldIdx
            // Use averaged values of pixels: above, below, left and right of current
            data = (
                    _ripple_map[_map_index - _width] +
                    _ripple_map[_map_index + _width] +
                    _ripple_map[_map_index - 1] +
                    _ripple_map[_map_index + 1]
                    ) >> 1;    // right shift 1 is same as divide by 2

            // Subtract 'previous' value (we are about to overwrite rippleMap[newIdx+i])
            data -= _ripple_map[_current_index + i];

            // Reduce value more -- for damping
            // data = data - (data / 32)
            data -= data >> 5;

            // Set new value
            _ripple_map[_current_index + i] = data;

            // If data = 0 then water is flat/still,
            // If data > 0 then water has a wave
            data = 1024 - data;

            old_data = _last_map[i];
            _last_map[i] = data;

            if (old_data != data)  // if no change no need to alter image
            {
                // Recall using "<< 0" forces integer value
                // Calculate pixel offsets
                a = (((x - _half_width) * data / 1024)  << 0) + _half_width;
                b = (((y - _half_height) * data / 1024)  << 0) + _half_height;

                // Don't go outside the image (i.e. boundary check)
                if (a >= _width) a = _width - 1;
                if (a < 0) a = 0;
                if (b >= _height) b = _height - 1;
                if (b < 0) b = 0;

                // maps the original texture in the paint buffer
                // with the pixel changes calculated above
                _ripple->setPixel(x, y, _texture->pixel(a, b));
            }
            _map_index++;
            i++;
        }
    }
}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *m_event = static_cast<QMouseEvent *>(event);
        int x = m_event->x();
        int y = m_event->y();
        dropAt(x, y);
        return true;
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

